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
  mbit->messageBus.listen(MICROBIT_ID_IO_P0, MICROBIT_PIN_EVT_PULSE_HI, this,
                          &Receiver::onPulseHigh,
                          MESSAGE_BUS_LISTENER_IMMEDIATE);
  mbit->messageBus.listen(MICROBIT_ID_IO_P0, MICROBIT_PIN_EVT_PULSE_LO, this,
                          &Receiver::onPulseLow,
                          MESSAGE_BUS_LISTENER_IMMEDIATE);
};

void Receiver::tearDownListeners() {
  mbit->messageBus.ignore(MICROBIT_ID_IO_P0, MICROBIT_PIN_EVT_PULSE_HI, this,
                          &Receiver::onPulseHigh);
  mbit->messageBus.ignore(MICROBIT_ID_IO_P0, MICROBIT_PIN_EVT_PULSE_LO, this,
                          &Receiver::onPulseLow);
};

void Receiver::writeBit(uint8_t bit) {

  // If all 8bytes were written, ignore it.
  if (bits_written >= 64 + 8) {
    return;
  };

  uint8_t shift = bits_recieved % 8;
  buffer[bits_written / 8] <<= shift;
  buffer[bits_written / 8] += (bit & 0x1) << shift;
  bits_written++;

  // Check if header is over
  if (++this->bits_recieved >= 8) {
    if (buffer[0] != HEADER) {
      reset();
    } else {
      mid_packet = true;
    }
  }

  if (bits_written >= 64 + 8) {
    onPacket();
  }
};

void Receiver::reset() {
  bits_recieved = 0;
  bits_written = 0;
  mid_packet = 0;
  last_hi_tick = 0;
  // Reset the buffer
  std::memset(buffer, 0, sizeof(buffer));
}

void Receiver::onPacket() {
  DEBUG(mbit, "Complete!");
  reset();
}

void Receiver::onPulseHigh(MicroBitEvent event) {
  if (!this->mid_packet) {
    // This means it's the header.
    DEBUG(mbit, "Got a HI! Packet is starting");

    // Check if header is over
    if (++this->bits_recieved >= 3) {
      mid_packet = true;
    }
    return;
  }

  // We are in mid packet.
  // TODO: repetitions can overflow
  uint8_t repetitions = event.timestamp / TX_SLEEP;
  DEBUG(mbit, "Got a HI with %d repetitions", repetitions);
  for (uint8_t i = 0; i < repetitions; i++) {
    writeBit(1);
  }
}

void Receiver::onPulseLow(MicroBitEvent event) {

  if (!mid_packet && bits_written == 0) {
    return;
  }

  // We are in mid packet.
  // TODO: repetitions can overflow
  uint8_t repetitions = event.timestamp / TX_SLEEP;
  DEBUG(mbit, "Got a LO with %d repetitions", repetitions);
  for (uint8_t i = 0; i < repetitions; i++) {
    writeBit(0);
  }
}

void Receiver::start() {
  while (1) {
    mbit->sleep(100);
  }
}
