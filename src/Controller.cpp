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
