;
; Ullrich von Bassewitz, 19.11.2002
;
; CKOUT replacement function for the PETs
;

        .export         CKOUT
        .import         checkst


.proc   CKOUT

        jsr     $FFC9           ; Call kernal function
        jmp     checkst         ; Check status, return carry on error

.endproc


                     
