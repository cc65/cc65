;
; Oliver Schmidt, 2012-11-17
;
; IRQ handling (Apple2 version)
;

        .export         initirq, doneirq
        .import         callirq, __dos_type, _exit

        .include        "apple2.inc"

        .macpack        apple2

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
