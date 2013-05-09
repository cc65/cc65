;
; Low level file I/O routines, ONLY for module loading OR sth similar
;
; Maciej 'YTM/Elysium' Witkowiak <ytm@elysium.pl>
; 25.12.2002
;
; only ONE opened file at a time, only O_RDONLY flag

; int open (const char* name, int flags, ...);  /* May take a mode argument */
; int __fastcall__ close (int fd);
; int __fastcall__ read (int fd, void* buf, unsigned count);

FILEDES         = 3             ; first free to use file descriptor

            .importzp ptr1, ptr2, ptr3, tmp1
            .import addysp, popax
            .import __oserror
            .import _FindFile, _ReadByte
            .export _open, _close, _read

            .include "geossym.inc"
            .include "const.inc"
            .include "errno.inc"
            .include "fcntl.inc"

_open:
        cpy #4                  ; correct # of arguments (bytes)?
        beq @parmok             ; parameter count ok
        tya                     ; parm count < 4 shouldn't be needed to be...
        sec                     ; ...checked (it generates a c compiler warning)
        sbc #4
        tay
        jsr addysp              ; fix stack, throw away unused parameters

; Parameters ok. Pop the flags and save them into tmp3

@parmok:
        jsr popax               ; Get flags
        sta tmp1
        jsr popax               ; Get name
        sta ptr1
        stx ptr1+1
            
        lda filedesc            ; is there a file already open?
        bne @alreadyopen
            
        lda tmp1                ; check open mode
        and #(O_RDWR | O_CREAT)
        cmp #O_RDONLY           ; only O_RDONLY is valid
        bne @badmode
            
        lda ptr1
        ldx ptr1+1
        jsr _FindFile           ; try to find the file
        tax
        bne @oserror
            
        lda dirEntryBuf + OFF_DE_TR_SC ; tr&se for ReadByte (r1)
        sta f_track
        lda dirEntryBuf + OFF_DE_TR_SC + 1
        sta f_sector
        lda #<diskBlkBuf        ; buffer for ReadByte (r4)
        sta f_buffer
        lda #>diskBlkBuf
        sta f_buffer+1
        ldx #0                  ; offset for ReadByte (r5)
        stx f_offset
        stx f_offset+1
        lda #0                  ; clear errors
        sta __oserror
        jsr __seterrno
        lda #FILEDES            ; return fd
        sta filedesc
        rts
@badmode:
        lda #EINVAL             ; invalid parameters - invalid open mode
        .byte $2c               ; skip
@alreadyopen:
        lda #EMFILE             ; too many opened files (there can be only one)
        jmp __directerrno       ; set errno, clear oserror, return -1
@oserror:
        jmp __mappederrno       ; set platform error code, return -1

_close:
        lda #0
        sta __oserror
        jsr __seterrno          ; clear errors
        lda #0                  ; clear fd
        sta filedesc
        tax
        rts

_read:
    ; a/x - number of bytes
    ; popax - buffer ptr
    ; popax - fd, must be == to the above one
    ; return -1+__oserror or number of bytes read

        eor #$ff
        sta ptr1
        txa
        eor #$ff
        sta ptr1+1              ; -(# of bytes to read)-1
        jsr popax
        sta ptr2
        stx ptr2+1              ; buffer ptr
        jsr popax
        cmp #FILEDES            ; lo-byte == FILEDES
        bne @filenotopen
        txa                     ; hi-byte == 0
        beq @fileok             ; fd must be == FILEDES

@filenotopen:
        lda #EBADF
        jmp __directerrno       ; Sets _errno, clears _oserror, returns -1

@fileok:
        lda #0
        sta ptr3
        sta ptr3+1              ; put 0 into ptr3 (number of bytes read)
        sta __oserror           ; clear error flags
        jsr __seterrno

        lda f_track             ; restore stuff for ReadByte
        ldx f_sector
        sta r1L
        stx r1H
        lda f_buffer
        ldx f_buffer+1
        sta r4L
        stx r4H
        lda f_offset
        ldx f_offset+1
        sta r5L
        stx r5H

        clc
        bcc @L3                 ; branch always

@L0:    jsr _ReadByte
        ldy #0                  ; store the byte
        sta (ptr2),y
        inc ptr2                ; increment target address
        bne @L1
        inc ptr2+1

@L1:    inc ptr3                ; increment byte count
        bne @L2
        inc ptr3+1

@L2:    lda __oserror           ; was there error ?
        beq @L3
        cmp #BFR_OVERFLOW       ; EOF?
        beq @done               ; yes, we're done
        jmp __mappederrno       ; no, we're screwed

@L3:    inc ptr1                ; decrement the count
        bne @L0
        inc ptr1+1
        bne @L0

@done:
        lda r1L                 ; preserve data for ReadByte
        ldx r1H
        sta f_track
        stx f_sector
        lda r4L
        ldx r4H
        sta f_buffer
        stx f_buffer+1
        lda r5L
        ldx r5H
        sta f_offset
        stx f_offset+1

        lda ptr3                ; return byte count
        ldx ptr3+1
        rts

.bss

filedesc:
        .res 1                  ; file open flag - 0 (no file opened) or 1
f_track:
        .res 1                  ; values preserved for ReadByte
f_sector:
        .res 1
f_offset:
        .res 2
f_buffer:
        .res 2
