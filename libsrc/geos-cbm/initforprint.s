;
; 2026 cc65 project (added for PRINTER driver support, see grc65)
;

; void InitForPrint (void);

            .export _InitForPrint

            .include "printdrv.inc"

_InitForPrint:
        jmp InitForPrint
