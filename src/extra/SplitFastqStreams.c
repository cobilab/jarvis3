#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// This application splits FASTQ into four channels of information: 
//        * HEADERS;
//        * DNA.
//        * N's
//        * QUALITY-SCORES.

int main(int argc, char *argv[]){

  FILE *HEADERS, *N, *DNA, *QUALITIES;
  int c;

  if((HEADERS = fopen("HEADERS.JV2", "w")) == NULL){
    fprintf(stderr, "Error: could not open file!");
    return 1;
    }

  if((N = fopen("N.JV2", "w")) == NULL){
    fprintf(stderr, "Error: could not open file!");
    return 1;
    }

  if((DNA = fopen("DNA.JV2", "w")) == NULL){
    fprintf(stderr, "Error: could not open file!");
    return 1;
    }

  if((QUALITIES = fopen("QUALITIES.JV2", "w")) == NULL){
    fprintf(stderr, "Error: could not open file!");
    return 1;
    }

  z:
  while((c = fgetc(stdin)) != EOF){

    while((c = fgetc(stdin)) != EOF){
      if(c == EOF) goto x;
      fprintf(HEADERS, "%c", c);
      if(c == '\n') break;
      }

    while((c = fgetc(stdin)) != EOF){
      switch(c){
        case EOF:  goto x; break;
	case '\n': goto y; break;
	case 'N':  fprintf(N, "b"); break;
        default:   fprintf(N, "a"); fprintf(DNA, "%c", c); break;	     
        }
      }
      
    y:

    while((c = fgetc(stdin)) != EOF){
      if(c == EOF)  goto x;
      if(c == '\n') break;
      }

    while((c = fgetc(stdin)) != EOF){
      switch(c){
        case EOF:  goto x; break;
	case '\n': fprintf(QUALITIES, "\n"); goto z; break;
        default:   fprintf(QUALITIES, "%c", c); break;
        }
      }
    }

  x:

  if(!HEADERS)   fclose(HEADERS);
  if(!N)         fclose(N);
  if(!DNA)       fclose(DNA);
  if(!QUALITIES) fclose(QUALITIES);
 
  return EXIT_SUCCESS;
  }



