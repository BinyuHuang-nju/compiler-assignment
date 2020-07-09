%{
	#include <stdio.h>
	#include <ctype.h>
	#include <string.h>
	#include "frame.h"

	#define YYSTYPE \
		struct TreeNode*
	void yyerror(char* msg);
	int yylex(void);
	const char* any = "Non-terminal";
%}
%locations
/* declared types */
%define api.value.type {struct TreeNode*}
/*%union
{
	struct TreeNode* type_struct;
}*/
/* declared tokens */
%token INT
%token FLOAT
%token SEMI COMMA
%token STRUCT RETURN IF ELSE WHILE LOWER_THAN_ELSE
%token TYPE ID LC RC
%token ASSIGNOP OR AND RELOP PLUS MINUS STAR DIV NOT UMINUS
%token LP RP LB RB DOT
/* declared non-terminals */
/*
%type <type_struct> Program ExtDefList ExtDef ExtDecList Specifier
%type <type_struct> StructSpecifier OptTag Tag VarDec FunDec VarList
%type <type_struct> ParamDec CompSt StmtList Stmt DefList Def DecList
%type <type_struct> Dec Exp Args
*/
%start Program

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
/* right-associative: assign and unary operators. */
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT UMINUS
%left LP RP LB RB DOT

%%
/*High-level Definitions*/
Program : ExtDefList { $$ = createNode(Program_,"Program",@$.first_line);
			connect_PoneC($$,$1); Root = $$; }
	;
ExtDefList : ExtDef ExtDefList { 
			$$ = createNode(ExtDefList_,"ExtDefList",@$.first_line);
			connect_PtwoC($$,$1,$2); }
	| /*empty */ { $$ = createNode(ExtDefList_,"ExtDefList",@$.first_line);}
	;
ExtDef : Specifier ExtDecList SEMI {
			$$ = createNode(ExtDef_,"ExtDef",@$.first_line);
			connect_PthreeC($$,$1,$2,$3);}
	| Specifier SEMI {
			$$ = createNode(ExtDef_,"ExtDef",@$.first_line);
			connect_PtwoC($$,$1,$2); }
	| Specifier FunDec CompSt {
			$$ = createNode(ExtDef_,"ExtDef",@$.first_line);
			connect_PthreeC($$,$1,$2,$3); }
	| Specifier error SEMI { yyerrok;}
	| error SEMI { yyerrok; }
	;
ExtDecList : VarDec { 
			$$ = createNode(ExtDecList_,"ExtDecList",@$.first_line);
			connect_PoneC($$,$1); }
	| VarDec COMMA ExtDecList {
			$$ = createNode(ExtDecList_,"ExtDecList",@$.first_line);
			connect_PthreeC($$,$1,$2,$3); }
	| VarDec COMMA error { yyerrok; }
	| error COMMA ExtDecList { yyerrok; }
	;

/*Specifiers*/
Specifier : TYPE { $$ = createNode(Specifier_,"Specifier",@$.first_line);
			connect_PoneC($$,$1); }
	| StructSpecifier {
			$$ = createNode(Specifier_,"Specifier",@$.first_line);
			connect_PoneC($$,$1); }
	;
StructSpecifier : STRUCT OptTag LC DefList RC {
		$$ = createNode(StructSpecifier_,"StructSpecifier",@$.first_line);
			connect_PfiveC($$,$1,$2,$3,$4,$5); }
	| STRUCT Tag { 
		$$ = createNode(StructSpecifier_,"StructSpecifier",@$.first_line);
			connect_PtwoC($$,$1,$2);}
	| STRUCT OptTag LC error RC { yyerrok; }
	;
OptTag : ID { $$ = createNode(OptTag_,"OptTag",@$.first_line);
			connect_PoneC($$,$1); }
	| /*empty*/ { $$ = createNode(OptTag_,"OptTag",@$.first_line);}
	;
Tag : ID { $$ = createNode(Tag_,"Tag",@$.first_line);
			connect_PoneC($$,$1); }
	;

/*Declarators*/
VarDec : ID { $$ = createNode(VarDec_,"VarDec",@$.first_line);
			connect_PoneC($$,$1); }
	| VarDec LB INT RB {
			$$ = createNode(VarDec_,"VarDec",@$.first_line);
			connect_PfourC($$,$1,$2,$3,$4); }
	| VarDec LB error RB { yyerrok; }
	;
FunDec : ID LP VarList RP {
			$$ = createNode(FunDec_,"FunDec",@$.first_line);
			connect_PfourC($$,$1,$2,$3,$4); }
	| ID LP RP { $$ = createNode(FunDec_,"FunDec",@$.first_line);
			connect_PthreeC($$,$1,$2,$3); }
	| ID LP error RP { yyerrok; }
	;
VarList : ParamDec COMMA VarList { 
			$$ = createNode(VarList_,"VarList",@$.first_line);
			connect_PthreeC($$,$1,$2,$3); }
	| ParamDec { $$ = createNode(VarList_,"VarList",@$.first_line); 
			connect_PoneC($$,$1); }
	| error COMMA VarList { yyerrok; }
	| ParamDec COMMA error { yyerrok; }
	;
ParamDec : Specifier VarDec {
			$$ = createNode(ParamDec_,"ParamDec",@$.first_line);
			connect_PtwoC($$,$1,$2); }
	;

/*Statements*/
CompSt : LC DefList StmtList RC {
			$$ = createNode(CompSt_,"CompSt",@$.first_line);
			connect_PfourC($$,$1,$2,$3,$4); }
	| LC error RC { yyerrok; }
	;
StmtList : Stmt StmtList { 
			$$ = createNode(StmtList_,"StmtList",@$.first_line);
			connect_PtwoC($$,$1,$2); }
	| /*empty*/ { $$ = createNode(StmtList_,"StmtList",@$.first_line);}
	;
Stmt : Exp SEMI { $$ = createNode(Stmt_,"Stmt",@$.first_line);
			connect_PtwoC($$,$1,$2);}
	| CompSt { $$ = createNode(Stmt_,"Stmt",@$.first_line);
			connect_PoneC($$,$1); }
	| RETURN Exp SEMI {
			$$ = createNode(Stmt_,"Stmt",@$.first_line);
			connect_PthreeC($$,$1,$2,$3); }
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {
			$$ = createNode(Stmt_,"Stmt",@$.first_line);
			connect_PfiveC($$,$1,$2,$3,$4,$5); }
	| IF LP Exp RP Stmt ELSE Stmt {
			$$ = createNode(Stmt_,"Stmt",@$.first_line);
			connect_PsevenC($$,$1,$2,$3,$4,$5,$6,$7); }
	| WHILE LP Exp RP Stmt {
			$$ = createNode(Stmt_,"Stmt",@$.first_line);
			connect_PfiveC($$,$1,$2,$3,$4,$5); }
	| error SEMI { yyerrok; }
	| RETURN error SEMI { yyerrok; }
	| Exp error {yyerrok ;}
	| IF LP error RP Stmt %prec LOWER_THAN_ELSE { yyerrok;}
	| IF LP error RP Stmt ELSE Stmt { yyerrok; }
	| WHILE LP error RP Stmt { yyerrok; }
	;

/*Local Definitions*/
DefList : Def DefList { $$ = createNode(DefList_,"DefList",@$.first_line);
			connect_PtwoC($$,$1,$2);}
	| /*empty*/ { $$ = createNode(DefList_,"DefList",@$.first_line); }
	;
Def : Specifier DecList SEMI { $$ = createNode(Def_,"Def",@$.first_line);
			connect_PthreeC($$,$1,$2,$3); }
	| Specifier error SEMI { yyerrok; }
	| error DecList SEMI { yyerrok; }
	;
DecList : Dec { $$ = createNode(DecList_,"DecList",@$.first_line);
			connect_PoneC($$,$1); }
	| Dec COMMA DecList {
			$$ = createNode(DecList_,"DecList",@$.first_line); 
			connect_PthreeC($$,$1,$2,$3); }
	| error COMMA DecList { yyerrok; }
	| Dec COMMA error { yyerrok; }
	;
Dec : VarDec { $$ = createNode(Dec_,"Dec",@$.first_line);
			connect_PoneC($$,$1); }
	| VarDec ASSIGNOP Exp {
			$$ = createNode(Dec_,"Dec",@$.first_line);
			connect_PthreeC($$,$1,$2,$3); }
	| VarDec ASSIGNOP error { yyerrok; }
	;

/*Expressions*/
Exp : Exp ASSIGNOP Exp { $$ = createNode(Exp_,"Exp",@$.first_line);
			connect_PthreeC($$,$1,$2,$3); }
	| Exp AND Exp { $$ = createNode(Exp_,"Exp",@$.first_line);
			connect_PthreeC($$,$1,$2,$3); }
	| Exp OR Exp { $$ = createNode(Exp_,"Exp",@$.first_line);
			connect_PthreeC($$,$1,$2,$3); }
	| Exp RELOP Exp { $$ = createNode(Exp_,"Exp",@$.first_line);
			connect_PthreeC($$,$1,$2,$3); }
	| Exp PLUS Exp { $$ = createNode(Exp_,"Exp",@$.first_line);
			connect_PthreeC($$,$1,$2,$3); }
	| Exp MINUS Exp { $$ = createNode(Exp_,"Exp",@$.first_line);
			connect_PthreeC($$,$1,$2,$3); }
	| Exp STAR Exp { $$ = createNode(Exp_,"Exp",@$.first_line);
			connect_PthreeC($$,$1,$2,$3); }
	| Exp DIV Exp { $$ = createNode(Exp_,"Exp",@$.first_line);
			connect_PthreeC($$,$1,$2,$3); }
	| LP Exp RP { $$ = createNode(Exp_,"Exp",@$.first_line);
			connect_PthreeC($$,$1,$2,$3); }
	| MINUS Exp %prec UMINUS { $$ = createNode(Exp_,"Exp",@$.first_line);
			connect_PtwoC($$,$1,$2);}
	| NOT Exp { $$ = createNode(Exp_,"Exp",@$.first_line);
			connect_PtwoC($$,$1,$2);}
	| ID LP Args RP { $$ = createNode(Exp_,"Exp",@$.first_line);
			connect_PfourC($$,$1,$2,$3,$4);}
	| ID LP RP { $$ = createNode(Exp_,"Exp",@$.first_line);
			connect_PthreeC($$,$1,$2,$3); }
	| Exp LB Exp RB {
			$$ = createNode(Exp_,"Exp",@$.first_line);
			connect_PfourC($$,$1,$2,$3,$4);}
	| Exp DOT ID { $$ = createNode(Exp_,"Exp",@$.first_line);
			connect_PthreeC($$,$1,$2,$3); }
	| ID { $$ = createNode(Exp_,"Exp",@$.first_line);
			connect_PoneC($$,$1);}
	| INT { $$ = createNode(Exp_,"Exp",@$.first_line);
			connect_PoneC($$,$1);}
	| FLOAT { $$ = createNode(Exp_,"Exp",@$.first_line);
			connect_PoneC($$,$1);}
	| ID LP error RP { yyerrok; }
	| Exp LB error RB { yyerrok; }
	| Exp error Exp { yyerrok; }
	;
Args : Exp COMMA Args {
			$$ = createNode(Args_,"Args",@$.first_line);
			connect_PthreeC($$,$1,$2,$3); }
	| Exp { $$ = createNode(Args_,"Args",@$.first_line);
			connect_PoneC($$,$1); }
	;

%%
#include "lex.yy.c"

void yyerror(char* msg)
{
	printf("Error type B at Line %d: syntax error \n",
		yylineno);
	total_error += 1;
}

