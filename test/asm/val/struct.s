; test .struct and .union features

.code

; exit with 0
.export _main
_main:
    lda #0
    tax
    rts

; test storage allocator sizes and offsets

.struct Storage
    mb1 .byte
    mb5 .byte 5
    mr1 .res 1
    mr5 .res 5
    mdb1 .dbyt
    mdb5 .dbyt 5
    mw1 .word
    mw5 .word 5
    ma1 .addr
    ma5 .addr 5
    mf1 .faraddr
    mf5 .faraddr 5
    mdw1 .dword
    mdw5 .dword 5
.endstruct

.assert .sizeof(Storage::mb1)  =  1, error, ".struct .byte member has unexpected .sizeof"
.assert .sizeof(Storage::mb5)  =  5, error, ".struct .byte 5 member has unexpected .sizeof"
.assert .sizeof(Storage::mr1)  =  1, error, ".struct .res 1 member has unexpected .sizeof"
.assert .sizeof(Storage::mr5)  =  5, error, ".struct .res 5 member has unexpected .sizeof"
.assert .sizeof(Storage::mdb1) =  2, error, ".struct .dbyt member has unexpected .sizeof"
.assert .sizeof(Storage::mdb5) = 10, error, ".struct .dbyt 5 member has unexpected .sizeof"
.assert .sizeof(Storage::mw1)  =  2, error, ".struct .word member has unexpected .sizeof"
.assert .sizeof(Storage::mw5)  = 10, error, ".struct .word 5 member has unexpected .sizeof"
.assert .sizeof(Storage::ma1)  =  2, error, ".struct .addr member has unexpected .sizeof"
.assert .sizeof(Storage::ma5)  = 10, error, ".struct .addr 5 member has unexpected .sizeof"
.assert .sizeof(Storage::mf1)  =  3, error, ".struct .faraddr member has unexpected .sizeof"
.assert .sizeof(Storage::mf5)  = 15, error, ".struct .faraddr 5 member has unexpected .sizeof"
.assert .sizeof(Storage::mdw1) =  4, error, ".struct .dword member has unexpected .sizeof"
.assert .sizeof(Storage::mdw5) = 20, error, ".struct .dword 5 member has unexpected .sizeof"

.assert Storage::mb1  = 0, error, ".struct storage offset is incorrect"
.assert Storage::mb5  = Storage::mb1  + .sizeof(Storage::mb1),  error, ".struct storage offset is incorrect"
.assert Storage::mr1  = Storage::mb5  + .sizeof(Storage::mb5),  error, ".struct storage offset is incorrect"
.assert Storage::mr5  = Storage::mr1  + .sizeof(Storage::mr1),  error, ".struct storage offset is incorrect"
.assert Storage::mdb1 = Storage::mr5  + .sizeof(Storage::mr5),  error, ".struct storage offset is incorrect"
.assert Storage::mdb5 = Storage::mdb1 + .sizeof(Storage::mdb1), error, ".struct storage offset is incorrect"
.assert Storage::mw1  = Storage::mdb5 + .sizeof(Storage::mdb5), error, ".struct storage offset is incorrect"
.assert Storage::mw5  = Storage::mw1  + .sizeof(Storage::mw1),  error, ".struct storage offset is incorrect"
.assert Storage::ma1  = Storage::mw5  + .sizeof(Storage::mw5),  error, ".struct storage offset is incorrect"
.assert Storage::ma5  = Storage::ma1  + .sizeof(Storage::ma1),  error, ".struct storage offset is incorrect"
.assert Storage::mf1  = Storage::ma5  + .sizeof(Storage::ma5),  error, ".struct storage offset is incorrect"
.assert Storage::mf5  = Storage::mf1  + .sizeof(Storage::mf1),  error, ".struct storage offset is incorrect"
.assert Storage::mdw1 = Storage::mf5  + .sizeof(Storage::mf5),  error, ".struct storage offset is incorrect"
.assert Storage::mdw5 = Storage::mdw1 + .sizeof(Storage::mdw1), error, ".struct storage offset is incorrect"
.assert .sizeof(Storage) = Storage::mdw5 + .sizeof(Storage::mdw5), error, ".struct has unexpected .sizeof"

; test union offset and size

.union UStorage
    mb1 .byte
    mb5 .byte 5
    mr1 .res 1
    mr5 .res 5
    mdb1 .dbyt
    mdb5 .dbyt 5
    mw1 .word
    mw5 .word 5
    ma1 .addr
    ma5 .addr 5
    mf1 .faraddr
    mf5 .faraddr 5
    mdw1 .dword
    mdw5 .dword 5
.endunion

.assert .sizeof(UStorage::mb1)  =  1, error, ".union .byte member has unexpected .sizeof"
.assert .sizeof(UStorage::mb5)  =  5, error, ".union .byte 5 member has unexpected .sizeof"
.assert .sizeof(UStorage::mr1)  =  1, error, ".union .res 1 member has unexpected .sizeof"
.assert .sizeof(UStorage::mr5)  =  5, error, ".union .res 5 member has unexpected .sizeof"
.assert .sizeof(UStorage::mdb1) =  2, error, ".union .dbyt member has unexpected .sizeof"
.assert .sizeof(UStorage::mdb5) = 10, error, ".union .dbyt 5 member has unexpected .sizeof"
.assert .sizeof(UStorage::mw1)  =  2, error, ".union .word member has unexpected .sizeof"
.assert .sizeof(UStorage::mw5)  = 10, error, ".union .word 5 member has unexpected .sizeof"
.assert .sizeof(UStorage::ma1)  =  2, error, ".union .addr member has unexpected .sizeof"
.assert .sizeof(UStorage::ma5)  = 10, error, ".union .addr 5 member has unexpected .sizeof"
.assert .sizeof(UStorage::mf1)  =  3, error, ".union .faraddr member has unexpected .sizeof"
.assert .sizeof(UStorage::mf5)  = 15, error, ".union .faraddr 5 member has unexpected .sizeof"
.assert .sizeof(UStorage::mdw1) =  4, error, ".union .dword member has unexpected .sizeof"
.assert .sizeof(UStorage::mdw5) = 20, error, ".union .dword 5 member has unexpected .sizeof"
.assert .sizeof(UStorage) = 20, error, ".union has unexpected .sizeof"

.assert UStorage::mb1  = 0, error, ".union storage offset is incorrect"
.assert UStorage::mb5  = 0, error, ".union storage offset is incorrect"
.assert UStorage::mr1  = 0, error, ".union storage offset is incorrect"
.assert UStorage::mr5  = 0, error, ".union storage offset is incorrect"
.assert UStorage::mdb1 = 0, error, ".union storage offset is incorrect"
.assert UStorage::mdb5 = 0, error, ".union storage offset is incorrect"
.assert UStorage::mw1  = 0, error, ".union storage offset is incorrect"
.assert UStorage::mw5  = 0, error, ".union storage offset is incorrect"
.assert UStorage::ma1  = 0, error, ".union storage offset is incorrect"
.assert UStorage::ma5  = 0, error, ".union storage offset is incorrect"
.assert UStorage::mf1  = 0, error, ".union storage offset is incorrect"
.assert UStorage::mf5  = 0, error, ".union storage offset is incorrect"
.assert UStorage::mdw1 = 0, error, ".union storage offset is incorrect"
.assert UStorage::mdw5 = 0, error, ".union storage offset is incorrect"

; test tag

storage: .tag Storage
.assert (*-storage)=.sizeof(Storage), error, ".tag reserved size incorrect"

; test nested structures

.struct Point
    xc .word
    yc .word
.endstruct

.struct Nested
    pad .res 13
    tag .tag Point
    ch .struct Child
        ca .word ; offset = 0
        gch .struct Grandchild
            gca .word ; offset = 0
            gcb .byte
        .endstruct
        cb .byte
    .endstruct
    anon .struct
        aa .dword ; offset = Nested::anon (anonymous .struct)
        ab .dword
    .endstruct
    chu .union Chunion
        ua .byte ; offset = 0
        ub .dword
    .endunion
    chanon .union
        uc .byte ; offset = Nested::chanon
        ud .dword
    .endunion
    last .byte
.endstruct

.assert Nested::pad                    = 0, error, "Nested .struct has unexpected starting offset"
.assert Nested::Child::ca              = 0, error, "Nested .struct has unexpected starting offset"
.assert Nested::Child::Grandchild::gca = 0, error, "Nested .struct has unexpected starting offset"

.assert .sizeof(Nested::tag) = .sizeof(Point), error, ".tag in .struct has unexpected .sizeof"
.assert .sizeof(Nested::Child::Grandchild) = 2 + 1, error, "Nested .struct has unexpected .sizeof"
.assert .sizeof(Nested::Child) = 2 + 1 + .sizeof(Nested::Child::Grandchild), error, "Nested .struct has unpexpected .sizeof"
.assert .sizeof(Nested::ch) = .sizeof(Nested::Child), error, "Nested .struct has unexpected member .sizeof"
.assert .sizeof(Nested::Child::gch) = .sizeof(Nested::Child::Grandchild), error, "Nested .struct has unexpected member .sizeof"
.assert .sizeof(Nested::anon) = 8, error, "Nested anonymous member .struct has unexpected .sizeof"
.assert .sizeof(Nested::aa) = 4, error, "Nested anonymous .struct member has unexpected .sizeof"
.assert .sizeof(Nested::ab) = 4, error, "Nested anonymous .struct member has unexpected .sizeof"
.assert .sizeof(Nested::Chunion) = 4, error, "Nested .union has unexpected .sizeof"
.assert .sizeof(Nested::chu) = .sizeof(Nested::Chunion), error, "Nested member .union has unexpected .sizeof"
.assert .sizeof(Nested::chanon) = 4, error, "Nested anonymous member .union as unexpected .sizeof"

.assert Nested::tag    = Nested::pad + .sizeof(Nested::pad),     error, ".tag within .struct has unexpected offset"
.assert Nested::ch     = Nested::tag + .sizeof(Nested::tag),     error,  "Nested .struct has unexpected offset"
.assert Nested::anon   = Nested::ch + .sizeof(Nested::ch),       error, "Nested anonymous member .struct has unexpected offset"
.assert Nested::aa     = Nested::anon,                           error, "Nested anonymous .struct member has unexpected offset"
.assert Nested::ab     = Nested::aa + .sizeof(Nested::aa),       error, "Nested anonymous .struct member has unexpected offset"
.assert Nested::chu    = Nested::ab + .sizeof(Nested::ab),       error, "Nested member .union has unexpected offset"
.assert Nested::chanon = Nested::chu + .sizeof(Nested::Chunion), error, "Nested anonymous member .union has unexpected offset"
.assert Nested::uc     = Nested::chanon,                         error, "Nested anonymous .union member has unexpected offset"
.assert Nested::ud     = Nested::chanon,                         error, "Nested anonymous .union member has unexpected offset"
.assert Nested::last   = Nested::ud + .sizeof(Nested::ud),       error, ".struct member has unexpected offset after anonymous nested .struct"

; test .org

start:

.struct OrgStruct
    ma .byte
    mb .byte
    .org $1234
    mc .byte
    md .byte
    .struct Nested
        me .byte
        .org $5678
        mf .byte
        mg .byte
    .endstruct
    mh .byte
.endstruct

.assert start <> (OrgStruct::mh+1), error, "Fatal test error: accidental code position conflict, move OrgStruct .org to another arbitrary address."
.assert * = start, error, ".org within .struct does not return to previous location at .endstruct"
.assert OrgStruct::ma = 0, error, ".struct with .org has unexpected offset"
.assert OrgStruct::mb = 1, error, ".struct with .org has unexpected offset"
.assert OrgStruct::mc = $1234, error, ".struct with .org has unexpected offset"
.assert OrgStruct::md = $1235, error, ".struct with .org has unexpected offset"
.assert OrgStruct::Nested::me = 0, error, "Nested .struct with .org has unexpected offset"
.assert OrgStruct::Nested::mf = $5678, error, "Nested .struct with .org has unexpected offset"
.assert OrgStruct::Nested::mg = $5679, error, "Nested .struct with .org has unexpected offset"
.assert OrgStruct::mh = $1239, error, ".struct with .org has unexpected offset"
.assert .sizeof(OrgStruct) = 8, error, ".struct with .org has unexpected .sizeof"

.union OrgUnion
    ma .byte
    mb .word
    .org $1234
    mc .byte
    md .word
.endunion

.assert start <> OrgUnion::md, error, "Fatal test error: accidental code position conflict, move OrgUnion .org to another arbitrary address."
.assert * = start, error, ".org within .union does not return to previous location at .endunion"
.assert OrgUnion::ma = 0, error, ".union with .org has unexpected offset"
.assert OrgUnion::mb = 0, error, ".union with .org has unexpected offset"
.assert OrgUnion::mc = $1234, error, ".union with .org has unexpected offset"
.assert OrgUnion::md = $1234, error, ".union with .org has unexpected offset"
.assert .sizeof(OrgUnion) = 2, error, ".union with .org has unexpected .sizeof"
