#ifndef __WORD_H__
#define __WORD_H__

#include <stdint.h>

typedef struct Word {
    uint8_t *syms;
    uint32_t len;
} Word;

typedef Word *WordTable;

Word *word_create(uint8_t *syms, uint32_t len);

Word *word_append_sym(Word *w, uint8_t sym);

void word_delete(Word *w);

WordTable *wt_create(void);

void wt_reset(WordTable *wt);

void wt_delete(WordTable *wt);

#endif
