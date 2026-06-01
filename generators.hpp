#ifndef GENERATORS_HPP
#define GENERATORS_HPP

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

extern uint32_t lcg_state;
void lcg_seed(uint32_t seed);
uint32_t next_lcg();

extern uint32_t lfib_buffer[17];
extern int lfib_index;
extern uint32_t lfib_carry;
void lagfib_carry_seed(uint32_t seed);
uint32_t next_lagfib_carry();

extern uint32_t rot_state[3];
uint32_t rot_left(uint32_t x, int bits);
uint32_t rot_right(uint32_t x, int bits);
void rotmix_seed(uint32_t seed);
uint32_t next_rotmix();

#endif