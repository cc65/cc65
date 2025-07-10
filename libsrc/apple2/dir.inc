.struct DIR
        FD                .word
        ENTRY_LENGTH      .byte
        ENTRIES_PER_BLOCK .byte
        FILE_COUNT        .word
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
