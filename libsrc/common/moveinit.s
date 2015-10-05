;
; 2015-10-04, Greg King
;

        .export         move_init

        .import         __INIT_LOAD__, __INIT_RUN__, __INIT_SIZE__ ; Linker-generated
        .importzp       init_load_, init_run_

        .macpack        cpu
        .macpack        generic


; Move the INIT segment from where it was loaded (over the bss segments)
; into where it must be run (in the heap).  The two areas might overlap; and,
; the segment is moved upwards.  Therefore, this code starts at the highest
; address, and decrements to the lowest address.  The low bytes of the starting
; pointers are not sums.  The high bytes are sums; but, they do not include the
; carry.  Both the low-byte sums and the carries will be done when the pointers
; are indexed by the .Y register.

move_init:
        lda     #<__INIT_LOAD__
        ldx     #>__INIT_LOAD__ + >__INIT_SIZE__
        sta     init_load_
        stx     init_load_+1
        lda     #<__INIT_RUN__
        ldx     #>__INIT_RUN__ + >__INIT_SIZE__
        sta     init_run_
        stx     init_run_+1

; First, move the last, partial page.
; Then, move all of the full pages.

        ldx     #>__INIT_SIZE__ + 1     ; number of pages, including partial
        ldy     #<__INIT_SIZE__         ; size of partial page
.if     .cpu & CPU_ISET_65SC02
        bra     L3
.else
        jmp     L3
.endif

L1:     dec     init_load_+1
        dec     init_run_+1

L2:     dey
        lda     (init_load_),y
        sta     (init_run_),y
        tya
L3:     bnz     L2                      ; page not finished

        dex
        bnz     L1                      ; move next page
        rts
