;
; Christian Groessler, June 2000
;
; this file provides the default for the __graphmode_used variable

	.export	__graphmode_used

	.data

__graphmode_used:
	.byte	0		; text screen
; disabled, because currently there aren't any screen mode functions available
;	.byte	9		; mode with max. memory usage

