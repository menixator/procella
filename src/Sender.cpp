#include "Sender.hpp"

Sender::Sender(MicroBit *mbit) { this->mbit = mbit; };

Sender::~Sender() { tearDownListeners(); };

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

void Sender::transmit() {
  // convert the signal to an intger
  int value = morse::stoi(&buffer);

  // empty the buffer
  buffer.clear();

  // obfuscate the value
  int obfuscated_value = morse::obfuscate(value, CEASER_SHIFT);

  int packet[PACKET_SIZE] = {morse::ESC, obfuscated_value, morse::EOW};

  // Reset the value
  // mbit->io.P0.setDigitalValue(0);
  vector<MorseTick> tx_buffer;
  int delay;

  for (const int &data : packet) {
    morse::itos(data, &tx_buffer);
    for (MorseTick tick : tx_buffer) {
      mbit->io.P0.setDigitalValue(1);
      delay = 0;

      if (tick == DASH) {
        delay = TX_WAIT_DASH;
      } else if (tick == DOT) {
        delay = TX_WAIT_DOT;
      }

      if (delay > 0) {
        mbit->sleep(delay);
      }
    }

    tx_buffer.clear();
  }
  // Reset the value
  mbit->io.P0.setDigitalValue(0);

  // Reset value before returning.
  sending = false;
}
