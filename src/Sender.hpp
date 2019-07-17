#ifndef SENDER_HPP
#define SENDER_HPP

#include "MicroBit.h"
#include "common.hpp"
#include "morse.hpp"
#include <vector>

#define SENDER_IDLE 20
#define PACKET_SIZE 3

using namespace std;

class Sender {
private:
  MicroBit *mbit;
  vector<MorseTick> buffer;
  bool sending = false;

  // Timestamp of the last time a was pressed in microseconds.
  uint64_t buttonADownTimestamp = 0;

  void onButtonADown(MicroBitEvent event);
  void onButtonAUp(MicroBitEvent event);
  void onButtonBPress(MicroBitEvent event);

  void setupListeners();
  void tearDownListeners();

  Sender(MicroBit *mbit);
  virtual ~Sender();

  void transmit();

public:
  void start();
};

#endif
