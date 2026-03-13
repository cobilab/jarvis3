#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "defs.h"
#include "mem.h"
#include "common.h"
#include "pmodels.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

PMODEL *CreatePModel(uint32_t n)
  {
  PMODEL *PM = (PMODEL *) Calloc(1, sizeof(PMODEL));
  PM->freqs  = (uint32_t *) Calloc(n, sizeof(uint32_t));
  return PM;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

CMWEIGHT *CreateWeightModel(uint32_t size)
  {
  uint32_t n;
  CMWEIGHT *CMW = (CMWEIGHT *) Calloc(1, sizeof(CMWEIGHT));
  const double fraction = 1.0 / size;

  CMW->totModels   = size;
  CMW->totalWeight = 0.0;
  CMW->gamma       = (double *) Calloc(CMW->totModels, sizeof(double));
  CMW->weight      = (double *) Calloc(CMW->totModels, sizeof(double));

  for(n = 0 ; n < CMW->totModels ; ++n)
    {
    CMW->weight[n] = fraction;
    CMW->gamma[n]  = DEFAULT_GAMMA;
    }

  return CMW;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ResetWeightModel(CMWEIGHT *CMW)
  {
  uint32_t n;
  const double fraction = 1.0 / CMW->totModels;

  CMW->totalWeight = 0.0;

  for(n = 0 ; n < CMW->totModels ; ++n)
    {
    CMW->weight[n] = fraction;
    CMW->gamma[n]  = DEFAULT_GAMMA;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RenormalizeWeights(CMWEIGHT *CMW)
  {
  uint32_t n;
  double inv;

  if(CMW->totalWeight == 0.0)
    return;

  inv = 1.0 / CMW->totalWeight;

  for(n = 0 ; n < CMW->totModels ; ++n)
    CMW->weight[n] *= inv;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void CalcDecayment(CMWEIGHT *CMW, PMODEL **PM, uint8_t sym)
  {
  uint32_t n;
  double total = 0.0;

  for(n = 0 ; n < CMW->totModels ; ++n)
    {
    PMODEL *P = PM[n];
    const double ps = (double) P->freqs[sym] / P->sum;
    CMW->weight[n] = Power(CMW->weight[n], CMW->gamma[n]) * ps;
    total += CMW->weight[n];
    }

  CMW->totalWeight = total;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveWeightModel(CMWEIGHT *CMW)
  {
  Free(CMW->weight);
  Free(CMW->gamma);
  Free(CMW);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ComputeMXProbs(FPMODEL *PT, PMODEL *MX, uint32_t nSym)
  {
  (void) nSym;

  const uint32_t f0 = 1u + (uint32_t)(PT->freqs[0] * MX_PMODEL);
  const uint32_t f1 = 1u + (uint32_t)(PT->freqs[1] * MX_PMODEL);
  const uint32_t f2 = 1u + (uint32_t)(PT->freqs[2] * MX_PMODEL);
  const uint32_t f3 = 1u + (uint32_t)(PT->freqs[3] * MX_PMODEL);

  MX->freqs[0] = f0;
  MX->freqs[1] = f1;
  MX->freqs[2] = f2;
  MX->freqs[3] = f3;
  MX->sum = f0 + f1 + f2 + f3;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemovePModel(PMODEL *PM)
  {
  Free(PM->freqs);
  Free(PM);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

FPMODEL *CreateFloatPModel(uint32_t n)
  {
  FPMODEL *F = (FPMODEL *) Calloc(1, sizeof(FPMODEL));
  F->freqs   = (double *) Calloc(n, sizeof(double));
  F->nSym    = n;
  return F;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void RemoveFPModel(FPMODEL *FM)
  {
  Free(FM->freqs);
  Free(FM);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ComputeWeightedFreqs(double w, PMODEL *P, FPMODEL *PT, uint32_t nSym)
  {
  (void) nSym;

  const double f = w / P->sum;

  PT->freqs[0] += (double) P->freqs[0] * f;
  PT->freqs[1] += (double) P->freqs[1] * f;
  PT->freqs[2] += (double) P->freqs[2] * f;
  PT->freqs[3] += (double) P->freqs[3] * f;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

double PModelNats(PMODEL *P, uint32_t s)
  {
  return log((double) P->sum / P->freqs[s]);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

double PModelStat(PMODEL *P, uint32_t s)
  {
  return (double) P->sum / P->freqs[s];
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
