; Stefan Haubenthal, 2012-05-06
; based on code by Twilighte
; void __fastcall__ atmos_load(const char* name);

        .export         _atmos_load
        .import         store_filename


.proc   _atmos_load

        sei
        jsr     store_filename
        ldx     #$00
        stx     $02ad
        stx     $02ae
        stx     $025a
        stx     $025b
        jsr     cload_bit
        cli
        rts
cload_bit:
        pha
        jmp     $e874

.endproc
