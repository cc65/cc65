;
; void __fastcall__ _sys (struct regs* r);
;
; Ullrich von Bassewitz, 16.12.1998
;

        .export         __sys
        .import         jmpvec
        .importzp       ptr1


__sys:  sta     ptr1
        stx     ptr1+1          ; Save the pointer to r

; Fetch the PC and store it into the jump vector

        ldy     #5
        lda     (ptr1),y
        sta     jmpvec+2
        dey
        lda     (ptr1),y
        sta     jmpvec+1

; Remember the flags so we can restore them to a known state after calling the
; routine

        php

; Get the flags, keep the state of bit 4 and 5 using the other flags from
; the flags value passed by the caller. Push the new flags and push A.

        dey
        php
        pla                     ; Current flags -> A
        eor     (ptr1),y
        and     #%00110000
        eor     (ptr1),y
        pha                     ; Push new flags value
        ldy     #0
        lda     (ptr1),y
        pha

; Get and assign X and Y

        iny
        lda     (ptr1),y
        tax
        iny
        lda     (ptr1),y
        tay

; Set a and the flags, call the machine code routine

        pla
        plp
        jsr     jmpvec

; Back from the routine. Save the flags and A.

        php
        pha

; Put the register values into the regs structure

        tya
        ldy     #2
        sta     (ptr1),y
        dey
        txa
        sta     (ptr1),y
        dey
        pla
        sta     (ptr1),y
        ldy     #3
        pla
        sta     (ptr1),y

; Restore the old flags value

        plp

; Done

        rts

