;
; Fixed point cosine/sine functions.
;
; int __fastcall__ _sin (unsigned x);
; int __fastcall__ _cos (unsigned x);
;
; Returns the cosine/sine for the given argument as angular degree.
; Valid argument range is 0..360 for both functions. They will return
; garbage if the argument is not in a valid range. Result is in 8.8 fixed
; point format, so $100 is 1.0 and $FF00 is -1.0.
;
;
; Ullrich von Bassewitz, 2009-10-29
;

        .export         __cos, __sin


; ---------------------------------------------------------------------------
; Sinus table covering values from 0..86° as 0.8 fixed point values. Values
; for 87..90° are actually 1.0 (= $100), will therefore not fit in the table
; and are covered specially in the code below.

.rodata

_sintab:
        .byte   $00, $04, $09, $0D, $12, $16, $1B, $1F, $24, $28
        .byte   $2C, $31, $35, $3A, $3E, $42, $47, $4B, $4F, $53
        .byte   $58, $5C, $60, $64, $68, $6C, $70, $74, $78, $7C
        .byte   $80, $84, $88, $8B, $8F, $93, $96, $9A, $9E, $A1
        .byte   $A5, $A8, $AB, $AF, $B2, $B5, $B8, $BB, $BE, $C1
        .byte   $C4, $C7, $CA, $CC, $CF, $D2, $D4, $D7, $D9, $DB
        .byte   $DE, $E0, $E2, $E4, $E6, $E8, $EA, $EC, $ED, $EF
        .byte   $F1, $F2, $F3, $F5, $F6, $F7, $F8, $F9, $FA, $FB
        .byte   $FC, $FD, $FE, $FE, $FF, $FF, $FF



; ---------------------------------------------------------------------------
; Cosine function. Is actually implemented as _cos(x) = _sin(x+90)

.code

__cos:

; _cos(x) = _sin(x+90)

        clc
        adc     #90
        bcc     @L1
        inx

; If x is now larger than 360, we need to subtract 360.

@L1:    cpx     #>360
        bne     @L2
        cmp     #<360
@L2:    bcc     __sin

        sbc     #<360
        bcs     @L3
        dex
@L3:    dex

; ---------------------------------------------------------------------------
; Sine function. Uses
;
;       table lookup            for 0..89°
;       _sin(x) = _sin(180-x)   for 90°..179°
;       _sin(x) = -_sin(x-180)  for 180..360°
;
; Plus special handling for the values missing in the table.

__sin:

; If the high byte is non zero, argument is > 255

        cpx     #0
        bne     L3
        cmp     #180
        bcs     L4

; 0..179°

        cmp     #90
        bcc     L1

; 90..179°. Value is identical to _sin(180-val). Carry is set on entry.
;
;       180-val := -val + 180.
; With
;       -val := (val ^ $FF) + 1
; we get
;       180-val = (val ^ $FF) + 1 + 180
; Since carry is set, we can drop the "+ 1".
;

        eor     #$FF
        adc     #180            ; 180-val

; 0..89°. Values for 87..90° are actually 1.0. Since this format doesn't fit
; into the table, we have to check for it manually.

L1:     cmp     #87
        bcc     L2

; The value is 1.0

        ldx     #>(1 << 8)
        lda     #<(1 << 8)
        rts

; 0..86°. Read the value from the table.

L2:     tay
        ldx     #0
        lda     _sintab,y
        rts

; 180..360°. _sin(x) = -_sin(x-180). Since the argument is in range 0..180
; after the subtraction, we don't need to handle the high byte.

L3:     sec
L4:     sbc     #180

        cmp     #90
        bcc     L5

; 270..360°. Value is identical to -_sin(180-val). Carry is set on entry.
;
;       180-val := -val + 180.
; With
;       -val := (val ^ $FF) + 1
; we get
;       180-val = (val ^ $FF) + 1 + 180
; Since carry is set, we can drop the "+ 1".
;

        eor     #$FF
        adc     #180            ; 180-val

; 180..269°. Values for 267..269° are actually -1.0. Since this format doesn't
; fit into the table, we have to check for it manually.

L5:     ldx     #$FF
        cmp     #87
        bcc     L6

; The value is -1.0

        lda     #<(-1 << 8)
        rts

; 180..266°. Read the value from the table. Carry is clear on entry.

L6:     tay
        txa                     ; A = $FF
        eor     _sintab,y
        adc     #1
        bcc     L7
        inx
L7:     rts
