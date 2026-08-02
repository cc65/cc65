;
; Robert Justice, 07.06.2026
;
; Apple /// SOS call api
;

        .include        "apple3.inc"
        .include        "sos.inc"

        .data

sosparam:
        .tag    SOS

sosoption:
        .tag    OPTION


        .code

callsos:
        ; Store parameters
        sta     call
        stx     sosparam

        ldy     #<sosoption
        ldx     #>sosoption

        cmp     #CREATE_CALL
        bne     :+
        sty     sosparam + SOS::CREATE::OPTION_LIST
        stx     sosparam + SOS::CREATE::OPTION_LIST + 1
        jmp     gosos
:
        cmp     #GET_INFO_CALL
        bne     :+
        sty     sosparam + SOS::INFO::OPTION_LIST
        stx     sosparam + SOS::INFO::OPTION_LIST + 1
        jmp     gosos
:
        cmp     #OPEN_CALL
        bne     :+
        sty     sosparam + SOS::OPEN::OPTION_LIST
        stx     sosparam + SOS::OPEN::OPTION_LIST + 1
:
        cmp     #D_INFO_CALL
        bne     :+
        sty     sosparam + SOS::DINFO::OPTION_LIST
        stx     sosparam + SOS::DINFO::OPTION_LIST + 1
:


gosos:  
        ; Call SOS
        brk
call:   .byte   $00
        .addr   sosparam

        rts

