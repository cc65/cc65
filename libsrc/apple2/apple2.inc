; Break vector
BRKVec  = $03F0

; Goto Dos
RESTOR	=	$03D0

; Top of available memory
; This is actually for DOS 3.3 need to change it for ProDos
TOPMEM  =       $9600

; Soft switches
;
; write to USEROM to enable apple rom C000-CFFF
USEROM	=	$C007
; 80 column card switches
C80ON	=	$C00C
C80OFF	=	$C00D
RD80COL	=	$C01F
PG2OFF	=	$C054
PG2ON	=	$C055
RDPAGE2	=	$C01C

; Text routines
MIN_X	=	$20
MAX_X	=	$21
MIN_Y	=	$22
MAX_Y	=	$23
CH	=	$24
CV	=	$25
BASL	=	$28
TEXTTYP	=	$32
MEMSIZE	=	$73		; Highest free RAM location
HOME	=	$FC58
VTABZ	=	$FC24
COUT    = 	$FDED

; Keyboard entries
RDKEY	=	$FD0C
CLEAR_KEY_STROBE = $C010
KEY_STROBE       = $C000

; Game controller
OPEN_APPLE = $C061
CLOSED_APPLE = $C062
