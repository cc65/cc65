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
	.import	      	popa, popax
	.export	       	_rs232_init, _rs232_params, _rs232_done, _rs232_get
	.export	      	_rs232_put, _rs232_pause, _rs232_unpause, _rs232_status

	.include	"c64.inc"


NmiExit = $febc     ;exit address for nmi

ACIA   	= $DE00



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
Turbo232:    	.res   	1      	; Flag indicating turbo-232
HackedFlag:  	.res   	1      	; Flag indicating hacked-crystal swiftlink
CpuSpeed:    	.res   	1      	; In MHz
RecvHead:    	.res	1      	; Head of receive buffer
RecvTail:	.res	1      	; Tail of receive buffer
RecvFreeCnt:	.res	1      	; Number of bytes in receive buffer
SendHead:    	.res   	1      	; Head of send buffer
SendTail:    	.res   	1      	; Tail of send buffer
SendFreeCnt: 	.res   	1      	; Number of bytes free in send buffer
BaudCode:    	.res   	1      	; Current baud in effect

; Send and receive buffers: 256 bytes each
RecvBuf:	.res	256
SendBuf:	.res	256

.data
NmiContinue: 	.byte  	$4c    	; JMP instruction for NMI save -- continue
NmiSave:     	.res   	2      	; normal NMI handler

; Switftlink register offsets
RegData     		= 0    	; Data register
RegStatus   		= 1     ; Status register
RegCommand  		= 2     ; Command register
RegControl  		= 3     ; Control register
RegClock    		= 7     ; Turbo232 external baud-rate generator

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

;** set hacked-crystal

@L1:  	sta 	HackedFlag

;** check for turbo-232

      	lda 	#$00
       	sta    	ACIA+RegControl
      	tax
       	lda    	ACIA+RegClock
      	beq 	@L3
      	dex
@L3:  	stx 	Turbo232

;** get C128/C64 cpu speed

      	lda 	#1
      	sta 	CpuSpeed

;** check for super-cpu at 20 MHz

      	bit 	$d0bc
   	bmi 	@L4
   	bit 	$d0b8
   	bvs 	@L4
   	lda 	#20
   	sta 	CpuSpeed

;** initialize buffers & control

@L4:	lda 	#0
   	sta 	RecvHead
      	sta 	SendHead
   	sta 	RecvTail
   	sta 	SendTail
   	sta 	Errors
   	sta 	Stopped
   	lda 	#255
       	sta    	RecvFreeCnt
      	sta 	SendFreeCnt

;** set up nmi's

   	lda 	NMIVec
   	ldy 	NMIVec+1
   	sta 	NmiSave+0
   	sty 	NmiSave+1
   	lda 	#<NmiHandler
   	ldy 	#>NmiHandler
   	sta 	NMIVec
   	sty 	NMIVec+1

;** set default to 2400-8N1, enable interrupts

	lda	ACIA+RegData
	lda	ACIA+RegStatus
      	lda 	#$18
      	bit 	HackedFlag
      	bpl 	@L5
      	lda 	#$1a
@L5:   	sta    	ACIA+RegControl

      	lda 	#$01
      	sta 	RtsOff
      	ora 	#$08
       	sta    	ACIA+RegCommand
      	lda 	#$06
      	sta 	BaudCode

;** return
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

@L1:	and 	#%11100000
   	ora 	#%00000001
    	sta 	tmp2

; Check cpu speed against baud rate

   	jsr	popa
       	sta	tmp1
   	and 	#$0f
   	cmp 	#$0c
   	bcc 	@L3
   	ldx 	CpuSpeed
   	cpx 	#1+1
   	bcc 	@L2
   	cmp 	#$0c
   	beq 	@L3
   	cpx 	#4
   	bcs 	@L3
@L2:	lda 	#ErrBaudTooFast
   	bne	@L9

; Set baud/parameters

@L3:	lda 	tmp1
   	and 	#$0f
   	tax
   	lda 	NormBauds,x
   	bit 	HackedFlag
   	bpl 	@L4
   	lda 	HackBauds,x
@L4:  	cmp 	#$ff
   	bne 	@L5
   	lda 	#ErrBaudNotAvail
   	bne	@L9

@L5:  	tax
      	and 	#$30
       	beq 	@L6
    	bit 	Turbo232
    	bmi 	@L6
    	lda 	#ErrBaudNotAvail
   	bne	@L9

@L6:  	lda 	tmp1
    	and 	#$0f
    	sta 	BaudCode
    	lda 	tmp1
    	and 	#%11100000
    	ora 	#%00010000
    	sta 	tmp1
    	txa
    	and 	#$0f
    	ora 	tmp1
    	sta	ACIA+RegControl
    	txa
    	and 	#%00110000
    	beq 	@L7
    	lsr
    	lsr
    	lsr
    	lsr
    	eor 	#%00000011
    	sta	ACIA+RegClock

; Set new parity

@L7:  	lda 	tmp2
    	sta 	RtsOff
    	ora 	#%00001000
      	sta	ACIA+RegCommand
    	lda	#0
@L9:	ldx	#0
   	rts

.rodata

NormBauds:
   .byte $ff,$ff,$ff,$05,$06,$07,$08,$0a,$0c,$0e,$0f,$10,$20,$30,$ff,$ff
HackBauds:
   .byte $01,$03,$04,$06,$07,$08,$0a,$0c,$0e,$0f,$ff,$ff,$00,$ff,$ff,$ff
     ;in:  0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
     ;baud50 110 134   3   6  12  24  48  96  19  38  57 115 230 exp exp
     ;out masks: $0F=Baud, val$FF=err
     ;           $30=t232ExtBaud: $00=none, $10=57.6, $20=115.2, $30=230.4
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
   	sta	ACIA+RegCommand

; Restore NMI vector

   	lda 	NmiSave+0
      	ldy 	NmiSave+1
   	sta 	NMIVec
   	sty 	NMIVec+1

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
 	stx	ptr1+1			; Store pointer to received char
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
   	sta	ACIA+RegCommand

; Get byte from buffer

@L4:  	ldx 	RecvHead
       	lda 	RecvBuf,x
      	inc 	RecvHead
   	inc 	RecvFreeCnt
       	ldx 	#$00
   	sta    	(ptr1,x)
       	txa				; Return code = 0
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

@L3:  	ldx 	SendTail
    	sta 	SendBuf,x
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
   	sta	ACIA+RegCommand

; Delay for flow stop to be received

   	ldx 	BaudCode
   	lda 	PauseTimes,x
   	jsr 	DelayMs

; Stop rx interrupts

   	lda 	RtsOff
   	ora 	#$02
   	sta	ACIA+RegCommand
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
   	sta	ACIA+RegCommand

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

 	lda    	ACIA+RegStatus
 	ldy    	#0
 	sta    	(ptr1),y
    	jsr    	PollReceive  		; bug-recovery hack
 	lda    	Errors
       	sta    	(ptr2),y
 	tya
 	tax
@L9:	rts

;----------------------------------------------------------------------------
;
; NMI handler
; C128 NMI overhead=76 cycles: int=7, maxLatency=6, ROMenter=33, ROMexit=30
; C64  NMI overhead=76 cycles: int=7, maxLatency=6, ROMenter=34, ROMexit=29
;
; timing: normal=76+43+9=128 cycles, assertFlow=76+52+9=137 cycles
;
; C128 @ 115.2k: 177 cycles avail (fast)
; C64  @  57.6k: 177 cycles avail, worstAvail=177-43? = 134
; SCPU @ 230.4k: 868 cycles avail: for a joke!
;

NmiHandler:
     	pha
       	lda    	ACIA+RegStatus       	;(4) ;status ;check for byte received
     	and 	#$08           		;(2)
       	beq    	@L9  			;(2*)
     	cld
     	txa
     	pha
     	tya
     	pha
       	lda    	ACIA+RegStatus       	;(4) opt ;status ;check for receive errors
    	and 	#$07           		;(2) opt
       	beq    	@L1            		;(3*)opt
    	inc 	Errors       		;(5^)opt
@L1:	lda 	ACIA+RegData		;(4) ;data  ;get byte and put into receive buffer
    	ldy 	RecvTail 		;(4)
    	ldx 	RecvFreeCnt  	       	;(4)
      	beq 	@L3  			;(2*)
    	sta 	RecvBuf,y 		;(5)
    	inc 	RecvTail             	;(6)
    	dec 	RecvFreeCnt          	;(6)
    	cpx 	#33            		;(2)  ;check for buffer space low
       	bcc    	@L2            		;(2*)
    	jmp 	NmiExit      		;(3)

; Assert flow control

@L2:  	lda 	RtsOff       		;(3)  ;assert flow control if buffer space too low
    	sta 	ACIA+RegCommand	;(4) ;command
    	sta 	Stopped      		;(3)
    	jmp 	NmiExit      		;(3)

; Drop this char

@L3:	inc 	DropCnt+0    		;not time-critical
    	bne 	@L4
    	inc 	DropCnt+1
    	bne 	@L4
    	inc 	DropCnt+2
    	bne 	@L4
    	inc 	DropCnt+3
@L4:   	jmp 	NmiExit

@L9:	pla
	jmp NmiContinue

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
   	sta 	tmp1	       	; Remember tryHard flag
@L0:   	lda 	SendFreeCnt
    	cmp 	#$ff
    	beq 	@L3	       	; Bail out

; Check for flow stopped

@L1:	lda 	Stopped
       	bne    	@L3	       	; Bail out

;** check that swiftlink is ready to send

@L2:   	lda	ACIA+RegStatus
    	and 	#$10
    	bne 	@L4
    	bit 	tmp1	       	;keep trying if must try hard
       	bmi 	@L0
@L3:	rts

;** send byte and try again

@L4:  	ldx 	SendHead
    	lda 	SendBuf,x
    	sta	ACIA+RegData
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
      	lda 	#$08
      	and    	ACIA+RegStatus
      	beq 	@L9
       	and    	ACIA+RegStatus
      	beq 	@L9
       	lda    	ACIA+RegData
      	ldx 	RecvFreeCnt
      	beq 	@L9
      	ldx 	RecvTail
      	sta 	RecvBuf,x
      	inc 	RecvTail
      	dec 	RecvFreeCnt
@L9:  	rts

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
@L1:  	ldy 	CpuSpeed
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



