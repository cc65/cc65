;
; Oliver Schmidt, 2012-11-17
;
; IRQ handling (Apple2 version)
;

        .export         initirq, doneirq
        .import         callirq, __dos_type, _exit

        .include        "apple2.inc"

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
        .byte   $8D,     't'|$80, 'p'|$80, 'u'|$80, 'r'|$80, 'r'|$80
        .byte   'e'|$80, 't'|$80, 'n'|$80, 'i'|$80, ' '|$80, 'c'|$80
        .byte   'o'|$80, 'l'|$80, 'l'|$80, 'a'|$80, ' '|$80, 'o'|$80
        .byte   't'|$80, ' '|$80, 'd'|$80, 'e'|$80, 'l'|$80, 'i'|$80
        .byte   'a'|$80, 'F'|$80, $8D
        .else
        .byte   $8D,     'T'|$80, 'P'|$80, 'U'|$80, 'R'|$80, 'R'|$80
        .byte   'E'|$80, 'T'|$80, 'N'|$80, 'I'|$80, ' '|$80, 'C'|$80
        .byte   'O'|$80, 'L'|$80, 'L'|$80, 'A'|$80, ' '|$80, 'O'|$80
        .byte   'T'|$80, ' '|$80, 'D'|$80, 'E'|$80, 'L'|$80, 'I'|$80
        .byte   'A'|$80, 'F'|$80, $8D
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
