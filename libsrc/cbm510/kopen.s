;
; Ullrich von Bassewitz, 2003-12-20
;
; OPEN kernal call.
;
; NOTE: The OPEN system call in the CBM610 kernal is different from the 
; standard. It evaluates the carry flag and does a normal open if carry clear
; and some strange things (output sa 15 + name on IEC) if carry set. To be
; compatible with our CBM file stuff, we have to clear the carry before 
; calling the real OPEN.

        .export         OPEN

.proc   OPEN

        clc
        jmp     $FFC0

.endproc



