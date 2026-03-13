#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "defs.h"
#include "dna.h"
#include "mem.h"
#include "common.h"
#include "cm.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline uint64_t PowU64(uint32_t base, uint32_t exp)
  {
  uint64_t r = 1;
  while(exp--)
    r *= base;
  return r;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

CMODEL *CreateCModel(uint32_t ctx, uint32_t aDen, uint8_t ref, uint32_t edits,
uint32_t eDen, uint32_t nSym, double gamma, double eGamma, uint32_t ir,
uint32_t eIr)
  {
  CMODEL   *M = (CMODEL *) Calloc(1, sizeof(CMODEL));
  uint64_t prod = 1;
  uint32_t n;

  M->nSym        = nSym;
  M->nPModels    = PowU64(M->nSym, ctx);
  M->ctx         = ctx;
  M->ir          = ir;
  M->alphaDen    = aDen;
  M->edits       = edits;
  M->gamma       = gamma;
  M->eGamma      = eGamma;
  M->pModelIdx   = 0;
  M->pModelIdxIR = M->nPModels - 1;
  M->ref         = ref == 0 ? 0 : 1;

  if(M->nPModels * M->nSym * sizeof(ACC) >> 20 > MAX_ARRAY_MEMORY)
    {
    M->mode = HASH_TABLE_MODE;
    M->HT   = CreateHashTable(M->nSym);
    }
  else
    {
    M->mode = ARRAY_MODE;
    M->AT   = CreateArrayTable(M->nSym, M->nPModels);
    }

  for(n = 0 ; n + 1 < M->ctx ; ++n)
    prod *= M->nSym;

  M->multiplier = prod;

  if(edits != 0)
    M->TM = CreateTolerantModel(edits, eDen, M->ctx, nSym, eIr);

  return M;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ResetCModelIdx(CMODEL *M)
  {
  M->pModelIdx   = 0;
  M->pModelIdxIR = M->nPModels - 1;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void GetPModelIdx(uint8_t *p, CMODEL *M)
  {
  M->pModelIdx = ((M->pModelIdx - *(p - M->ctx) * M->multiplier) * M->nSym) + *p;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t GetPModelIdxIR(uint8_t *p, CMODEL *M)
  {
  M->pModelIdxIR = (M->pModelIdxIR >> 2) + CompNum(*p) * M->multiplier;
  return CompNum(*(p - M->ctx));
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint64_t GetPModelIdxCorr(uint8_t *p, CMODEL *M, uint64_t idx)
  {
  return ((idx - *(p - M->ctx) * M->multiplier) * M->nSym) + *p;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ComputePModel(CMODEL *M, PMODEL *PM, uint64_t idx, uint32_t aDen,
long *freqs, long *sum)
  {
  if(M->mode == HASH_TABLE_MODE)
    {
    HCC *hc = GetHCCounters(M->HT, idx);
    if(!hc)
      hc = M->HT->zeroCounters;

    if(M->nSym == 4)
      {
      const uint32_t p0 = 1u + aDen * hc[0];
      const uint32_t p1 = 1u + aDen * hc[1];
      const uint32_t p2 = 1u + aDen * hc[2];
      const uint32_t p3 = 1u + aDen * hc[3];

      const long f0 = 1l + hc[0];
      const long f1 = 1l + hc[1];
      const long f2 = 1l + hc[2];
      const long f3 = 1l + hc[3];

      PM->freqs[0] = p0;
      PM->freqs[1] = p1;
      PM->freqs[2] = p2;
      PM->freqs[3] = p3;
      PM->sum = p0 + p1 + p2 + p3;

      freqs[0] = f0;
      freqs[1] = f1;
      freqs[2] = f2;
      freqs[3] = f3;
      sum[0] = f0 + f1 + f2 + f3;
      }
    else
      {
      uint32_t x;
      PM->sum = 0;
      for(x = 0 ; x < M->nSym ; ++x)
        {
        PM->freqs[x] = 1 + aDen * hc[x];
        PM->sum += PM->freqs[x];
        }

      sum[0] = 0;
      for(x = 0 ; x < M->nSym ; ++x)
        {
        freqs[x] = 1 + hc[x];
        sum[0] += freqs[x];
        }
      }
    return;
    }

  if(M->mode == ARRAY_MODE)
    {
    ACC *ac = &M->AT->counters[idx * M->nSym];

    if(M->nSym == 4)
      {
      const uint32_t p0 = 1u + aDen * ac[0];
      const uint32_t p1 = 1u + aDen * ac[1];
      const uint32_t p2 = 1u + aDen * ac[2];
      const uint32_t p3 = 1u + aDen * ac[3];

      const long f0 = 1l + ac[0];
      const long f1 = 1l + ac[1];
      const long f2 = 1l + ac[2];
      const long f3 = 1l + ac[3];

      PM->freqs[0] = p0;
      PM->freqs[1] = p1;
      PM->freqs[2] = p2;
      PM->freqs[3] = p3;
      PM->sum = p0 + p1 + p2 + p3;

      freqs[0] = f0;
      freqs[1] = f1;
      freqs[2] = f2;
      freqs[3] = f3;
      sum[0] = f0 + f1 + f2 + f3;
      }
    else
      {
      uint32_t x;
      PM->sum = 0;
      for(x = 0 ; x < M->nSym ; ++x)
        {
        PM->freqs[x] = 1 + aDen * ac[x];
        PM->sum += PM->freqs[x];
        }

      sum[0] = 0;
      for(x = 0 ; x < M->nSym ; ++x)
        {
        freqs[x] = 1 + ac[x];
        sum[0] += freqs[x];
        }
      }
    return;
    }

  fprintf(stderr, "Error: not implemented!\n");
  exit(1);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void UpdateCModelCounter(CMODEL *M, uint32_t sym, uint64_t idx)
  {
  if(M->mode == HASH_TABLE_MODE)
    UpdateHashCounter(M->HT, sym, idx);
  else
    UpdateArrayCounter(M->AT, sym, idx);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveCModel(CMODEL *M)
  {
  if(M->mode == HASH_TABLE_MODE)
    RemoveHashTable(M->HT);
  else
    RemoveArrayTable(M->AT);

  if(M->edits > 0)
    RemoveTolerantModel(M->TM);

  Free(M);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
