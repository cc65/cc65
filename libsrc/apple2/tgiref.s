;
; Colin Leroy-Mira, 2025-05-10
;

        .export         tgi_libref
        .import          _exit

.ifndef __APPLE2ENH__

        .import         iie_or_newer

tgi_libref      := iie_or_newer

.else

tgi_libref      := _exit

.endif
