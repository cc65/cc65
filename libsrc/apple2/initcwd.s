;
; Oliver Schmidt, 18.04.2005
;

        .export         initcwd
        .import		__cwd

        .include	"zeropage.inc"
        .include        "mli.inc"

initcwd:
        ; Use imported buffer
        lda	#<__cwd
        ldx	#>__cwd
        sta     mliparam + MLI::PREFIX::PATHNAME
        stx     mliparam + MLI::PREFIX::PATHNAME+1

        ; Get current working directory
        lda     #GET_PREFIX_CALL
        ldx     #PREFIX_COUNT
        jsr     callmli

        ; Check length byte
        ldx	__cwd
        beq	done

        ; Replace trailing slash with zero
        sta	__cwd,x		; A = 0

        ; Remove length byte
        tax
:       inx
        lda	__cwd,x
        sta	__cwd-1,x
        bne	:-

done:   rts
