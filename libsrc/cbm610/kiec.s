;
; Ullrich von Bassewitz, 2003-12-18
;
; IEC bus routines for the 610.
;

        .export         TALK, LISTEN, SECOND, TKSA, CIOUT, UNTLK, UNLSN, ACPTR

        .import         UPDST
	.import         tpi1: zp, cia: zp

	.include      	"cbm610.inc"


; -------------------------------------------------------------------------
; TALK subroutine

TALK:   ora     #$40
        bne     talk_listen

; -------------------------------------------------------------------------
; LISTEN subroutine

LISTEN: ora     #$20

talk_listen:
        pha
        lda     #$3F
        ldy     #TPI::DDRA
        sta     (tpi1),y
        lda     #$FF
        ldy     #CIA::PRA
        sta     (cia),y
        ldy     #CIA::DDRA
        sta     (cia),y
        lda     #$FA
        ldy     #TPI::PRA
        sta     (tpi1),y
        lda     CTemp
        bpl     LF268
        lda     (tpi1),y
        and     #$DF
        sta     (tpi1),y
        lda     snsw1
        jsr     transfer_byte
        lda     CTemp
        and     #$7F
        sta     CTemp
        ldy     #TPI::PRA
        lda     (tpi1),y
        ora     #$20
        sta     (tpi1),y

LF268:  lda     (tpi1),y                ; TPI::PRA
        and     #$F7
        sta     (tpi1),y
        pla
;       jmp     transfer_byte

; -------------------------------------------------------------------------
; Output A (without EOF flag)

transfer_byte:
        eor     #$FF
        ldy     #CIA::PRA
        sta     (cia),y
        ldy     #TPI::PRA
        lda     (tpi1),y
        ora     #$12
        sta     (tpi1),y
        lda     (tpi1),y
        and     #%11000000
        beq     LF2D4
        lda     #$80
        jsr     UPDST
        bne     LF304                   ; Branch always

; Wait until NRFD is high

LF2D4:  lda     (tpi1),y
        bpl     LF2D4
        and     #$EF
        sta     (tpi1),y

LF2DE:  jsr     SetTimB32ms
        bcc     LF2E4                   ; Branch always

LF2E3:  sec
LF2E4:  ldy     #TPI::PRA
        lda     (tpi1),y
        and     #$40
        bne     LF2FC
        ldy     #CIA::ICR
        lda     (cia),y
        and     #$02
        beq     LF2E4
        lda     TimOut
        bmi     LF2DE
        bcc     LF2E3
        lda     #$01
        jsr     UPDST

LF2FC:  ldy     #TPI::PRA
        lda     (tpi1),y
        ora     #$10
        sta     (tpi1),y

LF304:  lda     #$FF
        ldy     #CIA::PRA
        sta     (cia),y
        rts

; -------------------------------------------------------------------------
; Output secondary address after listen

SECOND: jsr     transfer_byte

scatn:  ldy     #TPI::PRA
        lda     (tpi1),y
        ora     #$08
        sta     (tpi1),y
        rts

; -------------------------------------------------------------------------
; Output secondary address

TKSA:   jsr     transfer_byte

LF283:  ldy     #TPI::PRA
        lda     (tpi1),y
        and     #$39

; A -> IEC control, data ready for input

set_listen:
        ldy     #TPI::PRA
        sta     (tpi1),y
        lda     #$C7
        ldy     #TPI::DDRA
        sta     (tpi1),y
        lda     #$00
        ldy     #CIA::DDRA
        sta     (cia),y
        jmp     scatn

; -------------------------------------------------------------------------
; CIOUT routine

CIOUT:  pha
        lda     CTemp
        bpl     @L1
        lda     snsw1
        jsr     transfer_byte
        lda     CTemp
@L1:    ora     #$80
        sta     CTemp
        pla
        sta     snsw1
        rts

; -------------------------------------------------------------------------
; UNTALK/UNLISTEN

UNTLK:  lda     #$5F
        bne     LF2B1
UNLSN:  lda     #$3F
LF2B1:  jsr     talk_listen
        lda     #$F9
        jmp     set_listen

; -------------------------------------------------------------------------
; ACPTR routine

ACPTR:  ldy     #TPI::PRA
        lda     (tpi1),y
        and     #$B9
        ora     #$81
        sta     (tpi1),y

LF314:  jsr     SetTimB32ms
        bcc     LF31A

LF319:  sec

LF31A:  ldy     #TPI::PRA
        lda     (tpi1),y
        and     #$10
        beq     LF33F
        ldy     #CIA::ICR
        lda     (cia),y
        and     #$02
        beq     LF31A                   ; Loop if not timeout

        lda     TimOut
        bmi     LF314
        bcc     LF319
        lda     #$02
        jsr     UPDST
        ldy     #TPI::PRA
        lda     (tpi1),y
        and     #$3D
        sta     (tpi1),y
        lda     #$0D
        rts

LF33F:  lda     (tpi1),y                ; TPI::PRA
        and     #$7F
        sta     (tpi1),y
        and     #$20
        bne     LF350
        lda     #$40
        jsr     UPDST

LF350:  ldy     #CIA::PRA
        lda     (cia),y
        eor     #$FF
        pha
        ldy     #TPI::PRA
        lda     (tpi1),y
        ora     #$40
        sta     (tpi1),y

LF35E:  lda     (tpi1),y                ; TPI::PRA
        and     #$10
        beq     LF35E
        lda     (tpi1),y
        and     #$BF
        sta     (tpi1),y
        pla
        rts

; -------------------------------------------------------------------------
; Set timer B to 32,64 ms and start it

SetTimB32ms:
        lda     #$FF            ; 255*256*0,5 µs
        ldy     #CIA::TBHI
        sta     (cia),y         ; as high byte, low byte = 0
        lda     #$11
        ldy     #CIA::CRB
        sta     (cia),y         ; Start the timer
        ldy     #CIA::ICR
        lda     (cia),y         ; Clear the interrupt flag
        clc
        rts

