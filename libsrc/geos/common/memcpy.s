;
; void* memcpy (void* dest, const void* src, size_t n);
; void* memmove (void* dest, const void* src, size_t n);
;
; Maciej 'YTM/Elysium' Witkowiak, 15.07.2001
;

  	.export		_memcpy, _memmove
	.import		_MoveData

_memcpy	= _MoveData
_memmove = _MoveData
