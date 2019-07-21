#include "Controller.hpp"
#include "MicroBit.h"
#include "common.hpp"

int main() {
  // Initializes the controller.
  // Notice how the Controller instance is being declared on the heap as opposed
  // to the stack. The MicroBit will page out the data otherwise.
  Controller *controller = new Controller();

  // Start the Controller's main loop
  controller->start();

  // Free the occupied memory
  delete controller;

  release_fiber();
};
