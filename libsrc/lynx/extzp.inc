;
; extzp.inc for the Lynx
;
; Ullrich von Bassewitz, 2004-11-06
;
; Assembler include file that imports the runtime zero page locations used
; by the Lynx runtime, ready for usage in asm code.
;


        .global         __iodat: zp
        .global         __iodir: zp
        .global         __viddma: zp
        .global         __sprsys: zp
        .global         _abc_score_ptr0: zp
        .global         _abc_score_ptr1: zp
        .global         _abc_score_ptr2: zp
        .global         _abc_score_ptr3: zp
        .global         _FileEntry: zp
        .global         _FileStartBlock: zp
        .global         _FileBlockOffset: zp
        .global         _FileExecFlag: zp
        .global         _FileDestAddr: zp
        .global         _FileFileLen: zp
        .global         _FileCurrBlock: zp
        .global         _FileBlockByte: zp
        .global         _FileDestPtr: zp



