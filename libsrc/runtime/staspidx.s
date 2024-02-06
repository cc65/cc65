;
; Ullrich von Bassewitz, 26.10.2000
;
; CC65 runtime: Store a indirect into address at top of stack with index
;

        .export         staspidx
        .import         incsp2
        .importzp       sp, tmp1, ptr1

.proc   staspidx

        pha
        sty     tmp1            ; Save Index
        ldy     #1
        lda     (sp),y
        sta     ptr1+1
        dey
        lda     (sp),y
        sta     ptr1            ; Pointer now in ptr1
        ldy     tmp1            ; Restore offset
        pla                     ; Restore value
        sta     (ptr1),y        ; Store
        jmp     incsp2          ; Drop address

.endproc


