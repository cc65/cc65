;
; Christian Kruger, 20-May-2018
;
; CC65 runtime: Pop registers on stack to ptr1 or ptr2 and ptr1.
; X is untouched, low byte in A, Y is defined to be 0!

        .export         popptr1
        .import 		incsp2
        .importzp       sp, ptr1

		.macpack cpu

.proc 	popptr1					; 14 bytes, 

        ldy     #1
        lda     (sp),y          ; get hi byte
        sta		ptr+1           ; into ptr hi
        dey						; note: apply even for 65C02 to have Y=0 at exit! 
.if (.cpu .bitand ::CPU_ISET_65SC02)
        lda     (sp)            ; get lo byte
.else
        lda     (sp),y          ; get lo byte
.endif
		sta		ptr1 			; to ptr lo

		jmp		incsp2
.endproc


