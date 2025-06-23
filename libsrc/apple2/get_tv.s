;
; Colin Leroy-Mira <colin@colino.net>, 2025
;
; unsigned char __fastcall__ get_tv(void)
;
        .export         _get_tv

        .import         _set_iigs_speed, _get_iigs_speed
        .import         ostype

        .constructor    calibrate_tv, 8 ; After ostype

        .include    "accelerator.inc"
        .include    "apple2.inc"
        .include    "get_tv.inc"

        .segment "ONCE"

; Cycle wasters
waste_72:
        jsr     waste_36
waste_36:
        jsr     waste_12
waste_24:
        jsr     waste_12
waste_12:
        rts

.proc calibrate_tv
        lda     ostype
        bmi     iigs
        cmp     #$20
        bcc     iip
        cmp     #$40
        bcc     iie

iic:    jmp     calibrate_iic
iigs:   jmp     calibrate_iigs
iie:    jmp     calibrate_iie
iip:    rts                   ; Keep TV::OTHER.
.endproc


; Magic numbers
WASTE_LOOP_CYCLES = 92                      ; The wait loop total cycles
NTSC_LOOP_COUNT   = 17030/WASTE_LOOP_CYCLES ; How many loops expected on NTSC
PAL_LOOP_COUNT    = 20280/WASTE_LOOP_CYCLES ; How many loops expected on PAL
STOP_PTRIG        = 16500/WASTE_LOOP_CYCLES ; Stop PTRIG at 16.5ms

; Carry set at enter: wait for VBL +
; Carry clear at enter: wait for VBL -
; Increments X every 92 cycles.
.proc count_until_vbl_bit
        lda     #$10          ; BPL
        bcc     :+
        lda     #$30          ; BMI
:       sta     sign

        ; Wait for VBLsign change with 92 cycles loops.
        ; Hit PTRIG repeatedly so that accelerators will slow down.
        ; But stop hitting PTRIG after 16.5ms cycles, so that on the //c,
        ; the VBLINT will not be reset right before we get it. 16.5ms
        ; is a good value because it's far enough from 17ms for NTSC
        ; models, and close enough to 20.2ms for PAL models that accelerators
        ; will stay slow until there. (5ms usually).

:       cpx     #STOP_PTRIG   ; 2     - see if we spent 16.5ms already
        bcs     notrig        ; 4 / 5 - if so, stop hitting PTRIG
        sta     PTRIG         ; 8     - otherwise hit it
        bcc     count         ; 11
notrig:
        nop                   ;     7 - keep cycle count constant when not
        nop                   ;     9 - hitting PTRIG
        nop                   ;     11
count:
        inx                   ; 13
        jsr     waste_72      ; 85
        bit     RDVBLBAR      ; 89    - Wait for VBL change
sign:
        bpl     :-            ; 92    - patched with bpl/bmi
        rts
.endproc

.proc calibrate_iic
        php
        sei

        sta     IOUDISOFF
        lda     RDVBLMSK
        pha                   ; Back up for cleanup

        bit     ENVBL
        bit     PTRIG         ; Reset VBL interrupt flag
:       bit     RDVBLBAR      ; Wait for one VBL
        bpl     :-

        bit     PTRIG         ; Reset VBL interrupt flag again
        ldx     #$00
        clc
        jsr     count_until_vbl_bit

        pla                   ; Cleanup
        asl
        bcs     :+            ; VBL interrupts were already enabled
        bit     DISVBL
:       sta     IOUDISON      ; IIc Tech Ref Man: The firmware normally leaves IOUDIS on.

        plp
        jmp     calibrate_done
.endproc

.proc calibrate_iie
:       bit     RDVBLBAR      ; Wait for bit 7 to be off (VBL start)
        bmi     :-
:       bit     RDVBLBAR      ; Wait for bit 7 to be on (VBL end)
        bpl     :-

        ; Wait and count during a full cycle
        ldx     #$00
        sec
        jsr     count_until_vbl_bit
        clc
        jsr     count_until_vbl_bit

        jmp     calibrate_done
.endproc

.proc calibrate_iigs
        ; Backup speed and slow down
        jsr     _get_iigs_speed
        pha
        lda     #SPEED_SLOW
        jsr     _set_iigs_speed

        ; The same as IIe, but reverted, because... something?
:       bit     RDVBLBAR      ; Wait for bit 7 to be on (VBL start)
        bpl     :-
:       bit     RDVBLBAR      ; Wait for bit 7 to be off (VBL end)
        bmi     :-

        ; Wait and count during a full cycle
        ldx     #$00
        clc
        jsr     count_until_vbl_bit
        sec
        jsr     count_until_vbl_bit

        jsr     calibrate_done

        ; Restore user speed
        pla
        jmp     _set_iigs_speed
.endproc

.proc calibrate_done
        ; Consider X +/- 3 to be valid,
        ; anything else is unknown.

        lda     #TV::NTSC
        cpx     #NTSC_LOOP_COUNT-3
        bcc     unexpected
        cpx     #NTSC_LOOP_COUNT+3
        bcc     matched

        lda     #TV::PAL
        cpx     #PAL_LOOP_COUNT-3
        bcc     unexpected
        cpx     #PAL_LOOP_COUNT+3
        bcs     unexpected

matched:
        sta     tv

unexpected:
        rts
.endproc

        .code

; The only thing remaining from that code after init
.proc _get_tv
        lda     tv
        ldx     #>$0000
        rts
.endproc

        .segment "INIT"

tv:     .byte   TV::OTHER
