;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Increment the stackpointer by 6
;

        .export         incsp6
        .import         addysp

.proc   incsp6

        ldy     #6
        jmp     addysp

.endproc




        
