;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Increment the stackpointer by 8
;

        .export         incsp8
        .import         addysp

.proc   incsp8

        ldy     #8
        jmp     addysp

.endproc


