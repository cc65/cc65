;
; Text mode character mouse callbacks for the Ataris
;
; Christian Groessler, 03.01.2014
;
; derived from Apple2 version by
; Oliver Schmidt, 22.09.2005
;
; All functions in this module should be interrupt safe, because they may
; be called from an interrupt handler
;

        .export         _mouse_txt_callbacks
        .importzp       tmp4
        .import         mul40,loc_tmp
        .importzp       mouse_txt_char          ; screen code of mouse cursor

        .include        "atari.inc"

; ------------------------------------------------------------------------

        .bss

backup: .res    1
visible:.res    1

; ------------------------------------------------------------------------

        .segment        "EXTZP" : zeropage
scrptr: .res    2

; ------------------------------------------------------------------------


        .rodata

        ; Callback structure
_mouse_txt_callbacks:
        .addr   hide
        .addr   show
        .addr   prep
        .addr   draw
        .addr   movex
        .addr   movey

; ------------------------------------------------------------------------

        .data

; setcursor

getcursor:
column: ldy     #$00            ; Patched at runtime
        lda     (scrptr),y
        rts

setcursor:
column2:ldy     #$00            ; Patched at runtime
        sta     (scrptr),y
        rts

; ------------------------------------------------------------------------

        .code

done:
        rts

; Hide the mouse cursor.
hide:
        dec     visible

prep:
        jsr     getcursor       ; Get character at cursor position
        cmp     #mouse_txt_char ; "mouse" character
        bne     overwr          ; no, probably program has overwritten it
        lda     backup          ; 
        jmp     setcursor       ; Draw character
overwr: sta     backup
        rts

; Show the mouse cursor.
show:
        inc     visible

draw:
        lda     visible
        beq     done
        jsr     getcursor       ; Cursor visible at current position?
        sta     backup          ; Save character at cursor position
        lda     #mouse_txt_char
        jmp     setcursor       ; Draw cursor


; Move the mouse cursor x position to the value in A/X.
movex:
        cpx     #1
        ror     a
        lsr     a               ; convert to character position
        lsr     a
        sta     column+1
        sta     column2+1
        rts

; Move the mouse cursor y position to the value in A/X.
movey:
        tax
        ldy     tmp4            ; mul40 uses tmp4
        lda     loc_tmp         ; and this local variable
        pha
        txa                     ; get parameter back
        lsr     a               ; convert y position to character line
        lsr     a
        lsr     a
        jsr     mul40
        clc
        adc     SAVMSC
        sta     scrptr
        txa
        adc     SAVMSC+1
        sta     scrptr+1
        pla
        sta     loc_tmp
        sty     tmp4
        rts
