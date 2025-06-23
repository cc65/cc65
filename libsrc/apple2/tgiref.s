;
; Colin Leroy-Mira, 2025-05-10
;

        .export         tgi_libref
        .import          _exit

.ifndef __APPLE2ENH__

        .import         machinetype

tgi_libref      := machinetype

.else

tgi_libref      := _exit

.endif
