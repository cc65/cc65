        .scope  outer
                foo     = $12
                .scope  inner
                        lda     a:foo,x
                        foo     = $1234
                .endscope
        .endscope
