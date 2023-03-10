#include "msg.h"
#include <stdio.h>
#include <stdlib.h>

void ModelsExplanation(void){
  fprintf(stderr,
  "                                                                       \n"
  "  -tm <c>:<d>:<g>/<m>:<e>:<a>  target model (-tm 1:1:0.8/0:0:0),       \n"
  "  -tm <c>:<d>:<g>/<m>:<e>:<a>  target model (-tm 7:100:0.9/2:10:0.85), \n"
  "  ...                                                                  \n"
  "                         target context model templates use <c> for    \n"
  "                         context-order size, <d> for alpha (1/<d>), <g>\n"
  "                         for gamma (decayment forgetting factor) [0;1),\n"
  "                         <m> to the maximum sets the allowed mutations,\n"
  "                         on the context without being discarded (for   \n"
  "                         deep contexts), under the estimator <e>, using\n"
  "                         <a> for gamma (decayment forgetting factor)   \n"
  "                         [0;1) (tolerant model),                       \n");
  } 

void PrintMenuD(void){
  fprintf(stderr,
  "Usage: XScoreD [OPTION]... [FILE]:[...]                                \n"
  "Decompression of quality sequences (compressed by XScoreC).            \n"
  "                                                                       \n"
  "Non-mandatory arguments:                                               \n"
  "                                                                       \n"
  "  -h                    give this help,                                \n"
  "  -v                    verbose mode (more information),               \n"
  "                                                                       \n"
  "Mandatory arguments:                                                   \n"
  "                                                                       \n"
  "  <FILE>                file to uncompress (last argument). For        \n"
  "                        more files use splitting \":\" characters.     \n"
  "                                                                       \n");
  }

void PrintMenu(void){
  fprintf(stderr,
  "Usage: XScoreC [OPTION]... [FILE]:[...]                                \n"
  "Compression of quality sequences.                                      \n"
  "                                                                       \n"
  "Non-mandatory arguments:                                               \n"
  "                                                                       \n"
  "  -h                     give this help,                               \n"
  "  -s                     show XScoreC compression levels,              \n"
  "  -v                     verbose mode (more information),              \n"
  "  -V                     display version number,                       \n"
  "  -l <level>             level of compression [1;9] (lazy -tm setup),  \n");
  ModelsExplanation();
  fprintf(stderr,
  "                                                                       \n"
  "Mandatory arguments:                                                   \n"
  "                                                                       \n"
  "  <FILE>:<...>:<...>     file to compress (last argument). For more    \n"
  "                         files use splitting \":\" characters.         \n"
  "                                                                       \n");
  }


void PrintVersion(void){
  fprintf(stderr,
  "                                                                       \n"
  "                          ================                             \n"
  "                          | XScore v%u.%u |                        \n"
  "                          ================                             \n"
  "                                                                       \n"
  "     Efficient compression and decompression of quality sequences.     \n"
  "                                                                       \n"
  "Copyright (C) 2017-2018 University of Aveiro. This is a Free software. \n"
  "You may redistribute copies of it under the terms of the GNU - General \n"
  "Public License v3 <http://www.gnu.org/licenses/gpl.html>. There is NOT \n"
  "ANY WARRANTY, to the extent permitted by law. Developed and Written by \n"
  "Diogo Pratas.\n\n", VERSION, RELEASE);
  }

