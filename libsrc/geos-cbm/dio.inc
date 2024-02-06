; offsets into sectsizetab (DIO functions)
sst_flag                =       0       ; length 1
sst_sectsize            =       1       ;        2
sst_driveno             =       3       ;        1  (drive #)
sst_size                =       4       ; size of one entry
                                        ; if changed, adapt diopncls.s

; offsets into dio_phys_pos
diopp_head              =       0       ; head
diopp_track             =       1       ; track / cylinder
diopp_sector            =       3       ; sector
diopp_size              =       5       ; size of structure
