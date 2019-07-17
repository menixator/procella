#ifndef MORSE_H
#define MORSE_H
#include "common.hpp"
#include <algorithm>
#include <cstring>
#include <vector>

using namespace std;

namespace morse {

/*
 * The lexicon contains all the characters that can be encoded/decoded to/from
 * morse code. The exclamation mark is a placeholder to make sure that the
 * binary tree is a complete one.
 */
extern const char *LEXICON;

extern const int LEXICON_LENGTH;

enum ControlSequences {
  // Escape sequence is not valid in the payload. It ALWAYS has a special
  // meaning.
  ESC = 19,
  // Acknowledgement.
  OK = 34,
  // This was present in the spec. It corresponds to an acute accented e.
  ACUTE_E = 36,
  // Halt
  HLT = 40,
  // Transmitting
  TX = 53,
  // End Of Work
  EOW = 69,
  // Error
  ERR = 128,
};

// Determines whether or not a value is special

bool isSpecial(uint8_t value);
// Converts an uint8_teger to a deque of booleans
void itos(uint8_t value, vector<MorseTick> *encoded);

// Converts a signal to an uint8_teger
uint8_t stoi(vector<MorseTick> *encoded);

// Obfuscates the value by shifting the value to the right in the lexicon
uint8_t obfuscate(uint8_t value, uint8_t delta);

uint8_t deobfuscate(uint8_t value, uint8_t delta);

} // namespace morse

#endif
