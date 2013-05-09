;
; Maciej 'YTM/Alliance' Witkowiak
;
; 30.10.99

; void InitForIO  (void);
; void DoneWithIO (void);

            .export _InitForIO, _DoneWithIO

            .include "jumptab.inc"

_InitForIO      = InitForIO
            
_DoneWithIO     = DoneWithIO