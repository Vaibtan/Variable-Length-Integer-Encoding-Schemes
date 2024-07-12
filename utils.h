#include <bits/stdc++.h>
using namespace std;

using encoder_func = vector<uint8_t> (*)(const vector<uint64_t> &);
using decoder_func = void (*)(const uint8_t *, uint64_t *, size_t);

struct codec_descriptor {
    const char* name;
    encoder_func encoder;
    decoder_func decoder;
};

inline constexpr unsigned count_leading_zeros_64(uint64_t x) { return __builtin_clzll(x); }

inline constexpr unsigned count_trailing_zeros_32(uint32_t x) { return __builtin_ctz(x); }

inline uint64_t unaligned_load_u64(const uint8_t* p) {
  uint64_t x;
  std::memcpy(&x, p, 8);
  return x;
}

inline uint16_t unaligned_load_u16(const uint8_t* p) {
  uint16_t x;
  std::memcpy(&x, p, 2);
  return x;
}

#define LIKELY(X) __builtin_expect(!!(X), 1)