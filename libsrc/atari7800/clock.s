;
; 2022-03-15, Karri Kaksonen
;
; clock_t clock (void);
;

        .export         _clock, clock_count
        .interruptor    update_clock, 2 ; (low priority)
        .constructor    init_clock

        .import         sreg: zp
	.import		_zonecounter
        .include        "atari7800.inc"

        .macpack        generic

	.code

;-----------------------------------------------------------------------------
; Read the clock counter.
;
        .proc   _clock

        lda     #0
        sta     sreg+1          ; Promote 24 bits up to 32 bits
        lda     clock_count+2
        sta     sreg
        ldx     clock_count+1
        lda     clock_count

        rts
        .endproc

;-----------------------------------------------------------------------------
; This interrupt handler increments a 24-bit counter at every video
; vertical-blanking time.
; Update the clock only on interrupt while the drawing on screen is on
; _zonecounter == 1 (from 1st visible scanline to last visible scanline)
;
update_clock:
	lda	_zonecounter
	and	#01
	beq	@L1
        inc     clock_count
        bne     @L1
        inc     clock_count+1
        bne     @L1
        inc     clock_count+2
@L1:    ;clc                    ; General interrupt was not reset
        rts

;-----------------------------------------------------------------------------
; Set time to zero at startup
;
        .segment        "ONCE"
init_clock:
	lda	#0
	sta	clock_count+2
	sta	clock_count+1
	sta	clock_count
        rts

;-----------------------------------------------------------------------------
; Store time in 3 bytes
;
        .bss
clock_count:
        .res    3

