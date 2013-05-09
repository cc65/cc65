;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Increment the stackpointer by 3
;

        .export         incsp3
        .import         addysp

.proc   incsp3

        ldy     #3
        jmp     addysp

.endproc




        
