#!/bin/bash
ca65 --cpu 6502 hello.asm -o hello.obj -l hello.lst
cl65 -t c64 -C c64-asm.cfg -u __EXEHDR__ -Ln hello.lbl hello.obj -o hello.prg
