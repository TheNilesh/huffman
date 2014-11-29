#include<stdio.h>
#include<string.h>
#include<malloc.h>
#include "bitmanip.c"
#define MAX 16
#define NEED_MORE_BITS 1

typedef struct symCode
{ char x;
  char code[MAX];
}symCode;

symCode *symlist;
int decodeBuffer(char *ch,char buffer);

int main(int argc,char *argv[])
{
FILE *fp,*outfile;
char buffer,ch,padding;
int t,i;
char N;

fp=fopen(argv[1],"rb");
if(fread(&buffer,sizeof(char),1,fp)!=0)
	N=buffer;		//No. of structures to read;

symlist=(symCode *)malloc(sizeof(symCode)*N);	//allocate memory for structures

if(fread(symlist,sizeof(symCode),N,fp)==0)	//Read all structures
{ fclose(fp); return; }

for(i=0;i<N;i++)
      printf("\n%c|%s",symlist[i].x,symlist[i].code);

if(fread(&buffer,sizeof(char),1,fp)!=0)
	padding=buffer;		//No. of bits to discard

outfile=fopen(argv[2],"wb");
while(fread(&buffer,sizeof(char),1,fp)!=0)	//Read 1 byte at a time
{
	printf("\n");
	showbits(buffer);
	/*while(decodeBuffer(&ch,buffer)!=NEED_MORE_BITS)	//one buffer can have multiple ch symbols
		fwrite(&ch,sizeof(char),1,outfile);*/
}
fclose(fp);
fclose(outfile);
return 0;
}

int decodeBuffer(char *ch,char buffer)
{

return 2;
}


