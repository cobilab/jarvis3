#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <ctype.h>
#include <time.h>
#include "mem.h"
#include "defs.h"
#include "msg.h"
#include "buffer.h"
#include "alphabet.h"
#include "levels.h"
#include "common.h"
#include "pmodels.h"
#include "tolerant.h"
#include "context.h"
#include "bitio.h"
#include "arith.h"
#include "arith_aux.h"

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - C O M P R E S S O R - - - - - - - - - - - - - -

int Compress(Parameters *P, CModel **cModels, uint8_t id, uint32_t 
refNModels, INF *I){
  FILE        *Reader  = Fopen(P->tar[id], "r");
  char        *name    = concatenate(P->tar[id], ".co");
  FILE        *Writter = Fopen(name, "w");
  uint32_t    n, k, x, cModel, totModels, idxPos;
  uint64_t    i, size = 0;
  uint8_t     *readerBuffer, sym, *pos;
  double      se = 0;
  PModel      **pModel, *MX;
  FloatPModel *PT;
  CBUF        *symBuf = CreateCBuffer(BUFFER_SIZE, BGUARD);
  CMWeight    *WM;

  if(P->verbose)
    fprintf(stderr, "Analyzing data and creating models ...\n");

  #ifdef ESTIMATE
  FILE *IAE = NULL;
  char *IAEName = NULL;
  if(P->estim == 1){
    IAEName = concatenate(P->tar[id], ".iae");
    IAE = Fopen(IAEName, "w");
    }
  #endif
  
  _bytes_output = 0;
  size = NBytesInFile(Reader);

  fprintf(stderr, "Checking quality lines ... \n");
  
  uint64_t line_size = 0;
  while((sym = fgetc(Reader)) != EOF && sym != '\n' && sym != '\0')
    ++line_size;
  ++line_size;

  if((line_size = LineSizesEqual(Reader, line_size)) != 0)
    fprintf(stderr, "Lines have the same size (%"PRIu64")\n", line_size);
  else
    fprintf(stderr, "Lines have different size\n");

  // BUILD ALPHABET
  ALPHABET *AL = CreateAlphabet(0);
  LoadAlphabet(AL, Reader, line_size);
  PrintAlphabet(AL);

  // EXTRA MODELS DERIVED FROM EDITS
  totModels = P->nModels;
  for(n = 0 ; n < P->nModels ; ++n) 
    if(P->model[n].edits != 0){
      totModels++;
      }

  fprintf(stderr, "Using %u probabilistic models\n", totModels);

  pModel        = (PModel  **) Calloc(totModels, sizeof(PModel *));
  for(n = 0 ; n < totModels ; ++n)
    pModel[n]   = CreatePModel(AL->cardinality);
  MX            = CreatePModel(AL->cardinality);
  PT            = CreateFloatPModel(AL->cardinality);
  WM            = CreateWeightModel(totModels);
  readerBuffer  = (uint8_t *) Calloc(BUFFER_SIZE, sizeof(uint8_t));

  for(n = 0 ; n < P->nModels ; ++n)
    if(P->model[n].type == TARGET){
      cModels[n] = CreateCModel(P->model[n].ctx, P->model[n].den, TARGET, 
      P->model[n].edits, P->model[n].eDen, AL->cardinality, P->model[n].gamma,
      P->model[n].eGamma);
      }

  if(P->verbose){
    fprintf(stderr, "Done!\n");
    fprintf(stderr, "Compressing target sequence %d [%"PRIu64" symbols] ...\n", 
    id + 1, size);
    }

  startoutputtingbits();
  start_encode();

  WriteNBits(WATERMARK,           WATERMARK_BITS, Writter);
  WriteNBits(P->checksum,          CHECKSUM_BITS, Writter);
  WriteNBits(size,                     SIZE_BITS, Writter);
  WriteNBits(line_size,                  LS_BITS, Writter);

  WriteNBits(AL->cardinality,                   CARDINALITY_BITS, Writter);
  for(x = 0 ; x < AL->cardinality ; ++x)
    WriteNBits(AL->toChars[x],                          SYM_BITS, Writter);
  WriteNBits(P->nModels,                           N_MODELS_BITS, Writter);
  for(n = 0 ; n < P->nModels ; ++n){
    WriteNBits(cModels[n]->ctx,                         CTX_BITS, Writter);
    WriteNBits(cModels[n]->alphaDen,              ALPHA_DEN_BITS, Writter);
    WriteNBits((int)(cModels[n]->gamma * 65534),      GAMMA_BITS, Writter);
    WriteNBits(cModels[n]->edits,                     EDITS_BITS, Writter);
    if(cModels[n]->edits != 0){
      WriteNBits((int)(cModels[n]->eGamma * 65534), E_GAMMA_BITS, Writter);
      WriteNBits(cModels[n]->TM->den,                 E_DEN_BITS, Writter);
      }
    WriteNBits(P->model[n].type,                       TYPE_BITS, Writter);
    }

  I[id].header = _bytes_output;

  // GIVE SPECIFIC GAMMA:
  int pIdx = 0;
  for(n = 0 ; n < P->nModels ; ++n){
    WM->gamma[pIdx++] = cModels[n]->gamma;
    if(P->model[n].edits != 0){
      WM->gamma[pIdx++] = cModels[n]->eGamma;
      }
    }

  i = 0;
  while((k = fread(readerBuffer, 1, BUFFER_SIZE, Reader)))
    for(idxPos = 0 ; idxPos < k ; ++idxPos){

      CalcProgress(size, ++i);
     
      if(line_size != 0 && readerBuffer[idxPos] == '\n')
        continue;

      symBuf->buf[symBuf->idx] = sym = AL->revMap[ readerBuffer[idxPos] ];   
      memset((void *)PT->freqs, 0, AL->cardinality * sizeof(double));

      n = 0;
      pos = &symBuf->buf[symBuf->idx-1];
      for(cModel = 0 ; cModel < P->nModels ; ++cModel){
        CModel *CM = cModels[cModel];
        GetPModelIdx(pos, CM);
        ComputePModel(CM, pModel[n], CM->pModelIdx, CM->alphaDen);
        ComputeWeightedFreqs(WM->weight[n], pModel[n], PT, CM->nSym);
        if(CM->edits != 0){
          ++n;
          CM->TM->seq->buf[CM->TM->seq->idx] = sym;
          CM->TM->idx = GetPModelIdxCorr(CM->TM->seq->buf+
          CM->TM->seq->idx-1, CM, CM->TM->idx);
          ComputePModel(CM, pModel[n], CM->TM->idx, CM->TM->den);
          ComputeWeightedFreqs(WM->weight[n], pModel[n], PT, CM->nSym);
          }
        ++n;
        }

      ComputeMXProbs(PT, MX, AL->cardinality);

      AESym(sym, (int *)(MX->freqs), (int) MX->sum, Writter);
      #ifdef ESTIMATE
      if(P->estim != 0)
        fprintf(IAE, "%.3g\n", PModelSymbolNats(MX, sym) / M_LN2);
      #endif

      CalcDecayment(WM, pModel, sym);

      for(n = 0 ; n < P->nModels ; ++n)
        if(cModels[n]->ref == TARGET)
          UpdateCModelCounter(cModels[n], sym, cModels[n]->pModelIdx);

      RenormalizeWeights(WM);

      n = 0;
      for(cModel = 0 ; cModel < P->nModels ; ++cModel){
        if(cModels[cModel]->edits != 0)
          UpdateTolerantModel(cModels[cModel]->TM, pModel[++n], sym);
        ++n;
        }

      UpdateCBuffer(symBuf);
      }

  finish_encode(Writter);
  doneoutputtingbits(Writter);
  fclose(Writter);

  se = PrintSE(AL);

  #ifdef ESTIMATE
  if(P->estim == 1){
    fclose(IAE);
    Free(IAEName);
    }
  #endif

  RemovePModel(MX);
  Free(name);
  for(n = 0 ; n < P->nModels ; ++n)
    if(P->model[n].type == REFERENCE) {
      ResetCModelIdx(cModels[n]);
      RemoveCModel(cModels[n]);
    }
    else
      RemoveCModel(cModels[n]);
  for(n = 0 ; n < totModels ; ++n){
    RemovePModel(pModel[n]);
    }
  Free(pModel);

  RemoveFPModel(PT);
  Free(readerBuffer);
  RemoveCBuffer(symBuf);
  int card = AL->cardinality;
  RemoveAlphabet(AL);
  RemoveWeightModel(WM);
  fclose(Reader);

  if(P->verbose == 1)
    fprintf(stderr, "Done!                          \n");  // SPACES ARE VALID 

  I[id].bytes = _bytes_output;
  I[id].size  = i;
  I[id].se    = se;
  return card;
  }


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - M A I N - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int32_t main(int argc, char *argv[]){
  char        **p = *&argv, **xargv, *xpl = NULL;
  CModel      **refModels;
  int32_t     xargc = 0, cardinality = 1;
  uint32_t    n, k, refNModels;
  uint64_t    totalBytes, headerBytes, totalSize;
  clock_t     stop = 0, start = clock();
  double      se_average;
  
  Parameters  *P;
  INF         *I;

  P = (Parameters *) Malloc(1 * sizeof(Parameters));
  if((P->help = ArgsState(DEFAULT_HELP, p, argc, "-h")) == 1 || argc < 2){
    PrintMenu();
    return EXIT_SUCCESS;
    }

  if(ArgsState(DEF_VERSION, p, argc, "-V")){
    PrintVersion();
    return EXIT_SUCCESS;
    }

  if(ArgsState(0, p, argc, "-s")){
    PrintLevels(); 
    return EXIT_SUCCESS;
    }

  P->verbose = ArgsState  (DEFAULT_VERBOSE, p, argc, "-v" );
  P->force   = ArgsState  (DEFAULT_FORCE,   p, argc, "-f" );
  P->estim   = ArgsState  (0,               p, argc, "-e" );
  P->level   = ArgsNum    (0,   p, argc, "-l", MIN_LEVEL, MAX_LEVEL);
  P->low     = ArgsNum    (10,  p, argc, "-t", MIN_THRESHOLD, MAX_THRESHOLD);

  P->nModels = 0;
  for(n = 1 ; n < argc ; ++n)
    if(strcmp(argv[n], "-rm") == 0 || strcmp(argv[n], "-tm") == 0)
      P->nModels += 1;

  if(P->nModels == 0 && P->level == 0)
    P->level = DEFAULT_LEVEL;
  
  if(P->level != 0){
    xpl = GetLevels(P->level);
    xargc = StrToArgv(xpl, &xargv);
    for(n = 1 ; n < xargc ; ++n)
      if(strcmp(xargv[n], "-rm") == 0 || strcmp(xargv[n], "-tm") == 0)
        P->nModels += 1;
    }

  if(P->nModels == 0){
    fprintf(stderr, "Error: at least you need to use a context model!\n");
    return 1;
    }

  P->model = (ModelPar *) Calloc(P->nModels, sizeof(ModelPar));

  k = 0;
  refNModels = 0;
  for(n = 1 ; n < argc ; ++n)
    if(strcmp(argv[n], "-rm") == 0){
      P->model[k++] = ArgsUniqModel(argv[n+1], 1);
      ++refNModels;
      }
  if(P->level != 0){
    for(n = 1 ; n < xargc ; ++n)
      if(strcmp(xargv[n], "-rm") == 0){
        P->model[k++] = ArgsUniqModel(xargv[n+1], 1);
        ++refNModels;
        }
    }

  for(n = 1 ; n < argc ; ++n)
    if(strcmp(argv[n], "-tm") == 0)
      P->model[k++] = ArgsUniqModel(argv[n+1], 0);
  if(P->level != 0){
    for(n = 1 ; n < xargc ; ++n)
      if(strcmp(xargv[n], "-tm") == 0)
        P->model[k++] = ArgsUniqModel(xargv[n+1], 0);
    }

  P->ref      = ArgsString (NULL, p, argc, "-r");
  P->nTar     = ReadFNames (P, argv[argc-1]);
  P->checksum = 0;
  if(P->verbose) 
    PrintArgs(P);

  refModels = (CModel **) Malloc(P->nModels * sizeof(CModel *));

  I = (INF *) Calloc(P->nTar, sizeof(INF));

  se_average  = 0;
  totalSize   = 0;
  totalBytes  = 0;
  headerBytes = 0;
  cardinality = 1;
  for(n = 0 ; n < P->nTar ; ++n){
    cardinality  = Compress(P, refModels, n, refNModels, I);
    totalSize   += I[n].size;
    totalBytes  += I[n].bytes;
    headerBytes += I[n].header;
    se_average  += I[n].se;
    }
  se_average /= P->nTar;

  if(P->nTar > 1)
    for(n = 0 ; n < P->nTar ; ++n){
      fprintf(stdout, "File %d compressed bytes: %"PRIu64" (", n+1, (uint64_t) 
      I[n].bytes);
      PrintHRBytes(I[n].bytes);
      fprintf(stdout, ") , Normalized Dissimilarity Rate: %.6g\n", 
      (8.0*I[n].bytes)/(log2(cardinality)*I[n].size));

      fprintf(stdout, "Shannon entropy: %.6g\n", I[n].se);
      }

  fprintf(stdout, "Total bytes: %"PRIu64" (", totalBytes);
  PrintHRBytes(totalBytes);
  fprintf(stdout, "), %.5g bps, %.5g bps w/ no header\n",
  ((8.0*totalBytes)/totalSize), ((8.0*(totalBytes-headerBytes))/totalSize)); 

  fprintf(stdout, "Normalized Dissimilarity Rate: %.6g\n", (8.0*totalBytes)/
  (log2(cardinality)*totalSize));  

  fprintf(stdout, "Average Shannon entropy: %.6g\n", se_average);
  if(P->level != 0){
    Free(xargv[0]);
    Free(xargv);
  }
  Free(I);
  Free(refModels);
  Free(P->model);
  Free(P->tar);
  Free(P);
  stop = clock();
  fprintf(stdout, "Spent %g sec.\n", ((double)(stop-start))/CLOCKS_PER_SEC);

  return EXIT_SUCCESS;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
