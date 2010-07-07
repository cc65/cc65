;
; Oliver Schmidt, 2010-05-24
;
; int __fastcall__ rootdir (unsigned char drive, char* buf);
;

        .export 	_rootdir
        .import		popa

        .include	"zeropage.inc"
        .include	"errno.inc"
        .include	"mli.inc"

_rootdir:
        ; Save buf
        sta	ptr1
        stx	ptr1+1

        ; Set buf
        sta	mliparam + MLI::ON_LINE::DATA_BUFFER
        stx	mliparam + MLI::ON_LINE::DATA_BUFFER+1

        ; Set drive
        jsr	popa
        sta	mliparam + MLI::ON_LINE::UNIT_NUM

        ; Get volume name
        lda	#ON_LINE_CALL
        ldx	#ON_LINE_COUNT
        jsr	callmli
        bcs	oserr

        ; Get volume name length
        ldy	#$00
        lda	(ptr1),y
        and	#15		; Max volume name length
        sta	tmp1
        
        ; Add leading slash
        lda	#'/'
        sta	(ptr1),y

	; Add terminating zero
        ldy	tmp1
        iny        
        lda	#$00
        sta	(ptr1),y
        
        ; Return success	; A = 0
        
        ; Set __oserror
oserr:  jmp	__mappederrno
