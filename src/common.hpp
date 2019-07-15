
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

#endif /* COMMON_HPP */
