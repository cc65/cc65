;
; Serial driver for the C64 using a Swiftlink or Turbo-232 cartridge.
;
; Ullrich von Bassewitz, 2003-04-18
;
; The driver is based on the cc65 rs232 module, which in turn is based on
; Craig Bruce device driver for the Switftlink/Turbo-232.
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

        .include        "zeropage.inc"
        .include        "ser-kernel.inc"
        .include        "ser-error.inc"
	.include	"c64.inc"


; ------------------------------------------------------------------------
; Header. Includes jump table

.segment        "JUMPTABLE"

; Driver signature

        .byte   $73, $65, $72           ; "ser"
        .byte   $00                     ; Serial API version number

; Jump table.

        .word   INSTALL
        .word   UNINSTALL
        .word   PARAMS
        .word   GET
        .word   PUT
        .word   PAUSE
	.word	UNPAUSE
        .word   STATUS
        .word   IOCTL

;----------------------------------------------------------------------------
; I/O definitions

ACIA   	        = $DE00
ACIA_DATA       = ACIA+0        ; Data register
ACIA_STATUS     = ACIA+1        ; Status register
ACIA_CMD        = ACIA+2        ; Command register
ACIA_CTRL       = ACIA+3        ; Control register
ACIA_CLOCK      = ACIA+7        ; Turbo232 external baud-rate generator

;----------------------------------------------------------------------------
;
; Global variables
;

.bss
DropCnt:     	.res  	4      	; Number of bytes lost from rx buffer full
Stopped:     	.res   	1      	; Flow-stopped flag
RtsOff:		.res	1      	;
Errors:		.res	1      	; Number of bytes received in error, low byte
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

.rodata

; Tables used to translate RS232 params into register values

BaudTable:                      ; bit7 = 1 means setting is invalid
        .byte   $FF             ; SER_BAUD_45_5
        .byte   $FF             ; SER_BAUD_50
        .byte   $FF             ; SER_BAUD_75
        .byte   $FF             ; SER_BAUD_110
        .byte   $FF             ; SER_BAUD_134_5
        .byte   $FF             ; SER_BAUD_150
        .byte   $05             ; SER_BAUD_300
        .byte   $06             ; SER_BAUD_600
        .byte   $07             ; SER_BAUD_1200
        .byte   $FF             ; SER_BAUD_1800
        .byte   $08             ; SER_BAUD_2400
        .byte   $FF             ; SER_BAUD_3600
        .byte   $0A             ; SER_BAUD_4800
        .byte   $FF             ; SER_BAUD_7200
        .byte   $0C             ; SER_BAUD_9600
        .byte   $0E             ; SER_BAUD_19200
        .byte   $0F             ; SER_BAUD_38400
        .byte   $FF             ; SER_BAUD_57600
        .byte   $FF             ; SER_BAUD_115200
        .byte   $FF             ; SER_BAUD_230400

BitTable:
        .byte   $60             ; SER_BITS_5
        .byte   $40             ; SER_BITS_6
        .byte   $20             ; SER_BITS_7
        .byte   $00             ; SER_BITS_8

StopTable:
        .byte   $00             ; SER_STOP_1
        .byte   $80             ; SER_STOP_2

ParityTable:
        .byte   $00             ; SER_PAR_NONE
        .byte   $20             ; SER_PAR_ODD
        .byte   $60             ; SER_PAR_EVEN
        .byte   $A0             ; SER_PAR_MARK
        .byte   $E0             ; SER_PAR_SPACE

HandshakeTable:                 ; bit7 = 1 means that setting is invalid
        .byte   $FF             ; SER_HS_NONE
        .byte   $00             ; SER_HS_HW
        .byte   $FF             ; SER_HS_SW

; Delay times: 32 byte-receive times in milliseconds, or 100 max.
; Formula = 320,000 / baud. Invalid values do contain $FF.
PauseTimes:
        .byte   $FF             ; SER_BAUD_45_5
        .byte   $FF             ; SER_BAUD_50
        .byte   $FF             ; SER_BAUD_75
        .byte   $FF             ; SER_BAUD_110
        .byte   $FF             ; SER_BAUD_134_5
        .byte   $FF             ; SER_BAUD_150
        .byte   100             ; SER_BAUD_300
        .byte   100             ; SER_BAUD_600
        .byte   100             ; SER_BAUD_1200
        .byte   $FF             ; SER_BAUD_1800
        .byte   100             ; SER_BAUD_2400
        .byte   $FF             ; SER_BAUD_3600
        .byte   67              ; SER_BAUD_4800
        .byte   $FF             ; SER_BAUD_7200
        .byte   34              ; SER_BAUD_9600
        .byte   17              ; SER_BAUD_19200
        .byte   9               ; SER_BAUD_38400
        .byte   $FF             ; SER_BAUD_57600
        .byte   $FF             ; SER_BAUD_115200
        .byte   $FF             ; SER_BAUD_230400


.code

;----------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present.
; Must return an SER_ERR_xx code in a/x.

INSTALL:

; Initialize buffers & control

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

; Set up nmi's

   	lda 	NMIVec
   	ldy 	NMIVec+1
   	sta 	NmiSave+0
   	sty 	NmiSave+1
   	lda 	#<NmiHandler
   	ldy 	#>NmiHandler
   	sta 	NMIVec
   	sty 	NMIVec+1

; Set default to 2400-8N1, enable interrupts

   	lda	ACIA_DATA
   	lda	ACIA_STATUS
      	lda 	#$18
        sta    	ACIA_CTRL

      	lda 	#$01
      	sta 	RtsOff
      	ora 	#$08
       	sta    	ACIA_CMD
      	lda 	#$06
      	sta 	BaudCode

; Done, return an error code

        lda     #SER_ERR_OK
        tax
      	rts

;----------------------------------------------------------------------------
; UNINSTALL routine. Is called before the driver is removed from memory.
; Must return an SER_ERR_xx code in a/x.

UNINSTALL:

; Stop interrupts, drop DTR

   	lda 	RtsOff
   	and 	#%11100010
   	ora 	#%00000010
   	sta	ACIA_CMD

; Restore NMI vector

   	lda 	NmiSave+0
      	ldy 	NmiSave+1
   	sta 	NMIVec
   	sty 	NMIVec+1

; Flag uninitialized

        lda 	#SER_ERR_OK
   	tax
   	rts

;----------------------------------------------------------------------------
; PARAMS routine. A pointer to a ser_params structure is passed in ptr1.
; Must return an SER_ERR_xx code in a/x.

PARAMS:
        ldy     #SER_PARAMS_BAUDRATE
        lda     (ptr1),y                ; Baudrate index
        tax
        lda     BaudTable,x             ; Get 6551 value
        bmi     invalid                 ; Branch if rate not supported
        sta     tmp1

        iny                             ; Databits
        lda     (ptr1),y
        tax
        lda     BitTable,x
        ora     tmp1
        sta     tmp1

        iny                             ; Stopbits
        lda     (ptr1),y
        tax
        lda     StopTable,x
        ora     tmp1
        sta     tmp1

        iny                             ; Parity
        lda     (ptr1),y
        tax
        lda     ParityTable,x
        ora     tmp1
        sta     tmp1

        iny                             ; Handshake
        lda     (ptr1),y
        tax
        lda     HandshakeTable,x
        bmi     invalid






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
        and 	#%11100000              ; Save new parity
   	ora 	#%00000001
    	sta 	tmp2

; Check cpu speed against baud rate

   	jsr	popa
       	sta	tmp1
   	and 	#$0f
   	cmp 	#$0c
   	bcc 	@L3

   	ldx 	CpuSpeed                ; ###
   	cpx 	#1+1
   	bcc 	@L2
   	cmp 	#$0c
   	beq 	@L3
   	cpx 	#4
   	bcs 	@L3
@L2:	lda 	#RS_ERR_BAUD_TOO_FAST
   	bne	@L9

; Set baud/parameters

@L3:	lda 	tmp1
   	and 	#$0f
   	tax
   	lda 	BaudTable,x
        cmp 	#$ff
   	bne 	@L5
   	lda 	#RS_ERR_BAUD_NOT_AVAIL
   	bne	@L9

@L5:  	tax
      	and 	#$30
       	beq 	@L6
    	bit 	Turbo232
    	bmi 	@L6
    	lda 	#RS_ERR_BAUD_NOT_AVAIL
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
    	sta	ACIA_CTRL
    	txa
    	and 	#%00110000
    	beq 	@L7
    	lsr
    	lsr
    	lsr
    	lsr
    	eor 	#%00000011
    	sta	ACIA_CLOCK

; Set new parity

@L7:  	lda 	tmp2
    	sta 	RtsOff
    	ora 	#%00001000
      	sta	ACIA_CMD
    	lda	#0
@L9:	ldx	#0
   	rts

.rodata

BaudTable:
   .byte $ff,$ff,$ff,$05,$06,$07,$08,$0a,$0c,$0e,$0f,$10,$20,$30,$ff,$ff
     ;in:  0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
     ;baud50 110 134   3   6  12  24  48  96  19  38  57 115 230 exp exp
     ;out masks: $0F=Baud, val$FF=err
     ;           $30=t232ExtBaud: $00=none, $10=57.6, $20=115.2, $30=230.4
.code

;----------------------------------------------------------------------------
; GET: Will fetch a character from the receive buffer and store it into the
; variable pointer to by ptr1. If no data is available, SER_ERR_NO_DATA is
; return.
;

GET:

; Check for bytes to send

	ldx 	SendFreeCnt
   	cpx 	#$ff
   	beq 	@L1
   	lda 	#$00
   	jsr    	TryToSend

; Check for buffer empty

@L1:  	lda 	RecvFreeCnt             ; (25)
   	cmp 	#$ff
   	bne 	@L2
   	lda	#SER_ERR_NO_DATA
   	ldx 	#0
   	rts

; Check for flow stopped & enough free: release flow control

@L2:  	ldx 	Stopped                 ; (34)
   	beq 	@L3
   	cmp 	#63
   	bcc 	@L3
   	lda 	#$00
   	sta 	Stopped
   	lda 	RtsOff
   	ora 	#%00001000
   	sta	ACIA_CMD

; Get byte from buffer

@L3:  	ldx 	RecvHead                ; (41)
       	lda 	RecvBuf,x
      	inc 	RecvHead
   	inc 	RecvFreeCnt
       	ldx 	#$00                    ; (59)
   	sta    	(ptr1,x)
       	txa				; Return code = 0
   	rts

;----------------------------------------------------------------------------
; PUT: Output character in A. 
; Must return an error code in a/x.
;

PUT:

; Try to send

        ldx 	SendFreeCnt
   	cpx 	#$ff
   	beq 	@L2
   	pha
   	lda 	#$00
   	jsr 	TryToSend
   	pla

; Put byte into send buffer & send

@L2:  	ldx 	SendFreeCnt
   	bne 	@L3
   	lda 	#<SER_ERR_OVERFLOW      ; X is already zero
   	rts

@L3:  	ldx 	SendTail
    	sta 	SendBuf,x
    	inc 	SendTail
    	dec 	SendFreeCnt
   	lda 	#$ff
   	jsr 	TryToSend
   	lda	#<SER_ERR_OK
   	tax
       	rts

;----------------------------------------------------------------------------
; PAUSE: Assert flow control and disable interrupts.
; Must return an error code in a/x.
;

PAUSE:

; Assert flow control

  	lda 	RtsOff
   	sta 	Stopped
   	sta	ACIA_CMD

; Delay for flow stop to be received

   	ldx 	BaudCode
   	ldy 	PauseTimes,x
   	jsr 	DelayMs

; Stop rx interrupts

   	lda 	RtsOff
   	ora 	#$02
   	sta	ACIA_CMD
   	lda	#<SER_ERR_OK
   	tax
   	rts


;----------------------------------------------------------------------------
; UNPAUSE: Re-enable interrupts and release flow control.
; Must return an error code in a/x.
;

UNPAUSE:

; Re-enable rx interrupts & release flow control

@L1:   	lda 	#$00
       	sta 	Stopped
       	lda 	RtsOff
       	ora 	#%00001000
       	sta	ACIA_CMD

; Poll for stalled char & exit

       	jsr 	PollReceive
       	lda	#<SER_ERR_OK
   	tax
   	rts

;----------------------------------------------------------------------------
; STATUS: Return the status in the variable pointed to by ptr1.
; Must return an error code in a/x.
;

STATUS:

; Get status

        lda    	ACIA_STATUS
 	ldy    	#0
 	sta    	(ptr1),y
    	jsr    	PollReceive  		; bug-recovery hack
 	tya                             ; SER_ERR_OK
 	tax
        rts

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
       	lda    	ACIA_STATUS             ;(4) ;status ;check for byte received
     	and 	#$08           		;(2)
       	beq    	@L9  			;(2*)
     	cld
     	txa
     	pha
     	tya
     	pha
       	lda    	ACIA_STATUS             ;(4) opt ;status ;check for receive errors
    	and 	#$07           		;(2) opt
       	beq    	@L1            		;(3*)opt
    	inc 	Errors       		;(5^)opt
@L1:	lda 	ACIA_DATA               ;(4) ;data  ;get byte and put into receive buffer
    	ldy 	RecvTail 		;(4)
    	ldx 	RecvFreeCnt  	       	;(4)
      	beq 	@L3  			;(2*)
    	sta 	RecvBuf,y 		;(5)
    	inc 	RecvTail             	;(6)
    	dec 	RecvFreeCnt          	;(6)
    	cpx 	#33            		;(2)  ;check for buffer space low
       	bcc    	@L2            		;(2*)
    	jmp 	NMIEXIT                 ;(3)

; Assert flow control

@L2:  	lda 	RtsOff       		;(3)  ;assert flow control if buffer space too low
    	sta 	ACIA_CMD	;(4) ;command
    	sta 	Stopped      		;(3)
    	jmp 	NMIEXIT      		;(3)

; Drop this char

@L3:	inc 	DropCnt+0    		; not time-critical
    	bne 	@L4
    	inc 	DropCnt+1
    	bne 	@L4
    	inc 	DropCnt+2
    	bne 	@L4
    	inc 	DropCnt+3
@L4:   	jmp 	NMIEXIT

@L9:	pla
	jmp 	NmiContinue

;----------------------------------------------------------------------------
; Try to send a byte. Internal routine. A = TryHard

.proc   TryToSend

   	sta 	tmp1	       	; Remember tryHard flag
@L0:   	lda 	SendFreeCnt
    	cmp 	#$ff
    	beq 	@L3	       	; Bail out

; Check for flow stopped

@L1:	lda 	Stopped
       	bne    	@L3	       	; Bail out

; Check that swiftlink is ready to send

@L2:   	lda	ACIA_STATUS
    	and 	#$10
    	bne 	@L4
    	bit 	tmp1	       	;keep trying if must try hard
       	bmi 	@L0
@L3:	rts

; Send byte and try again

@L4:  	ldx 	SendHead
    	lda 	SendBuf,x
    	sta	ACIA_DATA
    	inc 	SendHead
    	inc 	SendFreeCnt
    	jmp 	@L0

.endproc

;----------------------------------------------------------------------------
;
; PollReceive - poll for rx char
;   This function is useful in odd cases where the 6551 has a character in
;   it but it fails to raise an NMI.  It might be edge-triggering conditions?
;   Actually, I'm not entirely sure that this condition can still arrise, but
;   calling this function does no harm.
;

.proc   PollReceive

      	lda 	#$08
      	and    	ACIA_STATUS
      	beq 	@L9
       	and    	ACIA_STATUS
      	beq 	@L9
       	lda    	ACIA_DATA
      	ldx 	RecvFreeCnt
      	beq 	@L9
      	ldx 	RecvTail
      	sta 	RecvBuf,x
      	inc 	RecvTail
      	dec 	RecvFreeCnt
@L9:  	rts

.endproc

;----------------------------------------------------------------------------
;
;  DelayMs : delay for given number of milliseconds
;    This implementation isn't very rigerous; it merely delays for the
;    approximate number of clock cycles for the processor speed.
;    Algorithm:
;       repeat for number of milliseconds:
;         delay for 1017 clock cycles
;


.proc   DelayMs                 ;( .A=milliseconds )

@L1:   	ldx 	#203   		;(2)
@L2:   	dex 	       		;(2)
       	bne    	@L2		;(3) // 1017 cycles
      	dey
      	bne 	@L1
      	rts

.endproc


.end



