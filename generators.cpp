/**
 * @file generators.cpp
 * @brief Реализация трёх генераторов псевдослучайных чисел
 *
 * Содержит определения функций, объявленных в generators.hpp
 */

#include "generators.hpp"

uint32_t lcg_state;
void lcg_seed(uint32_t seed) { lcg_state = seed; }
uint32_t next_lcg() {
  lcg_state = 1664525 * lcg_state + 1013904223;
  uint32_t out = lcg_state;
  out ^= out >> 16;
  out ^= out << 13;
  out ^= out >> 5;
  return out;
}

uint32_t lfib_buffer[17];
int lfib_index;
uint32_t lfib_carry;
void lagfib_carry_seed(uint32_t seed) {
  uint32_t s = seed;
  for (int i = 0; i < 17; ++i) {
    s = s * 1103515245 + 12345;
    lfib_buffer[i] = s;
  }
  lfib_index = 0;
  lfib_carry = 0;
}
uint32_t next_lagfib_carry() {
  const int lag1 = 17;
  const int lag2 = 5;
  int prev1 = (lfib_index + lag1 - 1) % lag1;
  int prev2 = (lfib_index + lag1 - lag2) % lag1;

  uint32_t a = lfib_buffer[prev1];
  uint32_t b = lfib_buffer[prev2];
  uint32_t new_val = (a + b) ^ (a >> 3);
  new_val += lfib_carry;

  lfib_carry = ((a + b) >> 28) ^ (new_val >> 28);
  lfib_carry &= 0xF;

  new_val ^= new_val >> 16;
  new_val ^= new_val << 13;
  new_val ^= new_val >> 5;

  lfib_buffer[lfib_index] = new_val;
  lfib_index = (lfib_index + 1) % lag1;
  return new_val;
}

uint32_t rot_state[3];
uint32_t rot_left(uint32_t x, int bits) {
  return (x << bits) | (x >> (32 - bits));
}
uint32_t rot_right(uint32_t x, int bits) {
  return (x >> bits) | (x << (32 - bits));
}
void rotmix_seed(uint32_t seed) {
  uint32_t s = seed;
  for (int i = 0; i < 3; ++i) {
    s = s * 1664525 + 1013904223;
    rot_state[i] = s;
  }
}
uint32_t next_rotmix() {
  uint32_t a = rot_state[0];
  uint32_t b = rot_state[1];
  uint32_t c = rot_state[2];

  uint32_t new_a = rot_left(a, 7) ^ rot_right(b, 13) ^ c;
  uint32_t new_b = rot_left(b, 11) ^ rot_right(c, 17) ^ a;
  uint32_t new_c = rot_left(c, 19) ^ rot_right(a, 23) ^ b;

  rot_state[0] = new_a;
  rot_state[1] = new_b;
  rot_state[2] = new_c;

  uint32_t out = new_a ^ new_b ^ new_c;
  out ^= out >> 16;
  out *= 0x85EBCA77;
  out ^= out >> 13;
  return out;
}