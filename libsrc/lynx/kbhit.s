;
; Karri Kaksonen, Harry Dodgson 2006-01-06
;
; unsigned char kbhit (void);
;

        .export         _kbhit
        .export         KBEDG
        .export         KBSTL
        .import         return1

; --------------------------------------------------------------------------
; The Atari Lynx has a very small keyboard - only 3 keys
; Opt1, Opt2 and Pause.
; But the designers have decided that pressing Pause and Opt1 at the
; same time means Restart and pressing Pause and Opt2 means Flip screen.

; For "easter egg" use I have also included all three keys pressed '?'
; and Opt1 + Opt2 pressed '3'.
; So the keyboard returns '1', '2', '3', 'P', 'R', 'F' or '?'.

        .data
KBTMP:          .byte   0
KBPRV:          .byte   0
KBEDG:          .byte   0
KBSTL:          .byte   0
KBDEB:          .byte   0
KBNPR:          .byte   0

        .code
_kbhit:
        lda     $FCB0           ; Read the Opt buttons
        and     #$0c
        sta     KBTMP
        lda     $FCB1           ; Read Pause
        and     #1
        ora     KBTMP           ; 0000210P
        tax
        and     KBPRV
        sta     KBSTL           ; for multibutton
        txa
        and     KBDEB
        sta     KBEDG           ; for just depressed
        txa
        and     KBNPR
        sta     KBDEB           ; for debouncing
        txa
        eor     #$ff
        sta     KBNPR           ; inverted previous ones pressed
        stx     KBPRV
        lda     KBEDG
        beq     @L1
        jmp     return1         ; Key hit

@L1:    tax                     ; No new keys hit
        rts
