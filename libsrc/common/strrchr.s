;
; Ullrich von Bassewitz, 31.05.1998
; Christian Krueger: 2013-Aug-01, optimization
;
; char* strrchr (const char* s, int c);
;

        .export         _strrchr
        .import         popax
        .importzp       ptr1, tmp1, tmp2

_strrchr:
        sta tmp1        ; Save c
        jsr popax       ; get s
        tay             ; low byte to y
        stx ptr1+1
        ldx #0          ; default function result is NULL, X is high byte...
        stx tmp2        ; tmp2 is low-byte
        stx ptr1        ; low-byte of source string is in Y, so clear real one... 
   
testChar:
        lda (ptr1),y    ; get char
        beq finished    ; jump if end of string
        cmp tmp1        ; found?
        bne nextChar    ; jump if no

charFound:
        sty tmp2        ; y has low byte of location, save it
        ldx ptr1+1      ; x holds high-byte of result

nextChar:
        iny
        bne testChar
        inc ptr1+1
        bne testChar    ; here like bra...

; return the pointer to the last occurrence

finished:
        lda tmp2        ; high byte in X is already correct...
        rts
