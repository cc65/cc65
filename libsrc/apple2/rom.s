;
; Oliver Schmidt, 30.05.2004
;
; Apple2 ROM routines
;

	.export		TEXT
	.export		HGR2
	.export		HGR
	.export		HCLR
	.export		BKGND
	.export		HPOSN
	.export		HPLOT
	.export		HLIN
	.export		HFIND
	.export		DRAW
	.export		XDRAW
	.export		SETHCOL

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

