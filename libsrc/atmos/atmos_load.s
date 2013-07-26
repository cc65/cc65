; Based on code by Twilighte.
; 2012-05-06, Stefan Haubenthal
; 2013-07-22, Greg King
;
; void __fastcall__ atmos_load(const char* name);

        .export         _atmos_load
        .import         store_filename

        .include        "atmos.inc"


.proc   _atmos_load

        sei
        jsr     store_filename
        ldx     #$00
        stx     AUTORUN         ; don't try to run the file
        stx     LANGFLAG        ; BASIC
        stx     JOINFLAG        ; don't join it to another BASIC program
        stx     VERIFYFLAG      ; load the file
        jsr     cload_bit
        cli
        rts
cload_bit:
        pha
        jmp     $e874

.endproc
