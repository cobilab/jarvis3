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
// LOOKUP TABLES FOR SPEED ENHANCEMENT
//
double LT  [MAX_LT][MAX_LT];
double LT_C[MAX_LT][MAX_LT];

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// GET NUMERICAL BASE FROM PACKED SEQUENCE BY ID. THE ID%4 IS GIVEN BY THE 2
// LESS SIGNIFICATIVE BITS (ID&3).
//
uint8_t GetNBase(uint8_t *b, uint64_t i){
  return (uint8_t) (((0x3<<((3-(i&0x3))<<1)) & b[i>>2])>>((3-(i&0x3))<<1));
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// FILL LOOKUP TABLE
//
void FillLT(void)
  {
  for(uint32_t x = 0 ; x < MAX_LT ; ++x)
    for(uint32_t y = 0 ; y < MAX_LT ; ++y)
      {
      LT  [y][x] = (y + 1.0) / (x + 2.0);
      LT_C[y][x] = (1 - LT[y][x]) / 3;
      }

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// CREATES THE RCLASS BASIC STRUCTURE 
//
RCLASS *CreateRC(uint32_t m, double b, uint32_t l, uint32_t c,
double g, uint8_t i, double w, uint64_t s)
  {
  RCLASS *C      = (RCLASS *) Calloc(1, sizeof(RCLASS));
  C->P           = (RPARAM *) Calloc(1, sizeof(RPARAM));
  C->RM          = (RMODEL *) Calloc(m, sizeof(RMODEL));
  C->mRM         = m;
  C->P->rev      = i;
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

  // TABLE
  C->T           = (RTABLE *) Calloc(1, sizeof(RTABLE));
  C->T->nPos     = s;
  C->T->nPosAnd1 = C->T->nPos + 1;
  C->T->size     = pow(NSYM, C->P->ctx) * C->T->nPosAnd1;
  C->T->array    = (POS_PREC *) Calloc(C->T->size + 1, sizeof(POS_PREC));

  FillLT();

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
  POS_PREC *E = &C->T->array[i * C->T->nPosAnd1];
  uint32_t last = E[C->T->nPos];

  if(last == 0) return 0;

  RMODEL *RM = &C->RM[m];
  uint64_t idx = rand() % C->T->nPos; 

  if(r == 0) // REGULAR REPEAT
    {
    if(E[idx] == 0)
      RM->pos = E[last]; // IF POSITION 0 THEN USE LATEST  
    else 
      RM->pos = E[idx];
    }
  else // INVERTED REPEAT
    {
    if(E[idx] <= C->P->ctx+1) 
      return 0;
    RM->pos = E[idx] - C->P->ctx - 1;
    }

  RM->nHits  = 0;
  RM->nTries = 0;
  RM->rev    = r;
  RM->acting = 0;
  RM->weight = C->P->iWeight;

  memset((void *)RM->probs, 0, NSYM * sizeof(double));

  return 1;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// ADD KMER POSITION INTO RTABLE 
//
void AddKmerPos(RCLASS *C, uint64_t key, POS_PREC pos)
  {
  POS_PREC *TC = &C->T->array[key * C->T->nPosAnd1];
  uint32_t nPos = C->T->nPos;
  uint32_t idx = TC[nPos];

  if(idx == nPos)
    {
    TC[0] = pos;
    TC[nPos] = 0;
    }
  else
    {
    TC[++idx] = pos;
    TC[nPos] = idx;
    }
  
  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// COMPUTE REPEAT MODEL PROBABILITIES
//
void ComputeRMProbs(RCLASS *C, RMODEL *R, uint8_t *b)
  {
  uint8_t s = (R->rev) ? CompNum(GetNBase(b, R->pos)) : GetNBase(b, R->pos);
  double comp_prob;	

  if(R->nTries < MAX_LT)
    {
    R->probs[s] = LT[R->nHits][R->nTries];
    comp_prob = LT_C[R->nHits][R->nTries];
    }
  else
    {
    R->probs[s] = (R->nHits+1.0) / (R->nTries+2.0); 
    comp_prob = (1-R->probs[s])/3;
    }
    
  if(0 != s) R->probs[0] = comp_prob;
  if(1 != s) R->probs[1] = comp_prob;
  if(2 != s) R->probs[2] = comp_prob;
  if(3 != s) R->probs[3] = comp_prob;
  
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
      ++R->nHits;
      R->lastHit = 0;
      }
    }
  else
    {
    if(CompNum(GetNBase(b, R->pos--)) == s)
      {
      ++R->nHits;
      R->lastHit = 0;
      }
    }

  ++R->nTries;
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
    for(a = 0, n = 0 ; n < C->nRM ; ++n)
      {
      RMODEL *R = &C->RM[n];
      if((R->acting = R->acting * C->P->beta + R->lastHit) > C->P->limit || 
	  R->pos == 0)
        {
        if(n != C->nRM-1)
          C->RM[n] = C->RM[C->nRM-1];
        C->nRM--;
        a = 1;
        break;
        }
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
      ++C->nRM;

  if(C->P->rev != 0)
    {
    uint64_t idx_rev = GetIdxRev(b, C);
    if(C->nRM < C->mRM && StartRM(C, C->nRM, idx_rev, 1))
      ++C->nRM;
    }

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// COMPUTE AND EXTRACT MIXTURED PROBABILITIES
//
void ComputeMixture(RCLASS *C, PMODEL *M, uint8_t *b)
  {
  double F[4] = {0,0,0,0};
  
  for(uint32_t r = 0 ; r < C->nRM ; ++r)
    {
    RMODEL *R = &C->RM[r];
    ComputeRMProbs(C, R, b);
    double rmw = R->weight;
    F[0] += R->probs[0] * rmw;
    F[1] += R->probs[1] * rmw;
    F[2] += R->probs[2] * rmw;
    F[3] += R->probs[3] * rmw;
    }

  M->sum  = (M->freqs[0] = 1 + (uint32_t)(F[0] * MAXC));
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
  for(uint32_t r = 0 ; r < C->nRM ; ++r)
    {
    RMODEL *R = &C->RM[r];
    R->weight = Power(R->weight, C->P->gamma) * R->probs[s];
    UpdateRM(R, b, s);
    }

  return;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
