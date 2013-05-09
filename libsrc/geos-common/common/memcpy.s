;
; void* __fastcall__ memcpy (void* dest, const void* src, size_t n);
;
; Maciej 'YTM/Elysium' Witkowiak, 15.07.2001
;

            .export _memcpy
            .import _MoveData

_memcpy         = _MoveData
