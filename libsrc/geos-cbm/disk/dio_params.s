;
; Maciej 'YTM/Elysium' Witkowiak
; 2.7.2001
;
; this function is used by dio_read and dio_write to fix parameters (secnum)
; this function calls SetDevice so that more than one drive can be used at once

            .export dio_params,dio_secnum
            .import popax,pushax,_dio_log_to_phys
            .importzp ptr1

            .include "geossym.inc"
            .include "jumptab.inc"
            .include "dio.inc"

.bss

dio_secnum:
        .res 2

.code

dio_params:
        sta r4L
        stx r4H

        jsr popax
        sta dio_secnum
        stx dio_secnum+1

        jsr popax               ; get 3rd parameter
        pha                     ; save it
        txa
        pha
        jsr pushax              ; put it back
        pla                     ; restore it
        sta ptr1+1
        pla
        sta ptr1
        ldy #sst_driveno
        lda (ptr1),y
        clc
        adc #8
        jsr SetDevice           ; setup device, load driver

        lda #<dio_secnum
        ldx #>dio_secnum
        jsr pushax

        lda #<r1L
        ldx #>r1H
        jmp _dio_log_to_phys
