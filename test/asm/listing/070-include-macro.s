.macro IncludeFile FilePath
    .proc bar
        .include FilePath
    .endproc
.endmacro

IncludeFile "070-include-macro.inc"

.ifdef bar::foo
    .out "bar::foo is defined"
.else
    .out "bar::foo is undefined"
.endif
