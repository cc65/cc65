; Test new-style (@:) and legacy-style (:) unnamed labels.
; Make sure that they have identical behavior.

.ORG $0000

@:  nop
:   nop
.ASSERT @<< = $0000, error
.ASSERT @-- = $0000, error
.ASSERT :<< = $0000, error
.ASSERT :-- = $0000, error
.ASSERT @< = $0001, error
.ASSERT @- = $0001, error
.ASSERT :< = $0001, error
.ASSERT :- = $0001, error
.ASSERT @> = $0002, error
.ASSERT @+ = $0002, error
.ASSERT :> = $0002, error
.ASSERT :+ = $0002, error
.ASSERT @>> = $0003, error
.ASSERT @++ = $0003, error
.ASSERT :>> = $0003, error
.ASSERT :++ = $0003, error
@:  nop
:   nop
