;
; Ullrich von Bassewitz, 12.11.2002
;
; char* __fastcall__ ctime (time_t* timep);
;

        .export         _ctime
        .import         _localtime, _asctime


.proc   _ctime

; return asctime (localtime (timep));

        jsr     _localtime
        jmp     _asctime

.endproc

