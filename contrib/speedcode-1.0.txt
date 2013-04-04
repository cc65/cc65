Speedcode generator V1.0
------------------------

Compiled with CC65 V2.12.9
Targets: The speedcode generator should run on any system with an ansi 
				 c compiler, but has only been tested on C64 emulator.
				 The example code only runs on a C64.

I got very tired writing the speedcode generation routines
again for every demo part i made.
So i decided to write a routine, which takes the adress of
a code snippet, the length of the snippet and 
a pointer to a modification function for the code snippet 
as parameters for automatic speedcode generation.
This is the result i want to share with you.

The application of the routine is very simple. 
First take a look into the header file,
then follow the instructions below or use and expand the testcode. 

Following example sets up code for scrolling the first textline of the screen:

-Write a code snippet: 
  
 export _label1,_label2
 _label1=*+1
  LDA SCREENRAM+1
 _label2=*+1 
  STA SCREENRAM

-Write a modification routine:
 
	bool modspeedcode(unsigned int reppos){
		extern unsigned int label1,label2;
		label2=label1;
		++label1;
		return (reppos<40)? true:false;
	}
 
-Prepare a struct with adress of code snippet:
 extern void* label1;
 SPEEDCODE_SU spdcdef={label1,6};
 
-Reserve memory for the speedcode:
 spdcadr = malloc(SNIPPETSIZE*REPEATS);
 
-Call create_speedcode:
 create_speedcode(&spdcdef,spdcadr,&modspeedcode);
 
It is also possible to recursively call create_speedcode via the 
modification routine. This way, extra code can be added at the 
end of a line for calculating values which are valid for the whole
new line. Imagine, you want to do a plasma effect. The sin(y) value
has to be calculated only once a row. If the effect is 40 chars wide,
every 40th copy of the speedcode, extra code must be added to calculate sin(y).
The subroutine create_speedcode remembers the current destination adress
for the snippet. If you deliver a NULL pointer to the parameter dest 
of that function, the new snippet will be added 
at the current destination adress. This was necessary to enable 
the possibility for recursive calls.
The parameter func must never be a NULL pointer, else the assertion will fail.
If you don't want to call any modification code, just point func to an RTS.

For suggestions or bug reports:

montecarlos@gmx.net

27.7.2009
