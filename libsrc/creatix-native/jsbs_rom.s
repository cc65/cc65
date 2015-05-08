;; jsbs_rom.s, ca65

;; Emulate (zp),y ca65 uses these jsbs for (zp),y

;; The overhead is 72 cylces.. + 6 cycles for executing the JSB..

;; Must be placed in ROM at $FFE0
;;
;; .org $FFE0

;; .word jsb0_lda_y
;; .word jsb1_sta_y
;; .word jsb2_adc_y
;; .word jsb3_sbc_y
;; .word jsb4_and_y
;; .word jsb5_ora_y
;; .word jsb6_eor_y
;; .word jsb7_cmp_y
        
.macro JSB_START_EMUL_IND_Y
        pli                     ; 1 b, 6 c
        php                     ; 1 b, 3 c
        pha                     ; 1 b, 3 c
        stx tx                  ; 2 b, 3 c
        sty ty                  ; 2 b, 3 c
        ldx ty                  ; 2 b, 3 c
        ; load a with addr of indirect var and inc i
        lan                     ; 1 b, 3 c
        sta zindptr1            ; 2 b, 3 c
        ; can be done once at startup!
        sti #0,zindptr1_hi      ; 3 b, 4 c
        ; indirect load of lo addr
        lda (zindptr1)          ; 2 b, 5 c
        sta zindptr2            ; 2 b, 3 c
        inc zindptr1            ; 2 b, 5 c
        ; indirect load of hi addr
        lda (zindptr1)          ; 2 b, 5 c
        sta zindptr2+1          ; 2 b, 3 c
        pla                     ; 1 b, 4 c
        plp                     ; 1 b, 4 c
.endmacro                       ; 27 b, 60 c
        
.macro JSB_END_EMUL_IND_Y
        php                     ; 1 b, 3 c
        ldx tx                  ; 2 b, 3 c
        plp                     ; 1 b, 4 c
        tip                     ; 1 b, 2 c
.endmacro                       ; 5 b, 12 c

                                ; 32 b, 72 c

jsb0_lda_y:
        JSB_START_EMUL_IND_Y
        lda (zindptr2),x
        JSB_END_EMUL_IND_Y
jsb1_sta_y:
        JSB_START_EMUL_IND_Y
        sta (zindptr2),x                
        JSB_END_EMUL_IND_Y
jsb2_adc_y:
        JSB_START_EMUL_IND_Y
        adc (zindptr2),x                
        JSB_END_EMUL_IND_Y
jsb3_sbc_y:
        JSB_START_EMUL_IND_Y
        sbc (zindptr2),x                
        JSB_END_EMUL_IND_Y
jsb4_and_y:
        JSB_START_EMUL_IND_Y
        and (zindptr2),x                
        JSB_END_EMUL_IND_Y
jsb5_ora_y:
        JSB_START_EMUL_IND_Y
        ora (zindptr2),x
        JSB_END_EMUL_IND_Y
jsb6_eor_y:
        JSB_START_EMUL_IND_Y
        eor (zindptr2),x
        JSB_END_EMUL_IND_Y
jsb7_cmp_y:
        JSB_START_EMUL_IND_Y
        cmp (zindptr2),x
        JSB_END_EMUL_IND_Y
