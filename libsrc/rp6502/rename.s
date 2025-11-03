;
; Ullrich von Bassewitz, 2003-08-12
;
; int __fastcall__ rename (const char* oldname, const char* newname);
;

        .export         _rename

        .import         __sysrename


;--------------------------------------------------------------------------

_rename = __sysrename
