;
; Ullrich von Bassewitz, 2009-09-17
;
; intmax_t __fastcall__ strtoimax (const char* nptr, char** endptr, int base);
;

        .import         _strtol
        .export         _strtoimax = _strtol

