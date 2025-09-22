        .scope  foo
                .scope  outer
                        .scope  inner
                                bar = 1
                        .endscope
                .endscope
                .scope  another
                        .scope  nested
                                lda     #::outer::inner::bar    ; 2
                        .endscope
                .endscope
        .endscope

        .scope  outer
                .scope  inner
                        bar = 2
                .endscope
        .endscope
