// --------------------------------------------------------------------------
// Sym-1 Notepad
//
// Wayne Parham
//
// wayne@parhamdata.com
// --------------------------------------------------------------------------
//
// Note:  This program requires RAM memory in locations 0xE000 - 0xEFFF
//        Alternatively,  the tape I/O buffer location and size can  be
//        changed by altering the defined TAPIO values below.
//
// --------------------------------------------------------------------------

#include <sym1.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAPIO_ADDRESS  0xE000
#define TAPIO_MAX_SIZE 0x0FFF

int main (void) {
   char c                 = 0x00;
   int l                  = 0x00;
   int p                  = 0x00;
   int error              = 0x00;
   int running            = 0x01;
   int writing            = 0x01;
   int instruction_needed = 0x01;
   int heap_size          = 0x00;
   char* tapio  = (char*) TAPIO_ADDRESS;
   char* buffer;

   heap_size = _heapmaxavail();

   if ( heap_size > TAPIO_MAX_SIZE ) { // No need to malloc more than
      heap_size = TAPIO_MAX_SIZE;      // the interface allows
   }

   buffer = malloc ( heap_size );
   memset ( buffer, 0x00, heap_size );

   if ( buffer == 0x00 ) {
      puts ("Memory full.");
      running = 0;
   }

   tapio[0] = 0x00;                    // Check tape interface memory
   if ( tapio[0] != 0x00 )
      error = 1;

   tapio[0] = 0xFF;
   if ( tapio[0] != 0xFF )
      error = 1;

   tapio[TAPIO_MAX_SIZE] = 0x00;
   if ( tapio[TAPIO_MAX_SIZE] != 0x00 )
      error = 1;

   tapio[TAPIO_MAX_SIZE] = 0xFF;
   if ( tapio[TAPIO_MAX_SIZE] != 0xFF )
      error = 1;

   if ( error ) {
      printf ("\nNo memory at location %p, aborting.\n", tapio);
      running = 0;
   }
   else {
      memset ( tapio, 0, TAPIO_MAX_SIZE );
   }


   while ( running ) {

      putchar ( '\r' );
      for ( l = 0; l < 25; l++ ) {
         putchar ( '\n' );
      }

      puts ("===================== Sym-1 Notepad ====================");

      if ( instruction_needed ) {
         puts ("Enter  text  and you can save it to tape  for  reloading");
         puts ("later.  There are four special 'command' characters:\n");
         puts ("   Control-S   Save to tape");
         puts ("   Control-L   Load from tape");
         puts ("   Control-C   Clear memory");
         puts ("   Control-X   Exit");
         puts ("========================================================\n");
      }

      while ( writing ) {

         c = getchar();

         if ( c == 0x08 ) {            // Backspace
            if ( p > 0 ) {
               buffer[p] = 0x00;
               p--;
            }
         }
         else if ( c == 0x13 ) {       // Save
            puts ("\n========================= Save =========================");
            puts ("\nPress any key to save.");
            c = getchar();
            for ( l = 0; l <= p; l++ ) {
               tapio[l] = buffer[l];
            }
            l++;
            tapio[l] = 0x00;
            puts ("Saving to tape.");
            error = dumpt ( 'N', tapio, tapio+p );
            if ( error ) {
               puts ("\nTape error.");
            }
            else
            {
               putchar ( '\r' );
               for ( l = 0; l < 25; l++ ) {
                  putchar ( '\n' );
               }
            }
            puts ("===================== Sym-1 Notepad ====================\n");
            for ( l = 0; l <= p; l++ ) {
               putchar ( buffer[l] );
            }
         }
         else if ( c == 0x0C ) {       // Load
            p = 0;
            puts ("\nLoading from tape.");
            memset ( buffer, 0, heap_size );
            memset ( tapio, 0, TAPIO_MAX_SIZE );
            error = loadt ( 'N' );
            if ( error ) {
               puts ("\nTape error.");
               puts ("===================== Sym-1 Notepad ====================\n");
            }
            else
            {
                for ( l = 0; l <= heap_size; l++ ) {
                    buffer[l] = tapio[l];
                }

                p = strlen ( buffer );

                putchar ( '\r' );
                for ( l = 0; l < 25; l++ ) {
                    putchar ( '\n' );
                }
                puts ("===================== Sym-1 Notepad ====================\n");

                for ( l = 0; l <= p; l++ ) {
                    putchar ( buffer[l] );
                }
            }
         }
         else if ( c == 0x03 ) {       // Clear
            p = 0;
            memset ( buffer, 0, heap_size );
            putchar ( '\r' );
            for ( l = 0; l < 25; l++ ) {
               putchar ( '\n' );
            }
            puts ("===================== Sym-1 Notepad ====================\n");
         }
         else if ( c == 0x18 ) {       // Exit
            writing = 0;
            running = 0;
         }
         else {
            if ( p >= heap_size - 1 ) {
               puts ("\n========================= End  =========================");
               puts ("Buffer full.");
            }
            else {
               if ( c == '\n' ) {
                  putchar ( '\n' );
               }
               buffer[p] = c;
            }
            p++;
         }
      }
   }

   free ( buffer );

   puts ("\nEnjoy your day!\n");

   return 0;
}
