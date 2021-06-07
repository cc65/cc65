// --------------------------------------------------------------------------
// Sym-1 front panel display example
//
// Wayne Parham
//
// wayne@parhamdata.com
// --------------------------------------------------------------------------

#include <stdio.h>
#include <sym1.h>

void main (void) {
   int  delay = 10;
   int  flashes = 255;
   int  displayable = 1;
   int  e = 0;
   int  r = 0;  
   int  d = 0;
   int  i = 0;
   int  l = 0;
   int  t = 0;
   int  z = 0;
   char c = 0x00;
   char buffer[41] = { 0x00 };

   puts( "\nType a message (40 chars max) and press ENTER, please:\n" );

   while( (c != '\r') && (i < 41) ) {
      c = getchar();
      putchar( c );
      buffer[i] = c;
      i++;
      if( i == 40 ) {
         puts( "\n\n--- Reached 40 character limit. ---" );
      }
   }

   i--;                                // index is one past end

   while( z == 0 ) {
      puts( "\n\nHow many times (0 for forever) to repeat?" );
      c = getchar();
      putchar( c );
      if( (c >= '0') && (c <= '9') ) { // between 1 and 9 loops allowed
         z = 1;                        // a number was pressed 
         t = c - '0';                  // convert char to int
         puts( "\n\nLook at the front panel.\n" );
      }
      else {
         puts( "\nWhat?" );
         z = 0;                        // keep asking for a number
      }
   }

   z = 0;
   while( (z < t) || (t == 0) ) {

      z++;

      putchar( '\r' );                 // Send CR to console


      set_D0( DISP_SPACE );            // Clear the display
      set_D1( DISP_SPACE );
      set_D2( DISP_SPACE );
      set_D3( DISP_SPACE );
      set_D4( DISP_SPACE );
      set_D5( DISP_SPACE );
      set_D6( DISP_SPACE );

      for( d = 0; d < flashes ; d++ ) {
         fdisp();                      // Display
      }

      for( l = 0; l <= i; l++ ) {

         displayable = 1;              // Assume character is mapped

         switch( buffer[l] ) {         // Put the typed charaters
            case '1':                  // into the display buffer
               set_D6( DISP_1 );       // one at a time
               break;
            case '2':
               set_D6( DISP_2 );
               break;
            case '3':
               set_D6( DISP_3 );
               break;
            case '4':
               set_D6( DISP_4 );
               break;
            case '5':
               set_D6( DISP_5 );
               break;
            case '6':
               set_D6( DISP_6 );
               break;
            case '7':
               set_D6( DISP_7 );
               break;
            case '8':
               set_D6( DISP_8 );
               break;
            case '9':
               set_D6( DISP_9 );
               break;
            case '0':
               set_D6( DISP_0 );
               break;
            case 'A':
               set_D6( DISP_A );
               break;
            case 'a':
               set_D6( DISP_A );
               break;
            case 'B':
               set_D6( DISP_b );
               break;
            case 'b':
               set_D6( DISP_b );
               break;
            case 'C':
               set_D6( DISP_C );
               break;
            case 'c':
               set_D6( DISP_c );
               break;
            case 'D':
               set_D6( DISP_d );
               break;
            case 'd':
               set_D6( DISP_d );
               break;
            case 'E':
               set_D6( DISP_E );
               break;
            case 'e':
               set_D6( DISP_e );
               break;
            case 'F':
               set_D6( DISP_F );
               break;
            case 'f':
               set_D6( DISP_F );
               break;
            case 'G':
               set_D6( DISP_G );
               break;
            case 'g':
               set_D6( DISP_g );
               break;
            case 'H':
               set_D6( DISP_H );
               break;
            case 'h':
               set_D6( DISP_h );
               break;
            case 'I':
               set_D6( DISP_I );
               break;
            case 'i':
               set_D6( DISP_i );
               break;
            case 'J':
               set_D6( DISP_J );
               break;
            case 'j':
               set_D6( DISP_J );
               break;
            case 'K':
               set_D6( DISP_K );
               break;
            case 'k':
               set_D6( DISP_K );
               break;
            case 'L':
               set_D6( DISP_L );
               break;
            case 'l':
               set_D6( DISP_L );
               break;
            case 'M':
               set_D0( get_D1() );
               set_D1( get_D2() );
               set_D2( get_D3() );
               set_D3( get_D4() );
               set_D4( get_D5() );
               set_D5( DISP_M_1 );
	       set_D6( DISP_M_2 );
               break;
            case 'm':
               set_D0( get_D1() );
               set_D1( get_D2() );
               set_D2( get_D3() );
               set_D3( get_D4() );
               set_D4( get_D5() );
               set_D5( DISP_M_1 );
	       set_D6( DISP_M_2 );
               break;
            case 'N':
               set_D6( DISP_n );
               break;
            case 'n':
               set_D6( DISP_n );
               break;
            case 'O':
               set_D6( DISP_O );
               break;
            case 'o':
               set_D6( DISP_o );
               break;
            case 'P':
               set_D6( DISP_P );
               break;
            case 'p':
               set_D6( DISP_P );
               break;
            case 'Q':
               set_D6( DISP_q );
               break;
            case 'q':
               set_D6( DISP_q );
               break;
            case 'R':
               set_D6( DISP_r );
               break;
            case 'r':
               set_D6( DISP_r );
               break;
            case 'S':
               set_D6( DISP_S );
               break;
            case 's':
               set_D6( DISP_S );
               break;
            case 'T':
               set_D6( DISP_t );
               break;
            case 't':
               set_D6( DISP_t );
               break;
            case 'U':
               set_D6( DISP_U );
               break;
            case 'u':
               set_D6( DISP_u );
               break;
            case 'V':
               set_D0( get_D1() );
               set_D1( get_D2() );
               set_D2( get_D3() );
               set_D3( get_D4() );
               set_D4( get_D5() );
               set_D5( DISP_V_1 );
               set_D6( DISP_V_2 );
               break;
            case 'v':
               set_D0( get_D1() );
               set_D1( get_D2() );
               set_D2( get_D3() );
               set_D3( get_D4() );
               set_D4( get_D5() );
               set_D5( DISP_V_1 );
               set_D6( DISP_V_2 );
               break;
            case 'W':
               set_D0( get_D1() );
               set_D1( get_D2() );
               set_D2( get_D3() );
               set_D3( get_D4() );
               set_D4( get_D5() );
               set_D5( DISP_W_1 );
               set_D6( DISP_W_2 );
               break;
            case 'w':
               set_D0( get_D1() );
               set_D1( get_D2() );
               set_D2( get_D3() );
               set_D3( get_D4() );
               set_D4( get_D5() );
               set_D5( DISP_W_1 );
               set_D6( DISP_W_2 );
               break;
            case 'Y':
               set_D6( DISP_Y );
               break;
            case 'y':
               set_D6( DISP_Y );
               break;
            case 'Z':
               set_D6( DISP_Z );
               break;
            case 'z':
               set_D6( DISP_Z );
               break;
            case ' ':
               set_D6( DISP_SPACE );
               break;
            case '.':
               set_D6( DISP_PERIOD );
               break;
            case '-':
               set_D6( DISP_HYPHEN );
               break;
            case '\'':
               set_D6( DISP_APOSTR );
               break;
            case '"':
               set_D6( DISP_APOSTR );
               break;
            case '=':
               set_D6( DISP_EQUAL );
               break;
            case '_':
               set_D6( DISP_BOTTOM );
               break;
            case '/':
               set_D6( DISP_SLASH );
               break;
            case '\\':
               set_D6( DISP_BACKSLASH );
               break;
            default:
               displayable = 0;        // Character not mapped
         }

         if( displayable ) {

            putchar( buffer[l] );      // Send it to the console

            set_D0( get_D1() );        // Scroll to the left
            set_D1( get_D2() );
            set_D2( get_D3() );
            set_D3( get_D4() );
            set_D4( get_D5() );
            set_D5( get_D6() );

            for( d = 0; d < flashes ; d++ ) {
               fdisp();                // Display
            }
         }
      }

      for( e = 0; e < 6; e++ ) {       // Gradually fill the
         set_D0( get_D1() );           // display with spaces
         set_D1( get_D2() );
         set_D2( get_D3() );
         set_D3( get_D4() );
         set_D4( get_D5() );
         set_D5( DISP_SPACE );
         set_D6( DISP_SPACE );
         for( d = 0; d < flashes ; d++ ) {
            fdisp();                   // Display
         }
      }
   }

   puts( "\n\nEnjoy your day!\n\n" );
}
