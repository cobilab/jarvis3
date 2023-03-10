#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// This application splits FASTA into three channels of information: 
//        * HEADERS;
//        * EXTRA;
//        * DNA.

int main(int argc, char *argv[]){

  FILE *HEADERS, *EXTRA, *DNA;
  int c;

  if((HEADERS = fopen ("HEADERS.JV2", "w")) == NULL){
    fprintf(stderr, "Error: could not open file!");
    return 1;
    }

  if((EXTRA = fopen ("EXTRA.JV2", "w")) == NULL){
    fprintf(stderr, "Error: could not open file!");
    return 1;
    }

  if((DNA = fopen ("DNA.JV2", "w")) == NULL){
    fprintf(stderr, "Error: could not open file!");
    return 1;
    }

  while((c = fgetc(stdin)) != EOF){
    
    if(c == '>'){
      fprintf(EXTRA, "%c", c);
      while((c = fgetc(stdin)) != EOF){
	if(c == EOF) goto x;
	fprintf(HEADERS, "%c", c);
	if(c == '\n') break;
        }
      continue;
      }
      
    switch(c){
      
      case 'A': case 'C': case 'G': case 'T':
        fprintf(DNA,   "%c", c); 
        fprintf(EXTRA, "%c", 0); 
      break;

      case 'a': case 'c': case 'g': case 't':
        fprintf(DNA,   "%c", toupper(c));  
        fprintf(EXTRA, "%c", 1);
      break;
      
      default:
        fprintf(EXTRA, "%c", c);
      break;      
      }
    }

  x:

  if(!HEADERS) fclose(HEADERS);
  if(!EXTRA)   fclose(EXTRA);
  if(!DNA)     fclose(DNA);
  return EXIT_SUCCESS;
  }



