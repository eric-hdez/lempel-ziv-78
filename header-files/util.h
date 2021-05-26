#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdint.h>

static inline uint8_t get_bit(uint8_t buf[], uint32_t bit) {
    return (buf[bit / 8] >> (bit % 8)) & 1; 
}

static inline void set_bit(uint8_t buf[], uint32_t bit) {
    buf[bit / 8] |= (1 << (bit % 8));
}

static inline void clr_bit(uint8_t buf[], uint32_t bit) {
    buf[bit / 8] &= ~(1 << (bit % 8));
}

static inline uint8_t sym_get_bit(uint8_t sym, uint32_t bit) {
    return (sym >> (bit % 8)) & 1;
}

static inline uint16_t code_get_bit(uint16_t code, uint32_t bit) {
    return (code >> (bit % 16)) & 1;
}

static inline int bits_to_bytes(int bits) {
    return !(bits % 8) ? (bits / 8) : (bits / 8) + 1;
}

static inline int bit_len(uint16_t code) {
    int bitlen = 0;

    while (code > 0) {
        bitlen++;
        code = code >> 1;
    }

    return bitlen;
}

#endif
