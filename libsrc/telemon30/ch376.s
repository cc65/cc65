	.export 	_ch376_set_file_name
	.export 	_ch376_file_open
	.export 	_ch376_ic_get_version
	.export 	_ch376_reset
	.export 	_ch376_check_exist
	.export 	_ch376_disk_mount
	
	.import 	popax
	.importzp 	sp,tmp2,tmp3,tmp1
	.include    "telemon30.inc"

; CODE FOR CH376_SET_USB_MODE *************************************************

CH376_SET_USB_MODE_CODE_USB_HOST_SOF_PACKAGE_AUTOMATICALLY := $06

CH376_USB_INT_DISK_READ := $1d
CH376_USB_INT_SUCCESS 	:= $14
CH376_ERR_MISS_FILE 	:= $42
	
CH376_DATA		:=$340
CH376_COMMAND	:=$341

CH376_GET_IC_VER		:= $01
CH376_SET_BAUDRATE 		:= $02
CH376_GET_ENTER_SLEEP 	:= $03
CH376_RESET_ALL			:= $05
CH376_CHECK_EXIST		:= $06
CH376_GET_FILE_SIZE		:= $0C
CH376_SET_USB_MODE		:= $15
CH376_GET_STATUS		:= $22
CH376_RD_USB_DATA0		:= $27
CH376_SET_FILE_NAME		:= $2f
CH376_DISK_CONNECT		:= $30 ; check the disk connection status
CH376_DISK_MOUNT		:= $31
CH376_FILE_OPEN  		:= $32
CH376_FILE_ENUM_GO		:= $33
CH376_FILE_CLOSE 		:= $36
CH376_BYTE_READ			:= $3A
CH376_BYTE_RD_GO 		:= $3b
CH376_BYTE_WRITE		:= $3C
CH376_DISK_CAPACITY 	:= $3E
CH376_DISK_RD_GO 		:= $55



; void ch376_set_file_name(char *filename)
.proc _ch376_set_file_name
	sta tmp1
	stx tmp1+1
	lda #CH376_SET_FILE_NAME ;$2f
	sta CH376_COMMAND
	ldy #0
loop:
	lda (tmp1),y ; replace by bufnom
	beq end ; we reached 0 value
	sta CH376_DATA
	iny
	cpy #13 ; because we don't manage longfilename shortname =11
	bne loop
end:	
	sta CH376_DATA
	rts
.endproc 
	
; void _ch376_file_open();
	
.proc _ch376_file_open
	lda #CH376_FILE_OPEN ; $32
	sta CH376_COMMAND
	jsr _ch376_wait_response
	rts
.endproc 

	;CMD_GET_FILE_SIZE
	
.proc _ch376_get_file_size
	lda #CH376_GET_FILE_SIZE
	sta CH376_COMMAND
	lda #$68
	sta CH376_DATA
	; store file leng
	lda CH376_DATA
	sta tmp1
	lda CH376_DATA
	sta tmp1+1
	lda CH376_DATA
	sta tmp2
	lda CH376_DATA
	sta tmp2+1
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
	lda CH376_DATA
	rts
.endproc 

; void ch376_set_usb_mode(char mode)
	
.proc _ch376_set_usb_mode
; CH376_SET_USB_MODE_CODE_USB_HOST_SOF_PACKAGE_AUTOMATICALLY
	sta tmp1
	lda #CH376_SET_USB_MODE ; $15
	sta CH376_COMMAND
	lda tmp1
	sta CH376_DATA
	rts
.endproc 
	
	
; 	void ch376_set_bytes_write(int value);
.proc _ch376_set_bytes_write
	sta tmp1
	stx tmp1+1
	ldx #CH376_BYTE_WRITE
	stx CH376_COMMAND
	lda tmp1
	sta CH376_DATA
	lda tmp1+1
	sta CH376_DATA
	jsr _ch376_wait_response
	rts
.endproc 	
	
.proc _ch376_set_bytes_read
	ldx #CH376_BYTE_READ
	stx CH376_COMMAND
	sta CH376_DATA
	sty CH376_DATA
	jsr _ch376_wait_response
	rts
.endproc 		

; char 	ch376_disk_mount();
.proc _ch376_disk_mount
	lda #CH376_DISK_MOUNT ; $31
	sta CH376_COMMAND
	jsr _ch376_wait_response
	; if we read data value, we have then length of the volume name
	rts	
.endproc 


; char 	ch376_wait_response();
.proc _ch376_wait_response
; 1 return 1 if usb controller does not respond
; else A contains answer of the controller
	ldy #$ff
loop3:
	ldx #$ff ; merci de laisser une valeur importante car parfois en mode non debug, le controleur ne répond pas tout de suite
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

