#include "Controller.h"

void Controller::onButtonAPress(MicroBitEvent event) { onRoleDecided(Sender); };

void Controller::onButtonBPress(MicroBitEvent event) {
  onRoleDecided(Receiver);
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

Controller::Controller() { setupListeners(); };

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

Controller::~Controller() { delete mbit; };

void Controller::start() {
  while (role == None) {
    mbit->sleep(ROLE_CHECK_INTERVAL);
  }

  // TODO: either read or write.
};
