; Based on code by Twilighte.
; 2012-05-06, Stefan Haubenthal
; 2013-07-22, Greg King
;
; void __fastcall__ atmos_save(const char* name, const void* start, const void* end);

        .export         _atmos_save
        .import         popax, store_filename

        .include        "atmos.inc"


.proc   _atmos_save

        sei
        sta     FILEEND
        stx     FILEEND+1
        jsr     popax
        sta     FILESTART
        stx     FILESTART+1
        jsr     popax
        jsr     store_filename
        lda     #00
        sta     AUTORUN
        jsr     csave_bit
        cli
        rts
csave_bit:
        php
        jmp     $e92c

.endproc
