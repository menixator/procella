#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "MicroBit.h"
#include "common.hpp"
#include "morse.hpp"
#include <vector>

// How long to sleep when role is undecided
#define ROLE_CHECK_INTERVAL 15

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
