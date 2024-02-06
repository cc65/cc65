; Extended Memory Driver for the Kerberos MIDI interface.
; http://www.frank-buss.de/kerberos/
; based on the code for RamCart 64/128KB cartridge.
; 2020-06-16 Dirk Jagdmann <doj@cubic.org>

        .include        "zeropage.inc"
        .include        "em-kernel.inc"
        .include        "em-error.inc"

        .macpack        generic
        .macpack        module

; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _c64_kerberos_emd

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

RAMC_WINDOW       = $DF00                 ; Address of Kerberos SRAM
RAMC_PAGE_LO      = $DE3E                 ; Page register low
RAMC_PAGE_HI      = $DE3F                 ; Page register high

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an EM_ERR_xx code in a/x.
;

INSTALL:
        ;; write $55 into first page
        lda     #0
        sta     RAMC_PAGE_LO
        sta     RAMC_PAGE_HI
        lda     #$55
        sta     RAMC_WINDOW

        ;; write $AA into second page
        lda     #1
        sta     RAMC_PAGE_LO
        sta     RAMC_PAGE_HI
        lda     #$AA
        sta     RAMC_WINDOW

        ;; check $55 in first page
        lda     #0
        sta     RAMC_PAGE_LO
        sta     RAMC_PAGE_HI
        lda     RAMC_WINDOW
        cmp     #$55
        bne     @notpresent

        ;; check $AA in first page
        lda     #1
        sta     RAMC_PAGE_LO
        sta     RAMC_PAGE_HI
        lda     RAMC_WINDOW
        cmp     #$AA
        bne     @notpresent

        lda     #EM_ERR_OK
        .assert EM_ERR_OK = 0, error
        tax
        rts

@notpresent:
        lda     #EM_ERR_NO_DEVICE
        ldx     #0 ; return value is char
        ; use rts from UNINSTALL below

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
        lda     #<(128 * 1024 / 256)
        ldx     #>(128 * 1024 / 256)
        rts

; ------------------------------------------------------------------------
; USE: Tell the driver that the window is now associated with a given page.
; The Kerberos cartridge does not copy but actually map the window, so USE is
; identical to MAP.

USE     = MAP

; ------------------------------------------------------------------------
; MAP: Map the page in a/x into memory and return a pointer to the page in
; a/x. The contents of the currently mapped page (if any) may be discarded
; by the driver.
;

MAP:    sta     RAMC_PAGE_LO
        txa
        and     #1
        sta     RAMC_PAGE_HI
        lda     #<RAMC_WINDOW
        ldx     #>RAMC_WINDOW

; Use the RTS from COMMIT below to save a precious byte of storage

; ------------------------------------------------------------------------
; COMMIT: Commit changes in the memory window to extended storage.

COMMIT: rts

; ------------------------------------------------------------------------
; COPYFROM: Copy from extended into linear memory. A pointer to a structure
; describing the request is passed in a/x.
; The function must not return anything.
;

COPYFROM:
        jsr     setup

; Setup is:
;
;   - ptr1 contains the struct pointer
;   - ptr2 contains the linear memory buffer
;   - ptr3 contains -(count-1)
;   - tmp1 contains the low page register value
;   - tmp2 contains the high page register value
;   - X contains the page offset
;   - Y contains zero

        beq     @L5 ; will always branch, because setup ends with ldy #0

@L1:    lda     RAMC_WINDOW,x
        sta     (ptr2),y
        iny
        bne     @L2
        inc     ptr2+1
@L2:    inx
        beq     @L4

; Bump count and repeat

@L3:    inc     ptr3
        bne     @L1
        inc     ptr3+1
        bne     @L1
        rts

; Bump page register

@L4:    inc     tmp1
        bne     @L5
        inc     tmp2
@L5:    lda     tmp1
        sta     RAMC_PAGE_LO
        lda     tmp2
        sta     RAMC_PAGE_HI
        jmp     @L3

; ------------------------------------------------------------------------
; COPYTO: Copy from linear into extended memory. A pointer to a structure
; describing the request is passed in a/x.
; The function must not return anything.
;

COPYTO:
        jsr     setup

; Setup is:
;
;   - ptr1 contains the struct pointer
;   - ptr2 contains the linear memory buffer
;   - ptr3 contains -(count-1)
;   - tmp1 contains the low page register value
;   - tmp2 contains the high page register value
;   - X contains the page offset
;   - Y contains zero

        beq     @L5 ; will always branch, because setup ends with ldy #0

@L1:    lda     (ptr2),y
        sta     RAMC_WINDOW,x
        iny
        bne     @L2
        inc     ptr2+1
@L2:    inx
        beq     @L4

; Bump count and repeat

@L3:    inc     ptr3
        bne     @L1
        inc     ptr3+1
        bne     @L1
        rts

; Bump page register

@L4:    inc     tmp1
        bne     @L5
        inc     tmp2
@L5:    lda     tmp1
        sta     RAMC_PAGE_LO
        lda     tmp2
        sta     RAMC_PAGE_HI
        jmp     @L3

; ------------------------------------------------------------------------
; Helper function for COPYFROM and COPYTO: Store the pointer to the request
; structure and prepare data for the copy

setup:  sta     ptr1
        stx     ptr1+1          ; Save passed pointer

; Get the page number from the struct and adjust it so that it may be used
; with the hardware. That is: lower 6 bits in tmp1, high bits in tmp2.

        ldy     #EM_COPY::PAGE+1
        lda     (ptr1),y
        sta     tmp2
        dey
        lda     (ptr1),y
        sta     tmp1

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

; Get the page offset into X and clear Y

        ldy     #EM_COPY::OFFS
        lda     (ptr1),y
        tax
        ldy     #$00

; Done

        rts
