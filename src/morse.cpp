#include "morse.hpp"

namespace morse {

/*
 * The lexicon contains all the characters that can be encoded/decoded to/from
 * morse code. The exclamation mark is a placeholder to make sure that the
 * binary tree is a complete one.
 */
const char *LEXICON = "etianmsurwdkgohvf!l!pjbxcyzq!!54!3!!!2!!+!!!!16=/"
                      "!!!(!7!8!90!!!!?!\"!!.@!\'!!-!)!,:!!!!!";

const int LEXICON_LENGTH = strlen(LEXICON);

// Determines whether or not a value is special
bool isSpecial(int value) {
  switch (value) {
  case ESC:
  case OK:
  case ACUTE_E:
  case HLT:
  case TX:
  case EOW:
  case ERR:
    return true;
  default:
    return false;
  };
}

// Converts an integer to a deque of booleans
void itos(int value, vector<MorseTick> *encoded) {
  // If position is even, it's a dot
  while (value > 1) {
    if (value % 2 == 0) {
      encoded->push_back(DOT);
      value = value / 2;
    } else {
      encoded->push_back(DASH);
      value = (value - 1) / 2;
    }
  }
  // Reverse the vector
  reverse(encoded->begin(), encoded->end());
}

// Converts a signal to an integer
int stoi(vector<MorseTick> *encoded) {
  int pos = 1;
  for (const MorseTick &tick : *encoded) {
    if (tick == DOT) {
      pos = pos * 2;
    } else if (tick == DASH) {
      pos = pos * 2 + 1;
    }
  }
  return pos;
}

// Obfuscates the value by shifting the value to the right in the lexicon
int obfuscate(int value, int delta) {
  value += delta;
  value %= LEXICON_LENGTH;
  return value;
}

int deobfuscate(int value, int delta) {
  value -= delta;
  // If the value is negative, then convert it to a positive integer, and fit it
  // into the range 0-LEXICON_LENGTH
  if (value < 0) {
    value = LEXICON_LENGTH - ((-1 * value) % LEXICON_LENGTH);
  }
  return value % LEXICON_LENGTH;
}

} // namespace morse
