%{
	#include <stdio.h>
	#include <stdlib.h> // atoi,stof
	#include <ctype.h>
	#include <assert.h>
	#include <string.h> // strcpy,strlen
	#include "frame.h"
	#include "syntax.tab.h"
	int lines = 0;

//	#ifndef YYSTYPE
//	#define YYSTYPE (struct TreeNode*)
//	#endif

	int yycolumn = 1;
	#define YY_USER_ACTION \
		yylloc.first_line = yylloc.last_line = yylineno; \
		yylloc.first_column = yycolumn; \
		yylloc.last_column = yycolumn + yyleng - 1; \
		yycolumn = yycolumn + yyleng;

/*	enum { INT_,FLOAT_,ID_,SEMI_,COMMA_,ASSIGNOP_,RELOP_,PLUS_,
		MINUS_,STAR_,DIV_,AND_,OR_,DOT_,NOT_,TYPE_,LP_,RP_,LB_,RB_,
		LC_,RC_,STRUCT_,RETURN_,IF_,ELSE_,WHILE_
	};
*/
%}
%option yylineno
%x COMMENT

TOKEN_INT (0|([1-9][0-9]*)|(0[1-7][0-7]*)|(0[xX][1-9a-fA-F][0-9a-fA-F]*))
TOKEN_FLOAT (([0-9]*\.[0-9]+)|([0-9]+\.))(([eE][+-]?[0-9]+)?)
TOKEN_ID [a-zA-Z_]([a-zA-Z_0-9]{0,31})
TOKEN_SEMI ;
TOKEN_COMMA ,
TOKEN_ASSIGNOP =
TOKEN_RELOP (>)|(<)|(>=)|(==)|(!=)|(<=)
TOKEN_PLUS \+
TOKEN_MINUS -
TOKEN_STAR \*
TOKEN_DIV \/
TOKEN_AND &&
TOKEN_OR \|\|
TOKEN_DOT \.
TOKEN_NOT \!
TOKEN_TYPE int|float 
TOKEN_LP \(
TOKEN_RP \)
TOKEN_LB \[
TOKEN_RB \]
TOKEN_LC \{
TOKEN_RC \}
TOKEN_STRUCT struct
TOKEN_RETURN return
TOKEN_IF if
TOKEN_ELSE else
TOKEN_WHILE while

%%

{TOKEN_INT} {//printf("Line %d: (INT, %s)\n",yylineno,yytext);
		yylval = createNode(INT_,yytext,yylineno);
//		yylval->value.i = atoi(yytext);
		if(yylval->name[0]=='0' && (yylval->name[1]=='x'||yylval->name[1]=='X'))
		{
			int len = strlen(yylval->name);
			int sum = 0,temp = 0;
			int sub = 1;
			for(int i = len-1;i>=2;i--)
			{
				if(yylval->name[i]>='0'&& yylval->name[i]<='9')
					temp = yylval->name[i]-'0';
				else if(yylval->name[i]>='a'&& yylval->name[i]<='f')
					temp = yylval->name[i]-'a'+10;
				else if(yylval->name[i]>='A'&& yylval->name[i]<='F')
					temp = yylval->name[i]-'A'+10;
				sum = sum + temp*sub;
				sub = sub*16;
			}
			yylval->value.i = sum;
		}
		else if(yylval->name[0]=='0')
		{
			int len = strlen(yylval->name);
			int sum = 0,temp = 0;
			int sub = 1;
			for(int i = len-1;i>=1;i--)
			{
				temp = yylval->name[i]-'0';
				sum = sum + temp*sub;
				sub = sub*8;
			}
			yylval->value.i = sum;
		}
		else
			yylval->value.i = atoi(yytext);
		return INT;
		}
{TOKEN_FLOAT} {//printf("Line %d: (FLOAT, %s)\n",yylineno,yytext);
		yylval = createNode(FLOAT_,yytext,yylineno);
//		yylval->value.f = (float)atof(yytext);
		int tag = 0;
		int len = strlen(yylval->name);
		for(int i=0;i<len;i++)
			if(yylval->name[i]=='e'||yylval->name[i]=='E')
				tag = i;
		if(tag > 0)
		{
			char temp[32];
			for(int i=0;i<tag;i++)
				temp[i]=yylval->name[i];
			temp[tag] = '\0';
			float base = (float)atof(temp);
			for(int i=tag+1;i<len;i++)
				temp[i-tag-1] = yylval->name[i];
			temp[len-tag-1] = '\0';
			int sub = atoi(temp);
			float n=1;
			if(sub >= 0)
			{
				for(int i=0;i<sub;i++)
					n = n*10;
			}
			else
				for(int i=0;i<(-sub);i++)
					n = n/10;
			yylval->value.f = base*n;
		}
		else
		{
			yylval->value.f = (float)atof(yytext);
		}
		return FLOAT;
		}
{TOKEN_SEMI} { yylval = createNode(SEMI_,"SEMI",yylineno); 
			strcpy(yylval->value.c,yytext); return SEMI;}
{TOKEN_COMMA} { yylval = createNode(COMMA_,"COMMA",yylineno);
			strcpy(yylval->value.c,yytext);  return COMMA;}
{TOKEN_RELOP} {yylval = createNode(RELOP_,"RELOP",yylineno);
			strcpy(yylval->value.c,yytext);
			return RELOP; // Shift the sequence to avoid the conflict;
			}

"//".* { /* Do not need to do anything. */}
"/*" { BEGIN COMMENT;}
<COMMENT>"*/" { BEGIN INITIAL;}
<COMMENT>.|\n { }
<COMMENT><<EOF>> {printf("Error type A at Line %d: COMMENT NO END\n",yylineno);  				assert(0); }

{TOKEN_ASSIGNOP} { 
		yylval = createNode(ASSIGNOP_,"ASSIGNOP",yylineno);
		strcpy(yylval->value.c,yytext);
			return ASSIGNOP;}
{TOKEN_PLUS} { yylval = createNode(PLUS_,"PLUS",yylineno);
			strcpy(yylval->value.c,yytext); return PLUS;}
{TOKEN_MINUS} { yylval = createNode(MINUS_,"MINUS",yylineno);
			strcpy(yylval->value.c,yytext); return MINUS;}
{TOKEN_STAR} { yylval = createNode(STAR_,"STAR",yylineno);
			strcpy(yylval->value.c,yytext); return STAR;}
{TOKEN_DIV} { yylval = createNode(DIV_,"DIV",yylineno);
			strcpy(yylval->value.c,yytext); return DIV;}
{TOKEN_AND} {yylval = createNode(AND_,"AND",yylineno);
			strcpy(yylval->value.c,yytext); return AND;}
{TOKEN_OR} { yylval = createNode(OR_,"OR",yylineno);
			strcpy(yylval->value.c,yytext); return OR;}
{TOKEN_DOT} { yylval = createNode(DOT_,"DOT",yylineno);
			strcpy(yylval->value.c,yytext); return DOT;}
{TOKEN_NOT} { yylval = createNode(NOT_,"NOT",yylineno);
			strcpy(yylval->value.c,yytext); return NOT;}
{TOKEN_TYPE} { //printf("Line %d: (TYPE, %s)\n",yylineno,yytext); 
		yylval = createNode(TYPE_,yytext,yylineno);
		strcpy(yylval->value.c,yytext); return TYPE;
		}
{TOKEN_LP} { yylval = createNode(LP_,"LP",yylineno);
			strcpy(yylval->value.c,yytext); return LP;}
{TOKEN_RP} { yylval = createNode(RP_,"RP",yylineno);
			strcpy(yylval->value.c,yytext); return RP;}
{TOKEN_LB} { yylval = createNode(LB_,"LB",yylineno);
			strcpy(yylval->value.c,yytext); return LB;}
{TOKEN_RB} { yylval = createNode(RB_,"RB",yylineno);
			strcpy(yylval->value.c,yytext); return RB;}
{TOKEN_LC} { yylval = createNode(LC_,"LC",yylineno);
			strcpy(yylval->value.c,yytext); return LC;}
{TOKEN_RC} { yylval = createNode(RC_,"RC",yylineno);
			strcpy(yylval->value.c,yytext); return RC;}
{TOKEN_STRUCT} { yylval = createNode(STRUCT_,"STRUCT",yylineno);
			strcpy(yylval->value.c,yytext); return STRUCT;}
{TOKEN_RETURN} { yylval = createNode(RETURN_,"RETURN",yylineno);
			strcpy(yylval->value.c,yytext); return RETURN;}
{TOKEN_IF} { yylval = createNode(IF_,"IF",yylineno); 
			strcpy(yylval->value.c,yytext);return IF;}
{TOKEN_ELSE} { yylval = createNode(ELSE_,"ELSE",yylineno);
			strcpy(yylval->value.c,yytext); return ELSE;}
{TOKEN_WHILE} { yylval = createNode(WHILE_,"WHILE",yylineno);
			strcpy(yylval->value.c,yytext); return WHILE;}
{TOKEN_ID} {//to avoid redefinition.
		//printf("Line %d: (ID, %s)\n",yylineno,yytext);
		yylval = createNode(ID_,yytext,yylineno);
		strcpy(yylval->value.c,yytext); return ID;
	}
\n { lines++; yycolumn = 1;}
[\t\r\n ]+ { /* Do not need to do anything.*/}
. {	printf("Error type A at Line %d: Mysterious characters \'%s\'\n",
		yylineno,yytext);
	total_error += 1;
	}
%%
//#include "syntax.tab.h"
/*
int main(int argc,char** argv)
{
	if(argc>1)
	{
		if(!(yyin = fopen(argv[1], "r")))
		{
			perror(argv[1]);
			return 1;
		}
	}
	yylex();
	printf("%8d\n",lines);
	return 0;
}*/
