;
; Maciej 'YTM/Elysium' Witkowiak
; 2.7.2001
;
;
; unsigned char __fastcall__ dio_phys_to_log (dhandle_t handle,
;                                             dio_phys_pos *physpos,    /* input */
;                                             unsigned *sectnum);       /* output */
;

            .export _dio_phys_to_log
            .export sectab_1541_l, sectab_1541_h        ; for log_to_phys
            .import popax,__oserror
            .importzp ptr1,ptr2,ptr3,tmp1,tmp2,tmp3,tmp4

            .include "dio.inc"
            .include "geossym.inc"
            .include "const.inc"

_dio_phys_to_log:
        sta ptr1
        stx ptr1+1              ; pointer to result
            
        jsr popax
        sta ptr2
        stx ptr2+1              ; pointer to input structure
            
        jsr popax
        sta ptr3
        stx ptr3+1              ; pointer to handle
            
        ldy #sst_flag
        lda (ptr3),y
        and #128
        beq _inv_hand           ; handle not open or invalid
            
            
        ldy #diopp_head
        lda (ptr2),y
        bne _inv_data           ; there is only head 0
        ldy #diopp_track
        lda (ptr2),y
        beq _inv_data           ; there is no track 0
        sta tmp1
        iny
        lda (ptr2),y
        bne _inv_data           ; there are no more than 256 tracks
        dec tmp1                ; normalize track to start from 0
        ldy #diopp_sector
        lda (ptr2),y
        sta tmp2
        iny
        lda (ptr2),y
        bne _inv_data           ; there are no more than 256 sectors

; tmp1 (int) holds track+sector, translate it using device info

        ldy #sst_driveno
        lda (ptr3),y
        tay
        lda driveType,y
        and #%00000011          ; this is for RamDrive compatibility
        cmp #DRV_1541
        beq dio_cts1541
        cmp #DRV_1571
        beq dio_cts1571
        cmp #DRV_1581
        beq dio_cts1581
            
        lda #DEV_NOT_FOUND      ; unknown device
        ldx #0
        beq ret

dio_ctsend:
        ldy #1
        lda tmp2
        sta (ptr1),y
        dey
        lda tmp1
        sta (ptr1),y
            
        ldx #0
        txa
ret:        
        sta __oserror
        rts                     ; return success

; errors

_inv_data:
        lda #INV_TRACK
        .byte $2c
_inv_hand:
        lda #INCOMPATIBLE
        ldx #0
        beq ret

; device-depended stuff, tmp1=track-1, tmp2=sector

dio_cts1541:
        ldy tmp1
        cpy #35
        bcs _inv_data
        lda sectab_1541_l,y
        clc
        adc tmp2
        sta tmp1
        lda sectab_1541_h,y
        adc #0
        sta tmp2
        jmp dio_ctsend

dio_cts1571:
        lda tmp1
        cmp #70
        bcs _inv_data
        cmp #35                 ; last track of one side
        bcs _sub35
        jmp dio_cts1541         ; track <=35 - same as 1541

_sub35:
        sec
        sbc #35
        sta tmp1
        jsr dio_cts1541         ; get offset on second side of disk
        lda tmp1                ; add second side base
        clc
        adc #<683
        sta tmp1
        lda tmp2
        adc #>683
        sta tmp2
        jmp dio_ctsend

dio_cts1581:
; 1581 has 80 tracks, 40 sectors each secnum=track*40+sector
        ldx #0
        stx tmp3
        stx tmp4
        lda tmp1
        beq _nomult
        cmp #80
        bcs _inv_data

; mul40 by Christian Groessler
        sta tmp4
        asl a
        rol tmp3
        asl a
        rol tmp3                ; val * 4
        adc tmp4
        bcc L1
        inc tmp3                ; val * 5
L1:     asl a
        rol tmp3                ; val * 10
        asl a
        rol tmp3
        asl a
        rol tmp3                ; val * 40 = AX
        ldx tmp3
        sta tmp3
        stx tmp4

_nomult:
        lda tmp2
        clc
        adc tmp3
        sta tmp1
        lda tmp4
        adc #0
        sta tmp2
        jmp dio_ctsend

.rodata

sectab_1541_l:
        .byte $00, $15, $2a, $3f, $54, $69, $7e, $93
        .byte $a8, $bd, $d2, $e7, $fc, $11, $26, $3b
        .byte $50, $65, $78, $8b, $9e, $b1, $c4, $d7
        .byte $ea, $fc, $0e, $20, $32, $44, $56, $67
        .byte $78, $89, $9a, $ab
sectab_1541_h:
        .byte $00, $00, $00, $00, $00, $00, $00, $00
        .byte $00, $00, $00, $00, $00, $01, $01, $01
        .byte $01, $01, $01, $01, $01, $01, $01, $01
        .byte $01, $01, $02, $02, $02, $02, $02, $02
        .byte $02, $02, $02, $02
