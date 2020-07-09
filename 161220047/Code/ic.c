#include "ir.h"

#define FALL -1

int current_code_size = 0;
int current_var_no = 1;
int current_temp_no = 1;
int current_label_no = 1;
IntermediateCode* TotalCode = NULL;
IntermediateCode* funcCode = NULL;
int mark = 1;
void printMark(struct TreeNode* current)
{
	printf("%d:(%d) %s\n",mark,current->lineno,current->name);
	mark++;
}
void initCode()
{	
	mark = 1;
	TotalCode = NULL;
	funcCode = NULL;
	current_temp_no = 1;
	current_code_size = 0;
	current_var_no = 1;
	current_label_no = 1;
}
void printOperand(Operand op)
{
	switch(op.kind)
	{
		case OP_VARIABLE:
		{
			printf("v%d",op.u.var_no);
			break;
		}
		case OP_TEMP:
		{
			printf("t%d",op.u.temp_no);
			break;
		}
		case OP_CONSTANT_INT:
		{
			printf("#%d",op.u.value_i);
			break;
		}
		case OP_CONSTANT_FLOAT:
		{
			printf("#%.6f",op.u.value_f);
			break;
		}
		case OP_LABEL:
		{
			printf("label%d",op.u.label);
			break;
		}
		case OP_FUNCTION:
		{
			printf("%s",op.u.func_name);
			break;
		}
		default : break;
	}
}
void printIR()
{
	for(IntermediateCode* code = TotalCode;code!=NULL;code = code->next)
	{
		switch(code->kind)
		{
			case IC_ASSIGN:
			{
				if(code->u.pairop.result.kind == OP_TEMP && code->u.pairop.result.u.temp_no == 0)
					break;
				printOperand(code->u.pairop.result);
				printf(" := ");
				printOperand(code->u.pairop.op);
				printf("\n");
				break;
			}
			case IC_PLUS: case IC_MINUS: case IC_STAR: case IC_DIV:
			{
				printOperand(code->u.binop.result);
				printf(" := ");
				printOperand(code->u.binop.op1);
				if(code->kind == IC_PLUS)
					printf(" + ");
				else if(code->kind == IC_MINUS)
					printf(" - ");
				else if(code->kind == IC_STAR)
					printf(" * ");
				else
					printf(" / ");
				printOperand(code->u.binop.op2);
				printf("\n");
				break;
			}
			case IC_RELOP_G: case IC_RELOP_L: case IC_RELOP_GE:
			case IC_RELOP_LE: case IC_RELOP_EQ: case IC_RELOP_NE:
			{
				printf("IF ");
				printOperand(code->u.binop.op1);
				if(code->kind == IC_RELOP_G)
					printf(" > ");
				else if(code->kind == IC_RELOP_L)
					printf(" < ");
				else if(code->kind == IC_RELOP_GE)
					printf(" >= ");
				else if(code->kind == IC_RELOP_LE)
					printf(" <= ");
				else if(code->kind == IC_RELOP_EQ)
					printf(" == ");
				else if(code->kind == IC_RELOP_NE)
					printf(" != ");
				else
					printf("Mistake in RELOP");
				printOperand(code->u.binop.op2);
				printf(" GOTO ");
				printOperand(code->u.binop.result);
   				printf("\n");
				break;
			}
			case IC_LABEL:
			{
				printf("LABEL ");
				printOperand(code->u.singleop.result);
				printf(" :\n");
				break;
			}
			case IC_GOTO:
			{
				printf("GOTO ");
				printOperand(code->u.singleop.result);
				printf("\n");
				break;
			}
			case IC_FUNCTION:
			{
				printf("FUNCTION ");
				printOperand(code->u.singleop.result);
				printf(" :\n");
				break;
			}
			case IC_PARAM:
			{
				printf("PARAM ");
				printOperand(code->u.singleop.result);
				printf("\n");
				break;
			}
			case IC_ARG:
			{
				printf("ARG ");
				printOperand(code->u.singleop.result);
				printf("\n");
				break;
			}
			case IC_CALL:
			{
				printOperand(code->u.pairop.result);
				printf(" := CALL ");
				printOperand(code->u.pairop.op);
				printf("\n");
				break;
			}
			case IC_RETURN:
			{
				printf("RETURN ");
				printOperand(code->u.singleop.result);
				printf("\n");
				break;
			}
			case IC_DEC:
			{
				printf("DEC ");
				printOperand(code->u.pairop.result);
				printf(" %d\n",code->size);
				break;
			}
			case IC_ADDR:
			{
				printOperand(code->u.pairop.result);
				printf(" := &");
				printOperand(code->u.pairop.op);
				printf("\n");
				break;
			}
			case IC_POINT_LEFT:
			{
				printf("*");
				printOperand(code->u.pairop.result);
				printf(" := ");
				printOperand(code->u.pairop.op);
				printf("\n");
				break;
			}
			case IC_POINT_RIGHT:
			{
				printOperand(code->u.pairop.result);
				printf(" := *");
				printOperand(code->u.pairop.op);
				printf("\n");
				break;
			}
			case IC_READ:
			{
				printf("READ ");
				printOperand(code->u.singleop.result);
				printf("\n");
				break;
			}
			case IC_WRITE:
			{
				printf("WRITE ");
				printOperand(code->u.singleop.result);
				printf("\n");
				break;
			}
			default: break;
		}
	}
}
int newVar()
{
	int x = current_var_no;
	current_var_no = current_var_no + 1;
	return x;
}
int newTemp()
{
	int x = current_temp_no;
	current_temp_no = current_temp_no + 1;
	return x;
}
int newLabel()
{
	int x = current_label_no;
	current_label_no = current_label_no + 1;
	return x;
}

enum IC_KIND getKind(struct TreeNode* relop)
{
	enum IC_KIND kind = IC_RELOP_G;
	if(strcmp(relop->value.c,">") == 0)
		kind = IC_RELOP_G;
	else if(strcmp(relop->value.c,"<") == 0)
		kind = IC_RELOP_L;
	else if(strcmp(relop->value.c,">=") == 0)
		kind = IC_RELOP_GE;
	else if(strcmp(relop->value.c,"<=") == 0)
		kind = IC_RELOP_LE;
	else if(strcmp(relop->value.c,"==") == 0)
		kind = IC_RELOP_EQ;
	else if(strcmp(relop->value.c,"!=") == 0)
		kind = IC_RELOP_NE;
	else
		printf("Mistake in getKind\n");
	return kind;
}
enum IC_KIND getOppositeKind(struct TreeNode* relop)
{
	enum IC_KIND kind = IC_RELOP_G;
	if(strcmp(relop->value.c,">") == 0)
		kind = IC_RELOP_LE;
	else if(strcmp(relop->value.c,"<") == 0)
		kind = IC_RELOP_GE;
	else if(strcmp(relop->value.c,">=") == 0)
		kind = IC_RELOP_L;
	else if(strcmp(relop->value.c,"<=") == 0)
		kind = IC_RELOP_G;
	else if(strcmp(relop->value.c,"==") == 0)
		kind = IC_RELOP_NE;
	else if(strcmp(relop->value.c,"!=") == 0)
		kind = IC_RELOP_EQ;
	else
		printf("Mistake in getKind\n");
	return kind;
}


IntermediateCode* genLabel(int label)
{
	IntermediateCode* code = (IntermediateCode*)malloc(sizeof(IntermediateCode));
	code->kind = IC_LABEL;
	code->prev = code->next = NULL;
	code->u.singleop.result.kind = OP_LABEL;
	code->u.singleop.result.u.label = label;
	return code;
}
IntermediateCode* genGoto(int label)
{
	IntermediateCode* code = (IntermediateCode*)malloc(sizeof(IntermediateCode));
	code->kind = IC_GOTO;
	code->prev = code->next = NULL;
	code->u.singleop.result.kind = OP_LABEL;
	code->u.singleop.result.u.label = label;
	return code;
}

void insertTable_read()
{
	struct Symbol* read_sym = (struct Symbol*)malloc(sizeof(struct Symbol));
	read_sym->field = 0;
	read_sym->kind = FUNCTION;
	strcpy(read_sym->name,"read");
	Function read_func = (Function)malloc(sizeof(struct Function_));
	read_func->paramsize = read_func->lineno = 0;
	Semtype retType = (Semtype)malloc(sizeof(struct Type_));
	retType->kind = BASIC;
	retType->lineno = 0;
	retType->u.basic = BASIC_INT;
	read_func->retType = retType;
	read_func->paramlist = NULL;
	read_func->def = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	memset(read_func->def,0,sizeof(struct TreeNode));
	read_sym->u.func = read_func;
	insertTable(read_sym);
}
void insertTable_write()
{
	struct Symbol* write_sym = (struct Symbol*)malloc(sizeof(struct Symbol));
	write_sym->field = 0;
	write_sym->kind = FUNCTION;
	strcpy(write_sym->name,"write");
	Function write_func = (Function)malloc(sizeof(struct Function_));
	write_func->paramsize = write_func->lineno = 0;
	Semtype retType = (Semtype)malloc(sizeof(struct Type_));
	retType->kind = BASIC;
	retType->lineno = 0;
	retType->u.basic = BASIC_INT;
	write_func->retType = retType;
	FieldList param = (FieldList)malloc(sizeof(struct FieldList_));
	param->tail = NULL;
	param->type = (Semtype)malloc(sizeof(struct Type_));
	param->type->lineno = 0;
	param->type->kind = BASIC;
	param->type->u.basic = BASIC_INT;
	write_func->paramlist = param;
	write_func->def = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	memset(write_func->def,0,sizeof(struct TreeNode));
	write_sym->u.func = write_func;
	insertTable(write_sym);
}

IntermediateCode* bindCode(IntermediateCode* code1,IntermediateCode* code2)
{
	if(code1 == NULL && code2 == NULL)
		return NULL;
	else if(code1 == NULL)
		return code2;
	else if(code2 == NULL)
		return code1;
	else if(code1->next == NULL)
	{
		code1->next = code2;
		code2->prev = code1;
	}
	else
	{
		IntermediateCode* p = code1;
		while(p->next != NULL)
			p = p->next;
		p->next = code2;
		code2->prev = p;
	}
	return code1;
}
int getSize(Semtype type)
{
	if(type->kind == BASIC)
		return 4;
	else if(type->kind == ARRAY)
		return (type->u.array.size)*(getSize(type->u.array.elem));
	else
	{
		FieldList fieldlist = type->u.structure;
		int size = 0;
		while(fieldlist != NULL)
		{
			size = size + getSize(fieldlist->type);
			fieldlist = fieldlist->tail;
		}
		return size;
	}
}
IntermediateCode* translate_Program(struct TreeNode* program)
{
//	printf("Program  ");
//	printMark(program);
	struct TreeNode* extdeflist = program->first_child;
	IntermediateCode* code = translate_ExtDefList(extdeflist);
	return code;
}
IntermediateCode* translate_ExtDefList(struct TreeNode* extdeflist)
{
//	printf("ExtDefList  ");
//	printMark(extdeflist);
	IntermediateCode* code = NULL;
	if(extdeflist->first_child!=NULL)
	{
		struct TreeNode* extdef = extdeflist->first_child;
		struct TreeNode* extdeflist1 = extdef->sibling;
		IntermediateCode* code1 = translate_ExtDef(extdef);
		IntermediateCode* code2 = translate_ExtDefList(extdeflist1);
		code = bindCode(code1,code2);
	}
	return code;
}
IntermediateCode* translate_ExtDef(struct TreeNode* extdef)
{
//	printf("ExtDef  ");
//	printMark(extdef);
	IntermediateCode* code = NULL;
	struct TreeNode* specifier = extdef->first_child;
	if(specifier->sibling->type == ExtDecList_)
		code = translate_ExtDecList(specifier->sibling);
	else if(specifier->sibling->type == FunDec_)
	{
		struct TreeNode* fundec = specifier->sibling;
		struct TreeNode* compst = fundec->sibling;
		IntermediateCode* code1 = translate_FunDec(fundec);
		IntermediateCode* code2 = translate_CompSt(compst);
		code = bindCode(code1,code2);
	}
	// else Specifier SEMI : type is known in typeTable
	return code;
}
IntermediateCode* translate_ExtDecList(struct TreeNode* extdeclist)
{
//	printf("ExtDecList  ");
//	printMark(extdeclist);
	IntermediateCode* code = NULL;
	struct TreeNode* vardec = extdeclist->first_child;
	IntermediateCode* code1 = translate_VarDec(vardec);
	if(vardec->sibling != NULL)
	{
		struct TreeNode* extdeclist1 = vardec->sibling->sibling;
		IntermediateCode* code2 = translate_ExtDecList(extdeclist1);
		code = bindCode(code1,code2);
		return code;
	}
	return code1;
}
IntermediateCode* translate_FunDec(struct TreeNode* fundec)
{
//	printf("FunDec  ");
//	printMark(fundec);
	IntermediateCode* code = NULL;
	struct TreeNode* id = fundec->first_child;
	IntermediateCode* code1 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
	code1->kind = IC_FUNCTION;
	code1->prev = code1->next = NULL;
	code1->u.singleop.result.kind = OP_FUNCTION;
	strcpy(code1->u.singleop.result.u.func_name,id->name);
	code1->u.singleop.result.symbol = searchTable(id->name);
	if(code1->u.singleop.result.symbol == NULL)
		printf("Mistake in FunDec\n");
	if(id->sibling->sibling->type == VarList_)
	{
		funcCode = NULL;
		struct TreeNode* varlist = id->sibling->sibling;
		IntermediateCode* code2 = translate_VarList(varlist);
		code = bindCode(code1,code2);
		code = bindCode(code,funcCode);
		return code;
	}
	return code1;
}
IntermediateCode* translate_VarList(struct TreeNode* varlist)
{
//	printf("VarList  ");
//	printMark(varlist);
	IntermediateCode* code = NULL;
	struct TreeNode* paramdec = varlist->first_child;
	IntermediateCode* code1 = translate_ParamDec(paramdec);
	if(paramdec->sibling != NULL)
	{
		struct TreeNode* varlist1 = paramdec->sibling->sibling;
		IntermediateCode* code2 = translate_VarList(varlist1);
		code = bindCode(code1,code2);
		return code;
	}
	return code1;
}
IntermediateCode* translate_ParamDec(struct TreeNode* paramdec)
{
//	printf("ParamDec  ");
//	printMark(paramdec);
	IntermediateCode* code = NULL;
	struct TreeNode* vardec = paramdec->first_child->sibling;
	while(vardec->first_child->type != ID_)
		vardec = vardec->first_child;
	struct TreeNode* id = vardec->first_child;
	IntermediateCode* code1 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
	int v1 = newVar();
	struct Symbol* symbol = searchTable(id->name);
	code1->kind = IC_PARAM;
	code1->prev = code1->next = NULL;
	code1->u.singleop.result.kind = OP_VARIABLE;
	code1->u.singleop.result.u.var_no = v1;
	code1->u.singleop.result.symbol = symbol;
	code = code1;
	if(symbol == NULL)
		printf("Mistake in ParamDec\n");
	symbol->var_no = v1;
	int size = getSize(symbol->u.type);
	if(size > 4)
	{
		int t1 = newTemp();	
		symbol->temp_no = t1;
		IntermediateCode* code2 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
		code2->kind = IC_ADDR;
		code2->prev = code2->next = NULL;
		code2->u.pairop.result.kind = OP_TEMP;
		code2->u.pairop.result.u.temp_no = t1;
		code2->u.pairop.op.kind = OP_VARIABLE;
		code2->u.pairop.op.u.var_no = v1;
		funcCode = bindCode(funcCode,code2);
	}
	return code;
}
IntermediateCode* translate_CompSt(struct TreeNode* compst)
{
//	printf("CompSt  ");
//	printMark(compst);
	IntermediateCode* code = NULL;
	struct TreeNode* deflist = compst->first_child->sibling;
	struct TreeNode* stmtlist = deflist->sibling;
	IntermediateCode* code1 = translate_DefList(deflist);
	IntermediateCode* code2 = translate_StmtList(stmtlist);
	code = bindCode(code1,code2);
	return code;
}
IntermediateCode* translate_VarDec(struct TreeNode* vardec)
{
//	printf("VarDec  ");
//	printMark(vardec);
	IntermediateCode* code = NULL;
	if(vardec->first_child->type == VarDec_)
		code = translate_VarDec(vardec->first_child);
	else
	{
		struct TreeNode* id = vardec->first_child;
		struct Symbol* symbol = searchTable(id->name);
		if(symbol == NULL || symbol->kind == FUNCTION)
			printf("Mistake in VarDec\n");
		int size = getSize(symbol->u.type);
		if(size == 4)
		{
			int v1 = newVar();
			symbol->var_no = v1;
		}
		else if(size > 4)
		{
			int v1 = newVar();
			int t1 = newTemp();
			IntermediateCode* code1 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
			code1->kind = IC_DEC;
			code1->prev = code1->next = NULL;
			code1->u.singleop.result.kind = OP_VARIABLE;
			code1->u.singleop.result.u.var_no = v1;
			code1->u.singleop.result.symbol = symbol;
			code1->size = size;
			symbol->var_no = v1;
			IntermediateCode* code2 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
			code2->kind = IC_ADDR;
			code2->prev = code2->next = NULL;
			code2->u.pairop.op.kind = OP_VARIABLE;
			code2->u.pairop.op.u.var_no = v1;
			code2->u.pairop.op.symbol = symbol;
			code2->u.pairop.result.kind = OP_TEMP;
			code2->u.pairop.result.u.temp_no = t1;
			code2->u.pairop.result.symbol = symbol;
			symbol->temp_no = t1;
			
			code = bindCode(code1,code2);
		}
	}
	return code;
}
IntermediateCode* translate_DefList(struct TreeNode* deflist)
{
//	printf("DefList  ");
//	printMark(deflist);
	IntermediateCode* code = NULL;
	if(deflist->first_child != NULL)
	{
		struct TreeNode* def = deflist->first_child;
		struct TreeNode* deflist1 = def->sibling;
		IntermediateCode* code1 = translate_Def(def);
		IntermediateCode* code2 = translate_DefList(deflist1);
		code = bindCode(code1,code2);
	}
	return code;
}
IntermediateCode* translate_Def(struct TreeNode* def)
{
//	printf("Def  ");
//	printMark(def);
	struct TreeNode* declist = def->first_child->sibling;
	IntermediateCode* code = translate_DecList(declist);
	return code;
}
IntermediateCode* translate_DecList(struct TreeNode* declist)
{
//	printf("DecList  ");
//	printMark(declist);
	IntermediateCode* code = NULL;
	struct TreeNode* dec = declist->first_child;
	IntermediateCode* code1 = translate_Dec(dec);
	if(dec->sibling != NULL)
	{
		struct TreeNode* declist1 = dec->sibling->sibling;
		IntermediateCode* code2 = translate_DecList(declist1);
		code = bindCode(code1,code2);
		return code;
	}
	return code1;
}
IntermediateCode* translate_Dec(struct TreeNode* dec)
{
//	printf("Dec  ");
//	printMark(dec);
	IntermediateCode* code = NULL;
	struct TreeNode* vardec = dec->first_child;
	if(vardec->sibling == NULL)
		code = translate_VarDec(vardec);
	else
	{
		int t1 = newTemp();
		struct TreeNode* exp = vardec->sibling->sibling;
		struct TreeNode* id = vardec->first_child;
		IntermediateCode* code1 = translate_Exp(exp,t1);
		struct Symbol* symbol = searchTable(id->name);
		if(symbol == NULL)
			printf("Mistake in Dec\n");
		IntermediateCode* code2 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
		int v1 = newVar();
		code2->kind = IC_ASSIGN;
		code2->prev = code2->next = NULL;
		code2->u.pairop.result.kind = OP_VARIABLE;
		code2->u.pairop.result.symbol = symbol;
		code2->u.pairop.result.u.var_no = v1;
		symbol->var_no = v1;
		code2->u.pairop.op.kind = OP_TEMP;
		code2->u.pairop.op.u.temp_no = t1;

		code = bindCode(code1,code2);
	}
	return code;
}
IntermediateCode* translate_StmtList(struct TreeNode* stmtlist)
{
//	printf("StmtList  ");
//	printMark(stmtlist);
	IntermediateCode* code = NULL;
	if(stmtlist->first_child != NULL)
	{
		struct TreeNode* stmt = stmtlist->first_child;
		struct TreeNode* stmtlist1 = stmt->sibling;
		IntermediateCode* code1 = translate_Stmt(stmt);
		IntermediateCode* code2 = translate_StmtList(stmtlist1);
		code = bindCode(code1,code2);
	}
	return code;
}
IntermediateCode* translate_Stmt(struct TreeNode* stmt)
{
//	printf("Stmt  ");
//	printMark(stmt);
	IntermediateCode* code = NULL;
	if(stmt->first_child->type == Exp_)
	{	
		code = translate_Exp(stmt->first_child,0);
	}
	else if(stmt->first_child->type == CompSt_)
		code = translate_CompSt(stmt->first_child);
	else if(stmt->first_child->type == RETURN_)
	{
		int t1 = newTemp();
		struct TreeNode* exp = stmt->first_child->sibling;
		IntermediateCode* code1 = translate_Exp(exp,t1);
		IntermediateCode* code2 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
		code2->kind = IC_RETURN;
		code2->prev = code2->next = NULL;
		code2->u.singleop.result.kind = OP_TEMP;
		code2->u.singleop.result.u.temp_no = t1;

		code = bindCode(code1,code2);
	}
	else if(stmt->first_child->type == IF_)
	{
		struct TreeNode* exp = stmt->first_child->sibling->sibling;
		struct TreeNode* stmt1 = exp->sibling->sibling;
		if(stmt1->sibling == NULL)
		{
			int label1 = FALL;       //B.true
			int label2 = newLabel(); //B.false
			IntermediateCode* code1 = translate_Cond(exp,label1,label2);
			IntermediateCode* code2 = translate_Stmt(stmt1);
			IntermediateCode* code_label2 = genLabel(label2);

			IntermediateCode* c1 = bindCode(code2,code_label2);
			IntermediateCode* c2 = bindCode(code1,c1);

			code = c2;
		}
		else
		{
			int label1 = FALL;       //B.true
			int label2 = newLabel(); //B.false
			int label3 = newLabel(); //end
			struct TreeNode* stmt2 = stmt1->sibling->sibling;

			IntermediateCode* code1 = translate_Cond(exp,label1,label2);
			IntermediateCode* code2 = translate_Stmt(stmt1);
			IntermediateCode* code3 = translate_Stmt(stmt2);

			IntermediateCode* code_label2 = genLabel(label2);
			IntermediateCode* code_label3 = genLabel(label3);
			IntermediateCode* code_gotolabel3 = genGoto(label3);

			IntermediateCode* c1 = bindCode(code3,code_label3);
			IntermediateCode* c2 = bindCode(code_label2,c1);
			IntermediateCode* c3 = bindCode(code_gotolabel3,c2);
			IntermediateCode* c4 = bindCode(code2,c3);
			IntermediateCode* c5 = bindCode(code1,c4);

			code = c5;
		}
	}
	else if(stmt->first_child->type == WHILE_)
	{
		int label1 = newLabel(); //begin
		int label2 = FALL;       //B.true
		int label3 = newLabel(); //B.false
		struct TreeNode* exp = stmt->first_child->sibling->sibling;
		struct TreeNode* stmt1 = exp->sibling->sibling;

		IntermediateCode* code1 = translate_Cond(exp,label2,label3);
		IntermediateCode* code2 = translate_Stmt(stmt1);

		IntermediateCode* code_label1 = genLabel(label1);
		IntermediateCode* code_label3 = genLabel(label3);
		IntermediateCode* code_gotolabel1 = genGoto(label1);

		IntermediateCode* c1 = bindCode(code_gotolabel1,code_label3);
		IntermediateCode* c2 = bindCode(code2,c1);
		IntermediateCode* c3 = bindCode(code1,c2);
		IntermediateCode* c4 = bindCode(code_label1,c3);

		code = c4;
	}
	else
		printf("Mistake in Stmt\n");
	return code;
}
IntermediateCode* translate_Cond(struct TreeNode* exp,int label_true,int label_false)
{
//	printf("Cond  ");
//	printMark(exp);
	IntermediateCode* code = NULL;
	if(exp->first_child->type == NOT_)
	{
		struct TreeNode* exp1 = exp->first_child->sibling;
		code = translate_Cond(exp1,label_false,label_true);
	}
	else if(exp->first_child->type == Exp_ && exp->first_child->sibling->type == RELOP_)
	{	
		int t1 = newTemp();
		int t2 = newTemp();
		struct TreeNode* exp1 = exp->first_child;
		struct TreeNode* relop = exp1->sibling;
		struct TreeNode* exp2 = relop->sibling;
		IntermediateCode* code1 = translate_Exp(exp1,t1);
		IntermediateCode* code2 = translate_Exp(exp2,t2);
		IntermediateCode* c = bindCode(code1,code2);
		if(label_true != FALL && label_false != FALL)
		{
			IntermediateCode* code_relop = (IntermediateCode*)malloc(sizeof(IntermediateCode));
			code_relop->kind = getKind(relop);
			code_relop->prev = code_relop->next = NULL;
			code_relop->u.binop.op1.kind = OP_TEMP;
			code_relop->u.binop.op1.u.var_no = t1;
			code_relop->u.binop.op2.kind = OP_TEMP;
			code_relop->u.binop.op2.u.var_no = t2;
			code_relop->u.binop.result.kind = OP_LABEL;
			code_relop->u.binop.result.u.label = label_true;

			IntermediateCode* code_gotolabel = genGoto(label_false);

			IntermediateCode* c1 = bindCode(code_relop,code_gotolabel);
			IntermediateCode* c2 = bindCode(c,c1);
			code = c2;
		}
		else if(label_true != FALL)
		{
			IntermediateCode* code_relop = (IntermediateCode*)malloc(sizeof(IntermediateCode));
			code_relop->kind = getKind(relop);
			code_relop->prev = code_relop->next = NULL;
			code_relop->u.binop.op1.kind = OP_TEMP;
			code_relop->u.binop.op1.u.var_no = t1;
			code_relop->u.binop.op2.kind = OP_TEMP;
			code_relop->u.binop.op2.u.var_no = t2;
			code_relop->u.binop.result.kind = OP_LABEL;
			code_relop->u.binop.result.u.label = label_true;

			IntermediateCode* c1 = bindCode(c,code_relop);
			code = c1;
		}
		else if(label_false != FALL)
		{
			IntermediateCode* code_relop = (IntermediateCode*)malloc(sizeof(IntermediateCode));
			code_relop->kind = getOppositeKind(relop);
			code_relop->prev = code_relop->next = NULL;
			code_relop->u.binop.op1.kind = OP_TEMP;
			code_relop->u.binop.op1.u.var_no = t1;
			code_relop->u.binop.op2.kind = OP_TEMP;
			code_relop->u.binop.op2.u.var_no = t2;
			code_relop->u.binop.result.kind = OP_LABEL;
			code_relop->u.binop.result.u.label = label_false;

			IntermediateCode* c1 = bindCode(c,code_relop);
			code = c1;
		}
		else
			code = c;
	}
	else if(exp->first_child->type == Exp_ && exp->first_child->sibling->type == AND_)
	{
		int label1;  //B1.false
		if(label_false == FALL)
			label1 = newLabel();
		else
			label1 = label_false;
		struct TreeNode* exp1 = exp->first_child;
		struct TreeNode* exp2 = exp1->sibling->sibling;
		IntermediateCode* code1 = translate_Cond(exp1,FALL,label1);
		IntermediateCode* code2 = translate_Cond(exp2,label_true,label_false);
		IntermediateCode* c = bindCode(code1,code2);
		if(label_false == FALL)
		{
			IntermediateCode* code_label = genLabel(label1);
			IntermediateCode* c1 = bindCode(c,code_label);
			code = c1;
		}
		else
			code = c;
	}
	else if(exp->first_child->type == Exp_ && exp->first_child->sibling->type == OR_)
	{
		int label1;
		if(label_true != FALL)
			label1 = label_true;
		else
			label1 = newLabel();
		struct TreeNode* exp1 = exp->first_child;
		struct TreeNode* exp2 = exp1->sibling->sibling;
		IntermediateCode* code1 = translate_Cond(exp1,label1,FALL);
		IntermediateCode* code2 = translate_Cond(exp2,label_true,label_false);
		IntermediateCode* c = bindCode(code1,code2);
		if(label_true != FALL)
			code = c;
		else
		{
			IntermediateCode* code_label = genLabel(label1);
			IntermediateCode* c1 = bindCode(c,code_label);
			code = c1;
		}
	}
	else
	{
		int t1 = newTemp();
		IntermediateCode* code1 = translate_Exp(exp,t1);
		IntermediateCode* code2 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
		code2->kind = IC_RELOP_NE;
		code2->prev = code2->next = NULL;
		code2->u.binop.op1.kind = OP_TEMP;
		code2->u.binop.op1.u.temp_no = t1;
		code2->u.binop.op2.kind = OP_CONSTANT_INT;
		code2->u.binop.op2.u.value_i = 0;
		code2->u.binop.result.kind = OP_LABEL;
		code2->u.binop.result.u.label = label_true;

		IntermediateCode* code_gotolabel = genGoto(label_false);
		IntermediateCode* c1 = bindCode(code2,code_gotolabel);
		IntermediateCode* c2 = bindCode(code1,c1);
		code = c2;
	}
	return code;
}
IntermediateCode* translate_Exp(struct TreeNode* exp,int place)
{
//	printf("Exp   ");
//	printMark(exp);
	IntermediateCode* code = NULL;
	struct TreeNode* first = exp->first_child;
	switch(first->type)
	{
		case INT_:
		{
			IntermediateCode* code1 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
			code1->kind = IC_ASSIGN;
			code1->prev = code1->next = NULL;
			code1->u.pairop.result.kind = OP_TEMP;
			code1->u.pairop.result.u.temp_no = place;
			code1->u.pairop.op.kind = OP_CONSTANT_INT;
			code1->u.pairop.op.u.value_i = first->value.i;
			code = code1;
			break;
		}
		case FLOAT_:
		{
			IntermediateCode* code1 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
			code1->kind = IC_ASSIGN;
			code1->prev = code1->next = NULL;
			code1->u.pairop.result.kind = OP_TEMP;
			code1->u.pairop.result.u.temp_no = place;
			code1->u.pairop.op.kind = OP_CONSTANT_FLOAT;
			code1->u.pairop.op.u.value_f = first->value.f;
			code = code1;
			break;
		}
		case ID_:
		{	
			if(first->sibling == NULL)
			{	
				struct Symbol* symbol = searchTable(first->name);
				if(symbol == NULL)
					printf("Mistake in Exp_ID\n");
				
				IntermediateCode* code1 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
				code1->kind = IC_ASSIGN;
				code1->prev = code1->next = NULL;
				code1->u.pairop.result.kind = OP_TEMP;
				code1->u.pairop.result.u.temp_no = place;
				code1->u.pairop.op.kind = OP_VARIABLE;
				code1->u.pairop.op.u.var_no = symbol->var_no;
				code = code1;
			}
			else if(first->sibling->sibling->type == RP_)
			{
				struct Symbol* symbol = searchTable(first->value.c);
				if(strcmp(first->value.c,"read")==0)
				{
					IntermediateCode* code1 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
					code1->kind = IC_READ;
					code1->prev = code1->next = NULL;
					code1->u.singleop.result.kind = OP_TEMP;
					code1->u.singleop.result.u.temp_no = place;
					code = code1;
				}
				else
				{
					IntermediateCode* code1 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
					code1->kind = IC_CALL;
					code1->prev = code1->next = NULL;
					code1->u.pairop.result.kind = OP_TEMP;
					code1->u.pairop.result.u.temp_no = place;
					code1->u.pairop.op.kind = OP_FUNCTION;
					code1->u.pairop.op.symbol = symbol;
					strcpy(code1->u.pairop.op.u.func_name,first->value.c);
					code = code1;
				}
			}
			else if(first->sibling->sibling->type == Args_)
			{
				struct Symbol* symbol = searchTable(first->value.c);
				struct TreeNode* args = first->sibling->sibling;
				ArgList* arg_list = NULL;
				IntermediateCode* code1 = translate_Args(args,&arg_list);
				if(arg_list == NULL)
					printf("Mistake in Exp_Args\n");
				if(strcmp(first->value.c,"write")==0)
				{
					IntermediateCode* code2 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
					code2->kind = IC_WRITE;
					code2->prev = code2->next = NULL;
					code2->u.singleop.result.kind = OP_TEMP;
					code2->u.singleop.result.u.temp_no = arg_list->temp_no;
					code = bindCode(code1,code2);
				}
				else
				{
					IntermediateCode* code2 = NULL;
					for(ArgList *p = arg_list;p!=NULL;p=p->next)
					{
						IntermediateCode* code3 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
						code3->kind = IC_ARG;
						code3->prev = code3->next = NULL;
						code3->u.singleop.result.kind = OP_TEMP;
						code3->u.singleop.result.u.temp_no = p->temp_no;
						if(code2 == NULL)
							code2 = code3;
						else
							code2 = bindCode(code2,code3);
					}
					IntermediateCode* code4 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
					code4->prev = code4->next = NULL;
					code4->kind = IC_CALL;
					code4->u.pairop.result.kind = OP_TEMP;
					code4->u.pairop.result.u.temp_no= place;
					code4->u.pairop.op.kind = OP_FUNCTION;
					code4->u.pairop.op.symbol = symbol;
					strcpy(code4->u.pairop.op.u.func_name,first->value.c);

					IntermediateCode* c1 = bindCode(code2,code4);
					IntermediateCode* c2 = bindCode(code1,c1);
					code = c2;
				}
			}
			break;
		}
		case MINUS_:
		{
			int t1 = newTemp();
			struct TreeNode* exp1 = first->sibling;
			IntermediateCode* code1 = translate_Exp(exp1,t1);
			IntermediateCode* code2 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
			code2->kind = IC_MINUS;
			code2->prev = code2->next = NULL;
			code2->u.binop.result.kind = OP_TEMP;
			code2->u.binop.result.u.temp_no = place;
			code2->u.binop.op1.kind = OP_CONSTANT_INT;
			code2->u.binop.op1.u.value_i = 0;
			code2->u.binop.op2.kind = OP_TEMP;
			code2->u.binop.op2.u.temp_no = t1;

			code = bindCode(code1,code2);
			break;
		}
		case Exp_:
		{
			if(first->sibling->type == PLUS_)
			{
				int t1 = newTemp();
				int t2 = newTemp();
				struct TreeNode* exp1 = first;
				struct TreeNode* exp2 = exp1->sibling->sibling;
				struct TreeNode* sym = exp1->sibling;
				IntermediateCode* code1 = translate_Exp(exp1,t1);
				IntermediateCode* code2 = translate_Exp(exp2,t2);
				IntermediateCode* code3 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
				code3->kind = IC_PLUS;
				code3->prev = code3->next = NULL;
				code3->u.binop.result.kind = OP_TEMP;
				code3->u.binop.result.u.temp_no = place;
				code3->u.binop.op1.kind = OP_TEMP;
				code3->u.binop.op1.u.temp_no = t1;
				code3->u.binop.op2.kind = OP_TEMP;
				code3->u.binop.op2.u.temp_no = t2;

				IntermediateCode* c1 = bindCode(code2,code3);
				IntermediateCode* c2 = bindCode(code1,c1);
				code = c2;
			}
			else if(first->sibling->type == MINUS_)
			{
				int t1 = newTemp();
				int t2 = newTemp();
				struct TreeNode* exp1 = first;
				struct TreeNode* exp2 = exp1->sibling->sibling;
				struct TreeNode* sym = exp1->sibling;
				IntermediateCode* code1 = translate_Exp(exp1,t1);
				IntermediateCode* code2 = translate_Exp(exp2,t2);
				IntermediateCode* code3 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
				code3->kind = IC_MINUS;
				code3->prev = code3->next = NULL;
				code3->u.binop.result.kind = OP_TEMP;
				code3->u.binop.result.u.temp_no = place;
				code3->u.binop.op1.kind = OP_TEMP;
				code3->u.binop.op1.u.temp_no = t1;
				code3->u.binop.op2.kind = OP_TEMP;
				code3->u.binop.op2.u.temp_no = t2;

				IntermediateCode* c1 = bindCode(code2,code3);
				IntermediateCode* c2 = bindCode(code1,c1);
				code = c2;
			}
			else if(first->sibling->type == STAR_)
			{
				int t1 = newTemp();
				int t2 = newTemp();
				struct TreeNode* exp1 = first;
				struct TreeNode* exp2 = exp1->sibling->sibling;
				struct TreeNode* sym = exp1->sibling;
				IntermediateCode* code1 = translate_Exp(exp1,t1);
				IntermediateCode* code2 = translate_Exp(exp2,t2);
				IntermediateCode* code3 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
				code3->kind = IC_STAR;
				code3->prev = code3->next = NULL;
				code3->u.binop.result.kind = OP_TEMP;
				code3->u.binop.result.u.temp_no = place;
				code3->u.binop.op1.kind = OP_TEMP;
				code3->u.binop.op1.u.temp_no = t1;
				code3->u.binop.op2.kind = OP_TEMP;
				code3->u.binop.op2.u.temp_no = t2;

				IntermediateCode* c1 = bindCode(code2,code3);
				IntermediateCode* c2 = bindCode(code1,c1);
				code = c2;
			}
			else if(first->sibling->type == DIV_)
			{
				int t1 = newTemp();
				int t2 = newTemp();
				struct TreeNode* exp1 = first;
				struct TreeNode* exp2 = exp1->sibling->sibling;
				struct TreeNode* sym = exp1->sibling;
				IntermediateCode* code1 = translate_Exp(exp1,t1);
				IntermediateCode* code2 = translate_Exp(exp2,t2);
				IntermediateCode* code3 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
				code3->kind = IC_DIV;
				code3->prev = code3->next = NULL;
				code3->u.binop.result.kind = OP_TEMP;
				code3->u.binop.result.u.temp_no = place;
				code3->u.binop.op1.kind = OP_TEMP;
				code3->u.binop.op1.u.temp_no = t1;
				code3->u.binop.op2.kind = OP_TEMP;
				code3->u.binop.op2.u.temp_no = t2;

				IntermediateCode* c1 = bindCode(code2,code3);
				IntermediateCode* c2 = bindCode(code1,c1);
				code = c2;
			}
			else if(first->sibling->type == LB_)
			{
				struct TreeNode* exp1 = first;
				struct TreeNode* exp2 = exp1->sibling->sibling;
				//int t1 = newTemp();
				int t2 = newTemp();
				//IntermediateCode* code1 = translate_Exp(exp1,t1);
				IntermediateCode* code2 = translate_Exp(exp2,t2);
				struct TreeNode* p =exp1;
				while(p->type != ID_)
					p = p->first_child;
				struct Symbol* arr = searchTable(p->name);
				if(arr == NULL)
					printf("Mistake in Exp_ARRAY\n");
				int t3 = newTemp();
				IntermediateCode* code3 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
				code3->kind = IC_STAR;
				code3->prev = code3->next = NULL;
				code3->u.binop.result.kind = OP_TEMP;
				code3->u.binop.result.u.temp_no = t3;
				code3->u.binop.op1.kind = OP_TEMP;
				code3->u.binop.op1.u.temp_no = t2;
				code3->u.binop.op2.kind = OP_CONSTANT_INT;
				code3->u.binop.op2.u.value_i = getSize(exp->exp_type);
				int t4 = place;
				IntermediateCode* code4 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
				code4->kind = IC_PLUS;
				code4->prev = code4->next = NULL;
				code4->u.binop.result.kind = OP_TEMP;
				code4->u.binop.result.u.temp_no = t4;
				code4->u.binop.op1.kind = OP_TEMP;
				code4->u.binop.op1.u.temp_no = arr->temp_no;
				code4->u.binop.op2.kind = OP_TEMP;
				code4->u.binop.op2.u.temp_no = t3;

				IntermediateCode* code5 = NULL;
				if(exp->exp_type->kind == BASIC)
				{	
					t4 = newTemp();
					code5 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
					code5->kind = IC_POINT_RIGHT;
					code5->prev = code5->next = NULL;
					code5->u.pairop.result.kind = OP_TEMP;
					code5->u.pairop.result.u.temp_no = place;
					code5->u.pairop.op.kind = OP_TEMP;
					code5->u.pairop.op.u.temp_no = t4;
					code4->u.binop.result.u.temp_no = t4;
				}
				IntermediateCode* c1 = bindCode(code4,code5);
				IntermediateCode* c2 = bindCode(code3,c1);
				IntermediateCode* c3 = bindCode(code2,c2);
				//IntermediateCode* c4 = bindCode(code1,c3);
				code = c3;
			}
			else if(first->sibling->type == DOT_)
			{
				//int t1 = newTemp();
				//IntermediateCode* code1 = translate_Exp(first,t1);
				struct TreeNode* id = first->sibling->sibling;
				struct TreeNode* p =first;
				while(p->type != ID_)
					p = p->first_child;
				struct Symbol* symbol = searchTable(p->name);
				if(symbol == NULL || symbol->u.type->kind != STRUCTURE)
					printf("Mistake in Exp_STRUCTURE\n");
				FieldList fieldlist = symbol->u.type->u.structure;
				int size = 0;
				while(strcmp(fieldlist->name,id->name)!= 0)
				{
					size = size + getSize(fieldlist->type);
					fieldlist = fieldlist->tail;
				}
				int t2 = newTemp();
				IntermediateCode* code2 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
				code2->kind = IC_PLUS;
				code2->prev = code2->next = NULL;
				code2->u.binop.result.kind = OP_TEMP;
				code2->u.binop.result.u.temp_no = t2;
				code2->u.binop.op1.kind = OP_TEMP;
				code2->u.binop.op1.u.temp_no = symbol->temp_no;
				code2->u.binop.op2.kind = OP_CONSTANT_INT;
				code2->u.binop.op2.u.value_i = size;
				
				IntermediateCode* code3 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
				code3->kind = IC_POINT_RIGHT;
				code3->prev = code3->next = NULL;
				code3->u.pairop.result.kind = OP_TEMP;
				code3->u.pairop.result.u.temp_no = place;
				code3->u.pairop.op.kind = OP_TEMP;
				code3->u.pairop.op.u.temp_no = t2;

				IntermediateCode* c1 = bindCode(code2,code3);
				//IntermediateCode* c2 = bindCode(code1,c1);
				code = c1;
			}
			else if(first->sibling->type == ASSIGNOP_)
			{
				if(first->first_child->type == ID_)
				{
					struct TreeNode* exp1 = first;
					struct TreeNode* exp2 = exp1->sibling->sibling;
					struct Symbol* symbol = searchTable(first->first_child->name);
					int t1= newTemp();
					IntermediateCode* code1 = translate_Exp(exp2,t1);
					IntermediateCode* code2 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
					code2->kind = IC_ASSIGN;
					code2->prev = code2->next = NULL;
					code2->u.pairop.result.kind = OP_VARIABLE;
					code2->u.pairop.result.u.var_no = symbol->var_no;
					code2->u.pairop.op.kind = OP_TEMP;
					code2->u.pairop.op.u.temp_no = t1;
					if(place != 0)
					{
						IntermediateCode* code3 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
						code3->kind = IC_ASSIGN;
						code3->prev = code3->next = NULL;
						code3->u.pairop.result.kind = OP_TEMP;
						code3->u.pairop.result.u.temp_no = place;
						code3->u.pairop.op.kind = OP_VARIABLE;
						code3->u.pairop.op.u.var_no = symbol->var_no;
						code2 = bindCode(code2,code3);
					}
					//IntermediateCode* c1 = bindCode(code2,code3);
					IntermediateCode* c2 = bindCode(code1,code2);

					code = c2;
				}
				else if(first->first_child->type == Exp_)
				{
					struct TreeNode* exp1 = first->first_child;
					if(exp1->sibling->type == LB_)
					{
						struct TreeNode* exp2 = exp1->sibling->sibling;
						//int t1 = newTemp();
						int t2 = newTemp();
						//IntermediateCode* code1 = translate_Exp(exp1,t1);
						IntermediateCode* code2 = translate_Exp(exp2,t2);
						int t3 = newTemp();
						IntermediateCode* code3 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
						code3->kind = IC_STAR;
						code3->prev = code3->next = NULL;
						code3->u.binop.result.kind = OP_TEMP;
						code3->u.binop.result.u.temp_no = t3;
						code3->u.binop.op1.kind = OP_TEMP;
						code3->u.binop.op1.u.temp_no = t2;
						code3->u.binop.op2.kind = OP_CONSTANT_INT;
						code3->u.binop.op2.u.value_i = getSize(first->exp_type);
						int t4 = newTemp();
						struct TreeNode* p = exp1;
						while(p->type != ID_)
							p = p->first_child;
						struct Symbol* symbol = searchTable(p->name);
						IntermediateCode* code4 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
						code4->prev = code4->next = NULL;
						code4->kind = IC_PLUS;
						code4->u.binop.result.kind = OP_TEMP;
						code4->u.binop.result.u.temp_no = t4;
						code4->u.binop.op1.kind = OP_TEMP;
						code4->u.binop.op1.u.temp_no = symbol->temp_no;
						code4->u.binop.op2.kind = OP_TEMP;
						code4->u.binop.op2.u.temp_no = t3;
						int t5 = newTemp();
						IntermediateCode* code5 = translate_Exp(first->sibling->sibling,t5);
						IntermediateCode* code6 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
						code6->prev = code6->next = NULL;
						code6->kind = IC_POINT_LEFT;
						code6->u.pairop.result.kind = OP_TEMP;
						code6->u.pairop.result.u.temp_no = t4;
						code6->u.pairop.op.kind = OP_TEMP;
						code6->u.pairop.op.u.temp_no = t5;
						IntermediateCode* code7 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
						code7->prev = code7->next = NULL;
						code7->kind = IC_ASSIGN;
						code7->u.pairop.result.kind = OP_TEMP;
						code7->u.pairop.result.u.temp_no = place;
						code7->u.pairop.op.kind = OP_TEMP;
						code7->u.pairop.op.u.temp_no = t5;

						IntermediateCode* c1 = bindCode(code6,code7);
						IntermediateCode* c2 = bindCode(code5,c1);
						IntermediateCode* c3 = bindCode(code4,c2);
						IntermediateCode* c4 = bindCode(code3,c3);
						IntermediateCode* c5 = bindCode(code2,c4);
						//IntermediateCode* c6 = bindCode(code1,c5);

						code = c5;
					}
					else if(exp1->sibling->type == DOT_)
					{
						struct TreeNode* exp1 = first->first_child;
						struct TreeNode* id = exp1->sibling->sibling;
						//int t1 = newTemp();
						//IntermediateCode* code1 = translate_Exp(exp1,t1);
						int size = 0;
						struct TreeNode* p = exp1;
						while(p->type != ID_)
							p = p->first_child;
						struct Symbol* symbol = searchTable(p->name);
						FieldList fieldlist = symbol->u.type->u.structure;
						while(strcmp(fieldlist->name,id->name)!=0)
						{
							size = size + getSize(fieldlist->type);
							fieldlist = fieldlist->tail;
						}
						int t2 = newTemp();
						IntermediateCode* code2 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
						code2->prev = code2->next = NULL;
						code2->kind = IC_PLUS;
						code2->u.binop.result.kind = OP_TEMP;
						code2->u.binop.result.u.temp_no = t2;
						code2->u.binop.op1.kind = OP_TEMP;
						code2->u.binop.op1.u.temp_no = symbol->temp_no;
						code2->u.binop.op2.kind = OP_CONSTANT_INT;
						code2->u.binop.op2.u.value_i = size;
						
						int t3 = newTemp();
						IntermediateCode* code3 = translate_Exp(first->sibling->sibling,t3);
						IntermediateCode* code4 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
						code4->prev = code4->next = NULL;
						code4->kind = IC_POINT_LEFT;
						code4->u.pairop.result.kind = OP_TEMP;
						code4->u.pairop.result.u.temp_no = t2;
						code4->u.pairop.op.kind = OP_TEMP;
						code4->u.pairop.op.u.temp_no = t3;
						IntermediateCode* code5 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
						code5->prev = code5->next = NULL;
						code5->kind = IC_ASSIGN;
						code5->u.pairop.result.kind = OP_TEMP;
						code5->u.pairop.result.u.temp_no = place;
						code5->u.pairop.op.kind = OP_TEMP;
						code5->u.pairop.op.u.temp_no = t3;

						IntermediateCode* c1 = bindCode(code4,code5);
						IntermediateCode* c2 = bindCode(code3,c1);
						IntermediateCode* c3 = bindCode(code2,c2);
						//IntermediateCode* c4 = bindCode(code1,c3);
						code = c3;
					}
				}
			}
			break;
		}
		case  LP_:
		{
			code = translate_Exp(first->sibling,place);
			break;
		}
		default: break;
	}
	struct TreeNode* second = NULL;
	if(first->sibling != NULL)
	{	second = first->sibling;
		if((first->type == Exp_ && second->type == RELOP_)||
		(first->type == Exp_ && second->type == AND_)||
		(first->type == Exp_ && second->type == OR_)||
		(first->type == NOT_ && second->type == Exp_))
		{
			int label1 = newLabel();
			int label2 = newLabel();
			IntermediateCode* code1 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
			code1->kind = IC_ASSIGN;
			code1->prev = code1->next = NULL;
			code1->u.pairop.result.kind = OP_TEMP;
			code1->u.pairop.result.u.temp_no = place;
			code1->u.pairop.op.kind = OP_CONSTANT_INT;
			code1->u.pairop.op.u.value_i = 0;
		
			IntermediateCode* code2 = translate_Cond(exp,label1,label2);
			IntermediateCode* code3 = genLabel(label1);
			IntermediateCode* code4 = (IntermediateCode*)malloc(sizeof(IntermediateCode));
			code4->kind = IC_ASSIGN;
			code4->prev = code4->next = NULL;
			code4->u.pairop.result.kind = OP_TEMP;
			code4->u.pairop.result.u.temp_no = place;
			code4->u.pairop.op.kind = OP_CONSTANT_INT;
			code4->u.pairop.op.u.value_i = 1;

			IntermediateCode* code5 = genLabel(label2);

			IntermediateCode* c1 = bindCode(code4,code5);
			IntermediateCode* c2 = bindCode(code3,c1);
			IntermediateCode* c3 = bindCode(code2,c2);
			IntermediateCode* c4 = bindCode(code1,c3);

			code = c4;
		}
	}
	return code;
}
IntermediateCode* translate_Args(struct TreeNode* args,ArgList** arg_list)
{
//	printf("Args  ");
//	printMark(args);
	int t1 = newTemp();
	struct TreeNode* exp = args->first_child;
	IntermediateCode* code1 = translate_Exp(exp,t1);
	ArgList* arg1 = (ArgList*)malloc(sizeof(ArgList));
	arg1->prev = arg1->next = NULL;
	arg1->temp_no = t1;
	if(*arg_list == NULL)
		*arg_list = arg1;
	else
	{
		ArgList* p = *arg_list;
		*arg_list = arg1;
		(*arg_list)->next = p;
		p->prev = *arg_list;
	}
	if(exp->sibling == NULL)
		return code1;
	else
	{
		struct TreeNode* args1 = exp->sibling->sibling;
		IntermediateCode* code2 = translate_Args(args1,arg_list);
		IntermediateCode* code = bindCode(code1,code2);
		return code;
	}
	return code1;
}

void genIR()
{
	IntermediateCode* code = translate_Program(Root);
	TotalCode = code;
	//printIR();
}
