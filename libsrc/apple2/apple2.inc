
;-----------------------------------------------------------------------------
; Zero page stuff

MIN_X	=	$20
MAX_X	=	$21
MIN_Y	=	$22
MAX_Y	=	$23
CH	=	$24
CV	=	$25
BASL	=	$28
TEXTTYP	=	$32
RNDL    =       $4E             ; Random value, low byte
RNDH    =       $4F             ; Random value, high byte
MEMSIZE	=	$73		; Highest free RAM location

;-----------------------------------------------------------------------------
; Vectors

BRKVec  =       $03F0           ; Break vector
RESTOR	=	$03D0           ; Goto Dos

;-----------------------------------------------------------------------------
; Hardware

; Soft switches
;
; Write to USEROM to enable apple rom C000-CFFF
USEROM 	:=      $C007

; 80 column card switches
C80ON	:= 	$C00C
C80OFF	:= 	$C00D
RD80COL	:= 	$C01F
PG2OFF	:= 	$C054
PG2ON	:= 	$C055
RDPAGE2	:= 	$C01C

; Keyboard entries
CLEAR_KEY_STROBE:= $C010
KEY_STROBE      := $C000

; Game controller
OPEN_APPLE      := $C061
CLOSED_APPLE    := $C062

;-----------------------------------------------------------------------------
; ROM routines

RDKEY  	:=      $FD0C

; Text output and helpers
HOME   	:=     	$FC58
VTABZ  	:=	$FC24
COUT    :=     	$FDED

; Graphics entry points, by cbmnut (applenut??) cbmnut@hushmail.com
WHITE   = $01           ; White for $1c, I think...
COLOR   = $1C           ; Current plot color

TEXT    := $F399        ; Return to text screen
HGR2    := $F3D8        ; Initialize and clear hi-res page 2.
HGR     := $F3E2        ; Initialize and clear hi-res page 1.
HCLR    := $F3F2        ; Clear the current hi-res screen to black.
BKGND   := $F3F6        ; Clear the current hi-res screen to the
                        ; last plotted color (from ($1C).
HPOSN   := $F411        ; Positions the hi-res cursor without
                        ; plotting a point.
                        ; Enter with (A) = Y-coordinate, and
                        ; (Y,X) = X-coordinate.
HPLOT   := $F457        ; Calls HPOSN and tries to plot a dot at
                        ; the cursor's position.  If you are
                        ; trying to plot a non-white color at
                        ; a complementary color position, no
                        ; dot will be plotted.
HLIN    := $F53A        ; Draws a line from the last plotted
                        ; point or line destination to:
                        ; (X,A) = X-coordinate, and
                        ; (Y) = Y-coordinate.
HFIND   := $F5CB        ; Converts the hi-res coursor's position
                        ; back to X- and Y-coordinates; stores
                        ; X-coordinate at $E0,E1 and Y-coordinate
                        ; at $E2.
DRAW    := $F601        ; Draws a shape.  Enter with (Y,X) = the
                        ; address of the shape table, and (A) =
                        ; the rotation factor.  Uses the current
                        ; color.
XDRAW   := $F65D        ; Draws a shape by inverting the existing
                        ; color of the dots the shape draws over.
                        ; Same entry parameters as DRAW.
SETHCOL := $F6EC        ; Set the hi-res color to (X), where (X)
                        ; must be between 0 and 7.
PREAD   := $FB1E        ; Read paddle in X, return AD conv. value in Y
