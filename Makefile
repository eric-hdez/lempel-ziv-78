CC = clang
CFLAGS = -Werror -Wextra -Wpedantic -Wall -g

BINARIES = encode decode

OFILES =  trie.o io.o word.o

all: $(BINARIES)

encode: encode.o $(OFILES)
	$(CC) -o $@ $@.o $(OFILES)

decode: decode.o $(OFILES)
	$(CC) -o $@ $@.o $(OFILES)

trie.o: trie.c
	$(CC) $(CFLAGS) -c $<

io.o: io.c
	$(CC) $(CFLAGS) -c $<

word.o: word.c
	$(CC) $(CFLAGS) -c $<

format: 
	clang-format -i -style=file *.[ch]

clean:
	rm -rf encode.o decode.o $(BINARIES) $(OFILES)
