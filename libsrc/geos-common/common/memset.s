;
; void* memset (void* ptr, int c, size_t n);
; void* __bzero (void* ptr, size_t n);
; void bzero (void* ptr, size_t n);
;
; Maciej 'YTM/Elysium' Witkowiak, 20.08.2003
;

            .export _memset, _bzero, ___bzero
            .import _ClearRam, _FillRam

_bzero          = _ClearRam
___bzero        = _ClearRam
_memset         = _FillRam
