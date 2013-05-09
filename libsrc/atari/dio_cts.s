;
; Christian Groessler, October 2000
;
; this file provides the _dio_phys_to_log function
; (previously called _dio_chs_to_snum, so the filename)
;
; on the Atari this function is a dummy, it ignores
; cylinder and head and returns as sector number the
; sector number it got
;
; unsigned char __fastcall__ dio_phys_to_log(dhandle_t handle,
;                                            dio_phys_pos *physpos,     /* input */
;                                            unsigned *sectnum);        /* output */
;
; dhandle_t - 16bit (ptr)
;

        .export         _dio_phys_to_log
        .import         popax,__oserror
        .importzp       ptr1,ptr2,ptr3
        .include        "atari.inc"

.proc   _dio_phys_to_log

        sta     ptr1
        stx     ptr1+1          ; pointer to result

        jsr     popax
        sta     ptr2
        stx     ptr2+1          ; pointer to input structure

        jsr     popax
        sta     ptr3
        stx     ptr3+1          ; pointer to handle

        ldy     #sst_flag
        lda     (ptr3),y
        and     #128
        beq     _inv_hand       ; handle not open or invalid

; ignore head and track and return the sector value

        ldy     #diopp_sector
        lda     (ptr2),y
        tax
        iny
        lda     (ptr2),y
        ldy     #1
        sta     (ptr1),y
        dey
        txa
        sta     (ptr1),y

        ldx     #0
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

