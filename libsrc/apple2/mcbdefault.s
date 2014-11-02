;
; Default mouse callbacks for the Apple II
;
; Oliver Schmidt, 22.09.2005
;
; All functions in this module should be interrupt safe, because they may
; be called from an interrupt handler
;

        .export         _mouse_def_callbacks
        
        .include        "apple2.inc"

; ------------------------------------------------------------------------

        .bss
        
backup: .res    1
visible:.res    1

; ------------------------------------------------------------------------

        .rodata

        ; Callback structure
_mouse_def_callbacks:
        .addr   hide
        .addr   show
        .addr   prep
        .addr   draw
        .addr   movex
        .addr   movey

; ------------------------------------------------------------------------

        .data

        .ifdef  __APPLE2ENH__
cursor = 'B'                    ; MouseText character
        .else
cursor = '+' | $40              ; Flashing crosshair
        .endif

getcursor:
        .ifdef  __APPLE2ENH__
        bit     RD80VID         ; In 80 column mode?
        bpl     column          ; No, skip bank switching
switch: bit     LOWSCR          ; Patched at runtime
        .endif
column: ldx     #$00            ; Patched at runtime
getscr: lda     $0400,x         ; Patched at runtime
        cmp     #cursor
        rts

setcursor:
        lda     #cursor
setscr: sta     $0400,x         ; Patched at runtime
        .ifdef  __APPLE2ENH__
        bit     LOWSCR          ; Doesn't hurt in 40 column mode
        .endif
        rts

; ------------------------------------------------------------------------

        .code

done:
        .ifdef  __APPLE2ENH__
        bit     LOWSCR          ; Doesn't hurt in 40 column mode
        .endif
return: rts

; Hide the mouse cursor.
hide:
        dec     visible
        ; Fall through

; Prepare to move the mouse cursor.
prep:
        jsr     getcursor       ; Cursor visible at current position?
        bne     done            ; No, we're done
        lda     backup          ; Get character at cursor position
        jmp     setscr          ; Draw character

; Show the mouse cursor.
show:
        inc     visible
        ; Fall through

; Draw the mouse cursor.
draw:
        lda     visible
        beq     return
        jsr     getcursor       ; Cursor visible at current position?
        beq     done            ; Yes, we're done
        sta     backup          ; Save character at cursor position
        jmp     setcursor       ; Draw cursor

; Move the mouse cursor x position to the value in A/X.
movex:
        dex                     ; Is position [256..279]?
        bmi     :+              ; No, start with column 0
        clc
        adc     #$0100 .MOD 7   ; Bias position
        ldx     #$0100 / 7 - 1  ; Bias column
:       sec
:       sbc     #7              ; 280 positions / 40 columns
        inx
        bcs     :-
        stx     column+1
        .ifdef  __APPLE2ENH__
        adc     #7 / 2          ; Left or right half of 40-col column?
        ldx     #<LOWSCR        ; Columns 1,3,5..79
        bcs     :+
        .assert LOWSCR + 1 = HISCR, error
        inx                     ; Columns 0,2,4..78
:       stx     switch+1
        .endif
        rts

; Move the mouse cursor y position to the value in A/X.
movey:
        tax                     ; ABCDExxx
        lsr                     ; 0ABCDExx
        lsr                     ; 00ABCDEx
        lsr                     ; 000ABCDE
        sta     getscr+1
        lsr                     ; 0000ABCD
        and     #%00000011      ; 000000CD
        ora     #>$0400         ; 000001CD
        sta     getscr+2
        sta     setscr+2
        txa                     ; ABCDExxx
        ror                     ; EABCDExx
        and     #%11100000      ; EAB00000
        ora     getscr+1        ; EABABCDE
        and     #%11111000      ; EABAB000
        sta     getscr+1
        sta     setscr+1
        rts
