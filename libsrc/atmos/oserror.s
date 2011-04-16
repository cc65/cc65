;
; Stefan Haubenthal, 2011-04-10
;
; int __fastcall__ _osmaperrno (unsigned char oserror);
; /* Map a system specific error into a system independent code */
;

	.export		__osmaperrno
	.import		return0

__osmaperrno	= return0
