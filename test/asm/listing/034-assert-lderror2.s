; 2022-06-18 Spiro Trikaliotis

        .ASSERT * = $0000, lderror, "Code not at $0000"
        .ASSERT * = $0001, lderror, "Code not at $0001"
        .ASSERT * = $1000, lderror, "Code not at $1000"
        .ASSERT * = $1001, lderror, "Code not at $1001"
        .ASSERT * = $8000, lderror, "Code not at $8000"
        .ASSERT * = $8001, lderror, "Code not at $8001"
