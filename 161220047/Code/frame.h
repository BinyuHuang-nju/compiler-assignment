#ifndef __FRAME_H__
#define __FRAME_H__

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "semantic.h"
extern int total_error;

//#define SYNTAX_TREE_ENABLED   //lab1_output
#define SEMANTIC_ENABLED      	//lab2
//#define SYMBOL_TABLE_ENABLED  //lab2_output
#define IR_ENABLED            	//lab3
//#define IR_OUTPUT_ENABLED       //lab3_output
#define OC_ENABLED            	//lab4


#define MAX_TOKEN_SIZE 32

typedef int bool;
enum {false,true};

enum Type{ INT_,FLOAT_,ID_,SEMI_,COMMA_,ASSIGNOP_,RELOP_,PLUS_,
		MINUS_,STAR_,DIV_,AND_,OR_,DOT_,NOT_,TYPE_,LP_,RP_,LB_,RB_,
		LC_,RC_,STRUCT_,RETURN_,IF_,ELSE_,WHILE_,

		Program_,ExtDefList_,ExtDef_,ExtDecList_,Specifier_,
		StructSpecifier_,OptTag_,Tag_,VarDec_,FunDec_,VarList_,
		ParamDec_,CompSt_,StmtList_,Stmt_,DefList_,Def_,DecList_,
		Dec_,Exp_,Args_,
		
		NULL_
};

struct TreeNode
{
	enum Type type;
	int lineno;
	char *name;
	union
	{
		int i;
		float f;
		char c[MAX_TOKEN_SIZE];
	}value;
	struct TreeNode* first_child;
	struct TreeNode* sibling;
	struct Type_* exp_type;
};
extern struct TreeNode* Root;

extern struct TreeNode* createNode(enum Type type,char* text,int lineno);

extern void printTree(struct TreeNode* root,int layer);

extern void connect_PoneC(struct TreeNode* parent,struct TreeNode* child);
extern void connect_PtwoC(struct TreeNode* parent,struct TreeNode* child_1,struct TreeNode* child_2);
extern void connect_PthreeC(struct TreeNode* parent,struct TreeNode* child_1,struct TreeNode* child_2,struct TreeNode* child_3);
extern void connect_PfourC(struct TreeNode* parent,struct TreeNode* child_1,struct TreeNode* child_2,struct TreeNode* child_3,struct TreeNode* child_4);
extern void connect_PfiveC(struct TreeNode* parent,struct TreeNode* child_1,struct TreeNode* child_2,struct TreeNode* child_3,struct TreeNode* child_4,struct TreeNode* child_5);
extern void connect_PsevenC(struct TreeNode* parent,struct TreeNode* child_1,struct TreeNode* child_2,struct TreeNode* child_3,struct TreeNode* child_4,struct TreeNode* child_5,struct TreeNode* child_6,struct TreeNode* child_7);
#endif
