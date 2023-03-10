#ifndef CONTEXT_H_INCLUDED
#define CONTEXT_H_INCLUDED

#include "defs.h"
#include "buffer.h"
#include "hash.h"
#include "array.h"
#include "pmodels.h"
#include "tolerant.h"

#define ARRAY_MODE            0
#define HASH_TABLE_MODE       1

typedef struct{
  U32        ctx;           // Current depth of context template
  U64        nPModels;      // Maximum number of probability models
  U32        alphaDen;      // Denominator of alpha
  U32        maxCount;      // Counters /= 2 if one counter >= maxCount
  U64        multiplier;
  U8         ref;
  U32        mode;
  HASH       *HT;
  ARRAY      *AT;
  TOLERANT   *TM;
  double     gamma;
  double     eGamma;
  U64        pModelIdx;     // IDX
  U32        edits;         // EDITS
  U32        nSym;          // EDITS
  }
CModel;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void            GetPModelIdx         (U8 *, CModel *);
uint64_t        GetPModelIdxCorr     (U8 *, CModel *, uint64_t);
void            ResetCModelIdx       (CModel *);
void            UpdateCModelCounter  (CModel *, U32, U64);
CModel          *CreateCModel        (U32, U32, U8, U32, U32, U32, double, double);
void            ComputePModel        (CModel *, PModel *, uint64_t, uint32_t);
void            RemoveCModel         (CModel *);
double          PModelSymbolNats     (PModel *, U32);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
