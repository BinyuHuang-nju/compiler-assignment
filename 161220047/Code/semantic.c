#include "semantic.h"
struct Symbol* typeTable[MAX_SYMBOL_SIZE];
struct Symbol* symbolTable[MAX_SYMBOL_SIZE];
int field[MAX_FIELD_SIZE];
int current_field_size = 0;
int current_type_size = 0;
int current_symbol_size = 0;
int struct_depth = 0;
struct Symbol* current_func = NULL;
Semtype no_use;
int exist_error = 0;

void outputTable()
{
	for(int i = 0;i<current_symbol_size;i++)
		printf("%d %s\n",i,symbolTable[i]->name);
	printf("--------------------------\n");
	for(int i = 0;i<current_type_size;i++)
		printf("%d %s\n",i,typeTable[i]->name);
	printf("--------------------------\n");
}

struct Symbol* searchType(char* name)
{
	for(int i=0;i<current_type_size;i++)
		if(strcmp(name,typeTable[i]->name)==0)
			return typeTable[i];
	return NULL;
}
void insertType(struct Symbol* symbol)
{
	typeTable[current_type_size] = symbol;
	current_type_size = current_type_size + 1;
}
struct Symbol* searchTable(char* name)
{
	for(int i = 0;i < current_symbol_size;i++)
	{
		if(strcmp(name,symbolTable[i]->name)==0)
		{
			return symbolTable[i];
		}
	}
	return NULL;
}
void insertTable(struct Symbol* symbol)
{
//	symbol->field = current_field_size;
	symbolTable[current_symbol_size] = symbol;
	current_symbol_size = current_symbol_size+1;
}
void initTable()
{
	for(int i=0;i<MAX_SYMBOL_SIZE;i++)
	{	
		symbolTable[i] = NULL;
		typeTable[i] = NULL;
	}
	for(int i=0;i<MAX_FIELD_SIZE;i++)
		field[i] = 0;
	current_type_size = 0;
	current_field_size = 0;
	current_symbol_size = 0;
	struct_depth = 0;
	current_func = NULL;
	no_use = (Semtype)malloc(sizeof(struct Type_));
	no_use->kind = INIT;
	exist_error = 0;
}
void initExp(struct TreeNode* root,Semtype type)
{
	if(root!=NULL)
	{
		root->exp_type = type;
		struct TreeNode* temp = root->first_child;
		while(temp!=NULL)
		{
			initExp(temp,type);
			temp = temp->sibling;
		}
	}
}
Semtype genStructType(struct TreeNode* structspecifier)
{
	Semtype newtype = (Semtype)malloc(sizeof(struct Type_));
	newtype->kind = STRUCTURE;
	newtype->lineno = structspecifier->lineno;
	newtype->u.structure = NULL;
	struct TreeNode* opttag=structspecifier->first_child->sibling;
	struct TreeNode* deflist=opttag->sibling->sibling;
	newtype->u.structure = NULL;
	newtype->u.structure = genFieldList(newtype->u.structure,deflist);
	if(opttag->first_child!=NULL)
	{
		struct Symbol* symbol=(struct Symbol*)malloc(sizeof(struct Symbol));
		symbol->kind = VARIABLE;
		strcpy(symbol->name,opttag->first_child->name);
		symbol->u.type = newtype;
		symbol->field = current_field_size;
		symbol->var_no = 0;
		//printf("struct_depth = %d\n",struct_depth);
		if(struct_depth==0)
		{	
			if(searchType(symbol->name)==NULL)
				insertType(symbol);
			else
				printf("Error type 16 at Line %d:Duplicated structure name \"%s\".\n",newtype->lineno,symbol->name);
		}
	}
	return newtype;
}
FieldList genFieldList(FieldList structure,struct TreeNode* deflist)
{
	if(deflist->first_child==NULL)
		return structure;
	struct TreeNode* def = deflist->first_child;
	struct TreeNode* declist = def->first_child->sibling;
	struct TreeNode* dec = declist->first_child;
	Semtype type = getType(def->first_child);
	do
	{
		dec = declist->first_child;
		struct TreeNode* vardec = dec->first_child;
		struct Symbol* symbol = (struct Symbol*)malloc(sizeof(struct Symbol));
		symbol->var_no = 0;
		if(vardec->first_child->sibling!=NULL)
		{
			symbol = genVarDecArray(type,vardec->first_child);
		}
		else
		{
			symbol->kind = VARIABLE;
			symbol->field = current_field_size;
			symbol->u.type = type;
			strcpy(symbol->name,vardec->first_child->name);
		}
		if(dec->first_child->sibling!=NULL)
			printf("Error type 15 at Line %d:Initialize argument \"%s\" in the definition of structure.\n",dec->first_child->lineno,symbol->name);
		FieldList fieldlist = (FieldList)malloc(sizeof(struct FieldList_));
		fieldlist->type = symbol->u.type;
		strcpy(fieldlist->name,symbol->name);
		if(structure == NULL)
		{
			structure = fieldlist;
			structure->tail = NULL;
		}
		else
		{
			FieldList fie = structure;
			int is_redefined = 0;
			while(fie!=NULL)
			{
				if(strcmp(fie->name,fieldlist->name)==0)
				{
					is_redefined = 1;
					break;
				}
				if(fie->tail == NULL)
					break;
				fie = fie->tail;
			}
			//fie = structure;
			//while(fie->tail!=NULL)
			//	fie = fie->tail;
			if(is_redefined == 0)
			{
				fie->tail = fieldlist;
				fieldlist->tail = NULL;
			}
			else
			{
				printf("Error type 15 at Line %d:Redefined field \"%s\".\n",fieldlist->type->lineno,fieldlist->name);
				
			}
		}
		if(dec->sibling!=NULL)
			declist = dec->sibling->sibling;
		//else
		//	break;
	}while(dec->sibling != NULL);

	return genFieldList(structure,def->sibling);
}
//FieldList getFieldList(FieldList structure,char* name){}
struct Symbol* genVarDecArray(Semtype type,struct TreeNode* vardec)
{
	int size = vardec->sibling->sibling->value.i;
	//printf("Array size: %d\n",size);
	struct Symbol* symbol = (struct Symbol*)malloc(sizeof(struct Symbol));
	symbol->var_no = 0;
	Semtype type_arr = (Semtype)malloc(sizeof(struct Type_));
	type_arr->lineno = vardec->first_child->lineno;
	if(vardec->first_child->sibling==NULL)
	{
		type_arr->kind = ARRAY;
		type_arr->u.array.size = size;
		type_arr->u.array.elem = type;
		symbol->kind = VARIABLE;
		symbol->field = current_field_size;
		symbol->u.type = type_arr;
		strcpy(symbol->name,vardec->first_child->name);
	}
	else
	{
		type_arr->kind = ARRAY;
		type_arr->u.array.size = size;
		type_arr->u.array.elem = type;
		symbol = genVarDecArray(type_arr,vardec->first_child);
	}
	return symbol;
}
struct Symbol* genVarDec(Semtype type,struct TreeNode* vardec)
{
	struct Symbol* symbol = (struct Symbol*)malloc(sizeof(struct Symbol));
	if(vardec->first_child->sibling==NULL)
	{
		symbol->var_no = 0;
		symbol->kind = VARIABLE;
		symbol->u.type = type;
		symbol->field = current_field_size;
		strcpy(symbol->name,vardec->first_child->name);
	}
	else
	{
		symbol = genVarDecArray(type,vardec->first_child);
	}
	return symbol;
}
Semtype getType(struct TreeNode* specifier)
{
	Semtype newtype = NULL;
	struct TreeNode* p = specifier->first_child;
//	printf("%s\n",p->name);
	if(p->type == StructSpecifier_)
	{
		if(p->first_child->sibling->type == OptTag_)
			newtype = genStructType(specifier->first_child);
		else
		{
			char* name=p->first_child->sibling->first_child->name;
			struct Symbol* find = searchType(name);
			if(find==NULL)
				printf("Error type 17 at Line %d:Undefined structure \"%s\".\n",
			p->first_child->lineno,name);
			else
				newtype = find->u.type;
		}
	}
	else
	{
		newtype = (Semtype)malloc(sizeof(struct Type_));
		newtype->kind = BASIC;
		newtype->lineno = p->lineno;
		//char* name = p->name;
		if(strcmp(p->name,"int")==0)
			newtype->u.basic = BASIC_INT;
		else
			newtype->u.basic = BASIC_FLOAT;
	}
	return newtype;
}
FieldList genParamDec(FieldList paramlist,struct TreeNode* varlist)
{
	struct TreeNode* paramdec = varlist->first_child;
	struct TreeNode* specifier = paramdec->first_child;
	struct TreeNode* vardec = specifier->sibling;
	Semtype type = getType(specifier);
	struct Symbol* symbol = genVarDec(type,vardec);
	FieldList fieldlist = (FieldList)malloc(sizeof(struct FieldList_));
	fieldlist->type = symbol->u.type;
	strcpy(fieldlist->name,symbol->name);
	if(paramlist == NULL)
	{
		paramlist = fieldlist;
		paramlist->tail = NULL;
	}
	else
	{
		FieldList fie = paramlist;
		int is_redefined = 0;
		while(fie!=NULL)
		{
			if(strcmp(fie->name,fieldlist->name)==0)
			{
				is_redefined = 1;
				break;
			}
			if(fie->tail == NULL)
				break;
			fie = fie->tail;
		}
		//fie = paramlist;
		//while(fie->tail!=NULL)
		//	fie = fie->tail;
		if(is_redefined == 0)
		{
			fie->tail = fieldlist;
			fieldlist->tail = NULL;
		}
		else
		{
			printf("Error type 3 at Line %d:Redefined variable \"%s\".\n",fieldlist->type->lineno,fieldlist->name);
		}
	}
	//  parameter of function inserted into table.
	if(searchTable(fieldlist->name)==NULL)
		insertTable(symbol);
	else
		printf("Error type 3 at Line %d:Redefined variable \"%s\".\n",paramdec->lineno,fieldlist->name);
	if(paramdec->sibling!=NULL)
	{
		paramlist =genParamDec(paramlist,paramdec->sibling->sibling);
	}
	return paramlist;
}
void gothroughExtDecList(Semtype type,struct TreeNode* extdeclist)
{
	struct TreeNode* vardec = extdeclist->first_child;
	struct Symbol* symbol = genVarDec(type,vardec);
	if(searchTable(symbol->name)==NULL)
		insertTable(symbol);
	else
	{
		printf("Error type 3 at Line %d:Redefined variable \"%s\".\n",vardec->lineno,symbol->name);
	}
	if(vardec->sibling!=NULL)
		gothroughExtDecList(type,vardec->sibling->sibling);
}
void gothroughFunDec(Semtype type,struct TreeNode* fundec)
{
	struct Symbol* symbol = (struct Symbol*)malloc(sizeof(struct Symbol));
	symbol->var_no = 0;
	symbol->kind = FUNCTION;
	strcpy(symbol->name,fundec->first_child->name);
	symbol->field = current_field_size;
	Function func = (Function)malloc(sizeof(struct Function_));
	func->retType = type;
	func->lineno = fundec->lineno;
	func->def = fundec->sibling;
	if(fundec->first_child->sibling->sibling->type == RP_)
	{	
		func->paramlist = NULL;
		func->paramsize = 0;
	}
	else
	{
		FieldList paramlist = NULL;
		func->paramlist = genParamDec(paramlist,fundec->first_child->sibling->sibling);
		int size = 0;
		for(FieldList p = func->paramlist;p!=NULL;p = p->tail)
			size++;
		func->paramsize = size;
	}
	symbol->u.func = func;
	current_func = symbol;
	if(searchTable(symbol->name)==NULL)
		insertTable(symbol);
	else
		printf("Error type 4 at Line %d:Redefined function \"%s\".\n",fundec->lineno,symbol->name);
}
int checkTypeEqual(Semtype type1,Semtype type2)
{
	if(type1 == NULL && type2 == NULL)
		return 1;
	if(type1->kind != type2->kind)
		return 0;
	if(type1->kind == BASIC)
	{
		if(type1->u.basic == type2->u.basic)
			return 1;
		return 0;
	}
	if(type1->kind == ARRAY)
		return checkTypeEqual(type1->u.array.elem,type2->u.array.elem);
	if(type1->kind == STRUCTURE)
		return checkTypeStructEqual(type1->u.structure,type2->u.structure);
	printf("Something wrong in checking if one type is equal to another.\n");
	return 0;
}
int checkTypeStructEqual(FieldList fieldlist1,FieldList fieldlist2)
{
	if(fieldlist1 ==NULL && fieldlist2 == NULL)
		return 1;
	if(fieldlist1 == NULL || fieldlist2 == NULL)
		return 0;
	if(checkTypeEqual(fieldlist1->type,fieldlist2->type)==0)
		return 0;
	return checkTypeStructEqual(fieldlist1->tail,fieldlist2->tail);
}
void gothroughDecList(Semtype type,struct TreeNode* declist)
{
	struct TreeNode* dec = declist->first_child;
	struct TreeNode* vardec = dec->first_child;
	struct Symbol* symbol = genVarDec(type,vardec);
	if(vardec->sibling != NULL)
	{
		struct TreeNode* exp = vardec->sibling->sibling;
		Semtype exp_type = getExpType(exp);
		if(checkTypeEqual(symbol->u.type,exp_type)==0)
			printf("Error type 5 at Line %d:Type mismatched for assignment.\n",vardec->sibling->lineno);
		else
		{
			if(searchTable(symbol->name)==NULL)
				insertTable(symbol);
			else
				printf("Error type 3 at Line %d:Redefined variable \"%s\".\n",vardec->lineno,symbol->name);
		}
	}
	else
	{
		if(searchTable(symbol->name)==NULL)
			insertTable(symbol);
		else
			printf("Error type 3 at Line %d:Redefined variable \"%s\".\n",vardec->lineno,symbol->name);
	}
	if(dec->sibling!=NULL)
		gothroughDecList(type,dec->sibling->sibling);
}
void checkStmt(struct TreeNode* stmt)
{
	struct TreeNode* first = stmt->first_child;
	switch(first->type)
	{
		case IF_: case WHILE_:
		{
			struct TreeNode* exp = first->sibling->sibling;
			Semtype type = getExpType(exp);
			//printf("%d\n",type->kind);
			if(type!=NULL && (type->kind!=BASIC||type->u.basic!=BASIC_INT))
				printf("Error type 7 at Line %d:Type mismatched for operands.\n",exp->lineno);
			break;
		}
		case RETURN_:
		{
			if(current_func == NULL)
				printf("What????? (current function wrong.)\n");
			else
			{
				struct TreeNode* exp = first->sibling;
				Semtype exp_type = getExpType(exp);
				if(checkTypeEqual(current_func->u.func->retType,exp_type)==0)
					printf("Error type 8 at Line %d:Type mismatched for return.\n",exp->lineno);
			}
			break;
		}
		default:break;//done at other places
	}
}

Semtype getExpType(struct TreeNode* exp)
{
	if(exp->exp_type!=no_use)
		return exp->exp_type;
	Semtype type = NULL;
	struct TreeNode* first = exp->first_child;
	switch(first->type)
	{
		case Exp_:
		{
			type = getExpType(first);
			struct TreeNode* second = first->sibling;
			switch(second->type)
			{
				case ASSIGNOP_:
				{
					if(checkLeftValue(first)==0)
					{
						type = NULL;
						printf("Error type 6 at Line %d:The left-hand side of an assignment must be a variable.\n",first->lineno);
					}
					else 
					{
						Semtype type2 = getExpType(second->sibling);
						if(checkTypeEqual(type,type2)==0)
						{
							type = NULL;
							printf("Error type 5 at Line %d:Type mismatched for assignment.\n",second->lineno);
						}
					}
					break;
				}
				case AND_: case OR_:
				{
					Semtype type2 = getExpType(second->sibling);
					if((type!=NULL&&type2!=NULL&&
						(type->kind!=BASIC||type->u.basic!=BASIC_INT||type2->kind!=BASIC||type2->u.basic!=BASIC_INT))||checkTypeEqual(type,type2)==0)
					{
						type = NULL;
						printf("Error type 7 at Line %d:Type mismatched for operands.\n",second->lineno);
					}
					else
					{
						type = (Semtype)malloc(sizeof(struct Type_));
						type->kind = BASIC;
						type->lineno = second->lineno;
						type->u.basic = BASIC_INT;
					}
					break;
				}
				case RELOP_:
				{
					Semtype type2 = getExpType(second->sibling);
					if((type!=NULL&&type2!=NULL&&(type->kind!=BASIC||type2->kind!=BASIC))||checkTypeEqual(type2,type2)==0)
					{
						type = NULL;
						printf("Error type 7 at Line %d:Type mismatched for operands.\n",second->lineno);
					}
					else
					{
						type = (Semtype)malloc(sizeof(struct Type_));
						type->kind = BASIC;
						type->lineno = second->lineno;
						type->u.basic = BASIC_INT;
					}
					break;
				}
				case PLUS_: case MINUS_: case STAR_ : case DIV_:
				{
					Semtype type2 = getExpType(second->sibling);
					if((type!=NULL&&type2!=NULL&&(type->kind!=BASIC||type2->kind!=BASIC))||checkTypeEqual(type,type2)==0)
					{
						type = NULL;
						printf("Error type 7 at Line %d:Type mismatched for operands.\n",second->lineno);
					}
					break;
				}
				case LB_:
				{
					if(type!=NULL)
					{
						if(type->kind != ARRAY)
						{
							type = NULL;
							printf("Error type 10 at Line %d: \"%s\" is not an array\n",first->lineno,first->first_child->name);
						}
						else
						{
							Semtype type2 = getExpType(second->sibling);
							if(type2!=NULL&&(type2->kind!=BASIC||type2->u.basic!=BASIC_INT))
							{
								type = NULL;
								printf("Error type 12 at Line %d: \"%s\" is not an integer.\n",second->sibling->lineno,second->sibling->first_child->name);
								//TODO
								exist_error++;
							}
							else
								type = type->u.array.elem;
						}
					}
					
					break;
				}
				case DOT_:
				{
					if(type!=NULL)
					{
						if(type->kind != STRUCTURE)
						{
							type = NULL;
							printf("Error type 13 at Line %d:Illegal use of \".\".\n",second->lineno);
						}
						else
						{
							char* name = second->sibling->name;
							FieldList fieldlist = type->u.structure;
							FieldList found = NULL;
							while(fieldlist!=NULL)
							{
								if(strcmp(fieldlist->name,name)==0)
								{
									found = fieldlist;
									break;
								}
								fieldlist = fieldlist->tail;
							}
							if(found == NULL)
							{
								type = NULL;
								printf("Error type 14 at Line %d:Non-existent field \"%s\".\n",second->sibling->lineno,name);
							}
							else
								type = found->type;
						}
					}
					break;
				}
			}
			break;
		}
		case LP_:
		{
			type = getExpType(first->sibling);
			break;
		}
		case MINUS_:
		{
			type = getExpType(first->sibling);
			if(type!=NULL&&type->kind!=BASIC)
			{
				type = NULL;
				printf("Error type 7 at Line %d:Type mismatched for operands.\n",first->sibling->lineno);
			}
			break;
		}
		case NOT_:
		{
			type = getExpType(first->sibling);
			if(type!=NULL && (type->kind!=BASIC||type->u.basic!=BASIC_INT))
			{
				type = NULL;
				printf("Error type 7 at Line %d:Type mismatched for operands.\n",first->sibling->lineno);
			}
			break;
		}
		case INT_:
		{
			type = (Semtype)malloc(sizeof(struct Type_));
			type->kind = BASIC;
			type->lineno = first->lineno;
			type->u.basic = BASIC_INT;
			break;
		}
		case FLOAT_:
		{
			type = (Semtype)malloc(sizeof(struct Type_));
			type->kind = BASIC;
			type->lineno = first->lineno;
			type->u.basic = BASIC_FLOAT;
			break;
		}
		case ID_:
		{
			//deal with ID whose parent is exp
			struct Symbol* symbol = searchTable(first->name);
			if(symbol==NULL)
			{
				FieldList paramlist = current_func->u.func->paramlist;
				while(paramlist!=NULL)
				{
					if(strcmp(first->name,paramlist->name)==0)
					{
						symbol = (struct Symbol*)malloc(sizeof(struct Symbol));
						symbol->field = current_field_size;
						symbol->kind = VARIABLE;
						symbol->u.type = paramlist->type;
						strcpy(symbol->name,paramlist->name);
						break;
					}
					paramlist = paramlist->tail;
				}
			}
			if(symbol==NULL)
			{
				type = NULL;
				if(first->sibling == NULL)
				{	printf("Error type 1 at Line %d:Undefined variable \"%s\".\n",first->lineno,first->name);
					//TODO
					exist_error++;
				}
				else
					printf("Error type 2 at Line %d:Undefined function \"%s\".\n",first->lineno,first->value.c);
			}
			else
			{	
				if(first->sibling == NULL)
				{
					if(symbol->kind !=VARIABLE)
						printf("function->variable\n");
					
					type = symbol->u.type;
				}
				else //first->sibling != NULL ---> FUNCTION
				{
					if(symbol->kind != FUNCTION)
					{	printf("Error type 11 at Line %d: \"%s\" is not a function.\n",first->lineno,first->name);
						type = NULL;
					}
					else //symbol -> kind == FUNCTION
					{
						struct TreeNode* args = first->sibling->sibling;
						if(args->type == RP_)
						{	
							if(symbol->u.func->paramsize!=0)
								printf("Error type 9 at Line %d:Function \"%s\" is not applicable.\n",first->lineno,first->name);
						}
						else // args->type == Args_
							checkArgs(args,symbol->u.func->paramlist,first->name);
						
						type = symbol->u.func->retType;
					}
				}
			}
			break;
		}
		default : break;
	}
	exp->exp_type = type;
	return type;
}
void checkArgs(struct TreeNode* args,FieldList paramlist,char* func_name)
{
	int is_match = 1;
	if(paramlist==NULL)
		is_match = 0;
	else
	{
		struct TreeNode* exp = args->first_child;
		Semtype exp_type = getExpType(exp);
		is_match = checkTypeEqual(paramlist->type,exp_type);
		//printf("2 %d\n",is_match);
		if(is_match == 1 && exp->sibling != NULL)
		{	
			checkArgs(exp->sibling->sibling,paramlist->tail,func_name);
			return;
		}
		else if(exp->sibling == NULL && paramlist->tail!=NULL)
			is_match = 0;
		//printf("3 %d\n",is_match);
	}
	//printf("4 %d\n",is_match);
	if(is_match == 0)
		printf("Error type 9 at Line %d:Function \"%s\" is not applicable.\n",args->first_child->lineno,func_name);
}


int checkLeftValue(struct TreeNode* exp)
{
	struct TreeNode* first = exp->first_child;
	if(first->type == Exp_)
	{
		struct TreeNode* second = first->sibling;
		if(second->type==ASSIGNOP_||second->type==LB_||second->type==DOT_)
			return 1;
		return 0;
	}
	else if(first->type == LP_)
	{
		return checkLeftValue(first->sibling);
	}
	else if(first->type == ID_)
	{
		if(first->sibling==NULL)
			return 1;
		return 0;
	}
	else
		return 0;
	return 0;
}
void semantic_analyse(struct TreeNode* parent)
{
	if(parent == NULL)
		return;
	if(parent->type > WHILE_ && parent->first_child==NULL)
		return;
	switch(parent->type)
	{
		case ExtDef_:
		{
//			printf("in\n");
			Semtype type = getType(parent->first_child);
			// include ExtDef->Specifier SEMI condition
			if(type!=NULL)
			{
				if(parent->first_child->sibling->type == ExtDecList_)
				{
					gothroughExtDecList(type,parent->first_child->sibling);
				}
				else if(parent->first_child->sibling->type == FunDec_)
				{
					current_field_size++;
					gothroughFunDec(type,parent->first_child->sibling);
				}
			}
			break;
		}
		case StructSpecifier_://done in parent node 'ExtDef'	
		{	
			if(parent->first_child->sibling->type == OptTag_)
				struct_depth++;
			break;
		}
		//case CompSt_:
		//{
		//	current_field_size = current_field_size +1;
		//	break;
		//}
		case Stmt_:
		{	
			if(parent->first_child->type == CompSt_)
				current_field_size++;
			checkStmt(parent);
			break;
		}
		default: break;
	}

	for(struct TreeNode *p=parent->first_child;p!=NULL;p=p->sibling)
		semantic_analyse(p);
	
	switch(parent->type)
	{
		case StructSpecifier_:
		{
			if(parent->first_child->sibling->type == OptTag_)
				struct_depth = struct_depth-1;
			break;
		}
		case Def_:
		{
			if(struct_depth == 0)
			{
				Semtype type = getType(parent->first_child);
				if(type != NULL)
				{
					gothroughDecList(type,parent->first_child->sibling);
				}
				// else have done in getType
			}
			// the case of struct_depth != 0 was done in struct and
			// do not need to save in the symboltable.
			break;
		}
		case Exp_:
		{
			getExpType(parent);
			break;
		}
		case Stmt_:
		{
			//checkStmt(parent);
			//if(parent->first_child->type == CompSt_)
			//	current_field_size--;
			break;
		}
		case ID_:
		{
			// one part done in exp ,onr part done in fundec/vardec
		 	break;
		}
		//case FunDec_:
		//{
		//	current_field_size--;
		//	break;
		//}
		case CompSt_:
		{	
			current_field_size = current_field_size-1;
			break;
		}
		default: break;
	}
}
