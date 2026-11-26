;
; void* memset (void* ptr, int c, size_t n);
; void* __bzero (void* ptr, size_t n);
; void bzero (void* ptr, size_t n);
;
; Maciej 'YTM/Elysium' Witkowiak, 20.08.2003
;

            .export _bzero, ___bzero, _memset, ___memset_common
            .import _ClearRam, popax, ldax0sp

            .include "geossym.inc"
            .include "jumptab.inc"

_bzero          = _ClearRam
___bzero        = _ClearRam

_memset:
        sta r0L
        stx r0H
        jsr popax       ; pop 'int c'
___memset_common:
        sta r2L
        jsr ldax0sp     ; load 'ptr'
        sta r1L
        stx r1H
        jsr FillRam
        jsr popax       ; pop 'ptr' and return it
        rts
