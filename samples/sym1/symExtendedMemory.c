// --------------------------------------------------------------------------
// Sym-1 Extended Memory
//
// Wayne Parham
//
// wayne@parhamdata.com
// --------------------------------------------------------------------------
//
// Note:  This  program examines memory above the monitor ROM (8000-8FFF)  to
//        Determine what, if any, memory is available.  It then adds whatever
//        4K segments it finds to the heap.
//
//        Memory Segment     Remark
//            0x9000         Usually available
//            0xA000         System I/O, always unavailable
//            0xB000         Used by RAE, but normally available
//            0xC000         Used by BASIC, normally unavailable
//            0xD000         Used by BASIC, normally unavailable
//            0xE000         Used by RAE, but normally available
//            0xF000         Normally available, but only to FF7F
//
// --------------------------------------------------------------------------

#include <sym1.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STD_MEM        0x7FFF            // Last address of standard memory
#define SEGMENT        0x9000            // First 4K segment of extended memory
#define SEG_END        0x0FFF            // Last location of segment
#define BLOCK_SIZE     0x1000            // Size of segment
#define TOP_END        0x0F7F            // Last location of memory
#define TOP_SIZE       0x0F80            // Size of top segment
#define UNAVAILABLE    0xA000            // System I/O area

int main (void) {
   int      error     = 0;
   unsigned heap_size = 0x0000;
   char*    segment   = (char*) SEGMENT;

   printf ( "Analyzing memory.\n\n" );

   heap_size = _heapmemavail();

   printf ( "Main memory has %u bytes available.\n", heap_size );

   if ( heap_size > STD_MEM ) {
      printf ( "Extended memory already installed.\n" );
   } else {

      while ( (int) segment < 0xEFFF ) { // Iterate through 4K memory blocks
         if( (int) segment != UNAVAILABLE ) {
            segment[0] = 0x00;           // Check beginning of segment
            if ( segment[0] != 0x00 )
               error = 1;
            segment[0] = 0xFF;
            if ( segment[0] != 0xFF )
               error = 1;
            segment[SEG_END] = 0x00;     // Check end of segment
            if ( segment[SEG_END] != 0x00 )
               error = 1;
            segment[SEG_END] = 0xFF;
            if ( segment[SEG_END] != 0xFF )
               error = 1;
            if ( ! error ) {             // If memory found, add to the heap
               printf ( "Memory found at location %p, ", segment );
               _heapadd ( segment, BLOCK_SIZE );
               heap_size = _heapmemavail();
               printf( "so the system now has %u bytes available.\n", heap_size );
            } else {
               error = 0;
            }
         }
         segment += 0x1000;              // Increment to next segment
      }

      segment[0] = 0x00;                 // Check beginning of top memory segment
      if ( segment[0] != 0x00 )
         error = 1;
      segment[0] = 0xFF;
      if ( segment[0] != 0xFF )
         error = 1;
      segment[TOP_END] = 0x00;           // Check end of usable memory
      if ( segment[TOP_END] != 0x00 )
         error = 1;
      segment[TOP_END] = 0xFF;
      if ( segment[TOP_END] != 0xFF )
         error = 1;
      if ( ! error ) {                   // If memory found, add to the heap
         printf ( "Memory found at location %p, ", segment );
         _heapadd ( segment, TOP_SIZE );
         heap_size = _heapmemavail();
         printf( "so the system now has %u bytes available.\n", heap_size );
      }
   }

   puts ("\nEnjoy your day!\n");

   return 0;
}
