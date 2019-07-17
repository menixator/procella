#include "utils.hpp"

namespace utils {
uint8_t parity(uint8_t value) {
  value ^= value >> 4;
  value ^= value >> 2;
  value ^= value >> 1;
  return (~value) & 1;
};
}; // namespace utils
