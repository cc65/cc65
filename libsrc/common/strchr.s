;
; Ullrich von Bassewitz, 31.05.1998
; Christian Krueger, 2013-Aug-04, minor optimization
;
; const char* strchr (const char* s, int c);
;

        .export         _strchr
        .import         popax
        .importzp       ptr1, tmp1
        .macpack        cpu

_strchr:
        sta tmp1        ; Save c
        jsr popax       ; get s
        tay             ; low byte of pointer to y
        stx ptr1+1
.if (.cpu .bitand ::CPU_ISET_65SC02)
        stz ptr1
.else
        lda #0
        sta ptr1        ; access from page start, y contains low byte
.endif

Loop:   lda (ptr1),y    ; Get next char
        beq EOS         ; Jump on end of string
        cmp tmp1        ; Found?
        beq Found       ; Jump if yes
        iny
        bne Loop
        inc ptr1+1
        bne Loop        ; Branch always

; End of string. Check if we're searching for the terminating zero

EOS:
        lda tmp1        ; Get the char we're searching for
        bne NotFound    ; Jump if not searching for terminator

; Found. Set pointer to c.

Found:
        ldx ptr1+1      ; Load high byte of pointer
        tya             ; low byte is in y
        rts

; Not found, return NULL

NotFound:
        lda #0
        tax
        rts
