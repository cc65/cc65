;
; Karri Kaksonen, 2010
;
; This function reads count bytes from the place where the address counter is.
; Use lseek to place the address counter where you want to read from.
;
; The file descriptor is ignored in this implementation. The read operation
; reads bytes from a raw cart and does not understand the concept of files.
; So if you read over the end of file you get data from the next file.
;
; The count-parameter can be positive (Atari style) or negative (BLL style).
; In any case the read routine will work correctly.
;
; int __fastcall__ read(int fd,void *buf,int count)
;
	.importzp	_FileDestPtr
	.import		lynxread0
	.import		pushax,ldaxysp,ldax0sp,incsp6
	.export		_read

.segment	"CODE"

.proc	_read: near

.segment	"CODE"

	jsr     pushax
	ldy     #$03
	jsr     ldaxysp
	sta     _FileDestPtr
	stx     _FileDestPtr+1
	jsr     ldax0sp
	phx			; The BLL kit uses negative counts
	plx			; while the basic Lynx uses positive
	bmi	@1		; make all counts negative
	eor	#$FF
	pha
	txa
	eor	#$FF
	bra	@2
@1:	pha
	txa
@2:	tay
	plx
	jsr     lynxread0
	jsr     ldax0sp
	jmp     incsp6

.endproc

