#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#define CHRNUM 25


void cumsum(const uint32_t array[], int n, uint32_t reArray[]);
void checkIntArray(uint32_t array[], int n);
void FatalError(char * String);

int main(const int argc, const char * argv[])
{
  FILE * bedfile;
  FILE * indexFile;
  char * indexFileName;
  char skip;
  unsigned char * array;
  uint16_t chr;
  uint32_t chrstart, chrstop, pos, start, stop, n_Byte, Mask;
  uint32_t chrlen[CHRNUM] = {249250621, 243199373, 198022430, 191154276, 180915260, 171115067,
                        159138663, 146364022, 141213431, 135534747, 135006516, 133851895,
                        115169878, 107349540, 102531392, 90354753, 81195210, 78077248,
                        59128983, 63025520, 48129895, 51304566, 155270560, 59373566, 16571
                        };
  unsigned char Bytebit[8]={0x1,0x2,0x4,0x8,0x10,0x20,0x40,0x80};
  uint32_t genomeByte, genome;
  //checkIntArray(chrlen, CHRNUM);
  uint32_t chrSum[CHRNUM];
  cumsum(chrlen, CHRNUM, chrSum);
  genome = chrSum[CHRNUM-1] + chrlen[CHRNUM-1];
  if(genome % 8 ==0)
  {
    genomeByte = (chrSum[CHRNUM-1] + chrlen[CHRNUM-1])>>3;
  }else{
    genomeByte = ((chrSum[CHRNUM-1] + chrlen[CHRNUM-1])>>3) + 1;
  }
  //printf("%u\n", chrSum[CHRNUM-1] + chrlen[CHRNUM-1]);
  array = (unsigned char *) malloc(genomeByte);
  for(int i=0; i<genomeByte; i++)
    array[i]=0;
  if(array==NULL) FatalError("out of space");
  //checkIntArray(chrSum, CHRNUM);
  if((bedfile=fopen(argv[1], "rt"))==NULL)
    FatalError("bedfile can't be open");
  indexFileName = (char *) malloc(strlen(argv[1])+5);
  strncpy(indexFileName, argv[1], strlen(argv[1]));
  strncat(indexFileName, ".arr", 5);
  if((indexFile=fopen(indexFileName, "wb"))==NULL)
    FatalError("file can't be open to write");
  while(true)
  {
    if(fscanf(bedfile, "%I16u %I32u %I32u", &chr, &chrstart, &chrstop) != 3) break;
    while((skip=getc(bedfile)) != '\n') continue;
    pos = chrSum[chr-1] + chrstart - 1;
    stop = chrSum[chr-1] + chrstop - 1;
    while(pos <= stop)
    {
      n_Byte = pos>>3;
      Mask = pos & 7;
      array[n_Byte] |= Bytebit[Mask];
      pos++;
    }
  }
  fclose(bedfile);

if(argc > 2 && !strcmp(argv[2], "-view"))
{
  FILE * file;
  char ch;
  file = fopen("index.view", "wt");
  for(int i=0; i<genomeByte; i++)
  {
    for(int j=0;j<8;j++)
    {
      ch = ((array[i]>>j) & 0x1) + '0';
      putc(ch, file);
    }
    putc('\n', file);
  }
}
  fwrite(&genomeByte, sizeof(genomeByte), 1, indexFile);
  fwrite(array, sizeof(unsigned char),genomeByte,indexFile);
  fclose(indexFile);
  free(array);
  free(indexFileName);

/*
  array = (unsigned char *) malloc(genomeByte);
  FILE * readinFile;
  readinFile = fopen(indexFileName, "rb");
  //fseek(readinFile, 0L, SEEK_END);
  uint32_t flong;
  fread(&flong, sizeof(genomeByte), 1, readinFile); //ftell(readinFile);
  //printf("%u\n%u\n",flong, genomeByte);
  fread(array, sizeof(unsigned char), flong, readinFile);
*/

/*
  //FILE * file;
  //char ch;
  file = fopen("bbb.txt", "wt");
  for(int i=0; i<flong; i++)
  {
    for(int j=0;j<8;j++)
    {
      ch = ((array[i]>>j) & 0x1) + '0';
      putc(ch, file);
    }
    putc('\n', file);
  }
*/

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

void checkIntArray(uint32_t array[], int n)
{
  for(int i=0; i<n; i++)
    printf("%I32u\n", array[i]);
}
void FatalError(char * String)
{
  printf("FatalError: %s\n", String);
  exit(EXIT_FAILURE);
}





