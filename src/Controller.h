#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "MicroBit.h"
#include "Receiver.hpp"
#include "Sender.hpp"
#include "common.hpp"

/**
 * Decides how long the Controller component will put the microbit into a sleep
 * when the role is undecided.
 */
#define ROLE_CHECK_INTERVAL 15

class Controller {
private:
  /**
   * Role enumeration that is used to differentiate whether or not the microbit
   * the program is running is is a Sender, or a Reciever.
   */
  enum Role { None, Alpha, Beta };

  /*
   * The main MicroBit instance. Notice how it's a pointer. This is to prevent
   * the object from being paged out.
   */
  MicroBit *mbit = new MicroBit();

  // Initialize role as none.
  Role role = None;

public:
  /**
   * Event listeners for Button A and Button B's press events.
   */
  void onButtonAPress(MicroBitEvent event);
  void onButtonBPress(MicroBitEvent event);
  void onRoleDecided(Role role);

  /**
   * Helpers that setup listeners and tears them down when required.
   */
  void setupListeners();
  void tearDownListeners();

  /**
   * The Constructor of the class.
   */
  Controller();
  /**
   * Destructor of the class.
   */
  virtual ~Controller();
  void start();
};

#endif /* CONTROLLER_H */
