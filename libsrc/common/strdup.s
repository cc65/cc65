;
; Ullrich von Bassewitz, 18.07.2000
; Colin Leroy-Mira, 05.01.2024
;
; char* __fastcall__ strdup (const char* S);
;
; Note: The code knowns which zero page locations are used by malloc,
; memcpy and strlen.
;

        .importzp       ptr2, ptr3, ptr4, tmp1, tmp2, tmp3
        .import         _strlen_ptr4, _malloc, _memcpy, pushax
        .export         _strdup

_strdup:
        ; Get length (and store source in ptr4)
        sta     ptr4
        stx     ptr4+1
        stx     tmp1            ; Backup high byte, which
        jsr     _strlen_ptr4    ; strlen may increment

        ; Add null byte for terminator
.if .cap(CPU_HAS_INA)
        inc     a
.else
        clc
        adc     #1
.endif
        bne     :+
        inx

        ; Store length
:       sta     tmp2
        stx     tmp3

        ; Allocate memory
        jsr     _malloc

        ; Check for NULL
        bne     :+
        cpx     #$00
        beq     OutOfMemory

        ; Push dest
:       jsr    pushax

        ; Push source
        lda     ptr4
        ldx     tmp1
        jsr     pushax

        ; Push length
        lda     tmp2
        ldx     tmp3

        ; Copy and return the dest pointer
        jmp     _memcpy

OutOfMemory:
        rts
