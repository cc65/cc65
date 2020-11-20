;
; 2003-04-13, Ullrich von Bassewitz
; 2012-02-06, Greg King
;
; clock_t clock (void);
; clock_t _clocks_per_sec (void);
;
; clocks_per_sec()'s test-values are based on the numbers in "set_tv.s".
; If you change the numbers there, then change them here, too.
;

        .export         _clock, __clocks_per_sec, clock_count
        .interruptor    update_clock, 2 ; (low priority)
        .constructor    init_clock

        .import         sreg: zp
        .include        "lynx.inc"

        .macpack        generic


        .proc   _clock
        php
        sei                     ; Disable interrupts

; Read the clock counter.

        lda     clock_count
        ldx     clock_count+1
        ldy     clock_count+2

        plp                     ; Re-enable interrupts
        sty     sreg
        stz     sreg+1          ; Promote 24 bits up to 32 bits
        rts
        .endproc

;-----------------------------------------------------------------------------
; Return the number of clock ticks in one second.
;
__clocks_per_sec:
        ldx     #$00            ; >50, >60, >75
        ldy     PBKUP
        lda     #<75
        cpy     #$20 + 1
        blt     @ok
        lda     #<60
        cpy     #$29 + 1
        blt     @ok
        lda     #<50
@ok:    stz     sreg            ; return 32 bits
        stz     sreg+1
        rts

;-----------------------------------------------------------------------------
; This interrupt handler increments a 24-bit counter at every video
; vertical-blanking time.
;
        .segment        "LOWCODE"
update_clock:
        lda     INTSET
        and     #%00000100
        beq     @NotVBlank      ; Not vertical-blank interrupt

        inc     clock_count
        bne     @L1
        inc     clock_count+1
        bne     @L1
        inc     clock_count+2
@L1:    ;clc                    ; General interrupt was not reset
@NotVBlank:
        rts

;-----------------------------------------------------------------------------
; Enable the interrupt that update_clock needs.
;
        .segment        "ONCE"
init_clock:
        lda     #%10000000
        tsb     VTIMCTLA
        rts

;-----------------------------------------------------------------------------
;
        .bss
clock_count:
        .res    3
