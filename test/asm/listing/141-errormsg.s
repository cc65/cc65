.macro  mac1
        .delmac mac2
.endmacro
.macro  mac2
        mac1
.endmacro
mac2
