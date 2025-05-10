;
; Colin Leroy-Mira, 2025-05-10
;

        .export         tgi_libref
        .import          _exit

.ifndef __APPLE2ENH__

        .import         has_80cols_card

tgi_libref      := has_80cols_card

.else

tgi_libref      := _exit

.endif
