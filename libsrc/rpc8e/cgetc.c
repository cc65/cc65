#include <rpc8e.h>


char cgetc (void) {
	char tempBuffer;
	while (UINT8_KEY_CURCHAR_INDEX == UINT8_KEY_NEWCHAR_POS_INDEX) {} //Stall
	tempBuffer = UINT8_KEY_READ_REG;
	UINT8_KEY_CURCHAR_INDEX++;
	return tempBuffer;
}