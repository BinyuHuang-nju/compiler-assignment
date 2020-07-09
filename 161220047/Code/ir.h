#ifndef __IR_H__
#define __IR_H__

#include "frame.h"
#include "semantic.h"

typedef struct Operand_ Operand;
typedef struct IntermediateCode_ IntermediateCode;
typedef struct ArgList_ ArgList;

struct Operand_
{
	enum { 
		OP_VARIABLE,OP_TEMP,OP_CONSTANT_INT,OP_CONSTANT_FLOAT,OP_LABEL,OP_FUNCTION
	}kind;
	union 
	{
		int var_no;    // VARIABLE 
		int temp_no;   // TEMP
		int value_i;   // CONSTANT
		float value_f; // CONSTANT
		//int addr;    // ADDRESS
		int label;     // LABEL
		char func_name[MAX_NAME_SIZE];  // FUNCTION
	}u;
	struct Symbol* symbol;
};
enum IC_KIND{
	IC_ASSIGN,
	IC_PLUS,IC_MINUS,IC_STAR,IC_DIV,
	IC_RELOP_G,IC_RELOP_L,IC_RELOP_GE,IC_RELOP_LE,IC_RELOP_EQ,IC_RELOP_NE,
	IC_AND,IC_OR,
	IC_LABEL,IC_GOTO,
	IC_FUNCTION,IC_PARAM,IC_ARG,IC_CALL,IC_RETURN,
	IC_DEC,IC_ADDR,IC_POINT_LEFT,IC_POINT_RIGHT,
	IC_READ,IC_WRITE
};
struct IntermediateCode_
{
	enum IC_KIND kind;
	union{
		struct {Operand result,op1,op2;} binop;
		struct {Operand result,op;} pairop;
		struct {Operand result;} singleop;
	}u;
	int size;
	IntermediateCode* prev;
	IntermediateCode* next;
};

struct ArgList_
{
	int temp_no;
	ArgList* prev;
	ArgList* next;
};

extern IntermediateCode* TotalCode;
//extern ArgList* arg_list;

void printIR();

extern int current_code_size;

void initCode();
int newVar();
int newTemp();
int newLabel();
void printOperand(Operand op);
enum IC_KIND getKind(struct TreeNode* relop);
enum IC_KIND getOppositeKind(struct TreeNode* relop);
IntermediateCode* genLabel(int label);
IntermediateCode* genGoto(int label);
void insertTable_read();
void insertTable_write();
IntermediateCode* bindCode(IntermediateCode* code1,IntermediateCode* code2);
int getSize(Semtype type);
void genIR();
IntermediateCode* translate_Program(struct TreeNode* program);
IntermediateCode* translate_ExtDefList(struct TreeNode* extdeflist);
IntermediateCode* translate_ExtDef(struct TreeNode* extdef);
IntermediateCode* translate_ExtDecList(struct TreeNode* extdeclist);
IntermediateCode* translate_FunDec(struct TreeNode* fundec);
IntermediateCode* translate_VarList(struct TreeNode* varlist);
IntermediateCode* translate_ParamDec(struct TreeNode* paramdec);
IntermediateCode* translate_CompSt(struct TreeNode* compst);
IntermediateCode* translate_VarDec(struct TreeNode* vardec);
IntermediateCode* translate_DefList(struct TreeNode* deflist);
IntermediateCode* translate_Def(struct TreeNode* def);
IntermediateCode* translate_DecList(struct TreeNode* declist);
IntermediateCode* translate_Dec(struct TreeNode* dec);
IntermediateCode* translate_StmtList(struct TreeNode* stmtlist);
IntermediateCode* translate_Stmt(struct TreeNode* stmt);
IntermediateCode* translate_Cond(struct TreeNode* exp,int label_true,int label_false);
IntermediateCode* translate_Exp(struct TreeNode* exp,int place);
IntermediateCode* translate_Args(struct TreeNode* args,ArgList** arg_list);
#endif
