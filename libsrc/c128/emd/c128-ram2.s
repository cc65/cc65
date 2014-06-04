;
; Extended memory driver for the C128 RAM in banks #1, #2 and #3. Driver works without
; problems when statically linked.
;
; Ullrich von Bassewitz, 2002-12-04
;
; Updated to use banks 2 and 3 as well by
; Marco van den Heuvel, 2010-01-21
;

        .include        "zeropage.inc"

        .include        "em-kernel.inc"
        .include        "em-error.inc"
        .include        "c128.inc"

        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _c128_ram2_emd

; Driver signature

        .byte   $65, $6d, $64           ; "emd"
        .byte   EMD_API_VERSION         ; EM API version number

; Library reference

        .addr   $0000

; Jump table

        .addr   INSTALL
        .addr   UNINSTALL
        .addr   PAGECOUNT
        .addr   MAP
        .addr   USE
        .addr   COMMIT
        .addr   COPYFROM
        .addr   COPYTO

; ------------------------------------------------------------------------
; Constants

BASE    = $400

; ------------------------------------------------------------------------
; Data.

.bss
curpage:        .res    2               ; Current page number
curbank:        .res    1               ; Current bank number
copybank:       .res    2               ; temp bank number

window:         .res    256             ; Memory "window"

pagecount:      .res    2               ; Number of available pages

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an EM_ERR_xx code in a/x.
;

INSTALL:
        ldx     #0
        stx     ptr1
        ldx     #4
        stx     ptr1+1
        ldx     #<ptr1
        stx     FETVEC
        stx     STAVEC
        ldy     #0
        ldx     #MMU_CFG_RAM1
        jsr     FETCH
        sta     tmp1
        ldx     #MMU_CFG_RAM3
        jsr     FETCH
        cmp     tmp1
        bne     @has_4_banks
        tax
        inx
        txa
        ldx     #MMU_CFG_RAM1
        jsr     STASH
        ldx     #MMU_CFG_RAM3
        jsr     FETCH
        cmp     tmp1
        beq     @has_4_banks
        ldx     #0
        lda     #251
        bne     @setok

@has_4_banks:
        ldx     #2
        lda     #241
@setok:
        sta     pagecount
        stx     pagecount+1
        ldx     #$FF
        stx     curpage
        stx     curpage+1       ; Invalidate the current page
        inx
        txa                     ; A = X = EM_ERR_OK
;       rts                     ; Run into UNINSTALL instead

; ------------------------------------------------------------------------
; UNINSTALL routine. Is called before the driver is removed from memory.
; Can do cleanup or whatever. Must not return anything.
;

UNINSTALL:
        rts


; ------------------------------------------------------------------------
; PAGECOUNT: Return the total number of available pages in a/x.
;

PAGECOUNT:
        lda     pagecount
        ldx     pagecount+1
        rts

; ------------------------------------------------------------------------
; MAP: Map the page in a/x into memory and return a pointer to the page in
; a/x. The contents of the currently mapped page (if any) may be discarded
; by the driver.
;

MAP:    sei
        sta     curpage
        stx     curpage+1               ; Remember the new page

        jsr     calculate_bank_and_correct_page
        stx     curbank

        clc
        adc     #>BASE
        sta     ptr1+1
        ldy     #$00
        sty     ptr1
        lda     #<ptr1
        sta     FETVEC

; Transfer one page

@L1:    ldx     curbank
        jsr     getcurbankmmu
        jsr     FETCH
        sta     window,y
        iny
        bne     @L1

; Return the memory window

        lda     #<window
        ldx     #>window                ; Return the window address
        cli
        rts

; ------------------------------------------------------------------------
; USE: Tell the driver that the window is now associated with a given page.

USE:    sta     curpage
        stx     curpage+1               ; Remember the page
        lda     #<window
        ldx     #>window                ; Return the window
        rts

; ------------------------------------------------------------------------
; COMMIT: Commit changes in the memory window to extended storage.

COMMIT: sei
        lda     curpage                 ; Get the current page
        ldx     curpage+1
        bmi     done                    ; Jump if no page mapped

        jsr     calculate_bank_and_correct_page
        stx     curbank

        clc
        adc     #>BASE
        sta     ptr1+1
        ldy     #$00
        sty     ptr1

        lda     #<ptr1
        sta     STAVEC

; Transfer one page. Y must be zero on entry

@L1:    lda     window,y
        ldx     curbank
        jsr     getcurbankmmu
        jsr     STASH
        iny
        bne     @L1
        cli

; Done

done:   rts

; ------------------------------------------------------------------------
; COPYFROM: Copy from extended into linear memory. A pointer to a structure
; describing the request is passed in a/x.
; The function must not return anything.
;

COPYFROM:
        sei
        jsr     setup

; Setup is:
;
;   - ptr1 contains the struct pointer
;   - ptr2 contains the linear memory buffer
;   - ptr3 contains -(count-1)
;   - ptr4 contains the page buffer and offset
;   - tmp1 contains the bank
;   - tmp2 contains zero (used for linear memory buffer offset)

        lda     #<ptr4
        sta     FETVEC
        jmp     @L3

@L1:    ldx     tmp1
        jsr     getcurbankmmu
        ldy     #0
        jsr     FETCH
        ldy     tmp2
        sta     (ptr2),y
        inc     tmp2
        bne     @L2
        inc     ptr2+1
@L2:    inc     ptr4
        beq     @L4

; Bump count and repeat

@L3:    inc     ptr3
        bne     @L1
        inc     ptr3+1
        bne     @L1
        cli
        rts

; Bump page register

@L4:    inc     ptr4+1
        lda     ptr4+1
        cmp     #$ff
        bne     @L3
        lda     #4
        sta     ptr4+1
        inc     tmp1
@L5:
        jmp     @L3


; ------------------------------------------------------------------------
; COPYTO: Copy from linear into extended memory. A pointer to a structure
; describing the request is passed in a/x.
; The function must not return anything.
;

COPYTO:
        sei
        jsr     setup

; Setup is:
;
;   - ptr1 contains the struct pointer
;   - ptr2 contains the linear memory buffer
;   - ptr3 contains -(count-1)
;   - ptr4 contains the page buffer and offset
;   - tmp1 contains the bank
;   - tmp2 contains zero (used for linear memory buffer offset)

        lda     #<ptr4
        sta     STAVEC
        jmp     @L3

@L1:
        ldy     tmp2
        lda     (ptr2),y
        ldx     tmp1
        jsr     getcurbankmmu
        ldy     #0
        jsr     STASH
        inc     tmp2
        bne     @L2
        inc     ptr2+1
@L2:    inc     ptr4
        beq     @L4

; Bump count and repeat

@L3:    inc     ptr3
        bne     @L1
        inc     ptr3+1
        bne     @L1
        cli
        rts

; Bump page register

@L4:    inc     ptr4+1
        lda     ptr4+1
        cmp     #$ff
        bne     @L3
        inc     tmp1
        lda     #4
        sta     ptr4+1
@L5:
        jmp     @L3

; ------------------------------------------------------------------------
; Helper function to calculate the correct bank and page
; when addressing bank 2 or 3

calculate_bank_and_correct_page:
        cpx     #2
        beq     @calculate_bank_3_with_2
        cpx     #1
        beq     @calculate_bank_2_or_3_with_1
        sec
        sbc     #251
        bcs     @calculate_bank_2_with_0
        ldx     #1
        lda     curpage
        rts

@calculate_bank_3_with_2:
        lda     curpage
        clc
        adc     #10
@calculate_bank_3_with_1:
        ldx     #3
        rts

@calculate_bank_2_or_3_with_1:
        sec
        sbc     #246
        bcs     @calculate_bank_3_with_1
        lda     curpage
        clc
        adc     #5
@calculate_bank_2_with_0:
        ldx     #2
        rts

; ------------------------------------------------------------------------
; Helper function to get the correct mmu value in x

getcurbankmmu:
        cpx     #1
        beq     @bank1
        cpx     #2
        beq     @bank2
        ldx     #MMU_CFG_RAM3
        rts
@bank2:
        ldx     #MMU_CFG_RAM2
        rts
@bank1:
        ldx     #MMU_CFG_RAM1
        rts

; ------------------------------------------------------------------------
; Helper function for COPYFROM and COPYTO: Store the pointer to the request
; structure and prepare data for the copy

setup:  sta     ptr1
        stx     ptr1+1          ; Save passed pointer

; Get the page number from the struct and adjust it so that it may be used
; with the hardware. That is: page pointer in ptr4 and bank in tmp1

        ldy     #EM_COPY::PAGE+1
        lda     (ptr1),y
        tax
        dey
        lda     (ptr1),y
        sta     curpage
        jsr     calculate_bank_and_correct_page
        clc
        adc     #4
        sta     ptr4+1
        stx     tmp1

; Get the buffer pointer into ptr2

        ldy     #EM_COPY::BUF
        lda     (ptr1),y
        sta     ptr2
        iny
        lda     (ptr1),y
        sta     ptr2+1

; Get the count, calculate -(count-1) and store it into ptr3

        ldy     #EM_COPY::COUNT
        lda     (ptr1),y
        eor     #$FF
        sta     ptr3
        iny
        lda     (ptr1),y
        eor     #$FF
        sta     ptr3+1

; Get the page offset into the low byte of ptr4 clear tmp2

        ldy     #EM_COPY::OFFS
        lda     (ptr1),y
        sta     ptr4
        lda     #0
        sta     tmp2

; Done

        rts
