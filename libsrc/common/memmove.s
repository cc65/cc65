;
; Ullrich von Bassewitz, 2003-08-20
;
; void* __fastcall__ memmove (void* dest, const void* src, size_t size);
;
; NOTE: This function uses entry points from memcpy!
;

       	.export	    	_memmove
        .import         memcpy_getparams, memcpy_upwards
       	.importzp      	ptr1, ptr2, ptr3, ptr4, tmp1

        .macpack        generic
        .macpack        longbranch

; ----------------------------------------------------------------------
_memmove:
        sta     ptr4
        stx     ptr4+1          ; Size -> ptr4

        jsr     memcpy_getparams

; Check for the copy direction. If dest < src, we must copy upwards (start at
; low addresses and increase pointers), otherwise we must copy downwards
; (start at high addresses and decrease pointers).

        sec
        sbc     ptr1
        txa
        sbc     ptr1+1
        jcc     memcpy_upwards  ; Branch if dest < src (upwards copy)

; Copy downwards. Adjust the pointers to the end of the memory regions.

        lda	ptr1+1
       	add	ptr4+1
	sta	ptr1+1

	lda	ptr2+1
	add	ptr4+1
	sta	ptr2+1

; Load the low offset into Y, and the counter low byte into X.

        ldy     ptr4
        ldx     ptr3
        jmp     @L2

; Copy loop

@L1:    dey
        lda     (ptr1),y
        sta     (ptr2),y

@L2:    inx                     ; Bump counter low byte
        bne     @L1
        dec     ptr1+1
        dec     ptr2+1
        inc     ptr3+1          ; Bump counter high byte
        bne     @L1

; Done, return dest

done:  	lda	ptr2
       	ldx    	tmp1            ; get function result (dest)
       	rts

