#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "MicroBit.h"
#include "common.hpp"
#include "morse.hpp"
#include <vector>

// Some flexible definitions for ranges on the duration that the user can hold
// down the button/s for.
#define DIT_MIN 250
#define DIT_MAX 500

#define DAH_MIN 750
#define DAH_MAX 1500

#define PACKET_SIZE 3

// Defines how long to wait before sending the message over
#define IDLE_TIME 3500
#define IDLE_CHECK_INTERVAL 100

class Controller {
private:
  // helps differentiate between senders and recievers.
  enum Role { None, Sender, Receiver };

  MicroBit *mbit = new MicroBit();

  vector<MorseTick> buffer;

  // Whether either of the buttons is held down
  bool isButtonAHeldDown = false;

  // Time when the button was held down
  uint64_t holdStartTimestamp = 0;
  uint64_t lastEventTimestamp = 0;

  void transmit();

public:
  void onButtonADown(MicroBitEvent event);
  void onButtonAUp(MicroBitEvent event);
  Controller();
  virtual ~Controller();
  bool isReadyToSend();
  void start();
};

#endif /* CONTROLLER_H */
