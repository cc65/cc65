
;-----------------------------------------------------------------------------
; Zero page stuff

WNDLFT	:=	$20	; Left edge of text window
WNDWDTH	:=	$21	; Right edge of text window
WNDTOP	:=	$22	; Top of text window
WNDBTM	:=	$23	; Bottom+1 of text window
CH	:=	$24	; Cursor horizontal position
CV	:=	$25	; Cursor vertical position
BASL	:=	$28	; Text base address
INVFLG	:=	$32	; Normal/inverse(/flash)
RNDL	:=	$4E	; Random counter low
RNDH	:=	$4F	; Random counter high
HIMEM	:=	$73	; Highest available memory address+1

;-----------------------------------------------------------------------------
; Vectors

DOSWARM	:=	$03D0	; DOS warmstart vector
BRKVec	:=	$03F0	; Break vector
MLI	:=	$BF00	; ProDOS Machine Language Interface

;-----------------------------------------------------------------------------
; Hardware

; Keyboard entries
KBD	:=	$C000	; Read keyboard
KBDSTRB	:=	$C010	; Clear keyboard strobe

; 80 column card switches
CLRALTCHAR :=	$C00E	; Normal Apple II char set
SETALTCHAR :=	$C00F	; Norm/inv LC, no flash
ALTCHARSET :=	$C01E	; >127 if alt charset switched in
RD80VID :=	$C01F	; >127 if 80 column video enabled

; Video soft switches
MIXCLR	:=	$C052	; Disable 4 lines of text
MIXSET	:=	$C053	; Enable 4 lines of text
LOWSCR	:=	$C054	; Page 1
HISCR	:=	$C055	; Page 2

; Game controller
BUTN0	:=	$C061	; Open-Apple Key
BUTN1	:=	$C062	; Closed-Apple Key

;-----------------------------------------------------------------------------
; ROM routines

PREAD	:=	$FB1E	; Read paddle in X, return AD conv. value in Y
RDKEY	:=	$FD0C	; Display prompt and read key from user input routine

; Text output and helpers
VTABZ	:=	$FC24	; Calculate screen address for row in accumulator
HOME	:=	$FC58	; Clear current text screen
COUT	:=	$FDED	; Vector to user output routine

; Graphics entry points, by cbmnut (applenut??) cbmnut@hushmail.com
TEXT    :=	$F399	; Return to text screen
HGR2    :=	$F3D8	; Initialize and clear hi-res page 2.
HGR     :=	$F3E2	; Initialize and clear hi-res page 1.
HCLR    :=	$F3F2	; Clear the current hi-res screen to black.
BKGND   :=	$F3F6	; Clear the current hi-res screen to the
                        ; last plotted color (from ($1C).
HPOSN   :=	$F411	; Positions the hi-res cursor without
                        ; plotting a point.
                        ; Enter with (A) = Y-coordinate, and
                        ; (Y,X) = X-coordinate.
HPLOT   :=	$F457	; Calls HPOSN and tries to plot a dot at
                        ; the cursor's position.  If you are
                        ; trying to plot a non-white color at
                        ; a complementary color position, no
                        ; dot will be plotted.
HLIN    :=	$F53A	; Draws a line from the last plotted
                        ; point or line destination to:
                        ; (X,A) = X-coordinate, and
                        ; (Y) = Y-coordinate.
HFIND   :=	$F5CB	; Converts the hi-res coursor's position
                        ; back to X- and Y-coordinates; stores
                        ; X-coordinate at $E0,E1 and Y-coordinate
                        ; at $E2.
DRAW    :=	$F601	; Draws a shape.  Enter with (Y,X) = the
                        ; address of the shape table, and (A) =
                        ; the rotation factor.  Uses the current
                        ; color.
XDRAW   :=	$F65D	; Draws a shape by inverting the existing
                        ; color of the dots the shape draws over.
                        ; Same entry parameters as DRAW.
SETHCOL :=	$F6EC	; Set the hi-res color to (X), where (X)
                        ; must be between 0 and 7.
