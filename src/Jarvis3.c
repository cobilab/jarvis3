//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                    J A R V I S 3    2 0 1 4 - 2 0 2 4                    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

#include "defs.h"
#include "common.h"
#include "levels.h"
#include "dna.h"
#include "args.h"
#include "repeats.h"
#include "cm.h"
#include "nn.h"
#include "mix.h"
#include "pmodels.h"
#include "files.h"
#include "strings.h"
#include "mem.h"
#include "msg.h"
#include "bitio.h"
#include "arith.h"
#include "arith_aux.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// FIND DATA TYPE
//
int FindDataType(char *FN)
  {
  FILE *F = Fopen(FN, "r");
  int type = 0;

  switch(fgetc(F))
    {
    case '>': type = 1; break;
    case '@': type = 2; break;
    case '#': type = 3; break;
    default : type = 0;
    }
  fclose(F);

  return type;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// ERR AND EXIT ON UNKNOWN SYMBOL
//
void ErrAndExit(void)
  {
  fprintf(stderr, "Error: non ACGT symbol in sequence!\n");
  exit(1);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// ENCODE HEADER RMS ONLY
//
void EncodeHeaderOnlyRMs(PARAM *P, RCLASS **RC, FILE *F)
  {
  uint32_t n;

  WriteNBits(P->datatype,                          DATA_TYPE_BITS, F);
  WriteNBits(P->seed,                                   SEED_BITS, F);
  WriteNBits(P->size,                                   SIZE_BITS, F);
  WriteNBits(P->length,                               LENGTH_BITS, F);
  WriteNBits(P->hs,                                       HS_BITS, F);
  WriteNBits((uint16_t)(P->lr * 65534),                   LR_BITS, F);
  WriteNBits(0,                                     NCMODELS_BITS, F);
  WriteNBits(P->nCPModels,                          NCMODELS_BITS, F);
  WriteNBits(P->nRMClasses,                       NRMCLASSES_BITS, F);
  for(n = 0 ; n < P->nRMClasses ; ++n){
    WriteNBits(RC[n]->mRM,                       MAX_RMODELS_BITS, F);
    WriteNBits((uint16_t)(RC[n]->P->beta  * 65534),     BETA_BITS, F);
    WriteNBits((uint16_t)(RC[n]->P->gamma * 65534),    GAMMA_BITS, F);
    WriteNBits((uint16_t)(RC[n]->P->iWeight * 65534), WEIGHT_BITS, F);
    WriteNBits(RC[n]->P->limit,                        LIMIT_BITS, F);
    WriteNBits(RC[n]->P->ctx,                            CTX_BITS, F);
    WriteNBits(RC[n]->P->rev,                             IR_BITS, F);
    WriteNBits(RC[n]->P->c_max,                        CACHE_BITS, F);
    }

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// ENCODE HEADER
//
void EncodeHeader(PARAM *P, RCLASS **RC, CMODEL **CM, FILE *F)
  {
  uint32_t n;

  WriteNBits(P->datatype,                          DATA_TYPE_BITS, F);
  WriteNBits(P->seed,                                   SEED_BITS, F);
  WriteNBits(P->size,                                   SIZE_BITS, F);
  WriteNBits(P->length,                               LENGTH_BITS, F);
  WriteNBits(P->hs,                                       HS_BITS, F);
  WriteNBits((uint16_t)(P->lr * 65534),                   LR_BITS, F);
  WriteNBits(P->nCModels,                           NCMODELS_BITS, F);
  for(n = 0 ; n < P->nCModels ; ++n){
    WriteNBits(CM[n]->ctx,                               CTX_BITS, F);
    WriteNBits(CM[n]->alphaDen,                    ALPHA_DEN_BITS, F);
    WriteNBits((int)(CM[n]->gamma * 65534),            GAMMA_BITS, F);
    WriteNBits(CM[n]->ir,                                 IR_BITS, F);
    WriteNBits(CM[n]->edits,                           EDITS_BITS, F);
    if(CM[n]->edits != 0){
      WriteNBits((int)(CM[n]->eGamma * 65534),       E_GAMMA_BITS, F);
      WriteNBits(CM[n]->TM->den,                       E_DEN_BITS, F);
      WriteNBits(CM[n]->TM->ir,                           IR_BITS, F);
      }
    }
  WriteNBits(P->nCPModels,                          NCMODELS_BITS, F);
  WriteNBits(P->nRMClasses,                       NRMCLASSES_BITS, F);
  for(n = 0 ; n < P->nRMClasses ; ++n){
    WriteNBits(RC[n]->mRM,                       MAX_RMODELS_BITS, F);
    WriteNBits((uint16_t)(RC[n]->P->beta  * 65534),     BETA_BITS, F);
    WriteNBits((uint16_t)(RC[n]->P->gamma * 65534),    GAMMA_BITS, F);
    WriteNBits((uint16_t)(RC[n]->P->iWeight * 65534), WEIGHT_BITS, F);
    WriteNBits(RC[n]->P->limit,                        LIMIT_BITS, F);
    WriteNBits(RC[n]->P->ctx,                            CTX_BITS, F);
    WriteNBits(RC[n]->P->rev,                             IR_BITS, F);
    WriteNBits(RC[n]->P->c_max,                        CACHE_BITS, F);
    } 

  #ifdef DEBUG
  printf("datatype = %u\n", P->datatype);
  printf("seed     = %"PRIu32"\n", P->seed);
  printf("size     = %"PRIu64"\n", P->size);
  printf("hs       = %u\n",        P->hs);
  printf("lr       = %g\n",        P->lr);
  printf("select   = %u\n",        P->selection);
  printf("n CMs    = %u\n",        P->nCModels);
  printf("n CPMs   = %u\n",        P->nCPModels);
  for(n = 0 ; n < P->nCModels ; ++n){
    printf("  cmodel %u\n",        n + 1);
    printf("    ctx       = %u\n", CM[n]->ctx);
    printf("    alpha den = %u\n", CM[n]->alphaDen);
    printf("    gamma     = %g\n", CM[n]->gamma);
    printf("    ir        = %u\n", CM[n]->ir);
    printf("    edits     = %u\n", CM[n]->edits);
    if(CM[n]->edits != 0){
      printf("      eGamma = %g\n", CM[n]->eGamma);
      printf("      eDen   = %u\n", CM[n]->TM->den);
      printf("      eIr    = %u\n", CM[n]->TM->ir);
      }
    }
  printf("n class = %u\n",        P->nRMClasses);
  for(n = 0 ; n < P->nRMClasses ; ++n){
    printf("  class %u\n",        n);
    printf("    max rep = %u\n",  RC[n]->mRM);
    printf("    beta    = %g\n",  RC[n]->P->beta);
    printf("    gamma   = %g\n",  RC[n]->P->gamma);
    printf("    weight  = %g\n",  RC[n]->P->iWeight);
    printf("    limit   = %u\n",  RC[n]->P->limit);
    printf("    ctx     = %u\n",  RC[n]->P->ctx);
    printf("    ir      = %u\n",  RC[n]->P->rev);
    printf("    cache   = %lu\n", RC[n]->P->c_max);
    }
  #endif
  }


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// UPDATE CMODELS
//
void UpdateCModels(CMODEL **CM, CBUF *SB, uint8_t sym, uint32_t nCModels)
  {
  uint8_t irSym = 0;
  uint32_t r;	
  
  for(r = 0 ; r < nCModels ; ++r){
    switch(CM[r]->ir){
      case 0:
      UpdateCModelCounter(CM[r], sym, CM[r]->pModelIdx);
      break;
      case 1:
      UpdateCModelCounter(CM[r], sym, CM[r]->pModelIdx);
      irSym = GetPModelIdxIR(SB->buf+SB->idx, CM[r]);
      UpdateCModelCounter(CM[r], irSym, CM[r]->pModelIdxIR);
      break;
      case 2:
      irSym = GetPModelIdxIR(SB->buf+SB->idx, CM[r]);
      UpdateCModelCounter(CM[r], irSym, CM[r]->pModelIdxIR);
      break;
      default:
      UpdateCModelCounter(CM[r], sym, CM[r]->pModelIdx);
      break;
      }
    }

  return;
  }
  
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// COMPRESSION RMs ONLY
//
void CompressRMsOnly(PARAM *P, char *fn)
  {
  FILE      *IN  = Fopen(fn, "r"), *OUT = Fopen(P->output, "w");
  uint64_t  i = 0, mSize = MAX_BUF, pos = 0;
  uint32_t  m, n;
  uint8_t   t[NSYM], *buf = (uint8_t *) Calloc(mSize, sizeof(uint8_t)), sym = 0, 
            *p; 

  RCLASS    **RC;
  PMODEL    *MX_RM;
  CBUF      *SB;

  srand(P->seed);

  if(P->verbose)
    fprintf(stderr, "Analyzing data and creating models ...\n");
 
  if(P->nRMClasses < 1){
    fprintf(stderr, "Error: At least one repeat class model must be set!\n");
    exit(1);    
    }

  P->nCPModels = P->nRMClasses;
  
  MX_RM = CreatePModel(NSYM);
  SB    = CreateCBuffer(BUFFER_SIZE, BGUARD);

  RC = (RCLASS **) Malloc(P->nRMClasses * sizeof(RCLASS *));
  for(n = 0 ; n < P->nRMClasses ; ++n)
    RC[n] = CreateRC(P->rmodel[n].nr, P->rmodel[n].beta,  
            P->rmodel[n].limit, P->rmodel[n].ctx, P->rmodel[n].gamma,
            P->rmodel[n].ir, P->rmodel[n].weight, P->rmodel[n].cache);

  P->length = NBytesInFile(IN);

  if(P->length > 4294967295){
    fprintf(stderr, "Error: DNA sequence larger than 2^32.\n");
    fprintf(stderr, "Tip: Use JARVIS3.sh or split to split data.\n");
    exit(1);
    }

  if(P->datatype != 0)
    {
    fprintf(stderr, "Error: currently JARVIS3 only supports DNA format.\n");
    fprintf(stderr, "Tip: Use JARVIS.sh for FASTA or FASTQ data.\n");
    exit(1);
    }

  P->size = P->length>>2;

  if(P->verbose){
    fprintf(stderr, "Compressing %"PRIu64" symbols ...\n", P->length);
    }

  startoutputtingbits();
  start_encode();

  EncodeHeaderOnlyRMs(P, RC, OUT);

  while((m = fread(t, sizeof(uint8_t), NSYM, IN)) == NSYM)
    {
    if(!strchr("ACGT", t[0])) ErrAndExit();
    if(!strchr("ACGT", t[1])) ErrAndExit();
    if(!strchr("ACGT", t[2])) ErrAndExit();
    if(!strchr("ACGT", t[3])) ErrAndExit();

    buf[i] = S2N(t[3])|(S2N(t[2])<<2)|(S2N(t[1])<<4)|(S2N(t[0])<<6); // PACK 4

    for(n = 0 ; n < m ; ++n)
      {
      SB->buf[SB->idx] = sym = S2N(t[n]);
      p = &SB->buf[SB->idx-1];

      StopRM           (RC[0]);
      StartMultipleRMs (RC[0], p);
      AddKmerPos       (RC[0]->T, RC[0]->P->idx, pos++);     // pos = (i<<2)+n
      RenormWeights    (RC[0]);
      ComputeMixture   (RC[0], MX_RM, buf);

      AESym(sym, (int *) (MX_RM->freqs), (int) MX_RM->sum, OUT);

      UpdateWeights(RC[0], buf, sym);
      UpdateCBuffer(SB);
      }

    if(++i == mSize)    // REALLOC BUFFER ON OVERFLOW 4 STORE THE COMPLETE SEQ
      buf = (uint8_t *) Realloc(buf, (mSize+=ADD_SPACE) * sizeof(uint8_t));

    #ifdef PROGRESS
    if(P->extended && i && i % (P->size / 20) == 0)
      fprintf(stderr, "%2.0f%%\t%.5f\n", (100.*i)/P->size, _bytes_output*2./(i+1));
    if(P->progress && !P->extended) 
      Progress(P->size, i);
    #endif
    }

  WriteNBits(m, 8, OUT);
  for(n = 0 ; n < m ; ++n)
    WriteNBits(S2N(t[n]), 8, OUT);        // ENCODE REMAINING SYMBOLS

  fprintf(stdout, "Compressed %"PRIu64" sym to %"PRIu64" bytes [ %.4g bps "
		  "; %.3g ratio ; %.4g NC ]\n", P->length, (uint64_t) 
		  _bytes_output, (double) _bytes_output*8.0 / P->length, 
		  (double) P->length / _bytes_output, _bytes_output * 4.0 /
		  (double) P->length);

  finish_encode(OUT);
  doneoutputtingbits(OUT);

  fclose(IN);
  fclose(OUT);
  }
  
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// COMPRESSION NO NN
// 
void CompressNoNN(PARAM *P, char *fn)
  {
  FILE      *IN  = Fopen(fn, "r"), *OUT = Fopen(P->output, "w");
  uint64_t  i = 0, mSize = MAX_BUF, pos = 0, r = 0;
  uint32_t  m, n, q, j, c;
  uint8_t   t[NSYM], *buf = (uint8_t *) Calloc(mSize, sizeof(uint8_t)), sym = 0, 
            *p; 

  RCLASS    **RC;
  CMODEL    **CM;
  PMODEL    **PM;
  PMODEL    *MX_CM;
  PMODEL    **MX_RM;
  FPMODEL   *PT;
  CMWEIGHT  *WM;
  CBUF      *SB;

  srand(P->seed);

  if(P->verbose)
    fprintf(stderr, "Analyzing data and creating models ...\n");
 
  if(P->nCModels + P->nRMClasses < 1){
    fprintf(stderr, "Error: At least one model must be set!\n");
    exit(1);    
    }

  #ifdef ESTIMATE
  FILE *IAE = NULL;
  char *IAEName = NULL;
  if(P->estim == 1){
    IAEName = Cat(fn, ".info");
    IAE = Fopen(IAEName, "w");
    }
  #endif

  // EXTRA MODELS DERIVED FROM TOLERANT CONTEXT MODELS
  P->nCPModels = P->nCModels;
  for(n = 0 ; n < P->nCModels ; ++n)
    if(P->cmodel[n].edits != 0)
      P->nCPModels += 1;
  P->nCPModels += P->nRMClasses; // FOR MIXING

  int nmodels = P->nCPModels + 1;
  float **probs = calloc(nmodels, sizeof(float *));
  for(n = 0 ; n < nmodels ; ++n)
    probs[n] = calloc(NSYM, sizeof(float));
  long **freqs = calloc(P->nCPModels, sizeof(long *));
  for(n = 0 ; n < P->nCPModels ; ++n)
    freqs[n] = calloc(NSYM, sizeof(long));
  long *sums = calloc(P->nCPModels, sizeof(long));

  PM      = (PMODEL **) Calloc(P->nCPModels, sizeof(PMODEL *));
  for(n = 0 ; n < P->nCPModels ; ++n)
    PM[n] = CreatePModel(NSYM);
  MX_RM   = (PMODEL **) Calloc(P->nRMClasses, sizeof(PMODEL *));
  for(n = 0 ; n < P->nRMClasses ; ++n)
    MX_RM[n] = CreatePModel(NSYM);
  MX_CM   = CreatePModel(NSYM);
  PT      = CreateFloatPModel(NSYM);
  WM      = CreateWeightModel(P->nCPModels);
  SB      = CreateCBuffer(BUFFER_SIZE, BGUARD);

  CM = (CMODEL **) Malloc(P->nCModels * sizeof(CMODEL *));
  for(n = 0, r = 0; n < P->nCModels ; ++n){
    CM[n] = CreateCModel(P->cmodel[n].ctx,   P->cmodel[n].den,  1,
                         P->cmodel[n].edits, P->cmodel[n].eDen, NSYM,
                         P->cmodel[n].gamma, P->cmodel[n].eGamma,
                         P->cmodel[n].ir,    P->cmodel[n].eIr);
    WM->gamma[r++] = CM[n]->gamma;
    if(CM[n]->edits != 0)
      WM->gamma[r++] = CM[n]->eGamma;
    }

  RC = (RCLASS **) Malloc(P->nRMClasses * sizeof(RCLASS *));
  for(n = 0 ; n < P->nRMClasses ; ++n){
    RC[n] = CreateRC(P->rmodel[n].nr, P->rmodel[n].beta,  
            P->rmodel[n].limit, P->rmodel[n].ctx, P->rmodel[n].gamma,
            P->rmodel[n].ir, P->rmodel[n].weight, P->rmodel[n].cache);
    }

  P->length = NBytesInFile(IN);

  if(P->length > 4294967295){
    fprintf(stderr, "Error: DNA sequence larger than 2^32.\n");
    fprintf(stderr, "Tip: Use split to separate data into buckets...\n");
    exit(1);
    }

  P->size = P->length>>2;

  if(P->verbose){
    fprintf(stderr, "Compressing %"PRIu64" symbols ...\n", P->length);
    }

  startoutputtingbits();
  start_encode();
  EncodeHeader(P, RC, CM, OUT);

  while((m = fread(t, sizeof(uint8_t), NSYM, IN)) == NSYM)
    {
    if(!strchr("ACGT", t[0])) ErrAndExit();
    if(!strchr("ACGT", t[1])) ErrAndExit();
    if(!strchr("ACGT", t[2])) ErrAndExit();
    if(!strchr("ACGT", t[3])) ErrAndExit();

    buf[i] = S2N(t[3])|(S2N(t[2])<<2)|(S2N(t[1])<<4)|(S2N(t[0])<<6); // PACK 4

    for(n = 0 ; n < m ; ++n)
      {
      SB->buf[SB->idx] = sym = S2N(t[n]);

      memset((void *)PT->freqs, 0, NSYM * sizeof(double));
      p = &SB->buf[SB->idx-1];

      c = 0;
      for(r = 0 ; r < P->nCModels ; ++r)       // FOR ALL CMODELS
	{
        CMODEL *FCM = CM[r];
        GetPModelIdx(p, FCM);
        ComputePModel(FCM, PM[c], FCM->pModelIdx, FCM->alphaDen, 
	freqs[c], &sums[c]);
        ComputeWeightedFreqs(WM->weight[c], PM[c], PT, NSYM);
        if(FCM->edits != 0)
	  {
	  ++c;
          FCM->TM->seq->buf[FCM->TM->seq->idx] = sym;
          FCM->TM->idx = GetPModelIdxCorr(FCM->TM->seq->buf+
          FCM->TM->seq->idx-1, FCM, FCM->TM->idx);
          ComputePModel(FCM, PM[c], FCM->TM->idx, FCM->TM->den, 
	  freqs[c], &sums[c]);
          ComputeWeightedFreqs(WM->weight[c], PM[c], PT, FCM->nSym);
          }
        ++c;
        }

      for(r = 0 ; r < P->nRMClasses ; ++r)              // FOR ALL REPEAT MODELS
	{
        StopRM           (RC[r]);
        StartMultipleRMs (RC[r], p);
        AddKmerPos       (RC[r]->T, RC[r]->P->idx, pos);       // pos = (i<<2)+n
	RenormWeights    (RC[r]);
        ComputeMixture   (RC[r], MX_RM[r], buf);
	}

      for(j = c, q = 0 ; j < c + P->nRMClasses ; ++j, ++q)      // FOR * RMODELS
        { 
        PM[j]->sum = 0;
        for(r = 0 ; r < NSYM ; ++r)
	  {
          PM[j]->freqs[r] = MX_RM[q]->freqs[r];
	  freqs[j][r] = PM[j]->freqs[r];
	  }
	sums[j] = MX_RM[q]->sum;
        PM[j]->sum = MX_RM[q]->sum;
        ComputeWeightedFreqs(WM->weight[j], PM[j], PT, NSYM);
        }

      ComputeMXProbs(PT, MX_CM, NSYM);

      ++pos;

      AESym(sym, (int *) (MX_CM->freqs), (int) MX_CM->sum, OUT);
      #ifdef ESTIMATE
      if(P->estim != 0)
        fprintf(IAE, "%.3g\n", PModelNats(MX_CM, sym) / M_LN2);
      #endif

      CalcDecayment(WM, PM, sym);
      UpdateCModels(CM, SB, sym, P->nCModels);
      RenormalizeWeights(WM);

      for(r = 0, c = 0 ; r < P->nCModels ; ++r, ++c)
        if(CM[r]->edits != 0)
          UpdateTolerantModel(CM[r]->TM, PM[++c], sym);

      for(r = 0 ; r < P->nRMClasses ; ++r)
        UpdateWeights(RC[r], buf, sym);

      UpdateCBuffer(SB);
      }

    if(++i == mSize)    // REALLOC BUFFER ON OVERFLOW 4 STORE THE COMPLETE SEQ
      buf = (uint8_t *) Realloc(buf, (mSize+=ADD_SPACE) * sizeof(uint8_t));

    #ifdef PROGRESS
    if(P->extended && i && i % (P->size / 20) == 0)
      fprintf(stderr, "%2.0f%%\t%.5f\n", (100.*i)/P->size, _bytes_output*2./(i+1));
    if(P->progress && !P->extended)
      Progress(P->size, i);
    #endif
    }

  WriteNBits(m, 8, OUT);
  for(n = 0 ; n < m ; ++n)
    WriteNBits(S2N(t[n]), 8, OUT);        // ENCODE REMAINING SYMBOLS

  fprintf(stdout, "Compressed %"PRIu64" sym to %"PRIu64" bytes [ %.4g bps "
                  "; %.3g ratio ; %.4g NC ]\n", P->length, (uint64_t)
                  _bytes_output, (double) _bytes_output*8.0 / P->length,
                  (double) P->length / _bytes_output, _bytes_output * 4.0 /
                  (double) P->length);

  finish_encode(OUT);
  doneoutputtingbits(OUT);

  #ifdef ESTIMATE
  if(P->estim == 1){
    fclose(IAE);
    Free(IAEName);
    }
  #endif

  for (n = 0; n < P->nCPModels; ++n)
    free(freqs[n]);
  free(freqs);
  for(n = 0; n < nmodels; ++n)
    free(probs[n]);
  free(probs);

  fclose(IN);
  fclose(OUT);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// COMPRESSION
//
void Compress(PARAM *P, char *fn){
  FILE      *IN  = Fopen(fn, "r"), *OUT = Fopen(P->output, "w");
  uint64_t  i = 0, mSize = MAX_BUF, pos = 0, r = 0;
  uint32_t  m, n, q, j, c;
  uint8_t   t[NSYM], *buf = (uint8_t *) Calloc(mSize, sizeof(uint8_t)), sym = 0, 
            *p; 

  RCLASS    **RC;
  CMODEL    **CM;
  PMODEL    **PM;
  PMODEL    *MX_CM;
  PMODEL    **MX_RM;
  FPMODEL   *PT;
  CMWEIGHT  *WM;
  CBUF      *SB;

  srand(P->seed);

  if(P->verbose)
    fprintf(stderr, "Analyzing data and creating models ...\n");
 
  if(P->nCModels + P->nRMClasses < 1){
    fprintf(stderr, "Error: At least one model must be set!\n");
    exit(1);    
    }

  #ifdef ESTIMATE
  FILE *IAE = NULL;
  char *IAEName = NULL;
  if(P->estim == 1){
    IAEName = Cat(fn, ".info");
    IAE = Fopen(IAEName, "w");
    }
  #endif

  // EXTRA MODELS DERIVED FROM TOLERANT CONTEXT MODELS
  P->nCPModels = P->nCModels;
  for(n = 0 ; n < P->nCModels ; ++n)
    if(P->cmodel[n].edits != 0)
      P->nCPModels += 1;
  P->nCPModels += P->nRMClasses; // FOR MIXING

  // NEURAL NETWORK INITIALIZATION
  int nmodels = P->nCPModels + 1;
  float **probs = calloc(nmodels, sizeof(float *));
  for(n = 0 ; n < nmodels ; ++n)
    probs[n] = calloc(NSYM, sizeof(float));
  long **freqs = calloc(P->nCPModels, sizeof(long *));
  for(n = 0 ; n < P->nCPModels ; ++n)
    freqs[n] = calloc(NSYM, sizeof(long));
  long *sums = calloc(P->nCPModels, sizeof(long));
  mix_state_t *mxs = mix_init(nmodels, NSYM, P->hs);

  PM      = (PMODEL **) Calloc(P->nCPModels, sizeof(PMODEL *));
  for(n = 0 ; n < P->nCPModels ; ++n)
    PM[n] = CreatePModel(NSYM);
  MX_RM   = (PMODEL **) Calloc(P->nRMClasses, sizeof(PMODEL *));
  for(n = 0 ; n < P->nRMClasses ; ++n)
    MX_RM[n] = CreatePModel(NSYM);
  MX_CM   = CreatePModel(NSYM);
  PT      = CreateFloatPModel(NSYM);
  WM      = CreateWeightModel(P->nCPModels);
  SB      = CreateCBuffer(BUFFER_SIZE, BGUARD);

  CM = (CMODEL **) Malloc(P->nCModels * sizeof(CMODEL *));
  for(n = 0, r = 0; n < P->nCModels ; ++n){
    CM[n] = CreateCModel(P->cmodel[n].ctx,   P->cmodel[n].den,  1,
                         P->cmodel[n].edits, P->cmodel[n].eDen, NSYM,
                         P->cmodel[n].gamma, P->cmodel[n].eGamma,
                         P->cmodel[n].ir,    P->cmodel[n].eIr);
    // GIVE SPECIFIC GAMMA TO EACH MODEL:
    WM->gamma[r++] = CM[n]->gamma;
    if(CM[n]->edits != 0)
      WM->gamma[r++] = CM[n]->eGamma;
    }

  RC = (RCLASS **) Malloc(P->nRMClasses * sizeof(RCLASS *));
  for(n = 0 ; n < P->nRMClasses ; ++n){
    RC[n] = CreateRC(P->rmodel[n].nr, P->rmodel[n].beta,  
            P->rmodel[n].limit, P->rmodel[n].ctx, P->rmodel[n].gamma,
            P->rmodel[n].ir, P->rmodel[n].weight, P->rmodel[n].cache);
    }

  P->length = NBytesInFile(IN);

  if(P->length > 4294967295){
    fprintf(stderr, "Error: DNA sequence larger than 2^32.\n");
    fprintf(stderr, "Tip: Use split to separate data into buckets...\n");
    exit(1);
    }

  P->size = P->length>>2;

  if(P->verbose){
    fprintf(stderr, "Compressing %"PRIu64" symbols ...\n", P->length);
    }

  startoutputtingbits();
  start_encode();
  EncodeHeader(P, RC, CM, OUT);

  while((m = fread(t, sizeof(uint8_t), NSYM, IN)) == NSYM)
    {
    if(!strchr("ACGT", t[0])) ErrAndExit();
    if(!strchr("ACGT", t[1])) ErrAndExit();
    if(!strchr("ACGT", t[2])) ErrAndExit();
    if(!strchr("ACGT", t[3])) ErrAndExit();
    
    buf[i] = S2N(t[3])|(S2N(t[2])<<2)|(S2N(t[1])<<4)|(S2N(t[0])<<6); // PACK 4

    for(n = 0 ; n < m ; ++n)
      {
      SB->buf[SB->idx] = sym = S2N(t[n]);

      memset((void *)PT->freqs, 0, NSYM * sizeof(double));
      p = &SB->buf[SB->idx-1];

      c = 0;
      for(r = 0 ; r < P->nCModels ; ++r)       // FOR ALL CMODELS
	{
        CMODEL *FCM = CM[r];
        GetPModelIdx(p, FCM);
        ComputePModel(FCM, PM[c], FCM->pModelIdx, FCM->alphaDen, 
	freqs[c], &sums[c]);
        ComputeWeightedFreqs(WM->weight[c], PM[c], PT, NSYM);
        if(FCM->edits != 0)
	  {
	  ++c;
          FCM->TM->seq->buf[FCM->TM->seq->idx] = sym;
          FCM->TM->idx = GetPModelIdxCorr(FCM->TM->seq->buf+
          FCM->TM->seq->idx-1, FCM, FCM->TM->idx);
          ComputePModel(FCM, PM[c], FCM->TM->idx, FCM->TM->den, 
	  freqs[c], &sums[c]);
          ComputeWeightedFreqs(WM->weight[c], PM[c], PT, FCM->nSym);
          }
        ++c;
        }

      for(r = 0 ; r < P->nRMClasses ; ++r)              // FOR ALL REPEAT MODELS
	{
        StopRM           (RC[r]);
        StartMultipleRMs (RC[r], p);
        AddKmerPos       (RC[r]->T, RC[r]->P->idx, pos);       // pos = (i<<2)+n
	RenormWeights    (RC[r]);
        ComputeMixture   (RC[r], MX_RM[r], buf);
	}

      // PASS MX_RM[q] AS LAST MODEL AND SET IT AS PM[c]
      for(j = c, q = 0 ; j < c + P->nRMClasses ; ++j, ++q)
        {     // FOR * RMODELS
        PM[j]->sum = 0;
        for(r = 0 ; r < NSYM ; ++r)
	  {
          PM[j]->freqs[r] = MX_RM[q]->freqs[r];
	  freqs[j][r] = PM[j]->freqs[r];
	  }
	sums[j] = MX_RM[q]->sum;
        PM[j]->sum = MX_RM[q]->sum;
        ComputeWeightedFreqs(WM->weight[j], PM[j], PT, NSYM);
        }
     
      // FILL PROBABILITIES FOR ALL MODELS FOR NEURAL NETWORK
      for(q = 0 ; q < P->nCPModels ; ++q)
        for(j = 0 ; j < NSYM ; ++j)
          probs[q][j] = (float)freqs[q][j]/sums[q];
      for(j = 0 ; j < NSYM ; ++j)
        probs[P->nCPModels][j] = PT->freqs[j];
      const float *y = mix(mxs, probs);
      for(q = 0 ; q < NSYM ; ++q)
        PT->freqs[q] = y[q];

      ComputeMXProbs(PT, MX_CM, NSYM);

      mix_update_state(mxs, probs, sym, P->lr);

      ++pos;

      AESym(sym, (int *) (MX_CM->freqs), (int) MX_CM->sum, OUT);
      #ifdef ESTIMATE
      if(P->estim != 0)
        fprintf(IAE, "%.3g\n", PModelNats(MX_CM, sym) / M_LN2);
      #endif

      CalcDecayment(WM, PM, sym);

      UpdateCModels(CM, SB, sym, P->nCModels);

      RenormalizeWeights(WM);

      for(r = 0, c = 0 ; r < P->nCModels ; ++r, ++c)
        if(CM[r]->edits != 0)
          UpdateTolerantModel(CM[r]->TM, PM[++c], sym);

      for(r = 0 ; r < P->nRMClasses ; ++r)
        UpdateWeights(RC[r], buf, sym);

      UpdateCBuffer(SB);
      }

    if(++i == mSize)    // REALLOC BUFFER ON OVERFLOW 4 STORE THE COMPLETE SEQ
      buf = (uint8_t *) Realloc(buf, (mSize+=ADD_SPACE) * sizeof(uint8_t));

    #ifdef PROGRESS
    if(P->extended && i && i % (P->size / 20) == 0)
      fprintf(stderr, "%2.0f%%\t%.5f\n", (100.*i)/P->size, _bytes_output*2./(i+1));
    if(P->progress && !P->extended)
      Progress(P->size, i);
    #endif
    }

  WriteNBits(m, 8, OUT);
  for(n = 0 ; n < m ; ++n)
    WriteNBits(S2N(t[n]), 8, OUT);        // ENCODE REMAINING SYMBOLS

  fprintf(stdout, "Compressed %"PRIu64" sym to %"PRIu64" bytes [ %.4g bps "
                  "; %.3g ratio ; %.4g NC ]\n", P->length, (uint64_t)
                  _bytes_output, (double) _bytes_output*8.0 / P->length,
                  (double) P->length / _bytes_output, _bytes_output * 4.0 /
                  (double) P->length);

  finish_encode(OUT);
  doneoutputtingbits(OUT);

  #ifdef ESTIMATE
  if(P->estim == 1){
    fclose(IAE);
    Free(IAEName);
    }
  #endif

  mix_free(mxs);
  free(sums);
  for (n = 0; n < P->nCPModels; ++n)
    free(freqs[n]);
  free(freqs);
  for(n = 0; n < nmodels; ++n)
    free(probs[n]);
  free(probs);

  fclose(IN);
  fclose(OUT);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DECOMPRESSION
//
void Decompress(char *outname, char progress, char *fn)
  {
  FILE     *IN  = Fopen(fn, "r"), *OUT = Fopen(outname, "w");
  uint64_t i = 0, mSize = MAX_BUF, pos = 0;
  uint32_t m, n, j, q, r, c;
  uint8_t  *buf = (uint8_t *) Calloc(mSize, sizeof(uint8_t)), sym = 0, *p;
  RCLASS   **RC = NULL; 
  CMODEL   **CM = NULL;
  PARAM    *P = (PARAM *) Calloc(1, sizeof(PARAM));
  PMODEL   **PM;
  PMODEL   *MX_CM;
  FPMODEL  *PT;
  CMWEIGHT *WM;
  CBUF     *SB;

  startinputtingbits();
  start_decode(IN);

  P->progress  = progress;
  P->datatype  = ReadNBits(                 DATA_TYPE_BITS, IN);
  P->seed      = ReadNBits(                      SEED_BITS, IN);
  P->size      = ReadNBits(                      SIZE_BITS, IN);
  P->length    = ReadNBits(                    LENGTH_BITS, IN);
  P->hs        = ReadNBits(                        HS_BITS, IN);
  P->lr        = ReadNBits(                        LR_BITS, IN) / 65534.0;
  P->nCModels  = ReadNBits(                  NCMODELS_BITS, IN);
  CM = (CMODEL **) Malloc(P->nCModels * sizeof(CMODEL *));
  for(n = 0 ; n < P->nCModels ; ++n){
    uint32_t c = ReadNBits(                       CTX_BITS, IN);
    uint32_t a = ReadNBits(                 ALPHA_DEN_BITS, IN);
    double   g = ReadNBits(                     GAMMA_BITS, IN) / 65534.0;
    uint32_t i = ReadNBits(                        IR_BITS, IN);
    uint32_t e = ReadNBits(                     EDITS_BITS, IN);
    uint32_t d = 0, y = 0;
    double   b = 0;
    if(e != 0){
      b = ReadNBits(                          E_GAMMA_BITS, IN) / 65534.0;
      d = ReadNBits(                            E_DEN_BITS, IN);
      y = ReadNBits(                               IR_BITS, IN);
      }
    CM[n] = CreateCModel(c, a, 1, e, d, NSYM, g, b, i, y);
    }
  P->nCPModels = ReadNBits(                  NCMODELS_BITS, IN);

  P->nRMClasses  = ReadNBits(              NRMCLASSES_BITS, IN);
  RC = (RCLASS **) Malloc(P->nRMClasses * sizeof(RCLASS *));
  for(n = 0 ; n < P->nRMClasses ; ++n){
    uint32_t  m = ReadNBits(              MAX_RMODELS_BITS, IN);
    double    b = ReadNBits(                     BETA_BITS, IN) / 65534.0;
    double    g = ReadNBits(                    GAMMA_BITS, IN) / 65534.0;
    double    w = ReadNBits(                   WEIGHT_BITS, IN) / 65534.0;
    uint32_t  l = ReadNBits(                    LIMIT_BITS, IN);
    uint32_t  c = ReadNBits(                      CTX_BITS, IN);
    uint8_t   r = ReadNBits(                       IR_BITS, IN);
    uint64_t  s = ReadNBits(                    CACHE_BITS, IN);
    RC[n] = CreateRC(m, b, l, c, g, r, w, s);
    }
  
  srand(P->seed);

  #ifdef DEBUG
  printf("datatype = %u\n", P->datatype);
  printf("seed     = %"PRIu32"\n", P->seed);
  printf("size     = %"PRIu64"\n", P->size);
  printf("length   = %"PRIu64"\n", P->length);
  printf("hs       = %u\n",        P->hs);
  printf("lr       = %g\n",        P->lr);
  printf("n CMs    = %u\n",        P->nCModels);
  printf("n CPMs   = %u\n",        P->nCPModels);
  for(n = 0 ; n < P->nCModels ; ++n){
    printf("  cmodel %u\n",        n + 1);
    printf("    ctx       = %u\n", CM[n]->ctx);
    printf("    alpha den = %u\n", CM[n]->alphaDen);
    printf("    gamma     = %g\n", CM[n]->gamma);
    printf("    ir        = %u\n", CM[n]->ir);
    printf("    edits     = %u\n", CM[n]->edits);
    if(CM[n]->edits != 0){
      printf("      eGamma = %g\n", CM[n]->eGamma);
      printf("      eDen   = %u\n", CM[n]->TM->den);
      printf("      eIr    = %u\n", CM[n]->TM->ir);
      }
    }
  printf("n class = %u\n",        P->nRMClasses);
  for(n = 0 ; n < P->nRMClasses ; ++n){
    printf("  class %u\n",              n + 1);
    printf("    max rep = %u\n",        RC[n]->mRM);
    printf("    beta    = %g\n",        RC[n]->P->beta);
    printf("    gamma   = %g\n",        RC[n]->P->gamma);
    printf("    weight  = %g\n",        RC[n]->P->iWeight);
    printf("    limit   = %u\n",        RC[n]->P->limit);
    printf("    ctx     = %u\n",        RC[n]->P->ctx);
    printf("    ir      = %u\n",        RC[n]->P->rev);
    printf("    cache   = %"PRIu64"\n", RC[n]->P->c_max);
    }
  #endif
    
  if(P->nCModels == 0 && P->nRMClasses == 1)
    {
    PMODEL *MX_RM = CreatePModel(NSYM);
    SB = CreateCBuffer(BUFFER_SIZE, BGUARD);

    while(i < P->size)
      {                                       // NOT absolute size (CHAR SIZE)
      for(n = 0 ; n < NSYM ; ++n)
        {
        p = &SB->buf[SB->idx-1];

        StopRM           (RC[0]);
        StartMultipleRMs (RC[0], p);
        AddKmerPos       (RC[0]->T, RC[0]->P->idx, pos);     // pos = (i<<2)+n
        RenormWeights    (RC[0]);
        ComputeMixture   (RC[0], MX_RM, buf);

        ++pos;
        sym = ArithDecodeSymbol(4, (int *) MX_RM->freqs, (int) MX_RM->sum, IN);
        SB->buf[SB->idx] = sym;
      
        if(n == 0) buf[i] = sym<<6; 
	else       buf[i] |= (sym<<((3-n)<<1));
        fputc(N2S(sym), OUT);

        UpdateWeights(RC[0], buf, sym);
        UpdateCBuffer(SB);
        }

      if(++i == mSize) // REALLOC BUFFER ON OVERFLOW 4 STORE THE COMPLETE SEQ
        buf = (uint8_t *) Realloc(buf, (mSize+=ADD_SPACE) * sizeof(uint8_t));

      #ifdef PROGRESS
      if(P->progress) Progress(P->size, i);
      #endif
      }

    m = ReadNBits(8, IN);
    for(n = 0 ; n < m ; ++n)
      fputc(N2S(ReadNBits(8, IN)), OUT);    // DECODE REMAINING SYMBOLS

    finish_decode();
    doneinputtingbits();
    }
  else
    {
    if(P->lr == 0)
      {
      PM = (PMODEL **) Calloc(P->nCPModels, sizeof(PMODEL *));
      for(n = 0 ; n < P->nCPModels ; ++n)
        PM[n] = CreatePModel(NSYM);
      PMODEL **MX_RM   = (PMODEL **) Calloc(P->nRMClasses, sizeof(PMODEL *));
      for(n = 0 ; n < P->nRMClasses ; ++n)
        MX_RM[n] = CreatePModel(NSYM);
      MX_CM   = CreatePModel(NSYM);
      PT      = CreateFloatPModel(NSYM);
      WM      = CreateWeightModel(P->nCPModels);
      SB      = CreateCBuffer(BUFFER_SIZE, BGUARD);

      // GIVE SPECIFIC GAMMA TO EACH MODEL:
      for(n = 0, r = 0 ; n < P->nCModels ; ++n){
        WM->gamma[r++] = CM[n]->gamma;
        if(CM[n]->edits != 0)
          WM->gamma[r++] = CM[n]->eGamma;
        }

      // NEURAL NETWORK INITIALIZATION
      int nmodels = P->nCPModels + 1;
      float **probs = calloc(nmodels, sizeof(float *));
      for(n = 0 ; n < nmodels ; ++n)
        probs[n] = calloc(NSYM, sizeof(float));
      long **freqs = calloc(P->nCPModels, sizeof(long *));
      for(n = 0 ; n < P->nCPModels ; ++n)
        freqs[n] = calloc(NSYM, sizeof(long));
      long *sums = calloc(P->nCPModels, sizeof(long));

      while(i < P->size)
        {                                       // NOT absolute size (CHAR SIZE)
        for(n = 0 ; n < NSYM ; ++n)
          {
          memset((void *)PT->freqs, 0, NSYM * sizeof(double));
          p = &SB->buf[SB->idx-1];

          c = 0;
          for(r = 0 ; r < P->nCModels ; ++r)                  // FOR ALL CMODELS
            {
            CMODEL *FCM = CM[r];
            GetPModelIdx(p, FCM);
            ComputePModel(FCM, PM[c], FCM->pModelIdx, FCM->alphaDen, 
            freqs[c], &sums[c]);
            ComputeWeightedFreqs(WM->weight[c], PM[c], PT, NSYM);
            if(FCM->edits != 0)
	      {
	      ++c;
              FCM->TM->idx = GetPModelIdxCorr(FCM->TM->seq->buf+
              FCM->TM->seq->idx-1, FCM, FCM->TM->idx);
              ComputePModel(FCM, PM[c], FCM->TM->idx, FCM->TM->den, 
	      freqs[c], &sums[c]);
              ComputeWeightedFreqs(WM->weight[c], PM[c], PT, FCM->nSym);
              }
	    ++c;
            }

          for(r = 0 ; r < P->nRMClasses ; ++r)
            {
            StopRM           (RC[r]);
            StartMultipleRMs (RC[r], p);
            AddKmerPos       (RC[r]->T, RC[r]->P->idx, pos);   // pos = (i<<2)+n
            RenormWeights    (RC[r]);
            ComputeMixture   (RC[r], MX_RM[r], buf);
            }

          // PASS MX_RM AS LAST MODEL AND SET IT AS PM[c]
          for(j = c, q = 0 ; j < c + P->nRMClasses ; ++j, ++q)
	    {     // FOR ALL RMs
            PM[j]->sum = 0;
            for(r = 0 ; r < NSYM ; ++r)
	      {
              PM[j]->freqs[r] = MX_RM[q]->freqs[r];
	      freqs[j][r] = PM[j]->freqs[r];
	      }
            sums[j] = MX_RM[q]->sum;
            PM[j]->sum = MX_RM[q]->sum;
            ComputeWeightedFreqs(WM->weight[j], PM[j], PT, NSYM);
            }

          ComputeMXProbs(PT, MX_CM, NSYM);

          ++pos;

          SB->buf[SB->idx] = sym = ArithDecodeSymbol(NSYM, 
          (int *)MX_CM->freqs, (int)MX_CM->sum, IN);

          if(n == 0) buf[i] = sym<<6 ; else buf[i] |= (sym<<((3-n)<<1));
          fputc(N2S(sym), OUT);

          for(r = 0 ; r < P->nCModels ; ++r)
            if(CM[r]->edits != 0)
              CM[r]->TM->seq->buf[CM[r]->TM->seq->idx] = sym;

          CalcDecayment(WM, PM, sym);

          UpdateCModels(CM, SB, sym, P->nCModels);
      
          RenormalizeWeights(WM);

          for(r = 0, c = 0 ; r < P->nCModels ; ++r, ++c)
            if(CM[r]->edits != 0)
              UpdateTolerantModel(CM[r]->TM, PM[++c], sym);

          for(r = 0 ; r < P->nRMClasses ; ++r)
            UpdateWeights(RC[r], buf, sym);

          UpdateCBuffer(SB);
          }

        if(++i == mSize) // REALLOC BUFFER ON OVERFLOW 4 STORE THE COMPLETE SEQ
          buf = (uint8_t *) Realloc(buf, (mSize+=ADD_SPACE) * sizeof(uint8_t));

        #ifdef PROGRESS
        if(P->progress) Progress(P->size, i);
        #endif
        }

      m = ReadNBits(8, IN);
      for(n = 0 ; n < m ; ++n)
        fputc(N2S(ReadNBits(8, IN)), OUT);    // DECODE REMAINING SYMBOLS

      finish_decode();
      doneinputtingbits();

      free(sums);
      for (n = 0; n < P->nCPModels; ++n)
        free(freqs[n]);
      free(freqs);
      for(n = 0; n < nmodels; ++n)
        free(probs[n]);
      free(probs);
      }
    else
      {  
      PM = (PMODEL **) Calloc(P->nCPModels, sizeof(PMODEL *));
      for(n = 0 ; n < P->nCPModels ; ++n)
        PM[n] = CreatePModel(NSYM);
      PMODEL **MX_RM   = (PMODEL **) Calloc(P->nRMClasses, sizeof(PMODEL *));
      for(n = 0 ; n < P->nRMClasses ; ++n)
        MX_RM[n] = CreatePModel(NSYM);
      MX_CM   = CreatePModel(NSYM);
      PT      = CreateFloatPModel(NSYM);
      WM      = CreateWeightModel(P->nCPModels);
      SB      = CreateCBuffer(BUFFER_SIZE, BGUARD);

      // GIVE SPECIFIC GAMMA TO EACH MODEL:
      for(n = 0, r = 0 ; n < P->nCModels ; ++n){
        WM->gamma[r++] = CM[n]->gamma;
        if(CM[n]->edits != 0)
          WM->gamma[r++] = CM[n]->eGamma;
        }

      // NEURAL NETWORK INITIALIZATION
      int nmodels = P->nCPModels + 1;
      float **probs = calloc(nmodels, sizeof(float *));
      for(n = 0 ; n < nmodels ; ++n)
        probs[n] = calloc(NSYM, sizeof(float));
      long **freqs = calloc(P->nCPModels, sizeof(long *));
      for(n = 0 ; n < P->nCPModels ; ++n)
        freqs[n] = calloc(NSYM, sizeof(long));
      long *sums = calloc(P->nCPModels, sizeof(long));
      mix_state_t *mxs = mix_init(nmodels, NSYM, P->hs);

      while(i < P->size)
        {                                       // NOT absolute size (CHAR SIZE)
        for(n = 0 ; n < NSYM ; ++n)
          {
          memset((void *)PT->freqs, 0, NSYM * sizeof(double));
          p = &SB->buf[SB->idx-1];

          c = 0;
          for(r = 0 ; r < P->nCModels ; ++r)                  // FOR ALL CMODELS
            {
            CMODEL *FCM = CM[r];
            GetPModelIdx(p, FCM);
            ComputePModel(FCM, PM[c], FCM->pModelIdx, FCM->alphaDen, 
            freqs[c], &sums[c]);
            ComputeWeightedFreqs(WM->weight[c], PM[c], PT, NSYM);
            if(FCM->edits != 0)
	      {
	      ++c;
              FCM->TM->idx = GetPModelIdxCorr(FCM->TM->seq->buf+
              FCM->TM->seq->idx-1, FCM, FCM->TM->idx);
              ComputePModel(FCM, PM[c], FCM->TM->idx, FCM->TM->den, 
	      freqs[c], &sums[c]);
              ComputeWeightedFreqs(WM->weight[c], PM[c], PT, FCM->nSym);
              }
	    ++c;
            }

          for(r = 0 ; r < P->nRMClasses ; ++r)
            {
            StopRM           (RC[r]);
            StartMultipleRMs (RC[r], p);
            AddKmerPos       (RC[r]->T, RC[r]->P->idx, pos);   // pos = (i<<2)+n
            RenormWeights    (RC[r]);
            ComputeMixture   (RC[r], MX_RM[r], buf);
            }

          // PASS MX_RM AS LAST MODEL AND SET IT AS PM[c]
          for(j = c, q = 0 ; j < c + P->nRMClasses ; ++j, ++q)
	    {     // FOR ALL RMs
            PM[j]->sum = 0;
            for(r = 0 ; r < NSYM ; ++r)
	      {
              PM[j]->freqs[r] = MX_RM[q]->freqs[r];
	      freqs[j][r] = PM[j]->freqs[r];
	      }
            sums[j] = MX_RM[q]->sum;
            PM[j]->sum = MX_RM[q]->sum;
            ComputeWeightedFreqs(WM->weight[j], PM[j], PT, NSYM);
            }

          // FILL PROBABILITIES FOR ALL MODELS FOR NEURAL NETWORK
          for(q = 0 ; q < P->nCPModels ; ++q)
            for(j = 0 ; j < NSYM ; ++j)
              probs[q][j] = (float)freqs[q][j]/sums[q];
          for(j = 0 ; j < NSYM ; ++j)
            probs[P->nCPModels][j] = PT->freqs[j];
          const float *y = mix(mxs, probs);
          for(q = 0 ; q < NSYM ; ++q)
            PT->freqs[q] = y[q];

          ComputeMXProbs(PT, MX_CM, NSYM);

          ++pos;

          SB->buf[SB->idx] = sym = ArithDecodeSymbol(NSYM, 
          (int *)MX_CM->freqs, (int)MX_CM->sum, IN);
      
          mix_update_state(mxs, probs, sym, P->lr);

          if(n == 0) buf[i] = sym<<6 ; else buf[i] |= (sym<<((3-n)<<1));
          fputc(N2S(sym), OUT);

          for(r = 0 ; r < P->nCModels ; ++r)
            if(CM[r]->edits != 0)
              CM[r]->TM->seq->buf[CM[r]->TM->seq->idx] = sym;

          CalcDecayment(WM, PM, sym);

          UpdateCModels(CM, SB, sym, P->nCModels);
      
          RenormalizeWeights(WM);

          for(r = 0, c = 0 ; r < P->nCModels ; ++r, ++c)
            if(CM[r]->edits != 0)
              UpdateTolerantModel(CM[r]->TM, PM[++c], sym);

          for(r = 0 ; r < P->nRMClasses ; ++r)
            UpdateWeights(RC[r], buf, sym);

          UpdateCBuffer(SB);
          }

        if(++i == mSize) // REALLOC BUFFER ON OVERFLOW 4 STORE THE COMPLETE SEQ
          buf = (uint8_t *) Realloc(buf, (mSize+=ADD_SPACE) * sizeof(uint8_t));

        #ifdef PROGRESS 
        if(P->progress) Progress(P->size, i);
        #endif
        }

      m = ReadNBits(8, IN);
      for(n = 0 ; n < m ; ++n)
        fputc(N2S(ReadNBits(8, IN)), OUT);    // DECODE REMAINING SYMBOLS

      finish_decode();
      doneinputtingbits();

      mix_free(mxs);
      free(sums);
      for (n = 0; n < P->nCPModels; ++n)
        free(freqs[n]);
      free(freqs);
      for(n = 0; n < nmodels; ++n)
        free(probs[n]);
      free(probs);
      }
    }

  fclose(IN);
  fclose(OUT);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// MAIN 
//
int main(int argc, char **argv)
  {
  char       **p = *&argv, **xargv, *xpl = NULL;
  int32_t    n, k, xargc = 0;
  PARAM      *P;
  clock_t    stop = 0, start = clock();
  
  P = (PARAM *) Calloc(1, sizeof(PARAM));

  if((P->help = ArgState(DEFAULT_HELP, p, argc, "-h", "--help")) == 1 || 
  argc < 2){
    PrintMenu();
    return EXIT_SUCCESS;
    }

  if(ArgState(DEF_VERSION, p, argc, "-a", "--version")){
    PrintVersion();
    return EXIT_SUCCESS;
    }

  if(ArgState(DEF_EXPLANATION, p, argc, "-x", "--explanation")){
    ModelsExplanation();
    return EXIT_SUCCESS;
    }

  if(ArgState(0, p, argc, "-s", "--show-levels")){
    PrintLevels();
    return EXIT_SUCCESS;
    }

  P->verbose   = ArgState    (DEFAULT_VERBOSE, p, argc, "-v",  "--verbose");
  P->force     = ArgState    (DEFAULT_FORCE,   p, argc, "-f",  "--force");
  P->estim     = ArgState    (0,               p, argc, "-e",  "--estimate");
  P->progress  = ArgState    (0,               p, argc, "-p",  "--progress");
  P->extended  = ArgState    (0,               p, argc, "-P",  "--progress-extended");
  P->seed      = ArgNumber   (DEFAULT_SEED,    p, argc, "-sd", "--seed", 
		 1, 599999);
  P->hs        = ArgNumber   (DEFAULT_HS,      p, argc, "-hs", "--hidden-size", 
		 1, 999999);
  P->lr        = ArgDouble   (DEFAULT_LR,      p, argc, "-lr", "--learning-rate");
  P->level     = ArgNumber   (0,               p, argc, "-l",  "--level", 
		 MIN_LEVEL, MAX_LEVEL);
  P->mode      = ArgState    (DEF_MODE,        p, argc, "-d", "--decompress"); 
  
  if(!P->mode)
    {
    P->output = ArgsFileGen (p, argc, "-o", argv[argc-1], ".jc");
    P->datatype = FindDataType(argv[argc-1]);
    }
  else
    P->output = ArgsFileGen (p, argc, "-o", argv[argc-1], ".jd");

  for(n = 1 ; n < argc ; ++n){
    if(strcmp(argv[n], "-cm") == 0){
      P->nCModels++;
      P->nModels++;
      }
    if(strcmp(argv[n], "-rm") == 0){
      P->nRMClasses++;
      P->nModels++;
      }
    }

  if(P->nModels == 0 && P->level == 0)
    P->level = DEFAULT_LEVEL;

  if(P->level != 0 && !P->mode){
    xpl = GetLevels(P->level);
    xargc = StrToArgv(xpl, &xargv);
    for(n = 1 ; n < xargc ; ++n){
      if(strcmp(xargv[n], "-cm") == 0){
        P->nCModels++;
        P->nModels++;
        }
      if(strcmp(xargv[n], "-rm") == 0){
        P->nRMClasses++;
        P->nModels++;
        }

      if(strcmp(xargv[n], "-hs") == 0){
        P->hs = atoi(xargv[n+1]);
	}
      
      if(strcmp(xargv[n], "-lr") == 0){
        P->lr = atof(xargv[n+1]);
        }
      }
    }

  P->lr = ((int)(P->lr * 65534)) / 65534.0;

  if(P->nModels == 0 && !P->mode){
    MsgNoModels();
    return 1;
    }

  P->rmodel = (RModelPar *) Calloc(P->nRMClasses, sizeof(RModelPar));
  k = 0;
  for(n = 1 ; n < argc ; ++n)
    if(strcmp(argv[n], "-rm") == 0)
      P->rmodel[k++] = ArgsUniqRModel(argv[n+1], 0);
  if(P->level != 0){
    for(n = 1 ; n < xargc ; ++n)
      if(strcmp(xargv[n], "-rm") == 0)
        P->rmodel[k++] = ArgsUniqRModel(xargv[n+1], 0);
    }

  P->cmodel = (CModelPar *) Calloc(P->nCModels, sizeof(CModelPar));
  k = 0;
  for(n = 1 ; n < argc ; ++n)
    if(strcmp(argv[n], "-cm") == 0)
      P->cmodel[k++] = ArgsUniqCModel(argv[n+1], 0);
  if(P->level != 0){
    for(n = 1 ; n < xargc ; ++n)
      if(strcmp(xargv[n], "-cm") == 0)
        P->cmodel[k++] = ArgsUniqCModel(xargv[n+1], 0);
    }

  P->tar = argv[argc-1];
 
  if(!P->mode)
    {
    if(P->verbose) 
      {
      PrintArgs(P);
      fprintf(stderr, "Compressing ...\n"); 
      }
    if(P->nRMClasses == 1 && P->nCModels == 0) 
      CompressRMsOnly(P, argv[argc-1]);
    else
      { 
      if(P->lr == 0)
        {
        CompressNoNN(P, argv[argc-1]);
	}
      else
	{
        Compress(P, argv[argc-1]);
	}
      }
    }
  else
    {
    if(P->verbose)
      fprintf(stderr, "Decompressing ...\n"); 
    Decompress(P->output, P->progress, argv[argc-1]);
    }

  stop = clock();
  if(P->verbose)
    fprintf(stderr, "Spent %g seconds.              \n", 
    ((double)(stop-start)) / CLOCKS_PER_SEC); 

  return 0;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
