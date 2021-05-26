# lempel-ziv-78

an implementation of the LZ78 compression algorithm for file compression.

## build

    $ make
    $ make all

## running

    ENCODE : 

    $ ./encode -i <input file> -o <output file>
        -i <input file>  : file being compressed
        -o <output file> : file to output compressed file
        -v               : verbose, optional statistics

    DECODE :

    $ ./decode -i <input file> -o <output file>
        -i <input file>  : file being decompressed
        -o <output file> : file to output decompressed file
        -v               : verbose, optional statistics

## cleaning

    $ make clean

## NOTE : 

    you know who you are if you shouldn't be looking at this project ;) . Like Pontius Pilatus, I will 
    wash my hands and rid myself of any responsibility for you.
