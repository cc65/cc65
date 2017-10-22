;
; Ullrich von Bassewitz, 23.11.2002
; Christian Krueger, 11-Mar-2017, saved 5 bytes
;
; CC65 runtime: Convert char in ax into a long
;

        .export         aulong, along
        .importzp       sreg

; Convert A from char to long in EAX

along:  ldx     #$ff
        cmp     #$80            ; Positive?
        bcs     store           ; no, apply $FF

aulong: ldx     #0
store:  stx     sreg
        stx     sreg+1
        rts
