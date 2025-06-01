;
; Oliver Schmidt, 2020-06-14
;
; void waitvsync (void);
;
        .export         _waitvsync
        .import         ostype

        .ifndef  __APPLE2ENH__
        .import         machinetype
        .endif

        .include        "apple2.inc"

_waitvsync:
        .ifndef  __APPLE2ENH__
        bit     machinetype     ; IIe/enh?
        bpl     out             ; No, silently fail
        .endif

        bit     ostype
        bmi     iigs            ; $8x
        bvs     iic             ; $4x

        ; Apple IIe
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
iic:    php
        sei
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
        plp
out:    rts
