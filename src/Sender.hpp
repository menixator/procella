#ifndef SENDER_HPP
#define SENDER_HPP

#include "MicroBit.h"
#include "XXTEACipher.hpp"
#include "common.hpp"
#include "morse.hpp"
#include "utils.hpp"
#include <vector>

#define SENDER_IDLE 20

#define PACKET_SIZE 8

using namespace std;

class Sender {
private:
  MicroBit *mbit;
  vector<MorseTick> buffer;
  XXTEACipher *cipher;

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
  void sendPrelude();
  void writeBit(uint8_t bit);
  void writeByte(uint8_t byte);

public:
  void start();
};

#endif
