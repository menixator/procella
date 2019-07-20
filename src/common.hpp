
#ifndef COMMON_HPP
#define COMMON_HPP

#include "MicroBit.h"
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

#define PACKET_SIZE 10
#define PACKET_BODY_SIZE 8
#define BITS_TO_RECEIVE 67
#define HEADER 0xAB

#define CIPHER_KEY "TP@abbgW96NC@2?7"
#define CIPHER_KEY_LENGTH 16

#define ARROW_TIP 0, 0, 255, 0, 0, 0, 255, 0, 255, 0, 0, 0, 0, 0, 0
#define ARROW_BODY 0, 255, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 0
#define ARROW_TAIL 0, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0, 255, 0, 255, 0

const uint8_t TX_ANIM_DATA[] __attribute__((aligned(4))) = {
    0xff,      0xff,       15,         0,         5,         0,
    ARROW_TIP, ARROW_BODY, ARROW_TAIL, ARROW_TIP, ARROW_BODY};

const uint8_t RX_ANIM_DATA[] __attribute__((aligned(4))) = {
    0xff,       0xff,      15,         0,          5,        0,
    ARROW_BODY, ARROW_TIP, ARROW_TAIL, ARROW_BODY, ARROW_TIP};

const MicroBitImage CROSS_IMAGE("0,255,0\n255,255,255\n0,255,0\n");
const MicroBitImage
    DOT_IMAGE("0,0,0,0,0\n 0,0,0,0,0\n 0,0,255,0,0\n 0,0,0,0,0\n 0,0,0,0,0\n");
const MicroBitImage DASH_IMAGE(
    "0,0,0,0,0\n 0,0,0,0,0\n 0,255,255,255,0\n 0,0,0,0,0\n 0,0,0,0,0\n");
const MicroBitImage TX_ANIM((ImageData *)TX_ANIM_DATA);
const MicroBitImage RX_ANIM((ImageData *)RX_ANIM_DATA);

#endif /* COMMON_HPP */
