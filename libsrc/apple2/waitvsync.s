;
; Oliver Schmidt, 2020-06-14
;
; void waitvsync (void);
;
        .ifdef  __APPLE2ENH__

        .export         _waitvsync
        .import         ostype

        .include        "apple2.inc"

_waitvsync:
        bit     ostype
        bmi     iigs            ; $8x
        bvs     iic             ; $4x

:       bit     RDVBLBAR
        bpl     :-              ; Blanking
:       bit     RDVBLBAR
        bmi     :-              ; Drawing
        rts

        ; Apple IIgs TechNote #40, VBL Signal
iigs:   bit     RDVBLBAR
        bmi     iigs            ; Blanking
:       bit     RDVBLBAR
        bpl     :-              ; Drawing
        rts

        ; Apple IIc TechNote #9, Detecting VBL
iic:    sei
        sta     IOUDISOFF
        lda     RDVBLMSK
        bit     ENVBL
        bit     PTRIG           ; Reset VBL interrupt flag
:       bit     RDVBLBAR
        bpl     :-
        asl
        bcs     :+              ; VBL interrupts were already enabled
        bit     DISVBL
:       sta     IOUDISON        ; IIc Tech Ref Man: The firmware normally leaves IOUDIS on.
        cli
        rts

        .endif                  ; __APPLE2ENH__
