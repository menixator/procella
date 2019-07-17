
#ifndef COMMON_HPP
#define COMMON_HPP

#include "MicroBitComponent.h"
#include <cstdint>

typedef uint8_t MorseTick;

#define DOT 0
#define DASH 1
#define CEASER_SHIFT 42

#define ESCAPE_CHAR '!'

#define TX_WAIT_DOT 250
#define TX_WAIT_DASH 500

// Some flexible definitions for ranges on the duration that the user can hold
// down the button/s for.
#define DIT_MIN 250
#define DIT_MAX 500

#define DAH_MIN 750
#define DAH_MAX 1500
#endif /* COMMON_HPP */
