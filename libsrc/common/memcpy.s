;
; Ullrich von Bassewitz, 2003-08-20
;
; void* __fastcall__ memcpy (void* dest, const void* src, size_t n);
;
; NOTE: This function contains entry points for memmove, which will ressort
; to memcpy for an upwards copy. Don't change this module without looking
; at memmove!
;

       	.export	    	_memcpy, memcpy_upwards, memcpy_getparams
       	.import	    	popax
       	.importzp      	ptr1, ptr2, ptr3, tmp1

; ----------------------------------------------------------------------
_memcpy:
        jsr     memcpy_getparams

memcpy_upwards:
        ldy     #0
        ldx     ptr3            ; Get low counter byte

; Copy loop

@L1:    inx                     ; Bump low counter byte
        beq     @L3             ; Jump on overflow
@L2:    lda     (ptr1),y
        sta     (ptr2),y
        iny
        bne     @L1
       	inc   	ptr1+1		; Bump pointers
       	inc   	ptr2+1
        bne     @L1             ; Branch always
@L3:    inc     ptr3+1          ; Bump high counter byte
        bne     @L2

; Done. The low byte of dest is still in ptr2

done:  	lda	ptr2
       	ldx    	tmp1            ; get function result (dest)
       	rts

; ----------------------------------------------------------------------
; Get the parameters from stack as follows:
;
;       -(size-1)       --> ptr3
;       src             --> ptr1
;       dest            --> ptr2
;       high(dest)      --> tmp1
;
; dest is returned in a/x.

memcpy_getparams:
        eor     #$FF
        sta     ptr3
        txa
        eor     #$FF
        sta     ptr3+1          ; Save -(size-1)

       	jsr	popax		; src
       	sta	ptr1
       	stx	ptr1+1

       	jsr	popax		; dest
  	sta	ptr2
  	stx	ptr2+1		; Save work copy
        stx     tmp1            ; Save for function result

        rts

