;
; void* memset (void* ptr, int c, size_t n);
; void* _bzero (void* ptr, size_t n);
; void bzero (void* ptr, size_t n);
;
; Maciej 'YTM/Elysium' Witkowiak, 20.08.2003
;

            .export _memset, _bzero, __bzero
            .import _ClearRam, _FillRam

_bzero          = _ClearRam
__bzero         = _ClearRam
_memset         = _FillRam
