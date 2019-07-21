#ifndef RECEIVER_H
#define RECEIVER_H
#include "XXTEACipher.hpp"
#include "common.hpp"
#include "morse.hpp"
#include "utils.hpp"

class Receiver {
private:
  uint8_t bitsRead = 0;

  // Last hi tick is used to invalidate the data
  uint64_t lastActivity = 0;
  uint64_t lastScreenActivity = 0;

  // The pointer to the microbit
  MicroBit *mbit;

  uint8_t buffer[PACKET_SIZE] = {0};

  // Initialize a cipher instance to encrypt and decrypt the read data
  XXTEACipher *cipher;

  // Listener setup and destruction will be taken care of by these two methods
  void setupListeners();
  void tearDownListeners();

  // Pulse events
  void onPulseHigh(MicroBitEvent event);
  void onPulseLow(MicroBitEvent event);

  void onBit(uint8_t bit);
  void onPacket();
  void reset();

public:
  // Constructor
  Receiver(MicroBit *mbit);
  virtual ~Receiver();
  void start();
};

#endif /* RECEIVER_H */
