;
; Ullrich von Bassewitz, 2003-12-19
;
; RDTIM kernal call
;

        .export         RDTIM
        .import         time : zeropage


.proc   RDTIM

        sei
        lda     time+0
        ldx     time+1
        ldy     time+2
        cli
        rts

.endproc

