#include "Receiver.hpp"

Receiver::Receiver(MicroBit *mbit) {
  this->mbit = mbit;
  // Initialize the cipher.
  this->cipher = new XXTEACipher(CIPHER_KEY, CIPHER_KEY_LENGTH);
  // Prepare to listen for events.
  mbit->io.P1.eventOn(MICROBIT_PIN_EVENT_ON_PULSE);
  DEBUG(mbit, "Receiver has been initialized");
  setupListeners();
};

Receiver::~Receiver() {
  // Clean up after ourselves.
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

void Receiver::onBit(uint8_t bit) {
  // If a packet is being processed, ignore the new bit.
  if (bitsRead >= PACKET_SIZE * BITS_PER_BYTE) {
    return;
  };

  // Shift the byte we are writing to the LEFT(since we are receiving BIG ENDIAN
  // data).
  buffer[bitsRead / BITS_PER_BYTE] <<= 1;
  // Add the required bit in.
  buffer[bitsRead / BITS_PER_BYTE] += (bit & 0x1);

  bitsRead++;
  lastActivity = mbit->systemTime();

  // Check if header is over
  if (bitsRead >= BITS_PER_BYTE) {

    // If the first byte is not the marker byte, reset ourselves.
    if (buffer[0] != MARKER_BYTE) {
      reset();
    }
  }

  // If the full packet has been read, call onPacket()
  if (bitsRead >= PACKET_SIZE * BITS_PER_BYTE) {
    onPacket();
  }
};

void Receiver::reset() {
  bitsRead = 0;
  lastActivity = 0;
  // Reset the buffer
  std::memset(buffer, 0, sizeof(buffer));
}

void Receiver::onPacket() {
  DEBUG(mbit, "Complete!");
#if SHOULD_DEBUG
  DEBUGF(mbit, "char encrypted[] = {");
  for (int i = 0; i < PACKET_SIZE; i++) {
    DEBUGF(mbit, "0x%02x, ", buffer[i]);
  }
  DEBUG(mbit, "}");
#endif
  // So apparently, when you call a class method on another class, the member
  // data of the currnet class becomes unavailable? Really frustrating . . . .
  uint8_t encrypted_data[8] = {0};
  std::memcpy(encrypted_data, buffer + 1, 8);

  // Notice the 2, instead of the actual size, 8. This is because the ciper
  // works on 32bit blocks and 8 bytes will equate to 2 32bit blocks.
  cipher->decrypt((uint32_t *)encrypted_data, 2);

#if SHOULD_DEBUG
  DEBUGF(mbit, "char decrypted[] = {");
  for (int i = 0; i < PACKET_BODY_SIZE; i++) {
    DEBUGF(mbit, "0x%02x, ", encrypted_data[i]);
  }
  DEBUG(mbit, "}");
#endif

  if (buffer[0] != MARKER_BYTE || buffer[PACKET_SIZE - 1] != MARKER_BYTE ||
      encrypted_data[0] != morse::ESC ||
      encrypted_data[PACKET_BODY_SIZE - 1] != morse::EOW) {
    DEBUG(mbit, "basic checks failed on the packet!");
    mbit->display.printAsync(CROSS_IMAGE);
    lastScreenActivity = mbit->systemTime();
  } else {
    uint8_t parity = encrypted_data[2];
    uint8_t obfuscated = encrypted_data[1];
    uint8_t deobfuscated = morse::deobfuscate(obfuscated, CEASER_SHIFT);
    if (utils::parity(obfuscated) != parity) {
      DEBUG(mbit, "parity check failed");
      mbit->display.printAsync(CROSS_IMAGE);
      lastScreenActivity = mbit->systemTime();
    } else {
      DEBUG(mbit, "success: character is %c", morse::LEXICON[deobfuscated]);
      mbit->display.printAsync(morse::LEXICON[deobfuscated]);
      lastScreenActivity = mbit->systemTime();
    }
  }

  reset();
};

void Receiver::onPulseHigh(MicroBitEvent event) {
  uint8_t repetitions = (event.timestamp / 1000) / TX_SPEED;
  DEBUG(mbit, "Got a HI with %d repetitions, bits written: %d", repetitions,
        bitsRead);
  for (uint8_t i = 0; i < repetitions; i++) {
    onBit(1);
  }
}

void Receiver::onPulseLow(MicroBitEvent event) {
  // If the first pulse is a low pulse, ignore it.
  if (bitsRead == 0) {
    return;
  }

  // Get the number of repetitions.
  uint8_t repetitions = (event.timestamp / 1000) / TX_SPEED;
  DEBUG(mbit, "Got a LO with %d repetitions, bits written: %d", repetitions,
        bitsRead);
  for (uint8_t i = 0; i < repetitions; i++) {
    onBit(0);
  }
}

void Receiver::start() {
  while (1) {
    mbit->sleep(20);
    // Clears the screen when 1000ms have passed without the screen
    // changing.
    if (lastScreenActivity > 0 &&
        mbit->systemTime() - lastScreenActivity > 1000) {
      lastScreenActivity = 0;
      mbit->display.clear();
    }

    // Reset if a a packet goes incomplete for too long.
    if (lastActivity > 0 && mbit->systemTime() - lastActivity > 3000) {
      reset();
    }
  }
}
