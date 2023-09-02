;
; Oliver Schmidt, 2012-11-17
; Colin Leroy-Mira, 2023-09-02 - Handle IRQs without ProDOS
;
; IRQ handling (Apple2 version)
;

        .export         initirq, doneirq
        .import         callirq, __dos_type, _exit

        .include        "apple2.inc"

        .macpack        apple2

; ------------------------------------------------------------------------

.ifdef __APPLE2ENH__
; Faster IRQ handler for enhanced Apple2

        .segment       "DATA"

_prev_rom_irq_vector:   .res 2
_prev_ram_irq_vector:   .res 2
_a_backup:              .res 1

        .segment        "ONCE"

initirq:
        ; Disable IRQs
        sei

        ; Save previous ROM IRQ vector
        lda     ROMIRQVEC
        ldx     ROMIRQVEC+1
        sta     _prev_rom_irq_vector
        stx     _prev_rom_irq_vector+1

        ; Update ROM IRQ vector
        lda     #<handle_rom_irq
        ldx     #>handle_rom_irq
        sta     ROMIRQVEC
        stx     ROMIRQVEC+1

        ; Check for Apple IIgs
        bit     ROMIN
        lda     $FE1F
        cmp     #$60
        ; Keep standard IRQ vector at FFFE/FFFF,
        ; IIgs uses this one even from ROM
        ; so we need it standard
        bne     :+

        ; Switch to RAM
        bit     LCBANK2
        bit     LCBANK2

        ; Save previous RAM IRQ vector
        lda     RAMIRQVEC
        ldx     RAMIRQVEC+1
        sta     _prev_ram_irq_vector
        stx     _prev_ram_irq_vector+1

        ; And update it
        lda     #<handle_ram_irq
        ldx     #>handle_ram_irq
        sta     RAMIRQVEC
        stx     RAMIRQVEC+1

        ; Enable IRQs
:       cli
        rts

; ------------------------------------------------------------------------

.code

doneirq:
        ; Restore ROM IRQ vector
        lda     _prev_rom_irq_vector
        ldx     _prev_rom_irq_vector+1
        sta     ROMIRQVEC
        stx     ROMIRQVEC+1

        ; Check for Apple IIgs
        lda     $FE1F
        cmp     #$60
        bne     :+

        ; Switch to RAM
        bit     LCBANK2
        bit     LCBANK2

        ; Same for RAM
        lda     _prev_ram_irq_vector
        ldx     _prev_ram_irq_vector+1
        sta     RAMIRQVEC
        stx     RAMIRQVEC+1

        ; And back to ROM, we're exiting
        bit     ROMIN
:       rts

; ------------------------------------------------------------------------

.segment        "LOWCODE"

handle_ram_irq:
        ; Check for BRK
        sta     _a_backup
        pla
        pha
        asl     a
        asl     a
        asl     a
        bpl     :+
        ; Give BRK to the standard handler
        jmp     (_prev_ram_irq_vector)

        ; It's an IRQ
:       lda     _a_backup
        pha                     ; Save A,X,Y
        phx
        phy
        jsr     callirq
        ply                     ; Restore Y,X,A
        plx
        pla
        rti

handle_rom_irq:                 ; ROM saves things for us
        jsr     callirq
        rti

.else
; Slower, ProDOS-based IRQ handler for unenhanced Apple2 that have
; firmware quirks

        .segment        "ONCE"

initirq:
        ; Check for ProDOS
        lda     __dos_type
        beq     prterr

        ; Allocate interrupt vector table entry
        jsr     $BF00           ; MLI call entry point
        .byte   $40             ; Alloc interrupt
        .addr   i_param
        bcs     prterr

        ; Enable interrupts, as old ProDOS versions (i.e. 1.1.1)
        ; jump to SYS and BIN programs with interrupts disabled.
        cli
        rts

        ; Print error message and exit
prterr: ldx     #msglen-1
:       lda     errmsg,x
        jsr     $FDED           ; COUT
        dex
        bpl     :-
        jmp     _exit

errmsg: .ifdef  __APPLE2ENH__
        scrcode $0D, "tpurretni colla ot deliaF", $0D
        .else
        scrcode $0D, "TPURRETNI COLLA OT DELIAF", $0D
        .endif

msglen = * - errmsg

        .code

doneirq:
        ; Check for valid interrupt vector table entry number which
        ; IS necessary as this gets called even if initirq failed.
        lda     int_num
        beq     :+

        ; Deallocate interrupt vector table entry
        dec     i_param         ; Adjust parameter count
        jsr     $BF00           ; MLI call entry point
        .byte   $41             ; Dealloc interrupt
        .addr   i_param
:       rts

        .segment        "LOWCODE"

intptr:
        ; ProDOS TechRefMan, chapter 6.2:
        ; "Each installed routine must begin with a CLD instruction."
        cld

        ; Call interruptors and check for success
        jsr     callirq
        bcc     :+

        ; ProDOS TechRefMan, chapter 6.2:
        ; "When the routine that can process the interrupt is called, it
        ;  should ... return (via an RTS) with the carry flag clear."
        clc
        rts

        ; ProDOS TechRefMan, chapter 6.2:
        ; "When a routine that cannot process the interrupt is called,
        ;  it should return (via an RTS) with the cary flag set ..."
:       sec
        rts

        .data

        ; MLI parameter list for (de)alloc interrupt
i_param:.byte   $02             ; param_count
int_num:.byte   $00             ; int_num
        .addr   intptr          ; int_code

.endif
