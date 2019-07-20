#include "Receiver.hpp"

Receiver::Receiver(MicroBit *mbit) {
  this->mbit = mbit;
  this->cipher = new XXTEACipher(CIPHER_KEY, CIPHER_KEY_LENGTH);
  mbit->io.P1.eventOn(MICROBIT_PIN_EVENT_ON_PULSE);
  DEBUG(mbit, "Receiver has been initialized");
  setupListeners();
};

Receiver::~Receiver() {
  tearDownListeners();
  delete cipher;
};

void Receiver::setupListeners() {
  DEBUG(mbit, "Setting up listeners");
  mbit->messageBus.listen(MICROBIT_ID_IO_P1, MICROBIT_PIN_EVT_PULSE_HI, this,
                          &Receiver::onPulseHigh,
                          MESSAGE_BUS_LISTENER_IMMEDIATE);
  mbit->messageBus.listen(MICROBIT_ID_IO_P1, MICROBIT_PIN_EVT_PULSE_LO, this,
                          &Receiver::onPulseLow,
                          MESSAGE_BUS_LISTENER_IMMEDIATE);
};

void Receiver::tearDownListeners() {
  mbit->messageBus.ignore(MICROBIT_ID_IO_P1, MICROBIT_PIN_EVT_PULSE_HI, this,
                          &Receiver::onPulseHigh);
  mbit->messageBus.ignore(MICROBIT_ID_IO_P1, MICROBIT_PIN_EVT_PULSE_LO, this,
                          &Receiver::onPulseLow);
};

void Receiver::writeBit(uint8_t bit) {

  // If all 8bytes were written, ignore it.
  if (bits_written >= 64 + 8 + 8) {
    return;
  };

  buffer[bits_written / 8] <<= 1;
  buffer[bits_written / 8] += (bit & 0x1);
  bits_written++;

  // Check if header is over
  if (this->bits_written >= 8) {
    if (buffer[0] != HEADER) {
      reset();
    }
  }

  if (bits_written >= 64 + 8 + 8) {
    onPacket();
  }
};

void Receiver::reset() {
  bits_written = 0;
  last_hi_tick = 0;
  // Reset the buffer
  std::memset(buffer, 0, sizeof(buffer));
}

void Receiver::onPacket() {
  DEBUG(mbit, "Complete!");
  if (SHOULD_DEBUG) {
    DEBUGF(mbit, "char encrypted[] = {");
    for (int i = 0; i < PACKET_SIZE; i++) {
      DEBUGF(mbit, "0x%02x, ", buffer[i]);
    }
    DEBUG(mbit, "}");
  }
  // So apparently, when you call a class method, the member data becomes
  // unavailable? Really frustrating . . . .
  uint8_t encrypted_data[8] = {0};
  std::memcpy(encrypted_data, buffer + 1, 8);

  cipher->decrypt((uint32_t *)encrypted_data, 2);

  if (SHOULD_DEBUG) {
    DEBUGF(mbit, "char decrypted[] = {");
    for (int i = 0; i < PACKET_BODY_SIZE; i++) {
      DEBUGF(mbit, "0x%02x, ", encrypted_data[i]);
    }
    DEBUG(mbit, "}");
  }

  if (buffer[0] != HEADER || buffer[PACKET_SIZE - 1] != HEADER ||
      encrypted_data[0] != morse::ESC ||
      encrypted_data[PACKET_BODY_SIZE - 1] != morse::EOW) {
    // TODO: fail
  } else {
    uint8_t parity = encrypted_data[2];
    uint8_t deobfuscated = morse::deobfuscate(encrypted_data[1], CEASER_SHIFT);
    mbit->display.print(morse::LEXICON[deobfuscated]);
    // TODO: verify parity
  }

  reset();
};

void Receiver::onPulseHigh(MicroBitEvent event) {

  // We are in mid packet.
  // TODO: repetitions can overflow
  uint8_t repetitions = (event.timestamp / 1000) / TX_SLEEP;
  DEBUG(mbit, "Got a HI with %d repetitions, bits written: %d", repetitions,
        bits_written);
  for (uint8_t i = 0; i < repetitions; i++) {
    writeBit(1);
  }
}

void Receiver::onPulseLow(MicroBitEvent event) {

  if (bits_written == 0) {
    return;
  }

  // We are in mid packet.
  // TODO: repetitions can overflow
  uint8_t repetitions = (event.timestamp / 1000) / TX_SLEEP;
  DEBUG(mbit, "Got a LO with %d repetitions, bits written: %d", repetitions,
        bits_written);
  for (uint8_t i = 0; i < repetitions; i++) {
    writeBit(0);
  }
}

void Receiver::start() {
  while (1) {
    mbit->sleep(20);
  }
}
