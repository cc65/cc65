;
; Ullrich von Bassewitz, 31.08.1998
;
; CC65 runtime: Push word from stack
;

        .export         pushb, pushbidx
        .import         pushax
        .importzp       ptr1

pushbidx:
        sty     ptr1
        clc
        adc     ptr1
        bcc     pushb
        inx
pushb:  sta     ptr1
        stx     ptr1+1
        ldx     #0              ; Load index/high byte
.if .cap(CPU_HAS_ZPIND)
        lda     (ptr1)          ; Save one cycle for the C02
.else
        lda     (ptr1,x)
.endif
        bpl     L1
        dex                     ; Make high byte FF
L1:     jmp     pushax

