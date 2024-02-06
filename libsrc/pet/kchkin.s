;
; Ullrich von Bassewitz, 19.11.2002
;
; CHKIN replacement function for the PETs
;

        .export         CHKIN
        .import         checkst


.proc   CHKIN

        jsr     $FFC6           ; Call kernal function
        jmp     checkst         ; Check status, return carry on error

.endproc


