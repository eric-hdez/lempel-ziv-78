#include "code.h"
#include "io.h"
#include "trie.h"
#include "util.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int options;

    // default i/o and verbose
    int infile = STDIN_FILENO;
    int outfile = STDOUT_FILENO;
    bool verbose = false;

    while ((options = getopt(argc, argv, "vi:o:")) != -1) {
        switch (options) {
        case 'v': {
            verbose = true; // will print statistics
            break;
        }
        case 'i': {
            if ((infile = open(optarg, O_RDONLY)) == -1) {
                fprintf(stderr, "%s : failed to open input file.\n", optarg);
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

    // grab the input file permissions and write out the appropriate
    // header
    struct stat statbuff;
    fstat(infile, &statbuff);

    FileHeader outheader = { 0, 0 };
    outheader.magic = MAGIC;
    outheader.protection = statbuff.st_mode;

    fchmod(outfile, outheader.protection);
    write_header(outfile, &outheader);

    // The encoding begins with the creation of a trie and nodes 
    // that track the current and previous nodes in the trie
    TrieNode *root = trie_create();
    TrieNode *curr_node = root, *prev_node = NULL;

    uint8_t curr_sym = 0, prev_sym = 0;
    int next_code = START_CODE;

    // read each symbol from the input file, writing sym and code pairs
    // if we have not encountered sym in our trie. Reset the trie if 
    // we have reached the max code
    while (read_sym(infile, &curr_sym)) {
        TrieNode *next_node = trie_step(curr_node, curr_sym);

        if (next_node) {
            prev_node = curr_node;
            curr_node = next_node;
        } else {
            write_pair(outfile, curr_node->code, curr_sym, bit_len(next_code));
            curr_node->children[curr_sym] = trie_node_create(next_code);
            curr_node = root;
            next_code = next_code + 1;
        }

        if (next_code == MAX_CODE) {
            trie_reset(root);
            curr_node = root;
            next_code = START_CODE;
        }

        prev_sym = curr_sym;
    }
    
    if (curr_node != root) {
        write_pair(outfile, prev_node->code, prev_sym, bit_len(next_code));
        next_code = (next_code + 1) % MAX_CODE;
    }
    
    // write the stop code to denote the end of compression, flush any
    // remaining pairs to the output file
    write_pair(outfile, STOP_CODE, 0, bit_len(next_code));
    flush_pairs(outfile);
    
    // take out the trash
    trie_delete(root);
    close(infile);
    close(outfile);

    // print stats if verbose enabled
    if (verbose) {
        double saving = 1.0 - ((float) to_bytes(total_bits) / (float) total_syms);
        fprintf(stderr, "Compressed file size: %" PRIu64 " bytes\n", to_bytes(total_bits));
        fprintf(stderr, "Uncompressed file size: %" PRIu64 " bytes\n", total_syms);
        fprintf(stderr, "Space saving: %.2f%%\n", saving);
    }

    return 0;
}
