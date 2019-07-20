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
  std::memcpy(key, key_arr, length);
};

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
