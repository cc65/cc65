;
; Ullrich von Bassewitz, 2003-08-20
; Performance increase (about 20%) by
; Christian Krueger, 2009-09-13
;
; void* __fastcall__ memcpy (void* dest, const void* src, size_t n);
;
; NOTE: This function contains entry points for memmove, which will ressort
; to memcpy for an upwards copy. Don't change this module without looking
; at memmove!
;

        .export         _memcpy, memcpy_downwards, memcpy_getparams
        .import         popax, popptr1
        .importzp       sp, ptr1, ptr2, ptr3
        .macpack        generic

; ----------------------------------------------------------------------
_memcpy:
        jsr     memcpy_getparams

memcpy_downwards:
; Copy downwards. Adjust the pointers to the end of the memory regions.

        lda     ptr1+1
        add     ptr3+1
        sta     ptr1+1

        lda     ptr2+1
        add     ptr3+1
        sta     ptr2+1

; handle fractions of a page size first

        ldy     ptr3            ; count, low byte
        bne     @entry          ; something to copy?
        beq     PageSizeCopy    ; here like bra...

@copyByte:
        lda     (ptr1),y
        sta     (ptr2),y
@entry:
        dey
        bne     @copyByte
        lda     (ptr1),y        ; copy remaining byte
        sta     (ptr2),y

PageSizeCopy:                   ; assert Y = 0
        ldx     ptr3+1          ; number of pages
        beq     done            ; none? -> done

@initBase:
        dec     ptr1+1          ; adjust base...
        dec     ptr2+1
        dey                     ; in entry case: 0 -> FF
@copyBytes:
        .repeat 3               ; unroll this a bit to make it faster...
        lda     (ptr1),y        ; important: unrolling three times gives a nice
        sta     (ptr2),y        ; 255/3 = 85 loop which ends at 0
        dey
        .endrepeat
@copyEntry:                     ; in entry case: 0 -> FF
        bne     @copyBytes
        lda     (ptr1),y        ; Y = 0, copy last byte
        sta     (ptr2),y
        dex                     ; one page to copy less
        bne     @initBase       ; still a page to copy?

done:   jmp     popax           ; Pop ptr and return as result

; ----------------------------------------------------------------------
; Get the parameters from stack as follows:
;
;       size            --> ptr3
;       src             --> ptr1
;       dest            --> ptr2
;       First argument (dest) will remain on stack and is returned in a/x!

memcpy_getparams:               ; IMPORTANT! Function has to leave with Y=0!
        sta     ptr3
        stx     ptr3+1          ; save n to ptr3

        jsr     popptr1         ; save src to ptr1

                                ; save dest to ptr2
        iny                     ; Y=0 guaranteed by popptr1, we need '1' here...
                                ; (direct stack access is three cycles faster
                                ; (total cycle count with return))
        lda     (sp),y
        tax
        stx     ptr2+1          ; save high byte of ptr2
        dey                     ; Y = 0
        lda     (sp),y          ; Get ptr2 low
        sta     ptr2
        rts
