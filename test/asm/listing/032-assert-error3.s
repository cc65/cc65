; 2022-06-18 Spiro Trikaliotis

        .assert * = $0000, error, "Code not at $0000"
        .assert * = $0001, error, "Code not at $0001"
        .assert * = $1000, error, "Code not at $1000"
        .assert * = $1001, error, "Code not at $1001"
        .assert * = $8000, error, "Code not at $8000"
        .assert * = $8001, error, "Code not at $8001"
