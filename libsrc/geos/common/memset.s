;
; void* memset (void* ptr, int c, size_t n);
;
; Maciej 'YTM/Elysium' Witkowiak, 15.07.2001
;

 	.export		_memset
	.import		_FillRam

_memset = _FillRam
