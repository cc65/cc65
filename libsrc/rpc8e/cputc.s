; void __fastcall__ cputcxy (unsigned char x, unsigned char y, char c);
; void __fastcall__ cputc (char c);
;
		.export         _cputc
		.import         pusha,pushax,tosaddax,incsp1,decsp1,incsp2
		.importzp       sp,tmp5,ptr5

        .code

; Plot a character at the current cursor x,y - also used as internal function
;Manually Modified from the following c code because all the regular temp variables are used when this is called (hence tmp5 and ptr5)
;void __fastcall__ cputc(char input) {
;    *((char *)0x0310+UINT8_CHARX) = input;
;    UINT8_CHARX++;
;    if (UINT8_CHARX >= 80) {
;        UINT8_CHARY++;
;        UINT8_MEMORYROW++;
;        UINT8_CHARX = 0;
;    }
;    return;
;}



.proc _cputc
        jsr     pusha
		
        ldx     #$00
        lda     $0301
        jsr     pushax
        ldx     #$03
        lda     #$10
        jsr     tosaddaxTMP5
        jsr     pushax
        ldy     #$02
        ldx     #$00
        lda     (sp),y
        ldy     #$00
        jsr     staspidxTMP5
        ldx     #$00
        lda     $0301
        inc     $0301
		
        ldx     #$00
        lda     $0301
        cmp     #$50
        lda     #$00
        ldx     #$00
        rol     a
        beq     L0008
		
        ldx     #$00
        lda     $0302
        inc     $0302
	    ldx     #$00
        lda     $0300
        inc     $0300
        ldx     #$00
        lda     #$00
        sta     $0301
		
        ldx     #$00
        lda     $0301
        cmp     #$50
        lda     #$00
        ldx     #$00
        rol     a
        beq     L0008
        ldx     #$00
        lda     $0302
        inc     $0302		
		
L0008:  jsr     incsp1
		rts
.endproc

.proc   staspidxTMP5

        pha
        sty     tmp5            ; Save Index
        ldy     #1
        lda     (sp),y
        sta     ptr5+1
        dey
        lda     (sp),y
        sta     ptr5            ; Pointer now in ptr1
        ldy     tmp5            ; Restore offset
        pla                     ; Restore value
        sta     (ptr5),y        ; Store
        jmp     incsp2          ; Drop address

.endproc

.proc tosaddaxTMP5
        clc                     ; (2)
        ldy     #0              ; (4)
        adc     (sp),y          ; (9) lo byte
        iny                     ; (11)
        sta     tmp5            ; (14) save it
        txa                     ; (16) 
        adc     (sp),y          ; (21) hi byte
        tax                     ; (23)
        clc                     ; (25)
        lda     sp              ; (28)
        adc     #2              ; (30)
        sta     sp              ; (33)
        bcc     L1              ; (36)
        inc     sp+1            ; (-1+5)
L1:     lda     tmp5            ; (39) restore low byte
        rts                     ; (6502: 45 cycles, 26 bytes <-> 65SC02: 42 cycles, 22 bytes )
		
.endproc