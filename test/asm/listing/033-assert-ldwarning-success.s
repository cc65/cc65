; 2022-06-15 Spiro Trikaliotis

        .ASSERT * = $0000, ldwarning, "Code not at $0000"
        .assert * = $0000, ldwarning, "Code not at $0000"

        .ASSERT * = $0001, ldwarning, "Code not at $0001"
        .assert * = $0001, ldwarning, "Code not at $0001"

        .ASSERT * = $1000, ldwarning, "Code not at $1000"
        .assert * = $1000, ldwarning, "Code not at $1000"

        .ASSERT * = $1001, ldwarning, "Code not at $1001"
        .assert * = $1001, ldwarning, "Code not at $1001"

        .ASSERT * = $8000, ldwarning, "Code not at $8000"
        .assert * = $8000, ldwarning, "Code not at $8000"

        .ASSERT * = $8001, ldwarning, "Code not at $8001"
        .assert * = $8001, ldwarning, "Code not at $8001"

        nop

        .ASSERT * = $0000, ldwarning, "Code not at $0000"
        .assert * = $0000, ldwarning, "Code not at $0000"

        .ASSERT * = $0001, ldwarning, "Code not at $0001"
        .assert * = $0001, ldwarning, "Code not at $0001"

        .ASSERT * = $1000, ldwarning, "Code not at $1000"
        .assert * = $1000, ldwarning, "Code not at $1000"

        .ASSERT * = $1001, ldwarning, "Code not at $1001"
        .assert * = $1001, ldwarning, "Code not at $1001"

        .ASSERT * = $8000, ldwarning, "Code not at $8000"
        .assert * = $8000, ldwarning, "Code not at $8000"

        .ASSERT * = $8001, ldwarning, "Code not at $8001"
        .assert * = $8001, ldwarning, "Code not at $8001"



        .org $8000

        .ASSERT * = $0000, ldwarning, "Code not at $0000"
        .assert * = $0000, ldwarning, "Code not at $0000"

        .ASSERT * = $0001, ldwarning, "Code not at $0001"
        .assert * = $0001, ldwarning, "Code not at $0001"

        .ASSERT * = $1000, ldwarning, "Code not at $1000"
        .assert * = $1000, ldwarning, "Code not at $1000"

        .ASSERT * = $1001, ldwarning, "Code not at $1001"
        .assert * = $1001, ldwarning, "Code not at $1001"

        .ASSERT * = $8000, ldwarning, "Code not at $8000"
        .assert * = $8000, ldwarning, "Code not at $8000"

        .ASSERT * = $8001, ldwarning, "Code not at $8001"
        .assert * = $8001, ldwarning, "Code not at $8001"

        nop

        .ASSERT * = $0000, ldwarning, "Code not at $0000"
        .assert * = $0000, ldwarning, "Code not at $0000"

        .ASSERT * = $0001, ldwarning, "Code not at $0001"
        .assert * = $0001, ldwarning, "Code not at $0001"

        .ASSERT * = $1000, ldwarning, "Code not at $1000"
        .assert * = $1000, ldwarning, "Code not at $1000"

        .ASSERT * = $1001, ldwarning, "Code not at $1001"
        .assert * = $1001, ldwarning, "Code not at $1001"

        .ASSERT * = $8000, ldwarning, "Code not at $8000"
        .assert * = $8000, ldwarning, "Code not at $8000"

        .ASSERT * = $8001, ldwarning, "Code not at $8001"
        .assert * = $8001, ldwarning, "Code not at $8001"
