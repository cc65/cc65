
; Maciej 'YTM/Elysium' Witkowiak
; 06.06.2002

; This is source for main VLIR-structured program part

    ; include some GEOS defines

	    .include "../../../libsrc/geos/inc/const.inc"
	    .include "../../../libsrc/geos/inc/jumptab.inc"
	    .include "../../../libsrc/geos/inc/geossym.inc"
	    .include "../../../libsrc/geos/inc/geosmac.ca65.inc"

    ; import load addresses for all VLIR chains
    ; by default they are all the same, but this is not required
    ; these labels are defined upon linking with ld65 - each segment has it
	    .import __VLIR1_LOAD__
	    .import __VLIR2_LOAD__

    ; import names of functions defined (and exported) in each VLIR part
    ; of your application
    ; here I used VLIRx_ prefix to prevent name clash

	    .import VLIR1_Function1
	    .import VLIR2_Function2

    ; segments "CODE", "DATA", "RODATA" and "BSS" all go to VLIR0 chain

	    .segment "CODE"
	    ; code segment for VLIR 0 chain
ProgExec:
		LoadW r0, paramString		; show something
	        jsr DoDlgBox

		MoveW dirEntryBuf+OFF_DE_TR_SC, r1
		LoadW r4, fileHeader
		jsr GetBlock			; load back VLIR t&s table
		bnex error

		lda #1
		jsr PointRecord			; we want next module (#1)
		LoadW r2, $ffff			; length - as many bytes as there are
		LoadW r7, __VLIR1_LOAD__	; all VLIR segments have the same load address
		jsr ReadRecord			; load it
		bnex error
		jsr VLIR1_Function1		; execute something

		lda #2
		jsr PointRecord			; next module
		LoadW r2, $ffff
		LoadW r7, __VLIR2_LOAD__
		jsr ReadRecord			; load it
		bnex error
		jsr VLIR2_Function2		; execute something

error:		jmp EnterDeskTop		; end of application

	    .segment "RODATA"
	    ; read-only data segment
paramString:
	    .byte DEF_DB_POS | 1
	    .byte DBTXTSTR, TXT_LN_X, TXT_LN_2_Y
	    .word line1
	    .byte DBTXTSTR, TXT_LN_X, TXT_LN_3_Y
	    .word line2
	    .byte OK, DBI_X_0, DBI_Y_2
	    .byte NULL

line1:	    .byte BOLDON, "Hello World!",0
line2:	    .byte OUTLINEON,"Hello",PLAINTEXT," world!",0

	    .segment "DATA"
	    ; read/write initialized data segment
counter:    .word 0

	    .segment "BSS"
	    ; read/write uninitialized data segment
	    ; this space doesn't go into output file, only its size and
	    ; position is remembered
