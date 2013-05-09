;
; Oliver Schmidt, 24.03.2005
;

        .export         dioprolog, diocommon, dioepilog
        .import         popax

        .include        "errno.inc"
        .include        "mli.inc"

dioprolog:
        ; Set buffer
        sta     mliparam + MLI::RW_BLOCK::DATA_BUFFER
        stx     mliparam + MLI::RW_BLOCK::DATA_BUFFER+1

        ; Get and set sect_num
        jsr     popax
        sta     mliparam + MLI::RW_BLOCK::BLOCK_NUM
        stx     mliparam + MLI::RW_BLOCK::BLOCK_NUM+1

        ; Get and set handle
        jsr     popax
        sta     mliparam + MLI::RW_BLOCK::UNIT_NUM
        rts

diocommon:
        ; Call read_block or write_block
        ldx     #RW_BLOCK_COUNT
        jsr     callmli         

dioepilog:
        ; Return success or error
        sta     __oserror
        ldx     #$00
        rts
