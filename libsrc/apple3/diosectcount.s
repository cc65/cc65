;
; Oliver Schmidt, 31.03.2005
; Robert Justice, 2026
;
; unsigned __fastcall__ dio_query_sectcount (dhandle_t handle);
;

        .export         _dio_query_sectcount
        .import         _dio_query_sectsize, _malloc, _free

        .include        "zeropage.inc"
        .include        "errno.inc"
        .include        "sos.inc"

_dio_query_sectcount:

        ; Set handle
        sta     sosparam + SOS::DINFO::DEV_NUM

        ; Alloc buffer for devname
        lda     #16
        ldx     #0
        jsr     _malloc
        sta     sosparam + SOS::DINFO::DEV_NAME
        stx     sosparam + SOS::DINFO::DEV_NAME+1

        ; Check buffer (hibyte is enough)
        txa
        beq     nomem

        ; option list needed to get total blocks type
        ldx     #7
        stx     sosparam + SOS::DINFO::LENGTH

        ; Get device details
        lda     #D_INFO_CALL
        ldx     #D_INFO_COUNT
        jsr     callsos
        bne     oserr         ; error, not valid

        ; Cleanup buffer
        lda     sosparam + SOS::DINFO::DEV_NAME
        ldx     sosparam + SOS::DINFO::DEV_NAME+1
        jsr     _free

        ; Get total blocks and return
        lda     sosoption + OPTION::DINFO::TOTAL_BLOCKS
        ldx     sosoption + OPTION::DINFO::TOTAL_BLOCKS+1

        rts


nomem:  lda     #$FF            ; Error code for sure not used by MLI
oserr:  sta     ___oserror

        ; Cleanup buffer
        lda     sosparam + SOS::DINFO::DEV_NAME
        ldx     sosparam + SOS::DINFO::DEV_NAME+1
        jsr     _free

        ; Save total blocks for failure
        lda     #$00
        ldx     #$00
        rts
