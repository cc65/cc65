;
; Oliver Schmidt, 2012-09-10
;
; char* __fastcall__ getdevicedir (unsigned char device, char* buf, size_t size);
;

        .export         _getdevicedir
        .import         diskinit, devicestr, fnunit
        .import         popa, popax
        .importzp       ptr2, ptr3

        .include        "errno.inc"

;------------------------------------------------------------------------------
; _getdevicedir

.proc   _getdevicedir

; Save size

        sta     ptr3
        stx     ptr3+1

; Save buf

        jsr     popax
        sta     ptr2
        stx     ptr2+1

; Check device readiness

        jsr     popa
        jsr     diskinit
        beq     size
        jsr     __mappederrno
        bne     fail            ; Branch always

; Check for sufficient buf size

size:   lda     ptr3+1
        bne     okay            ; Buf >= 256
        lda     ptr3
        cmp     #3
        bcs     okay            ; Buf >= 3
        lda     #<ERANGE
        jsr     __directerrno
fail:   lda     #0              ; Return NULL
        tax
        rts

; Copy device string representation into buf

okay:   lda     fnunit          ; Set by diskinit
        jsr     devicestr       ; Returns 0 in A
        sta     __oserror       ; Clear _oserror

; Success, return buf

        lda     ptr2
        ldx     ptr2+1
        rts

.endproc
