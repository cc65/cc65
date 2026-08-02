;
; Oliver Schmidt, 24.03.2005
; Robert Justice, 2026
;

        .export         dioprolog, diocommon, dioepilog
        .import         popax

        .include        "errno.inc"
        .include        "sos.inc"

dioprolog:
        ; Set buffer
        sta     sosparam + SOS::D_RW::DATA_BUFFER
        stx     sosparam + SOS::D_RW::DATA_BUFFER+1

        ; Get and set sect_num
        jsr     popax
        sta     sosparam + SOS::D_RW::BLOCK_NUM
        stx     sosparam + SOS::D_RW::BLOCK_NUM+1

        ; Get and set handle
        jsr     popax
        sta     sosparam + SOS::D_RW::DEV_NUM
        rts

diocommon:
        ; Call read_block or write_block
        jsr     callsos

dioepilog:
        ; Return success or error
        sta     ___oserror
        ldx     #>$0000
        rts
