;
; Ullrich von Bassewitz, 22.11.2002
;
; size_t __fastcall__ fread (void* buf, size_t size, size_t count, FILE* f);
; /* Read from a file */
;

        .export         _fread

        .import         _read
        .import         pushax, incsp6, addysp, ldaxysp, pushwysp, return0
        .import         tosumulax, tosudivax

        .importzp       ptr1, tmp1

        .include        "errno.inc"
        .include        "_file.inc"


; ------------------------------------------------------------------------
; Code

.proc   _fread

; Save f and place it into ptr1

	sta  	f
	sta  	ptr1
	stx  	f+1
	stx  	ptr1+1

; Check if the file is open

	ldy  	#_FILE_f_flags
	lda  	(ptr1),y
	and  	#_FOPEN		      	; Is the file open?
       	bne    	@L2			; Branch if yes

; File not open

   	lda  	#EINVAL
   	sta  	__errno
   	lda  	#0
   	sta  	__errno+1
@L1:    jsr     incsp6
        jmp     return0

; Check if the stream is in an error state

@L2:	lda  	(ptr1),y		; get f->f_flags again
	and  	#_FERROR
	bne     @L1

; Build the stackframe for read()

        ldy     #_FILE_f_fd
        lda     (ptr1),y
        ldx     #$00
        jsr     pushax                  ; f->f_fd

        ldy     #9
        jsr     pushwysp                ; buf

; Stack is now: buf/size/count/f->fd/buf
; Calculate the number of bytes to read: count * size

        ldy     #7
        jsr     pushwysp                ; count
        ldy     #9
        jsr     ldaxysp                 ; Get size
        jsr     tosumulax               ; count * size -> a/x

; Check if the number of bytes is zero. Don't call read in this case

        cpx     #0
        bne     @L3
        cmp     #0
        bne     @L3

; The number of bytes to read is zero, just return count

        ldy     #5
        jsr     ldaxysp                 ; Get count
        ldy     #10
        jmp     addysp                  ; Drop params, return

; Call read(). This will leave the original 3 params on the stack

@L3:    jsr     pushax
        jsr     _read

; Check for errors in read

        cpx     #$FF
        bne     @L5
        cmp     #$FF
        bne     @L5

; Error in read. Set the stream error flag and bail out. _oserror and/or
; errno are already set by read().

        lda     #_FERROR
@L4:    sta     tmp1
        lda     f
        sta     ptr1
        lda     f+1
        sta     ptr1+1
        ldy     #_FILE_f_flags
        lda     (ptr1),y
        ora     tmp1
        sta     (ptr1),y
        bne     @L1                     ; Return zero

; Read was ok, check for end of file.

@L5:    cmp     #0                      ; Zero bytes read?
        bne     @L6
        cpx     #0
        bne     @L6

; Zero bytes read. Set the EOF flag

        lda     #_FEOF
        bne     @L4                     ; Set flag and return zero

; Return the number of items successfully read. Since we've checked for
; bytes == 0 above, size cannot be zero here, so the division is safe.

@L6:    jsr     pushax                  ; Push number of bytes read
        ldy     #5
        jsr     ldaxysp                 ; Get size
        jsr     tosudivax               ; bytes / size -> a/x
        jmp     incsp6                  ; Drop params, return

.endproc

; ------------------------------------------------------------------------
; Data

.bss
f:	.res	2

