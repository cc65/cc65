;
; Startup code for cc65 (CBM 600/700 version)
;

        .export         _exit, BRKVec
        .export         __STARTUP__ : absolute = 1      ; Mark as startup

        .import         callirq_y, initlib, donelib
        .import         push0, callmain
        .import         __BSS_RUN__, __BSS_SIZE__, __EXTZP_RUN__
        .import         __INTERRUPTOR_COUNT__
        .import         scnkey, UDTIM

        .include        "zeropage.inc"
        .include        "extzp.inc"
        .include        "cbm610.inc"


; ------------------------------------------------------------------------
; The BASIC header and a small BASIC program. Since it isn't possible to start
; programs in other banks using SYS, the BASIC program will write a small
; machine code program into memory at $100; and, start that machine code
; program. The machine code program will then start the machine language
; code in bank 1, which will initialize the system by copying stuff from
; the system bank, and start the application.
;
; Here's the BASIC program that's in the following lines:
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
; lda     #$01
; sta     $00           <-- Switch to bank 1 after this command
;
; Initialization is complex not only because of the jumping from one bank
; into another. but also because we want to save memory; and because of
; that, we will use the system memory ($00-$3FF) for initialization stuff
; that is overwritten later.
;

.segment        "EXEHDR"

        .byte   $03,$00,$11,$00,$0a,$00,$81,$20,$49,$b2,$30,$20,$a4,$20,$34,$00
        .byte   $19,$00,$14,$00,$87,$20,$4a,$00,$27,$00,$1e,$00,$97,$20,$32,$35
        .byte   $36,$aa,$49,$2c,$4a,$00,$2f,$00,$28,$00,$82,$20,$49,$00,$39,$00
        .byte   $32,$00,$9e,$20,$32,$35,$36,$00,$4f,$00,$3c,$00,$83,$20,$31,$32
        .byte   $30,$2c,$31,$36,$39,$2c,$31,$2c,$31,$33,$33,$2c,$30,$00,$00,$00

;------------------------------------------------------------------------------
; A table that contains values that must be transferred from the system zero-
; page into our zero-page. Contains pairs of bytes, first one is the address
; in the system ZP, second one is our ZP address. The table goes into page 2;
; but, is declared here because it is needed earlier.

.SEGMENT        "PAGE2"

; (We use .proc because we need both a label and a scope.)

.proc   transfer_table

        .byte   $9F, DEVNUM
        .byte   $CA, CURS_Y
        .byte   $CB, CURS_X
        .byte   $CC, graphmode
        .byte   $D4, config

.endproc


;------------------------------------------------------------------------------
; Page 3 data. This page contains the break vector and the bankswitch
; subroutine that is copied into high memory on startup. The space occupied by
; this routine will later be used for a copy of the bank 15 stack. It must be
; saved since we're going to destroy it when calling bank 15.

.segment        "PAGE3"

BRKVec: .addr   _exit           ; BRK indirect vector

.proc   callbank15

        excrts  := $FF05        ; In bank 15 ROM

.org    $FECB

entry:  php
        pha
        lda     #$0F            ; Bank 15
        sta     IndReg
        txa
        pha
        tya
        pha
        sei
        ldy     #$FF
        lda     (sysp1),y
        tay
        lda     ExecReg
        sta     (sysp1),y
        dey

        lda     #.hibyte(excrts-1)
        sta     (sysp1),y
        dey
        lda     #.lobyte(excrts-1)
        sta     (sysp1),y

        tya
        sec
        sbc     #7
        sta     $1FF            ; Save new sp
        tay

        tsx

        pla
        iny
        sta     (sysp1),y
        pla
        iny
        sta     (sysp1),y
        pla
        iny
        sta     (sysp1),y
        pla
        iny
        sta     (sysp1),y

        lda     $105,x
        sec
        sbc     #3
        iny
        sta     (sysp1),y
        lda     $106,x
        sbc     #0
        iny
        sta     (sysp1),y

        ldy     $1FF            ; Restore sp in bank 15

        lda     #.hibyte(expull-1)
        sta     (sysp1),y
        dey
        lda     #.lobyte(expull-1)
        sta     (sysp1),y
        dey
        pla
        pla
        tsx
        stx     $1FF
        tya
        tax
        txs
        lda     IndReg
        jmp     $FFF6

expull: pla
        tay
        pla
        tax
        pla
        plp
        rts

.if (expull <> $FF2E)
.error "Symbol expull must be aligned with Kernal in bank 15"
.endif

.reloc

.endproc

;------------------------------------------------------------------------------
; The code in the target bank when switching back will be put at the bottom
; of the stack. We will jump here to switch segments. The range $F2..$FF is
; not used by any Kernal routine.

.segment        "STARTUP"

Back:   sta     ExecReg

; We are at $100 now. The following snippet is a copy of the code that is poked
; in the system bank memory by the BASIC header program; it's only for
; documentation, and not actually used here:

        sei
        lda     #$01
        sta     ExecReg

; This is the actual starting point of our code after switching banks for
; startup. Beware: The following code will get overwritten as soon as we
; use the stack (since it's in page 1)! We jump to another location since
; we need some space for subroutines that aren't used later.

        jmp     Origin

; Hardware vectors, copied to $FFF6

.proc   vectors
        sta     ExecReg
        rts
        nop
        .word   nmi             ; NMI vector
        .word   0               ; Reset -- not used
        .word   irq             ; IRQ vector
.endproc

; Initializers for the extended zero-page. See "extzp.s".

.proc   extzp
        .word   $0100           ; sysp1
        .word   $0300           ; sysp3
        .word   $d800           ; crtc
        .word   $da00           ; sid
        .word   $db00           ; ipccia
        .word   $dc00           ; cia
        .word   $dd00           ; acia
        .word   $de00           ; tpi1
        .word   $df00           ; tpi2
        .word   $ea29           ; ktab1
        .word   $ea89           ; ktab2
        .word   $eae9           ; ktab3
        .word   $eb49           ; ktab4
.endproc

; Switch the indirect segment to the system bank.

Origin: lda     #$0F
        sta     IndReg

; Initialize the extended zero-page.

        ldx     #.sizeof(extzp)-1
L1:     lda     extzp,x
        sta     <__EXTZP_RUN__,x
        dex
        bpl     L1

; Save the old stack pointer from the system bank; and, set up our hw sp.

        tsx
        txa
        ldy     #$FF
        sta     (sysp1),y       ; Save system stack point into $F:$1FF
        ldx     #$FE            ; Leave $1FF untouched for cross-bank calls
        txs                     ; Set up our own stack

; Copy stuff from the system zero-page to ours.

        lda     #.sizeof(transfer_table)
        sta     ktmp
L2:     ldx     ktmp
        ldy     transfer_table-2,x
        lda     transfer_table-1,x
        tax
        lda     (sysp0),y
        sta     $00,x
        dec     ktmp
        dec     ktmp
        bne     L2

; Set the interrupt, NMI, and other vectors.

        ldx     #.sizeof(vectors)-1
L3:     lda     vectors,x
        sta     $10000 - .sizeof(vectors),x
        dex
        bpl     L3

; Set up the C stack.

        lda     #.lobyte(callbank15::entry)
        sta     sp
        lda     #.hibyte(callbank15::entry)
        sta     sp+1

; Set up the subroutine and jump vector table that redirects Kernal calls to
; the system bank.

        ldy     #.sizeof(callbank15)
@L1:    lda     callbank15-1,y
        sta     callbank15::entry-1,y
        dey
        bne     @L1

; Set up the jump vector table. Y is zero on entry.

        ldx     #45-1           ; Number of vectors
@L2:    lda     #$20            ; JSR opcode
        sta     $FF6F,y
        iny
        lda     #.lobyte(callbank15::entry)
        sta     $FF6F,y
        iny
        lda     #.hibyte(callbank15::entry)
        sta     $FF6F,y
        iny
        dex
        bpl     @L2

; Set the indirect segment to the bank that we're executing in.

        lda     ExecReg
        sta     IndReg

; Zero the BSS segment. We will do that here instead of calling the routine
; in the common library, since we have the memory anyway; and this way,
; it's reused later.

        lda     #<__BSS_RUN__
        sta     ptr1
        lda     #>__BSS_RUN__
        sta     ptr1+1
        lda     #0
        tay

; Clear full pages.

        ldx     #>__BSS_SIZE__
        beq     Z2
Z1:     sta     (ptr1),y
        iny
        bne     Z1
        inc     ptr1+1          ; Next page
        dex
        bne     Z1

; Clear the remaining page.

Z2:     ldx     #<__BSS_SIZE__
        beq     Z4
Z3:     sta     (ptr1),y
        iny
        dex
        bne     Z3
Z4:     jmp     Init

; ------------------------------------------------------------------------
; We are at $200 now. We may now start calling subroutines safely since
; the code we execute is no longer in the stack page.

.segment        "PAGE2"

; Activate the chained interrupt handlers; then, enable interrupts.

Init:   lda     #.lobyte(__INTERRUPTOR_COUNT__*2)
        sta     irqcount
        cli

; Call module constructors.

        jsr     initlib

; Push the command-line arguments; and, call main().

        jsr     callmain

; Call the module destructors. This is also the exit() entry and the default entry
; point for the break vector.

_exit:  pha                     ; Save the return code
        jsr     donelib         ; Run module destructors
        lda     #$00
        sta     irqcount        ; Disable custom irq handlers

; Address the system bank.

        lda     #$0F
        sta     IndReg

; Copy stuff back from our zero-page to the system's.

.if 0
        lda     #.sizeof(transfer_table)
        sta     ktmp
@L0:    ldx     ktmp
        ldy     transfer_table-2,x
        lda     transfer_table-1,x
        tax
        lda     $00,x
        sta     (sysp0),y
        dec     ktmp
        dec     ktmp
        bne     @L0
.endif

; Place the program return code into BASIC's status variable.

        pla
        ldy     #$9C            ; ST
        sta     (sysp0),y

; Set up the welcome code at the stack bottom in the system bank.

        ldy     #$FF
        lda     (sysp1),y       ; Load system bank sp
        tax
        iny                     ; Y = 0
        lda     #$58            ; CLI opcode
        sta     (sysp1),y
        iny
        lda     #$60            ; RTS opcode
        sta     (sysp1),y
        lda     IndReg
        sei
        txs
        jmp     Back

; -------------------------------------------------------------------------
; The IRQ handler goes into PAGE2. For performance reasons, and to allow
; easier chaining, we do handle the IRQs in the execution bank (instead of
; passing them to the system bank).

; This is the mapping of the active IRQ register of the 6525 (tpi1):
;
; Bit   7       6       5       4       3       2       1       0
;                               |       |       |       |       ^ 50 Hz.
;                               |       |       |       ^ SRQ IEEE 488
;                               |       |       ^ CIA
;                               |       ^ IRQB ext. Port
;                               ^ ACIA

irq:    pha
        txa
        pha
        tya
        pha
        lda     IndReg
        pha
        lda     ExecReg
        sta     IndReg          ; Be sure to address our segment
        tsx
        lda     $105,x          ; Get the flags from the stack
        and     #$10            ; Test break flag
        bne     dobrk

; It's an IRQ.

        cld

; Call the chained IRQ handlers.

        ldy     irqcount
        beq     irqskip
        jsr     callirq_y       ; Call the functions

; Done with the chained IRQ handlers; check the TPI for IRQs, and handle them.

irqskip:lda     #$0F
        sta     IndReg
        ldy     #TPI::AIR
        lda     (tpi1),y        ; Interrupt Register 6525
        beq     noirq

; 50/60Hz. interrupt

        cmp     #%00000001      ; ticker IRQ?
        bne     irqend
        jsr     scnkey          ; Poll the keyboard
        jsr     UDTIM           ; Bump the time

; Done.

irqend: ldy     #TPI::AIR
        sta     (tpi1),y        ; Clear interrupt

noirq:  pla
        sta     IndReg
        pla
        tay
        pla
        tax
        pla
nmi:    rti

dobrk:  jmp     (BRKVec)

; -------------------------------------------------------------------------
; Data area

.bss
irqcount:       .byte   0
