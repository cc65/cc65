;
; SwiftLink/Turbo-232 v0.90 device driver, by Craig Bruce, 14-Apr-1998.
;
; This software is Public Domain.  It is in Buddy assembler format.
;
; This device driver uses the SwiftLink RS-232 Serial Cartridge, available from
; Creative Micro Designs, Inc, and also supports the extensions of the Turbo232
; Serial Cartridge.  Both devices are based on the 6551 ACIA chip.  It also
; supports the "hacked" SwiftLink with a 1.8432 MHz crystal.
;
; The code assumes that the kernal + I/O are in context.  On the C128, call
; it from Bank 15.  On the C64, don't flip out the Kernal unless a suitable
; NMI catcher is put into the RAM under then Kernal.  For the SuperCPU, the
; interrupt handling assumes that the 65816 is in 6502-emulation mode.
;
;--------------------------------------------------------------------------
;
; Adapted for the use with the cc65 runtime library by
; Ullrich von Bassewitz (uz@musoftware.de) 02-May-1999.
;
; All external functions are C callable, the return value is an error code.
;


   	.importzp     	ptr1, ptr2, tmp1, tmp2
   	.importzp	acia, RecvBuf, SendBuf
   	.import	      	popa, popax
	.import		sys_bank, restore_bank
   	.export	       	_rs232_init, _rs232_params, _rs232_done, _rs232_get
   	.export	      	_rs232_put, _rs232_pause, _rs232_unpause, _rs232_status
	.export		k_rs232

   	.include	"zeropage.inc"


;----------------------------------------------------------------------------
;
; Global variables
;

.bss
DropCnt:     	.res  	4      	; Number of bytes lost from rx buffer full
Initialized: 	.res	1      	; Flag indicating driver is initialized
Stopped:     	.res   	1      	; Flow-stopped flag
RtsOff:		.res	1      	;
Errors:		.res	1      	; Number of bytes received in error, low byte
BaudCode:    	.res   	1      	; Current baud in effect

; Segment, the RS232 buffers are in
BufferSeg		= 2

; UART register offsets
RegData     		= 0    	; Data register
RegStatus   		= 1     ; Status register
RegCommand  		= 2     ; Command register
RegControl  		= 3     ; Control register

; Error codes. Beware: The codes must match the codes in the C header file
ErrNotInitialized 	= $01
ErrBaudTooFast    	= $02
ErrBaudNotAvail   	= $03
ErrNoData         	= $04
ErrOverflow       	= $05


.code

;----------------------------------------------------------------------------
;
; unsigned char __fastcall__ rs232_init (char hacked);
; /* Initialize the serial port, install the interrupt handler. The parameter
;  * must be true (non zero) for a hacked swiftlink and false (zero) otherwise.
;  */
;

_rs232_init:
       	bit 	Initialized    	;** shut down if started
       	bpl 	@L1
       	pha
       	jsr 	_rs232_done
       	pla

; Initialize buffers & control

@L1:	lda 	#0
   	sta 	RecvHead
      	sta 	SendHead
   	sta 	RecvTail
   	sta 	SendTail
   	sta 	Errors
   	sta 	Stopped
   	lda 	#255
       	sta    	RecvFreeCnt
      	sta 	SendFreeCnt

; Set default to 2400-8N1, enable interrupts

	jsr	sys_bank		; Switch indirect to system bank

	ldy	#RegData
   	lda	(acia),y
	ldy	#RegStatus
   	lda	(acia),y
      	lda 	#$18
	ldy	#RegControl
   	sta    	(acia),y

      	lda 	#$01
      	sta 	RtsOff
      	ora 	#$08
	ldy	#RegCommand
       	sta    	(acia),y
      	lda 	#$06
      	sta 	BaudCode

	jsr	restore_bank

      	lda 	#$ff
      	sta 	Initialized
      	lda	#$00
   	tax
      	rts

;----------------------------------------------------------------------------
;
; unsigned char __fastcall__ rs232_params (unsigned char params, unsigned char parity);
; /* Set the port parameters. Use a combination of the #defined values above. */
;
; Set communication parameters.
;
; baud rates              stops     word    |   parity
; ---------------------   -----     -----   |   ---------
; $00=50     $08=9600     $00=1     $00=8   |   $00=none
; $01=110    $09=19200    $80=2     $20=7   |   $20=odd
; $02=134.5  $0a=38400              $40=6   |   $60=even
; $03=300    $0b=57600              $60=5   |   $A0=mark
; $04=600    $0c=115200                     |   $E0=space
; $05=1200   $0d=230400
; $06=2400   $0e=future
; $07=4800   $0f=future
;

_rs232_params:
       	jsr	CheckInitialized	;** check initialized
       	bcc    	@L1
 	rts

; Save new parity

@L1:   	and    	#%11100000
       	ora 	#%00000001
       	sta 	tmp2

; Set baud/parameters

       	jsr	popa
       	sta	tmp1
       	and 	#$0f
       	tax
       	lda 	Bauds,x
       	cmp 	#$ff
       	bne 	@L5
       	lda 	#ErrBaudNotAvail
       	bne	@L9

@L5:   	jsr	sys_bank		; Indirect segment to system bank
       	tax
       	lda 	tmp1
       	and 	#$0f
       	sta 	BaudCode
       	lda 	tmp1
       	and 	#%11100000
       	ora 	#%00010000
       	sta 	tmp1
       	txa
       	and 	#$0f
       	ora 	tmp1
       	ldy	#RegControl
       	sta	(acia),y

; Set new parity

@L7:   	lda 	tmp2
       	sta 	RtsOff
       	ora 	#%00001000
       	ldy	#RegCommand
       	sta	(acia),y
       	jsr	restore_bank   		; Restore indirect bank
       	lda	#0
@L9:   	ldx	#0
       	rts

.rodata
Bauds:
   .byte $01,$03,$04,$06,$07,$08,$0a,$0c,$0e,$0f,$ff,$ff,$ff,$ff,$ff,$ff
     ;in:  0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
     ;baud50 110 134   3   6  12  24  48  96  19  38  57 115 230 exp exp
     ;out masks: $0F=Baud, val$FF=err
.code

;----------------------------------------------------------------------------
;
; unsigned char __fastcall__ rs232_done (void);
; /* Close the port, deinstall the interrupt hander. You MUST call this function
;  * before terminating the program, otherwise the machine may crash later. If
;  * in doubt, install an exit handler using atexit(). The function will do
;  * nothing, if it was already called.
;  */
;


_rs232_done:
     	bit 	Initialized  		;** check initialized
       	bpl 	@L9

; Stop interrupts, drop DTR

     	lda 	RtsOff
     	and 	#%11100010
     	ora 	#%00000010
	ldx	IndReg
	ldy	#$0F
	sty	IndReg			; Set indirect to system bank
	ldy	#RegCommand
     	sta    	(acia),y
	stx	IndReg			; Restore old indirect bank

; Flag uninitialized

@L9:   	lda 	#$00
     	sta 	Initialized
     	tax
     	rts

;----------------------------------------------------------------------------
;
; unsigned char __fastcall__ rs232_get (char* B);
; /* Get a character from the serial port. If no characters are available, the
;  * function will return RS_ERR_NO_DATA, so this is not a fatal error.
;  */
;

_rs232_get:
   	jsr 	CheckInitialized	; Check if initialized
   	bcc 	@L1
   	rts

; Check for bytes to send

@L1:  	sta	ptr1
 	stx	ptr1+1	    		; Store pointer to received char
	ldx 	SendFreeCnt
   	cpx 	#$ff
   	beq 	@L2
   	lda 	#$00
   	jsr    	TryToSend

; Check for buffer empty

@L2:  	lda 	RecvFreeCnt
   	cmp 	#$ff
   	bne 	@L3
   	lda	#ErrNoData
   	ldx 	#0
   	rts

; Check for flow stopped & enough free: release flow control

@L3:  	ldx 	Stopped
   	beq 	@L4
   	cmp 	#63
   	bcc 	@L4
   	lda 	#$00
   	sta 	Stopped
   	lda 	RtsOff
   	ora 	#%00001000
	ldx	IndReg
	ldy	#$0F  			; Set indirect to system bank
	sty	IndReg
	ldy	#RegCommand
   	sta	(acia),y
	stx	IndReg

; Get byte from buffer

@L4:	ldx	IndReg
	lda	#BufferSeg		; Set indirect to buffer bank
	sta	IndReg
	ldy	RecvHead
	lda	(RecvBuf),y
	stx	IndReg			; Restore indirect bank
      	inc 	RecvHead
   	inc 	RecvFreeCnt
       	ldx 	#$00
   	sta    	(ptr1,x)
       	txa		    		; Return code = 0
   	rts

;----------------------------------------------------------------------------
;
; unsigned char __fastcall__ rs232_put (char B);
; /* Send a character via the serial port. There is a transmit buffer, but
;  * transmitting is not done via interrupt. The function returns
;  * RS_ERR_OVERFLOW if there is no space left in the transmit buffer.
;  */
;

_rs232_put:
   	jsr 	CheckInitialized	; Check initialized
   	bcc 	@L1
   	rts

; Try to send

@L1:  	ldx 	SendFreeCnt
   	cpx 	#$ff
   	beq 	@L2
   	pha
   	lda 	#$00
   	jsr 	TryToSend
   	pla

; Put byte into send buffer & send

@L2:  	ldx 	SendFreeCnt
   	bne 	@L3
   	lda 	#ErrOverflow
   	ldx	#$00
   	rts
	  
; There is enough room (character still in A)

@L3:	ldx	IndReg
       	ldy    	#BufferSeg     		; Set indirect to buffer segment
	sty	IndReg
	ldy	SendTail
    	sta 	(SendBuf),y
	stx	IndReg			; Restore indirect bank
    	inc 	SendTail
    	dec 	SendFreeCnt
   	lda 	#$ff
   	jsr 	TryToSend
   	lda	#$00
   	tax
       	rts

;----------------------------------------------------------------------------
;
; unsigned char __fastcall__ rs232_pause (void);
; /* Assert flow control and disable interrupts. */
;

_rs232_pause:
; Check initialized
   	jsr 	CheckInitialized
   	bcc 	@L1
      	rts

; Assert flow control

@L1:	lda 	RtsOff
   	sta 	Stopped
	jsr	sys_bank    		; Set indirect to system bank
	ldy	#RegCommand
   	sta	(acia),y

; Delay for flow stop to be received

   	ldx 	BaudCode
   	lda 	PauseTimes,x
   	jsr 	DelayMs

; Stop rx interrupts

   	lda 	RtsOff
   	ora 	#$02
	ldy	#RegCommand
	sta	(acia),y
       	jsr    	restore_bank		; Restore indirect segment
   	lda	#0
   	tax
   	rts


.rodata
; Delay times: 32 byte-receive times in milliseconds, or 100 max.
; Formula = 320,000 / baud
PauseTimes:
       	.byte 100,100,100,100,100,100,100,067,034,017,009,006,003,002,001,001
          ;in:  0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
          ;baud50 110 134   3   6  12  24  48  96  19  38  57 115 230 exp exp
.code

;----------------------------------------------------------------------------
;
; unsigned char __fastcall__ rs232_unpause (void);
; /* Re-enable interrupts and release flow control */
;

_rs232_unpause:
; Check initialized
     	jsr 	CheckInitialized
   	bcc 	@L1
   	rts

; Re-enable rx interrupts & release flow control

@L1:  	lda 	#$00
    	sta 	Stopped
    	lda 	RtsOff
    	ora 	#%00001000
	ldx	IndReg
	ldy	#$0F
	sty	IndReg	       		; Set indirect to system bank
	ldy	#RegCommand
	sta	(acia),y
	stx	IndReg			; Restore indirect bank

; Poll for stalled char & exit

   	jsr 	PollReceive
   	lda	#0
   	tax
   	rts

;----------------------------------------------------------------------------
;
; unsigned char __fastcall__ rs232_status (unsigned char* status,
;					   unsigned char* errors);
; /* Return the serial port status. */
;

_rs232_status:
 	sta    	ptr2
 	stx    	ptr2+1
 	jsr    	popax
 	sta    	ptr1
 	stx    	ptr1+1
   	jsr    	CheckInitialized
       	bcs    	@L9

; Get status

	ldx	IndReg			; Save indirect segment
	lda	#$0F
	sta	IndReg			; Set system bank as indirect segment
	ldy	#RegStatus
	lda	(acia),y		; Read status register
	stx	IndReg
 	ldy    	#0
 	sta    	(ptr1),y
    	jsr    	PollReceive  		; bug-recovery hack
 	lda    	Errors
	ldy	#0
       	sta    	(ptr2),y
       	tya
 	tax
@L9:	rts

;----------------------------------------------------------------------------
;
; RS232 interrupt handler.
; The RS232 handler will be called with	the system bank as indirect bank
; and all registers saved.
;

k_rs232:
	ldy	#RegStatus
       	lda    	(acia),y       		; check for byte received
     	and 	#$08
     	beq 	@L9	       		; Nothing to receive
       	lda    	(acia),y       		; check for receive errors
    	and 	#$07
       	beq    	@L1
    	inc 	Errors
@L1:   	ldy	#RegData
	lda 	(acia),y       		; get byte and put into receive buffer
    	ldx 	RecvFreeCnt
      	beq 	@L3
	ldy	#BufferSeg
	sty	IndReg
       	ldy    	RecvTail
	sta	(RecvBuf),y		; Store received character
	lda	#$0F
	sta	IndReg			; Restore indirect segment
    	inc 	RecvTail
    	dec 	RecvFreeCnt
    	cpx 	#33            	       	; check for buffer space low
       	bcs    	@L9

; Assert flow control

@L2:  	lda 	RtsOff         	       	; assert flow control if buffer space too low
	ldy	#RegCommand
    	sta 	(acia),y
    	sta 	Stopped
	rts

; Drop this char

@L3:	inc 	DropCnt+0    		;not time-critical
    	bne 	@L9
    	inc 	DropCnt+1
    	bne 	@L9
    	inc 	DropCnt+2
    	bne 	@L9
    	inc 	DropCnt+3
@L9:   	rts


;----------------------------------------------------------------------------
;
; CheckInitialized  -  internal check if initialized
; Set carry and an error code if not initialized, clear carry and do not
; change any registers if initialized.
;

CheckInitialized:
      	bit 	Initialized
      	bmi 	@L1
      	lda 	#ErrNotInitialized
      	ldx	#0
      	sec
      	rts

@L1:  	clc
      	rts

;----------------------------------------------------------------------------
; Try to send a byte. Internal routine. A = TryHard

TryToSend:
   	sta 	tmp1  	       	; Remember tryHard flag
	ldx	IndReg		; Save indirect segment
	lda	#$0F
	sta	IndReg		; Set system segment as indirect segment
@L0:   	lda 	SendFreeCnt
    	cmp 	#$ff
    	beq 	@L3   	       	; Bail out

; Check for flow stopped

@L1:	lda 	Stopped
       	bne    	@L3   	       	; Bail out

; Check that the UART is ready to send

@L2:   	ldy	#RegStatus
  	lda	(acia),y
    	and 	#$10
    	bne 	@L4
    	bit 	tmp1  	       	; Keep trying if must try hard
       	bmi 	@L0
@L3:   	stx	IndReg		; Restore indirect segment
  	rts

; Send byte and try again

@L4:   	lda	#BufferSeg
	sta	IndReg
	ldy	SendHead
	lda	(SendBuf),y
	ldy	#$0F
	sty	IndReg
	ldy	#RegData
	sta	(acia),y
    	inc 	SendHead
    	inc 	SendFreeCnt
    	jmp 	@L0


;----------------------------------------------------------------------------
;
; PollReceive - poll for rx char
;   This function is useful in odd cases where the 6551 has a character in
;   it but it fails to raise an NMI.  It might be edge-triggering conditions?
;   Actually, I'm not entirely sure that this condition can still arrise, but
;   calling this function does no harm.
;

PollReceive:
	ldx	IndReg			; Save indirect segment
	lda	#$0F
	sta	IndReg			; Set system bank as indirect segment
	ldy	#RegStatus
	lda	(acia),y
      	and	#$08
      	beq 	@L9
	lda	(acia),y		; Read a second time? ###
	and	#$08
      	beq 	@L9
	ldy	#RegData
	lda	(acia),y
       	ldy 	RecvFreeCnt
      	beq 	@L9
	ldy	#BufferSeg
	sty	IndReg
       	ldy 	RecvTail
      	sta	(RecvBuf),y
      	inc 	RecvTail
      	dec 	RecvFreeCnt
@L9:  	stx	IndReg			; Restore indirect segment
	rts

;----------------------------------------------------------------------------
;
;  DelayMs : delay for given number of milliseconds
;    This implementation isn't very rigerous; it merely delays for the
;    approximate number of clock cycles for the processor speed.
;    Algorithm:
;       repeat for number of milliseconds:
;          repeat for number of MHz of cpu speed:
;             delay for 1017 clock cycles
;

DelayMs:			;( .A=milliseconds )
@L1:   	ldy    	#2		; 2MHz
@L2:   	ldx 	#203   		;(2)
@L3:   	dex 	       		;(2)
      	bne 	@L3		;(3) // 1017 cycles
      	dey
      	bne 	@L2
      	sec
      	sbc 	#1
      	bne 	@L1
      	rts

.end



