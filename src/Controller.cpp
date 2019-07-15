#include "Controller.h"

// Called for any button down events
void Controller::onButtonADown(MicroBitEvent event) {
  // Check if a button is being held down.
  // If it is, ignore the button press.
  if (isButtonAHeldDown) {
    return;
  }
  mbit->serial.printf("button A is down\r\n");

  isButtonAHeldDown = true;

  // Set lastEventTimestamp to detect idleness
  holdStartTimestamp = lastEventTimestamp = event.timestamp / 1000;
}

void Controller::onButtonAUp(MicroBitEvent event) {
  if (!isButtonAHeldDown) {
    return;
  }

  mbit->serial.printf("button A is up\r\n");

  isButtonAHeldDown = false;
  // event.timestamp is in microseconds

  uint64_t downEndTimestamp = lastEventTimestamp = event.timestamp / 1000;
  uint64_t duration = downEndTimestamp - holdStartTimestamp;

  if (duration >= DIT_MIN && duration < DIT_MAX) {
    mbit->serial.printf("added a dot\r\n");
    buffer.push_back(DOT);
  } else if (duration >= DAH_MIN && duration < DAH_MAX) {
    mbit->serial.printf("added a dash\r\n");
    buffer.push_back(DASH);
  }
}

Controller::Controller() {
  mbit->messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_UP, this,
                          &Controller::onButtonAUp);

  mbit->messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_DOWN, this,
                          &Controller::onButtonADown);
};

Controller::~Controller() {
  mbit->messageBus.ignore(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_UP, this,
                          &Controller::onButtonAUp);

  mbit->messageBus.ignore(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_DOWN, this,
                          &Controller::onButtonADown);
  delete mbit;
};

void Controller::start() {
  while (1) {
    if (isReadyToSend()) {
      mbit->serial.printf("ready to send\r\n");
      transmit();
    } else {
      mbit->serial.printf("not ready to send\r\n");
    }
  }
};

// Checks whether or not the controller is ready to send the data over.
bool Controller::isReadyToSend() {
  return lastEventTimestamp > 0 &&
         (buffer.size() > 0 &&
          (mbit->systemTime() - lastEventTimestamp) > IDLE_TIME);
};

void Controller::transmit() {
  mbit->serial.printf("transmitting\r\n");
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
      // mbit->io.P0.setDigitalValue(1);
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
  // mbit->io.P0.setDigitalValue(0);
}
