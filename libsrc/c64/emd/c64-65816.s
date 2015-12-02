;
; Extended memory driver for 65816 based extra RAM. Driver works without
; problems when statically linked.
;
; Marco van den Heuvel, 2015-12-01
;

        .include        "zeropage.inc"

        .include        "em-kernel.inc"
        .include        "em-error.inc"


        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _c64_65816_emd

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
; Data.

.bss
curpage:        .res    1               ; Current page number
curbank:        .res    1               ; Current bank number (+1)
bankcount:      .res    1               ; Number of available banks (pages = banks * 256)
window:         .res    256             ; Memory "window"

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an EM_ERR_xx code in a/x.
;

INSTALL:
        sei
        clc
        sed
        lda     #$99
        adc     #$01                    ; on 65C02, 65SC02, 65CE02, 65802 and 65816 this has been fixed
        cld
        bne     @not_present            
        clc
.P816
        sep     #$01                    ; nop #$01 on 65C02/65SC02 and lda ($01,s),y on 65CE02
.P02
        bcc     @not_present
        lda     $07e8
        pha
        ldx     #$ff
@fillloop:
        txa
        pha
.P816
        plb                             ; pull dbr
.P02
        stx     $07e8
        dex
        cpx     #$ff
        bne     @fillloop
        inx
@compareloop:
        txa
        pha
.P816
        plb
.P02
        cmp     $07e8
        bne     @found_pages
        inx
        bne     @compareloop
@found_pages:
        dex
        lda     #$00
        pha
.P816
        plb
.P02
        pla
        sta     $07e8
        stx     bankcount
        lda     #<EM_ERR_OK
        ldx     #>EM_ERR_OK
        cli
        rts
@notpresent:
        lda     #<EM_ERR_NO_DEVICE
        ldx     #>EM_ERR_NO_DEVICE
        cli
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
        lda     #$00
        ldx     bankcount
        rts

; ------------------------------------------------------------------------
; MAP: Map the page in a/x into memory and return a pointer to the page in
; a/x. The contents of the currently mapped page (if any) may be discarded
; by the driver.
;

MAP:    sei
        sta     curpage                 ; Remember the new page
        stx     curbank                 ; Remember the new bank

        sta     ptr2+1                  ; src address low
        lda     #$00
        sta     ptr2                    ; src address high
        stx     tmp2                    ; src bank

        sta     tmp1                    ; dst bank

        sta     ptr3+1                  ; length high
        lda     #$ff
        sta     ptr3                    ; length low

        lda     #<window
        sta     ptr1                    ; dst address low
        ldx     #>window
        stx     ptr1+1                  ; dst address high

        jsr     transfer

        cli
        rts

; ------------------------------------------------------------------------
; USE: Tell the driver that the window is now associated with a given page.

USE:    sta     curpage                 ; Remember the page
        stx     curbank                 ; Remember the bank
        lda     #<window
        ldx     #>window                ; Return the window
        rts

; ------------------------------------------------------------------------
; COMMIT: Commit changes in the memory window to extended storage.

COMMIT: lda     curpage                 ; Get the current page
        bmi     done                    ; Jump if no page mapped

        clc
        adc     #>BASE
        sta     ptr2+1
        ldy     #$00
        sty     ptr2

        lda     #<window
        sta     ptr1
        lda     #>window
        sta     ptr1+1

; Transfer one page. Y must be zero on entry

transfer:
        ldx     $01                     ; Remember c64 control port
        txa
        and     #$F8                    ; Bank out ROMs, I/O
        sei
        sta     $01

; Unroll the following loop

loop:   .repeat 8
        lda     (ptr1),y
        sta     (ptr2),y
        iny
        .endrepeat

        bne     loop

; Restore the old memory configuration, allow interrupts

        stx     $01                     ; Restore the old configuration
        cli

; Done

done:   rts               

; ------------------------------------------------------------------------
; COPYFROM: Copy from extended into linear memory. A pointer to a structure
; describing the request is passed in a/x.
; The function must not return anything.
;

COPYFROM:
        sta     ptr3
        stx     ptr3+1                  ; Save the passed em_copy pointer
                       
        ldy     #EM_COPY::OFFS
        lda     (ptr3),y
        sta     ptr1
        ldy     #EM_COPY::PAGE
        lda     (ptr3),y
        clc
        adc     #>BASE
        sta     ptr1+1                  ; From

        ldy     #EM_COPY::BUF
        lda     (ptr3),y
        sta     ptr2
        iny
        lda     (ptr3),y
        sta     ptr2+1                  ; To

common: ldy     #EM_COPY::COUNT+1
        lda     (ptr3),y                ; Get number of pages
        beq     @L2                     ; Skip if no full pages
        sta     tmp1

; Copy full pages allowing interrupts after each page copied

        ldy     #$00
@L1:    jsr     transfer
        inc     ptr1+1
        inc     ptr2+1
        dec     tmp1
        bne     @L1

; Copy the remainder of the page

@L2:    ldy     #EM_COPY::COUNT
        lda     (ptr3),y                ; Get bytes in last page
        beq     @L4
        tax

        lda     $01                     ; Remember c64 control port
        pha
        and     #$F8                    ; Bank out ROMs, I/O
        sei
        sta     $01

; Transfer the bytes in the last page

        ldy     #$00
@L3:    lda     (ptr1),y
        sta     (ptr2),y
        iny
        dex
        bne     @L3

; Restore the old memory configuration, allow interrupts

        pla
        sta     $01                     ; Restore the old configuration
        cli

; Done

@L4:    rts

; ------------------------------------------------------------------------
; COPYTO: Copy from linear into extended memory. A pointer to a structure
; describing the request is passed in a/x.
; The function must not return anything.
;

COPYTO: sta     ptr3
        stx     ptr3+1                  ; Save the passed em_copy pointer

        ldy     #EM_COPY::OFFS
        lda     (ptr3),y
        sta     ptr2
        ldy     #EM_COPY::PAGE
        lda     (ptr3),y
        clc
        adc     #>BASE
        sta     ptr2+1                  ; To

        ldy     #EM_COPY::BUF
        lda     (ptr3),y
        sta     ptr1
        iny
        lda     (ptr3),y
        sta     ptr1+1                  ; From

        jmp     common


; ------------------------------------------------------------------------
; Helper function for moving a block, the following is used:
; ptr1: dst
; ptr2: src
; ptr3: length
; tmp1: dst bank
; tmp2: src bank

transfer:
.P816
.A8
.I8
        pha
        phx
        phy
        ldx     tmp1                    ; load srcbank
        stx     @move+1                 ; store srcbank in move + 1
        ldy     tmp2                    ; load dstbank
        sty     @mapmove+2              ; store dstbank in move + 2
        clc                             ; switch to native mode
        xce
        php                             ; save status bits
        rep     #%00110000              ; set A and index to 16bit
.A16
.I16
        ldy     ptr1  
        ldx     ptr2
        lda     ptr3
@move:
        mvn 0,0
        plp                             ; restore status bits
.A8
.I8
        ply
        plx
        pla
        rts
