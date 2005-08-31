;
; Oliver Schmidt, 17.04.2005
;
; unsigned char __fastcall__ _syschdir (const char* name);
;

        .export         __syschdir
        .import		pushname, popname
	.import		__cwd

	.include	"zeropage.inc"
	.include	"mli.inc"

__syschdir:
        ; Push name
        jsr	pushname
        bne	oserr

        ; Set pushed name
        lda	sp
        ldx	sp+1
        sta	mliparam + MLI::PREFIX::PATHNAME
        stx	mliparam + MLI::PREFIX::PATHNAME+1

        ; Change directory
        lda	#SET_PREFIX_CALL
        ldx	#PREFIX_COUNT
        jsr	callmli
	bcs	cleanup

	ldy	#$01
:	lda	(sp),y
	sta	__cwd-1,y
	beq	cleanup
	iny
	bne	:-		; Branch always

        ; Cleanup name
cleanup:jsr	popname		; Preserves A

oserr:	rts
