.struct DIR
        FD                .word
        ENTRY_LENGTH      .byte
        ENTRIES_PER_BLOCK .byte
        CURRENT_ENTRY     .byte

        .union
                BYTES               .byte 512
                .struct CONTENT
                        PREV_BLOCK  .word
                        NEXT_BLOCK  .word
                        ENTRIES     .byte
                .endstruct
        .endunion
.endstruct
