;
; Ullrich von Bassewitz, 26.10.2000
; Christian Krueger, 12-Mar-2017, added 65SC02 optimization
;
; CC65 runtime: Store a/x indirect into address at top of stack with index
;

        .export         staxspidx
        .import         incsp2
        .importzp       c_sp, tmp1, ptr1

        .macpack        cpu

.proc   staxspidx

        sty     tmp1            ; Save Y
        pha                     ; Save A
        ldy     #1
        lda     (c_sp),y
        sta     ptr1+1
.if (.cpu .bitand ::CPU_ISET_65SC02)
        lda     (c_sp)
.else
        dey
        lda     (c_sp),y
.endif
        sta     ptr1            ; Address now in ptr1
        ldy     tmp1            ; Restore Y
        iny                     ; Address high byte
        txa                     ; Get high byte
        sta     (ptr1),y        ; Store high byte
        dey                     ; Address low byte
        pla                     ; Restore low byte into A
        sta     (ptr1),y        ; Store low byte
        jmp     incsp2          ; Drop address

.endproc


