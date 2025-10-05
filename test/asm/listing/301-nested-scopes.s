        .scope  outer
                foo     = 2
                .scope  inner
                        lda     #foo
                        foo     = 3
                .endscope
        .endscope
