;
; Maciej 'YTM/Elysium' Witkowiak
;
; 29.10.1999, 2.1.2003

; char TestPoint        (struct pixel *mypixel);

            .import PointRegs
            .import return0, return1
            .export _TestPoint

            .include "jumptab.inc"
            
_TestPoint:
        jsr PointRegs
        jsr TestPoint
        bcc goFalse
        jmp return1
goFalse:jmp return0
