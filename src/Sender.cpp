#include "Sender.hpp"

Sender::Sender(MicroBit *mbit) {
  this->mbit = mbit;
  cipher = new XXTEACipher(CIPHER_KEY, CIPHER_KEY_LENGTH);
  setupListeners();
};

Sender::~Sender() {
  tearDownListeners();
  delete cipher;
};

void Sender::setupListeners() {
  DEBUG(mbit, "Setting up listeners");
  mbit->messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_DOWN, this,
                          &Sender::onButtonADown);
  mbit->messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_UP, this,
                          &Sender::onButtonAUp);
  mbit->messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, this,
                          &Sender::onButtonBPress);
};

void Sender::tearDownListeners() {
  mbit->messageBus.ignore(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_DOWN, this,
                          &Sender::onButtonADown);
  mbit->messageBus.ignore(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_UP, this,
                          &Sender::onButtonAUp);
  mbit->messageBus.ignore(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, this,
                          &Sender::onButtonBPress);
};

void Sender::onButtonADown(MicroBitEvent event) {
  // Dont allow the buffer to be modified while we are sending data.
  if (sending) {
    return;
  }
  // If button A was somehow already down, ignore the current event.
  if (buttonADownTimestamp > 0) {
    return;
  }
  DEBUG(mbit, "Button A is down!");
  buttonADownTimestamp = event.timestamp / 1000;
};

void Sender::onButtonAUp(MicroBitEvent event) {
  if (sending) {
    return;
  }
  // If button A was somehow already up, ignore this event.
  if (buttonADownTimestamp == 0) {
    return;
  }
  DEBUG(mbit, "Button A is up!");

  uint64_t buttonAUpTimestamp = event.timestamp / 1000;
  uint64_t duration = buttonAUpTimestamp - buttonADownTimestamp;

  if (duration >= DIT_MIN && duration < DIT_MAX) {
    DEBUG(mbit, "Dot added!");

    buffer.push_back(DOT);
  } else if (duration >= DAH_MIN && duration < DAH_MAX) {
    DEBUG(mbit, "Dash added!");
    buffer.push_back(DASH);
  }

  // Reset the value.
  buttonADownTimestamp = 0;

  if (buffer.size() >= BUFFER_MAX_SIZE) {
    startTransmitting();
  }
};

void Sender::onButtonBPress(MicroBitEvent event) {
  DEBUG(mbit, "Button B is up!");
  startTransmitting();
};

void Sender::startTransmitting() {
  if (!sending && buffer.size() > 0) {
    DEBUG(mbit, "Sending the data soon!");
    sending = true;
  }
};

void Sender::start() {
  while (true) {
    if (sending) {
      transmit();
    }
    mbit->sleep(SENDER_IDLE);
  }
};

void Sender::writeBit(uint8_t bit) {
  mbit->io.P0.setDigitalValue(bit > 0);
  mbit->sleep(TX_SLEEP);
};

void Sender::writeByte(uint8_t byte) {
  for (int i = 0; i < 8; i++) {
    writeBit(byte & (1 << (7 - i)));
  }
};

void Sender::writeHeader() {
  writeBit(0);
  writeBit(1);
  writeBit(0);
  writeBit(1);
}

void Sender::transmit() {
  // convert the signal to an intger
  uint8_t value = morse::stoi(&buffer);

  // empty the buffer
  buffer.clear();

  // TODO: check if the value is a special character
  if (morse::isValid(value)) {

    DEBUG(mbit, "character is: %c", morse::LEXICON[value]);
    // obfuscate the value
    uint8_t obfuscated_value = morse::obfuscate(value, CEASER_SHIFT);

    uint8_t packet[PACKET_SIZE] = {
        morse::ESC, obfuscated_value, utils::parity(obfuscated_value), 0, 0, 0,
        0,          morse::EOW};

    DEBUGF(mbit, "char raw[] = {");
    for (int i = 0; i < PACKET_SIZE; i++) {
      DEBUGF(mbit, "0x%02x %s", packet[i], i == PACKET_SIZE - 1 ? "" : ", ");
      writeByte(packet[i]);
    }
    DEBUG(mbit, "}");

    cipher->encrypt((uint32_t *)packet, 2);

    writeHeader();

    DEBUGF(mbit, "char encrypted[] = {");
    for (int i = 0; i < PACKET_SIZE; i++) {
      DEBUGF(mbit, "0x%02x %s", packet[i], i == PACKET_SIZE - 1 ? "" : ", ");
      writeByte(packet[i]);
    }
    DEBUG(mbit, "}");
  }

  // Reset the value
  mbit->io.P0.setDigitalValue(0);

  // Reset value before returning.
  sending = false;
}
