// --------------------------------------------------------------------------
// Sym-1 digital I/O interface example
//
// Wayne Parham
//
// wayne@parhamdata.com
// --------------------------------------------------------------------------

#include <sym1.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (void) {
   unsigned char ddr1a = 0x00;
   unsigned char ior1a = 0x00;
   unsigned char ddr1b = 0x00;
   unsigned char ior1b = 0x00;
   unsigned char ddr2a = 0x00;
   unsigned char ior2a = 0x00;
   unsigned char ddr2b = 0x00;
   unsigned char ior2b = 0x00;
   unsigned char ddr3a = 0x00;
   unsigned char ior3a = 0x00;
   unsigned char ddr3b = 0x00;
   unsigned char ior3b = 0x00;
   unsigned char   val = 0x00;
   int going           = 0x01;
   int instr           = 0x01;
   int l               = 0x00;
   char* vp            = 0x00;
   char cmd[20]        = { 0x00 };

   while ( going ) {

      putchar ( '\r' );
      for ( l = 0; l < 25; l++ ) {
         putchar ( '\n' );
      }

      ddr1a = VIA1.ddra;
      ior1a = VIA1.pra;
      ddr1b = VIA1.ddrb;
      ior1b = VIA1.prb;
      ddr2a = VIA2.ddra;
      ior2a = VIA2.pra;
      ddr2b = VIA2.ddrb;
      ior2b = VIA2.prb;
      ddr3a = VIA3.ddra;
      ior3a = VIA3.pra;
      ddr3b = VIA3.ddrb;
      ior3b = VIA3.prb;

      puts ("================== Digital I/O Status ==================");
      puts ("     Port1A   Port1B   Port2A   Port2B   Port3A   Port3B" );
      printf ("DDR    %02X       %02X       %02X       %02X       %02X       %02X\n\r",ddr1a,ddr1b,ddr2a,ddr2b,ddr3a,ddr3b);
      printf ("IOR    %02X       %02X       %02X       %02X       %02X       %02X\n\r",ior1a,ior1b,ior2a,ior2b,ior3a,ior3b);
      puts ("========================================================\n");

      if ( instr ) {
         puts ("You  can set any register by typing 'register value'  so");
         puts ("as  an example, to set register IOR2A with the top  five");
         puts ("bits  off  and  the bottom three on,  type  'IOR2A  07'.");
         puts ("Press  ENTER without any command to see register  values");
         puts ("without changing any of them.   Type 'help' to see these");
         puts ("instructions again and type 'quit' to end the program.\n");
         puts ("Available registers:   DDR1A, IOR1A, DDR1B, IOR1B, DDR2A");
         puts ("IOR2A, DDR2B, IOR2B, DDR3A, IOR3A, DDR3B and IOR3B.");
         instr = 0;
      }

      printf ("\n Command:  ");

      fgets ( cmd, sizeof(cmd)-1, stdin );
      cmd[strlen(cmd)-1] = '\0';

      if ( strncasecmp(cmd, "quit", 4) == 0 ) {
         going = 0;
      }
      else if ( strncasecmp(cmd, "help", 4) == 0 ) {
         instr = 1;
      }
      else if ( strncasecmp(cmd, "ddr1a", 5) == 0 ) {
         vp = strchr ( cmd, ' ' );
         if ( vp ) {
            val = (unsigned char) strtol( vp, NULL, 0 );
            VIA1.ddra = val;
         }
      }
      else if ( strncasecmp(cmd, "ior1a", 5) == 0 ) {
         vp = strchr ( cmd, ' ' );
         if ( vp ) {
            val = (unsigned char) strtol( vp, NULL, 0 );
            VIA1.pra = val;
         }
      }
      else if ( strncasecmp(cmd, "ddr1b", 5) == 0 ) {
         vp = strchr ( cmd, ' ' );
         if ( vp ) {
            val = (unsigned char) strtol( vp, NULL, 0 );
            VIA1.ddrb = val;
         }
      }
      else if ( strncasecmp(cmd, "ior1b", 5) == 0 ) {
         vp = strchr ( cmd, ' ' );
         if ( vp ) {
            val = (unsigned char) strtol( vp, NULL, 0 );
            VIA1.prb = val;
         }
      }
      else if ( strncasecmp(cmd, "ddr2a", 5) == 0 ) {
         vp = strchr ( cmd, ' ' );
         if ( vp ) {
            val = (unsigned char) strtol( vp, NULL, 0 );
            VIA2.ddra = val;
         }
      }
      else if ( strncasecmp(cmd, "ior2a", 5) == 0 ) {
         vp = strchr ( cmd, ' ' );
         if ( vp ) {
            val = (unsigned char) strtol( vp, NULL, 0 );
            VIA2.pra = val;
         }
      }
      else if ( strncasecmp(cmd, "ddr2b", 5) == 0 ) {
         vp = strchr ( cmd, ' ' );
         if ( vp ) {
            val = (unsigned char) strtol( vp, NULL, 0 );
            VIA2.ddrb = val;
         }
      }
      else if ( strncasecmp(cmd, "ior2b", 5) == 0 ) {
         vp = strchr ( cmd, ' ' );
         if ( vp ) {
            val = (unsigned char) strtol( vp, NULL, 0 );
            VIA2.prb = val;
         }
      }
      else if ( strncasecmp(cmd, "ddr3a", 5) == 0 ) {
         vp = strchr ( cmd, ' ' );
         if ( vp ) {
            val = (unsigned char) strtol( vp, NULL, 0 );
            VIA3.ddra = val;
         }
      }
      else if ( strncasecmp(cmd, "ior3a", 5) == 0 ) {
         vp = strchr ( cmd, ' ' );
         if ( vp ) {
            val = (unsigned char) strtol( vp, NULL, 0 );
            VIA3.pra = val;
         }
      }
      else if ( strncasecmp(cmd, "ddr3b", 5) == 0 ) {
         vp = strchr ( cmd, ' ' );
         if ( vp ) {
            val = (unsigned char) strtol( vp, NULL, 0 );
            VIA3.ddrb = val;
         }
      }
      else if ( strncasecmp(cmd, "ior3b", 5) == 0 ) {
         vp = strchr (cmd, ' ' );
         if ( vp ) {
            val = (unsigned char) strtol( vp, NULL, 0 );
            VIA3.prb = val;
         }
      }
   }

   puts ("\n\nEnjoy your day!\n\n");

   return 0;
}
