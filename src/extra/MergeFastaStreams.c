#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// This application merges three channels of information: 
//        * HEADERS;
//        * EXTRA;
//        * DNA;
// into a FASTA file.

int main(int argc, char *argv[]){

  FILE *HEADERS, *EXTRA, *DNA;
  int c, d = 0;

  if((HEADERS = fopen ("HEADERS.JV2", "r")) == NULL){
    fprintf(stderr, "Error: could not open file!");
    return 1;
    }

  if((EXTRA = fopen ("EXTRA.JV2", "r")) == NULL){
    fprintf(stderr, "Error: could not open file!");
    return 1;
    }

  if((DNA = fopen ("DNA.JV2", "r")) == NULL){
    fprintf(stderr, "Error: could not open file!");
    return 1;
    }

  while((c = fgetc(EXTRA)) != EOF){
    
    if(c == '>'){
      fprintf(stdout, "%c", c);
      while((c = fgetc(HEADERS)) != EOF){
	if(c == EOF) goto x;
	fprintf(stdout, "%c", c);
	if(c == '\n') break;
        }
      continue;
      }
    
    switch(c){
      
      case 0:
        if((d = fgetc(DNA)) == EOF){
          fprintf(stderr, "Error: invalid format!");
          return 1;
	  }
        fprintf(stdout, "%c", d); 
      break;

      case 1:
        if((d = fgetc(DNA)) == EOF){
          fprintf(stderr, "Error: invalid format!");
          return 1;
          }
	fprintf(stdout, "%c", tolower(d));
      break;
      
      default:
        fprintf(stdout, "%c", c);
      break;      
      }
    }

  x:

  if(!HEADERS) fclose(HEADERS);
  if(!EXTRA)   fclose(EXTRA);
  if(!DNA)     fclose(DNA);
  return EXIT_SUCCESS;
  }



