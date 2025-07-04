;
; Oliver Schmidt, 2012-09-04
;
; unsigned char getfirstdevice (void);
; unsigned char __fastcall__ getnextdevice (unsigned char device);
;

        .export         _getfirstdevice
        .export         _getnextdevice
        .import         isdisk
        .import         opencmdchannel
        .import         closecmdchannel
        .import         initst
        .import         READST
        .importzp       tmp2

;------------------------------------------------------------------------------
; _getfirstdevice

_getfirstdevice:
        lda     #$FF
        ; Fall through

;------------------------------------------------------------------------------
; _getnextdevice

_getnextdevice:
        tax
next:   inx
        cpx     #$FF
        beq     done

; [open|close]cmdchannel already call isdisk internally but they
; interpret a non-disk as a no-op while we need to interpret it
; as an error here

        jsr     isdisk  ; carry clear if the unit number in X is a disk
        bcs     next

; [open|close]cmdchannel don't call into the Kernal at all if they
; only [in|de]crement the reference count of the shared cmdchannel
; so we need to explicitly initialize ST here

        stx     tmp2    ; further calls my use X

        jsr     initst

        ldx     tmp2    ; get unit number back
        jsr     opencmdchannel
        ldx     tmp2    ; get unit number back
        jsr     closecmdchannel

        ldx     tmp2    ; get unit number back

        jsr     READST  ; preserves X, returns A and Flags

; Either the Kernal calls above were successful or there was
; already a cmdchannel to the device open - which is a pretty
; good indication of its existence ;-)

        bmi     next

done:   txa
        ldx     #$00
        rts
