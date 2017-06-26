#include "BitVec.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define BYTESIZE 8

// initializes the bitvector with initial cap as the initial number of bits
BitVec_s::BitVec(uint16_t initbytecap = 4) {
  size = 0;
  maxbytecap = initbytecap;
  // no one cares that it is potentially initialized to garbage because size = 0
  vec = (uint8_t *) malloc(maxbytecap * sizeof(uint8_t));
}

// deletes all data associated with this BitVec
BitVec_s::~BitVec() {
  free(vec);
}

// pushes bit (bit & 1) on end of vector
uint8_t BitVec_s::push(uint8_t bit) {
  if (size >= maxbytecap * BYTESIZE) {
    maxbytecap *= 2;
    vec = (uint8_t *) realloc(vec, maxbytecap * sizeof(uint8_t));
    if (vec == NULL) {
      fprintf(stderr, "Unable to expand BitVector\n");
      exit(1);
    }
  }
  // size is always the pointer to the next location to store
  *(vec + size / BYTESIZE) |= ((bit & 1) << (size & 0x7));
  return ++size;
}

// pops bit off the end of the vector
uint8_t BitVec_s::pop(void) {
  if (size == 0) { // TODO: should the program die here?
    return ~1;
  }
  return --size;
}

// sets bit (index) of vec
uint8_t BitVec_s::set(uint16_t index) {
  if (index >= size) {
    return ~1;
  }
  *(vec + index / BYTESIZE) |= (1 << (index & 0x7));
  return 1;
}

// clrs bit (index) of vec
uint8_t BitVec_s::clr(uint16_t index) {
  if (index >= size) {
    return ~1;
  }
  *(vec + index / BYTESIZE) &= ~(1 << (index & 0x7));
  return 1;
}

// gets bit (index) of vec
uint8_t BitVec_s::get(uint16_t index) {
  if (index >= size) {
    return ~1;
  }
  return *(vec + index / BYTESIZE) & (1 << (index & 0x7)) ? 1 : 0;
}

