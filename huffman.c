/* Title: Huffman Coding for files
   URL: htts://github.com/TheniL/huffman
   Date_start: 25/11/2014
   OC 1: 01/12/2014
   Author: niLesh	*/
/*
TODO: handle Codewords longer than MAX
TODO: Use free() to deallocate memory
TODO: sort linked list in non-increasing order, at genCode().
TODO: Store code in bit array form(in Header), rather than string form, to save space
*/

#include<stdio.h>
#include<malloc.h>
#include<string.h>
#include "huffman.h"
#define INTERNAL 1
#define LEAF 0

typedef struct node
{
	char x;
	int freq;
	char *code;
	int type;
	struct node *next;
	struct node *left;
	struct node *right;
}node;

node *HEAD,*ROOT;

void printll();
void makeTree();
void genCode(node *p,char* code);
void insert(node *p,node *m);
void addSymbol(char c);
void writeHeader(FILE *f);
void writeBit(int b,FILE *f);
void writeCode(char ch,FILE *f);
char *getCode(char ch);

node* newNode(char c)
{
	node *q;
	q=(node *)malloc(sizeof(node));
	q->x=c;
	q->type=LEAF;	//leafnode
	q->freq=1;
	q->next=NULL;
	q->left=NULL;
	q->right=NULL;
	return q;
}


void printll()
{
node *p;
p=HEAD;

 while(p!=NULL)
 {
   printf("[%c|%d]=>",p->x,p->freq);
   p=p->next;
 }
}

int main(int argc, char *argv[])
{
FILE *fp,*fp2;
char ch;
int t;
HEAD=NULL;
ROOT=NULL;
if(argc<=2)
{
	printf("Usage:\n %s <input-file-to-zip> <zipped-output-file>\n***Huffman File Encoder***\nAuthor: niLesh Akhade\nhttps://github.com/TheniL/huffman\n\n",argv[0]);
	if(argc==2)
	{
		argv[2]=(char *)malloc(sizeof(char)*(strlen(argv[1])+strlen(ext)+1));
		strcpy(argv[2],argv[1]);
		strcat(argv[2],ext);
		argc++;
	}
	else
		return 0;
}
fp=fopen(argv[1],"rb");
if(fp==NULL)
{
	printf("[!]Input file cannot be opened.\n");
	return -1;
}

printf("\n[Pass1]");
printf("\nReading input file %s",argv[1]);
	while(fread(&ch,sizeof(char),1,fp)!=0)
		addSymbol(ch);
	fclose(fp);

printf("\nConstructing Huffman-Tree..");
makeTree();
printf("\nAssigning Codewords.\n");
genCode(ROOT,"\0");	//preorder traversal

printf("\n[Pass2]");
fp=fopen(argv[1],"r");
if(fp==NULL)
{
	printf("\n[!]Input file cannot be opened.\n");
	return -1;
}
fp2=fopen(argv[2],"wb");
if(fp2==NULL)
{
	printf("\n[!]Output file cannot be opened.\n");
	return -2;
}

printf("\nReading input file %s",argv[1]);
printf("\nWriting file %s",argv[2]);
printf("\nWriting File Header.");
	writeHeader(fp2);
printf("\nWriting compressed content.");
	while(fread(&ch,sizeof(char),1,fp)!=0)
	{	
		//printf("\n%c replaced with ",ch);
		writeCode(ch,fp2);	//write corresponding code into fp2
	}
fclose(fp);
fclose(fp2);

printf("\nDone..\n");
return 0;
}

void writeHeader(FILE *f)
{//Table mapping 'codewords' to actual symbols
symCode record;
node *p;
int temp=0,i=0;
p=HEAD;
while(p!=NULL)	//determine number of unique symbols and padding of bits
{
	temp+=(strlen(p->code)) * (p->freq);		//temp stores padding
	if(strlen(p->code) > MAX) printf("\n[!] Codewords are longer than usual.");	//TODO: Solve this case
	temp%=8;
	i++;
	p=p->next;
}

if(i==256)
	N=0;	//if 256 diff bit combinations exist, then alias 256 as 0
else 
	N=i;

fwrite(&N,sizeof(unsigned char),1,f);	//read these many structures while reading
printf("\nN=%u",i);

p=HEAD;
while(p!=NULL)	//start from HEAD, write each char & its code
{
	record.x=p->x;
	strcpy(record.code,p->code);
	fwrite(&record,sizeof(symCode),1,f);
//	printf("\n%c|%s",record.x,record.code);
	p=p->next;
}
//discard 'padding' bits before data, while reading
padding=8-(char)temp;	//int to char & padding = 8-bitsExtra
fwrite(&padding,sizeof(char),1,f);
printf("\nPadding=%d",padding);
//do actual padding
for(i=0;i<padding;i++)
	writeBit(0,f);

}//fun

void writeCode(char ch,FILE *f)
{
char *code;
code=getCode(ch);
//printf("\n%s\n",code);
	while(*code!='\0')
	{
		if(*code=='1')
			writeBit(1,f); //write bit 1 into file f
		else
			writeBit(0,f);
	code++;
	}
	return;
}

void writeBit(int b,FILE *f)
{//My Logic: Maintain static buffer, if it is full, write into file 
	static char byte;
	static int cnt;
	char temp;
	//printf("\nSetting %dth bit = %d of %d ",cnt,b,byte);
	if(b==1)
	{	temp=1;
		temp=temp<<(7-cnt);		//right shift bits
		byte=byte | temp;
	}
	cnt++;
	
	if(cnt==8)	//buffer full
	{
//		printf("[%s]",bitsInChar(byte));
		fwrite(&byte,sizeof(char),1,f);
		cnt=0; byte=0;	//reset buffer
		return;// buffer written to file
	}
	return;
}

char *getCode(char ch)
{
node *p=HEAD;
	while(p!=NULL)
	{
	    if(p->x==ch)
		  return p->code;
	    p=p->next;
	}
	return NULL; //not found
}

void insert(node *p,node *m)
{ // insert p in list as per its freq., start from m to right,
// we cant place node smaller than m since we dont have ptr to node left to m
if(m->next==NULL)
{  m->next=p; return;}
	while(m->next->freq < p->freq)
	{  m=m->next;
	  if(m->next==NULL)
	    { m->next=p; return; }
	}
  p->next=m->next;
  m->next=p;
}

void addSymbol(char c)
{// Insert symbols into linked list if its new, otherwise freq++
node *p,*q,*m;
int t;

if(HEAD==NULL)
{	HEAD=newNode(c);
	return;
}
	p=HEAD; q=NULL;
if(p->x==c) //item found in HEAD
{
	p->freq+=1;
	if(p->next==NULL)
		return;
	if(p->freq > p->next->freq)
	{
		HEAD=p->next;
		p->next=NULL;
		insert(p,HEAD);
	}
	return;
}

while(p->next!=NULL && p->x!=c)
{
	q=p; p=p->next;
}

if(p->x==c)
{
	p->freq+=1;
        if(p->next==NULL)
		return;	
	if(p->freq > p->next->freq)
	{
		m=p->next;
		q->next=p->next;
		p->next=NULL;
		insert(p,HEAD);
	}
}
else  //p->next==NULL , all list traversed c is not found, insert it at beginning
{	
	q=newNode(c);
	q->next=HEAD;  //first because freq is minimum
	HEAD=q;
}
}

void makeTree()
{
node  *p,*q;
p=HEAD;
	while(p!=NULL)
	{
		q=newNode('@');
		q->type=INTERNAL;	//internal node
		q->left=p;		//join left subtree/node
		q->freq=p->freq;
		if(p->next!=NULL)
		{
			p=p->next;
			q->right=p;	//join right subtree /node
			q->freq+=p->freq;
		}
		p=p->next;	//consider next node frm list
		if(p==NULL)	//list ends
			break;
		//insert new subtree rooted at q into list starting from p
		//if q smaller than p
		if(q->freq <= p->freq)
		{//place it before p
			q->next=p;
			p=q;
		}
		else
			insert(q,p);	//find appropriate position
	}//while
	ROOT=q; //q created at last iteration is ROOT of h-tree
}

void genCode(node *p,char* code)
{
char *lcode,*rcode;
static node *s;
static int flag;
if(p!=NULL)
{
//sort linked list as it was
	if(p->type==LEAF)   //leaf node
	{	if(flag==0) //first leaf node
		{flag=1; HEAD=p;}
		else	//other leaf nodes
		{ s->next=p;}		//sorting LL
		p->next=NULL;
		s=p;
	}

//assign code
	p->code=code;	//assign code to current node
//	printf("[%c|%d|%s|%d]",p->x,p->freq,p->code,p->type);
	lcode=(char *)malloc(strlen(code)+2);
	rcode=(char *)malloc(strlen(code)+2);
	sprintf(lcode,"%s0",code);
	sprintf(rcode,"%s1",code);
//recursive DFS
	genCode(p->left,lcode);		//left child has 0 appended to current node's code
	genCode(p->right,rcode);
}
}

