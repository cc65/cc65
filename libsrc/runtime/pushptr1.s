;
; Colin Leroy-Mira, 2024
;
; CC65 runtime: Push ptr1 to stack.
; A/X destroyed (set to ptr1)

        .export         pushptr1
        .import         pushax
        .importzp       ptr1

pushptr1:
        lda     ptr1
        ldx     ptr1+1
        jmp     pushax
