;
; 2017-11-23, Greg King
;
; void cpeeks (char* s, unsigned length);
;
; C128 can't use "cbm/cpeeks.s" because both 40 and 80 columns must be handled.
; Stub file, for now, so that its library can be built.

        .export         _cpeeks

        .import         popax


_cpeeks:
        jmp     popax           ; pop s
