;
; Extended memory driver for the ISEPIC cartridge.
; Marco van den Heuvel, 2010-01-24
;

        .include        "zeropage.inc"

        .include        "em-kernel.inc"
        .include        "em-error.inc"


        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _c64_isepic_emd

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

IP_WINDOW       = $DF00         ; Address of ISEPIC window
IP_CTRL_BASE    = $DE00
PAGES           = 8

; ------------------------------------------------------------------------
; Code.

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an EM_ERR_xx code in a/x.
;

INSTALL:
        lda     #0
        sta     IP_CTRL_BASE
        ldx     IP_WINDOW
        cpx     IP_WINDOW
        bne     @notpresent
        inc     IP_WINDOW
        cpx     IP_WINDOW
        beq     @notpresent
        ldx     IP_WINDOW
        sta     IP_CTRL_BASE+1
        inx
        stx     IP_WINDOW
        dex
        sta     IP_CTRL_BASE
        cpx     IP_WINDOW
        beq     @setok

@notpresent:
        lda     #EM_ERR_NO_DEVICE
        ldx     #0 ; return value is char
        rts

@setok:
        lda     #EM_ERR_OK
        .assert EM_ERR_OK = 0, error
        tax
;       rts                             ; Run into UNINSTALL instead

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
        lda     #<PAGES
        ldx     #>PAGES
        rts

; ------------------------------------------------------------------------
; USE: Tell the driver that the window is now associated with a given page.
; The Isepic cartridge does not copy but actually map the window, so USE is
; identical to MAP.

USE     := MAP

; ------------------------------------------------------------------------
; MAP: Map the page in a/x into memory and return a pointer to the page in
; a/x. The contents of the currently mapped page (if any) may be discarded
; by the driver.
;

MAP:
        tax
        sta     IP_CTRL_BASE,x
        lda     #<IP_WINDOW
        ldx     #>IP_WINDOW

; Use the RTS from COMMIT below to save a precious byte of storage

; ------------------------------------------------------------------------
; COMMIT: Commit changes in the memory window to extended storage.

COMMIT:
        rts

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
;   - X contains the page offset
;   - Y contains zero

        jmp     @L5

@L1:
        lda     IP_WINDOW,x
        sta     (ptr2),y
        iny
        bne     @L2
        inc     ptr2+1
@L2:
        inx
        beq     @L4

; Bump count and repeat

@L3:
        inc     ptr3
        bne     @L1
        inc     ptr3+1
        bne     @L1
        rts

; Bump page register

@L4:
        inc     tmp1            ; Bump low page register
@L5:
        stx     tmp2
        ldx     tmp1
        sta     IP_CTRL_BASE,x
        ldx     tmp2
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
;   - X contains the page offset
;   - Y contains zero

        jmp     @L5

@L1:
        lda     (ptr2),y
        sta     IP_WINDOW,x
        iny
        bne     @L2
        inc     ptr2+1
@L2:
        inx
        beq     @L4

; Bump count and repeat

@L3:
        inc     ptr3
        bne     @L1
        inc     ptr3+1
        bne     @L1
        rts

; Bump page register

@L4:
        inc     tmp1            ; Bump page register
@L5:
        stx     tmp2
        ldx     tmp1
        sta     IP_CTRL_BASE,x
        ldx     tmp2
        jmp     @L3

; ------------------------------------------------------------------------
; Helper function for COPYFROM and COPYTO: Store the pointer to the request
; structure and prepare data for the copy

setup:
        sta     ptr1
        stx     ptr1+1          ; Save passed pointer

; Get the page number from the struct and remember it.

        ldy     #EM_COPY::PAGE
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
        ldy     #0

; Done

        rts
