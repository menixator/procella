#ifndef XXTEACIPHER_HPP
#define XXTEACIPHER_HPP

#include "ManagedString.h"
#include <stdint.h>

// Number of words(32bits) in a key
#define XXTEA_KEYLENGTH 4

// Both are in bytes
#define XXTEA_KEYSIZE 16
#define XXTEA_BLOCKSIZE 8

#define XXTEA_DELTA 0x9e3779b9
#define XXTEA_MX(e, p, y, z, sum, key)                                         \
  (((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4)) ^                                   \
   ((sum ^ y) + (key[(p & 3) ^ e] ^ z)))

class XXTEACipher {

private:
  uint32_t key[XXTEA_KEYLENGTH];

  // Write a character array to a key
  void write_key(const char *key_arr, const int length);

public:
  // Creates a cipher with a managed string as the key
  XXTEACipher(ManagedString key_str);

  // Creates a cipher with a character array and length
  XXTEACipher(const char *key_arr, int length);

  // encrypts an arbitrary sized uint32_t block
  void encrypt(uint32_t *data, const unsigned int length);
  
  // decrypts an arbitrary sized uint32_t block
  void decrypt(uint32_t *data, int length);
};

#endif
