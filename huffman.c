/* Title: Huffman Coding for files
   Date: 25/11/2014*/

/* Algorithm
1. Read input file
2. Record character frequency
3. Construct huffman tree
   i. Arrange char freq in ascending list
   ii.Create new node, add two least freq nodes to left and right
   iii.insert created node into ascending list
   iv. repeat ii & iii till no nodes remains
   v. Traverse tree in preorder and create new list with character codes at each.
4. Read input file [Pass2]
5. Write frequency table into output file
6. write code in place of each character into new file
7. End
*/

#include<stdio.h>
#include<malloc.h>
#include<string.h>

typedef struct node
{
	char x;
	int freq;
	char *code;
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
void genCode(node *p,char* code);

node* newNode(char c)
{
	node *q;
	q=(node *)malloc(sizeof(node));
	q->x=c;
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
printf("\nFrequency List:\n");
printll();
printf("\nConstructing Huffman-Tree..");
makeTree();
printf("\nTree Constructed..\nPreorder Traversal of H-Tree\n");
genCode(ROOT,"\0");
printf("\n\n[Pass2]");
printf("\nReading file %s",argv[1]);
printf("\nOpening file %s.hzip",argv[1]);
fp=fopen(argv[1],"r");
fp2=fopen(strcat(argv[1],".hzip"),"wb");
printf("\nWriting Header info");
	//freqtbl(fp2);
printf("\nWriting compressed content.");
	while((ch=getc(fp))!=EOF)
	{
		//fwrite(getCode(ch));
	}
	fclose(fp);
	fclose(fp2);
printf("\nDone..\n");
return 0;
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
{
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
		q=newNode('#');
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
	if(p!=NULL)
	{
	p->code=code;	//assign code to current node
	printf("[%c|%d|%s]",p->x,p->freq,p->code);
	lcode=(char *)malloc(strlen(code)+2);		//optimal memory allocation
	rcode=(char *)malloc(strlen(code)+2);
	sprintf(lcode,"%s0",code);
	sprintf(rcode,"%s1",code);
	genCode(p->left,lcode);		//left child has 0 appended to current node's code
	genCode(p->right,rcode);
	}
}
