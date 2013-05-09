;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Increment the stackpointer by 5
;

        .export         incsp5
        .import         addysp

.proc   incsp5

        ldy     #5
        jmp     addysp

.endproc




        
