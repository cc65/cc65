
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001

; void cclearxy (unsigned char x, unsigned char y, unsigned char length);
; void cclear (unsigned char length);
; void chlinexy (unsigned char x, unsigned char y, unsigned char length);
; void chline (unsigned char length);
; void cvlinexy (unsigned char x, unsigned char y, unsigned char length);
; void cvline (unsigned char length);

    	.export		_cclearxy, _cclear
    	.export		_chlinexy, _chline
    	.export		_cvlinexy, _cvline
	.import popa

; unimplemented, will do nothing now

_cclearxy:
_chlinexy:
_cvlinexy:

		jsr popa
_cclear:
_chline:
_cvline:
		rts
