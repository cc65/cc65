;
; Christian Groessler, Oct-2000
;
; offsets and defines for fdtable (internal use only)
;

MAX_FD_INDEX	=	12
MAX_FD_VAL	=	8	; we have 8 IOCBs

ft_entrylen = 4	; length of table entry (it's not sufficient to change here!
		; the code sometimes does two bit shifts to multiply/divide by
		; this length)

ft_usa  = 0	; usage counter
ft_iocb	= 1	; iocb index (0,$10,$20,etc.), $ff for empty entry
ft_dev  = 2	; device of open iocb (0 - device not remembered, eg. filename specified)
ft_flag = 3	; flags
		; lower 3 bits: device number (for R: and D:)

