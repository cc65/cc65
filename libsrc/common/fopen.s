;
; Ullrich von Bassewitz, 22.11.2002
;
; FILE* __fastcall__ fopen (const char* name, const char* mode)
; /* Open a file */
;

        .export         _fopen

        .import         __fopen, __fdesc
        .import         pushax, return0

        .include        "errno.inc"


; ------------------------------------------------------------------------
; Code

.proc   _fopen

; Bring the mode parameter on the stack

        jsr     pushax

; Allocate a new file stream

        jsr     __fdesc

; Check if we have a stream

        cmp     #$00
        bne     @L1
        cpx     #$00
        bne     @L1

; Failed to allocate a file stream

        lda     #EMFILE
        jsr     __seterrno      ; Set __errno, will return 0 in A
        tax
        rts                     ; Return zero

; Open the file and return the file descriptor. All arguments are already
; in place: name and mode on the stack, and f in a/x

@L1:    jmp     __fopen

.endproc

