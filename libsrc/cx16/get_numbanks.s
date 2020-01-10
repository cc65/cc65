;
; 2020-01-10, Greg King
;
; unsigned char get_numbanks (void);
; /* Return the number of RAM banks that the machine has. */
;
; The Commander X16 version of MEMTOP returns with an extra value:
; The accumulator describes the number of RAM banks that exist on the hardware.
;

        .export         _get_numbanks

        .import         MEMTOP


_get_numbanks:
        sec
        jsr     MEMTOP
        ldx     #>$0000
        rts
