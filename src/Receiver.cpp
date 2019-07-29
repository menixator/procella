#include "Receiver.hpp"

Receiver::Receiver(MicroBit *mbit) {
  this->mbit = mbit;
  INFO(mbit, "Receiver has been initialized");
  // Initialize the cipher.
  this->cipher = new XXTEACipher(CIPHER_KEY, CIPHER_KEY_LENGTH);
  // Prepare to listen for events.
  setupListeners();
};

Receiver::~Receiver() {
  // Clean up after ourselves.
  tearDownListeners();
  delete cipher;
};

void Receiver::setupListeners() {
  INFO(mbit, "Setting up listeners");
  mbit->messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, this,
                          &Receiver::onPacket);
};

void Receiver::tearDownListeners() {
  mbit->messageBus.ignore(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, this,
                          &Receiver::onPacket);
};

void Receiver::onPacket(MicroBitEvent _event) {
  INFO(mbit, "Complete!");
  uint8_t buffer[PACKET_SIZE] = {0};
  mbit->radio.datagram.recv(buffer, PACKET_SIZE);
#if DEBUG
  INFOF(mbit, "char encrypted[] = {");
  for (int i = 0; i < PACKET_SIZE; i++) {
    INFOF(mbit, "0x%02x, ", buffer[i]);
  }
  INFO(mbit, "}");
#endif
  // So apparently, when you call a class method on another class, the member
  // data of the currnet class becomes unavailable? Really frustrating . . . .
  uint8_t encrypted_data[8] = {0};
  std::memcpy(encrypted_data, buffer + 1, 8);

  // Notice the 2, instead of the actual size, 8. This is because the ciper
  // works on 32bit blocks and 8 bytes will equate to 2 32bit blocks.
  cipher->decrypt((uint32_t *)encrypted_data, 2);

#if DEBUG
  INFOF(mbit, "char decrypted[] = {");
  for (int i = 0; i < PACKET_BODY_SIZE; i++) {
    INFOF(mbit, "0x%02x, ", encrypted_data[i]);
  }
  INFO(mbit, "}");
#endif

  if (buffer[0] != MARKER_BYTE || buffer[PACKET_SIZE - 1] != MARKER_BYTE ||
      encrypted_data[0] != morse::ESC ||
      encrypted_data[PACKET_BODY_SIZE - 1] != morse::EOW) {
    INFO(mbit, "basic checks failed on the packet!");
    mbit->display.printAsync(CROSS_IMAGE);
    lastScreenActivity = mbit->systemTime();
  } else {
    uint8_t parity = encrypted_data[2];
    uint8_t obfuscated = encrypted_data[1];
    uint8_t deobfuscated = morse::deobfuscate(obfuscated, CEASER_SHIFT);
    if (utils::parity(obfuscated) != parity) {
      INFO(mbit, "parity check failed");
      mbit->display.printAsync(CROSS_IMAGE);
      lastScreenActivity = mbit->systemTime();
    } else {
      INFO(mbit, "success: character is %c", morse::LEXICON[deobfuscated]);
      mbit->display.printAsync(morse::LEXICON[deobfuscated]);
      lastScreenActivity = mbit->systemTime();
    }
  }

  reset();
};

void Receiver::start() {
  while (true) {
    mbit->sleep(TX_SPEED);
    // Clears the screen when 1000ms have passed without the screen
    // changing.
    if (lastScreenActivity > 0 &&
        mbit->systemTime() - lastScreenActivity > 1000) {
      lastScreenActivity = 0;
      mbit->display.clear();
    }
  }
}

void Receiver::reset() {}
