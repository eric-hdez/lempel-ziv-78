#include "word.h"

#include "code.h"

#include <stdio.h>
#include <stdlib.h>

// constructor for a Word ADT
//
Word *word_create(uint8_t *syms, uint32_t len) {
    Word *w = malloc(sizeof(Word));
    if (!w) {
        return NULL;
    }
    
    w->len = len;

    w->syms = syms ? calloc(len, sizeof(uint8_t)) : NULL;
    if (w->syms) {
        for (uint32_t sym = 0; sym < len; sym++) {
            w->syms[sym] = syms[sym];
        }
    }

    return w;
}

// creates a new word using the blueprint of the word passed in,
// appends the sym that is specified in the parameters
//
Word *word_append_sym(Word *w, uint8_t sym) {
    Word *new_w = word_create(NULL, w->len + 1);
    
    new_w->syms = calloc(new_w->len, sizeof(uint8_t));
    for (uint32_t symb = 0; symb < w->len; symb++) {
        new_w->syms[symb] = w->syms[symb];
    }

    new_w->syms[w->len] = sym; // appending the new symbol

    return new_w;
}

// destructor function for a Word ADT
// 
void word_delete(Word *w) {
    if (w) {
        if (w->syms) {
            free(w->syms);
            w->syms = NULL; // set syms array value NULL
        }

        free(w);
    }
}

// constructor for a word table ADT, an abstraction of a word
//
WordTable *wt_create(void) {
    WordTable *wt = calloc(MAX_CODE, sizeof(WordTable));
    if (!wt) {
        return NULL;
    }
        
    wt[EMPTY_CODE] = calloc(1, sizeof(Word));
    wt[EMPTY_CODE]->syms = NULL;
    wt[EMPTY_CODE]->len = 0;
    if (!wt[EMPTY_CODE]) {
        return NULL;
    }

    return wt;
}

// resets a wordtable to contain only the default STOP_CODE and
// EMPTY_CODE
//
void wt_reset(WordTable *wt) {
    if (wt) {
        for (int i = START_CODE; i < MAX_CODE; i++) {
            word_delete(wt[i]);
            wt[i] = NULL;
        }
    }
}

// destructor for the entire WordTable ADT
//
void wt_delete(WordTable *wt) {
    if (wt) {
        for (int i = STOP_CODE; i < MAX_CODE; i++) {
            word_delete(wt[i]); // delete each word of entire table
            wt[i] = NULL;
        }

        free(wt);
    }
}
