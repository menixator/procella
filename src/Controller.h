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

// How long to sleep when role is undecided
#define ROLE_CHECK_INTERVAL 15

#define PACKET_SIZE 3

class Controller {
private:
  // helps differentiate between senders and recievers.
  enum Role { None, Sender, Receiver };

  MicroBit *mbit = new MicroBit();

  // Initialize role as none.
  Role role = None;

public:
  void onButtonAPress(MicroBitEvent event);
  void onButtonBPress(MicroBitEvent event);
  void onRoleDecided(Role role);

  void setupListeners();
  void tearDownListeners();

  Controller();
  virtual ~Controller();
  void start();
};

#endif /* CONTROLLER_H */
