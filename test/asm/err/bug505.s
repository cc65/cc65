; Test for #505 taken from the issue
; Redefining a variable symbol "reopens" the old name space for cheap locals
; Behavior should be: First definition of a variable symbol opens a new
; scope for cheap locals, redefinitions of the same symbols do not.

;this starts a new scope for cheap local lables
SomeSymbol .set 4

        jmp @CheapLocal1

@CheapLocal0:

        .byte $8b

CheapLocalScopeBreaker0:

CheapLocalScopeBreaker1:

CheapLocalScopeBreaker2:

CheapLocalScopeBreaker3:

;this continues the same cheap scope as before, regardless of the many global labels in between
SomeSymbol .set 5

@CheapLocal1:

        lda @CheapLocal0
