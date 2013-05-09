;
; Ullrich von Bassewitz, 22.11.2002
;
; size_t __fastcall__ fwrite (const void* buf, size_t size, size_t count, FILE* file);
; /* Write to a file */
;

        .export         _fwrite

        .import         _write
        .import         pushax, incsp6, addysp, ldaxysp, pushwysp, return0
        .import         tosumulax, tosudivax

        .importzp       ptr1

        .include        "errno.inc"
        .include        "_file.inc"


; ------------------------------------------------------------------------
; Code
                                                          
.proc   _fwrite

; Save file and place it into ptr1

        sta     file
        sta     ptr1
        stx     file+1
        stx     ptr1+1

; Check if the file is open

        ldy     #_FILE::f_flags
        lda     (ptr1),y
        and     #_FOPEN                 ; Is the file open?
        bne     @L2                     ; Branch if yes

; File not open

@L1:    lda     #EBADF
        jsr     __seterrno              ; Returns with A = 0
        tax                             ; A = X = 0
        jmp     incsp6

; Check if the stream is in an error state

@L2:    lda     (ptr1),y                ; get file->f_flags again
        and     #_FERROR
        bne     @L1

; Build the stackframe for write()

        ldy     #_FILE::f_fd
        lda     (ptr1),y
        ldx     #$00
        jsr     pushax                  ; file->f_fd

        ldy     #9
        jsr     pushwysp                ; buf

; Stack is now: buf/size/count/file->fd/buf
; Calculate the number of bytes to write: count * size

        ldy     #7
        jsr     pushwysp                ; count
        ldy     #9
        jsr     ldaxysp                 ; Get size
        jsr     tosumulax               ; count * size -> a/x

; Check if the number of bytes is zero. Don't call write in this case

        cpx     #0
        bne     @L3
        cmp     #0
        bne     @L3

; The number of bytes to write is zero, just return count

        ldy     #5
        jsr     ldaxysp                 ; Get count
        ldy     #10
        jmp     addysp                  ; Drop params, return

; Call write(). This will leave the original 3 params on the stack

@L3:    jsr     _write

; Check for errors in write

        cpx     #$FF
        bne     @L4
        cmp     #$FF
        bne     @L4

; Error in write. Set the stream error flag and bail out. errno is already
; set by write().

        lda     file
        sta     ptr1
        lda     file+1
        sta     ptr1+1
        ldy     #_FILE::f_flags
        lda     (ptr1),y
        ora     #_FERROR
        sta     (ptr1),y
        bne     @L1                     ; Return zero

; Write was ok. Return the number of items successfully written. Since we've
; checked for bytes == 0 above, size cannot be zero here, so the division is
; safe.

@L4:    jsr     pushax                  ; Push number of bytes written
        ldy     #5
        jsr     ldaxysp                 ; Get size
        jsr     tosudivax               ; bytes / size -> a/x
        jmp     incsp6                  ; Drop params, return

.endproc

; ------------------------------------------------------------------------
; Data

.bss
file:   .res    2

