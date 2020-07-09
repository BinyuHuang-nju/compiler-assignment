#ifndef __OC_H__
#define __OC_H__

#include "ir.h"

typedef struct DataAddr_ DataAddr;
typedef struct Reg_ Reg;
typedef struct DataAddrList_ DataAddrList;

struct DataAddr_
{
	enum { DATA_TEMP,DATA_VAR }kind;
	int no;
	int offset;
};
struct Reg_
{
	bool unused;
	char name[8];
	DataAddr* da;
};
struct DataAddrList_
{
	DataAddr *da;
	DataAddrList* next;
};

extern Reg regs_t[10];

//void test_out();

void init_regs();
void printOC();
void insert_head();
void insertCode_read();
void insertCode_write();
void divide_basic_block(IntermediateCode* cur_code);
Reg* get_reg(Operand* opr);
DataAddr* get_da(Operand* opr);
DataAddr* add_da(Operand* opr);
int allocate_reg(Operand* opr);
void spill_reg(Reg* reg);
void free_reg(Reg* reg);
void clear_da();
void goto_new_function();
void ret_old_function();
void writeback_da();
void add_param_da(Operand* op);
void add_array_da(Operand* op,int size);

#endif
