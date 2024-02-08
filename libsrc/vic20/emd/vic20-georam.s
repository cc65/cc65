;
; Extended memory driver for the GEORAM cartridge through the masC=erade
; c64 cartridge adapter. Driver works without problems when statically
; linked.
;
; Marco van den Heuvel, 2018-03-18
;

        .include        "zeropage.inc"

        .include        "em-kernel.inc"
        .include        "em-error.inc"


        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _vic20_georam_emd

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

GR_WINDOW       = $9800                 ; Address of GEORAM window
GR_PAGE_LO      = $9CFE                 ; Page register low
GR_PAGE_HI      = $9CFF                 ; Page register high

; ------------------------------------------------------------------------
; Data.

.data

pagecount:      .res    2               ; Number of available pages

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an EM_ERR_xx code in a/x.
;

INSTALL:
        ldx     GR_WINDOW
        cpx     GR_WINDOW
        bne     @notpresent
        inc     GR_WINDOW
        cpx     GR_WINDOW
        beq     @notpresent

        lda     #4
        jsr     check
        cpy     GR_WINDOW
        beq     @has64k
        lda     #8
        jsr     check
        cpy     GR_WINDOW
        beq     @has128k
        lda     #16
        jsr     check
        cpy     GR_WINDOW
        beq     @has256k
        lda     #32
        jsr     check
        cpy     GR_WINDOW
        beq     @has512k
        lda     #64
        jsr     check
        cpy     GR_WINDOW
        beq     @has1024k
        lda     #128
        jsr     check
        cpy     GR_WINDOW
        beq     @has2048k
        ldx     #>16384
        bne     @setok

@has64k:
        ldx     #>256
        bne     @setok
@has128k:
        ldx     #>512
        bne     @setok
@has256k:
        ldx     #>1024
        bne     @setok
@has512k:
        ldx     #>2048
        bne     @setok
@has1024k:
        ldx     #>4096
        bne     @setok
@has2048k:
        ldx     #>8192
        bne     @setok

@notpresent:
        lda     #EM_ERR_NO_DEVICE
        ldx     #0 ; return value is char
        rts

@setok:
        lda     #0
        sta     pagecount
        stx     pagecount+1
        .assert EM_ERR_OK = 0, error
        tax
        rts

check:
        ldx     #0
        stx     GR_PAGE_LO
        stx     GR_PAGE_HI
        ldy     GR_WINDOW
        iny
        sta     GR_PAGE_HI
        sty     GR_WINDOW
        ldx     #0
        stx     GR_PAGE_HI
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
; USE: Tell the driver that the window is now associated with a given page.
; The GeoRAM cartridge does not copy but actually map the window, so USE is
; identical to MAP.

USE     = MAP

; ------------------------------------------------------------------------
; MAP: Map the page in a/x into memory and return a pointer to the page in
; a/x. The contents of the currently mapped page (if any) may be discarded
; by the driver.
;

MAP:    sta     tmp1
        txa
        asl     tmp1
        rol     a
        asl     tmp1
        rol     a

        sta     GR_PAGE_HI
        lda     tmp1
        lsr     a
        lsr     a
        sta     GR_PAGE_LO

        lda     #<GR_WINDOW
        ldx     #>GR_WINDOW

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

        jmp     @L5

@L1:    lda     GR_WINDOW,x
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

@L4:    inc     tmp1            ; Bump low page register
        bit     tmp1            ; Check for overflow in bit 6
        bvc     @L6             ; Jump if no overflow
        inc     tmp2
@L5:    lda     tmp2
        sta     GR_PAGE_HI
@L6:    lda     tmp1
        sta     GR_PAGE_LO
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

        jmp     @L5

@L1:    lda     (ptr2),y
        sta     GR_WINDOW,x
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

@L4:    inc     tmp1            ; Bump low page register
        bit     tmp1            ; Check for overflow in bit 6
        bvc     @L6             ; Jump if no overflow
        inc     tmp2
@L5:    lda     tmp2
        sta     GR_PAGE_HI
@L6:    lda     tmp1
        sta     GR_PAGE_LO
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
        asl     a
        rol     tmp2
        asl     a
        rol     tmp2
        lsr     a
        lsr     a
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


