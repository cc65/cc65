;
; Startup code for cc65 (C128 version)
;

        .export         _exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup
        .import         initlib, donelib
        .import         zerobss
        .import         push0, callmain
        .import         RESTOR, BSOUT, CLRCH
        .import         __MAIN_START__, __MAIN_SIZE__, __STACKSIZE__
        .importzp       ST

        .include        "zeropage.inc"
        .include        "c128.inc"


; ------------------------------------------------------------------------
; Startup code

.segment        "STARTUP"

Start:

; Switch to the second charset.

        lda     #14
        jsr     BSOUT

; Before doing anything else, we have to set up our banking configuration.
; Otherwise, just the lowest 16K are actually RAM. Writing through the ROM
; to the underlying RAM works; but, it is bad style.

        lda     MMU_CR          ; Get current memory configuration...
        pha                     ; ...and save it for later
        lda     #MMU_CFG_CC65   ; Bank0 with Kernal ROM
        sta     MMU_CR

; Save the zero-page locations that we need.

        ldx     #zpspace-1
L1:     lda     c_sp,x
        sta     zpsave,x
        dex
        bpl     L1

; Clear the BSS data.

        jsr     zerobss

; Save some system stuff; and, set up the stack.

        pla                     ; Get MMU setting
        sta     mmusave

        tsx
        stx     spsave          ; Save the system stack pointer

        lda     #<(__MAIN_START__ + __MAIN_SIZE__ + __STACKSIZE__)
        ldx     #>(__MAIN_START__ + __MAIN_SIZE__ + __STACKSIZE__)
        sta     c_sp
        stx     c_sp+1          ; Set argument stack ptr

; Call the module constructors.

        jsr     initlib

; Disable the BASIC part of the IRQ handler. It would usually (once per frame)
; copy the VIC shadow register, move sprites, play music. This would only get
; in the way, so we turn it off.

        lda     INIT_STATUS
        sta     initsave
        and     #$fe
        sta     INIT_STATUS

; Set the bank for the file name to our execution bank. We must do this
; *after* calling the constructors because some of them might depend on
; the original value of this register.

        lda     #0
        sta     FNAM_BANK

; Push the command-line arguments; and, call main().

        jsr     callmain

; Back from main() [this is also the exit() entry]. Run the module destructors.

_exit:  pha                     ; Save the return code on stack
        jsr     donelib

; Copy back the zero-page stuff.

        ldx     #zpspace-1
L2:     lda     zpsave,x
        sta     c_sp,x
        dex
        bpl     L2

; Enable the BASIC interrupt again

        lda     initsave
        sta     INIT_STATUS

; Place the program return code into BASIC's status variable.

        pla
        sta     ST

; Reset the stack and the memory configuration.

        ldx     spsave
        txs
        ldx     mmusave
        stx     MMU_CR

; Done, return to BASIC.

        rts

; ------------------------------------------------------------------------
; Data

.segment        "INIT"

zpsave: .res    zpspace

; ------------------------------------------------------------------------

.bss

spsave:   .res    1
mmusave:  .res    1
initsave: .res    1
