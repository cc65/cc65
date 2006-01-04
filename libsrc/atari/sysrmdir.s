;
; Stefan Haubenthal, 2005-12-24
;
; unsigned char __fastcall__ _sysrmdir (const char* name);
;

	.export		__sysrmdir
	.import		__sysremove

__sysrmdir := __sysremove
