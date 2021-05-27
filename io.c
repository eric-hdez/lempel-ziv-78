#include "io.h"

#include "code.h"
#include "endian.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BITS 8 // size of a char in bits

uint64_t total_syms = 0;
uint64_t total_bits = 0;

static uint8_t symbuf[BLOCK] = { 0 };
static int symindex = 0;

static uint8_t bitbuf[BLOCK] = { 0 };
static int bitindex = 0;

// wrapper function for syscall read(), used to buffer BLOCK of data 
// from an input file
//
int read_bytes(int infile, uint8_t *buf, int to_read) {
    int reads = -1, total = 0;

    while ((total != to_read) && (reads != 0)) {
        if ((reads = read(infile, buf, to_read)) == -1) {
            fprintf(stderr, "failed to read bytes in file.\n");
            exit(EXIT_FAILURE);
        }

        total += reads;
        buf += reads;
    }

    return total;
}

// wrapper function for syscall write(), used to write buffered BLOCKs
// of data to an output file
//
int write_bytes(int outfile, uint8_t *buf, int to_write) {
    int writes = -1, total = 0;

    while ((total != to_write) && (writes != 0)) {
        if ((writes = write(outfile, buf, to_write)) == -1) {
            fprintf(stderr, "failed to write bytes to file.\n");
            exit(EXIT_FAILURE);
        }

        total += writes;
        buf += writes;
    }

    return total;
}

// reads the header info of an input file, swaps endianess depending on
// system, and stores the information
//
void read_header(int infile, FileHeader *header) {
    read_bytes(infile, (uint8_t *) header, sizeof(FileHeader));

    if (big_endian()) {
        header->magic = swap32(header->magic);
        header->protection = swap16(header->protection);
    }

    total_bits += (sizeof(FileHeader) * BITS);
}

// writes the header info to an output file, swaps endianess depending on
// system, and sotires the information
// 
void write_header(int outfile, FileHeader *header) {
    if (big_endian()) {
        header->magic = swap32(header->magic);
        header->protection = swap16(header->protection);
    }

    write_bytes(outfile, (uint8_t *) header, sizeof(FileHeader));
    total_bits += (sizeof(FileHeader) * BITS);
}

// buffers symbols from an input file and reads out those symbols to 'sym'
//
bool read_sym(int infile, uint8_t *sym) {
    static int end = -1;

    if (!symindex) {
        int reads = read_bytes(infile, symbuf, BLOCK);
        if (reads < BLOCK) {
            end = reads + 1;
        }
    }

    *sym = symbuf[symindex];
    symindex = (symindex + 1) % BLOCK;
    total_syms += 1;

    return symindex == end ? false : true;
}

// writes out code and sym pairs to an output file
//
void write_pair(int outfile, uint16_t code, uint8_t sym, int bitlen) {
    if (big_endian()) {
        swap16(code);
    }

    for (int bit = 0; bit < bitlen; bit++) {
        if (bitindex == BITS * BLOCK) {
            write_bytes(outfile, bitbuf, BLOCK);
            memset(bitbuf, 0, BLOCK);
            bitindex = 0;
        }

        if (code_get_bit(code, bit)) {
            set_bit(bitbuf, bitindex);
        }

        bitindex += 1;
    }

    for (int bit = 0; bit < BITS; bit++) {
        if (bitindex == BITS * BLOCK) {
            write_bytes(outfile, bitbuf, BLOCK);
            memset(bitbuf, 0, BLOCK);
            bitindex = 0;
        }

        if (sym_get_bit(sym, bit)) {
            set_bit(bitbuf, bitindex);
        }

        bitindex += 1;
    }

    total_bits += (bitlen + BITS);
}

// flushes any remaining pairs from the bit buffer
//
void flush_pairs(int outfile) {
    int bytes_to_write = to_bytes(bitindex);
    write_bytes(outfile, bitbuf, bytes_to_write);
}

// buffers code and sym pairs from an input file and then outputs the code
// and sym to their specified parameters, respectively
//
bool read_pair(int infile, uint16_t *code, uint8_t *sym, int bitlen) {
    *code = 0, *sym = 0;

    for (int bit = 0; bit < bitlen; bit++) {
        if (!bitindex) {
            read_bytes(infile, bitbuf, BLOCK);
        }

        *code |= get_bit(bitbuf, bitindex) << bit;
        bitindex = (bitindex + 1) % (BITS * BLOCK);
    }

    for (int bit = 0; bit < BITS; bit++) {
        if (!bitindex) {
            read_bytes(infile, bitbuf, BLOCK);
        }

        *sym |= get_bit(bitbuf, bitindex) << bit;
        bitindex = (bitindex + 1) % (BITS * BLOCK);
    }

    if (big_endian()) {
        *code = swap16(*code);
    }

    total_bits += (bitlen + BITS);

    return *code != STOP_CODE ? true : false;
}

// buffers word syms, writes them to an output file when full
//
void write_word(int outfile, Word *w) {
    for (uint32_t i = 0; i < w->len; i++) {
        if (symindex == BLOCK) {
            write_bytes(outfile, symbuf, BLOCK);
            symindex = 0;
        }

        symbuf[symindex] = w->syms[i];
        symindex += 1;
    }

    total_syms += w->len;
}

// flushes out any remaining words in the sym buffer
//
void flush_words(int outfile) {
    write_bytes(outfile, symbuf, symindex);
}
