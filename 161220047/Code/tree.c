#include "frame.h"

struct TreeNode* Root = NULL;

struct TreeNode* createNode(enum Type type,char* text,int lineno)
{
	struct TreeNode* newNode = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	newNode->lineno = lineno;
	newNode->type = type;
	newNode->first_child = newNode->sibling = NULL;
	newNode->name = malloc(strlen(text)+1);
	strcpy(newNode->name,text);
	return newNode;
}

void printTree(struct TreeNode* root,int layer)
{
	if(root!=NULL)
	{
		for(int i = 0;i < layer;i++)
			printf("  ");
//		printf("%s (%d)\n",root->name,root->lineno);
		if(root->type == ID_)
			printf("ID:%s (%d)\n",root->name,root->lineno);
		else if(root->type == TYPE_)
			printf("TYPE:%s (%d)\n",root->name,root->lineno);
		else if(root->type == INT_)
			printf("INT:%d (%d)\n",root->value.i,root->lineno);
		else if(root->type == FLOAT_)
			printf("FLOAT:%.6f (%d)\n",root->value.f,root->lineno);
		else
			printf("%s (%d)\n",root->name,root->lineno);
		struct TreeNode* temp = root->first_child;
		while(temp!=NULL)
		{
			printTree(temp,layer+1);
			temp = temp->sibling;
		}
	}
}

void connect_PoneC(struct TreeNode* parent,struct TreeNode* child)
{
	parent->first_child = child;
}
void connect_PtwoC(struct TreeNode* parent,struct TreeNode* child_1,struct TreeNode* child_2)
{
	parent->first_child = child_1;
	child_1->sibling = child_2;
}
void connect_PthreeC(struct TreeNode* parent,struct TreeNode* child_1,struct TreeNode* child_2,struct TreeNode* child_3)
{
	parent->first_child = child_1;
	child_1->sibling = child_2;
	child_2->sibling = child_3;
}
void connect_PfourC(struct TreeNode* parent,struct TreeNode* child_1,struct TreeNode* child_2,struct TreeNode* child_3,struct TreeNode* child_4)
{
	parent->first_child = child_1;
	child_1->sibling = child_2;
	child_2->sibling = child_3;
	child_3->sibling = child_4;
}
void connect_PfiveC(struct TreeNode* parent,struct TreeNode* child_1,struct TreeNode* child_2,struct TreeNode* child_3,struct TreeNode* child_4,struct TreeNode* child_5)
{
	parent->first_child = child_1;
	child_1->sibling = child_2;
	child_2->sibling = child_3;
	child_3->sibling = child_4;
	child_4->sibling = child_5;
}
void connect_PsevenC(struct TreeNode* parent,struct TreeNode* child_1,struct TreeNode* child_2,struct TreeNode* child_3,struct TreeNode* child_4,struct TreeNode* child_5,struct TreeNode* child_6,struct TreeNode* child_7)
{
	parent->first_child = child_1;
	child_1->sibling = child_2;
	child_2->sibling = child_3;
	child_3->sibling = child_4;
	child_4->sibling = child_5;
	child_5->sibling = child_6;
	child_6->sibling = child_7;
}
