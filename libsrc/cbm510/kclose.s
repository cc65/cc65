;
; Ullrich von Bassewitz, 2003-12-21
;
; CLOSE kernal call.
;
; NOTE: The CLOSE system call in the CBM610 kernal will only remove the file
; entry and not close the file on IEC if the carry is clear on entry. To make
; this somewhat compatible with the C64, set the carry before jumping to the
; kernal.

        .export         CLOSE

.proc   CLOSE

        sec
        jmp     $FFC3

.endproc



