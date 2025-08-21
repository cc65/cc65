        .scope  foo
                bar     = 3
        .endscope

        .scope  outer
                lda     #foo::bar  ; Will load 3, not 2!
                .scope  foo
                        bar     = 2
                .endscope
        .endscope
