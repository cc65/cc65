;
; Piotr Fusik, 24.10.2003
; originally by Ullrich von Bassewitz
;
; CC65 runtime: Decrement the stackpointer by value in y
;

        .export         subysp
        .importzp       c_sp

.proc   subysp

        tya
        eor     #$ff
        sec
        adc     c_sp
        sta     c_sp
        bcs     @L1
        dec     c_sp+1
@L1:    rts

.endproc


