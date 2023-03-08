#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "repeats.h"
#include "common.h"
#include "dna.h"
#include "mem.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// SHIFT BUFFER TO RIGHT. PERHAPS A CIRCULAR BUFFER IS A BETTER APPROACH...
//
void ShiftRBuf(uint8_t *b, int32_t s, uint8_t n){
  memmove(b, b+1, s * sizeof(uint8_t));
  b[s-1] = n;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// GET NUMERICAL BASE FROM PACKED SEQUENCE BY ID. THE ID%4 IS GIVEN BY THE 2
// LESS SIGNIFICATIVE BITS (ID&3).
//
uint8_t GetNBase(uint8_t *b, uint64_t i){
  return (uint8_t) (((0x3<<((3-(i&0x3))<<1)) & b[i>>2])>>((3-(i&0x3))<<1));
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// CREATES THE RCLASS BASIC STRUCTURE 
//
RCLASS *CreateRC(uint32_t m, double a, double b, uint32_t l, uint32_t c,
double g, uint8_t i, double w, uint64_t s)
  {
  RCLASS *C      = (RCLASS *) Calloc(1, sizeof(RCLASS));
  C->P           = (RPARAM *) Calloc(1, sizeof(RPARAM));
  C->RM          = (RMODEL *) Calloc(m, sizeof(RMODEL));
  C->mRM         = m;
  C->P->rev      = i;
  C->P->alpha    = ((int)(a*65534))/65534.0;
  C->P->beta     = ((int)(b*65534))/65534.0;
  C->P->gamma    = ((int)(g*65534))/65534.0;
  C->P->limit    = l;
  C->P->ctx      = c;
  C->P->mult     = CalcMult(c);
  C->P->idx      = 0;
  C->P->idxRev   = 0;
  C->P->c_pos    = 0;
  C->P->c_max    = s;
  C->P->c_idx    = 0;
  C->P->c_idxRev = 0;
  C->P->iWeight  = w;

  C->table        = (RTABLE *) Calloc(1, sizeof(RTABLE));
  C->table->nPos  = 10;
  C->table->size  = pow(4, C->P->ctx) * (C->table->nPos + 1);
  C->table->array = (POS_PREC *) Calloc(C->table->size, sizeof(POS_PREC));

  return C;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// REVERSE COMPLEMENT INDEX BASED ON PAST SYMBOLS
//
uint64_t GetIdxRev(uint8_t *p, RCLASS *C){
  return (C->P->idxRev = (C->P->idxRev>>2)+CompNum(*p)*C->P->mult);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// INDEX CALC BASED ON PAST SYMBOLS
//
uint64_t GetIdx(uint8_t *p, RCLASS *C){
  return (C->P->idx = ((C->P->idx-*(p-C->P->ctx)*C->P->mult)<<2)+*p);
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// START EACH REPEAT MODEL, CURRENTLY BY RANDOM STORED POSITION 
//
int32_t StartRM(RCLASS *C, uint32_t m, uint64_t i, uint8_t r)
  {
  POS_PREC *E = &C->table->array[i * C->table->nPos];

  uint64_t idx = rand() % C->table->nPos; 

  if(r == 0) // REGULAR REPEAT
    C->RM[m].pos = E[idx];
  else // INVERTED REPEAT
    {
    if(E[idx] <= C->P->ctx+1) 
      return 0;
    C->RM[m].pos = E[idx] - C->P->ctx - 1;
    }

  C->RM[m].nHits  = 0;
  C->RM[m].nTries = 0;
  C->RM[m].rev    = r;
  C->RM[m].acting = 0;
  C->RM[m].weight = C->P->iWeight;

  C->RM[m].probs[0] = 0;
  C->RM[m].probs[1] = 0;
  C->RM[m].probs[2] = 0;
  C->RM[m].probs[3] = 0;

  return 1;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// INSERT KMER POSITION INTO RHASH TABLE 
//
void InsertKmerPos(RCLASS *C, uint64_t key, POS_PREC pos)
  {
  POS_PREC *TC = &C->table->array[key * C->table->nPos];
  POS_PREC index_max = C->table->nPos - 1;
  POS_PREC index_pos = TC[index_max];

  if(index_pos == index_max)
    {
    TC[0] = pos;
    TC[index_max] = 0;
    }
  else
    {
    TC[index_pos + 1] = pos;
    TC[index_max] = index_pos + 1;
    }
  
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// COMPUTE REPEAT MODEL PROBABILITIES
//
void ComputeRMProbs(RCLASS *C, RMODEL *R, uint8_t *b){
  
  uint8_t n, s;
  
  s = (R->rev != 0) ? CompNum(GetNBase(b, R->pos)) : GetNBase(b, R->pos);

  R->probs[s] = (R->nHits+C->P->alpha) / (R->nTries+2*C->P->alpha);  //TODO: SPACE FOR OPT >>1
  for(n = 0 ; n < 4 ; ++n)
    if(n != s){
      R->probs[n] = (1-R->probs[s])/3;                     //TODO: SPACE FOR OPT (complement... and FOR)
      }

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// UPDATE REPEAT MODEL
//
void UpdateRM(RMODEL *R, uint8_t *b, uint8_t s)
  {
  R->lastHit = 1;

  if(R->rev == 0)
    {
    if(GetNBase(b, R->pos++) == s)
      {
      R->nHits++;
      R->lastHit = 0;
      }
    }
  else
    {
    if(CompNum(GetNBase(b, R->pos--)) == s)
      {
      R->nHits++;
      R->lastHit = 0;
      }
    }

  R->nTries++;
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// RENORMALIZE REPEAT WEIGHTS
//
void RenormWeights(RCLASS *C)
  {
  uint32_t n;
  double t = 0;
  for(n = 0 ; n < C->nRM ; ++n)
    t += C->RM[n].weight;
  for(n = 0 ; n < C->nRM ; ++n)
    C->RM[n].weight /= t;

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// STOP USELESS REPEAT MODELS
//
void StopRM(RCLASS *C)
  {
  uint32_t n, a;
  do
    {
    a = 0;
    for(n = 0 ; n < C->nRM ; ++n)
      if((C->RM[n].acting = C->P->beta * C->RM[n].acting + C->RM[n].lastHit) > 
      C->P->limit * 1.0 || C->RM[n].pos == 0)
        {
        if(n != C->nRM-1)
          C->RM[n] = C->RM[C->nRM-1];
        C->nRM--;
        a = 1;
        break;
        }
    }
  while(a);

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// START NEW REPEAT MODELS IF THERE IS STILL SPACE
//                         
void StartMultipleRMs(RCLASS *C, uint8_t *b)
  {  
  uint64_t idx = GetIdx(b, C); // DO NOT SET IT INSIDE THE NEXT IF!
			       
  if(C->P->rev != 2)
    if(C->nRM < C->mRM && StartRM(C, C->nRM, idx, 0))
      C->nRM++;

  if(C->P->rev != 0)
    {
    uint64_t idx_rev = GetIdxRev(b, C);
    if(C->nRM < C->mRM && StartRM(C, C->nRM, idx_rev, 1))
      C->nRM++;
    }

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// COMPUTE AND EXTRACT MIXTURED PROBABILITIES
//
void ComputeMixture(RCLASS *C, PMODEL *M, uint8_t *b)
  {
  uint32_t r; 
  double F[4] = {0,0,0,0};
  
  for(r = 0 ; r < C->nRM ; ++r)
    {
    double rmw = C->RM[r].weight;
    ComputeRMProbs(C, &C->RM[r], b);
    F[0] += C->RM[r].probs[0] * rmw;
    F[1] += C->RM[r].probs[1] * rmw;
    F[2] += C->RM[r].probs[2] * rmw;
    F[3] += C->RM[r].probs[3] * rmw;
    }

  M->sum = 0;
  M->sum += (M->freqs[0] = 1 + (uint32_t)(F[0] * MAXC)); // SHIFT <<  WITH 2^ TO MAXC?
  M->sum += (M->freqs[1] = 1 + (uint32_t)(F[1] * MAXC));
  M->sum += (M->freqs[2] = 1 + (uint32_t)(F[2] * MAXC));
  M->sum += (M->freqs[3] = 1 + (uint32_t)(F[3] * MAXC));
  
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// UPDATE WEIGHTS ACCORDING TO THE PERFORMANCE OF EACH REPEAT MODEL
//
void UpdateWeights(RCLASS *C, uint8_t *b, uint8_t s)
  {
  uint32_t r;

  for(r = 0 ; r < C->nRM ; ++r)
    {
    C->RM[r].weight = Power(C->RM[r].weight, C->P->gamma) * C->RM[r].probs[s];
    UpdateRM(&C->RM[r], b, s);
    }

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
