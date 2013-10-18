; This file defines the chunk header for the main program load chunk

        .export         __MAINCHNKHDR__: absolute = 1
        .import         __RAM_START__, __BSS_LOAD__

.segment        "MAINHDR"

        .word   __RAM_START__
        .word   __BSS_LOAD__ - 1
