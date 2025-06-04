;
; Startup code for cc65 (Agat version)
;

        .export         __STARTUP__ : absolute = 1      ; Mark as startup

        .import         initlib, donelib
        .import         zerobss, callmain
        .import         __ONCE_LOAD__, __ONCE_SIZE__    ; Linker generated

        .include        "zeropage.inc"
        .include        "agat.inc"

; ------------------------------------------------------------------------

	.segment        "STARTUP"
	lda	HIMEM
	ldx	HIMEM+1
	sta	sp
	stx	sp+1
	jsr	initlib
	jsr	zerobss
	jsr	callmain
	jsr	donelib
	rts
