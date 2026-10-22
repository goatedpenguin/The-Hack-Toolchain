#include "bitvector.h"
#include <stdio.h>
#include <stdlib.h>

extern BitVector* bv;

void initBitVector(size_t numBits) {
    size_t bytes = (numBits + 7) / 8;
    bv = (BitVector*)malloc(sizeof(BitVector));
    bv->data = (unsigned char*)calloc(bytes, sizeof(unsigned char));
    bv->numBits = numBits;
    bv->size = bytes;
}

void setBit(size_t index) {
    if (index >= bv->numBits)
        return;
    size_t byteIdx = index / 8;
    size_t bitPos = index % 8;
    unsigned char mask = (unsigned char)1u << bitPos;
    bv->data[byteIdx] |= mask;
    // It's possible to put 1 << bitPos where mask is, but its left
    // there for learning purposes... Done on the last line
}

// static void clearBit(size_t index) {
// function not needed but implemeneted just to learn
//     if (index >= bv->numBits)
//         return;
//     size_t byteIdx = index / 8;
//     size_t bitPos = index % 8;
//     // set to all 0s except one 1 in the mask, shift bitPos times and then
//     // negate everything such that everything is 0xff except the 1 we shifted at
//     // first
//     bv->data[byteIdx] &= ~((unsigned char)1u << bitPos);
// }

static int getBit(size_t index) {
    if (index >= bv->numBits)
        return -1;  
    size_t byteIdx = index / 8;
    size_t bitPos = index % 8;
    // shift our target bits to the right by bitPos times, then AND it with one to preserve our target bit
    return (bv->data[byteIdx] >> bitPos) & 1;
}

size_t getNextFreeSlot() {
    for (size_t i = 16; i < bv->numBits; i++) {
        int bit = getBit(i);
        if (bit < 0) return (size_t)-1;   
        if (bit == 0) return i;           
    }
    return (size_t)-1;
}

void freeBitVector() {
    free(bv->data);
    free(bv);
    bv = NULL;
}
