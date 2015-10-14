;
; Oliver Schmidt, 18.04.2005
;

        .export         initcwd
        .import         __cwd

        .include        "zeropage.inc"
        .include        "mli.inc"

initcwd:
        ; Set static prefix buffer
        lda     #<__cwd
        ldx     #>__cwd
        sta     mliparam + MLI::PREFIX::PATHNAME
        stx     mliparam + MLI::PREFIX::PATHNAME+1

        ; Get current working directory
        lda     #GET_PREFIX_CALL
        ldx     #PREFIX_COUNT
        jsr     callmli

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
