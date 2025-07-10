;
; Colin Leroy-Mira, 2024
;
; char* __fastcall__ asctime (const struct tm* timep)
;

        .export         _asctime
        .import         _strftime, pushax
        .importzp       ptr1
        .include        "time.inc"

; ------------------------------------------------------------------------
; Special values

; We need to be able to store up to 38 bytes:
;     1234567890123456789012345678901234567
;    "Wednesday September ..1 00:00:00 1970"
MAX_BUF_LEN = 38

; ------------------------------------------------------------------------
; Code

_asctime:
        ; Backup timep
        .if .cap(CPU_HAS_PUSHXY)
        pha
        phx
        .else
        sta     ptr1
        stx     ptr1+1
        .endif

        ; Push buf
        lda     #<buf
        ldx     #>buf
        jsr     pushax

        ; Push sizeof(buf)
        lda     #<MAX_BUF_LEN
        ldx     #>MAX_BUF_LEN
        jsr     pushax

        ; Push format string
        lda     #<fmt
        ldx     #>fmt
        jsr     pushax

        ; Restore timep
        .if .cap(CPU_HAS_PUSHXY)
        plx
        pla
        .else
        lda     ptr1
        ldx     ptr1+1
        .endif

        ; Call formatter
        jsr     _strftime

        ; Check return status
        bne     :+
        cpx     #$00
        bne     :+
        rts

:       lda     #<buf
        ldx     #>buf
        rts

        .data

fmt:    .byte '%'
        .byte 'c'
        .byte $0A
        .byte $00

        .bss

buf:    .res MAX_BUF_LEN
