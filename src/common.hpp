
#ifndef COMMON_HPP
#define COMMON_HPP

#include "MicroBit.h"
#include <cstdint>

#define DEBUG 1
#if DEBUG
#define INFOF(mbit, fmt, ...)                                                  \
  do {                                                                         \
    if (DEBUG)                                                                 \
      mbit->serial.printf(fmt, ##__VA_ARGS__);                                 \
  } while (0)
#define INFO(mbit, fmt, ...)                                                   \
  do {                                                                         \
    if (DEBUG)                                                                 \
      mbit->serial.printf(fmt "\r\n", ##__VA_ARGS__);                          \
  } while (0)
#endif

typedef uint8_t MorseTick;

#define DOT 0
#define DASH 1
#define CEASER_SHIFT 42

/**
 * Escape character for the morse lexicon
 */
#define ESCAPE_CHAR '!'

/**
 * Transmission speed. This reflects how many milliseconds it will
 * take to transfer 1 bit over from 1 microbit to another.
 */
#define TX_SPEED 35

/**
 * Some flexible definitions for ranges on the duration that the user can hold
 * down the button/s for.
 */
#define DIT_MIN 0
#define DIT_MAX 500

#define DAH_MIN 500
#define DAH_MAX 2500

/**
 * Some definitions to avoid the classic 'magic number' pitfall
 */

#define BITS_PER_BYTE 8
/**
 * The actual packet size.
 */
#define PACKET_SIZE 10
/**
 * The body size(this will be the number of BYTES that will be encrypted.
 */
#define PACKET_BODY_SIZE 8

/**
 * A marker byte which will start and end on a . . . high note . . .
 */
#define MARKER_BYTE 0xAB

/**
 * The cipher key and the length of the cipher key.
 */
#define CIPHER_KEY "TP@abbgW96NC@2?7"
#define CIPHER_KEY_LENGTH 16

/**
 * Some definitions to make defining the animations easier.
 */
#define ARROW_HEAD 0, 0, 255, 0, 0
#define ARROW_BODY 0, 255, 0, 255, 0
#define ARROW_TAIL 0, 0, 0, 0, 0

/**
 * Store the animation data on the flash memory.
 * Refer to:
 * https://lancaster-university.github.io/microbit-docs/data-types/image/#storing-images-in-flash-memory
 */
// clang-format off
const uint8_t TX_ANIM_DATA[] __attribute__((aligned(4))) = {
  0xff, 0xff,
  // Width, followed by the mandatory 0
  25, 0,
  // Height, followed by the mandatory 0
  5, 0,
  ARROW_TAIL,  ARROW_TAIL, ARROW_HEAD, ARROW_BODY, ARROW_TAIL,
  ARROW_TAIL,  ARROW_HEAD, ARROW_BODY, ARROW_TAIL, ARROW_TAIL,
  ARROW_HEAD,  ARROW_BODY, ARROW_TAIL, ARROW_TAIL, ARROW_TAIL,
  ARROW_BODY,  ARROW_TAIL, ARROW_TAIL, ARROW_TAIL, ARROW_HEAD,
  ARROW_TAIL,  ARROW_TAIL, ARROW_TAIL, ARROW_HEAD, ARROW_BODY,
};

const uint8_t RX_ANIM_DATA[] __attribute__((aligned(4))) = {
  0xff, 0xff,
  // Width, followed by the mandatory 0
  25, 0,
  // Height, followed by the mandatory 0
  5, 0,
  ARROW_TAIL,  ARROW_TAIL, ARROW_TAIL, ARROW_HEAD, ARROW_BODY,
  ARROW_BODY,  ARROW_TAIL, ARROW_TAIL, ARROW_TAIL, ARROW_HEAD,
  ARROW_HEAD,  ARROW_BODY, ARROW_TAIL, ARROW_TAIL, ARROW_TAIL,
  ARROW_TAIL,  ARROW_HEAD, ARROW_BODY, ARROW_TAIL, ARROW_TAIL,
  ARROW_TAIL,  ARROW_TAIL, ARROW_HEAD, ARROW_BODY, ARROW_TAIL,
};
// clang-format on

const MicroBitImage CROSS_IMAGE("255,0,255\n0,255,0\n255,0,255\n");
const MicroBitImage
    DOT_IMAGE("0,0,0,0,0\n 0,0,0,0,0\n 0,0,255,0,0\n 0,0,0,0,0\n 0,0,0,0,0\n");
const MicroBitImage DASH_IMAGE(
    "0,0,0,0,0\n 0,0,0,0,0\n 0,255,255,255,0\n 0,0,0,0,0\n 0,0,0,0,0\n");
const MicroBitImage TX_ANIM((ImageData *)TX_ANIM_DATA);
const MicroBitImage RX_ANIM((ImageData *)RX_ANIM_DATA);

#endif /* COMMON_HPP */
