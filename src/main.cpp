#include "Controller.h"
#include "MicroBit.h"
#include "common.hpp"
//
// Initialize the wrapper
int main() {

  Controller controller;

  controller.start();

  release_fiber();
};
