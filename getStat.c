#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <zlib.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include "module.c"

#define KB 1024
#define CHRNUM 25
#define LINE 1023

//usage: getStat bedindex mpileupFile filterFile


unsigned char * getBedIndex(const char bedindex_char[]);
void cumsum(const uint32_t array[], int n, uint32_t reArray[]);

int main(const int argc, const char * argv[])
{ 
  unsigned int chr_pos, depth, chr, numA, numT, numC, numG, numRef, indel, is_target;
  char chr_str[6];
  char *info, *tmp;
  char refbase, info_char, skip, not_space;
  unsigned char * array, * targetArray;


  uint32_t n_Byte, Mask, pos;
  unsigned char Bytebit[8] = {0x1,0x2,0x4,0x8,0x10,0x20,0x40,0x80};
  uint32_t chrlen[CHRNUM] = {249250621, 243199373, 198022430, 191154276, 180915260, 171115067,
                             159138663, 146364022, 141213431, 135534747, 135006516, 133851895,
                             115169878, 107349540, 102531392, 90354753, 81195210, 78077248,
                             59128983, 63025520, 48129895, 51304566, 155270560, 59373566, 16571
                            };
  uint32_t chrSum[CHRNUM];
  cumsum(chrlen, CHRNUM, chrSum);


  //argv[1] is bedfile array, argv[2] is mpileup file, argv[3] is filter file, argv[4] is target bed file index
  FILE *mpileup;
  gzFile filterFile;
  //gzFile statFile;
  mpileup = (argv[2][0]=='-')? stdin: fopen(argv[2], "r");
  if(mpileup==NULL)
    FatalError("mpileup file cant open;");
  //if((statFile = gzopen(argv[3], "wb"))==NULL)
  //  FatalError("statFile cant open;");
  if((filterFile = gzopen(argv[3], "wb"))==NULL)
    FatalError("filterFile cant open;");

  if((info = (char *) malloc(50 * KB * sizeof(char)))==NULL)
    FatalError("out of space");
  array = getBedIndex(argv[1]);
  targetArray = getBedIndex(argv[4]);
  //int aa=0, bb;
  while((not_space=getc(mpileup)) != EOF)
  {
    //fseek(mpileup, -1L, SEEK_CUR);
    ungetc(not_space, mpileup);
    numRef=numA=numT=numC=numG=0;
    if(fscanf(mpileup, "%s %u %c %u", chr_str, &chr_pos, &refbase, &depth) != 4)
      FatalError("number of fields conflict1");
    if(getc(mpileup) != '\t') fputs("there's a tab missing\n", stderr);
    //printf("aa:%d\nbb:%d\n", aa, bb);
    if((not_space=getc(mpileup)) == '\t')
    {
      while(getc(mpileup) != '\n') continue;
      continue;
    }
    else
      ungetc(not_space, mpileup);
      //fseek(mpileup, -1L, SEEK_CUR);
    if(fscanf(mpileup, "%s", info) != 1)
      FatalError("number of fields conflict2");
    //printf("%s\t%u\t%c\t%u\n", chr_str,chr_pos,refbase,depth);
    while(getc(mpileup) != '\n') continue;
    switch(chr_str[3])
    {
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        chr = atoi(chr_str+3); break;
      case 'M':
        chr = 25; break;
      case 'X':
        chr = 23; break;
      case 'Y':
        chr = 24; break;
      default:
        break;
    }

    pos = chrSum[chr-1] + chr_pos - 1;
    n_Byte = pos>>3;
    Mask = pos & 7;
    if(!(Bytebit[Mask] & array[n_Byte]))
      continue;
    //if(depth < 2) continue;
    is_target = (Bytebit[Mask] & targetArray[n_Byte])? 1: 0;

    tmp = &info[0];
    while((info_char = toupper(*tmp++)) != '\0')
    {
      switch(info_char)
      {
        case ',':
        case '.':
          numRef++; break;
        case 'A':
          numA++; break;
        case 'T':
          numT++; break;
        case 'C':
          numC++; break;
        case 'G':
          numG++; break;
        case '^':
          tmp++; break;
        case '$':
          break;
        case '+':
        case '-':
          indel = atoi(tmp);
          while(isdigit(*tmp)) tmp++;
          while(indel-- > 0) tmp++;
          break;
        default:
          break;
      }
    }
    switch(toupper(refbase))
    {
      case 'A': numA = numRef; break;
      case 'T': numT = numRef; break;
      case 'C': numC = numRef; break;
      case 'G': numG = numRef; break;
      default: break;
    }
    gzprintf(filterFile, "%u\t%u\t%u\t%u\t%u\t%u\t%u\n", chr, chr_pos, numA, numT, numC, numG, is_target);
  }
  gzclose(filterFile);
  //gzclose(statFile);
  return 0;
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

unsigned char * getBedIndex(const char bedindex_char[])
{
  FILE * bedindex;
  uint32_t genomeByte;
  unsigned char * array;
  bedindex = fopen(bedindex_char, "rb");
  fread(&genomeByte, sizeof(genomeByte), 1, bedindex);
  array = (unsigned char *) malloc(genomeByte);
  if(array==NULL) FatalError("out of space");
  fread(array, sizeof(unsigned char), genomeByte, bedindex);
  fclose(bedindex);
  return array;
}














