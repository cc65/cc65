;
; Ullrich von Bassewitz, 2003-06-12
;
; int __fastcall__ unlink (const char* name);
;

        .export         _unlink
        .import         _remove

; unlink is just an alias for remove

        _unlink = _remove

