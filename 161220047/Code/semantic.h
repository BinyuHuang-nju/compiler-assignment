#ifndef __SEMANTIC_H__
#define __SEMANTIC_H__
#include "frame.h"


#define MAX_NAME_SIZE 32
#define MAX_SYMBOL_SIZE 128
#define MAX_FIELD_SIZE 10
typedef struct Type_* Semtype;
typedef struct FieldList_* FieldList;
typedef struct Function_* Function;

struct Type_
{
	enum {BASIC, ARRAY, STRUCTURE ,INIT} kind;
	int lineno;
	union
	{
		enum { BASIC_INT,BASIC_FLOAT}basic;
		struct
		{
			Semtype elem;
			int size;
		}array;
		FieldList structure;
	}u;
};
struct FieldList_
{
	char name[MAX_NAME_SIZE];
	Semtype type;
	FieldList tail;
};
struct Function_
{
 	Semtype retType;
	int paramsize;
	FieldList paramlist;
	int lineno;
	struct TreeNode* def;
};

struct Symbol
{
	enum { VARIABLE,FUNCTION }kind;
	char name[MAX_NAME_SIZE];
	int field;
	union
	{
		Semtype type;
		Function func;
	}u;
	int var_no;     //for variable(basic,array,structure)
	int temp_no;    //for array/structure ,marking the address
	//struct Symbol* next;
};

extern struct Symbol* symbolTable[MAX_SYMBOL_SIZE];
extern int field[MAX_FIELD_SIZE];
extern struct Symbol* typeTable[MAX_SYMBOL_SIZE];
extern int current_symbol_size;
extern int current_type_size;
extern Semtype no_use;

extern void initTable();
extern void initExp(struct TreeNode* root,Semtype type);
extern void semantic_analyse(struct TreeNode* parent);
extern struct Symbol* searchType(char* name);
extern void insertType(struct Symbol* symbol);
extern struct Symbol* searchTable(char* name);
extern void insertTable(struct Symbol* symbol);
extern FieldList genParamDec(FieldList paramlist,struct TreeNode* varlist);
extern Semtype genStructType(struct TreeNode* structspecifier);
extern FieldList genFieldList(FieldList structure,struct TreeNode* deflist);
extern Semtype getType(struct TreeNode* specifier);
extern struct Symbol* genVarDecArray(Semtype type,struct TreeNode* vardec);
extern struct Symbol* genVarDec(Semtype type,struct TreeNode* vardec);
extern void gothroughExtDecList(Semtype type,struct TreeNode* extdeclist);
extern void gothroughFunDec(Semtype type,struct TreeNode* fundec);
extern void gothroughDecList(Semtype type,struct TreeNode* declist);
extern int checkTypeEqual(Semtype type1,Semtype type2);
extern int checkTypeStructEqual(FieldList fieldlist1,FieldList fieldlist2);
extern Semtype getExpType(struct TreeNode* exp);
extern int checkLeftValue(struct TreeNode* exp);
extern void checkStmt(struct TreeNode* stmt);
extern void checkArgs(struct TreeNode* args,FieldList paramlist,char* func_name);

extern void outputTable();

#endif
