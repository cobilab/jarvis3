#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

// This application merges four channels of information: 
//        * HEADERS;
//        * DNA;
//        * N's
//        * QUALITY-SCORES;
// into a FASTQ file.

int main(int argc, char *argv[]){

  FILE *HEADERS, *N, *DNA, *QUALITIES;
  int c;

  if((HEADERS = fopen("HEADERS.JV3", "r")) == NULL){
    fprintf(stderr, "Error: could not open file!");
    return 1;
    }

  if((N = fopen("N.JV3", "r")) == NULL){
    fprintf(stderr, "Error: could not open file!");
    return 1;
    }

  if((DNA = fopen("DNA.JV3", "r")) == NULL){
    fprintf(stderr, "Error: could not open file!");
    return 1;
    }

  if((QUALITIES = fopen("QUALITIES.JV3", "r")) == NULL){
    fprintf(stderr, "Error: could not open file!");
    return 1;
    }

  // FIXME: IF READ LARGER THAN 2000000 PROBLEMS WILL OCCUR 
  uint64_t idx = 0, idx2 = 0, n, max = 1000000;
  int qual_str[max+1];
  int n_str   [max+1];

  while((c = fgetc(HEADERS)) != EOF){
      
    fprintf(stdout, "@");
    
    do{  
      if(c == EOF)  break;
      fprintf(stdout, "%c", c);
      if(c == '\n') break;
      }
    while((c = fgetc(HEADERS)) != EOF);

    idx = 0;
    while((c = fgetc(QUALITIES)) != EOF){
      if(c == EOF || c == '\n') break;
      else if(idx < max) qual_str[idx++] = c;
      }
    qual_str[idx] = '\n';

    idx2 = 0;
    while((c = fgetc(N)) != EOF){
      if(c == EOF) break;
      if(idx2 < max) n_str[idx2++] = c;
      if(idx2 >= idx) break;
      }

    for(n = 0 ; n < idx2 ; ++n){
      if(n_str[n] == 'a'){
	c = fgetc(DNA);
        if(c == EOF) break;
	else fprintf(stdout, "%c", c); // WRITE DNA
        }
      else fprintf(stdout, "N"); // WRITE N
      }

    fprintf(stdout, "\n+\n");
    for(n = 0 ; n <= idx ; ++n) // WRITE QUALS
      fprintf(stdout, "%c", qual_str[n]);
    }
    
  if(!HEADERS)   fclose(HEADERS);
  if(!N)         fclose(N);
  if(!DNA)       fclose(DNA);
  if(!QUALITIES) fclose(QUALITIES);

  return EXIT_SUCCESS;
  }



