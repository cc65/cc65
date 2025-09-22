        .scope  outer
                foo     = $1234
                .scope  inner
                        lda     foo,x
                        foo     = $12
                .endscope
        .endscope
