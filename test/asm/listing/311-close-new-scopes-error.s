;;; Regression test to ensure that newly inferred scopes are closed.

.scope foo
start:
        ;; Since `bar` is not a known scope at this point, it is
        ;; inferred to be `::foo::bar`, and since `::foo::bar::start`
        ;; does not exist this will produce an error. Importantly,
        ;; it does not resolve to `::foo::start` which would be
        ;; incorrect.
        jmp bar::start
.endscope

.scope bar
start:
        rts
.endscope
