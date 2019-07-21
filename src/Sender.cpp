#include "Sender.hpp"

Sender::Sender(MicroBit *mbit) {
  this->mbit = mbit;

  // The cipher has to be initialized on the HEAP.
  cipher = new XXTEACipher(CIPHER_KEY, CIPHER_KEY_LENGTH);

  // Sets up listeners immediately.
  setupListeners();
};

Sender::~Sender() {
  tearDownListeners();
  // Free the cipher instance.
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

  mbit->display.printAsync(DOT_IMAGE);
  // If button A was somehow already down, ignore the current event.
  if (buttonADownTimestamp > 0) {
    return;
  }
  DEBUG(mbit, "Button A is down!");

  // event.timestamp is in microseconds. To get milliseconds, we are dividing it
  // by 1000.
  buttonADownTimestamp = event.timestamp / 1000;
};

void Sender::onButtonAUp(MicroBitEvent event) {

  // If we are already sending the data, ignore this event.
  if (sending) {
    return;
  }
  // If button A was somehow already up, ignore this event.
  if (buttonADownTimestamp == 0) {
    return;
  }
  DEBUG(mbit, "Button A is up!");

  // event.timestamp is in microseconds, convert it to milliseconds
  uint64_t buttonAUpTimestamp = event.timestamp / 1000;

  // Get the duration for which, button a was down.
  uint64_t duration = buttonAUpTimestamp - buttonADownTimestamp;

  // Push the appropriate values to the buffer if the duration for which button
  // A was down fits the ranges defined by the macros (DIT/DAH)_(MIN/MAX).
  if (duration >= DIT_MIN && duration < DIT_MAX) {
    DEBUG(mbit, "Dot added!");
    buffer.push_back(DOT);
  } else if (duration >= DAH_MIN && duration < DAH_MAX) {
    DEBUG(mbit, "Dash added!");
    buffer.push_back(DASH);
  }

  // Reset the buttonADownTimestamp value.
  buttonADownTimestamp = 0;

  // Wipe any drawn images.
  mbit->display.clear();

  // If the buffer is 'full'. Start transmitting the data.
  if (buffer.size() >= BUFFER_MAX_SIZE) {
    startTransmitting();
  }
};

/**
 * Button B press listener that will immediately start transmitting data.
 */
void Sender::onButtonBPress(MicroBitEvent event) {
  (void)event;
  DEBUG(mbit, "Button B is up!");
  startTransmitting();
};

void Sender::startTransmitting() {
  if (!sending && buffer.size() > 0) {
    DEBUG(mbit, "Sending the data soon!");
    // Since all the data is being set in the main fiber, we just set a flag in
    // the listeners.
    sending = true;
  }
};

/**
 * Main loop of the Sender component..
 */
void Sender::start() {
  while (true) {

    if (sending) {
      transmit();
    }

    // Displays a dash on the screen if button A has been held down an
    // appropriate amount of time.
    if (buttonADownTimestamp > 0 &&
        (mbit->systemTime() - buttonADownTimestamp) > DAH_MIN &&
        (mbit->systemTime() - buttonADownTimestamp) < DAH_MAX) {
      mbit->display.printAsync(DASH_IMAGE);
    } else if (buttonADownTimestamp > 0 && (mbit->systemTime() - buttonADownTimestamp) > DAH_MAX) {
      // Otherwise, just clear the image.
      mbit->display.clear();
    }
    mbit->sleep(SENDER_IDLE);
  }
};

void Sender::writeBit(uint8_t bit) {
// A debugging routine that checks if we have slept an appropriate amount of
// time.
#if SHOULD_DEBUG
  static uint64_t last_call = 0;
  if (last_call > 0) {
    uint64_t diff = mbit->systemTime() - last_call;
    if (diff < TX_SLEEP) {
      DEBUG(mbit, "diff is: %d milliseconds", diff);
    }
  }
  last_call = mbit->systemTime();
#endif

  // Writes the value to the GPIO pin.
  mbit->io.P0.setDigitalValue(bit > 0);
  mbit->sleep(TX_SLEEP);
};

void Sender::writeByte(uint8_t byte) {
  // Loop over every bit in byte and sends it using writeBit.
  // This will write the bits in BIG ENDIAN ORDER.
  for (int i = 0; i < 8; i++) {
    writeBit(byte & (1 << (7 - i)));
  }
};

/**
 * Writes the marker byte
 */
void Sender::writeMarker() {
  writeBit(0);
  writeByte(MARKER_BYTE);
};

void Sender::transmit() {
  mbit->display.clear();
  // *sighs* eyecandy . . .
  mbit->display.animateAsync(TX_ANIM, 650, 5);

  // the vector buffer to an integer.
  uint8_t value = morse::stoi(&buffer);

  DEBUG(mbit, "value is: %d", value);

#if SHOULD_DEBUG
  DEBUGF(mbit, "The sequence is: ");
  for (MorseTick tick : buffer) {
    if (tick == DOT) {
      DEBUGF(mbit, ".");
    } else {
      DEBUGF(mbit, "-");
    }
  }

  DEBUG(mbit, "");
#endif
  // empty the buffer
  buffer.clear();

#if SHOULD_DEBUG
  DEBUGF(mbit, "Sequence starting from value: ");

  for (int i = value; i < morse::LEXICON_LENGTH; i++) {
    DEBUGF(mbit, "%c", morse::LEXICON[i]);
  }
  DEBUG(mbit, "");
#endif

  // Only send the value if it is a valid morse character.
  if (morse::isValid(value)) {

    DEBUG(mbit, "character is: %c", morse::LEXICON[value]);
    // obfuscate the value
    uint8_t obfuscated_value = morse::obfuscate(value, CEASER_SHIFT);

    DEBUG(mbit, "obfuscated value is: %d", obfuscated_value);

    // seed the RNG
    mbit->seedRandom();
    // Generate a 32 bit random number to fill the empty bytes
    int padding = mbit->random(0x7FFFFFFF);

    // Packet construction. The random number is filled into the unused places
    // to prevent the microbit from outputting the same sequence of bytes for
    // the same character.
    uint8_t packet[PACKET_BODY_SIZE] = {morse::ESC,
                                        obfuscated_value,
                                        utils::parity(obfuscated_value),
                                        (uint8_t)(padding & 0xFF),
                                        (uint8_t)((padding >> 8) & 0xFF),
                                        (uint8_t)((padding >> 16) & 0xFF),
                                        (uint8_t)((padding >> 24) & 0xFF),
                                        morse::EOW};

#if SHOULD_DEBUG
    DEBUGF(mbit, "char raw[] = {");
    for (int i = 0; i < PACKET_BODY_SIZE; i++) {
      DEBUGF(mbit, "0x%02x %s", packet[i],
             i == PACKET_BODY_SIZE - 1 ? "" : ", ");
    }
    DEBUG(mbit, "}");
#endif

    // Encrypt the data.
    cipher->encrypt((uint32_t *)packet, 2);

#if SHOULD_DEBUG
    DEBUGF(mbit, "char encrypted[] = {");
    for (int i = 0; i < PACKET_BODY_SIZE; i++) {
      DEBUGF(mbit, "0x%02x %s", packet[i],
             i == PACKET_BODY_SIZE - 1 ? "" : ", ");
    }
    DEBUG(mbit, "}");
#endif

    writeMarker();

    for (int i = 0; i < PACKET_BODY_SIZE; i++) {
      writeByte(packet[i]);
    }

    writeByte(MARKER_BYTE);

    mbit->io.P0.setDigitalValue(0);
  }

  // Reset value before returning.
  sending = false;
};
