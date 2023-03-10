#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#include <stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <unistd.h>

#pragma pack(1)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// RUNNING OPTIMIZATIONS : MEMORY / SPEED

#define ESTIMATE
#define PROGRESS

typedef uint64_t ULL;
typedef uint64_t U64;
typedef uint32_t U32;
typedef uint16_t U16;
typedef uint8_t  U8;
typedef int64_t  I64;
typedef int32_t  I32;
typedef int16_t  I16;
typedef int8_t   I8;

typedef struct{
  U32    ctx;
  U32    den;
  double gamma;
  double eGamma;
  U32    edits;
  U32    eDen;
  U8     type;
  }
ModelPar;

typedef struct{
  U8       help;
  U8       verbose;
  U8       force;
  U8       estim;
  U8       level;
  ModelPar *model;
  char     *ref;
  char     **tar;
  U8       nTar;
  U64      checksum;
  U64      size;
  U32      watermark;
  U32      nModels;
  U32      low;
  }
Parameters;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define RELEASE                1
#define VERSION                1

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define WATERMARK_BITS         8
#define CHECKSUM_BITS          8
#define SIZE_BITS              40
#define LS_BITS                40
#define LOW_BITS               16
#define N_LOW_BITS             8
#define LOW_SYM_BITS           8
#define CARDINALITY_BITS       9
#define SYM_BITS               8
#define N_MODELS_BITS          9
#define CTX_BITS               5
#define ALPHA_DEN_BITS         11
#define GAMMA_BITS             16
#define EDITS_BITS             6
#define E_GAMMA_BITS           16
#define E_DEN_BITS             9
#define TYPE_BITS              1

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define BUFFER_SIZE            262144      
#define PROGRESS_MIN           1000000
#define MAX_ARRAY_MEMORY       2048    //Size in MBytes
#define DEF_VERSION            0
#define DEF_EXAMPLE            0
#define DEFAULT_HELP           0
#define DEFAULT_VERBOSE        0
#define DEFAULT_FORCE          0
#define DEFAULT_LEVEL          2
#define MAX_LEVEL              9
#define MIN_LEVEL              1
#define MAX_THRESHOLD          1000000
#define MIN_THRESHOLD          1
#define MAX_CTX                31
#define MIN_CTX                1
#define MAX_DEN                1000000
#define MIN_DEN                1
#define BGUARD                 32
#define DEFAULT_MAX_COUNT      ((1 << (sizeof(ACC) * 8)) - 1)
#define CHECKSUMGF             255
#define WATERMARK              120
#define MAX_HISTORYSIZE        1000000
#define REFERENCE              1
#define TARGET                 0
#define EXTRA_MOD_DEN          1
#define EXTRA_MOD_CTX          3
#define EXTRA_BIN_DEN          1
#define EXTRA_BIN_CTX          8
#define EXTRA_N_DEN            1
#define EXTRA_N_CTX            8
#define EXTRA_L_DEN            1
#define EXTRA_L_CTX            8
#define MAX_STR                2048

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif

