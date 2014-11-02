;
; Zero page variables and I/O definitions for the CBM 610
;
; Taken from a kernal disassembly done by myself in 1987.
;
; 1998-09-28, Ullrich von Bassewitz
; 2014-04-02, Greg King


; ---------------------------------------------------------------------------
; Zeropage stuff

ExecReg         := $00          ; Controls execution memory bank
IndReg          := $01          ; Controls indirect indexed load-store bank

TXTPTR          := $85          ; Far pointer into BASIC source code
FNAM            := $90          ; Far pointer to LOAD/SAVE file-name
FNAM_LEN        := $9D          ; Holds length of file-name

; ---------------------------------------------------------------------------
; Screen size

XSIZE           = 80
YSIZE           = 25

; ---------------------------------------------------------------------------
; I/O definitions


; I/O  $d800: CRTC 6545

.struct CRTC
        ADDR    .byte
        DATA    .byte
.endstruct


; I/O  $db00: CIA 6526, Inter Process Communication
;
;       IPCcia          =       $db00

.struct CIA
        PRA     .byte
        PRB     .byte
        DDRA    .byte
        DDRB    .byte
        .union
            .struct
                TALO    .byte
                TAHI    .byte
            .endstruct
            TA          .word
        .endunion
        .union
            .struct
                TBLO    .byte
                TBHI    .byte
            .endstruct
            TB          .word
        .endunion
        TOD10   .byte
        TODSEC  .byte
        TODMIN  .byte
        TODHR   .byte
        SDR     .byte
        ICR     .byte
        CRA     .byte
        CRB     .byte
.endstruct


; I/O  $dc00: CIA 6526
;
;       cia             =       $dc00


; I/O  $dd00: ACIA 6551
;
;       acia            =       $dd00

.struct ACIA
        DATA    .byte
        STATUS  .byte
        CMD     .byte
        CTRL    .BYTE
.endstruct


; I/O  $de00: Triport #1 6525
;
;       tpi1            =       $de00

.struct TPI
        PRA     .byte
        PRB     .byte
        .union
                PRC     .byte
                INT     .byte
        .endunion
        DDRA    .byte
        DDRB    .byte
        .union
                DDRC    .byte
                IMR     .byte
        .endunion
        CR      .byte
        AIR     .byte
.endstruct


; I/O  $df00: Triport #2 6525

;       tpi2            =       $df00


;-----------------------------------------------------------------------------

BASIC_BUF       := $FA5E        ; Bank 1 location of command-line
BASIC_BUF_LEN   = 162           ; Maximum length of command-line
