;
; Maciej 'YTM/Elysium' Witkowiak <ytm@elysium.pl>
; 23.12.2002
;
; CC65 runtime: call function via pointer in ax
;

        .include "jumptab.inc"

	.export		callax

callax = CallRoutine
