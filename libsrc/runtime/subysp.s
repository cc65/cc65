;
; Piotr Fusik, 24.10.2003
; originally by Ullrich von Bassewitz
;
; CC65 runtime: Decrement the stackpointer by value in y
;

        .export         subysp
        .importzp       spc

.proc   subysp

        tya
        eor     #$ff
        sec
        adc     spc
        sta     spc
        bcs     @L1
        dec     spc+1
@L1:    rts

.endproc


