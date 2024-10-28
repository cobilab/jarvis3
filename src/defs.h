#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <unistd.h>

//#define DEBUG 1
#define ESTIMATE 1

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// RUNNING OPTIMIZATIONS : MEMORY / SPEED

typedef struct{
  uint32_t nr;
  uint32_t ctx;
  uint32_t ir;
  double   beta;
  int32_t  limit;
  double   gamma;
  double   weight;
  uint32_t cache;
  }
RModelPar;

typedef struct{
  uint32_t ctx;
  uint32_t den;
  uint32_t ir;
  double   gamma;
  uint32_t eIr;
  uint32_t edits;
  uint32_t eDen;
  double   eGamma;
  uint8_t  type;
  }
CModelPar;

typedef struct{
  uint8_t   help;
  uint8_t   verbose;
  uint8_t   force;
  uint8_t   estim;
  uint8_t   progress;
  uint8_t   extended;
  uint8_t   level;
  uint8_t   datatype;
  uint8_t   nn;
  uint8_t   mode;
  uint32_t  col;
  uint32_t  seed;
  double    lr;
  uint32_t  hs;
  uint32_t  nModels;
  uint32_t  nRPModels;
  uint32_t  nCPModels;
  uint32_t  nCModels;
  uint32_t  nRMClasses;
  CModelPar *cmodel;
  RModelPar *rmodel;
  char      *tar;
  uint8_t   nTar;
  uint32_t  watermark;
  uint32_t  selection;
  uint64_t  length;
  uint64_t  size;
  char      *output;
  }
PARAM;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define RELEASE                7
#define VERSION                3

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define BUFFER_SIZE            262144
#define ADD_SPACE              100000000
#define PROGRESS_MIN           1000000
#define DEF_VERSION            0
#define DEFAULT_HELP           0
#define DEFAULT_VERBOSE        0
#define DEFAULT_FORCE          0
#define DEFAULT_LEVEL          7
#define DEFAULT_HS             40
#define DEFAULT_LR             0.03
#define MAX_LEVEL              40
#define MIN_LEVEL              1
#define MAX_CTX                14
#define MAX_RCTX               15
#define MIN_CTX                1
#define MIN_RCTX               1
#define MIN_CACHE              1
#define MAX_CACHE              15
#define MAX_DEN                1000000
#define MIN_DEN                1
#define BGUARD                 32
#define DEFAULT_MAX_COUNT      ((1 << (sizeof(ACC) * 8)) - 1)
#define MX_PMODEL              65534
#define ALPHABET_SIZE          4
#define CHECKSUMGF             1073741824
#define WATERMARK              16042014
#define DEFAULT_GAMMA          0.90
#define MAX_STR                2048
#define REFERENCE              1
#define TARGET                 0
#define DEF_EXPLANATION        0
#define DEFAULT_SEED           0
#define DATA_TYPE_BITS         8
#define SEED_BITS              32
#define SIZE_BITS              48
#define LENGTH_BITS            48
#define SELECTION_BITS         8
#define NCMODELS_BITS          8
#define CTX_BITS               8
#define ALPHA_DEN_BITS         16
#define EDITS_BITS             8
#define E_GAMMA_BITS           16
#define E_DEN_BITS             16
#define IR_BITS                8
#define NRMCLASSES_BITS        16
#define MAX_RMODELS_BITS       16
#define LIMIT_BITS             8
#define ALPHA_BITS             16
#define BETA_BITS              16
#define GAMMA_BITS             16
#define WEIGHT_BITS            16
#define CACHE_BITS             34
#define HS_BITS                16
#define LR_BITS                16

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif

