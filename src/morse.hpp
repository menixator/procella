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
  ERR = 256,
};

// Determines whether or not a value is special

bool isSpecial(int value);
// Converts an integer to a deque of booleans
void itos(int value, vector<MorseTick> *encoded);

// Converts a signal to an integer
int stoi(vector<MorseTick> *encoded);

// Obfuscates the value by shifting the value to the right in the lexicon
int obfuscate(int value, int delta);

int deobfuscate(int value, int delta);

} // namespace morse

#endif
