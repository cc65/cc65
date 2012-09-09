;
; Oliver Schmidt, 2012-09-04
;
; unsigned char getfirstdevice (void);
; unsigned char __fastcall__ getnextdevice (unsigned char device);
;

        .export         _getfirstdevice
        .export         _getnextdevice

        .import         __dos_type

        .include        "zeropage.inc"
        .include        "mli.inc"

_getfirstdevice:
        lda     #$FF
        ; Fall through

_getnextdevice:
next:   tax
        inx
        txa
        cmp     #$FF
        beq     done

        ; Check for ProDOS 8
        ldx     __dos_type
        beq     next

        ; Up to 14 units may be active
        ldx     #<DEVLST
        ldy     #>DEVLST
        stx     ptr1
        sty     ptr1+1

        ; Number of on-line devices (minus 1)
        ldy     DEVCNT

	; Does the list contain the device?
:       cmp     (ptr1),y
        beq	done
        dey
        bpl     :-
        bmi     next            ; Branch always

done:   ldx     #$00
        rts
