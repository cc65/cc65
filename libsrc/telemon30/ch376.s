
	; For XA65 compatibily in the future
	.FEATURE 	c_comments,labels_without_colons,pc_assignment, loose_char_term
	.export 	_ch376_set_file_name
	.export 	_ch376_file_open
	.export 	_ch376_ic_get_version
	.export 	_ch376_reset
	.export 	_ch376_check_exist
	.export 	_ch376_disk_mount
	.export		_ch376_set_usb_mode

	.export		_ch376_file_close
	.export 	_ch376_seek_file	
	.export		_ch376_file_create	
	.export 	_ch376_fwrite
	; High level function

	
	.import 	popax
	.include    "zeropage.inc"
	.include    "telemon30.inc"
/*

*/
; CODE FOR CH376_SET_USB_MODE *************************************************

CH376_SET_USB_MODE_CODE_USB_HOST_SOF_PACKAGE_AUTOMATICALLY := $06

CH376_USB_INT_SUCCESS 		:= $14
CH376_USB_INT_CONNECT 		:= $15
CH376_USB_INT_DISCONNECT	:= $16
CH376_USB_INT_BUF_OVER 		:= $17
CH376_USB_INT_USB_READY 	:= $18
CH376_USB_INT_DISK_READ 	:= $1D
CH376_USB_INT_DISK_WRITE 	:= $1E
CH376_USB_INT_DISK_ERR 		:= $1F


CH376_ERR_OPEN_DIR 			:= $41
CH376_ERR_MISS_FILE 		:= $42
CH376_ERR_FOUND_NAME 		:= $43
CH376_ERR_DISK_DISCON 		:= $82
CH376_ERR_LARGE_SECTOR 		:= $84
CH376_ERR_TYPE_ERROR 		:= $92
CH376_ERR_BPB_ERROR 		:= $A1
CH376_ERR_DISK_FULL 		:= $B1
CH376_ERR_FDT_OVER 			:= $B2
CH376_ERR_FILE_CLOSE 		:= $B4



CH376_GET_IC_VER		:= $01
CH376_SET_BAUDRATE 		:= $02
CH376_GET_ENTER_SLEEP 	:= $03
CH376_RESET_ALL			:= $05
CH376_CHECK_EXIST		:= $06
CH376_GET_FILE_SIZE		:= $0C

CH376_SET_USB_MODE		:= $15
CH376_GET_STATUS		:= $22
CH376_RD_USB_DATA0		:= $27
CH376_CMD_WR_REQ_DATA	:= $2d
CH376_SET_FILE_NAME		:= $2F

CH376_DISK_CONNECT		:= $30 ; check the disk connection status
CH376_DISK_MOUNT		:= $31
CH376_FILE_OPEN  		:= $32
CH376_FILE_ENUM_GO		:= $33
CH376_FILE_CREATE 		:= $34
CH376_FILE_CLOSE 		:= $36
CH376_BYTE_LOCATE 		:= $39
CH376_BYTE_READ			:= $3A
CH376_BYTE_RD_GO 		:= $3B
CH376_BYTE_WRITE		:= $3C
CH376_BYTE_WR_GO		:= $3D
CH376_DISK_CAPACITY 	:= $3E
CH376_DISK_RD_GO 		:= $55

.proc _ch376_file_close
	lda 	#CH376_FILE_CLOSE
	sta 	CH376_COMMAND
	jsr 	_ch376_wait_response
	rts	
.endproc

.proc _ch376_seek_file
	ldx 	#CH376_BYTE_LOCATE
	stx 	CH376_COMMAND
	sta 	CH376_DATA
	sty 	CH376_DATA
	lda 	#$00 ; Don't manage 32 bits length
	sta 	CH376_DATA
	sta 	CH376_DATA
	jsr 	_ch376_wait_response
	rts
.endproc

; void ch376_set_file_name(char *filename)
.proc _ch376_set_file_name
	sta		ptr1
	stx		ptr1+1
	lda 	#CH376_SET_FILE_NAME ;$2f
	sta 	CH376_COMMAND
	ldy 	#0
loop:
	lda 	(ptr1),y ; replace by bufnom
	beq 	end ; we reached 0 value
	;BRK_TELEMON	XMINMA
	sta 	CH376_DATA
	iny
	cpy 	#13 ; because we don't manage longfilename shortname =11
	bne 	loop
end:	
	sta 	CH376_DATA
	rts
.endproc 
	
; char _ch376_file_open();
	
.proc _ch376_file_open
	lda 	#CH376_FILE_OPEN ; $32
	sta 	CH376_COMMAND
	jsr 	_ch376_wait_response
;	ldx #0
	rts
.endproc 

	;CMD_GET_FILE_SIZE
	
.proc _ch376_get_file_size
	lda 	#CH376_GET_FILE_SIZE
	sta 	CH376_COMMAND
	lda 	#$68
	sta 	CH376_DATA
	; store file leng
	lda 	CH376_DATA
	sta 	tmp1
	lda 	CH376_DATA
	sta 	tmp1+1
	lda 	CH376_DATA
	sta 	tmp2
	lda 	CH376_DATA
	sta 	tmp2+1
	rts
.endproc 

; void ch376_reset();
	
.proc _ch376_reset
	lda #CH376_RESET_ALL ; 5 
	sta CH376_COMMAND
	; waiting
	ldy #0
	ldx #0
loop:
	nop
	inx
	bne loop
	iny
	bne loop
	rts
.endproc 

; char  ch376_check_exist(char value);
	
.proc _ch376_check_exist
	sta tmp1
	lda #CH376_CHECK_EXIST ; 
	sta CH376_COMMAND
	lda tmp1
	sta CH376_DATA
	lda CH376_DATA
	rts
.endproc 
	
; char 	ch376_ic_get_version(void)
.proc _ch376_ic_get_version
	lda #CH376_GET_IC_VER
	sta CH376_COMMAND
	ldx #0
	lda CH376_DATA
	rts
.endproc 

; void ch376_set_usb_mode(char mode)
	
.proc _ch376_set_usb_mode
; CH376_SET_USB_MODE_CODE_USB_HOST_SOF_PACKAGE_AUTOMATICALLY
	ldx 	#CH376_SET_USB_MODE ; $15
	stx 	CH376_COMMAND
	sta 	CH376_DATA
	rts
.endproc 
		
; 	void ch376_set_bytes_write(int value);
.proc _ch376_set_bytes_write
	ldy		#CH376_BYTE_WRITE
	sty 	CH376_COMMAND
	sta 	CH376_DATA
	stx 	CH376_DATA
	lda 	#0
	sta 	CH376_DATA
	sta 	CH376_DATA
	jsr 	_ch376_wait_response
	rts
.endproc 	
	
.proc _ch376_set_bytes_read
	ldy 	#CH376_BYTE_READ
	sty 	CH376_COMMAND
	; Storing 32 bits value
	sta 	CH376_DATA
	stx 	CH376_DATA
	lda 	#0
	sta 	CH376_DATA
	sta 	CH376_DATA
	jsr 	_ch376_wait_response
	rts
.endproc 		

; char 	ch376_disk_mount();
.proc _ch376_disk_mount
	lda #CH376_DISK_MOUNT ; $31
	sta CH376_COMMAND
	jsr _ch376_wait_response
	; if we read data value, we have then length of the volume name
	ldx #0
	rts	
.endproc 


; char 	ch376_wait_response();
.proc _ch376_wait_response
; 1 return 1 if usb controller does not respond
; else A contains answer of the controller
	ldy #$ff
loop3:
	ldx #$ff ; don't decrease this counter. Because ch376 won't respond if there is a lower value
loop:
	lda CH376_COMMAND
	and #%10000000
	cmp #128
	bne no_error
	dex
	bne loop
	dey
	bne loop3
	; error is here
	rts
no_error:
	lda #CH376_GET_STATUS
	sta CH376_COMMAND
	lda CH376_DATA
	rts
.endproc 

.proc _ch376_fread
		; use ptr1 to count bytes
	jsr _ch376_set_bytes_read

continue:	
	cmp #CH376_USB_INT_DISK_READ  ; something to read
	beq we_read
	cmp #CH376_USB_INT_SUCCESS ; finished
	beq finished 
	; TODO  in A : $ff X: $ff
	lda #0
	tax
	rts
we_read:
	lda #CH376_RD_USB_DATA0
	sta CH376_COMMAND

	lda CH376_DATA ; contains length read
	sta tmp2; Number of bytes to read

	ldy #0
loop:
	lda CH376_DATA ; read the data
	sta (PTR_READ_DEST),y

	iny
	cpy tmp2
	bne loop
	tya
	clc
	adc PTR_READ_DEST
	bcc next
	inc PTR_READ_DEST+1
next:
	sta PTR_READ_DEST
	
	lda #CH376_BYTE_RD_GO
	sta CH376_COMMAND
	jsr _ch376_wait_response
	jmp continue
finished:
	; TODO  return bytes read
	lda tmp1

	ldx tmp1+1

	rts	
.endproc	

; void _ch376_fwrite(void *ptr,int number)
.proc _ch376_fwrite
		; use ptr1 to count bytes
	sta ptr2
	stx ptr2+1
	
	jsr popax
	sta PTR_READ_DEST
	stx PTR_READ_DEST+1
	
	lda ptr2
	ldx ptr2+1
	jsr _ch376_set_bytes_write
	;cmp #CH376_USB_INT_SUCCESS
	;beq finished
	;jsr popax
	
	;jsr _ch376_wait_response
	
continue:	
	cmp #CH376_USB_INT_DISK_WRITE  ; something to read
	beq we_read
	cmp #CH376_USB_INT_SUCCESS ; finished
	beq finished 
	; TODO  in A : $ff X: $ff
	lda #0
	tax
	rts
we_read:
	lda #CH376_CMD_WR_REQ_DATA
	sta CH376_COMMAND
	
	lda CH376_DATA ; contains length read
	sta tmp2; Number of bytes to read


	;ldy #0
loop:
	;lda (PTR_READ_DEST),y
	lda #65
	sta CH376_DATA ; read the data
	dec tmp2
	bne loop
;	dec ptr2
	;bne continue3
	;dec ptr2+1
	;bne continue3
;continue3
;	lda ptr2+1
	;bne continue2
	;lda ptr2
	;beq finished
;continue2	
;	iny
;	cpy tmp2
;	bne loop
;	tya
;	clc
;	adc PTR_READ_DEST
;	bcc next
;	inc PTR_READ_DEST+1
;next:
;	sta PTR_READ_DEST
	
	lda #CH376_BYTE_WR_GO
	sta CH376_COMMAND
	jsr _ch376_wait_response
	jmp continue
finished:
	; TODO  return bytes read
	lda tmp1
	;lda #<8000
	ldx tmp1+1
	;ldx #>8000
	rts	
.endproc	



.proc _ch376_file_create
	lda 	#CH376_FILE_CREATE
	sta 	CH376_COMMAND
	jsr 	_ch376_wait_response
	rts
.endproc