#==============================================================================#
#       XScore 2020 & MERGE & SPLIT     IEETA/DETI, UNIVERSITY OF AVEIRO       #
#==============================================================================#
BIN    = .
CC     = gcc
CPLP   =
#-------------------------------------------------------------------------------
CFLAGS = -O3 -Wall $(CPLP)
#-------------------------------------------------------------------------------
LIBS   = -lm
DEPS   = defs.h
PROGS  = $(BIN)/XScoreC $(BIN)/XScoreD \
	 $(BIN)/MergeFastqStreams $(BIN)/SplitFastqStreams \
         $(BIN)/MergeFastaStreams $(BIN)/SplitFastaStreams
OBJS   = mem.o msg.o buffer.o common.o hash.o array.o levels.o alphabet.o \
         arith.o arith_aux.o bitio.o pmodels.o tolerant.o context.o
#-------------------------------------------------------------------------------
all:
	$(MAKE) progs
progs: $(PROGS)
$(BIN)/MergeFastqStreams: MergeFastqStreams.c
	$(CC) $(CFLAGS) -o $(BIN)/MergeFastqStreams MergeFastqStreams.c
$(BIN)/SplitFastqStreams: SplitFastqStreams.c
	$(CC) $(CFLAGS) -o $(BIN)/SplitFastqStreams SplitFastqStreams.c
$(BIN)/MergeFastaStreams: MergeFastaStreams.c
	$(CC) $(CFLAGS) -o $(BIN)/MergeFastaStreams MergeFastaStreams.c
$(BIN)/SplitFastaStreams: SplitFastaStreams.c
	$(CC) $(CFLAGS) -o $(BIN)/SplitFastaStreams SplitFastaStreams.c
$(BIN)/XScoreC: xscorec.c $(DEPS) $(OBJS)
	$(CC) $(CFLAGS) -o $(BIN)/XScoreC xscorec.c $(OBJS) $(LIBS)
$(BIN)/XScoreD: xscored.c $(DEPS) $(OBJS)
	$(CC) $(CFLAGS) -o $(BIN)/XScoreD xscored.c $(OBJS) $(LIBS)
mem.o: mem.c mem.h $(DEPS)
	$(CC) -c $(CFLAGS) mem.c
msg.o: msg.c msg.h $(DEPS)
	$(CC) -c $(CFLAGS) msg.c
buffer.o: buffer.c buffer.h $(DEPS)
	$(CC) -c $(CFLAGS) buffer.c
common.o: common.c common.h $(DEPS)
	$(CC) -c $(CFLAGS) common.c
hash.o: hash.c hash.h $(DEPS)
	$(CC) -c $(CFLAGS) hash.c
array.o: array.c array.h $(DEPS)
	$(CC) -c $(CFLAGS) array.c
levels.o: levels.c levels.h $(DEPS)
	$(CC) -c $(CFLAGS) levels.c
alphabet.o: alphabet.c alphabet.h $(DEPS)
	$(CC) -c $(CFLAGS) alphabet.c
arith.o: arith.c arith.h $(DEPS)
	$(CC) -c $(CFLAGS) arith.c
arith_aux.o: arith_aux.c arith_aux.h $(DEPS)
	$(CC) -c $(CFLAGS) arith_aux.c
bitio.o: bitio.c bitio.h $(DEPS)
	$(CC) -c $(CFLAGS) bitio.c
pmodels.o: pmodels.c pmodels.h $(DEPS)
	$(CC) -c $(CFLAGS) pmodels.c
tolerant.o: tolerant.c tolerant.h $(DEPS)
	$(CC) -c $(CFLAGS) tolerant.c
context.o: context.c context.h $(DEPS)
	$(CC) -c $(CFLAGS) context.c

clean:
	/bin/rm -f *.o
cleanall:
	/bin/rm -f *.o $(PROGS)
#===============================================================================

