; Test ca65's handling of the .paramcount read-only variable.
; .paramcount should see all given arguments, even when they are empty.

.macro  push    r1, r2, r3, r4, r5, r6
        .out    .sprintf(" .paramcount = %u", .paramcount)
.if     .paramcount <> 0
.ifblank        r1
        .warning        "r1 is blank!"
.exitmacro
.endif
        lda     r1
        pha

        push    r2, r3, r4, r5, r6
.endif
.endmacro

        push    1, , {}
        push    1, ,
        push    1
