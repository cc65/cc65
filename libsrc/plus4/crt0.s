;
; Startup code for cc65 (Plus/4 version)
;

        .export         _exit
        .export         brk_jmp
        .export         __STARTUP__ : absolute = 1      ; Mark as startup

        .import         callirq_y, initlib, donelib
        .import         callmain, zerobss
        .import         __INTERRUPTOR_COUNT__
        .import         __HIMEM__                       ; Linker generated
        .importzp       ST

        .include        "zeropage.inc"
        .include        "plus4.inc"


; ------------------------------------------------------------------------
; Constants

IRQInd          = $500  ; JMP $0000 - used as indirect IRQ vector

; ------------------------------------------------------------------------
; Startup code

.segment        "STARTUP"

Start:

; Save the zero-page locations that we need.

        sei                     ; No interrupts since we're banking out the ROM
        sta     ENABLE_RAM
        ldx     #zpspace-1
L1:     lda     sp,x
        sta     zpsave,x
        dex
        bpl     L1
        sta     ENABLE_ROM
        cli

; Switch to the second charset.

        lda     #14
        jsr     $FFD2           ; BSOUT

; Save some system stuff; and, set up the stack. The stack starts at the top
; of the usable RAM.

        tsx
        stx     spsave          ; Save system stk ptr

        lda     #<__HIMEM__
        ldx     #>__HIMEM__
        sta     sp
        stx     sp+1

; Set up the IRQ vector in the banked RAM; and, switch off the ROM.

        lda     #<IRQ
        ldx     #>IRQ
        sei                     ; No ints, handler not yet in place
        sta     ENABLE_RAM
        sta     $FFFE           ; Install interrupt handler
        stx     $FFFF
        lda     IRQVec
        ldx     IRQVec+1
        sta     IRQInd+1
        stx     IRQInd+2
        lda     #<IRQStub
        ldx     #>IRQStub
        sta     IRQVec
        stx     IRQVec+1

        cli                     ; Allow interrupts

; Clear the BSS data.

        jsr     zerobss

; Initialize irqcount, which means that, from now on, custom linked-in IRQ
; handlers will be called (via condes).

        lda     #.lobyte(__INTERRUPTOR_COUNT__*2)
        sta     irqcount

; Call the module constructors.

        jsr     initlib

; Push the command-line arguments; and, call main().

        jsr     callmain

; Back from main() [this is also the exit() entry]. Run the module destructors.

_exit:  pha                     ; Save the return code
        jsr     donelib         ; Run module destructors

; Disable the chained IRQ handlers.

        lda     #0
        sta     irqcount        ; Disable custom IRQ handlers

        sei
        ldx     IRQInd+1
        ldy     IRQInd+2
        stx     IRQVec
        sty     IRQVec+1
        cli

; Copy back the zero-page stuff.

        ldx     #zpspace-1
L2:     lda     zpsave,x
        sta     sp,x
        dex
        bpl     L2

; Place the program return code into BASIC's status variable.

        pla
        sta     ST

; Restore the stack pointer.

        ldx     spsave
        txs

; Enable the ROM; and, return to BASIC.

        sta     ENABLE_ROM
        rts

; ------------------------------------------------------------------------
; IRQ handler. The handler in the ROM enables the Kernal, and jumps to
; $CE00, where the ROM code checks for a BRK or IRQ, and branches via the
; indirect vectors at $314/$316.
;
; When RAM is banked in, we skip the whole part of the ROM handler, and jump to
; the indirect vectors directly, after calling our own interrupt handlers.
;
; When ROM is banked in, a stub installed in the $314 indirect vector ensures
; that our interrupt handlers are still called (otherwise, interrupts that are
; not serviced by the ROM handler may cause a deadlock).

.segment        "LOWCODE"

IRQ:    cld                     ; Just to be sure
        pha
        txa
        pha
        tya
        pha
        tsx                     ; Get the stack pointer
        lda     $0104,x         ; Get the saved status register
        and     #$10            ; Test for BRK bit
        bne     dobreak

; Since the ROM handler will end with an RTI, we have to fake an IRQ return
; on the stack, so that we get control of the CPU after the ROM handler,
; and can switch back to RAM.

@L1:    lda     #>irq_ret       ; Push new return address
        pha
        lda     #<irq_ret
        pha
        php                     ; Push faked IRQ frame on stack
        pha                     ; Push faked A register
        pha                     ; Push faked X register
        pha                     ; Push faked Y register
        sta     ENABLE_ROM      ; Switch to ROM
        jmp     (IRQVec)        ; Jump indirect to IRQ stub

irq_ret:
        sta     ENABLE_RAM      ; Switch back to RAM
        pla
        tay
        pla
        tax
        pla
        rti

dobreak:
        lda     brk_jmp+2       ; Check high byte of address
        beq     nohandler
        jmp     brk_jmp         ; Jump to the handler

; No break handler installed, jump to ROM.

nohandler:
        sta     ENABLE_ROM
        jmp     (BRKVec)        ; Jump indirect to the break vector


; IRQ stub installed at $314, called by our handler above if RAM is banked in,
; or the Kernal IRQ handler if ROM is banked in.

; If we have handlers, call them. We will use a flag here instead of loading
; __INTERRUPTOR_COUNT__ directly, since the condes function is not reentrant.
; The irqcount flag will be set/reset from the main code, to avoid races.
IRQStub:
        cld                     ; Just to be sure
        sta     ENABLE_RAM
        ldy     irqcount
        beq     @L1
        jsr     callirq_y       ; Call the IRQ functions
@L1:    sta     ENABLE_ROM
        jmp     (IRQInd+1)      ; Jump to the saved IRQ vector

; ------------------------------------------------------------------------
; Data

.data

; BRK handling
brk_jmp:        jmp     $0000

spsave:         .res    1

irqcount:       .byte   0

.segment        "INIT"

zpsave:         .res    zpspace
