;
; Ullrich von Bassewitz, 2003-08-20
; Christian Krueger: 2009-Sep-13, performance increase (about 20%)
;                    2013-Jul-24, regcall parameter passing 
;
; void* __fastcall__ memcpy (void* dest, const void* src, size_t count);
; void* __fastcall__ _rc_memcpy(void);
; ptr1 = void* dest
; ptr2 = const void* src
; ptr3 = size_t n
;
; NOTE: This function contains entry points for memmove, which will ressort
; to memcpy for an upwards copy. Don't change this module without looking
; at memmove!
;

.export	    	_memcpy, _rc_memcpy, rc_memcpy_upwards
.importzp      	sp, ptr1, ptr2, ptr3, ptr4
.import 		popax

; ----------------------------------------------------------------------

_memcpy:
    sta ptr3
    stx ptr3+1
    
    jsr popax
    sta ptr2
    stx ptr2+1

    jsr popax
    sta ptr1
    stx ptr1+1

_rc_memcpy:
    lda ptr1+1      ; save destination
    sta ptr4+1      ; for restore at end (ptr1 untouched!)
rc_memcpy_upwards:  ; (entry point from memmove(), where dest is already saved
    ldy #0          ; init offset
	ldx	ptr3+1      ; Get high byte of n
    beq L2          ; Jump if zero

L1:
  .repeat 2		    ; unroll this a bit to make it faster...
	lda	(ptr2),Y	; copy a byte
	sta	(ptr1),Y
	iny
  .endrepeat
	bne	L1
	inc	ptr2+1
	inc	ptr1+1
	dex			    ; next 256 byte block
	bne	L1		    ; repeat if any

	; the following section could be 10% faster if we were able to copy
	; back to front - unfortunately we are forced to copy strict from
	; low to high since this function is also used for
	; memmove and blocks could be overlapping!
	; {
L2:				    ; assert Y = 0
	ldx	ptr3		; Get the low byte of n
	beq	done		; something to copy

L3:	lda	(ptr2),Y	; copy a byte
	sta	(ptr1),Y
	iny
	dex
	bne	L3

	; }

done:
    lda ptr1        ; restore dest for return...
    ldx ptr4+1      ; (touched high byte saved in ptr4)
    rts

