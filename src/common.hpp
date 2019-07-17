
#ifndef COMMON_HPP
#define COMMON_HPP

#include "MicroBitComponent.h"
#include <cstdint>

#define SHOULD_DEBUG 1
#ifdef SHOULD_DEBUG
#define DEBUGF(mbit, fmt, ...) mbit->serial.printf(fmt, ##__VA_ARGS__)
#define DEBUG(mbit, fmt, ...) mbit->serial.printf(fmt "\r\n", ##__VA_ARGS__)
#else
#define DEBUG(mbit, fmt, ...) 0
#define DEBUGF(mbit, fmt, ...) 0
#endif

typedef uint8_t MorseTick;

#define DOT 0
#define DASH 1
#define CEASER_SHIFT 42

#define ESCAPE_CHAR '!'

#define TX_SLEEP 40

// Some flexible definitions for ranges on the duration that the user can hold
// down the button/s for.
#define DIT_MIN 0
#define DIT_MAX 500

#define DAH_MIN 500
#define DAH_MAX 2500

#define CIPHER_KEY "TP@abbgW96NC@2?7"
#define CIPHER_KEY_LENGTH 16

#endif /* COMMON_HPP */
