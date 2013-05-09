;
; Sound driver for the Atari Lynx.
;
; Karri Kaksonen and Bjoern Spruck, 11.12.2012
;

        .include        "lynx.inc"
        .include        "zeropage.inc"

        .export         _lynx_snd_init
        .export         _lynx_snd_active
        .export         _lynx_snd_play
        .export         _lynx_snd_stop
        .export         _lynx_snd_stop_channel
        .export         _lynx_snd_pause
        .export         _lynx_snd_continue
        .interruptor    lynx_snd_handler
        .import         popa
        .importzp       ptr1

;----------------------------------------------------------------------------
; ZP variables that go into APPZP
;

        .segment "APPZP" : zeropage

SndSema:                .res    1
SndPtrTmp:              .res    2
SndTmp:                 .res    2
SndEnvPtr:              .res    2

;----------------------------------------------------------------------------
; Global variables
;

        .bss

SndRetAFlag2:           .res    1
SndRetAFlag:            .res    1
SndPtrLo:               .res    4
SndPtrHi:               .res    4
SndDelay:               .res    4
SndLoopCnt:             .res    4
SndLoopPtrLo:           .res    4
SndLoopPtrHi:           .res    4
SndVolume:              .res    4
SndMaxVolume:           .res    4
SndNotePlaying:         .res    4
SndRetAddr:             .res    8
SndActive:              .res    4
SndReqStop:             .res    4
SndEnvVol:              .res    4
SndEnvFrq:              .res    4
SndEnvWave:             .res    4
SndChannel:             .res    32
SndEnvVolCnt:           .res    4
SndEnvVolInc:           .res    4
SndEnvVolOff:           .res    4
SndEnvVolLoop:          .res    4
SndEnvVolParts:         .res    4
SndEnvVolParts2:        .res    4
SndEnvFrqCnt:           .res    4
SndEnvFrqInc:           .res    4
SndEnvFrqOff:           .res    4
SndEnvFrqLoop:          .res    4
SndEnvFrqParts:         .res    4
SndEnvFrqParts2:        .res    4
SndEnvWaveCnt:          .res    4
SndEnvWaveOff:          .res    4
SndEnvWaveLoop:         .res    4
SndEnvWaveParts:        .res    4
SndEnvWaveParts2:       .res    4

MAX_INSTRUMENTS         .set    64
SndEnvVolPtrLo:         .res    MAX_INSTRUMENTS
SndEnvVolPtrHi:         .res    MAX_INSTRUMENTS
SndEnvFrqPtrLo:         .res    MAX_INSTRUMENTS
SndEnvFrqPtrHi:         .res    MAX_INSTRUMENTS
SndEnvWavePtrLo:        .res    MAX_INSTRUMENTS
SndEnvWavePtrHi:        .res    MAX_INSTRUMENTS

        .rodata

SndOffsets:             .byte   $00,$08,$10,$18

;----------------------------------------------------------------------------
; Macros
;

if_count        .set    0
nest_count      .set    0

.macro  _IFNE
        if_count        .set    if_count +1
        nest_count      .set    nest_count +1
        beq             .ident (.sprintf ("else%04d", if_count))
        .ident (.sprintf ("push%04d", nest_count)) .set if_count
.endmacro

.macro  _IFEQ
        if_count        .set    if_count +1
        nest_count      .set    nest_count +1
        bne             .ident (.sprintf ("else%04d", if_count))
        .ident (.sprintf ("push%04d", nest_count)) .set if_count
.endmacro

.macro  _IFMI
        if_count        .set    if_count +1
        nest_count      .set    nest_count +1
        bpl             .ident (.sprintf ("else%04d", if_count))
        .ident (.sprintf ("push%04d", nest_count)) .set if_count
.endmacro

.macro  _IFPL
        if_count        .set    if_count +1
        nest_count      .set    nest_count +1
        bmi             .ident (.sprintf ("else%04d", if_count))
        .ident (.sprintf ("push%04d", nest_count)) .set if_count
.endmacro

.macro  _IFGE
        if_count        .set    if_count +1
        nest_count      .set    nest_count +1
        bcc             .ident (.sprintf ("else%04d", if_count))
        .ident (.sprintf ("push%04d", nest_count)) .set if_count
.endmacro

.macro  _IFCS
        if_count        .set    if_count +1
        nest_count      .set    nest_count +1
        bcc             .ident (.sprintf ("else%04d", if_count))
        .ident (.sprintf ("push%04d", nest_count)) .set if_count
.endmacro

.macro  _IFCC
        if_count        .set    if_count +1
        nest_count      .set    nest_count +1
        bcs             .ident (.sprintf ("else%04d", if_count))
        .ident (.sprintf ("push%04d", nest_count)) .set if_count
.endmacro

.macro  _ELSE
        bra     .ident (.sprintf ("endif%04d", .ident (.sprintf ("push%04d", nest_count))))
        .ident  (.sprintf ("else%04d", .ident (.sprintf ("push%04d", nest_count)))) := *
.endmacro

.macro  _ENDIF
        .if .not .defined( .ident (.sprintf ("else%04d", .ident (.sprintf ("push%04d", nest_count)))))
                .ident  (.sprintf ("else%04d", .ident (.sprintf ("push%04d", nest_count)))) := *
        .endif
        .ident  (.sprintf ("endif%04d", .ident (.sprintf ("push%04d", nest_count)))) := *
        nest_count      .set    nest_count -1
.endmacro

        .code

;----------------------------------------------------------------------------
; void lynx_snd_init() will initialize the sound engine.
;

_31250Hz        .set    %101

_lynx_snd_init:
        php
        sei
        lda     #%10011000|_31250Hz
        sta     STIMCTLA
        lda     #129
        sta     STIMBKUP        ; set up a 240Hz IRQ

        stz     AUD0VOL
        stz     AUD1VOL
        stz     AUD2VOL
        stz     AUD3VOL

        stz     $fd44           ; all channels full volume / no attenuation
        lda     #$ff
        stz     MSTEREO

        lda     #0
        sta     AUD0CTLA
        sta     AUD1CTLA
        sta     AUD2CTLA
        sta     AUD3CTLA

        ldx     #3
        lda     #0
init0:  stz     SndActive,x
        stz     SndReqStop,x
        stz     SndEnvVol,x
        stz     SndEnvFrq,x
        stz     SndEnvWave,x
        ldy     SndOffsets,x
        sta     SndChannel+2,y
        dex
        bpl     init0
        stz     SndRetAFlag
        stz     SndRetAFlag2
        stz     SndSema
        plp
        rts

;----------------------------------------------------------------------------
; lynx_snd_handler is run at every sound interrupt
;

lynx_snd_handler:
        lda     INTSET
        and     #SND_INTERRUPT
        bne     @L0
        clc
        rts
@L0:
        lda     #$ff
        tsb     SndSema
        bne     endirq
        phy
        ; *NOW* set all values which were "pre-set" in last interrupt
        jsr     SndSetValues
        cli

        lda SndRetAFlag   ; reset the return flag, but save it first
        _IFNE
                lda #$0F ; MASK
                sta SndRetAFlag2
                stz SndRetAFlag
        _ENDIF

        ldx #3
irq0:   phx
        lda SndActive,x
        _IFNE
                lda SndEnvVol,x
                _IFNE
                        phx
                        jsr SndChangeVol
                        plx
                _ENDIF
                lda SndEnvFrq,x
                _IFNE
                        phx
                        jsr SndChangeFrq
                        plx
                _ENDIF
                lda SndEnvWave,x
                _IFNE
                        phx
                        jsr SndChangeWave
                        plx
                _ENDIF
                jsr SndGetCmd
        _ENDIF
        plx
        dex
        bpl irq0
        sei
        ply
        stz SndSema
endirq:
        clc
        rts

;----------------------------------------------------------------------------
; A process table with addresses to sound functions
;

SndCmdsLo:
        .byte <((SndLoop)-1)
        .byte <((SndDo)-1)
        .byte <((SndPause)-1)
        .byte <((SndNoteOff)-1)
        .byte <((SndSetInstr)-1)
        .byte <((SndNewNote2)-1)
        .byte <((SndCallPattern)-1)
        .byte <((SndRetToSong)-1)
        .byte <((SndDefEnvVol)-1)
        .byte <((SndSetEnvVol)-1)
        .byte <((SndDefEnvFrq)-1)
        .byte <((SndSetEnvFrq)-1)
        .byte <((SndDefEnvWave)-1)
        .byte <((SndSetEnvWave)-1)
        .byte <((SndSetStereo)-1)
        .byte <((SndSetAttenuationOn)-1)
        .byte <((SndSetChnAttenution)-1)
        .byte <((SndPlayerFreq)-1)
        .byte <((SndReturnAll)-1)

SndCmdsHi:
        .byte >((SndLoop)-1)
        .byte >((SndDo)-1)
        .byte >((SndPause)-1)
        .byte >((SndNoteOff)-1)
        .byte >((SndSetInstr)-1)
        .byte >((SndNewNote2)-1)
        .byte >((SndCallPattern)-1)
        .byte >((SndRetToSong)-1)
        .byte >((SndDefEnvVol)-1)
        .byte >((SndSetEnvVol)-1)
        .byte >((SndDefEnvFrq)-1)
        .byte >((SndSetEnvFrq)-1)
        .byte >((SndDefEnvWave)-1)
        .byte >((SndSetEnvWave)-1)
        .byte >((SndSetStereo)-1)
        .byte >((SndSetAttenuationOn)-1)
        .byte >((SndSetChnAttenution)-1)
        .byte >((SndPlayerFreq)-1)
        .byte >((SndReturnAll)-1)

;----------------------------------------------------------------------------
; Get next sound command from stream
;

SndGetCmd:
        lda SndReqStop,x
        bne SndStop

        lda SndRetAFlag2
        and SndMask,x
        _IFNE
                eor SndRetAFlag2
                sta SndRetAFlag2
                lda SndRetAddr,x
                sta SndPtrLo,x
                lda SndRetAddr+4,x
                sta SndPtrHi,x
                ;;; force the direct continue return
        _ELSE
                dec SndDelay,x
                bne cmd991 ;; check special case
        _ENDIF

        lda SndPtrLo,x
        sta SndPtrTmp
        lda SndPtrHi,x
        sta SndPtrTmp+1
cmd0:   lda (SndPtrTmp)
        beq SndStop
        _IFMI
                and #$7f
                tay
                jsr SndCallCmd
        _ELSE
                jsr SndNewNote
        _ENDIF
        clc
        tya
        and #$7f
        adc SndPtrTmp
        sta SndPtrLo,x
        sta SndPtrTmp
        lda #0
        adc SndPtrTmp+1
        sta SndPtrHi,x
        sta SndPtrTmp+1

        tya
        bmi cmd0
cmd991:
        ;; now check if delay is only 1 AND next one is return all.
        lda #1
        cmp SndDelay,x
        bne cmd99

        ;; NOW read ahead ONE
        lda SndPtrLo,x
        sta SndPtrTmp
        lda SndPtrHi,x
        sta SndPtrTmp+1

        lda (SndPtrTmp)
        cmp #$92 ;; Return all
        _IFEQ
                sta SndRetAFlag ; just set !=0
        _ENDIF
cmd99:  rts

;----------------------------------------------------------------------------
; Call function pointed to by y
;

SndCallCmd:
        lda SndCmdsHi,y
        pha
        lda SndCmdsLo,y
        pha
        ldy #1
        rts

;----------------------------------------------------------------------------
; Stop sound on one channel
;

SndStop:
        stz SndReqStop,x
        stz SndActive,x
        ldy SndOffsets,x
        lda #0
        sta SndChannel,y
        ina ;lda #1
        sta SndChannel+2,y
        tay ;ldy #1
        rts


;----------------------------------------------------------------------------
; Send a new note, length, volume triplet
;

SndNewNote:
        phx
        sta SndNotePlaying,x
        pha
        ldy #1
        lda (SndPtrTmp),y
        sta SndDelay,x
        ldy SndOffsets,x
        lda SndVolume,x
        sta SndChannel,y
        plx
        lda SndPrescaler,x
        sta SndChannel+5,y
        lda SndReload,x
        sta SndChannel+4,y
        lda #$FF   ; = -1
        sta SndChannel+2,y
        plx
        lda SndEnvVol,x
        _IFNE
                jsr SndSetEnvVol1
        _ENDIF
        lda SndEnvFrq,x
        _IFNE
                jsr SndSetEnvFrq1
        _ENDIF
        lda SndEnvWave,x
        _IFNE
                jsr SndSetEnvWave1
        _ENDIF
        lda #$2
        ldy SndDelay,x
        _IFNE
                ora #$80
        _ENDIF
        tay
        rts

;----------------------------------------------------------------------------
; Start a loop with count
;

SndLoop:
        lda (SndPtrTmp),y
        sta SndLoopCnt,x
        lda SndPtrTmp
        sta SndLoopPtrLo,x
        lda SndPtrTmp+1
        sta SndLoopPtrHi,x
        ldy #$82
        rts

SndDo:
        dec SndLoopCnt,x
        _IFNE
                lda SndLoopPtrLo,x
                sta SndPtrTmp
                lda SndLoopPtrHi,x
                sta SndPtrTmp+1
                ldy #$82
        _ELSE
                ldy #$81
        _ENDIF
        rts

;----------------------------------------------------------------------------
; Sound volume envelope
;

SndDefEnvVol:
        phx
        lda (SndPtrTmp),y               ; env #
        tax

        iny
        lda (SndPtrTmp),y
        sta SndEnvVolPtrLo,x
        iny
        lda (SndPtrTmp),y
        sta SndEnvVolPtrHi,x            ; Ptr to [cnt,inc]

        ldy #$84
        plx
        rts

SndSetEnvVol:
        lda (SndPtrTmp),y               ; # env

SndSetEnvVol1:
        and #$7f
        sta SndEnvVol,x                 ; save
        _IFEQ
                ldy #$82
                rts
        _ENDIF

        tay

        lda SndEnvVolPtrLo,y
        sta SndEnvPtr
        lda SndEnvVolPtrHi,y
        sta SndEnvPtr+1

        lda (SndEnvPtr)
        sta SndTmp
        asl
        sta SndEnvVolLoop,x             ; here is the loop-start

        ldy #1
        lda (SndEnvPtr),y
        sta SndEnvVolParts,x
        sec
        sbc SndTmp
        sta SndEnvVolParts2,x

        stz SndEnvVolCnt,x
        lda #2
        sta SndEnvVolOff,x

        ldy #$82
        rts

;----------------------------------------------------------------------------
; Sound frequency envelope
;

SndDefEnvFrq:
        phx
        lda (SndPtrTmp),y               ; env #
        tax

        iny
        lda (SndPtrTmp),y
        sta SndEnvFrqPtrLo,x
        iny
        lda (SndPtrTmp),y
        sta SndEnvFrqPtrHi,x            ; Ptr to [inc,cnt]
        plx
        ldy #$84
        rts

SndSetEnvFrq:
        lda (SndPtrTmp),y               ; # env

SndSetEnvFrq1:
        and #$7f
        sta SndEnvFrq,x                 ; save
        _IFEQ
                ldy #$82
                rts
        _ENDIF

        tay

        lda SndEnvFrqPtrLo,y
        sta SndEnvPtr
        lda SndEnvFrqPtrHi,y
        sta SndEnvPtr+1

        lda (SndEnvPtr)
        sta SndTmp
        asl
        sta SndEnvFrqLoop,x

        ldy #1
        lda (SndEnvPtr),y
        sta SndEnvFrqParts,x
        sec
        sbc SndTmp
        sta SndEnvFrqParts2,x

        stz SndEnvFrqCnt,x
        lda #2
        sta SndEnvFrqOff,x

        ldy #$82
        rts

;----------------------------------------------------------------------------
; Sound frequency envelope
;

SndDefEnvWave:

        phx
        lda (SndPtrTmp),y               ; env #
        tax

        iny
        lda (SndPtrTmp),y
        sta SndEnvWavePtrLo,x
        iny
        lda (SndPtrTmp),y
        sta SndEnvWavePtrHi,x            ; Ptr to [inc,cnt]
        plx
        ldy #$84
        rts

SndSetEnvWave:
        lda (SndPtrTmp),y               ; # env

SndSetEnvWave1:
        and #$7f
        sta SndEnvWave,x                 ; save
        _IFEQ
                ldy #$82
                rts
        _ENDIF

        tay

        lda SndEnvWavePtrLo,y
        sta SndEnvPtr
        lda SndEnvWavePtrHi,y
        sta SndEnvPtr+1

        lda (SndEnvPtr)
        sta SndTmp
        asl ; *4 -2
        dea
        asl
        sta SndEnvWaveLoop,x

        ldy #1
        lda (SndEnvPtr),y
        sta SndEnvWaveParts,x
        sec
        sbc SndTmp
        sta SndEnvWaveParts2,x

        stz SndEnvWaveCnt,x
        lda #2
        sta SndEnvWaveOff,x

        ldy #$82
        rts

;----------------------------------------------------------------------------
; Pause sound
;

SndPause:
                lda (SndPtrTmp),y
                sta SndDelay,x
                iny
SndDummy:        rts
;;;* This set the new Player Freq instantanious!!!
SndPlayerFreq:
                lda (SndPtrTmp),y
                sta STIMCTLA
                iny
                lda (SndPtrTmp),y
                sta STIMBKUP
                ldy #$83
                rts

SndNoteOff:
                ldy SndOffsets,x
                stz SndNotePlaying,x
                lda SndEnvVol,x
                ora #$80
                sta SndEnvVol,x
                lda SndEnvFrq,x
                ora #$80
                sta SndEnvFrq,x
                lda SndEnvWave,x
                ora #$80
                sta SndEnvWave,x
                lda #0
                sta SndChannel,y
                sta SndChannel+4,y
                sta SndChannel+5,y
                dea
                sta SndChannel+2,y
                ldy #$81
                rts
SndSetInstr:
                phx
                lda SndOffsets,x
                tax
                lda (SndPtrTmp),y
                sta SndChannel+3,x
                iny
                lda (SndPtrTmp),y
                sta SndChannel+7,x
                iny
                lda (SndPtrTmp),y
                sta SndChannel+1,x
                plx
                iny
                lda (SndPtrTmp),y
                sta SndVolume,x
                iny
                lda (SndPtrTmp),y
                sta SndMaxVolume,x

                ldy #$86
                rts
SndCallPattern:
                clc
                lda SndPtrTmp
                adc #3
                sta SndRetAddr,x
                lda SndPtrTmp+1
                adc #0
                sta SndRetAddr+4,x
                ldy #1
                lda (SndPtrTmp),y
                pha
                iny
                lda (SndPtrTmp),y
                sta SndPtrTmp+1
                pla
                sta SndPtrTmp
                ldy #$80
                rts

SndRetToSong:
                lda SndRetAddr,x
                sta SndPtrTmp
                lda SndRetAddr+4,x
                sta SndPtrTmp+1
                ldy #$80
                rts

SndReturnAll:
                lda #1
                sta SndRetAFlag
                sta SndDelay,x
                ldy #$0
                rts
SndNewNote2:
;;; Note,length,volume

                phx
                  sta SndNotePlaying,x
                  ldy #1
                  lda (SndPtrTmp),y             ; reload
                  pha
                  iny
                  lda (SndPtrTmp),y             ; prescale
                  pha
                  iny
                  lda (SndPtrTmp),y         ; laenge
                  sta SndDelay,x

                  ldy SndOffsets,x
                  lda SndVolume,x
                  sta SndChannel,y
                  pla
                  sta SndChannel+5,y
                  pla
                  sta SndChannel+4,y
                  lda #$FF ; = -1
                  sta SndChannel+2,y
                plx
                lda SndEnvVol,x
                _IFNE
                  jsr SndSetEnvVol1
                _ENDIF
                lda SndEnvFrq,x
                _IFNE
                  jsr SndSetEnvFrq1
                _ENDIF
                lda SndEnvWave,x
                _IFNE
                  jsr SndSetEnvWave1
                _ENDIF
                ldy #4
                rts

SndSetStereo:
                ldy #1
                lda (SndPtrTmp),y
                sta MSTEREO
                ldy #$82
                rts

SndSetAttenuationOn:
                ldy #1
                lda (SndPtrTmp),y
                sta $FD44
                ldy #$82
                rts

SndSetChnAttenution:
                ldy #1
                lda (SndPtrTmp),y
                sta $FD40,x
                ldy #$82
                rts

SndChangeVol:
                tay
                _IFMI
vol99:               rts
                _ENDIF
                lda SndNotePlaying,x
                beq vol99

                lda SndEnvVolPtrLo,y
                sta SndEnvPtr
                lda SndEnvVolPtrHi,y
                sta SndEnvPtr+1

                dec SndEnvVolCnt,x
                _IFMI
                  dec SndEnvVolParts,x
                  _IFMI
                    lda SndEnvVolLoop,x
                    _IFNE
                      tay
                      lda SndEnvVolParts2,x
                      sta SndEnvVolParts,x
                      bra vol1v
                    _ELSE
                      tya
                      ora #$80
                      sta SndEnvVol,x
                    _ENDIF
                  _ELSE
                    ldy SndEnvVolOff,x
vol1v:                 lda (SndEnvPtr),y
                    sta SndEnvVolCnt,x
                    iny
                    lda (SndEnvPtr),y
                    sta SndEnvVolInc,x
                    iny
                    tya
                    sta SndEnvVolOff,x
                  _ENDIF
                  rts
                _ENDIF

                  ldy SndOffsets,x
                  clc
                  lda SndEnvVolInc,x
                  _IFEQ
                    rts
                  _ENDIF
                  _IFPL
                    adc SndChannel,y
                    cmp SndMaxVolume,x
                    _IFCS
                      lda SndMaxVolume,x
                      stz SndEnvVolInc,x
                    _ENDIF
                    cmp #$80
                    _IFGE
                      lda #$7f
                      stz SndEnvVolInc,x
                    _ENDIF
                 _ELSE
                   adc SndChannel,y
                   _IFCC
                     lda #0
                     sta SndEnvVolInc,x
                     ;; NEU: switch Tremolo off
                     sta SndNotePlaying,x
                   _ENDIF
                   cmp #$80
                   _IFGE
                     lda #0
                     sta SndEnvVolInc,x
                     ;; NEU: switch Tremolo off
                     sta SndNotePlaying,x
                   _ENDIF
                 _ENDIF
                  sta SndChannel,y
                  lda SndChannel+2,y
                  ora #1 ;; if already -1 ... no effect
                  sta SndChannel+2,y
                rts


SndChangeFrq:
                tay
                _IFMI
frq99:               rts
                _ENDIF
                lda SndNotePlaying,x
                beq frq99

                lda SndEnvFrqPtrLo,y
                sta SndEnvPtr
                lda SndEnvFrqPtrHi,y
                sta SndEnvPtr+1

                dec SndEnvFrqCnt,x
                _IFMI
                  dec SndEnvFrqParts,x
                  _IFMI
                    lda SndEnvFrqLoop,x
                    _IFNE
                      tay
                      lda SndEnvFrqParts2,x
                      sta SndEnvFrqParts,x
                      bra frq1f
                    _ELSE
                      tya
                      ora #$80
                      sta SndEnvFrq,x
                    _ENDIF
                  _ELSE
                    ldy SndEnvFrqOff,x
frq1f:                 lda (SndEnvPtr),y
                    sta SndEnvFrqCnt,x
                    iny
                    lda (SndEnvPtr),y
                    eor #$ff
                    ina
                    sta SndEnvFrqInc,x
                    iny
                    tya
                    sta SndEnvFrqOff,x
                  _ENDIF
                  rts
                  _ENDIF
                  ldy SndOffsets,x
                  clc
                  lda SndEnvFrqInc,x
                  _IFEQ
                    rts
                  _ENDIF

                  _IFMI
                    adc SndChannel+4,y
                    _IFPL
                      pha
                      lda SndChannel+5,y
                      _IFNE
                        dea
                        sta SndChannel+5,y
                        pla
                        eor #128
                      _ELSE
                        pla
                        pha
                        clc
                        adc SndEnvFrqInc,x
                        pla
                      _ENDIF
                    _ELSE
                      pha
                      lda SndChannel+5,y
                      _IFEQ
                        pla
                        sta SndChannel+4,y
                        rts
                      _ENDIF
                      pla
                    _ENDIF
                  _ELSE
                    adc SndChannel+4,y
                    _IFPL
                      pha
                      lda SndChannel+5,y
                      cmp #6
                      _IFNE
                        ina
                        sta SndChannel+5,y
                        pla
                        eor #128
                      _ELSE
                        lda SndChannel+4,y
                        _IFMI
                          pla
                          rts
                        _ENDIF
                        pla
                      _ENDIF
                    _ELSE
                      pha
                      lda SndChannel+5,y
                      cmp #6
                      _IFEQ
                        lda SndChannel+4,y
                        _IFPL
                          pla
                          rts
                        _ENDIF
                      _ENDIF
                      pla
                    _ENDIF
                  _ENDIF
                  sta SndChannel+4,y

                  ora #1 ;; if already -1 -> no effect
                rts

SndChangeWave:
                tay
                ;; Ab hier x Kanal 0-3, y Environment
                _IFMI
wav99:               rts
                _ENDIF
                lda SndNotePlaying,x
                beq wav99


                lda SndEnvWavePtrLo,y
                sta SndEnvPtr
                lda SndEnvWavePtrHi,y
                sta SndEnvPtr+1

                dec SndEnvWaveCnt,x
                _IFMI
                  dec SndEnvWaveParts,x
                  _IFMI
                    lda SndEnvWaveLoop,x
                    _IFNE
                      tay
                      ;; Ab hier x Kanal 0-3, y Offset im Environment
                      lda SndEnvWaveParts2,x
                      sta SndEnvWaveParts,x
                      bra wav1v
                    _ELSE
                      tya
                      ora #$80 ;; beende Env
                      sta SndEnvWave,x
                    _ENDIF
                  _ELSE
                    ldy SndEnvWaveOff,x
                ;; Ab hier x Kanal 0-3, y Offset im  Environment
wav1v:                 lda (SndEnvPtr),y
                    sta SndEnvWaveCnt,x
                  phx
                  lda SndOffsets,x
                  tax
                ;; Ab hier x Kanal (0-3)*8, y Offset im  Environment

                    iny
                    lda (SndEnvPtr),y
                    sta SndChannel+3,x  ; Shift LO
                    iny
                    lda (SndEnvPtr),y
                    sta SndChannel+7,x   ; Shift HI
                    iny
                    lda (SndEnvPtr),y
                    sta SndChannel+1,x  ; Feedback
                    iny
                    tya
                    ply
                ;; Ab hier x Kanal (0-3)*8,  y Kanal 0-3
                    sta SndEnvWaveOff,y
                  lda #$FF ; =-1 ;; stop timer to set new values...
                  sta SndChannel+2,x
                  _ENDIF
                _ENDIF
                rts


SndSetValues:
                ldx #4-1
set0:                ldy SndOffsets,x
                  lda SndChannel+2,y
                  _IFNE                        ; flag == 0 => don`t set

            bit #$80
                    _IFNE                       ;
                      lda #0
                      sta $fd25,y                 ; stop sound timer

                      lda SndChannel+3,y
                      sta $fd23,y                 ; shifter 1
                      ;;lda $fd27,y
                      ;;and #$0F
                      ;;ora SndChannel+7,y          ; shifter 2
                      lda SndChannel+7,y          ; shifter 2
                      sta $fd27,y
                      lda SndChannel+1,y
                      sta $fd21,y                 ; feedback
                    _ENDIF

                    lda SndChannel,y
                    sta $fd20,y                 ; volume
                    lda SndChannel+2,y
            bit #$80
                    _IFNE                       ;
                    lda SndChannel+4,y
                    sta $fd24,y                 ; reload
                    lda SndChannel+5,y
                    ora #%00011000 ;;; #%01011000
            ;; and #%00111111
                    sta $fd25,y                 ; re-enable timer
                    _ENDIF

                    lda #0
                    sta SndChannel+2,y          ; clear flag
                _ENDIF

                dex
        _IFPL
            jmp set0
        _ENDIF
        rts

_lynx_snd_play:
        sta ptr1
        stx ptr1+1
        jsr popa
        tax
        lda ptr1
        ldy ptr1+1
        php
        pha
        lda SndActive,x
        _IFNE
                dec SndReqStop,x
                lda #1
                sta SndDelay,x
start0:         lda SndActive,x
                bne start0
        _ENDIF
        bra start1
SndStartSoundx:
        php
        pha
start1:
        sei
        pla
        sta SndPtrLo,x
        tya
        sta SndPtrHi,x
        lda #1
        sta SndDelay,x
        stz SndEnvVol,x
        stz SndEnvFrq,x
        stz SndEnvWave,x
        sta SndActive,x
        stz SndReqStop,x
        plp
        rts
SndStartSound2:
        pha
        lda SndActive,x         ; check default
        beq start20                  ; inactive => ok
        phx
        ldx #3                  ; search free channel
start21:
        lda SndActive,x
        beq start22                ; found =>
        dex
        bpl start21
        plx                     ; not found
        dec SndReqStop,x        ; stop default-channel
        lda #1
        sta SndDelay,x
start23:
        lda SndActive,x
        bne start23
        bra start20
start22:
        pla             ; clear stack
start20:
        pla
        phx
        jsr SndStartSoundx      ; launch new sound
        plx
        rts

_lynx_snd_stop:
        ldx #3
        lda SndActive,x
        _IFNE
stop0:          dec SndReqStop,x
                lda #1
                sta SndDelay,x
stop1:          lda SndActive,x
                bne stop1
        _ENDIF
        dex
        bpl stop0
        rts

_lynx_snd_stop_channel:
        tax
        lda SndActive,x
        _IFNE
                dec SndReqStop,x
                lda #1
                sta SndDelay,x
stopc1:         lda SndActive,x
                bne stopc1
        _ENDIF
        rts

_lynx_snd_active:
        ldx #3
        lda #0
act0:   ldy SndActive,x
        _IFNE
                ora SndMask,x
        _ENDIF
        dex
        bpl act0
        rts

_lynx_snd_pause:
        php
        sei
        lda STIMCTLA
        sta SndPauseOff1+1
        stz STIMCTLA
        lda MSTEREO
        sta SndPauseOff2+1
        lda #$ff
        sta MSTEREO
        lda #$18
        trb AUD0CTLA
        trb AUD1CTLA
        trb AUD2CTLA
        trb AUD3CTLA
        plp
        rts

_lynx_snd_continue:
        php
        sei
SndPauseOff1:
        lda #0 ; Selbsmodifizierter Code!!!
        sta STIMCTLA
SndPauseOff2:
        lda #0 ; Selbsmodifizierter Code!!!
        sta MSTEREO

        lda #$18
        tsb AUD0CTLA
        tsb AUD1CTLA
        tsb AUD2CTLA
        tsb AUD3CTLA

        plp
        rts

        .rodata

SndMask:
        .byte 1,2,4,8

SndPrescaler:
        .byte $00,$06,$06,$06,$06,$05,$05,$05,$05,$05,$05,$05,$04,$04,$04,$04
        .byte $04,$04,$04,$04,$03,$03,$03,$03,$03,$03,$03,$03,$03,$03,$02,$02
        .byte $02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$01,$01,$01,$01,$01
        .byte $01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
        .byte $00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00

SndReload:
        .byte $00,$9A,$96,$8F,$86,$FA,$E5,$D1,$BE,$AC,$9C,$8D,$00,$E8,$D3,$C0
        .byte $AF,$A0,$93,$87,$FA,$E7,$D6,$C6,$B8,$AC,$A1,$96,$8D,$84,$FA,$EB
        .byte $DE,$D2,$C7,$BC,$B3,$AA,$A1,$9A,$93,$8C,$86,$00,$F5,$EB,$E1,$D8
        .byte $CF,$C7,$C0,$B9,$B2,$AB,$A5,$A0,$9A,$95,$90,$8B,$87,$82,$FD,$F5
        .byte $EE,$E7,$E0,$D9,$D3,$CD,$C8,$C2,$BD,$B8,$B3,$AE,$AA,$A5,$A1,$9D
        .byte $99,$96,$92,$8F,$8B,$88,$85,$82,$7F,$7C,$79,$77,$74,$72,$6F,$6D
        .byte $6B,$69,$67,$64,$63,$61,$5F,$5D,$5B,$59,$58,$56,$55,$53,$51,$50
        .byte $4F,$4D,$4C,$4B,$49,$48,$47,$46,$44,$43,$42,$41,$40,$3F,$3E,$3D



