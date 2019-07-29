#include "Controller.hpp"

/*
 * Listener for Button A press
 */
void Controller::onButtonAPress(MicroBitEvent event) {
  (void)event;
  mbit->display.animateAsync(TX_ANIM, 300, 5);
  onRoleDecided(Alpha);
};

/*
 * Listener for Button B press
 */
void Controller::onButtonBPress(MicroBitEvent event) {
  (void)event;
  mbit->display.animateAsync(RX_ANIM, 300, 5);
  onRoleDecided(Beta);
};

// Helper to set roles.
void Controller::onRoleDecided(Role newRole) {
  if (role != None) {
    return;
  }

  // When a role is decided, the listeners are torn down before changing the
  // role
  tearDownListeners();

  role = newRole;
}

Controller::Controller() {
  // Initialize the required components. ESPECIALLY THE SCHEDULER.
  mbit->init();
  mbit->radio.enable();
  setupListeners();
};

/*
 * Sets up the basic listeners required for the Controller component
 */
void Controller::setupListeners() {
  mbit->messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, this,
                          &Controller::onButtonAPress);
  mbit->messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, this,
                          &Controller::onButtonBPress);
};

/*
 * Removes the basic listeners required for the component.
 */
void Controller::tearDownListeners() {
  // If the role has been decided, assume that the listeners are already
  // removed.
  if (role != None) {
    return;
  }
  mbit->messageBus.ignore(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, this,
                          &Controller::onButtonAPress);
  mbit->messageBus.ignore(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, this,
                          &Controller::onButtonBPress);
};

/*
 * The constructor. Takes no arguments.
 */
Controller::~Controller() {
  tearDownListeners();
  delete mbit;
};

/*
 * The main activity of the Controller is done here
 */
void Controller::start() {
  INFO(mbit, "Controller is starting");

  // Sleep while the role is undecided.
  while (role == None) {
    mbit->sleep(TX_SPEED);
  }

  // Start the designated role controllers when the role is decided.
  if (role == Alpha) {
    INFO(mbit, "Initialized as a sender!");
    Sender *sender = new Sender(mbit);
    INFO(mbit, "Sender has been intialized");
    sender->start();

  } else if (role == Beta) {
    INFO(mbit, "Initialized as a receiver!");
    Receiver *receiver = new Receiver(mbit);
    INFO(mbit, "Receiver has been intialized");
    receiver->start();
  }
};
