#include "msg.h"
#include <stdio.h>
#include <stdlib.h>

void ModelsExplanation(void){
  fprintf(stderr,
  "                                                                   \n"
  "      -cm [NB_C]:[NB_D]:[NB_I]:[NB_G]/[NB_S]:[NB_E]:[NB_R]:[NB_A]  \n"
  "      Template of a context model.                                 \n"
  "      Parameters:                                                  \n"
  "      [NB_C]: (integer [1;14]) order size of the regular context   \n"
  "              model. Higher values use more RAM but, usually, are  \n"
  "              related to a better compression score.               \n"
  "      [NB_D]: (integer [1;5000]) denominator to build alpha, which \n"
  "              is a parameter estimator. Alpha is given by 1/[NB_D].\n"
  "              Higher values are usually used with higher [NB_C],   \n"
  "              and related to confident bets. When [NB_D] is one,   \n"
  "              the probabilities assume a Laplacian distribution.   \n"
  "      [NB_I]: (integer {0,1,2}) number to define if a sub-program  \n"
  "              which addresses the specific properties of DNA       \n"
  "              sequences (Inverted repeats) is used or not. The     \n"
  "              number 1 turns ON the sub-program using at the same  \n"
  "              time the regular context model. The number 2 does    \n"
  "              only contemple the inversions only (NO regular). The \n"
  "              number 0 does not contemple its use (Inverted repeats\n"
  "              OFF). The use of this sub-program increases the      \n"
  "              necessary time to compress but it does not affect the\n"
  "              RAM.                                                 \n"
  "      [NB_G]: (real [0;1)) real number to define gamma. This value \n"
  "              represents the decayment forgetting factor of the    \n"
  "              regular context model in definition.                 \n"
  "      [NB_S]: (integer [0;20]) maximum number of editions allowed  \n"
  "              to use a substitutional tolerant model with the same \n"
  "              memory model of the regular context model with       \n"
  "              order size equal to [NB_C]. The value 0 stands for   \n"
  "              turning the tolerant context model off. When the     \n"
  "              model is on, it pauses when the number of editions   \n"
  "              is higher that [NB_C], while it is turned on when    \n"
  "              a complete match of size [NB_C] is seen again. This  \n"
  "              is probabilistic-algorithmic model very useful to    \n"
  "              handle the high substitutional nature of genomic     \n"
  "              sequences. When [NB_S] > 0, the compressor used more \n"
  "              processing time, but uses the same RAM and, usually, \n"
  "              achieves a substantial higher compression ratio. The \n"
  "              impact of this model is usually only noticed for     \n"
  "              higher [NB_C].                                       \n"
  "      [NB_R]: (integer {0,1}) number to define if a sub-program    \n"
  "              which addresses the specific properties of DNA       \n"
  "              sequences (Inverted repeats) is used or not. It is   \n"
  "              similar to the [NR_I] but for tolerant models.       \n"
  "      [NB_E]: (integer [1;5000]) denominator to build alpha for    \n"
  "              substitutional tolerant context model. It is         \n"
  "              analogous to [NB_D], however to be only used in the  \n"
  "              probabilistic model for computing the statistics of  \n"
  "              the substitutional tolerant context model.           \n"
  "      [NB_A]: (real [0;1)) real number to define gamma. This value \n"
  "              represents the decayment forgetting factor of the    \n"
  "              substitutional tolerant context model in definition. \n"
  "              Its definition and use is analogus to [NB_G].        \n"
  "                                                                   \n"
  "      ... (you may use several context models)                     \n"
  "                                                                   \n"
  "                                                                   \n"
  "      -rm [NB_R]:[NB_C]:[NB_B]:[NB_L]:[NB_G]:[NB_I]:[NB_W]:[NB_Y]  \n"
  "      Template of a repeat model.                                  \n"
  "      Parameters:                                                  \n"
  "      [NB_R]: (integer [1;10000] maximum number of repeat models   \n"
  "              for the class. On very repetive sequences the RAM    \n"
  "              increases along with this value, however it also     \n"
  "              improves the compression capability.                 \n"
  "      [NB_C]: (integer [1;14]) order size of the repeat context    \n"
  "              model. Higher values use more RAM but, usually, are  \n"
  "              related to a better compression score.               \n"
  "      [NB_B]: (real (0;1]) beta is a real value, which is a        \n"
  "              parameter for discarding or maintaining a certain    \n"
  "              repeat model.                                        \n"
  "      [NB_L]: (integer (1;20]) a limit threshold to play with      \n"
  "              [NB_B]. It accepts or not a certain repeat model.    \n"
  "      [NB_G]: (real [0;1)) real number to define gamma. This value \n"
  "              represents the decayment forgetting factor of the    \n"
  "              regular context model in definition.                 \n"
  "      [NB_I]: (integer {0,1,2}) number to define if a sub-program  \n"
  "              which addresses the specific properties of DNA       \n"
  "              sequences (Inverted repeats) is used or not. The     \n"
  "              number 1 turns ON the sub-program using at the same  \n"
  "              time the regular context model. The number 0 does    \n"
  "              not contemple its use (Inverted repeats OFF). The    \n" 
  "              number 2 uses exclusively Inverted repeats. The      \n"
  "              use of this sub-program increases the necessary time \n"
  "              to compress but it does not affect the RAM.          \n"
  "      [NB_W]: (real (0;1)) initial weight for the repeat class.    \n"
  "      [NB_Y]: (integer {0}, [1;50]) maximum cache size. This will  \n"
  "              use a table cache with the specified size. The size  \n"
  "              must be in balance with the k-mer size [NB_C].       \n"
  "                                                                   \n");
  } 

void PrintMenu(void){
  fprintf(stderr,
  "                                                                   \n"
  "           ██ ███████ ███████ ██    ██ ██ ███████ ███████          \n"
  "           ██ ██   ██ ██   ██ ██    ██ ██ ██           ██          \n"
  "           ██ ███████ ██████  ██    ██ ██ ███████ ███████          \n"
  "      ██   ██ ██   ██ ██  ███  ██  ██  ██      ██      ██          \n"
  "      ███████ ██   ██ ██   ███  ████   ██ ███████ ███████          \n"
  "                                                                   \n"
  "NAME                                                               \n"
  "      JARVIS3 v%u.%u,                                              \n"
  "      Efficient lossless encoding of genomic sequences             \n"
  "                                                                   \n"
  "SYNOPSIS                                                           \n"
  "      ./JARVIS3 [OPTION]... [FILE]                                 \n"
  "                                                                   \n"
  "SAMPLE                                                             \n"
  "      Run Compression   -> ./JARVIS3 -v -l 13 sequence.txt         \n"
  "      Run Decompression -> ./JARVIS3 -v -d sequence.txt.jc         \n"
  "                                                                   \n"
  "DESCRIPTION                                                        \n"
  "      Lossless compression and decompression of genomic            \n"
  "      sequences for minimal storage and analysis purposes.         \n"
  "      Measure an upper bound of the sequence complexity.           \n"
  "                                                                   \n"
  "      -h,  --help                                                  \n"
  "           Usage guide (help menu).                                \n"
  "                                                                   \n"
  "      -a,  --version                                               \n"
  "           Display program and version information.                \n"
  "                                                                   \n"
  "      -x,  --explanation                                           \n"
  "           Explanation of the context and repeat models.           \n"
  "                                                                   \n"
  "      -f,  --force                                                 \n"
  "           Force mode. Overwrites old files.                       \n"
  "                                                                   \n"
  "      -v,  --verbose                                               \n"
  "           Verbose mode (more information).                        \n"
  "                                                                   \n"
  "      -p,  --progress                                              \n"
  "           Show progress bar.                                      \n"
  "                                                                   \n"
  "      -P,  --progress-extended                                     \n"
  "           Show compression progress for each 5%%.                 \n"
  "                                                                   \n"
  "      -d,  --decompress                                            \n"
  "           Decompression mode.                                     \n"
  "                                                                   \n",
  VERSION, RELEASE);

  #ifdef ESTIMATE
  fprintf(stderr,
  "      -e,  --estimate                                              \n"
  "           It creates a file with the extension \".iae\" with the  \n"
  "           respective information content. If the file is FASTA or \n"
  "           FASTQ it will only use the \"ACGT\" (genomic) sequence. \n");
  #endif
  
  fprintf(stderr,
  "                                                                   \n"
  "      -s,  --show-levels                                           \n"
  "           Show pre-computed compression levels (configured).      \n"
  "                                                                   \n"
  "      -l [NUMBER],  --level [NUMBER]                               \n"
  "           Compression level (integer).                            \n"
  "           Default level: %u.                                      \n"
  "           It defines compressibility in balance with computational\n"
  "           resources (RAM & time). Use -s for levels perception.   \n",
  DEFAULT_LEVEL);

  //ModelsExplanation();

  fprintf(stderr,
  "                                                                   \n"
  "      -sd [NUMBER],  --seed [NUMBER]                               \n"
  "           Pseudo-random seed.                                     \n"
  "           Default value: %u.                                      \n"
  "                                                                   \n"
  "      -hs [NUMBER],  --hidden-size [NUMBER]                        \n"
  "           Hidden size of the neural network (integer).            \n"
  "           Default value: %u.                                      \n"
  "                                                                   \n"
  "      -lr [DOUBLE],  --learning-rate [DOUBLE]                      \n"
  "           Neural Network leaning rate (double).                   \n"
  "           The 0 value turns the Neural Network off.               \n"
  "           Default value: %.2lf.                                   \n"
  "                                                                   \n"
  "      -o [FILENAME], --output [FILENAME]                           \n"
  "           Compressed/decompressed output filename.                \n"
  "                                                                   \n"
  "      [FILENAME]                                                   \n"
  "           Input sequence filename (to compress) -- MANDATORY.     \n"
  "           File to compress is the last argument.                  \n"
  "                                                                   \n"
  "COPYRIGHT                                                          \n"
  "      Copyright (C) 2014-2024.                                     \n"
  "      This is a Free software, under GPLv3. You may redistribute   \n"
  "      copies of it under the terms of the GNU - General Public     \n"
  "      License v3 <http://www.gnu.org/licenses/gpl.html>.         \n\n",
  DEFAULT_SEED, DEFAULT_HS, DEFAULT_LR);
  }


void PrintVersion(void){
  fprintf(stderr,
  "                                                                   \n"
  "           ██ ███████ ███████ ██    ██ ██ ███████ ███████          \n"
  "           ██ ██   ██ ██   ██ ██    ██ ██ ██           ██          \n"
  "           ██ ███████ ██████  ██    ██ ██ ███████ ███████          \n"
  "      ██   ██ ██   ██ ██  ███  ██  ██  ██      ██      ██          \n"
  "      ███████ ██   ██ ██   ███  ████   ██ ███████ ███████          \n"
  "                                                                   \n"
  "                      Version %u, Release %u                       \n"
  "                                                                   \n"
  "   Efficient lossless encoding and decoding of DNA sequences       \n"
  "                                                                   \n"
  "Developed at the CoBiLab (https://cobilab.github.io) at @IEETA.    \n"
  "Copyright (C) 2014-2024, University of Aveiro and University of    \n"
  "Helsinki. JARVIS3 is a Free software. Copies may be distributed    \n"
  "under the terms of the GNU-General Public License v3. See site:    \n"
  "http://www.gnu.org/licenses/gpl.html. There is NOT ANY WARRANTY    \n" 
  "Use it at your own risk.                                         \n\n", 
  VERSION, RELEASE);
  }

void MsgNoModels(void){
  fprintf(stderr, "Error: at least you need to use a context or repeat model!\n");
  }

void FailModelScheme(void){
  fprintf(stderr, "Error: unknown scheme for repeat model arguments!\n");
  fprintf(stderr, "Plz, reset the models according to the description:\n");
  ModelsExplanation();
  }

