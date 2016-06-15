;
; Christian Groessler, June-2016
;
; unsigned char doesclrscr(void);
;
; returns 0/1 if after program termination the screen isn't/is cleared
;

        .export  _doesclrscrafterexit
        .import  __is_cmdline_dos
        .import  return1

.ifdef __ATARIXL__
_doesclrscrafterexit = return1          ; the c65 runtime always clears the screen at program termination
.else   
_doesclrscrafterexit:
        jsr     __is_cmdline_dos        ; currently (unless a DOS behaving differently is popping up)
        eor     #$01                    ; we can get by with the inverse of __is_cmdline_dos
        rts
.endif
