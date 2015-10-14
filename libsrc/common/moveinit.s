;
; 2015-10-07, Greg King
;

        .export         moveinit

        .import         __INIT_LOAD__, __INIT_RUN__, __INIT_SIZE__ ; Linker-generated

        .macpack        cpu
        .macpack        generic


; Put this in the DATA segment because it is self-modifying code.

.data

; Move the INIT segment from where it was loaded (over the bss segments)
; into where it must be run (over the BSS segment).  The two areas might overlap;
; and, the segment is moved upwards.  Therefore, this code starts at the highest
; address, and decrements to the lowest address.  The low bytes of the starting
; pointers are not sums.  The high bytes are sums; but, they do not include the
; carry.  Both the low-byte sums and the carries will be done when the pointers
; are indexed by the .Y register.

moveinit:

; First, move the last, partial page.
; Then, move all of the full pages.

        ldy     #<__INIT_SIZE__         ; size of partial page
        ldx     #>__INIT_SIZE__ + (<__INIT_SIZE__ <> 0)  ; number of pages, including partial

L1:     dey
init_load:
        lda     __INIT_LOAD__ + (__INIT_SIZE__ & $FF00) - $0100 * (<__INIT_SIZE__ = 0),y
init_run:
        sta     __INIT_RUN__  + (__INIT_SIZE__ & $FF00) - $0100 * (<__INIT_SIZE__ = 0),y
        tya
        bnz     L1                      ; page not finished

        dec     init_load+2
        dec     init_run+2
        dex
        bnz     L1                      ; move next page
        rts
