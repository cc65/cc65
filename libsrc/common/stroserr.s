;
; Ullrich von Bassewitz, 17.07.2002
;
; const char* __fastcall__ __stroserror (unsigned char errcode);
; /* Map an operating system error number to an error message. */
;

        .export         ___stroserror
        .import         __sys_oserrlist
        .importzp       ptr1, tmp1

        .macpack        generic


; The table is built as a list of entries
;
;       .byte   entrylen
;       .byte   errorcode
;       .asciiz errormsg
;
; and terminated by an entry with length zero that is returned if the
; error code could not be found.

___stroserror:
        sta     tmp1                    ; Save the error code

        ldy     #<__sys_oserrlist
        sty     ptr1
        ldy     #>__sys_oserrlist
        sty     ptr1+1                  ; Setup pointer to message table

@L1:    ldy     #0
        lda     (ptr1),y                ; Get the length
        beq     Done                    ; Bail out if end of list reached

        iny
        lda     (ptr1),y                ; Compare the error code
        cmp     tmp1
        beq     Done                    ; Jump if found

; Not found, move pointer to next entry

        dey
        clc
        lda     ptr1
        adc     (ptr1),y
        sta     ptr1
        bcc     @L1
        inc     ptr1+1
        bcs     @L1                     ; Branch always

; We've found the code or reached the end of the list

Done:   ldx     ptr1+1
        lda     ptr1
        add     #2                      ; Add a total of #2
        bcc     @L1
        inx                             ; Bump high byte
@L1:    rts


