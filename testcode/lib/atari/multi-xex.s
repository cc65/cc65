; Multiple segment ATARI file format sample, using custom linker script.
;
; This sample defines a custom display-list screen with no code, writing all
; memory areas directly.
;
; See the linker script (multi-xex.cfg) for the definition of memory areas and
; segments.
;
; Compile with:
;    cl65 -tatari -Cmulti-xex.cfg multi-xex.s -o prog.xex

        .include        "atari.inc"

        .macpack        atari

; Default RUNAD is "start", export that:
        .export         start, show_load

; Loader
        .segment        "LOADER"
show_load:
        ldx     #0              ; channel 0
        lda     #<msg_load
        sta     ICBAL,x         ; address
        lda     #>msg_load
        sta     ICBAH,x
        lda     #$FF
        sta     ICBLL,x         ; length
        sta     ICBLH,x
        lda     #PUTREC
        sta     ICCOM,x
        jmp     CIOV

msg_load:
        .byte   "Loading....", ATEOL

; We load color values directly into registers
        .segment        "COLOR"

        .byte           $16     ; COLOR0
        .byte           $46     ; COLOR1
        .byte           $00     ; COLOR2
        .byte           $6A     ; COLOR3
        .byte           $82     ; COLOR4

; We load our display list over page 6
        .segment        "PAGE6"

display_list:
        .byte   DL_BLK8
        .byte   DL_BLK8
        .byte   DL_BLK8
        .byte   DL_BLK8
        .byte   DL_BLK8
        .byte   DL_BLK8
        .byte   DL_CHR20x8x2 | DL_LMS
        .word   screen_memory
        .byte   DL_CHR40x8x1
        .byte   DL_JVB
        .word   display_list

screen_memory:
        ; first text line: 20 bytes
        scrcode   "    HeLlO wOrLd!    "
        ; second text line, 40 bytes
        .byte    0, 0, 0, 0, 0, 0, 0, 0,70,71,70,71,70,71,70,71,70,71,70,71
        .byte   70,71,70,71,70,71,70,71,70,71,70,71, 0, 0, 0, 0, 0, 0, 0, 0

; We write directly to the display list pointer
        .segment        "SDLST"
        .word   display_list

; And we load our main program
        .code

.proc   start
        ; Jump forever
        jmp     start
.endproc

