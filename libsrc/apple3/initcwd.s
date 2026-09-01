;
; Oliver Schmidt, 18.04.2005
; Robert Justice, 2026
;

        .export         initcwd
        .import         __cwd

        .include        "zeropage.inc"
        .include        "apple3.inc"
        .include        "sos.inc"

initcwd:
        ; Call SOS
        brk
        .byte   GET_PREFIX_CALL ; SOS command
        .addr   prefixparams  ; SOS parameter

        ; Check for null prefix
        ldx     __cwd
        beq     done

        ; Remove length byte and trailing slash
        dex
        stx     tmp1
        ldx     #$00
:       lda     __cwd + 1,x
        sta     __cwd,x
        inx
        cpx     tmp1
        bcc     :-

        ; Add terminating zero
        lda     #$00
        sta     __cwd,x

done:   rts

        .data

prefixparams:
        .byte $02               ; Number of parameters
        .addr __cwd             ; Address of parameter
        .byte FILENAME_MAX      ; Length
