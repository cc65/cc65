;
; Ullrich von Bassewitz, 19.11.2002
;
; The kernal open routines do not return a carry on error, so check the IEEE
; status, set carry flag and return
;

        .export         checkst

        .include        "pet.inc"

.proc   checkst

        lda     STATUS
        beq     @L1
        lda     #5              ; ### Device not present
        sec
        rts

@L1:    clc
        rts

.endproc
