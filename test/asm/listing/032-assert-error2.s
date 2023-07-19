; 2022-06-18 Spiro Trikaliotis

        .ASSERT * = $0000, error, "Code not at $0000"
        .ASSERT * = $0001, error, "Code not at $0001"
        .ASSERT * = $1000, error, "Code not at $1000"
        .ASSERT * = $1001, error, "Code not at $1001"
        .ASSERT * = $8000, error, "Code not at $8000"
        .ASSERT * = $8001, error, "Code not at $8001"
