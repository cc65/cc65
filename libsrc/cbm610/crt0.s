;
; Startup code for cc65 (Plus/4 version)
;
; This must be the *first* file on the linker command line
;

	.export	     	_exit
	.import		initlib, donelib
	.import	     	push0, _main
	.import	     	initconio
	.import	       	__BSS_RUN__, __BSS_SIZE__
	.import		irq, nmi
       	.import	       	k_irq, k_nmi, k_plot, k_udtim, k_scnkey

	.include     	"zeropage.inc"
	.include    	"io.inc"


; ------------------------------------------------------------------------
; Define and export the ZP variables for the CBM610 runtime

	.exportzp	sp, sreg, regsave
	.exportzp	ptr1, ptr2, ptr3, ptr4
	.exportzp	tmp1, tmp2, tmp3, tmp4
	.exportzp	regbank, zpspace
      	.exportzp	crtc, sid, IPCcia, cia, acia, tpi1, tpi2
      	.exportzp	ktab1, ktab2, ktab3, ktab4, time, RecvBuf, SendBuf

.zeropage

zpstart	= *
sp:	      	.res   	2 	; Stack pointer
sreg:	      	.res	2	; Secondary register/high 16 bit for longs
regsave:      	.res	2	; slot to save/restore (E)AX into
ptr1:	      	.res	2
ptr2:	      	.res	2
ptr3:	      	.res	2
ptr4:	      	.res	2
tmp1:	      	.res	1
tmp2:	      	.res	1
tmp3:	      	.res	1
tmp4:	      	.res	1
regbank:      	.res	6	; 6 byte register bank

zpspace	= * - zpstart		; Zero page space allocated

.code

; ------------------------------------------------------------------------
; BASIC header and a small BASIC program. Since it is not possible to start
; programs in other banks using SYS, the BASIC program will write a small
; machine code program into memory at $100 and start that machine code
; program. The machine code program will then start the machine language
; code in bank 1, which will initialize the system by copying stuff from
; the system bank, and start the application.
;
; Here's the basic program that's in the following lines:
;
; 10 for i=0 to 4
; 20 read j
; 30 poke 256+i,j
; 40 next i
; 50 sys 256
; 60 data 120,169,1,133,0
;
; The machine program in the data lines is:
;
; sei
; lda 	  #$01
; sta     $00	       	<-- Switch to bank 1 after this command
;
; Initialization is not only complex because of the jumping from one bank
; into another. but also because we want to save memory, and because of
; this, we will use the system memory ($00-$3FF) for initialization stuff
; that is overwritten later.
;

; To make things more simple, make the code of this module absolute.

   	.org	$0001
Head:	.byte	$03,$00,$11,$00,$0a,$00,$81,$20,$49,$b2,$30,$20,$a4,$20,$34,$00
   	.byte	$19,$00,$14,$00,$87,$20,$4a,$00,$27,$00,$1e,$00,$97,$20,$32,$35
   	.byte	$36,$aa,$49,$2c,$4a,$00,$2f,$00,$28,$00,$82,$20,$49,$00,$39,$00
   	.byte	$32,$00,$9e,$20,$32,$35,$36,$00,$4f,$00,$3c,$00,$83,$20,$31,$32
   	.byte	$30,$2c,$31,$36,$39,$2c,$31,$2c,$31,$33,$33,$2c,$30,$00,$00,$00

; Since we need some vectors to access stuff in the system bank for our own,
; we will include them here, starting from $60:

      	.res	$60-*

crtc:  		.word  	$d800
sid:		.word  	$da00
IPCcia:		.word  	$db00
cia:   		.word  	$dc00
acia:  		.word  	$dd00
tpi1:		.word  	$de00
tpi2:  		.word  	$df00
ktab1:		.word  	$ea29
ktab2:	       	.word  	$ea89
ktab3:		.word	$eae9
ktab4:		.word  	$eb49
time:		.dword 	$0000
RecvBuf:	.word	$0100		; RS232 received buffer
SendBuf:	.word	$0200		; RS232 send buffer


; The code in the target bank when switching back will be put at the bottom
; of the stack. We will jump here to switch segments. The range $F2..$FF is
; not used by any kernal routine.

      	.res	$F8-*
Back:	ldx	spsave
   	txs
   	lda    	IndReg
   	sta	ExecReg

; The following code is a copy of the code that is poked in the system bank
; memory by the basic header program, it's only for documentation and not
; actually used here:

     	sei
     	lda	#$01
     	sta	ExecReg

; This is the actual starting point of our code after switching banks for
; startup. Beware: The following code will get overwritten as soon as we
; use the stack (since it's in page 1)!

      	tsx
       	stx	spsave 		; Save the system stackpointer
	ldx	#$FF
	txs	       		; Set up our own stack

; Set the interrupt, NMI and other vectors

      	ldy	#vectable_size
L0:   	lda	vectable-1,y
      	sta	$FF80,y
      	dey
       	bne    	L0

; Switch the indirect segment to the system bank

      	lda	#$0F
      	sta	IndReg

; Copy the kernal zero page ($90-$F2) from the system bank

   	lda  	#$90
   	sta	ptr1
	lda	#$00
   	sta	ptr1+1
   	ldy	#$62-1
L1:	lda	(ptr1),y
   	sta	$90,y
   	dey
   	bpl 	L1

; Copy the page 3 vectors in place

	ldy	#$00
L2:	lda	p3vectable,y
  	sta	$300,y
   	iny
  	cpy	#p3vectable_size
       	bne	L2

; Copy the rest of page 3 from the system bank

      	lda	#$00
      	sta	ptr1
	lda	#$03
	sta	ptr1+1
L3:	lda	(ptr1),y
   	sta	$300,y
   	iny
   	bne	L3

; Set the indirect segment to bank we're executing in

	lda	ExecReg
	sta	IndReg

; Zero the BSS segment. We will do that here instead calling the routine
; in the common library, since we have the memory anyway, and this way,
; it's reused later.

   	lda	#<__BSS_RUN__
   	sta	ptr1
	lda	#>__BSS_RUN__
   	sta	ptr1+1
	lda	#0
	tay

; Clear full pages

   	ldx	#>__BSS_SIZE__
   	beq	Z2
Z1:	sta	(ptr1),y
   	iny
   	bne	Z1
   	inc	ptr1+1			; Next page
   	dex
   	bne	Z1

; Clear the remaining page

Z2:	ldx	#<__BSS_SIZE__
   	beq	Z4
Z3:	sta	(ptr1),y
   	iny
   	dex
   	bne	Z3
Z4:

; Setup the C stack

	lda   	#<$FF81
	sta	sp
       	lda	#>$FF81
	sta	sp+1

; We expect to be in page 2 now

.if 	(* < $1FD)
	jmp	$200
   	.res	$200-*
.endif
.if	(* < $200)
     	.res	$200-*,$EA
.endif
.if    	(* >= $2F0)
.error	"Code range invalid"
.endif

; This code is in page 2, so we may now start calling subroutines safely,
; since the code we execute is no longer in the stack page.
; Call module constructors

	jsr	initlib

; Initialize conio stuff

	jsr	initconio

; Create the (empty) command line for the program

       	jsr    	push0 	       	; argc
       	jsr	push0	       	; argv

; Execute the program code

	jmp	Start

; ------------------------------------------------------------------------
; Additional data that we need for initialization and that's overwritten
; later

vectable:
      	jmp	$0000		; CINT
      	jmp	$0000		; IOINIT
      	jmp	$0000		; RAMTAS
      	jmp	$0000	      	; RESTOR
      	jmp	$0000	      	; VECTOR
      	jmp	$0000	      	; SETMSG
      	jmp	$0000  	      	; SECOND
      	jmp	$0000	      	; TKSA
      	jmp	$0000	      	; MEMTOP
      	jmp	$0000	      	; MEMBOT
      	jmp	k_scnkey	; SCNKEY
      	jmp	$0000	      	; SETTMO
      	jmp	$0000	      	; ACPTR
      	jmp	$0000	      	; CIOUT
      	jmp	$0000  	      	; UNTLK
      	jmp	$0000	      	; UNLSN
      	jmp	$0000	      	; LISTEN
      	jmp	$0000	      	; TALK
   	jmp	$0000	      	; READST
       	jmp    	k_setlfs	; SETLFS
       	jmp    	k_setnam 	; SETNAM
     	jmp	$0000	      	; OPEN
	jmp	$0000 	      	; CLOSE
	jmp	$0000	      	; CHKIN
	jmp	$0000	      	; CKOUT
	jmp	$0000	      	; CLRCH
	jmp	$0000	      	; BASIN
      	jmp	$0000	      	; BSOUT
	jmp	$0000	      	; LOAD
	jmp	$0000	      	; SAVE
	jmp	k_settim	; SETTIM
       	jmp    	k_rdtim		; RDTIM
	jmp	$0000	      	; STOP
   	jmp	$0000	      	; GETIN
	jmp	$0000	      	; CLALL
	jmp	k_udtim		; UDTIM
   	jmp	k_screen  	; SCREEN
	jmp	k_plot		; PLOT
	jmp	k_iobase	; IOBASE
   	sta	ExecReg
	rts
	.byte  	$01 	      	; Filler
       	.word	nmi
       	.word	0     	      	; Reset - not used
       	.word	irq
vectable_size	= * - vectable

p3vectable:
	.word	k_irq		; IRQ user vector
	.word	k_brk		; BRK user vector
	.word	k_nmi		; NMI user vector
p3vectable_size	= * - p3vectable


; ------------------------------------------------------------------------
; This is the program code after setup. It starts at $400

   	.res	$400-*

Start:

; Enable interrupts

      	cli

; Call the user code

       	ldy	#4    	       	; Argument size
       	jsr    	_main 		; call the users code

; Call module destructors. This is also the _exit entry.

_exit:	jsr	donelib		; Run module destructors

; Clear the start of the zero page, since it will be interpreted as a
; (garbage) BASIC program otherwise. This is also the default entry for
; the break vector.

k_brk:	sei
      	lda	#$00
      	ldx	#$3E
Clear:	sta	$02,x
      	dex
      	bne	Clear

; Setup the welcome code at the stack bottom in the system bank. Use
; the F4/F5 vector to access the system bank

      	lda	#$0F
      	sta	IndReg
      	ldy    	#$00
       	sty	$F4
      	iny
      	sty	$F5
   	ldy	#reset_size-1
@L1:	lda	reset,y
   	sta	($F4),y
   	dey
   	bne	@L1
   	jmp	Back

; ------------------------------------------------------------------------
; Code that is copied into the system bank at $100 when switching back

reset:	cli
	jmp	$8000		   	; BASIC cold start
reset_size = * - reset

; ------------------------------------------------------------------------
; Code for a few simpler kernal calls goes here

k_iobase:
	ldx	cia
	ldy	cia+1
	rts

k_screen:
   	ldx	#80		; Columns
	ldy	#25		; Lines
	rts

k_setlfs:
        sta     LogicalAdr
        stx     FirstAdr
        sty     SecondAdr
        rts

k_setnam:
        sta     FileNameLen
        lda     $00,x
        sta     FileNameAdrLo
        lda     $01,x
        sta     FileNameAdrHi
        lda     $02,x
        sta     FileNameAdrSeg
        rts

k_rdtim:
	sei
	lda	time+0
   	ldx	time+1
	ldy	time+2
	cli
	rts

k_settim:
	sei
	sta	time+0
   	stx	time+1
	sty	time+2
	cli
	rts

; -------------------------------------------------------------------------
; Data area - switch back to relocatable mode

	.reloc

.data
spsave:	.res	1


