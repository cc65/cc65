;
; Christian Groessler, October 2000
;
; this file provides the _dio_log_to_phys function
; (previously called _dio_snum_to_chs, so the filename)
;
; on the Atari this function is a dummy, it returns
; cylinder and head 0 and as sector the sectnum it got
;
; unsigned char __fastcall__ dio_log_to_phys(dhandle_t handle,
;                                            unsigned *sectnum,         /* input */
;                                            dio_phys_pos *physpos);    /* output */
;
; dhandle_t - 16bit (ptr)
;

        .export         _dio_log_to_phys
        .include        "atari.inc"
        .importzp       ptr1,ptr2,ptr3
        .import         popax,__oserror

.proc   _dio_log_to_phys

        sta     ptr2
        stx     ptr2+1          ; pointer to output structure

        jsr     popax
        sta     ptr1
        stx     ptr1+1          ; save pointer to input data

        jsr     popax
        sta     ptr3
        stx     ptr3+1          ; pointer to handle

        ldy     #sst_flag
        lda     (ptr3),y
        and     #128
        beq     _inv_hand       ; handle not open or invalid

        lda     #0
        tay
        tax
        sta     (ptr2),y        ; head
        iny
        sta     (ptr2),y        ; track (low)
        iny
        sta     (ptr2),y        ; track (high)
        iny

        lda     (ptr1,x)
        sta     (ptr2),y
        iny
        inc     ptr1
        bne     _l1
        inc     ptr1+1
_l1:    lda     (ptr1,x)
        sta     (ptr2),y

        txa
ret:
        sta     __oserror
        rts                     ; return success

; invalid handle

_inv_hand:
        ldx     #0
        lda     #BADIOC
        bne     ret

.endproc

