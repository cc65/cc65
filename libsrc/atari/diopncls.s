;
; Christian Groessler, February 2005
;
; This file provides the _dio_open and _dio_close functions
; Since on the Atari no real open and close is necessary, they
; do not open or close something. The _dio_open sets the sector
; size of the drive which is later used by the _dio_read and
; _dio_write functions. To query the sector size, the _dio_open
; accesses the disk drive.
;
; dhandle_t     __fastcall__ dio_open  (unsigned char device);
; unsigned char __fastcall__ dio_close (dhandle_t handle);
;

        .export         _dio_open, _dio_close
        .export         sectsizetab
        .import         __oserror, __sio_call, _dio_read
        .import         pushax, addysp, subysp
        .importzp       ptr2, sp
        .include        "atari.inc"


.bss

sectsizetab:
        .res    NUMDRVS * sst_size

.code

; code for _dio_open

_inv_drive:
        lda     #NONDEV         ; non-existent device
        sta     __oserror
        lda     #0
        tax
        rts                     ; return NULL

_dio_open:

        cmp     #NUMDRVS        ; valid drive id?
        bcs     _inv_drive
        tay                     ; drive #
        asl     a               ; make index from drive id
        asl     a
        tax
        lda     #128                            ; preset sectsize
        sta     sectsizetab+sst_sectsize,x
        sta     sectsizetab+sst_flag,x          ; set flag that drive is "open"
        lda     #0
        sta     sectsizetab+sst_sectsize+1,x
        sta     __oserror                       ; success
        tya
        sta     sectsizetab+sst_driveno,x
        stx     ptr2
        lda     #<sectsizetab
        clc
        adc     ptr2
        sta     ptr2
        lda     #>sectsizetab
        adc     #0
        tax
        stx     ptr2+1          ; ptr2: pointer to sectsizetab entry

; query drive for current sector size
; procedure:
;   - read sector #4 (SIO command $54) to update drive status;
;     read length is 128 bytes, buffer is allocated on the stack,
;          sector data is ignored;
;     returned command status is ignored, we will get an error with
;          a DD disk anyway (read size 128 vs. sector size 256);
;   - issue SIO command $53 (get status) to retrieve the sector size;
;     use the DVSTAT system area as return buffer;
;     if the command returns with an error, set sector size to 128
;          bytes;
;

        ldy     #128
        jsr     subysp          ; allocate buffer on the stack

        lda     sp
        pha
        lda     sp+1
        pha                     ; save sp (buffer address) on processor stack

        lda     ptr2
        ldx     ptr2+1
        jsr     pushax          ; handle

        ldx     #0
        lda     #4
        jsr     pushax          ; sect_num

        pla
        tax
        pla                     ; AX - buffer address

                                ; sst_sectsize currently 128
        jsr     _dio_read       ; read sector to update status

        ldy     #128
        jsr     addysp          ; discard stack buffer

        lda     ptr2
        ldx     ptr2+1
        jsr     pushax          ; handle

        ldx     #0
        lda     #4
        jsr     pushax          ; dummy sector #, ignored by this SIO command,
                                ; but set to circumvent the special 1-3 sector
                                ; handling in __sio_call

        ldx     #>DVSTAT
        lda     #<DVSTAT
        jsr     pushax          ; buffer address

        ldy     #sst_sectsize
        lda     #4
        sta     (ptr2),y        ; 4 bytes transfer

        ldx     #%01000000      ; direction value
        lda     #SIO_STAT       ; get status

        jsr     __sio_call

        bmi     error

        ldy     #sst_sectsize
        lda     DVSTAT
        and     #%100000
        beq     s128

;s256
        lda     #0
        sta     (ptr2),y
        iny
        lda     #1

finish: sta     (ptr2),y        ; set default sector size       
fini2:  lda     ptr2
        ldx     ptr2+1
        rts

error:  ldy     #sst_sectsize
s128:   lda     #128
        bne     finish

; end of _dio_open


.proc   _dio_close

        sta     ptr2
        stx     ptr2+1
        lda     #0
        ldy     #sst_flag
        sta     (ptr2),y
        sta     __oserror       ; success
        tax
        rts                     ; return no error

.endproc
