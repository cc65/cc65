        .export         _Mym_MusicStart
		.importzp sp,tmp2,tmp3,tmp1,ptr1
		
		.include        "telemon30.inc"
		
; To check: AYC
; http://cpcwiki.eu/index.php/AYC



	
_DecodedByte 		:=$D0 ; Byte being currently decoded from the MYM stream
_DecodeBitCounter 	:=$D2		; Number of bits we can read in the current byte
_DecodedResult		:=$D3	; What is returned by the 'read bits' function
_CurrentAYRegister	:=$D4		; Contains the number of the register being decoded	
_RegisterBufferHigh	:=$D5		; Points to the high byte of the decoded register buffer, increment to move to the next register	
_BufferFrameOffset	:=$D6		; From 0 to 127, used when filling the decoded register buffer
_MusicResetCounter	:=$D7		; 2 bytes Contains the number of rows to play before reseting
_CurrentFrame		:=$D9		; From 0 to 255 and then cycles... the index of the frame to play this vbl
_PlayerVbl			:=$DA
_FrameLoadBalancer	:=$DB		; We depack a new frame every 9 VBLs, this way the 14 registers are evenly depacked over 128 frames


VIA_1			:=	$30f
VIA_2			:=	$30c

_MusicData		:=	$c000

; mym(char *buf)




;
; Current PSG values during unpacking
;


.proc _Mym_MusicStart

	; The two first bytes of the MYM music is the number of rows in the music
	; We decrement that at each frame, and when we reach zero, time to start again.
	sta ptr1
	stx ptr1+1
	
	ldy #0
	lda (ptr1),y
	sta _MusicResetCounter+0
	iny
	lda (ptr1),y
	tax
	inx
	stx _MusicResetCounter+1
	
	;ldx _MusicData+0
	;stx _MusicResetCounter+0
	;ldx _MusicData+1
	;inx
	;stx _MusicResetCounter+1
		

	; Initialize the read bit counter
	ldy #2 ; should be useless because we can do iny which earn 1 byte
	
	lda ptr1
	clc
	adc #2
	bcc next20
	inc ptr1+1
	lda ptr1+1
	sta __auto_music_ptr+2
next20:
	sta ptr1
	sta __auto_music_ptr+1
	
	
	
	;lda #<(_MusicData+2)
	;sta __auto_music_ptr+1
	;lda #>(_MusicData+2)
	;sta __auto_music_ptr+2

	lda #1
	sta _DecodeBitCounter

	; Clear all data
	lda #0
	sta _DecodedResult
	sta _DecodedByte
	sta _PlayerVbl
	sta _PlayerRegCurrentValue
	sta _BufferFrameOffset
	sta _PlayerCount
	sta _CurrentAYRegister
	sta _CurrentFrame

	ldx #14
loop_init:
	dex
	sta _PlayerRegValues,x
	bne loop_init


	;
	; Unpack the 128 first register frames
	;
	
	lda #>_PlayerBuffer
	sta _RegisterBufferHigh

	ldx #0
unpack_block_loop:
	stx _CurrentAYRegister
	
	; Unpack that register
	jsr _PlayerUnpackRegister2

	; Next register
	ldx _CurrentAYRegister
	inx
	cpx #14
	bne unpack_block_loop
	

	lda #128
	sta _PlayerVbl+0

	lda #0
	sta _PlayerCount
	sta _CurrentAYRegister
	sta _CurrentFrame

	lda #9
	sta _FrameLoadBalancer

	lda #1
	sta _MusicPlaying

	;
	; Install the IRQ
	;
	php
	sei
	lda #<_Mym_PlayFrame
	sta _InterruptCallBack_3+1
	lda #>_Mym_PlayFrame
	sta _InterruptCallBack_3+2
	plp

	rts


_Mym_MusicStop:

	; Indicate the main code that the music is finished
	lda #0
	sta _MusicPlaying

	; Disable the IRQ so it does not conflict or cause weird things
	php
	sei
	lda #<_DoNothing
	sta _InterruptCallBack_3+1
	lda #>_DoNothing
	sta _InterruptCallBack_3+2
	plp

	; Cut the sound so it does not sounds like a dying cat

	; y=register number
	; x=value to write
	ldy #7 ; Control register
	ldx #$FF
	jsr _PsgPlayRegister

	ldy #8 	; Volume A
	ldx #0
	jsr _PsgPlayRegister

	ldy #9	; Volume B
	ldx #0
	jsr _PsgPlayRegister

	ldy #10	; Volume C
	ldx #0
	jsr _PsgPlayRegister
	rts


_Mym_PlayFrame:

	;
	; Check for end of music
	; CountZero: $81,$0d
	dec _MusicResetCounter+0
	bne music_contines
	dec _MusicResetCounter+1
	bne music_contines

music_resets:
	jmp _Mym_MusicStop
	
music_contines:

	;
	; Play a frame of 14 registers
	;
	
	lda _CurrentFrame
	sta _auto_psg_play_read+1
	lda #>_PlayerBuffer
	sta _auto_psg_play_read+2

	ldy #0
register_loop:

_auto_psg_play_read:
	ldx	_PlayerBuffer

	; y=register number
	; x=value to write
	jsr _PsgPlayRegister

	inc _auto_psg_play_read+2 
	iny
	cpy #14
	bne register_loop
	


	inc _CurrentFrame
	inc _PlayerCount

	lda _CurrentAYRegister
	cmp #14
	bcs end_reg

	
	dec _FrameLoadBalancer
	bne end

	jsr _PlayerUnpackRegister
	inc _CurrentAYRegister
	lda #9
	sta _FrameLoadBalancer
end:	
	rts
	

end_reg:
	
	lda _PlayerCount
	cmp #128
	bcc skip2

	lda #0
	sta _CurrentAYRegister
	sta _PlayerCount
	lda #9
	sta _FrameLoadBalancer
	
	clc
	lda _PlayerVbl+0
	adc #128
	sta _PlayerVbl+0
skip2:
	

	rts



; y=register number
; x=value to write
_PsgPlayRegister:

	sty	VIA_1
	txa

	pha
	lda	VIA_2
	ora	#$EE		; $EE	238	11101110
	sta	VIA_2

	and	#$11		; $11	17	00010001
	ora	#$CC		; $CC	204	11001100
	sta	VIA_2

	tax
	pla
	sta	VIA_1
	txa
	ora	#$EC		; $EC	236	11101100
	sta	VIA_2

	and	#$11		; $11	17	00010001
	ora	#$CC		; $CC	204	11001100
	sta	VIA_2

	rts




;
; Initialise X with the number of bits to read
; Y is not modifier
;
_ReadBits:

	lda #0
	sta _DecodedResult

	; Will iterate X times (number of bits to read)
loop_read_bits:

	dec _DecodeBitCounter
	beq get_next_byte

shift_bit:
	asl _DecodedByte
	rol _DecodedResult

	dex
	bne loop_read_bits
	rts

get_next_byte:
	; reset mask
	lda #8
	sta _DecodeBitCounter

	; fetch a new byte, and increment the adress.
__auto_music_ptr:
	lda _MusicData+2
	sta _DecodedByte

	inc __auto_music_ptr+1
	bne shift_bit
	inc __auto_music_ptr+2
	jmp shift_bit





_PlayerUnpackRegister:
	lda #>_PlayerBuffer
	clc
	adc _CurrentAYRegister
	sta _RegisterBufferHigh
_PlayerUnpackRegister2:
	;
	; Init register bit count and current value
	;	 
	ldx _CurrentAYRegister
	lda _PlayerRegValues,x
	sta _PlayerRegCurrentValue  
	

	;
	; Check if it's packed or not
	; and call adequate routine...
	;
	ldx #1
	jsr _ReadBits
	ldx _DecodedResult
	bne DecompressFragment

	
UnchangedFragment:

	;
	; No change at all, just repeat '_PlayerRegCurrentValue' 128 times 
	;
	lda _RegisterBufferHigh				; highpart of buffer adress + register number
	sta __auto_copy_unchanged_write+2

	ldx #128							; 128 iterations
	lda _PlayerRegCurrentValue			; Value to write

	ldy _PlayerVbl
	
repeat_loop:
__auto_copy_unchanged_write:
	sta _PlayerBuffer,y
	iny	
	dex
	bne repeat_loop


	jmp player_main_return

	
player_main_return:
	; Write back register current value
	ldx _CurrentAYRegister
	lda _PlayerRegCurrentValue  
	sta _PlayerRegValues,x

	; Move to the next register buffer
	inc _RegisterBufferHigh
	rts




DecompressFragment:
	lda _PlayerVbl						; Either 0 or 128 at this point else we have a problem...
	sta _BufferFrameOffset

decompressFragmentLoop:
	
player_copy_packed_loop:
	; Check packing method
	ldx #1
	jsr _ReadBits

	ldx _DecodedResult
	bne PlayerNotCopyLast

UnchangedRegister:

	; We just copy the current value 128 times
	lda _RegisterBufferHigh				; highpart of buffer adress + register number
	sta __auto_player_copy_last+2

	ldx _BufferFrameOffset				; Value between 00 and 7f
	lda _PlayerRegCurrentValue			; Value to copy
__auto_player_copy_last:
	sta _PlayerBuffer,x

	inc _BufferFrameOffset



player_return:

	; Check end of loop
	lda _BufferFrameOffset
	and #127
	bne decompressFragmentLoop

	jmp player_main_return


PlayerNotCopyLast:
	; Check packing method
	ldx #1
	jsr _ReadBits

	ldx _DecodedResult
	beq DecompressWithOffset

ReadNewRegisterValue:
	; Read new register value (variable bit count)
	ldx _CurrentAYRegister
	lda _PlayerRegBits,x
	tax
	jsr _ReadBits
	ldx _DecodedResult
	stx _PlayerRegCurrentValue

	; Copy to stream
	lda _RegisterBufferHigh				; highpart of buffer adress + register number
	sta __auto_player_read_new+2

	ldx _BufferFrameOffset				; Value between 00 and 7f
	lda _PlayerRegCurrentValue			; New value to write
__auto_player_read_new:
	sta _PlayerBuffer,x

	inc _BufferFrameOffset
	jmp player_return




DecompressWithOffset:

	; Read Offset (0 to 127)
	ldx #7
	jsr _ReadBits					

	lda _RegisterBufferHigh			; highpart of buffer adress + register number
	sta __auto_write+2				; Write adress
	sta __auto_read+2				; Read adress

	; Compute wrap around offset...
	lda _BufferFrameOffset				; between 0 and 255
	clc
	adc _DecodedResult					; + Offset Between 00 and 7f
	sec
	sbc #128							; -128
	tay

	; Read count (7 bits)
	ldx #7
	jsr _ReadBits
	
	inc	_DecodedResult					; 1 to 129


	ldx _BufferFrameOffset
	
player_copy_offset_loop:

__auto_read:
	lda _PlayerBuffer,y				; Y for reading
	iny

__auto_write:
	sta _PlayerBuffer,x				; X for writing

	inx
	dec _DecodedResult
	bne player_copy_offset_loop 

	stx _BufferFrameOffset
	sta _PlayerRegCurrentValue

	jmp player_return




;
; Size in bits of each PSG register
;
_PlayerRegBits:
	; Chanel A Frequency
	.byt 8
	.byt 4

	; Chanel B Frequency
	.byt 8
	.byt 4 

	; Chanel C Frequency
	.byt 8
	.byt 4

	; Chanel sound generator
	.byt 5

	; select
	.byt 8 

	; Volume A,B,C
	.byt 5
	.byt 5
	.byt 5

	; Wave period
	.byt 8 
	.byt 8

	; Wave form
	.byt 8

_PlayerCount:
	.res 1,0 ; must be equal to 0
_MusicPlaying:
	.res 1,0 ; must be equal to 0
	

_PlayerRegValues:
_RegisterChanAFrequency:
	; Chanel A Frequency
	.res 1,8
	.res 1,4

_RegisterChanBFrequency:
	; Chanel B Frequency
	.res 1,8
	.res 1,4

_RegisterChanCFrequency:
	; Chanel C Frequency
	.res 1,8
	.res 1,4

_RegisterChanNoiseFrequency:
	; Chanel sound generator
	.res 1,5

	; select
	.res 1,8

	; Volume A,B,C
_RegisterChanAVolume:
	.res 1,5
_RegisterChanBVolume:
	.res 1,5
_RegisterChanCVolume:
	.res 1,5

	; Wave period
	.res 1,8
	.res 1,8

	; Wave form
	.res 1,8

_PlayerRegCurrentValue:
	.res 1,0
_DoNothing:
	rts	

_InterruptCallBack_3:		; Used by the music player
	jsr _DoNothing			; Transformed to "jsr _Mym_PlayFrame" -> 12 cycles
	
;	jsr MiniScrollLoading   ; -> 338 cycles

	pla
	tay
	pla
	tax
	pla

	rti
_PlayerBuffer:	
	.res 256*14			; About 3.5 kilobytes somewhere in memory, we put the music file in overlay memory
	
.endproc	

