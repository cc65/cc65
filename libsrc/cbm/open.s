;
; Ullrich von Bassewitz, 16.11.2002
;
; int open (const char* name, int flags, ...);	/* May take a mode argument */
;

        .export         _open

        .import         addysp, popax
        .import         scratch, fnparse, fncomplete, fnset
        .import         opencmdchannel, closecmdchannel, readdiskerror
        .import         __errno, __oserror
        .import         fnunit
        .importzp       sp, tmp2, tmp3

        .include        "errno.inc"
        .include        "fcntl.inc"
        .include        "cbm.inc"
        .include        "filedes.inc"


;--------------------------------------------------------------------------
; initstdout: Open the stdout and stderr file descriptors for the screen.

.proc   _open

	cpy	#4     	   	; correct # of arguments (bytes)?
	beq	parmok 	   	; parameter count ok
	tya	       	   	; parm count < 4 shouldn't be needed to be...
       	sec    	       	       	; ...checked (it generates a c compiler warning)
	sbc	#4
	tay
	jsr	addysp	   	; fix stack, throw away unused parameters

; Parameters ok. Pop the flags and save them into tmp3

parmok: jsr     popax           ; Get flags
        sta     tmp3

; Get the filename from stack and parse it. Bail out if is not ok

        jsr     popax           ; Get name
        jsr     fnparse         ; Parse it
        cmp     #0
        bne     error           ; Bail out if problem with name

; Get a free file handle and remember it in tmp2

        jsr     freefd
        bcs     nofile
        stx     tmp2

; Check the flags. We cannot have:
;
;  - both, read and write flags set
;  - the append flag set
;

        lda     tmp3
        and     #O_RDWR
        beq     invflags        ; Neither read nor write
        cmp     #O_RDWR
        beq     invflags        ; Jump if both set
        cmp     #O_RDONLY
        beq     doread

; Write bit is set. We cannot open a file for writing without creating it,
; so check for the O_CREAT bit.

        lda     tmp3
        and     #O_CREAT
        beq     invflags

; If O_TRUNC is set, scratch the file, but ignore any errors

        lda     tmp3
        and     #O_TRUNC
        beq     notrunc
        jsr     scratch

; Complete the the file name

notrunc:
        lda     #'w'
        jsr     fncomplete

; Setup the real open flags

        lda     #LFN_WRITE
        bne     common

; Read bit is set. Add an 'r' to the name

doread: lda     #'r'
        jsr     fncomplete
        lda     #LFN_READ

; Common read/write code. Flags in A, handle in tmp2

common: sta     tmp3
        jsr     fnset           ; Set the file name

        lda     tmp2
        clc
        adc     #LFN_OFFS
        ldx     fnunit
        tay                     ; Use the LFN also as SA
        jsr     SETLFS          ; Set the file params

        jsr     OPEN
        bcs     error

; Open the the drive command channel and read it

        ldx     fnunit
        jsr     opencmdchannel
        bne     closeandexit
        ldx     fnunit
        jsr     readdiskerror
        bne     closeandexit    ; Branch on error

; File is open. Mark it as open in the table

        ldx     tmp2
        lda     tmp3
        sta     fdtab,x
        lda     fnunit
        sta     unittab,x       ; Remember

; Done. Return the handle in a/x

        txa                     ; Handle
        ldx     #0
        rts

; Error entry: No more file handles

nofile: lda     #1              ; Too many open files

; Error entry. Error code is in A.

error:  sta     __oserror
errout: lda     #$FF
        tax                     ; Return -1
        rts

; Error entry: Invalid flag parameter

invflags:
        lda     #EINVAL
        sta     __errno
        lda     #0
        sta     __errno+1
        beq     errout

; Error entry: Close the file and exit

closeandexit:
        pha
        lda     tmp2
        clc
        adc     #LFN_OFFS
        jsr     CLOSE
        ldx     fnunit
        jsr     closecmdchannel
        pla
        bne     error           ; Branch always

.endproc


