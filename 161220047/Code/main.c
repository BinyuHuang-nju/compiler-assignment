#include <stdio.h>
#include "syntax.tab.h"
#include "frame.h"
#include "semantic.h"
#include "ir.h"
#include "oc.h"
extern FILE* yyin;
extern int yylex();
extern void yyrestart(FILE* file);

int main(int argc,char** argv)
{
	if(argc > 1)
	{
		if(!(yyin = fopen(argv[1],"r")))
		{
			perror(argv[1]);
			return 1;
		}
	}
	FILE* outfile = NULL;
	if(argc == 3)
		outfile = freopen(argv[2],"w",stdout);

	total_error = 0;
 //	YYDEBUG = 1;
	yyrestart(yyin);
	yyparse();
#ifdef SYNTAX_TREE_ENABLED
	if(total_error == 0)
		printTree(Root,0);
#endif

	initTable();
	insertTable_read();
	insertTable_write();
#ifdef SEMANTIC_ENABLED
	initExp(Root,no_use);
	semantic_analyse(Root);
#endif

#ifdef SYMBOL_TABLE_ENABLED
	outputTable();
#endif

#ifdef IR_ENABLED
	initCode();
	genIR();
#endif
#ifdef IR_OUTPUT_ENABLED
	printIR();
#endif

#ifdef OC_ENABLED
	printOC();
//	test_out();
#endif

	if(argc == 3)
		fclose(outfile);
	return 0;
}
