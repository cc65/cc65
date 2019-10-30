;
; Karri Kaksonen, Harry Dodgson 2006-01-07
;
; char cgetc (void);
;

        .export         _cgetc
        .import         _kbhit
        .import         KBEDG
        .import         KBSTL

; --------------------------------------------------------------------------
; The Atari Lynx has a very small keyboard - only 3 keys
; Opt1, Opt2 and Pause.
; But the designers have decided that pressing Pause and Opt1 at the
; same time means Restart and pressing Pause and Opt2 means Flip screen.

; For "easter egg" use I have also included all three keys pressed '?'
; and Opt1 + Opt2 pressed '3'.
; So the keyboard returns '1', '2', '3', 'P', 'R', 'F' or '?'.

_cgetc:   
        jsr     _kbhit          ; Check for char available
        beq     _cgetc
        ora     KBSTL
        ldx     #0
        and     #1
        beq     @L5
        lda     KBEDG           ; Pause button is pressed
        and     #$0c
        beq     @L2
        ora     KBSTL
@L1:
        bit     #$04
        beq     @L3                     ; Pause + Opt 1 = Reset
        bit     #$08
        beq     @L4                     ; Pause + Opt 2 = Flip
        lda     #'?'                    ; All buttons pressed
        bra     reset_and_exit
@L2:
        lda     KBSTL           ; Pause alone was the last placed button
        and     #$0c
        bne     @L1
        lda     #'P'                    ; Pause pressed
        bra     reset_and_exit
@L3:
        lda     #'R'                    ; Reset pressed
        bra     reset_and_exit
@L4:
        lda     #'F'                    ; Flip pressed
        bra     reset_and_exit
@L5:
        lda     KBEDG           ; No Pause pressed
        ora     KBSTL
        bit     #$08
        beq     @L7
        bit     #$04
        beq     @L6
        lda     #'3'                    ; opt 1 + opt 2 pressed
        bra     reset_and_exit
@L6:
        lda     #'1'                    ; opt 1 pressed
        bra reset_and_exit
@L7:
        lda     #'2'                    ; opt 2 pressed

reset_and_exit:
        stz     KBEDG
        rts
