#!/bin/sh

# GEOS .cvt header generator...
# it's a really quick and dirty hack ... big and ugly...

# by Maciej 'YTM/Alliance' Witkowiak
# 9/10.3.2000

case "$1" in

    *.res)

#include resource file

    . $1

#test is arguments are really given, if not - set defaults

# binary file type
if [ -z "$progtype" ];	then progtype="APPLICATION"; fi

# filenames
if [ -z "$dosname" ];	then dosname="testapp"	; fi
if [ -z "$dostype" ];	then dostype="USR"	; fi

# date
if [ -z "$year" ];	then year=`date +%y`	; fi
if [ -z "$month" ];	then month=`date +%m`	; fi
if [ -z "$day" ];	then day=`date +%e`	; fi
if [ -z "$hour" ];	then hour=`date +%k`	; fi
if [ -z "$minute" ];	then minute=`date +%M`	; fi

# screenmode
if [ -z "$screenmode" ]; then screenmode=0	; fi

# names
if [ -z "$class" ];	then class="Programname"; fi
if [ -z "$version" ];	then version="V1.0"	; fi
if [ -z "$author" ];	then author="cc65"; fi
if [ -z "$note" ]; 	then note="Program compiled with cc65 and GEOSLib."; fi

# start generator

cat << __END__

; Maciej 'YTM/Alliance' Witkowiak
; 28.02.2000

; This is .cvt header for GEOS files, it is recognized by Convert v2.5 for GEOS
; and Star Commander (when copying GEOS files to/from .d64 images)

; currently only SEQUENTIAL structure is supported, no overlays

; THIS IS GENERATED FILE, ANY CHANGES WILL BE LOST!!!

			.segment "HEADER"

;
;filetypes
;	GEOS
NOT_GEOS		=	0
BASIC			=	1
ASSEMBLY		=	2
DATA			=	3
SYSTEM			=	4
DESK_ACC		=	5
APPLICATION		=	6
APPL_DATA		=	7
FONT			=	8
PRINTER 		=	9
INPUT_DEVICE		=	10
DISK_DEVICE		=	11
SYSTEM_BOOT		=	12
TEMPORARY		=	13
AUTO_EXEC		=	14
INPUT_128		=	15
NUMFILETYPES		=	16
;	structure
SEQUENTIAL		=	0
VLIR			=	1
;	DOS
DEL			=	0
SEQ			=	1
PRG			=	2
USR			=	3
REL			=	4
CBM			=	5

__END__

echo -e ProgType\\t\\t=\\t$progtype
echo -e \\n\\t\\t.byte $dostype '| $80'\\t\\t'; DOS filetype'
echo -e \\t\\t.word 0\\t\\t'; T&S, will be fixed by converter'
echo $dosname | awk '
{ len=length($0); printf "\t\t.byte %c%s%c\n", 34, substr($0,0,16), 34;
  if (len<16) { len=15-len; printf "\t\t.byte $a0"
  while (len>0) { printf ", $a0"; len-=1 } }
  print ""
  }'
echo -e \\t\\t.word 0\\t\\t'; header T&S'
echo -e \\t\\t.byte SEQUENTIAL
echo -e \\t\\t.byte ProgType
echo -e \\t\\t.byte $year
echo -e \\t\\t.byte $month
echo -e \\t\\t.byte $day
echo -e \\t\\t.byte $hour
echo -e \\t\\t.byte $minute
echo -e \\n\\t\\t.word 0
cat << __END__

		.byte "PRG formatted GEOS file V1.0"
						; converter stamp
		.res \$c4			; some bytes are left
	    
		.byte 3, 21, 63 | \$80	; icon picture header, 63 bytes follow

		;** hey, uberhacker! edit icon here!!! ;-))    
	        .byte %11111111, %11111111, %11111111
	        .byte %10000000, %00000000, %00000001
	        .byte %10000000, %00000000, %00000001
	        .byte %10000000, %00000000, %00000001
	        .byte %10000000, %00000000, %00000001
	        .byte %10000000, %00000000, %00000001
	        .byte %10000000, %00000000, %00000001
	        .byte %10000000, %00000000, %00000001
	        .byte %10000000, %00000000, %00000001
	        .byte %10000000, %00000000, %00000001
	        .byte %10000000, %00000000, %00000001
	        .byte %10000000, %00000000, %00000001
	        .byte %10000000, %00000000, %00000001
	        .byte %10000000, %00000000, %00000001
	        .byte %10000000, %00000000, %00000001
	        .byte %10000000, %00000000, %00000001
	        .byte %10000000, %00000000, %00000001
	        .byte %10000000, %00000000, %00000001
	        .byte %10000000, %00000000, %00000001
	        .byte %10000000, %00000000, %00000001
	        .byte %11111111, %11111111, %11111111

__END__

echo -e \\n\\t\\t.byte $dostype '| $80'\\t\\t';DOS filetype again'

cat << __END__
	        .byte ProgType		;again GEOS type
		.byte SEQUENTIAL	;structure
		.word \$0400		;ProgStart
		.word \$0400-1		;ProgEnd (needs proper value for DESK_ACC)
		.word \$0400		;ProgExec
__END__

echo -e \\t\\t'; GEOS class (11 chars padded with spaces, terminated with space (12th))'
echo $class | awk '
{ len=length($0); printf "\t\t.byte %c%s%c\n", 34, substr($0,0,12), 34;
  if (len<12) { len=11-len; printf "\t\t.byte $20"
  while (len>0) { printf ", $20"; len-=1 } }
  print ""
  }'
echo -e \\t\\t'; version info (4 characters)'
echo -en \\t\\t.byte 
echo $version | awk '{ printf " %c%s%c\n", 34, substr($0,1,4), 34}'
echo -e \\t\\t.byte 0\\t\\t\\t';string terminator'
echo -e \\t\\t.word 0
echo -e \\n\\t\\t.byte $screenmode\\t\\t\\t';40/80 columns capability'
echo -e \\n\\t\\t'; author, up to 62 characters'
echo $author | awk '
{ printf "\t\t.byte %c%s%c\n\t\t.byte 0\n\t\t.res (63-%i)\n", 34, substr($0,0,62), 34, length($0)+1; }'
echo -e \\n\\t\\t';note (up to 95 chars)'
echo $note | awk '
{ printf "\t\t.byte %c%s%c\n\t\t.byte 0\n\t\t.res (96-%i)\n", 34, substr($0,0,95), 34, length($0)+1; }'
echo -e \\n\\n';end of .cvt header, real code follows'

    ;;
    *)
echo "This is GEOSLib .cvt header generator by Maciej Witkowiak"
echo "Usage:"
echo "  headergen resourcefile.res"
echo
echo "Contents of resource file are (case sensitive):"
echo "dostype=[PRG,SEQ,USR]"
echo "dosname=filename"
echo "progtype=[APPLICATION,ASSEMBLY,DESK_ACC,PRINTER,INPUT_DEVICE,AUTO_EXEC,INPUT_128]"
echo "         currently only APPLICATION is supported"
echo "year,month,date,hour,minute=XX - if not given current will be used"
echo "screenmode=[0,64,128,192] -"
echo "		  0 - GEOS128 only 40 columns"
echo "		 64 - GEOS128 both 40/80 columns"
echo "		128 - does not run under GEOS128"
echo "		192 - GEOS128 only 80 columns"	
echo "class=Class	- GEOS class name"
echo "author=Author	- author name"
echo "note=Note	- note field"
echo 
echo "If any of those parameters is not given, default will be used"
echo "Output should be redirected to a file e.g. cvthead.s"
echo "compiled with ca65 and used in linking process as the first object file -"
echo "even before crt0.o"
    ;;
esac
