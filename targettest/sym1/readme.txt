These  simple build scripts can be used to build any single-file  C  program
you  might write.  Notice the diference in the linker line for the 4k  build
compared  with the 32k build.   Small programs can be compiled  with  either
build  script,  but  they won't run on a 4k machine if compiled  for  a  32k
system.  So  if you have a program that's small enough to fit in 4k,  it  is
probably better to build with the 4k script so it will run on Sym-1 machines
that do not have an expansion memory board.

Usage: build <program>   (don't include the .c extension)
       clean <program>   (removes intermediate and output files)

