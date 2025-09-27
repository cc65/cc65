;
; Ullrich von Bassewitz, 2003-08-12
;
; int __fastcall__ remove (const char* name);
;

        .export         _remove

        .import         __sysremove


;--------------------------------------------------------------------------

_remove = __sysremove
