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
	 $(BIN)/MergeFastqStreamsJ3 $(BIN)/SplitFastqStreamsJ3 \
         $(BIN)/MergeFastaStreamsJ3 $(BIN)/SplitFastaStreamsJ3
OBJS   = mem.o msg.o buffer.o common.o hash.o array.o levels.o alphabet.o \
         arith.o arith_aux.o bitio.o pmodels.o tolerant.o context.o
#-------------------------------------------------------------------------------
all:
	$(MAKE) progs
progs: $(PROGS)
$(BIN)/MergeFastqStreamsJ3: MergeFastqStreams.c
	$(CC) $(CFLAGS) -o $(BIN)/MergeFastqStreamsJ3 MergeFastqStreams.c
$(BIN)/SplitFastqStreamsJ3: SplitFastqStreams.c
	$(CC) $(CFLAGS) -o $(BIN)/SplitFastqStreamsJ3 SplitFastqStreams.c
$(BIN)/MergeFastaStreamsJ3: MergeFastaStreams.c
	$(CC) $(CFLAGS) -o $(BIN)/MergeFastaStreamsJ3 MergeFastaStreams.c
$(BIN)/SplitFastaStreamsJ3: SplitFastaStreams.c
	$(CC) $(CFLAGS) -o $(BIN)/SplitFastaStreamsJ3 SplitFastaStreams.c
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

