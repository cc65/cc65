;
; Christian Kruger, 20-May-2018
;
; CC65 runtime: Pop 2 bytes from stack to ptr1.
; X is untouched, low byte in A, Y is defined to be 0!

        .export         popptr1
        .import         incsp2
        .importzp       sp, ptr1

.proc   popptr1                 ; 14 bytes (four usages = at least 2 bytes saved)
        ldy     #1
        lda     (sp),y          ; get hi byte
        sta     ptr1+1          ; into ptr hi
        dey                     ; no optimization for 65C02 here to have Y=0 at exit!
        lda     (sp),y          ; get lo byte
        sta     ptr1            ; to ptr lo
        jmp     incsp2
.endproc
