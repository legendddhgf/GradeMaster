#ifndef BITVEC_H
#define BITVEC_H

#include <stdint.h>

// There won't be a use for large bitvectors in this system anyway
// (use up to 2^16 bits maxsize)
typedef struct BitVec {
  uint8_t *vec; // holds the reference to the entire block of data
  uint16_t size; // current amount of bits used
  uint16_t maxbytecap; // the total amount of bytes currently allocated
  BitVec(uint16_t initbytecap); // init the bitvector with initial cap num bytes
  ~BitVec(); // deletes all data associated with this BitVec
  uint8_t push(uint8_t bit); // pushes bit (bit & 1) on end of vector
  uint8_t pop(void); // pops bit off the end of the vector
  uint8_t maskSpec(uint64_t mask); // sets the vector to reflect mask
  uint8_t set(uint16_t index); // sets bit (index) of vec
  uint8_t clr(uint16_t index); // clrs bit (index) of vec
  uint8_t get(uint16_t index); // gets bit (index) of vec
} BitVec_s;

#endif
