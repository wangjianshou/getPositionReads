#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <zlib.h>
#include <ctype.h>
#define CHRNUM 25
#define LINE 1023

void FatalError(char * String);
void cumsum(const uint32_t array[], int n, uint32_t reArray[]);

int main(const int argc, const char * argv[])
{
  FILE * bedindex;
  uint32_t genomeByte, n_Byte, Mask, AbsPos, pos;
  //unsigned long int pos;
  unsigned int  chr,baseA,baseT,baseC,baseG;
  unsigned char * array;
  //char skip;
  unsigned char Bytebit[8]={0x1,0x2,0x4,0x8,0x10,0x20,0x40,0x80};
  uint32_t chrlen[CHRNUM] = {249250621, 243199373, 198022430, 191154276, 180915260, 171115067,
                             159138663, 146364022, 141213431, 135534747, 135006516, 133851895,
                             115169878, 107349540, 102531392, 90354753, 81195210, 78077248,
                             59128983, 63025520, 48129895, 51304566, 155270560, 59373566, 16571
                             };
  uint32_t chrSum[CHRNUM];
  cumsum(chrlen, CHRNUM, chrSum);
  bedindex = fopen(argv[1], "rb");
  fread(&genomeByte, sizeof(genomeByte), 1, bedindex);
  array = (unsigned char *) malloc(genomeByte);
  if(array==NULL)
    FatalError("out of space");
  fread(array, sizeof(unsigned char), genomeByte, bedindex);


  /*
  printf("%lu\n",genomeByte);
  FILE * viewfile;
  char ch;
  viewfile = fopen("use_index.view", "wt");
  for(int i=0; i<genomeByte; i++)
  {
    for(int j=0;j<8;j++)
    {
      ch = ((array[i]>>j) & 0x1) + '0';
      putc(ch, viewfile);
    }
    putc('\n', viewfile);
  }
  FILE * outfile;
  outfile=fopen("outfile", "wt");
  */
  //FILE * ff;
  //ff = fopen("aa.txt", "wb");
  gzFile outfile;
  gzFile infile;
  infile = gzopen(argv[2], "rb");
  outfile = gzopen(argv[3], "wb");
  char * line;
  line = (char *) malloc(sizeof(char) * 1024);
  if(line==NULL) FatalError("out of space");
  char chr_word[4];
  char pos_word[20];
  int i,j;
  while(gzgets(infile, line, LINE) != Z_NULL)
  {
    //if(gzgets(infile, line, LINE)==Z_NULL) FatalError("cant read gzfile");
    i=0;
    //printf("%d\n",i);
    while(isdigit(line[i])) 
    {
      chr_word[i]=line[i];
      i++;
    }
    chr_word[i]='\0';
    //puts(chr_word);
    while(isspace(line[i])) i++;

    /*
    char * char_ptr = &pos_word[0];
    while(isdigit(line[i]))
    {
      *char_ptr = line[i++];
      char_ptr++;
    }
    *char_ptr = '\0';
    */

    j=0;
    while(isdigit(line[i])) {pos_word[j++] = line[i++];}
    pos_word[j]='\0';
    //fprintf(ff, "j:%d\n", j);
    chr = atoi(chr_word);
    pos = strtoul(pos_word, NULL, 0);
    AbsPos = chrSum[chr-1] + pos - 1;
    n_Byte = AbsPos>>3;
    Mask = AbsPos & 7;
    if(Bytebit[Mask] & array[n_Byte])
      gzputs(outfile, line);
    //fprintf(ff, "%s\n", pos_word);
    //fflush(ff);
  }
  free(array);
  free(line);
  fclose(bedindex);
  gzclose(infile);
  gzclose(outfile);
  //fclose(viewfile);

}



void FatalError(char * String)
{
  printf("FatalError: %s\n", String);
  exit(EXIT_FAILURE);
}

void cumsum(const uint32_t array[], int n, uint32_t reArray[])
{
  reArray[0] = 0;
  for(int i=0, sum=0; i<n-1;)
  {
    sum += array[i++];
    reArray[i] = sum;
  }
}

