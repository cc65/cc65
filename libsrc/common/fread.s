;
; Ullrich von Bassewitz, 2002-11-22, 2004-05-14
;
; size_t __fastcall__ fread (void* buf, size_t size, size_t count, FILE* file);
; /* Read from a file */
;

        .export         _fread

        .import         _read
        .import         pusha0, pushax
        .import         incsp4, incsp6
        .import         ldaxysp, ldax0sp
        .import         pushwysp
        .import         tosumulax, tosudivax

        .importzp       ptr1, c_sp

        .include        "errno.inc"
        .include        "_file.inc"

        .macpack        generic

; ------------------------------------------------------------------------
; Code

.proc   _fread

; We will place a pointer to the file descriptor into the register bank

        .import regbank: zp
        file    = regbank

; Save two bytes from the register bank

        ldy     file+0
        sty     save
        ldy     file+1
        sty     save+1

; Save the file pointer into the register bank

        sta     file
        stx     file+1

; Check if the file is open

        ldy     #_FILE::f_flags
        lda     (file),y
        .if .cap(CPU_HAS_BITIMM)
        bit     #_FOPEN                 ; Is the file open?
        .else
        and     #_FOPEN                 ; Is the file open?
        .endif
        beq     @L1                     ; Branch if no

; Check if the stream is in an error state

        .if .cap(CPU_HAS_BITIMM)
        bit     #_FERROR
        .else
        lda     (file),y                ; get file->f_flags again
        and     #_FERROR
        .endif
        beq     @L2

; File not open or in error state

@L1:    lda     #EINVAL
        jsr     ___seterrno              ; Set __errno, return zero in A
        tax                             ; a/x = 0
        jmp     @L99                    ; Bail out

; Remember if we have a pushed back character and reset the flag.

@L2:    .if .cap(CPU_HAS_BITIMM)
        ldx     #$00
        bit     #_FPUSHBACK
        beq     @L3
        .else
        tax                             ; X = 0
        lda     (file),y
        and     #_FPUSHBACK
        beq     @L3
        lda     (file),y
        .endif
        and     #<~_FPUSHBACK
        sta     (file),y                ; file->f_flags &= ~_FPUSHBACK;
        inx                             ; X = 1
@L3:    stx     pb

; Build the stackframe for read()

        ldy     #_FILE::f_fd
        lda     (file),y
        jsr     pusha0                  ; file->f_fd

        ldy     #9
        jsr     pushwysp                ; buf

; Stack is now: buf/size/count/file->fd/buf
; Calculate the number of bytes to read: count * size

        ldy     #7
        jsr     pushwysp                ; count
        ldy     #9
        jsr     ldaxysp                 ; Get size
        jsr     tosumulax               ; count * size -> a/x

; Check if count is zero.

        cmp     #0
        bne     @L4
        cpx     #0
        bne     @L4

; Count is zero, drop the stack frame just built and return count

        jsr     incsp4                  ; Drop file->fd/buf
        jsr     ldax0sp                 ; Get count
        jmp     @L99                    ; Bail out

; Check if we have a buffered char from ungetc

@L4:    ldy     pb
        beq     @L6

; We have a buffered char from ungetc. Save the low byte from count

        pha

; Copy the buffer pointer into ptr1, and increment the pointer value passed
; to read() by one, so read() starts to store data at buf+1.

        .if .cap(CPU_HAS_ZPIND)
        lda     (c_sp)
        sta     ptr1
        add     #1
        sta     (c_sp)
        ldy     #1
        .else
        ldy     #0
        lda     (c_sp),y
        sta     ptr1
        add     #1
        sta     (c_sp),y
        iny
        .endif
        lda     (c_sp),y
        sta     ptr1+1
        adc     #0
        sta     (c_sp),y                ; ptr1 = buf++;

; Get the buffered character and place it as first character into the read
; buffer.

        ldy     #_FILE::f_pushback
        lda     (file),y
        .if .cap(CPU_HAS_ZPIND)
        sta     (ptr1)                  ; *buf = file->f_pushback;
        .else
        ldy     #0
        sta     (ptr1),y                ; *buf = file->f_pushback;
        .endif

; Restore the low byte of count and decrement count by one. This may result
; in count being zero, so check for that.

        pla
        sub     #1
        bcs     @L5
        dex
@L5:    cmp     #0
        bne     @L6
        cpx     #0
        beq     @L8

; Call read(). This will leave the original 3 params on the stack

@L6:    jsr     _read

; Check for errors in read

        cpx     #$FF
        bne     @L8
        cmp     #$FF
        bne     @L8

; Error in read. Set the stream error flag and bail out. errno has already
; been set by read(). On entry to label @L7, X must be zero.

        inx                             ; X = 0
        lda     #_FERROR
@L7:    ldy     #_FILE::f_flags         ; X must be zero here!
        ora     (file),y
        sta     (file),y
        txa                             ; a/x = 0
        beq     @L99                    ; Return zero

; Read was ok, account for the pushed back character (if any).

@L8:    add     pb
        bcc     @L9
        inx

; Check for end of file.

@L9:    cmp     #0                      ; Zero bytes read?
        bne     @L10
        cpx     #0
        bne     @L10

; Zero bytes read. Set the EOF flag

        lda     #_FEOF
        bne     @L7                     ; Set flag and return zero

; Return the number of items successfully read. Since we've checked for
; bytes == 0 above, size cannot be zero here, so the division is safe.

@L10:   jsr     pushax                  ; Push number of bytes read
        ldy     #5
        jsr     ldaxysp                 ; Get size
        jsr     tosudivax               ; bytes / size -> a/x
@L99:   ldy     save                    ; Restore zp register
        sty     file
        ldy     save+1
        sty     file+1
        jmp     incsp6                  ; Drop params, return

.endproc

; ------------------------------------------------------------------------
; Data

.bss
save:   .res    2
pb:     .res    1
