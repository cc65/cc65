;
; 2001-07-02, Maciej 'YTM/Elysium' Witkowiak
; 2015-08-27, Greg King
;
; unsigned char __fastcall__ dio_log_to_phys (dhandle_t handle,
;                                             unsigned *sectnum,        /* input */
;                                             dio_phys_pos *physpos);   /* output */
;

            .export _dio_log_to_phys
            .importzp ptr1,ptr2,ptr3,tmp1,tmp2
            .import popax,__oserror
            .import sectab_1541_l, sectab_1541_h

            .include "dio.inc"
            .include "geossym.inc"
            .include "const.inc"

_dio_log_to_phys:
; check device type
        sta ptr1
        stx ptr1+1              ; pointer to result (struct dio_phys_pos)
            
        jsr popax
        sta ptr2
        stx ptr2+1              ; pointer to input structure (pointer to int)
            
        jsr popax
        sta ptr3
        stx ptr3+1              ; pointer to handle
            
        ldy #sst_flag
        lda (ptr3),y
        and #128
        beq _inv_hand           ; handle not open or invalid

; fill in all we have
        ldy #diopp_head
        lda #0                  ; head 0
        sta (ptr1),y
        ldy #diopp_track+1
        sta (ptr1),y            ; track <256
        ldy #diopp_sector+1
        sta (ptr1),y            ; sector <256
            
        ldy #0
        lda (ptr2),y
        sta tmp1
        iny 
        lda (ptr2),y
        sta tmp2

; get drive info
        ldy #sst_driveno
        lda (ptr3),y
        tay 
        lda driveType,y
        and #%00001111          ; remove ramDisk flags
        cmp #DRV_1541
        beq dio_stc1541
        cmp #DRV_1571
        beq dio_stc1571
        cmp #DRV_1581
        beq dio_stc1581
            
        lda #INCOMPATIBLE       ; unsupported device
        ldx #0
        beq _ret

dio_stcend:
        ldy #diopp_track
        lda tmp1
        sta (ptr1),y
        ldy #diopp_sector
        lda tmp2
        sta (ptr1),y
            
        ldx #0
        txa 
_ret:       
        sta __oserror
        rts                     ; return success

; errors
_inv_data:
        lda #INV_TRACK
        .byte $2c
_inv_hand:
        lda #DEV_NOT_FOUND
        ldx #0
        beq _ret

dio_stc1541:
; if 1541:
; - compare with table to find track
; - subtract and find sector

        ldx #0                  ; index=(track-1)
_loop41:
        lda tmp2
        cmp sectab_1541_h+1,x
        bne _nxt
        lda tmp1
        cmp sectab_1541_l+1,x
_nxt:   bcc _found
        inx
        cpx #35
        bne _loop41
        beq _inv_data
            
_found:     
        lda tmp1
        sec 
        sbc sectab_1541_l,x
        sta tmp2
_fndend:    
        inx 
        stx tmp1
        jmp dio_stcend

dio_stc1571:
; if 1571:
; - check size, if too big - subtract and add 35 to track
; - fall down to 1541
        lda tmp2
        cmp #>683
        bne _if71
        lda tmp1
        cmp #<683
_if71:  bcc dio_stc1541
            
        lda tmp1
        sec 
        sbc #<683
        sta tmp1
        lda tmp2
        sbc #>683
        sta tmp2
        jsr dio_stc1541         ; will fall through here
        tay
        bne _ret                ; result beyond track 70
            
        ldy #diopp_track
        lda (ptr1),y
        clc 
        adc #35
        sta (ptr1),y
        lda #0
        beq _ret

; if 1581:
; - subtract 40 in loop (at most 80 times) to find track
; - the remainder is sector
dio_stc1581:
        ldx #0                  ; index=(track-1)
_loop81:    
        lda tmp2
        bne _sub81
        lda tmp1
        cmp #40
        bcc _got81
_sub81: lda tmp1
        sec 
        sbc #40
        sta tmp1
        lda tmp2
        sbc #0
        sta tmp2
        inx 
        cpx #80
        bne _loop81
        beq _inv_data
            
_got81: lda tmp1
        sta tmp2
        inx 
        stx tmp1
        jmp dio_stcend
