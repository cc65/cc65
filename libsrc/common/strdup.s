;
; Ullrich von Bassewitz, 18.07.2000
;
; char* __fastcall__ strdup (const char* S);
;
; Note: The code knowns which zero page locations are used by malloc.
;

        .importzp       sp, tmp1, ptr4
        .import         pushax, decsp4, incsp4
        .import         _strlen, _malloc, _memcpy
        .export         _strdup

        .macpack        cpu
        .macpack        generic

_strdup:

; Since we need some place to store the intermediate results, allocate a
; stack frame. To make this somewhat more efficient, create the stackframe
; as needed for the final call to the memcpy function.

        pha                     ; decsp will destroy A (but not X)
        jsr     decsp4          ; Target/source

; Store the pointer into the source slot

        ldy     #1
        txa
        sta     (sp),y
        pla
.if (.cpu .bitand CPU_ISET_65SC02)
        sta     (sp)
.else
        dey
        sta     (sp),y
.endif

; Get length of S (which is still in a/x)

        jsr     _strlen

; Calculate strlen(S)+1 (the space needed)

        add     #1
        bcc     @L1
        inx

; Save the space we're about to allocate in ptr4

@L1:    sta     ptr4
        stx     ptr4+1

; Allocate memory. _malloc will not use ptr4

        jsr     _malloc

; Store the result into the target stack slot

        ldy     #2
        sta     (sp),y          ; Store low byte
        sta     tmp1
        txa                     ; Get high byte
        iny
        sta     (sp),y          ; Store high byte

; Check for a NULL pointer

        ora     tmp1
        beq     OutOfMemory

; Copy the string. memcpy will return the target string which is exactly
; what we need here. It will also drop the allocated stack frame.

        lda     ptr4
        ldx     ptr4+1          ; Load size
        jmp     _memcpy         ; Copy string, drop stackframe

; Out of memory, return NULL (A = 0)

OutOfMemory:
        tax
        jmp     incsp4          ; Drop stack frame


