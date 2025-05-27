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
        bit     ostype
        bmi     iigs            ; $8x
        bvs     iic             ; $4x

        .ifndef  __APPLE2ENH__
        bit     machinetype     ; IIe/enh?
        bmi     :+

        lda     #$FF            ; ][+ Unsupported
        tax
        rts
        .endif

:       bit     RDVBLBAR
        bpl     :-              ; Blanking
:       bit     RDVBLBAR
        bmi     :-              ; Drawing
        bpl     out

        ; Apple IIgs TechNote #40, VBL Signal
iigs:   bit     RDVBLBAR
        bmi     iigs            ; Blanking
:       bit     RDVBLBAR
        bpl     :-              ; Drawing
        bmi     out

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
out:    lda     #$00
        tax
        rts
