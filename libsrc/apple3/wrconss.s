;
; Robert Justice, 2026
;
; Used to write console output in chunks for conio
;
        .export         wrconss
        .import         consref, setconioscr, consscrflg

        .include        "sos.inc"

        ; write chunk out to console
wrconss:
        lda     consref
        sta     sosparam + SOS::RW::REF_NUM

        bit     consscrflg    ; check if scroll is off
        beq     :+
        jsr     setconioscr
:
        lda     #WRITE_CALL
        ldx     #WRITE_COUNT
        jsr     callsos
        rts
