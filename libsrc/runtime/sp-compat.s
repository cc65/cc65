;
; Kugelfuhr, 2025-06-26
;
; Add "sp" as an alias for "c_sp" so we don't break old code but emit a
; linker warning if it is used. Added after renaming "sp" to "c_sp".
;

.include        "zeropage.inc"
.export         sp := c_sp
.assert         0, ldwarning, "Symbol 'sp' is deprecated - please use 'c_sp' instead"

