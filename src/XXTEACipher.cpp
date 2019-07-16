#include "XXTEACipher.hpp"

// Constructs a cipher with a ManagedString
XXTEACipher::XXTEACipher(ManagedString key_str) {
  write_key(key_str.toCharArray(), key_str.length());
}

// Constructs a cipher with a character array and a length
XXTEACipher::XXTEACipher(const char *key_arr, int length) {
  write_key(key_arr, length);
}

// Converts a character array to a sequence of words and saves it as the key
void XXTEACipher::write_key(const char *key_arr, const int length) {
  // Add it to the key

  // Index to write to next
  short int to_write = 0;
  uint32_t word_buffer = 0;

  // Assures that all 4 key entries are written to.
  for (int index = 0; index < XXTEA_KEYSIZE; index++) {
    // A char is 8 bits.
    // The buffer needs to be filled 4 times to make up a 32 bit key
    if ((index + 1) % 4 == 0) {
      // Every 4th iteration, copy the buffer over
      key[to_write] = word_buffer;
      word_buffer = 0;
      to_write++;
    } else {
      // shift left a byte
      word_buffer <<= 8;
      if (index < length) {
        word_buffer += (uint32_t)key_arr[index];
      }
    }
  }
}

// Encrypts an arbitrary number of uint32_t chunks
void XXTEACipher::encrypt(uint32_t *data, const unsigned int length) {
  uint32_t y, z, sum;
  unsigned p, rounds, e;

  rounds = 6 + 52 / length;
  sum = 0;

  // z is set to last uint32_t
  z = data[length - 1];

  do {
    // Add delta to sum every round
    sum += XXTEA_DELTA;

    e = (sum >> 2) & 3;

    // loop over every uint32_t except the last
    for (p = 0; p < length - 1; p++) {
      // y is set to the next uint32_t
      y = data[p + 1];

      // current uint32_t is set to the result of XTEA_MX
      z = data[p] += XXTEA_MX(e, p, y, z, sum, key);
    }

    // reset y and z
    y = data[0];
    z = data[length - 1] += XXTEA_MX(e, p, y, z, sum, key);

  } while (--rounds);
}

// Decrypts an arbitrary number of uint32_t chunks
void XXTEACipher::decrypt(uint32_t *data, int length) {
  uint32_t y, z, sum;
  unsigned p, rounds, e;

  rounds = 6 + 52 / length;
  sum = rounds * XXTEA_DELTA;
  y = data[0];
  do {
    e = (sum >> 2) & 3;
    for (p = length - 1; p > 0; p--) {
      z = data[p - 1];
      y = data[p] -= XXTEA_MX(e, p, y, z, sum, key);
    }
    z = data[length - 1];
    y = data[0] -= XXTEA_MX(e, p, y, z, sum, key);
    sum -= XXTEA_DELTA;
  } while (--rounds);
}
