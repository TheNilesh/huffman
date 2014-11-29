/* Title: Huffman Coding for files
   Date_start: 25/11/2014
   Author: niLesh	*/

/* Algorithm v2
[Pass1]
1. Read input file
2. Create sorted linked list of characters from file, as per character frequency
   for eah character ch from file
	if( ch available in linked list at node p) then 
	{
		p.freq++;
		sort Linked list as per node's freq;
	}
	else
		add new node at beginning of linked list with frequency=1;

3. Construct huffman tree from linked list
   i.Create new node q, join two least freq nodes to its left and right
   ii.insert created node q into ascending list
   iii. repeat i & ii till only one nodes remains, i.e, ROOT of h-tree
   iv. Traverse tree in preorder mark each node with its codeword. simultaneously Recreate linked list of leaf nodes.

[Pass2]
4. Write Mapping Table(character to codeword) to output file.
5. Read input file.
6. Write codeword in place of each character in input file to output file
   for each character ch from input file
	write corresponding codeword into o/p file (lookup in mapping table OR linked list)
7. End
DONE: '#' Should not be tag for internal nodes, identify internal node with some other tag
TODO: read input files in binary mode.
TODO: Make program efficient by sorting linked list in descending order.(high speed searching)
*/

#include<stdio.h>
#include<malloc.h>
#include<string.h>
#define MAX 9
#define BUFFER_NOT_WRITTEN -1
#define BUFFER_WRITTEN 0
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
int writeCode(char ch,FILE *f);
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
fp=fopen(argv[1],"r");
if(fp==NULL)
{ printf("File open error.");	return -1; }

printf("\n[Pass1]");
printf("\nReading file %s",argv[1]);
	while((ch=getc(fp))!=EOF)
	{
		addSymbol(ch);
		//printf("%c",ch);
	}
	fclose(fp);
printf("\nReading frequencies of characters.");
//printll();
printf("\nConstructing Huffman-Tree..");
makeTree();
printf("\nAssigning Codewords.\n");
genCode(ROOT,"\0");	//preorder traversal
printf("\n[Pass2]");
printf("\nReading file %s",argv[1]);
printf("\nWriting file %s.hzip",argv[1]);
fp=fopen(argv[1],"r");
fp2=fopen(strcat(argv[1],".hzip"),"wb");
printf("\nWriting Header info");
	writeHeader(fp2);
printf("\nWriting compressed content.");
	while((ch=getc(fp))!=EOF)
		t=writeCode(ch,fp2);	//write corresponding code into fp2
	if(t==BUFFER_NOT_WRITTEN)
		printf("\n[!]Some bits(<8) have not been written to file.");
	fclose(fp);
	fclose(fp2);
printf("\nDone..\n");
return 0;
}

void writeHeader(FILE *f)
{//Table mapping 'codewords' to actual symbols
typedef struct symCode
{ char x;
  char code[MAX];  //Max. codeword length=8,+1 for '\0', so MAX=9, TODO: Sometimes Max length of codeword >=12, How??..Handle that case
}symCode;
symCode xyz;
node *p;
int temp=0;
unsigned char padding,i=0;	//TODO: i shouldnt be int, because max no. of char(byte)=2^8=256, but i can have max value=255,here
p=HEAD;
while(p!=NULL)	//determine number of unique symbols and padding of bits
{
	temp+=(strlen(p->code)) * (p->freq);		//temp stores padding
	temp%=8;
	i++;
	p=p->next;
}
fwrite(&i,sizeof(char),1,f);	//read these many structures while reading
//printf("\nN=%d",i);

p=HEAD;
while(p!=NULL)	//start from HEAD, write each char & its code
{
	xyz.x=p->x;
	strcpy(xyz.code,p->code);
	fwrite(&xyz,sizeof(xyz),1,f);
//	printf("\n%c|%s",xyz.x,xyz.code);
	p=p->next;
}
//discard 'padding' bits before data, while reading
padding=8-(char)temp;	//int to char & padding = 8-bitsExtra
fwrite(&padding,sizeof(char),1,f);
//printf("\npadding=%d",padding);
//do actual padding
for(i=0;i<padding;i++)
	writeBit(0,f);

}//fun

int writeCode(char ch,FILE *f)
{
char *code;
int t;
code=getCode(ch);
//printf("\nwriting %s",code);
	while(*code!='\0')
	{
		if(*code=='1')
			t=writeBit(1,f); //write bit 1 into file f
		else
			t=writeBit(0,f);
	code++;
	}
	return t;
}

int writeBit(int b,FILE *f)
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
		fwrite(&byte,sizeof(char),1,f);
		cnt=0; byte=0;	//reset buffer
		return BUFFER_WRITTEN; // buffer written to file
	}
	return BUFFER_NOT_WRITTEN;//buffer is not full, so not written
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

