;
; Ullrich von Bassewitz, 25.10.2000
;
; CC65 runtime: Increment the stackpointer by 7
;

        .export         incsp7
        .import         addysp

.proc   incsp7

        ldy     #7
        jmp     addysp

.endproc




        
