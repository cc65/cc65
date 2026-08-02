;
; Oliver Schmidt, 15.04.2005
;
; unsigned char __fastcall__ _sysrmdir (const char* name);
;

        .export         __sysrmdir
        .import         __sysremove

__sysrmdir := __sysremove
