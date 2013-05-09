;  
; Ullrich von Bassewitz, 06.08.1998
;
; CC65 runtime: function epilogue
;

; exit a function.  pop stack and rts. The function comes in different
; flavours that provide default values for the return val, or drop a local
; stack frame with size in y.

        .export         leave00, leave0, leavey00, leavey0, leavey
        .export         leave
        .import         addysp
        .importzp       sp

leave00:
        lda     #0
leave0: ldx     #0
        beq     leave

leavey00:
        lda     #0              ; "return 0"
leavey0:
        ldx     #0              ; return < 256
leavey:
        jsr     addysp          ; drop stack frame
leave:  pha                     ; save A a sec
        ldy     #0
        lda     (sp),y          ; that's the pushed arg size
        sec                     ; Count the byte, the count's stored in
        adc     sp
        sta     sp
        bcc     L1
        inc     sp+1
L1:     pla                     ; Get return value back
        rts

