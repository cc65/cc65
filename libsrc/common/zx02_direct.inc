; ZX02 decompressor code, in its own file for inclusion in various users
; (zx02.s provides void __fastcall__ decompress_zx02(const void *src, void *dest),
;  and the Apple II system loader calls _decompress_zx02_direct directly).

_decompress_zx02_direct:
        ; Init values
        lda     #$80
        sta     bitr
        ldy     #$FF
        sty     pntr
        iny
        sty     offset_hi ; Y = 0 at end of init

; Decode literal: Copy next N bytes from compressed file
; Elias(length)  byte[1]  byte[2]  ...  byte[N]
decode_literal:
        ldx     #$01
        jsr     get_elias

cop0:
        lda     (ZX0_src), y
        sta     (ZX0_dst),y
        inc     ZX0_src
        bne     :+
        inc     ZX0_src+1

; Optionally, show decompression progress by animating a character on screen.
.ifdef ZX02_DECOMPRESSOR_SHOW_PROGRESS
indicator = *+1
        lda     #'-'|$80
        eor     #%00001100    ; switch between '!' (00100001) and '-' (00101101)
        sta     indicator

; Call the relevant platform's character output function
.ifdef __APPLE2__
        jsr     COUT
        dec     CH
.endif
.endif

:       inc     ZX0_dst
        bne     :+
        inc     ZX0_dst+1

:       dex
        bne     cop0
        bne     cop0

        asl     bitr
        bcs     dzx0s_new_offset

        ; Copy from last offset (repeat N bytes from last offset)
        ; Elias(length)
        inx
        jsr     get_elias

dzx0s_copy:
        lda     ZX0_dst+1
        sbc     offset_hi  ; C=0 from get_elias
        sta     pntr+1

cop1:
        ldy     ZX0_dst
        lda     (pntr), y
        ldy     #0
        sta     (ZX0_dst),y
        inc     ZX0_dst
        bne     :+
        inc     ZX0_dst+1
        inc     pntr+1
:       dex
        bne     cop1

        asl     bitr
        bcc     decode_literal

; Copy from new offset (repeat N bytes from new offset)
; Elias(MSB(offset))  LSB(offset)  Elias(length-1)
dzx0s_new_offset:
        ; Read elias code for high part of offset
        inx
        jsr     get_elias
        beq     exit  ; Read a 0, signals the end

        ; Decrease and divide by 2
        dex
        txa
        lsr
        sta     offset_hi

        ; Get low part of offset, a literal 7 bits
        lda     (ZX0_src), y
        inc     ZX0_src
        bne     :+
        inc     ZX0_src+1

:       ; Divide by 2
        ror
        eor     #$ff
        sta     pntr

        ; And get the copy length.
        ; Start elias reading with the bit already in carry:
        ldx     #1
        jsr     elias_skip1

        inx
        bcc     dzx0s_copy

; Read an elias-gamma interlaced code.
elias_get:
        ; Read next data bit to result
        asl     bitr
        rol
        tax

get_elias:
        ; Get one bit
        asl     bitr
        bne     elias_skip1

        ; Read new bit from stream
        lda     (ZX0_src), y
        inc     ZX0_src
        bne     :+
        inc     ZX0_src+1

:     ; sec   ; not needed, C=1 guaranteed from last bit
        rol
        sta     bitr

elias_skip1:
        txa
        bcs     elias_get

        ; Got ending bit, stop reading
exit:
        rts
