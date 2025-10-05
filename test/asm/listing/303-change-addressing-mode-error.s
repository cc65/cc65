        .scope  outer
                foo     = $12
                .scope  inner
                        lda     foo,x
                        foo     = $1234
                .endscope
        .endscope
