; for PR #2013
; should produce error output:
; ... Error: '.ENDMACRO' expected for macro 'test'

.macro test
    nop .endmacro
