;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.99

            .export DB_get2lines
            .importzp ptr3,ptr4
            .import popax

DB_get2lines:
        sta ptr4                ; fetch line 2
        stx ptr4+1
        jsr popax
        sta ptr3                ; fetch line 1
        stx ptr3+1
        ldx #ptr3
        jsr checknull
        ldx #ptr4
        jmp checknull

checknull:
        lda $0,x
        ora $1,x
        bne cn_end
        lda #<nullstring
        sta $0,x
        lda #>nullstring
        sta $1,x
cn_end: rts

.rodata

nullstring:
        .byte 0
