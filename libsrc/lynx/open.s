;
; Karri Kaksonen, 2010
;
; This function reads the directory entry for file "name".
;
; The name is actually plain ASCII string starting from
; "0", "1", up to "4095" which is the largest file number we can handle.
;
; open() does not take part in what kind of cart we have. If it is RAM
; you may also be able to write into it. Therefore we allow both reads
; and writes in this routine.
;
; int open(const char *name, int flags, ...)
;
; As helper functions we also provide.
; void openn(int fileno)
;
        .importzp       sreg, tmp3
        .macpack        longbranch
        .import         _atoi
        .import         _read
        .import         _lseek
        .import         addysp,popax,pushax,decsp6,pusha0,pusheax,ldaxysp
        .import         aslax3,axlong,tosaddeax,steaxysp,stax0sp,incsp8
        .import         ldax0sp
        .import         lynxskip0, lynxblock
        .importzp       _FileEntry
        .importzp       _FileStartBlock
        .importzp       _FileCurrBlock
        .importzp       _FileBlockOffset
        .import         __STARTOFDIRECTORY__
        .export         _open
        .export         _openn

        .include        "errno.inc"
        .include        "fcntl.inc"

.segment        "DATA"

_startofdirectory:
        .dword  __STARTOFDIRECTORY__

; ---------------------------------------------------------------
; int __near__ open (__near__ const unsigned char*, int)
; ---------------------------------------------------------------

.segment        "CODE"

.proc   _open

.segment        "CODE"

        dey
        dey
        dey
        dey
        beq     parmok
        jsr     addysp

parmok: jsr     popax
        sta     tmp3
        and     #(O_RDWR | O_CREAT)
        cmp     #O_RDONLY
        beq     flagsok
        cmp     #(O_WRONLY | O_CREAT)
        beq     flagsok
        jsr     popax
        lda     #EINVAL
        jmp     __directerrno

flagsok:
        jsr     popax
        jsr     _atoi
        jsr     _openn
        ldx     #$00
        lda     #$01
        stx     __oserror
        rts

.endproc

; ---------------------------------------------------------------
; void __near__ __fastcall__ openn (int)
; ---------------------------------------------------------------

.segment        "CODE"

.proc   _openn: near

.segment        "CODE"

        jsr     pushax
        jsr     decsp6
        lda     #$01
        jsr     pusha0
        lda     _startofdirectory+3
        sta     sreg+1
        lda     _startofdirectory+2
        sta     sreg
        ldx     _startofdirectory+1
        lda     _startofdirectory
        jsr     pusheax
        ldy     #$0D
        jsr     ldaxysp
        jsr     aslax3
        jsr     axlong
        jsr     tosaddeax
        jsr     pusheax
        ldx     #$00
        txa
        jsr     _lseek
        ldy     #$02
        jsr     steaxysp
        lda     #$01
        jsr     pusha0
        lda     #<_FileEntry
        ldx     #>_FileEntry
        jsr     pushax
        ldx     #$00
        lda     #$08
        jsr     _read
        lda     _FileStartBlock
        sta     _FileCurrBlock
        jsr     lynxblock
        lda     _FileBlockOffset+1
        eor     #$FF
        tay
        lda     _FileBlockOffset
        eor     #$FF
        tax
        jsr     lynxskip0
        jsr     stax0sp
        jmp     incsp8

.endproc

