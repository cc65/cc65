
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

	    .export DB_get2lines
	    .importzp ptr3,ptr4
	    .import popax

DB_get2lines:
	    sta ptr4
	    stx ptr4+1
	    jsr popax
	    sta ptr3
	    stx ptr3+1
	    rts
