#ifndef REPEATS_H_INCLUDED
#define REPEATS_H_INCLUDED

#include "defs.h"
#include "pmodels.h"
#include "cm.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define MAX_BUF          2000000
#define NSYM             4
#define MAXC             ((1<<(sizeof(uint16_t)*8))-1)
#define DEF_MRM          50
#define DEF_KMER         11
#define DEF_ALPHA        1
#define DEF_GAMMA        0.2
#define DEF_BETA         0.90
#define DEF_LIMIT        7
#define DEF_REV          0
#define DEF_MODE         0
#define INIWEIGHT        0.01
#define MAX_LT           250

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct
  {
  uint32_t *array;   // ARRAY WITH POSITIONS
  uint32_t nPos;     // NUMBER OF POSITIONS (CACHE)
  uint32_t nPosAnd1; // NUMBER OF POSITIONS (CACHE) PLUS ONE (INDEXES)
  uint32_t size;     // ARRAY_SIZE
  }
RTABLE;

typedef struct
  {
  uint64_t idx;      // CURRENT CONTEXT INDEX
  uint64_t idxRev;   // CURRENT INVERTED REPEAT INDEX
  uint64_t mult;     // CURRENT INVERTED REPEAT INDEX
  uint32_t ctx;      // CONTEXT TEMPLATE SIZE FOR REPEAT MODEL
  uint32_t limit;    // REPEAT PERFORMANCE LIMIT, ASSOCIATED WITH BETA
  double   beta;     // REPEAT PERFORMANCE DECAYING FOR REPEAT MOVE
  double   gamma;    // PERFORMANCE DECAYING PARAMETER
  uint8_t  rev;      // INVERTED REPEAT USAGE [HEAVY -> MEMORY/TIME]
  uint64_t c_pos;    // CACHE INDEX POSITION TO REMOVE STORED POSITION
  int64_t  c_max;    // CACHE MAXIMUM THRESHOLD TO REMOVE POSITION
  uint64_t c_idx;    // CACHE CONTEXT INDEX
  uint64_t c_idxRev; // CACHE INVERTED REPEAT INDEX
  double   iWeight;  // INITIAL WEIGHT FOR EACH REPEAT MODEL CLASS
  }
RPARAM;

typedef struct
  {
  uint64_t pos;      // POSITION OF THE SYMBOL
  uint32_t nHits;    // NUMBER OF TIMES THIS MODEL WAS CORRECT
  uint32_t nTries;   // NUMBER OF TIMES THIS MODEL WAS USED
  double   probs[4]; // REPEAT MODEL SYMBOL PROBABILITIES
  double   weight;   // WEIGHT OF THE MODEL FOR MIXTURE
  double   acting;   // THE ACTING PERFORMANCE
  double   lastHit;  // IS ON OR NOT
  uint32_t id;       // ID OF THE RHASH
  uint8_t  rev;      // INVERTED REPETAT MODEL. IF REV='Y' THEN IS TRUE
  }
RMODEL;

typedef struct
  {
  CMODEL   *C;       // CONTEXT MODEL FOR COMPLEMENTAR SYMBOL PROBS 
  RTABLE   *T;       // REPEATING KMERS TABLE
  RMODEL   *RM;      // POINTER FOR EACH OF THE MULTIPLE REPEAT MODELS
  RPARAM   *P;       // EXTRA PARAMETERS FOR REPEAT MODELS
  uint32_t nRM;      // CURRENT NUMBER OF REPEAT MODELS
  uint32_t mRM;      // MAXIMUM NUMBER OF REPEAT MODELS
  uint64_t size;     // SIZE OF THE INPUT SEQUENCE PACKED
  uint64_t length;   // LENGTH OF THE INPUT SEQUENCE
  }
RCLASS;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t   GetNBase           (uint8_t *, uint64_t);
RCLASS    *CreateRC          (uint32_t, double, uint32_t, uint32_t, double, 
		               uint8_t, double, uint64_t);
uint64_t  GetIdxRev          (uint8_t *, RCLASS *);
uint64_t  GetIdx             (uint8_t *, RCLASS *);
int32_t   StartRM            (RCLASS *, uint32_t, uint64_t, uint8_t);
void      AddKmerPos         (RTABLE *, uint64_t, uint32_t);
void      ComputeRMProbs     (RCLASS *, RMODEL *, uint8_t *);
void      UpdateRM           (RMODEL *, uint8_t *, uint8_t);
void      RenormWeights      (RCLASS *);
void      StopRM             (RCLASS *);
void      StartMultipleRMs   (RCLASS *, uint8_t *);
void      ComputeMixture     (RCLASS *, PMODEL *, uint8_t *);
void      UpdateWeights      (RCLASS *, uint8_t *, uint8_t);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
