;
; Oliver Schmidt, 18.04.2005
;

        .export         initcwd
        .import		__cwd
        .import		subysp, addysp

        .include	"zeropage.inc"
        .include        "mli.inc"

        .segment	"INIT"

initcwd:
        ; Alloc prefix buffer
        ldy	#1 + 64+1	; Length byte + max pathname length+trailing slash
        jsr	subysp

        ; Use allocated prefix buffer
        lda	sp
        ldx	sp+1
        sta     mliparam + MLI::PREFIX::PATHNAME
        stx     mliparam + MLI::PREFIX::PATHNAME+1

        ; Get current working directory
        lda     #GET_PREFIX_CALL
        ldx     #PREFIX_COUNT
        jsr     callmli
        bcs	done

        ; Check for null prefix
        ldy	#$00
        lda	(sp),y
        beq	done

        ; Set current working directory
        ; - omit trailing slash and length byte
        ; - terminating zero already in place
        tay
        dey
:       lda	(sp),y
        sta	__cwd-1,y
        dey
        bne	:-

        ; Cleanup stack
done:   ldy	#1 + 64+1	; Length byte + max pathname length+trailing slash
        jmp     addysp

