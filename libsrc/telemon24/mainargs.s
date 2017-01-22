;
; 2003-03-07, Ullrich von Bassewitz
; 2011-01-28, Stefan Haubenthal
; 2014-09-10, Greg King
;
; Set up arguments for main
;

        .constructor    initmainargs, 24
        .import         __argc, __argv

        .include        "telemon24.inc"
        .macpack        generic

MAXARGS  = 10                   ; Maximum number of arguments allowed


.segment        "ONCE"

.proc   initmainargs

.endproc

.segment        "INIT"

term:   .res    1
name:   .res    FNAME_LEN + 1
args:   .res    SCREEN_XSIZE * 2 - 1

.data

; This array has zeroes when initmainargs starts.
; char* argv[MAXARGS+1]={name};

argv:   .addr   name
        .res    MAXARGS * 2
