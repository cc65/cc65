#include <conio.h>
#include <stdio.h>

//TODO check if it's a filestream and not just the std stream
int __fastcall__ fgetc (FILE* f) {
	int returnValue;
	if (f == stdin) {
		returnValue = cgetc();
	}
	return returnValue & 0xFF;
}