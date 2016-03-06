;
; Ullrich von Bassewitz, 2003-03-07
; Maciej Witkowiak, 2003-05-02
;
; Setup arguments for main
;
; There is always either 1 or 3 arguments:
; <program name>,0
; or
; <program name>, <data file name>, <data disk name>, 0
; the 2nd case is when using DeskTop user drags an icon of a file and drops it
; on icon of your application
;

            .constructor initmainargs, 24
            .import __argc, __argv

            .include "const.inc"
            .include "geossym.inc"

.segment        "ONCE"

; Setup arguments for main

initmainargs:
; Setup a pointer to our argv vector

        lda #<argv
        sta __argv
        lda #>argv
        sta __argv+1

; Copy program name
        ldy #0
@fn_loop:
        lda dirEntryBuf+OFF_FNAME,y
.ifdef __GEOS_CBM__
        cmp #$a0
.else
        cmp #0
.endif
        beq @fn_end
        sta argv0,y
        iny
        cpy #16+1
        bne @fn_loop
@fn_end:
        lda #0
        sta argv0,y
        sta __argc+1

; Check if there are any more arguments
        lda dataFileName
        bne @threeargs
        ldx #0                  ; no dataFileName - NULL the 2nd argument
        stx argv+2
        stx argv+3
        inx                     ; there is only one argument
        bne @setargc
@threeargs:
        ldx #3                  ; there are three arguments
@setargc:
        stx __argc
        rts

.data

argv:
        .word argv0             ; Pointer to program name
        .word dataFileName      ; dataFileName or NULL if last one
        .word dataDiskName      ; dataDiskName
        .word $0000             ; last one must be NULL

.bss

argv0:
        .res 17                 ; Program name
