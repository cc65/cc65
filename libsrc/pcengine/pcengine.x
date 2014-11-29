MEMORY {

	ZP:  start = $00, size = $1A, type = rw, define = yes;

	# INES Cartridge Header
	#HEADER: start = $0, size = $10, file = %O ,fill = yes;

	# 2 16K ROM Banks
	# - startup
	# - code
	# - rodata
	# - data (load)
	# 1 8k CHR Bank
	ROM0: start = $e000, size = $1ff6, file = %O ,fill = yes, define = yes;
    # Hardware Vectors at End of 2nd 8K ROM
	ROMV: start = $fff6, size = $a, file = %O,fill = yes;

	ROM: start = $6000, size = $8000, file = %O, fill = yes,define=yes;


	# standard 2k SRAM (-zeropage)
	# $0100-$0200 cpu stack
	# $0200-$0500 3 pages for ppu memory write buffer
	# $0500-$0800 3 pages for cc65 parameter stack
	#SRAM: start = $0500, size = $0300, define = yes;

	# additional 8K SRAM Bank
	# - data (run)
	# - bss
	# - heap
#	RAM: start = $2200, size = $1000, define = yes;
#	RAM2: start = $3200, size = $0e00, define = yes;
	RAM: start = $2200, size = $1e00, define = yes;

}
SEGMENTS {
    #HEADER: load = HEADER, type = wprot;

    #aSTARTUP: load = ROM0, type = wprot, define = yes;
    STARTUP: load = ROM0, type = wprot, define = yes;

	CODE: load = ROM, type = wprot, define = yes;
    RODATA: load = ROM, type = wprot, define = yes;

	DATA: load = ROM0, run= RAM, type = rw, define = yes;
#	BSS: load = RAM2, type = bss, define = yes;
	BSS: load = RAM, type = bss, define = yes;

	VECTORS: load = ROMV, type = rw, define = yes;
    #CHARS: load = ROM2, type = rw;


    ZEROPAGE: load = ZP, type = zp, define = yes;

}
FEATURES {
    CONDES: segment = STARTUP,
	    type=constructor,
	    label=__CONSTRUCTOR_TABLE__,
	    count=__CONSTRUCTOR_COUNT__;
    CONDES: segment = STARTUP,
	    type=destructor,
	    label=__DESTRUCTOR_TABLE__,
	    count=__DESTRUCTOR_COUNT__;
}
SYMBOLS {
    __STACKSIZE__ = $0300;	# 3 pages stack
}
