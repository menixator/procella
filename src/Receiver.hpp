#ifndef RECEIVER_H
#define RECEIVER_H
#include "XXTEACipher.hpp"
#include "common.hpp"
#include "morse.hpp"
#include "utils.hpp"

class Receiver {
private:
  /**
   * Number of bits read
   */
  uint8_t bitsRead = 0;

  /**
   * When the last activity was noticed on the GPIO pin.
   */
  uint64_t lastActivity = 0;

  /**
   * When the last screen activity went down. Used to clean up after the screen.
   */
  uint64_t lastScreenActivity = 0;

  /**
   * The pointer to the microbit instance
   */
  MicroBit *mbit;

  /**
   * Internal buffer
   */
  uint8_t buffer[PACKET_SIZE] = {0};

  /**
   *  The cipher instance to decrypt the data
   */
  XXTEACipher *cipher;

  /**
   * Listener setup and destruction will be taken care of by these two methods
   */
  void setupListeners();
  void tearDownListeners();

  // Pulse events
  void onPulseHigh(MicroBitEvent event);
  void onPulseLow(MicroBitEvent event);

  /*
   * Called whenever a bit is received.
   */
  void onBit(uint8_t bit);

  /**
   * Called whenever a full packet is recieved.
   */
  void onPacket();

  /**
   * Resets the interal state and prepares to recieve the next packet.
   */
  void reset();

public:
  // Constructor
  Receiver(MicroBit *mbit);
  virtual ~Receiver();
  /**
   * Starts the main loop
   */
  void start();
};

#endif /* RECEIVER_H */
