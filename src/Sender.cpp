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

  uint64_t buttonAUpTimestamp = event.timestamp / 1000;
  uint64_t duration = buttonAUpTimestamp - buttonADownTimestamp;

  if (duration >= DIT_MIN && duration < DIT_MAX) {
    buffer.push_back(DOT);
  } else if (duration >= DAH_MIN && duration < DAH_MAX) {
    buffer.push_back(DASH);
  }

  // Reset the value.
  buttonADownTimestamp = 0;
};

void Sender::onButtonBPress(MicroBitEvent event) {
  if (!sending && buffer.size() > 0) {
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
  if (!morse::isSpecial(value)) {
    // obfuscate the value
    uint8_t obfuscated_value = morse::obfuscate(value, CEASER_SHIFT);

    uint8_t packet[PACKET_SIZE] = {
        morse::ESC, obfuscated_value, utils::parity(obfuscated_value), 0, 0, 0,
        0,          morse::EOW};

    cipher->encrypt((uint32_t *)packet, 2);

    writeHeader();

    for (int i = 0; i < PACKET_SIZE; i++) {
      writeByte(packet[i]);
    }
  }

  // Reset the value
  mbit->io.P0.setDigitalValue(0);

  // Reset value before returning.
  sending = false;
}
