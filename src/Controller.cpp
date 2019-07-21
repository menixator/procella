#include "Controller.h"

void Controller::onButtonAPress(MicroBitEvent event) {
  mbit->display.animateAsync(TX_ANIM, 300, 5);
  onRoleDecided(Alpha);
};

void Controller::onButtonBPress(MicroBitEvent event) {
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
  mbit->init();
  setupListeners();
};

void Controller::setupListeners() {
  mbit->messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, this,
                          &Controller::onButtonAPress);
  mbit->messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, this,
                          &Controller::onButtonBPress);
};

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
}

Controller::~Controller() {
  tearDownListeners();
  delete mbit;
};

void Controller::start() {
  DEBUG(mbit, "Controller is starting");
  while (role == None) {
    mbit->sleep(ROLE_CHECK_INTERVAL);
  }
  if (role == Alpha) {
    DEBUG(mbit, "Initialized as a sender!");
    Sender *sender = new Sender(mbit);
    DEBUG(mbit, "Sender has been intialized");
    sender->start();
  } else if (role == Beta) {
    DEBUG(mbit, "Initialized as a reciever!");
    Receiver *reciever = new Receiver(mbit);
    DEBUG(mbit, "Reciever has been intialized");
    reciever->start();
  }
};
