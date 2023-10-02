#ifndef LEVELS_H_INCLUDED
#define LEVELS_H_INCLUDED

#include "defs.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
#define LEVEL_1 " 1: -tm 3:1:0.2/0:0:0 "
#define LEVEL_2 " 2: -tm 1:1:0.95/0:0:0 -tm 4:200:0.2/0:0:0 "
#define LEVEL_3 " 3: -tm 2:10:0.95/0:0:0 -tm 4:200:0.2/0:0:0 "
#define LEVEL_4 " 4: -tm 2:10:0.95/0:0:0 -tm 6:500:0.2/0:0:0 "
#define LEVEL_5 " 5: -tm 1:1:0.95/0:0:0 -tm 8:500:0.2/0:0:0 "
#define LEVEL_6 " 6: -tm 1:1:0.95/0:0:0 -tm 10:500:0.2/0:0:0 "
#define LEVEL_7 " 7: -tm 1:1:0.95/0:0:0 -tm 11:500:0.2/0:0:0 "
#define LEVEL_8 " 8: -tm 1:1:0.95/0:0:0 -tm 3:1:0.95/0:0:0 -tm 5:500:0.2/0:0:0 "
#define LEVEL_9 " 9: -tm 1:1:0.5/0:0:0 -tm 3:10:0.5/0:0:0 -tm 5:200:0.5/0:0:0 "

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char    *GetLevels  (uint8_t);
void    PrintLevels (void);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif

