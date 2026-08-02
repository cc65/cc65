;
; Oliver Schmidt, 2012-09-04
; Robert Justice, 2026
;
; unsigned char getfirstdevice (void);
; unsigned char __fastcall__ getnextdevice (unsigned char device);
;

        .export         _getfirstdevice
        .export         _getnextdevice
        .import         isdevice

        .include        "zeropage.inc"

_getfirstdevice:
        lda     #$00
        ; Fall through

_getnextdevice:
        sta     tmp1
next:   inc     tmp1
        lda     tmp1
        cmp     #$19            ; MAX DEVICE
        bcc     :+
        
        lda     #$FF            ; INVALID_DEVICE
        bne     done

        ; Check for valid device
:       jsr     isdevice
        bne     next
        lda     tmp1
done:
        ldx     #>$0000
        rts
