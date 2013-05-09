;
; Karri Kaksonen, 2010
;
; lynx_exec(fileno) loads a file into memory but after the read the CPU
; does a jump into the loaded start address.
;
; lynx_exec is often used in compilation carts when you run small demos
; created with various (non-cc65) compilers.
; 
; void lynx_exec(int fileno)
;
        .importzp       _FileDestAddr
        .import         pushax,ldax0sp,incsp2
        .import         _lynx_load
        .export         _lynx_exec

; ---------------------------------------------------------------
; void __near__ __fastcall__ lynx_exec (int)
; ---------------------------------------------------------------

.segment        "CODE"

.proc   _lynx_exec: near

.segment        "CODE"

        jsr     pushax
        jsr     ldax0sp
        jsr     _lynx_load
        jsr     incsp2
        jmp     (_FileDestAddr)

.endproc

