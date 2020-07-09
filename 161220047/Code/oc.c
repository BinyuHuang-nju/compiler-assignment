#include "oc.h"

Reg regs_t[10];
IntermediateCode* start_ic;
IntermediateCode* end_ic;

int da_offset;
int param_offset;
DataAddrList* daList;

void init_regs()
{
	param_offset = 4;
	da_offset = 0;
	daList = NULL;

	for(int i=0;i<10;i++)
	{
		regs_t[i].name[0] = '$';
		regs_t[i].name[1] = 't';
		regs_t[i].name[2] = '0' + i;
		regs_t[i].name[3] = '\0';
		regs_t[i].unused = true;
		regs_t[i].da = NULL;
	}
}
void insert_head()
{
	printf(".data\n");
	printf("_prompt: .asciiz \"Enter an integer:\"\n");
	printf("_ret: .asciiz \"\\n\"\n");
	
	printf(".globl main\n");
	printf(".text\n");
}
void insertCode_read()
{
	printf("read:\n");
	printf("  li $v0, 4\n");
	printf("  la $a0, _prompt\n");
	printf("  syscall\n");
	printf("  li $v0, 5\n");
	printf("  syscall\n");
	printf("  jr $ra\n");
	printf("\n");
}
void insertCode_write()
{
	printf("write:\n");
	printf("  li $v0, 1\n");
	printf("  syscall\n");
	printf("  li $v0, 4\n");
	printf("  la $a0, _ret\n");
	printf("  syscall\n");
	printf("  move $v0,$0\n");
	printf("  jr $ra\n");
	printf("\n");
}

DataAddr* get_da(Operand* opr)
{
	DataAddrList* node = daList;
	for(;node != NULL;node = node->next)
	{
		if(opr->kind == OP_TEMP && node->da->kind == DATA_TEMP
			&&opr->u.temp_no == node->da->no)
			return node->da;
		if(opr->kind == OP_VARIABLE && node->da->kind == DATA_VAR
			&&opr->u.var_no == node->da->no)
			return node->da;
	}
	DataAddr* da = add_da(opr);
	return da;
}
DataAddr* add_da(Operand* opr)
{
	DataAddr* da = (DataAddr*)malloc(sizeof(DataAddr));
	if(opr->kind == OP_VARIABLE)
	{
		da->kind = DATA_VAR;
		da->no = opr->u.var_no;
	}
	else if(opr->kind == OP_TEMP)
	{
		da->kind = DATA_TEMP;
		da->no = opr->u.temp_no;
	}
	else
	{
		printf("something wrong in add_da");
	}
	da_offset = da_offset - 4;
	da->offset = da_offset;

	DataAddrList* oneNode = (DataAddrList*)malloc(sizeof(DataAddrList));
	oneNode->da = da;
	oneNode->next = daList;
	daList = oneNode;
	printf("  addi $sp, $sp, -4\n");
	return da;
}

void free_reg(Reg* reg)
{
	reg->unused = true;
	reg->da = NULL;
}
void spill_reg(Reg* reg)
{
	printf("  sw %s, %d($fp)\n",reg->name,reg->da->offset);
	free_reg(reg);
}

int allocate_reg(Operand* opr)
{
	Reg* reg = NULL;
	int flag = 0;
	//for(IntermediateCode* p = end_ic;p != start_ic; p = p->prev)
	IntermediateCode* p = NULL;
	if(end_ic != NULL)
		p = end_ic->prev;
	else // end_ic == NULL
	{
		p = start_ic;
		while(p->next != NULL)
			p = p->next;
	}
	while(1)
	{
		for(int i = 9;i >= 0; i--)
		{
			Reg* r = regs_t + i;
			if((p->u.binop.op1.kind == OP_TEMP && r->da->kind == DATA_TEMP && p->u.binop.op1.u.temp_no ==r->da->no)||(p->u.binop.op1.kind == OP_VARIABLE && r->da->kind == DATA_VAR && p->u.binop.op1.u.var_no == r->da->no))
			{
				reg =r;
				flag = i;
				spill_reg(reg);
				return flag;
			}
			if((p->u.binop.op2.kind == OP_TEMP && r->da->kind == DATA_TEMP && p->u.binop.op2.u.temp_no ==r->da->no)||(p->u.binop.op2.kind == OP_VARIABLE && r->da->kind == DATA_VAR && p->u.binop.op2.u.var_no == r->da->no))
			{
				reg =r;
				flag = i;
				spill_reg(reg);
				return flag;
			}
			if((p->u.binop.result.kind == OP_TEMP && r->da->kind == DATA_TEMP && p->u.binop.result.u.temp_no ==r->da->no)||(p->u.binop.result.kind == OP_VARIABLE && r->da->kind == DATA_VAR && p->u.binop.result.u.var_no == r->da->no))
			{
				reg =r;
				flag = i;
				spill_reg(reg);
				return flag;
			}
		}
		if(p == start_ic)
			break;
		p = p->prev;
	}
	spill_reg(regs_t+0);
	return 0;	
}

Reg* get_reg(Operand* opr)
{
	for(int i = 0;i<10;i++)
	{
		if(regs_t[i].unused==true)
		{
			regs_t[i].unused = false;
			if(opr->kind == OP_CONSTANT_INT)
				printf("  li %s, %d\n",regs_t[i].name,opr->u.value_i);
			else
			{
				regs_t[i].da = get_da(opr);
				printf("  lw %s, %d($fp)\n",regs_t[i].name,regs_t[i].da->offset);
			}
			return (regs_t + i);
		}
	}
	printf("Something wrong in get");
	return NULL;
}

void clear_da()
{
/*	old_daList = daList;
	old_da_offset = da_offset;
	old_param_offset = param_offset;*/

	da_offset = 0;
	param_offset = 4;
	daList = NULL;
}
void goto_new_function()
{
	printf("  addi $sp, $sp, -4\n");
	printf("  sw $fp, 0($sp)\n");
	printf("  move $fp, $sp\n");
}
void ret_old_function()
{
	printf("  move $sp, $fp\n");
	printf("  lw $fp, 0($sp)\n");
	printf("  addi $sp, $sp, 4\n");
}
void writeback_da()
{
/*	da_offset = old_da_offset;
	param_offset = old_param_offset;
	daList = old_daList;*/
	
}

void add_param_da(Operand* op)
{
	if(op->kind != OP_VARIABLE)
		printf("WRONG");
	DataAddr* da = (DataAddr*)malloc(sizeof(DataAddr));
	da->kind = DATA_VAR;
	da->no = op->u.var_no;
	//da->offset = param_offset;
	param_offset = param_offset + 4;
	da->offset = param_offset;
	DataAddrList* oneNode = (DataAddrList*)malloc(sizeof(DataAddrList));
	oneNode->da = da;
	oneNode->next = daList;
	daList = oneNode;
}
void add_array_da(Operand* op,int size)
{
	if(op->kind != OP_VARIABLE)
		printf("WRONG");
	DataAddr* da = (DataAddr*)malloc(sizeof(DataAddr));
	da->kind = DATA_VAR;
	da->no = op->u.var_no;
	da_offset = da_offset - size;
	da->offset = da_offset;
	DataAddrList * oneNode = (DataAddrList*)malloc(sizeof(DataAddrList));
	oneNode->da = da;
	oneNode->next = daList;
	daList = oneNode;
	printf("  addi $sp, $sp, -%d\n",size);
}

void divide_basic_block(IntermediateCode* cur_code)
{
	start_ic = cur_code;
	for(;start_ic!=NULL;start_ic = start_ic->next)
	{
		if(start_ic->kind==IC_ASSIGN||start_ic->kind==IC_PLUS||
			start_ic->kind==IC_MINUS||start_ic->kind==IC_STAR||
			start_ic->kind==IC_DIV||start_ic->kind==IC_ARG)
			break;
	}
	end_ic = start_ic;
	for(;end_ic!=NULL;end_ic = end_ic->next)
	{
		if(end_ic->kind!=IC_ASSIGN && end_ic->kind!=IC_PLUS &&
			end_ic->kind!=IC_MINUS && end_ic->kind!=IC_STAR &&
			end_ic->kind!=IC_DIV && end_ic->kind!=IC_ARG &&
			end_ic->kind!=IC_ADDR && end_ic->kind!=IC_POINT_LEFT &&
			end_ic->kind!=IC_POINT_RIGHT)
		{
			end_ic = end_ic->next;
			break;
		}
	}
}
void printOC()
{
	init_regs();
	insert_head();
	insertCode_read();
	insertCode_write();
	for(IntermediateCode* code = TotalCode; code!=NULL; code = code->next)
	{
		switch(code->kind)
		{
			case IC_ASSIGN:
			{
				Reg* rr = get_reg(&code->u.pairop.result);
				Reg* r1 = get_reg(&code->u.pairop.op);
				printf("  move %s, %s\n",rr->name,r1->name);
				spill_reg(rr);
				free_reg(r1);
				break;
			}
			case IC_PLUS: case IC_MINUS: case IC_STAR: case IC_DIV:
			{
				Reg* rr = get_reg(&code->u.binop.result);
				Reg* r1 = get_reg(&code->u.binop.op1);
				Reg* r2 = get_reg(&code->u.binop.op2);
				if(code->kind == IC_PLUS)
					printf("  add %s, %s, %s\n",rr->name,r1->name,r2->name);
				else if(code->kind == IC_MINUS)
					printf("  sub %s, %s, %s\n",rr->name,r1->name,r2->name);
				else if(code->kind == IC_STAR)
					printf("  mul %s, %s, %s\n",rr->name,r1->name,r2->name);
				else
				{
					printf("  div %s, %s\n",r1->name,r2->name);
					printf("  mflo %s\n",rr->name);
				}
				spill_reg(rr);
				free_reg(r1);
				free_reg(r2);
				break;
			}
			case IC_RELOP_G: case IC_RELOP_L: case IC_RELOP_GE:
			case IC_RELOP_LE: case IC_RELOP_EQ: case IC_RELOP_NE:
			{
				Reg* r1 = get_reg(&code->u.binop.op1);
				Reg* r2 = get_reg(&code->u.binop.op2);
				if(code->kind == IC_RELOP_G)
					printf("  bgt ");
				else if(code->kind == IC_RELOP_L)
					printf("  blt ");
				else if(code->kind == IC_RELOP_GE)
					printf("  bge ");
				else if(code->kind == IC_RELOP_LE)
					printf("  ble ");
				else if(code->kind == IC_RELOP_EQ)
					printf("  beq ");
				else
					printf("  bne ");
				printf("%s, %s, label%d\n",r1->name,r2->name,code->u.binop.result.u.label);
				free_reg(r1);
				free_reg(r2);
				break;
			}
			case IC_LABEL:
			{
				printf("label%d:\n",code->u.singleop.result.u.label);
				break;
			}
			case IC_GOTO:
			{
				printf("  j label%d\n",code->u.singleop.result.u.label);
			 	break;
			}
			case IC_FUNCTION:
			{
				printf("\n");
				printf("%s:\n",code->u.singleop.result.u.func_name);
				clear_da();
				goto_new_function();
				break;
			}
			case IC_PARAM:
			{
				add_param_da(&code->u.singleop.result);
				break;
			}
			case IC_ARG:
			{   //do not use $a0-$a3 , put args into stack
				Reg* rr = get_reg(&code->u.singleop.result);
				printf("  addi $sp, $sp, -4\n");
				printf("  sw %s, 0($sp)\n",rr->name);
				free_reg(rr);
				break;
			}
			case IC_CALL:
			{
				printf("  addi $sp, $sp, -4\n");
				printf("  sw $ra, 0($sp)\n");
				printf("  jal %s\n",code->u.pairop.op.u.func_name);
				printf("  lw $ra, 0($sp)\n");
				printf("  addi $sp, $sp, 4\n");
				Reg* rr = get_reg(&code->u.pairop.result);
				printf("  move %s, $v0\n",rr->name);
				spill_reg(rr);
				break;
			}
			case IC_RETURN:
			{
				Reg* rr = get_reg(&code->u.singleop.result);
				printf("  move $v0, %s\n",rr->name);
				ret_old_function();
				
				printf("  jr $ra\n");
				writeback_da();
				free_reg(rr);
				//free_reg(rr);
			 	break;
			}
			case IC_DEC:
			{
				add_array_da(&code->u.singleop.result,code->size);
				break;
			}
			case IC_ADDR:
			{
				Reg* rr = get_reg(&code->u.pairop.result);
				DataAddr* da = get_da(&code->u.pairop.op);
				printf("  la %s, %d($fp)\n",rr->name,da->offset);
				spill_reg(rr);
				break;
			}
			case IC_POINT_LEFT:
			{
				Reg* rr = get_reg(&code->u.pairop.result);
				Reg* r1 = get_reg(&code->u.pairop.op);
				printf("  sw %s, 0(%s)\n",r1->name,rr->name);
				free_reg(rr);
				free_reg(r1);
				break;
			}
			case IC_POINT_RIGHT:
			{
				Reg* rr = get_reg(&code->u.pairop.result);
				Reg* r1 = get_reg(&code->u.pairop.op);
				printf("  lw %s, 0(%s)\n",rr->name,r1->name);
				spill_reg(rr);
				free_reg(r1);
				break;
			}
			case IC_READ:
			{
				printf("  addi $sp, $sp, -4\n");
				printf("  sw $ra, 0($sp)\n");
				printf("  jal read\n");
				printf("  lw $ra, 0($sp)\n");
				printf("  addi $sp, $sp, 4\n");
				Reg* rr = get_reg(&code->u.singleop.result);
				printf("  move %s, $v0\n",rr->name);
				spill_reg(rr);
				break;
			}
			case IC_WRITE:
			{
				Reg* rr = get_reg(&code->u.singleop.result);
				printf("  move $a0, %s\n",rr->name);
				free_reg(rr);

				printf("  addi $sp, $sp, -4\n");
				printf("  sw $ra, 0($sp)\n");
				printf("  jal write\n");
				printf("  lw $ra, 0($sp)\n");
				printf("  addi $sp, $sp, 4\n");				
				break;
			}
			default: printf("Something left in output"); break;
		}
	}
	
}
/*

Reg* get_reg(Operand* opr)
{
	for(int i = 0;i<10;i++)
	{
		if(regs_t[i].unused==true)
		{
			regs_t[i].unused = false;
			if(opr->kind == OP_CONSTANT_INT)
				printf("  li %s, %d\n",regs_t[i].name,opr->u.value_i);
			else
			{
				regs_t[i].da = get_da(opr);
				printf("  lw %s, %d($fp)\n",regs_t[i].name,regs_t[i].da->offset);
			}
			return (regs_t + i);
		}
	}
	// every regs_t has been used , so allocate one
	int x = allocate_reg(opr);
	if(regs_t[x].unused == false)
		printf("WRONG IN ALLOCATE FIND IN GET");
	regs_t[x].unused = false;
	if(opr->kind == OP_CONSTANT_INT)
		printf("  li %s, %d\n",regs_t[x].name,opr->u.value_i);
	else
	{
		regs_t[x].da = get_da(opr);
		printf("  lw %s, %d($fp)\n",regs_t[x].name,regs_t[x].da->offset);
	}
	return (regs_t + x);
}

void printOC()
{
	init_regs();
	insert_head();
	insertCode_read();
	insertCode_write();
	IntermediateCode *currentCode = TotalCode;
	start_ic = end_ic = currentCode;
	while(end_ic != NULL)
	{
		divide_basic_block(end_ic); // argument considering
		for(IntermediateCode* code = start_ic;code!=end_ic;code=code->next)
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
					Reg* rr = get_reg(&code->u.pairop.result);
					Reg* r1 = get_reg(&code->u.pairop.op);
					printf("  move %s, %s\n",rr->name,r1->name);
					//TODO
					break;
				}
				default: break;
			}
		}
	}
}
*/

