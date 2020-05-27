;
; 2020-01-29, Greg King
;
; unsigned short get_numbanks (void);
; /* Return the number of RAM banks that the machine has. */
;
; The Commander X16 version of MEMTOP returns with an extra value:
; The accumulator describes the number of RAM banks that exist on the hardware.
; A zero accumulator means that there are 256 RAM banks.
;

        .export         _get_numbanks

        .import         MEMTOP


_get_numbanks:
        sec
        jsr     MEMTOP
        ldx     #>$0000
        cmp     #<$0100         ; are there 256 banks?
        bne     :+
        inx                     ; yes
:       rts
