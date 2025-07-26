;
; Colin Leroy-Mira, 2025-07-26
;
; CC65 runtime: Load ptr1 from offset in stack
;

        .export         ldptr10sp, ldptr1ysp
        .importzp       c_sp, ptr1

; Beware: The optimizer knows about the value in Y after return!

ldptr10sp:
        ldy     #1
ldptr1ysp:
        lda     (c_sp),y        ; get high byte
        sta     ptr1+1          ; and save it
        dey                     ; point to lo byte
        lda     (c_sp),y        ; load low byte
        sta     ptr1
        rts
