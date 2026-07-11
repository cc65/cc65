;
; int __fastcall__ rmdir (const char* name);
;
; On the RP6502, UNLINK removes a file or an empty directory, so rmdir is just
; an alias for remove()/unlink(). This overrides the common rmdir(), which would
; otherwise require __sysrmdir/__mappederrno that the RP6502 does not provide.
;

        .export         _rmdir

        .import         __sysremove


;--------------------------------------------------------------------------

_rmdir = __sysremove
