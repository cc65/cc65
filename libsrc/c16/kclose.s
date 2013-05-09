;
; Ullrich von Bassewitz, 2003-12-21
;
; CLOSE replacement function
;

        .export         CLOSE

.proc   CLOSE
        clc                             ; Force C64 compatible behaviour
        jmp     $FFC3                   ; Call the ROM routine
.endproc


