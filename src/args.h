#ifndef ARGS_H_INCLUDED
#define ARGS_H_INCLUDED

#include "defs.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

char        *WriteDataType  (uint8_t);
char        *ArgsFileGen    (char *[], uint32_t, char *, char *, char *);
uint32_t    ArgNumber       (uint32_t , char *[], uint32_t, char *, char *, 
		             uint32_t, uint32_t);
CModelPar   ArgsUniqCModel  (char *, uint8_t);
RModelPar   ArgsUniqRModel  (char *, uint8_t);
double      ArgDouble       (double, char *[], uint32_t, char *, char *);
uint8_t     ArgState        (uint8_t  , char *[], uint32_t, char *, char *);
char        *ArgString      (char    *, char *[], uint32_t, char *, char *);
void        PrintArgs       (PARAM *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
