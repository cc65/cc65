;
; Piotr Fusik, 24.10.2003
; originally by Ullrich von Bassewitz
;
; CC65 runtime: Decrement the stackpointer by value in y
;

        .export         subysp
        .importzp       sp

.proc   subysp

        tya
        eor     #$ff
        sec
        adc     sp
        sta     sp
        bcs     @L1
        dec     sp+1
@L1:    rts

.endproc


