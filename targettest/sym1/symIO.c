// --------------------------------------------------------------------------
// Sym-1 digital I/O interface example
//
// Wayne Parham
//
// wayne@parhamdata.com
// --------------------------------------------------------------------------

#include <stdio.h>;
#include <symio.h>;
#include <stdlib.h>;
#include <string.h>;

void main(void) {
   int ddr1a    =   0x00;
   int ior1a    =   0x00;
   int ddr1b    =   0x00;
   int ior1b    =   0x00;
   int ddr2a    =   0x00;
   int ior2a    =   0x00;
   int ddr2b    =   0x00;
   int ior2b    =   0x00;
   int ddr3a    =   0x00;
   int ior3a    =   0x00;
   int ddr3b    =   0x00;
   int ior3b    =   0x00;
   int     l    =   0x00;
   int   val    =   0x00;
   int going    =   0x01;
   int instr    =   0x01;
   char* vp     =   0x00;
   char cmd[20] =  { 0x00 };

   while( going ) {

      putchar( '\r' );
      for( l = 0; l < 25; l++ ) {
         putchar( '\n' );
      }

      ddr1a = get_DDR1A();
      ior1a = get_IOR1A();
      ddr1b = get_DDR1B();
      ior1b = get_IOR1B();
      ddr2a = get_DDR2A();
      ior2a = get_IOR2A();
      ddr2b = get_DDR2B();
      ior2b = get_IOR2B();
      ddr3a = get_DDR3A();
      ior3a = get_IOR3A();
      ddr3b = get_DDR3B();
      ior3b = get_IOR3B();

      puts( "================== Digital I/O Status ==================" );
      puts( "     Port1A   Port1B   Port2A   Port2B   Port3A   Port3B" );    
      printf( "DDR    %02X       %02X       %02X       %02X       %02X       %02X\n\r",ddr1a,ddr1b,ddr2a,ddr2b,ddr3a,ddr3b );
      printf( "IOR    %02X       %02X       %02X       %02X       %02X       %02X\n\r",ior1a,ior1b,ior2a,ior2b,ior3a,ior3b );
      puts( "========================================================\n" );

      if( instr ) {
         puts( "You  can set any register by typing 'register value'  so" );
         puts( "as  an example, to set register IOR2A with the top  five" );
         puts( "bits  off  and  the bottom three on,  type  'IOR2A  07'." );
         puts( "Press  ENTER without any command to see register  values" );
         puts( "without changing any of them.   Type 'help' to see these" );
         puts( "instructions again and type 'stop' to end the program.\n");
         puts( "Available registers:   DDR1A, IOR1A, DDR1B, IOR1B, DDR2A" );
         puts( "IOR2A, DDR2B, IOR2B, DDR3A, IOR3A, DDR3B and IOR3B." );
         instr = 0;
      }

      printf( "\n Command:  " );

      fgets(cmd, sizeof(cmd)-1, stdin);
      cmd[strlen(cmd)-1] = '\0';

      if( strncasecmp(cmd, "stop", 4) == 0) {
         going = 0;  
      }
      else if( strncasecmp(cmd, "help", 4) == 0) {
         instr = 1;
      }
      else if( strncasecmp(cmd, "ddr1a", 5) == 0) {
         vp = strchr(cmd, ' ');
         if( vp ) {
            val = atoi( vp );
            set_DDR1A( val );
         }
      }
      else if( strncasecmp(cmd, "ior1a", 5) == 0) {
         vp = strchr(cmd, ' ');
         if( vp ) {
            val = atoi( vp );
            set_IOR1A( val );
         }
      }
      else if( strncasecmp(cmd, "ddr1b", 5) == 0) {
         vp = strchr(cmd, ' ');
         if( vp ) {
            val = atoi( vp );
            set_DDR1B( val );
         }
      }
      else if( strncasecmp(cmd, "ior1b", 5) == 0) {
         vp = strchr(cmd, ' ');
         if( vp ) {
            val = atoi( vp );
            set_IOR1B( val );
         }
      }
      else if( strncasecmp(cmd, "ddr2a", 5) == 0) {
         vp = strchr(cmd, ' ');
         if( vp ) {
            val = atoi( vp );
            set_DDR2A( val );
         }
      }
      else if( strncasecmp(cmd, "ior2a", 5) == 0) {
         vp = strchr(cmd, ' ');
         if( vp ) {
            val = atoi( vp );
            set_IOR2A( val );
         }
      }
      else if( strncasecmp(cmd, "ddr2b", 5) == 0) {
         vp = strchr(cmd, ' ');
         if( vp ) {
            val = atoi( vp );
            set_DDR2B( val );
         }
      }
      else if( strncasecmp(cmd, "ior2b", 5) == 0) {
         vp = strchr(cmd, ' ');
         if( vp ) {
            val = atoi( vp );
            set_IOR2B( val );
         }
      }
      else if( strncasecmp(cmd, "ddr3a", 5) == 0) {
         vp = strchr(cmd, ' ');
         if( vp ) {
            val = atoi( vp );
            set_DDR3A( val );
         }
      }
      else if( strncasecmp(cmd, "ior3a", 5) == 0) {
         vp = strchr(cmd, ' ');
         if( vp ) {
            val = atoi( vp );
            set_IOR3A( val );
         }
      }
      else if( strncasecmp(cmd, "ddr3b", 5) == 0) {
         vp = strchr(cmd, ' ');
         if( vp ) {
            val = atoi( vp );
            set_DDR3B( val );
         }
      }
      else if( strncasecmp(cmd, "ior3b", 5) == 0) {
         vp = strchr(cmd, ' ');
         if( vp ) {
            val = atoi( vp );
            set_IOR3B( val );
         }
      }
   }
}
