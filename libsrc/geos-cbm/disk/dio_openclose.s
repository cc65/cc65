;
; Maciej 'YTM/Elysium' Witkowiak
;
; based on Atari version by Christian Groessler
; 2.7.2001
;
; dhandle_t     __fastcall__ dio_open  (unsigned char device);
; unsigned char __fastcall__ dio_close (dhandle_t handle);
;
; dio_open sets given device as current and initializes disk
; dio_close does nothing special

            .export _dio_open, _dio_close
            .import __oserror, _OpenDisk
            .importzp ptr1, tmp1

            .include "dio.inc"
            .include "jumptab.inc"
            .include "geossym.inc"
            .include "const.inc"

.bss

sectsizetab:
        .res 4 * sst_size       ; this is hardcoded

.code

_dio_open:
        pha
        tax
        lda driveType,x         ; check if there's a device
        beq _inv_drive
        txa
        clc
        adc #8                  ; normalize devnum
        sta curDevice
        jsr SetDevice
        jsr _OpenDisk           ; take care for errors there

        pla
        tay                     ; drive #
        asl a                   ; make index from drive id
        asl a
        tax
            
        lda #0
        sta sectsizetab+sst_sectsize,x
        lda #128
        sta sectsizetab+sst_flag,x ; set flag that drive is "open"
        lda #1
        sta sectsizetab+sst_sectsize+1,x
        tya
        sta sectsizetab+sst_driveno,x
            
        stx tmp1
        lda #<sectsizetab
        clc
        adc tmp1
        sta tmp1
        lda #>sectsizetab
        adc #0
        tax
        lda tmp1

        rts

_inv_drive:
        lda #DEV_NOT_FOUND
        sta __oserror
        lda #0
        tax
        rts

_dio_close:
        sta ptr1
        stx ptr1+1
        lda #0
        ldy #sst_flag
        sta (ptr1),y
        sta __oserror           ; success
        tax
        rts                     ; return no error
