/*
 * Input routines for OSI C1P platform.
 */

#include <conio.h>
#include <ctype.h>
#include <peekpoke.h>

// Address of hardware keyboard register.
#define KEYBOARD 0xdf00

// Some special keys.
#define ESC 0x1b
#define CR 0x0d
#define LF 0x0a

// Lookup table of keys, given keyboard row and bit numbers.
const unsigned char keys[8][8] = {
    { 0,   0,   0,    0,    0,  ESC,   0,  0  }, // Row 0
    { 0, 'p', ';',  '/',  ' ',  'z', 'a', 'q' }, // Row 1
    { 0, ',', 'm',  'n',  'b',  'v', 'c', 'x' }, // Row 2
    { 0, 'k', 'j',  'h',  'g',  'f', 'd', 's' }, // Row 3
    { 0, 'i', 'u',  'y',  't',  'r', 'e', 'w' }, // Row 4
    { 0,   0,   0,   CR,   LF,  'o', 'l', '.' }, // Row 5
    { 0,   0, '_',  '-',  ':',  '0', '9', '8' }, // Row 6
    { 0, '7', '6',  '5',  '4',  '3', '2', '1' }, // Row 7
};

// Table of bitmasks for active keys in each row.
const unsigned char columnMask[8] = {
    0x20, // Row 0, only ESC key
    0xfe, // Row 1
    0xfe, // Row 2
    0xfe, // Row 3
    0xfe, // Row 4
    0xf8, // Row 5
    0xfc, // Row 6
    0xfe, // Row 7
};

// Bit masks for modifier keys.
const unsigned char controlMask = 0x40; // Control
const unsigned char shiftMask = 0x06;   // Left Shift, Right Shift
const unsigned char lockMask = 0x01;    // Shift Lock

// Holds last key pressed.
unsigned char lastKey = 0;

/*
 * Write to keyboard to select row.
 * Shift a 1 based on row number.
 * Invert the bits.
 * Write to keyboard register.
 */
static void writeRow(unsigned char rowNumber)
{
    unsigned char d = ~(1 << rowNumber);
    POKE(KEYBOARD, d);
}

/*
 * Get key from keyboard without blocking.
 * Returns 0 if no key was pressed.
 */
static unsigned char getkey(void)
{
    unsigned char row, data;
    signed char bit;
    unsigned char key;
    unsigned char control = 0; // Control key in effect.
    unsigned char shift   = 0; // Shift key in effect.
    unsigned char lock    = 0; // Shift lock key in effect.

    for (row = 0; row <= 7; row++) { // Scan each row.

        writeRow(row); // Select row
        data = ~PEEK(KEYBOARD); // Read column data for row.

        // Handle modifier keys on row 0.
        if (row == 0) {
            if (data & controlMask) { // Control key pressed.
                control = 1;
            }
            if (data & shiftMask) { // Left Shift or Right Shift pressed.
                shift = 1;
            }
            if (data & lockMask) { // Shift Lock pressed.
                lock = 1;
            }
        }

        data &= columnMask[row]; // Mask out only the valid bits for the row.
 
        if (data != 0) {
            // Find bit for first pressed key.
            for (bit = 7; bit >= 0; bit--) {
                if (data & (1 << bit)) {
                    break;
                }
            }

            key = keys[row][bit]; // Get key character.

            if (key == 0) { // Check for invalid key.
                continue; // Shouldn't be possible, but just in case (user may have modified hardware).
            }

            // Handle possible control key. Takes precedence over shift.
            if (control) {
                key &= 0x1f; // Convert to control code.
            } else {
                // Handle possible caps lock key.
                if (lock) {
                    key = toupper(key); // Convert to upper case.
                }
                // Handle possible shift key.
                if (shift) {
                    if (key >= 'a' && key <= 'z') {
                        key -= 32; // Lower to upper case.
                    } else if (key >= '1' && key <= ';') {
                        key -= 16; // Number keys and punctuation.
                    } else if (key >= ',' && key <= '/') {
                        key += 16; // Punctuation.
                    } else if (key >= 'K' && key <= 'O') {
                        key += 16; // KLMNO become [\]^_
                    } else if (key == 'P') {
                        key = '@'; // P becomes @
                    }
                }
            }

            // Return the key.
            return key;
        }
    }

    return 0; // No key pressed.
}

/*
 * Return true (1) if key pressed, otherwise false (0). Saves
 * character so that it is guaranteed that if kbhit() returns true, a
 * subsequent call to cgetc() will not block.
 */
unsigned char kbhit (void)
{
    lastKey = getkey();    // Get possible key press.
    return (lastKey != 0); // Return whether key was pressed.
}

/*
 * Return key. Blocks until key pressed. Uses key from a previous call
 * to kbhit(), if any.
 */
char cgetc (void)
{
    unsigned char key;
    
    if (lastKey != 0) { // Did we previously get a key?
        key = lastKey; // If so, use it.
        lastKey = 0; // Then clear it.
    } else {
        while ((key = getkey()) != 0)
            ; // Wait for key to be pressed.
    }
    return key;
}
