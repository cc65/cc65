.include "mmu.inc"

.export _rb_enable, _rb_disable, _rb_map_device, _rb_set_window, _rb_set_window_ext, _rb_disable_ext


.segment "CODE"

;------------------------------------------
; void __inline__ rb_enable(void);
;------------------------------------------
_rb_enable:
		mmu $02
	rts


;------------------------------------------
; void __inline__ rb_disable(void);
;------------------------------------------
_rb_disable:
		mmu $82
	rts


;------------------------------------------
; void __fastcall__ rb_set_window(void* address);
;------------------------------------------
_rb_set_window:
	;switch to native 16bit
	    clc
        .byte $FB 			; XCE
	    .byte $C2, $30      ; REP #$30         

	; init redstone window at $0300
		;.byte $A9, $00, $03 ; LDA #$0300
		.byte $EB ; XBA
		stx $55
		ora $55
		.byte $EB ; XBA
	    mmu $01

	;switch to emulated 8bit
		.byte $E2, $30      ; SEP #$30
	    sec
	    .byte $FB 			; XCE
	rts


;------------------------------------------
; void __fastcall__ rb_map_device(unsigned char id);
;------------------------------------------
_rb_map_device:
		mmu $00
	rts


;------------------------------------------
; void __fastcall__ rb_set_window_ext(void* address);
;------------------------------------------
_rb_set_window_ext:
	;switch to native 16bit
	    clc
        .byte $FB 			; XCE
	    .byte $C2, $30      ; REP #$30         

	; init redstone external window:
		;.byte $A9, $00, $03 ; LDA #$0300
		.byte $EB ; XBA
		stx $55
		ora $55
		.byte $EB ; XBA
	    mmu $03

	;switch to emulated 8bit
		.byte $E2, $30      ; SEP #$30
		sec
		.byte $FB	    ;XCE
	rts

	
;------------------------------------------
; void __inline__ rb_enable_ext(void);
;------------------------------------------
_rb_enable_ext:
		mmu $04
	rts

	
;------------------------------------------
; void __inline__ rb_enable_ext(void);
;------------------------------------------
_rb_disable_ext:
		mmu $84
	rts

