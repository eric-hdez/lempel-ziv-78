#include "code.h"
#include "io.h"
#include "util.h"
#include "word.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int options; // used to parse the command line options

    // default i/o and verbose options
    int infile = STDIN_FILENO;
    int outfile = STDOUT_FILENO;
    bool verbose = false;

    while ((options = getopt(argc, argv, "vi:o:")) != -1) {
        switch (options) {
        case 'v': {
            verbose = true; // will print stats
            break;
        }
        case 'i': {
            if ((infile = open(optarg, O_RDONLY)) == -1) {
                fprintf(stderr, "%s : failed to open the input file.\n", optarg);
                close(outfile);
                exit(EXIT_FAILURE);
            }

            break;
        }
        case 'o': {
            if ((outfile = open(optarg, O_WRONLY | O_CREAT | O_TRUNC)) == -1) {
                fprintf(stderr, "%s : failed to open output file or file dne.\n", optarg);
                close(infile);
                exit(EXIT_FAILURE);
            }

            break;
        }
        default: {
            fprintf(stderr, "Program Usage : "
                            "%s -i <infile> -o <outfile> -v (optional stats)\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        }
    }

    FileHeader inheader = { 0, 0 };
    read_header(infile, &inheader);

    // not decodable if the magic does not match
    if (inheader.magic != MAGIC) {
        fprintf(stderr, "Bad magic number!\n");
        close(infile);
        close(outfile);
        exit(EXIT_FAILURE);
    }
    
    fchmod(outfile, inheader.protection);

    // decompression begins with the creation of an empty word table, and 
    // trackers for the current symbol, current code, and next code
    WordTable *table = wt_create();
    uint8_t curr_sym = 0;
    uint16_t curr_code = 0;
    uint16_t next_code = START_CODE;
    
    // read all of the compressed pairs of code and syms, writing words to the
    // output file as they are constructed in the word table. This ennds when
    // the max code is reached
    while ((read_pair(infile, &curr_code, &curr_sym, bit_len(next_code)))) {
        table[next_code] = word_append_sym(table[curr_code], curr_sym);
        write_word(outfile, table[next_code]);
        next_code += 1;

        if (next_code == MAX_CODE) {
            wt_reset(table);
            next_code = START_CODE;
        }
    }

    // flush any reminaing words out to output file
    flush_words(outfile);
    
    // take out the trash
    wt_delete(table);
    close(infile);
    close(outfile);

    //print stats if verbose enabled
    if (verbose) {
        double saving = 1.0 - ((float) to_bytes(total_bits) / (float) total_syms);
        fprintf(stderr, "Compressed file size: %" PRIu64 " bytes\n", to_bytes(total_bits));
        fprintf(stderr, "Uncompressed file size: %" PRIu64 " bytes\n", total_syms);
        fprintf(stderr, "Space saving: %.2f%%\n", saving);
    }

    return 0;
}
