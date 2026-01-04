;
; struct tm* __fastcall__ localtime (const time_t* timep);
;

        .export         _localtime

        .import         __localtime


;--------------------------------------------------------------------------

_localtime = __localtime
