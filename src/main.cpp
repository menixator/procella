#include "Controller.hpp"
#include "MicroBit.h"
#include "common.hpp"
//
// Initialize the wrapper
int main() {

  Controller *controller = new Controller();

  controller->start();

  release_fiber();
};
