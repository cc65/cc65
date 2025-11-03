;
; Ullrich von Bassewitz, 2003-08-12
;
; int __fastcall__ chdir (const char* name);
;

        .export         _chdir

        .import         __syschdir


;--------------------------------------------------------------------------

_chdir = __syschdir
