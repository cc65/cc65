#
# makefile for CC65 runtime library
#

.SUFFIXES: .o .s .c

#--------------------------------------------------------------------------
# Rules

.c.s:
	@$(CC) $(CFLAGS) $<

.s.o:
	@$(AS) -g -o $@ $(AFLAGS) $<

#--------------------------------------------------------------------------
# Object files

OBJS = 	add.o		\
       	addeqsp.o	\
	addysp.o	\
        along.o         \
       	and.o		\
       	aslax1.o	\
       	aslax2.o	\
       	aslax3.o	\
	aslax4.o	\
       	asleax1.o	\
       	asleax2.o	\
       	asleax3.o	\
	asleax4.o	\
       	asrax1.o	\
       	asrax2.o	\
       	asrax3.o	\
	asrax4.o	\
       	asreax1.o	\
       	asreax2.o	\
       	asreax3.o	\
	asreax4.o	\
       	axlong.o	\
       	bneg.o		\
       	bpushbsp.o	\
       	call.o		\
       	compl.o		\
	condes.o	\
	decax1.o	\
	decax2.o	\
       	decax3.o	\
	decax4.o	\
	decax5.o	\
	decax6.o	\
	decax7.o	\
	decax8.o	\
	decaxy.o	\
	decsp1.o	\
       	decsp2.o	\
	decsp3.o	\
	decsp4.o	\
	decsp5.o	\
	decsp6.o	\
	decsp7.o	\
	decsp8.o	\
       	div.o		\
       	enter.o		\
       	eq.o		\
       	ge.o		\
       	gt.o		\
       	icmp.o		\
	incax1.o	\
	incax2.o	\
       	incax3.o	\
	incax5.o	\
	incax6.o	\
	incax7.o	\
	incax8.o	\
       	incaxy.o	\
	incsp1.o	\
       	incsp2.o	\
	incsp3.o	\
	incsp4.o	\
	incsp5.o	\
	incsp6.o	\
	incsp7.o	\
	incsp8.o	\
        jmpvec.o        \
       	ladd.o		\
       	laddeq.o	\
       	laddeqsp.o	\
       	land.o		\
       	lbneg.o		\
       	lcmp.o		\
       	lcompl.o	\
       	ldai.o		\
       	ldau0sp.o	\
       	ldaui.o		\
       	ldauisp.o	\
       	ldaxi.o		\
       	ldaxsp.o	\
       	ldeaxi.o	\
	ldeaxysp.o	\
       	ldec.o		\
       	ldiv.o		\
       	le.o		\
       	leasp.o		\
       	leave.o		\
       	leq.o		\
       	lge.o		\
       	lgt.o		\
       	linc.o		\
       	lle.o		\
       	llt.o		\
       	lmod.o		\
       	lmul.o		\
       	lne.o		\
       	lneg.o		\
       	lor.o		\
       	lpop.o		\
       	lpush.o		\
       	lrsub.o		\
       	lsave.o		\
       	lshelp.o	\
       	lshl.o		\
       	lshr.o		\
       	lsub.o		\
       	lsubeq.o	\
       	lsubeqsp.o	\
       	lt.o  		\
       	ltest.o		\
       	ludiv.o		\
       	luge.o		\
       	lugt.o		\
       	lule.o		\
       	lult.o		\
       	lumod.o		\
       	lxor.o 		\
       	makebool.o	\
       	mod.o  		\
       	mul.o  		\
	mulax3.o	\
	mulax5.o	\
	mulax6.o	\
	mulax7.o	\
	mulax9.o	\
	mulax10.o	\
       	ne.o   		\
       	neg.o  		\
       	or.o   		\
	pleasp.o	\
	popa.o 		\
       	popsreg.o	\
	push1.o	       	\
       	push2.o	       	\
	push3.o	       	\
	push4.o	       	\
	push5.o	       	\
	push6.o	       	\
	push7.o	       	\
	pusha.o		\
	pushaff.o	\
	pushax.o	\
       	pushb.o		\
       	pushbsp.o	\
	pushc0.o	\
       	pushc1.o	\
	pushc2.o	\
	pushlysp.o	\
       	pushw.o		\
       	pushwsp.o	\
        regswap.o       \
        regswap1.o      \
        regswap2.o      \
	return0.o	\
	return1.o	\
       	rsub.o 		\
       	shelp.o		\
       	shl.o  		\
       	shr.o  		\
       	shrax1.o	\
       	shrax2.o	\
       	shrax3.o	\
	shrax4.o	\
       	shreax1.o	\
       	shreax2.o	\
       	shreax3.o	\
	shreax4.o	\
	staspidx.o	\
       	staxsp.o	\
	staxspi.o	\
       	steaxsp.o	\
	steaxspi.o	\
	stkchk.o	\
       	sub.o		\
       	subeqsp.o	\
	subysp.o	\
       	swap.o		\
       	test.o		\
       	tosint.o	\
       	toslong.o	\
       	udiv.o		\
       	uge.o		\
       	ugt.o		\
       	ule.o		\
       	ult.o		\
       	umod.o		\
       	xor.o           \
        zeropage.o

#--------------------------------------------------------------------------
# Targets

.PHONY:	all clean zap

all:   	$(OBJS)

clean:
	@rm -f *~ $(COBJS:.o=.s) $(OBJS)

zap:	clean

