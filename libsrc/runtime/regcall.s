; regcall.s
; Export of zero page locations for quick function calling via 'registers'
;
; (c) Christian Krüger, latest change: 17-Jul-2013
;
; This software is provided 'as-is', without any expressed or implied
; warranty.  In no event will the authors be held liable for any damages
; arising from the use of this software.
;
; Permission is granted to anyone to use this software for any purpose,
; including commercial applications, and to alter it and redistribute it
; freely, subject to the following restrictions:
;
; 1. The origin of this software must not be misrepresented; you must not
;    claim that you wrote the original software. If you use this software
;    in a product, an acknowledgment in the product documentation would be
;    appreciated but is not required.
; 2. Altered source versions must be plainly marked as such, and must not
;    be misrepresented as being the original software.
; 3. This notice may not be removed or altered from any source
;    distribution.
;

.include "zeropage.inc"

.EXPORTZP _R0 := ptr1
.EXPORTZP _R1 := ptr2
.EXPORTZP _R2 := ptr3
.EXPORTZP _R3 := tmp1
.EXPORTZP _R4 := tmp3

