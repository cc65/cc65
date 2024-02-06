;
; Ullrich von Bassewitz, 11.12.1998
;
; int strcoll (const char* s1, const char* s2);
;
; Since we don't have locales, this function is equivalent to strcmp.
;

        .export         _strcoll
        .import         _strcmp

_strcoll        = _strcmp

