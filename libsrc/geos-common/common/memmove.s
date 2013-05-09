;
; void* __fastcall__ memmove (void* dest, const void* src, size_t n);
;
; Maciej 'YTM/Elysium' Witkowiak, 15.07.2001
;

            .export _memmove
            .import _MoveData

_memmove        = _MoveData
