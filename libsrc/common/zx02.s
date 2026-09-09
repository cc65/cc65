; void __fastcall__ decompress_zx02(const void *src, void *dest)
;
; De-compressor for ZX02 files
;
; Compress with:
;    zx02 input.bin output.zx0
;
; (c) 2022 DMSC
; Code under MIT license, see LICENSE file.

        .export         _decompress_zx02

        .import         popax
        .importzp       ptr1, ptr2, ptr3, tmp1, tmp2

offset_hi = tmp1
ZX0_src   = ptr1
ZX0_dst   = ptr2
bitr      = tmp2
pntr      = ptr3

.proc _decompress_zx02
        sta     ZX0_dst
        stx     ZX0_dst+1

        jsr     popax
        sta     ZX0_src
        stx     ZX0_src+1

        .include "zx02_direct.inc"
.endproc
