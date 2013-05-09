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
        lda     KBSTL
        ora     KBEDG
        bne     @L1
        jsr     _kbhit          ; Check for char available
        tax                             ; Test result
        bra     _cgetc
@L1:
        ldx     #0
        and     #1
        beq     @L6
        lda     KBEDG           ; Pause button is pressed
        and     #$0c
        beq     @L3
        ora     KBSTL
@L2:
        bit     #$04
        beq     @L4                     ; Pause + Opt 1 = Reset
        bit     #$08
        beq     @L5                     ; Pause + Opt 2 = Flip
        lda     #'?'                    ; All buttons pressed
        rts
@L3:
        lda     KBSTL           ; Pause alone was the last placed button
        and     #$0c
        bne     @L2
        lda     #'P'                    ; Pause pressed
        rts
@L4:
        lda     #'R'                    ; Reset pressed
        rts
@L5:
        lda     #'F'                    ; Flip pressed
        rts
@L6:
        lda     KBEDG           ; No Pause pressed
        ora     KBSTL
        bit     #$08
        beq     @L8
        bit     #$04
        beq     @L7
        lda     #'3'                    ; opt 1 + opt 2 pressed
        rts
@L7:
        lda     #'1'                    ; opt 1 pressed
        rts
@L8:
        lda     #'2'                    ; opt 2 pressed
        rts
