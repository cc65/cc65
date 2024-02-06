;
; Ullrich von Bassewitz, 2009-09-17
;
; uintmax_t __fastcall__ strtoumax (const char* nptr, char** endptr, int base);
;

        .import         _strtoul
        .export         _strtoumax = _strtoul

