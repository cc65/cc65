;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001, 23.12.2002

; void clrscr (void);

            .export _clrscr
            .import fixcursor
            .importzp cursor_c, cursor_r

            .include "jumptab.inc"
            .include "geossym.inc"
            .include "const.inc"

_clrscr:
        lda curPattern          ; save current pattern
        pha
        lda #0                  ; set pattern to clear
        jsr SetPattern
        ldx #0
        stx r3L
        stx r3H
        stx r2L
        stx cursor_c
        stx cursor_r
        jsr fixcursor           ; home cursor
.ifdef __GEOS_CBM__
        lda #199
        sta r2H
        lda graphMode
        bpl L40
        lda #>639               ; 80 columns
        ldx #<639
        bne L99
L40:    lda #>319               ; 40 columns
        ldx #<319
L99:
.else
        lda #191
        sta r2H
        lda #>559
        ldx #<559
.endif
        sta r4H
        stx r4L
        jsr Rectangle
        pla
        jmp SetPattern          ; restore pattern
