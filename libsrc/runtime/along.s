;
; Ullrich von Bassewitz, 23.11.2002
;
; CC65 runtime: Convert char in ax into a long
;

        .export         aulong, along
        .importzp       sreg

; Convert A from char to long in EAX

aulong: ldx     #0
        stx     sreg
        stx     sreg+1
        rts

along:  cmp     #$80            ; Positive?
        bcc     aulong          ; Yes, handle like unsigned type
        ldx     #$ff
        stx     sreg
        stx     sreg+1
        rts


