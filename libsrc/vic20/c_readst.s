;
; 1999-06-03, Ullrich von Bassewitz
; 2021-01-12, Greg King
;
; unsigned char cbm_k_readst (void);
;
; This version works around a bug in VIC-20 Kernal's READST function.
;

        .include        "vic20.inc"
        .include        "../cbm/cbm.inc"

        .export         _cbm_k_readst


_cbm_k_readst:
        ldx     #>$0000
        lda     DEVNUM
        cmp     #CBMDEV_RS232
        beq     @L1

        jmp     READST

; Work-around:  Read the RS-232 status variable directly.

@L1:    lda     RSSTAT
        stx     RSSTAT          ; reset the status bits
        rts
