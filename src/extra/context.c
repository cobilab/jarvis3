#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "defs.h"
#include "mem.h"
#include "common.h"
#include "context.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

CModel *CreateCModel(U32 ctx, U32 aDen, U8 ref, U32 edits, U32 eDen, U32 nSym,
double gamma, double eGamma){
  CModel *M = (CModel *) Calloc(1, sizeof(CModel));
  U64    prod = 1, *mult;
  U32    n;
  double tmp;

  M->nSym        = nSym;
  mult           = (U64 *) Calloc(ctx, sizeof(U64));
  tmp            = pow(M->nSym, ctx);
  M->nPModels    = (U64) tmp;
  //M->nPModels  = (U64) pow(M->nSym, ctx);
  M->ctx         = ctx;
  M->alphaDen    = aDen;
  M->edits       = edits;
  M->gamma       = gamma;
  M->eGamma      = eGamma;
  M->pModelIdx   = 0;
  M->ref         = ref == 0 ? 0 : 1;

  //if((ULL)(M->nPModels) * M->nSym * sizeof(ACC) >> 20 > MAX_ARRAY_MEMORY || tmp > UINT64_MAX){
  if(tmp * (double)M->nSym * (double)sizeof(ACC) / pow(2,20) > MAX_ARRAY_MEMORY || tmp >= UINT64_MAX){
    M->mode = HASH_TABLE_MODE;
    M->HT   = CreateHashTable(M->nSym);
    }
  else{
    M->mode = ARRAY_MODE;
    M->AT   = CreateArrayTable(M->nSym, M->nPModels);
    }

  for(n = 0 ; n < M->ctx ; ++n){
    mult[n] = prod;
    prod *= M->nSym;
    }

  M->multiplier = mult[M->ctx-1];

  if(edits != 0){
    M->TM = CreateTolerantModel(edits, eDen, M->ctx, nSym);
    }

  Free(mult);
  return M;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ResetCModelIdx(CModel *M){
  M->pModelIdx = 0;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void GetPModelIdx(U8 *p, CModel *M){
  M->pModelIdx = ((M->pModelIdx-*(p-M->ctx)*M->multiplier)*M->nSym)+*p;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t GetPModelIdxCorr(U8 *p, CModel *M, uint64_t idx){
  return (((idx-*(p-M->ctx)*M->multiplier)*M->nSym)+*p);
  }
 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ComputePModel(CModel *M, PModel *P, uint64_t idx, uint32_t aDen){
  ACC *ac;
  HCC *hc;
  uint32_t x;
  switch(M->mode){
    case HASH_TABLE_MODE:
      if(!(hc = GetHCCounters(M->HT, idx)))
        hc = (HCC *) M->HT->zeroCounters;
      P->sum = 0;
      for(x = 0 ; x < M->nSym ; ++x){
        P->freqs[x] = 1 + aDen * hc[x];
        P->sum += P->freqs[x];
        }
    break;

    case ARRAY_MODE:
      ac = &M->AT->counters[idx*M->nSym];
      P->sum = 0;
      for(x = 0 ; x < M->nSym ; ++x){
        P->freqs[x] = 1 + aDen * ac[x];
        P->sum += P->freqs[x];
        }
    break;


    default:
    fprintf(stderr, "Error: not implemented!\n");
    exit(1);
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UpdateCModelCounter(CModel *M, uint32_t sym, uint64_t idx){
  if(M->mode == HASH_TABLE_MODE) UpdateHashCounter (M->HT, sym, idx);
  else                           UpdateArrayCounter(M->AT, sym, idx);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveCModel(CModel *M){
  if(M->mode == HASH_TABLE_MODE) RemoveHashTable (M->HT);
  else                           RemoveArrayTable(M->AT);

  if(M->edits > 0)               RemoveTolerantModel(M->TM);
  Free(M);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

double PModelSymbolNats(PModel *P, U32 s){
  return log((double) P->sum / P->freqs[s]);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
