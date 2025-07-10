;
; Ullrich von Bassewitz, 06.08.1998
; Christian Krueger, 11-Mar-2017, added 65SC02 optimization
;
; CC65 runtime: function epilogue
;

; exit a function.  pop stack and rts. The function comes in different
; flavours that provide default values for the return val, or drop a local
; stack frame with size in y.

        .export         leave00, leave0, leavey00, leavey0, leavey
        .export         leave
        .import         addysp
        .importzp       c_sp

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

.if .cap(CPU_HAS_ZPIND)

leave:  tay                     ; save A a sec
        lda     (c_sp)          ; that's the pushed arg size
        sec                     ; Count the byte, the count's stored in
        adc     c_sp
        sta     c_sp
        bcc     L1
        inc     c_sp+1
L1:     tya                     ; Get return value back

.else

leave:  pha                     ; save A a sec
        ldy     #0
        lda     (c_sp),y        ; that's the pushed arg size
        sec                     ; Count the byte, the count's stored in
        adc     c_sp
        sta     c_sp
        bcc     L1
        inc     c_sp+1
L1:     pla                     ; Get return value back

.endif
        rts

