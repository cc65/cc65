    .word __MAIN         ; 2 byte BLAOD address
    .word __END - __MAIN ; 2 byte BLOAD size
    .org  __MAIN         ; .org must come after header else offsets are wrong

