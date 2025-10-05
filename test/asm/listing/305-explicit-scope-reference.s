        bar     = 3

        .scope  foo
                bar     = 2
                lda     #::bar  ; Access the global bar (which is 3)
        .endscope
