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
isnotscpu:      .res    1               ; SuperCPU not present
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
        adc     #$01                    ; on 65C02, 65SC02, 65CE02, 65802 and 65816 sets the zero flag correctly
        cld
        bne     @not_present
        clc
.P816
        sep     #$01                    ; nop #$01 on 65C02/65SC02 and lda ($01,s),y on 65CE02
.P02
        bcc     @not_present
        lda     $d0bc
        and     #$80
        sta     isnotscpu
        lda     $07e8
        pha                             ; save value incase it was used somewhere else
        ldx     #$ff
@fillloop:                              ; fill from top (bank 255) to bottom
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
@compareloop:                           ; check from bottom to top
        txa
        pha
.P816
        plb
.P02
        cmp     $07e8
        bne     @found_pages
.P816
        inc
.P02
        sta     $07e8
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
        cli
        lda     isnotscpu
        bne     @noextradex
        dex
@noextradex:
        stx     bankcount
        lda     #<EM_ERR_OK
        ldx     #>EM_ERR_OK
        rts
@not_present:
        cli
        lda     #<EM_ERR_NO_DEVICE
        ldx     #>EM_ERR_NO_DEVICE
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
        lda     #$00                   ; a whole bank is either usable or not
        ldx     bankcount
        rts

; ------------------------------------------------------------------------
; MAP: Map the page in a/x into memory and return a pointer to the page in
; a/x. The contents of the currently mapped page (if any) may be discarded
; by the driver.
;

MAP:    sta     curpage                 ; Remember the new page
        stx     curbank                 ; Remember the new bank

        sta     ptr2+1                  ; src address low
        lda     #$00
        sta     ptr2                    ; src address high
        inx
        ldy     isnotscpu               ; check if not scpu
        bne     @notscpu
        inx
@notscpu:
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
        sta     ptr1+1                  ; dst high
        ldx     #$00
        stx     ptr1                    ; dst low

        lda     #<window
        sta     ptr2                    ; src low
        lda     #>window
        sta     ptr2+1                  ; src high

        stx     ptr3+1                  ; length high
        lda     #$ff
        sta     ptr3                    ; length low

        stx     tmp2                    ; src bank
        ldy     curbank                 ; Get the current bank
        iny
        ldx     isnotscpu
        bne     @notascpu
        iny
@notascpu:
        sty     tmp1                    ; dst bank

        jsr     transfer

        rts

; ------------------------------------------------------------------------
; COPYFROM: Copy from extended into linear memory. A pointer to a structure
; describing the request is passed in a/x.
; The function must not return anything.
;

COPYFROM:
        sta     ptr4
        stx     ptr4+1                  ; Save the passed em_copy pointer

        ldy     #EM_COPY::COUNT+1       ; start at the end of the struct
        lda     (ptr4),y                ; get high byte of count
        tax
        dey
        lda     (ptr4),y                ; get low byte of count
        bne     @nodex
        dex
@nodex:
.P816
        dec
.P02
        sta     ptr3                    ; length low
        stx     ptr3+1                  ; length high
        dey
        lda     (ptr4),y                ; get bank
.P816
        inc
.P02
        ldx     isnotscpu
        bne     @notscpu64
.P816
        inc
.P02
@notscpu64:
        sta     tmp2                    ; src bank
        dey
        lda     (ptr4),y                ; get page
        sta     ptr2+1                  ; src high
        dey
        lda     (ptr4),y                ; get offset in page
        sta     ptr2                    ; src low
        dey
        lda     (ptr4),y                ; get memory buffer high
        sta     ptr1+1                  ; dst high
        dey
        lda     (ptr4),y                ; get memory buffer low
        sta     ptr1                    ; dst low
        lda     #$00
        sta     tmp1                    ; dst bank

        jsr     transfer

        rts

; ------------------------------------------------------------------------
; COPYTO: Copy from linear into extended memory. A pointer to a structure
; describing the request is passed in a/x.
; The function must not return anything.
;

COPYTO: sta     ptr4
        stx     ptr4+1                  ; Save the passed em_copy pointer

        ldy     #EM_COPY::COUNT+1       ; start at the end of the struct
        lda     (ptr4),y                ; get high byte of count
        tax
        dey
        lda     (ptr4),y                ; get low byte of count
        bne     @nodex2
        dex
@nodex2:
.P816
        dec
.P02
        sta     ptr3                    ; length low
        txa
        sta     ptr3+1                  ; length high
        dey
        lda     (ptr4),y                ; get bank
.P816
        inc
.P02
        ldx     isnotscpu
        bne     @notascpu64
.P816
        inc
.P02
@notascpu64:
        sta     tmp1                    ; dst bank
        dey
        lda     (ptr4),y                ; get page
        sta     ptr1+1                  ; dst high
        dey
        lda     (ptr4),y                ; get page offset
        sta     ptr1                    ; dst low
        dey
        lda     (ptr4),y                ; get memory buffer high
        sta     ptr2+1                  ; src low
        dey
        lda     (ptr4),y                ; get memory buffer low
        sta     ptr2                    ; src high
        lda     #$00
        sta     tmp2                    ; src bank

        jsr     transfer

        rts

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
        sei
        pha
        phx
        phy
        ldx     tmp1                    ; load srcbank
        stx     @move+1                 ; store srcbank in move + 1
        ldy     tmp2                    ; load dstbank
        sty     @move+2                 ; store dstbank in move + 2
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
        lda     #$00
        pha
        plb                             ; restore dbr
        sec
        xce                             ; switch to emul mode
        ply
        plx
        pla
        cli
        rts
.P02
