#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

.c.s:
	@echo $<
	@$(CC) $(CFLAGS) $<

.s.o:
	@echo $<
	@$(AS) -g -o $@ $(AFLAGS) $<

OBJS   = runtime.o mul.o div.o push.o inc.o dec.o shl.o shr.o add.o\
	 sub.o rsub.o or.o xor.o and.o neg.o bneg.o compl.o icmp.o\
	 call.o swap.o switch.o gt.o ugt.o ge.o makebool.o ldau0sp.o\
	 uge.o lt.o ult.o le.o ule.o eq.o ne.o test.o subeqsp.o\
	 udiv.o umod.o mod.o shelp.o aslax1.o asrax1.o shrax1.o\
         aslax2.o asrax2.o shrax2.o aslax3.o asrax3.o shrax3.o\
	 enter.o leave.o leaysp.o popsreg.o ldai.o ldaxi.o ldauisp.o\
	 ldaui.o pushw.o pushb.o staxsp.o ldaxsp.o addeqsp.o\
	 ldasp.o ldausp.o bpushbsp.o pushwsp.o pushbsp.o

LOBJS  = lruntime.o lconvert.o ladd.o lsub.o lrsub.o leq.o lne.o\
	 lneg.o lbneg.o lcompl.o lpush.o land.o lor.o lxor.o ldeaxi.o\
	 ltest.o llt.o lle.o lge.o lgt.o lsave.o asleax1.o laddeqsp.o\
	 asreax1.o shreax1.o asleax2.o asreax2.o shreax2.o lsubeqsp.o\
	 asleax3.o asreax3.o shreax3.o lmul.o lshelp.o ludiv.o lumod.o\
	 ldiv.o lmod.o lswitch.o steaxsp.o lshr.o lshl.o lcmp.o lugt.o\
	 luge.o lult.o lule.o linc.o ldec.o lswap.o lpop.o ldeax.o\
	 lsubeq.o laddeq.o

all:	$(OBJS) $(LOBJS)

clean:
	@rm -f *~
	@rm -f $(COBJS:.o=.s)
	@rm -f $(OBJS)
	@rm -f $(LOBJS)


