#ifndef SENDER_HPP
#define SENDER_HPP

#include "MicroBit.h"
#include "XXTEACipher.hpp"
#include "common.hpp"
#include "morse.hpp"
#include "utils.hpp"
#include <vector>

#define BUFFER_MAX_SIZE 6

using namespace std;

class Sender {
private:
  /*
   * The MicroBit instance passed down from the Controller.
   */
  MicroBit *mbit;
  /*
   * A std::vector<MorseTick> type that holds all the data that the user has
   * input
   */
  vector<MorseTick> buffer;

  /**
   * A XXTEACipher instance used to encrypt the data that we are about to send.
   */
  XXTEACipher *cipher;

  /*
   * Simple boolean flag indicating whether or not we are sending any data.
   */
  bool sending = false;

  /**
   * Timestamp of the last time a was pressed in microseconds.
   */
  uint64_t buttonADownTimestamp = 0;

  /**
   * Event listeners for various events.
   */
  void onButtonADown(MicroBitEvent event);
  void onButtonAUp(MicroBitEvent event);
  void onButtonBPress(MicroBitEvent event);

  /**
   * Helpers to set up listeners and tear them down when required.
   */
  void setupListeners();
  void tearDownListeners();

  /**
   * Starts transmitting data on the GPIO pin when the user has deemed it
   * ready to be sent.
   */
  void transmit();

  /**
   * Writes a distinguishable sequences of bits to the GPIO pin that starts and
   * ends with a high bit.
   */
  void writeMarker();

  /**
   * Writes a single bit to the GPIO.
   */
  void writeBit(uint8_t bit);
  /**
   * Writes a byte to the GPIO pin.
   */
  void writeByte(uint8_t byte);

  /**
   * Helpers to allow the transmission to be started when the buffer reaches a
   * limit.
   */
  void startTransmitting();

public:
  /**
   * Constructor for the Sender class. Requires a pointer to a microbit
   * instance.
   */
  Sender(MicroBit *mbit);
  /**
   * Destructor. Frees any memory allocated dynamically.
   */
  virtual ~Sender();

  /**
   * The public facing function which starts the activity of the sender.
   */
  void start();
};

#endif
