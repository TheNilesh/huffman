#include<stdio.h>
#include<string.h>
#include<malloc.h>
#include "huffman.h"

symCode *symlist;
int n;
char *decodeBuffer(char buffer);
char *int2string(int n);
int match(char a[],char b[],int limit);
int fileError(FILE *fp);

int main(int argc,char *argv[])
{
FILE *fp,*outfile;
char buffer;
char *decoded;
int i;
if(argc<=2)
{
	printf("Usage:\n %s <file-to-unzip> <destination-filename>\n***Huffman File Decoder***\nAuthor: niLesh Akhade\nhttps://github.com/TheniL/huffman\n\n",argv[0]);
	if(argc==2)
	{
		argv[2]=(char *)malloc(sizeof(char)*(strlen(argv[1])+strlen(dext)+1));
		strcpy(argv[2],argv[1]);
		strcat(argv[2],dext);
		argc++;
	}
	else	
		return 0;
}
if((fp=fopen(argv[1],"rb"))==NULL)
{
	printf("[!]Input file cannot be opened.\n");
	return -1;
}

printf("\n[Reading File Header]");
if(fread(&buffer,sizeof(unsigned char),1,fp)==0) return (fileError(fp));
	N=buffer;		//No. of structures(mapping table records) to read
if(N==0)
	n=256;
else
	n=N;
printf("\nDetected: %u different characters.",n);

//allocate memory for mapping table
symlist=(symCode *)malloc(sizeof(symCode)*n);

printf("\nReading character to Codeword Mapping Table");
if(fread(symlist,sizeof(symCode),n,fp)==0) return (fileError(fp));
	//Read mapping table
/*
printf("\n");
for(i=0;i<n;i++)
	printf("[%c|%s] ",symlist[i].x,symlist[i].code);
*/

if(fread(&buffer,sizeof(char),1,fp)==0) return (fileError(fp));
	padding=buffer;		//No. of bits to discard
printf("\nDetected: %u bit padding.",padding);

if((outfile=fopen(argv[2],"wb"))==NULL)
{
	printf("[!]Output file cannot be opened.\n");
	fclose(fp);
	return -2;
}

printf("\n\n[Reading data]\nReplacing codewords with actual characters");
while(fread(&buffer,sizeof(char),1,fp)!=0)	//Read 1 byte at a time
{
	decoded=decodeBuffer(buffer);	//decoded is pointer to array of characters read from buffer byte
	i=0;
	while(decoded[i++]!='\0');	//i-1 characters read into decoded array
	//printf("%s",decoded);
	fwrite(decoded,sizeof(char),i-1,outfile);
}
fclose(fp);
fclose(outfile);
printf("\nEverything fine..\nOutput file %s written successfully.\n",argv[2]); 
return 0;
}

char *decodeBuffer(char b)
{
int i=0,j=0,t;
static int k;
static int buffer;	// buffer larger enough to hold two b's
char *decoded=(char *)malloc(MAX*sizeof(char));
/*
Logic:
buffer = [1 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0]
		k
   b   =        [ 1 0 1 1 0 0 1 1 ]
//put b in integer t right shift k+1 times then '&' with buffer; k=k+8;
buffer = [1 0 0 1 1 0 1 1 0 0 1 1 0 0 0 0]
				k
*/

t=(int)b;
//printf("\nt=%sk=%d",int2string(t),k);
t=t & 0x00FF;		//mask high byte
//printf("\nt=%sk=%d",int2string(t),k);
t=t<<8-k;		//shift bits keeping zeroes for old buffer 
//printf("\nt=%sk=%d",int2string(t),k);
buffer=buffer | t;	//joined b to buffer
k=k+8;			//first useless bit index +8 , new byte added

if(padding!=0)	// on first call
{buffer=buffer<<padding;
 k=8-padding;	//k points to first useless bit index
 padding=0;}

//printf("\nbuffer=%s, k=%d",int2string(buffer),k);
//loop to find matching codewords

while(i<n)
{
	if(!match(symlist[i].code, int2string(buffer),k))
	{	
		decoded[j++]=symlist[i].x;	//match found inserted decoded
		t=strlen(symlist[i].code);	//matched bits
		buffer=buffer<<t;		//throw out matched bits
		k=k-t;				//k will be less
		i=0;				//match from initial record
		//printf("\nBuffer=%s,removed=%c,k=%d",int2string(buffer),decoded[j-1],k);
		if(k==0) break;
		continue;
	}
i++;
}

decoded[j]='\0';
return decoded;

}//fun

int match(char a[],char b[],int limit)
{
	b[strlen(a)]='\0';
	b[limit]='\0';
	return strcmp(a,b);
}

char *int2string(int n)
{
int i,k,and,j;
char *temp=(char *)malloc(16*sizeof(char));
j=0;

for(i=15;i>=0;i--)
{
	and=1<<i;
	k=n & and;
	if(k==0) temp[j++]='0'; else temp[j++]='1';
}
temp[j]='\0';
return temp;
}

int fileError(FILE *fp)
{
	printf("[!]File read Error.\n[ ]File is not compressed using huffman.\n");
	fclose(fp);
	return -3;
}
