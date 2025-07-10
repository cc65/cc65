;
; void __fastcall__ scandisplay(unsigned char left, unsigned char middle, unsigned char right);
;

.include        "kim1.inc"

.import         popa

.export         _scandisplay

.proc           _scandisplay

        sta     $F9             ; Rightmost display data
        jsr     popa
        sta     $FA             ; Middle display data
        jsr     popa
        sta     $FB             ; Leftmost display data
        jmp     SCANDS

.endproc
