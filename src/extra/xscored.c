#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <ctype.h>
#include <time.h>
#include "mem.h"
#include "msg.h"
#include "defs.h"
#include "buffer.h"
#include "alphabet.h"
#include "common.h"
#include "pmodels.h"
#include "tolerant.h"
#include "context.h"
#include "bitio.h"
#include "arith.h"
#include "arith_aux.h"

//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - D E C O M P R E S S O R - - - - - - - - - - - -

void Decompress(Parameters *P, CModel **cModels, uint8_t id){
  FILE        *Reader  = Fopen(P->tar[id], "r");
  char        *name    = ReplaceSubStr(P->tar[id], ".co", ".de"); 
  FILE        *Writter = Fopen(name, "w");
  uint64_t    nSymbols = 0, line_size = 0, idx_line = 0;
  uint32_t    n, k, x, cModel, totModels;
  int32_t     idxOut = 0;
  uint8_t     *outBuffer, sym = 0, *pos;
  CBUF        *symBuf = CreateCBuffer(BUFFER_SIZE, BGUARD);
  PModel      **pModel, *MX;
  FloatPModel *PT;
  CMWeight    *WM;
  uint64_t    i = 0;
  int64_t     garbagex = 0;

  if(P->verbose)
    fprintf(stderr, "Decompressing %"PRIu64" symbols of target %d ...\n", 
    P[id].size, id + 1);

  startinputtingbits();
  start_decode(Reader);

  P[id].watermark        = ReadNBits(       WATERMARK_BITS, Reader);
  garbagex               = ReadNBits(        CHECKSUM_BITS, Reader);
  P[id].size             = ReadNBits(            SIZE_BITS, Reader);
  line_size              = ReadNBits(              LS_BITS, Reader);
  ALPHABET *AL = CreateAlphabet(0);
  AL->length   = P[id].size;
  AL->cardinality        = ReadNBits(     CARDINALITY_BITS, Reader);
  for(x = 0 ; x < 256 ; ++x)
    AL->revMap[x] = INVALID_SYMBOL;
  for(x = 0 ; x < AL->cardinality ; ++x){
    AL->toChars[x]       = ReadNBits(             SYM_BITS, Reader);
    AL->revMap[(uint8_t) AL->toChars[x]] = x;
    }
  P[id].nModels          = ReadNBits(        N_MODELS_BITS, Reader);
  for(k = 0 ; k < P[id].nModels ; ++k){
    P[id].model[k].ctx   = ReadNBits(             CTX_BITS, Reader);
    P[id].model[k].den   = ReadNBits(       ALPHA_DEN_BITS, Reader);
    P[id].model[k].gamma = ReadNBits(           GAMMA_BITS, Reader) / 65534.0;
    P[id].model[k].edits = ReadNBits(           EDITS_BITS, Reader);
    if(P[id].model[k].edits != 0){
      P[id].model[k].eGamma = ReadNBits(      E_GAMMA_BITS, Reader) / 65534.0;
      P[id].model[k].eDen   = ReadNBits(        E_DEN_BITS, Reader);
      }
    P[id].model[k].type  = ReadNBits(            TYPE_BITS, Reader);
    }

  PrintAlphabet(AL);

  // EXTRA MODELS DERIVED FROM EDITS
  totModels = P[id].nModels;
  for(n = 0 ; n < P[id].nModels ; ++n)
    if(P[id].model[n].edits != 0)
      ++totModels; // TOLERANT

  nSymbols      = P[id].size;
  pModel        = (PModel  **) Calloc(totModels, sizeof(PModel *));
  for(n = 0 ; n < totModels ; ++n)
    pModel[n]   = CreatePModel(AL->cardinality);
  MX            = CreatePModel(AL->cardinality);
  PT            = CreateFloatPModel(AL->cardinality);
  outBuffer     = (uint8_t  *) Calloc(BUFFER_SIZE, sizeof(uint8_t));
  WM            = CreateWeightModel(totModels);

  for(n = 0 ; n < P[id].nModels ; ++n){
    if(P[id].model[n].type == TARGET)
      cModels[n] = CreateCModel(P[id].model[n].ctx , P[id].model[n].den, 
      TARGET, P[id].model[n].edits, P[id].model[n].eDen, AL->cardinality,
      P[id].model[n].gamma, P[id].model[n].eGamma);
    }

  // GIVE SPECIFIC GAMMA:
  int pIdx = 0;
  for(n = 0 ; n < P[id].nModels ; ++n){
    WM->gamma[pIdx++] = cModels[n]->gamma;
    if(P[id].model[n].edits != 0){
      WM->gamma[pIdx++] = cModels[n]->eGamma;
      }
    }

  i = 0;
  idx_line = 0;
  while(nSymbols--){
    CalcProgress(P[id].size, ++i);

    if(line_size != 0 && line_size == ++idx_line){
      outBuffer[idxOut] = '\n';
      if(++idxOut == BUFFER_SIZE){
        fwrite(outBuffer, 1, idxOut, Writter);
        idxOut = 0;
        }
      idx_line = 0;
      continue;
      }

    memset((void *)PT->freqs, 0, AL->cardinality * sizeof(double));

    n = 0;
    pos = &symBuf->buf[symBuf->idx-1];
    for(cModel = 0 ; cModel < P[id].nModels ; ++cModel){
      CModel *CM = cModels[cModel];
      GetPModelIdx(pos, CM);
      ComputePModel(CM, pModel[n], CM->pModelIdx, CM->alphaDen);
      ComputeWeightedFreqs(WM->weight[n], pModel[n], PT, CM->nSym);
      if(CM->edits != 0){
        ++n;
        CM->TM->idx = GetPModelIdxCorr(CM->TM->seq->buf+
        CM->TM->seq->idx-1, CM, CM->TM->idx);
        ComputePModel(CM, pModel[n], CM->TM->idx, CM->TM->den);
        ComputeWeightedFreqs(WM->weight[n], pModel[n], PT, CM->nSym);
        }
      ++n;
      }

    ComputeMXProbs(PT, MX, AL->cardinality);

    symBuf->buf[symBuf->idx] = sym = ArithDecodeSymbol(AL->cardinality, (int *) 
    MX->freqs, (int) MX->sum, Reader);
    
    outBuffer[idxOut] = AL->toChars[sym];

    for(n = 0 ; n < P[id].nModels ; ++n)
      if(cModels[n]->edits != 0){
        cModels[n]->TM->seq->buf[cModels[n]->TM->seq->idx] = sym;
        }         

    CalcDecayment(WM, pModel, sym);

    for(n = 0 ; n < P[id].nModels ; ++n)
      if(P[id].model[n].type == TARGET)
        UpdateCModelCounter(cModels[n], sym, cModels[n]->pModelIdx);

    RenormalizeWeights(WM);

    n = 0;
    for(cModel = 0 ; cModel < P[id].nModels ; ++cModel){
      if(cModels[cModel]->edits != 0){      // CORRECT CMODEL CONTEXTS
        UpdateTolerantModel(cModels[cModel]->TM, pModel[++n], sym);
        }
      ++n;
      }

    if(++idxOut == BUFFER_SIZE){
      fwrite(outBuffer, 1, idxOut, Writter);
      idxOut = 0;
      }

    UpdateCBuffer(symBuf);
    }

  if(idxOut != 0) 
    fwrite(outBuffer, 1, idxOut, Writter);

  finish_decode();
  doneinputtingbits();

  fclose(Writter);
  RemovePModel(MX);
  Free(name);
  for(n = 0 ; n < P[id].nModels ; ++n){
    if(P[id].model[n].type == REFERENCE) {
      ResetCModelIdx(cModels[n]);
      RemoveCModel(cModels[n]);
    }
    else
      RemoveCModel(cModels[n]);
    }

  for(n = 0 ; n < totModels ; ++n)
    RemovePModel(pModel[n]);
  Free(pModel);

  RemoveWeightModel(WM);
  RemoveFPModel(PT);
  Free(outBuffer);
  RemoveCBuffer(symBuf);
  RemoveAlphabet(AL);
  fclose(Reader);

  if(P->verbose == 1)
    fprintf(stderr, "Done!                          \n");  // SPACES ARE VALID 
  }


//////////////////////////////////////////////////////////////////////////////
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - M A I N - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int32_t main(int argc, char *argv[]){
  char        **p = *&argv;
  CModel      **refModels; 
  uint32_t    n, k, *checksum, refNModels = 0, cardinality;
  Parameters  *P;
  FILE        *Reader = NULL;
  uint8_t     help, verbose, force, nTar = 1;
  clock_t     stop = 0, start = clock();
  int64_t     garbagex = 0;

  if((help = ArgsState(DEFAULT_HELP, p, argc, "-h")) == 1 || argc < 2){
    PrintMenuD();
    return EXIT_SUCCESS;
    }

  if(ArgsState(DEF_VERSION, p, argc, "-V")){
    PrintVersion();  
    return EXIT_SUCCESS;
    }

  verbose  = ArgsState  (DEFAULT_VERBOSE, p, argc, "-v");
  force    = ArgsState  (DEFAULT_FORCE,   p, argc, "-f");

  for(n = 0 ; n != strlen(argv[argc-1]) ; ++n)
    if(argv[argc-1][n] == ':')
      ++nTar;

  P        = (Parameters *) Malloc(nTar * sizeof(Parameters));
  checksum = (uint32_t   *) Calloc(nTar , sizeof(uint32_t));

  P[0].force   = force;
  P[0].verbose = verbose;
  P[0].nTar    = ReadFNames (P, argv[argc-1]);
  P[0].ref     = ArgsString (NULL, p, argc, "-r");
  for(n = 0 ; n < nTar ; ++n){
    Reader = Fopen(P[0].tar[n], "r");
    startinputtingbits();
    start_decode(Reader);

    refNModels = 0;
    P[n].watermark = ReadNBits(          WATERMARK_BITS, Reader);
    if(P[n].watermark != WATERMARK){
      fprintf(stderr, "Error: Invalid compressed file to decompress!\n");
      return 1;
      }
    checksum[n]    = ReadNBits(           CHECKSUM_BITS, Reader);
    P[n].size      = ReadNBits(               SIZE_BITS, Reader);
    garbagex       = ReadNBits(                 LS_BITS, Reader);
    cardinality    = ReadNBits(        CARDINALITY_BITS, Reader);
    for(k = 0 ; k < cardinality ; ++k)
      garbagex     = ReadNBits(                SYM_BITS,  Reader);
    P[n].nModels   = ReadNBits(           N_MODELS_BITS, Reader);
    P[n].model     = (ModelPar *) Calloc(P[n].nModels, sizeof(ModelPar));
    for(k = 0 ; k < P[n].nModels ; ++k){
      P[n].model[k].ctx   = ReadNBits(         CTX_BITS, Reader); 
      P[n].model[k].den   = ReadNBits(   ALPHA_DEN_BITS, Reader); 
      P[n].model[k].gamma = ReadNBits(       GAMMA_BITS, Reader) / 65534.0;
      P[n].model[k].edits = ReadNBits(       EDITS_BITS, Reader); 
      if(P[n].model[k].edits != 0){
        P[n].model[k].eGamma = ReadNBits(  E_GAMMA_BITS, Reader) / 65534.0;
        P[n].model[k].eDen   = ReadNBits(    E_DEN_BITS, Reader);
        }
      P[n].model[k].type  = ReadNBits(        TYPE_BITS, Reader);
      if(P[n].model[k].type == 1)
        ++refNModels;
      }

    finish_decode();
    doneinputtingbits();
    fclose(Reader);
    }

  if(P->verbose)
    PrintArgs(P);
 
  if(refNModels > 0 && P[0].ref == NULL){
    fprintf(stderr, "Error: using reference model(s) in nonexistent "
    "reference sequence!\n");
    exit(1);
    }

  refModels = (CModel **) Malloc(P->nModels * sizeof(CModel *));

  if(P->verbose && refNModels != 0)
    fprintf(stderr, "Checksum: %"PRIu64"\n", P->checksum); 

  for(n = 0 ; n < nTar ; ++n){
    Decompress(P, refModels, n);
    }

  Free(checksum);
  Free(refModels);
  for(n = 0 ; n < nTar ; ++n){
    Free(P[n].model);
  }
  Free(P->tar);
  Free(P);

  stop = clock();
  fprintf(stderr, "Spent %g sec.\n", ((double)(stop-start))/CLOCKS_PER_SEC);

  return EXIT_SUCCESS;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
