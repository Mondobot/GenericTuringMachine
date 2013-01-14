%{
#include <stdio.h>
#include <iostream>

#include "aux.h"
#include "elementary.h"
#include "complex.h"
#include "generic.h"
#include "parser.h"

%}

newline \n
os [ ]*
letter [a-zA-Z]
digit [0-9]
other [#$*@]

symbol {letter}|{digit}|{other}
name ({letter}|{digit}|_)*
elem {symbol}|<{name}>|&{name}
elementary_1 \[L\]|\[R\]|\[{elem}\]
elementary_2 \[L\({elem}")]"|\[R\({elem}")]"|\[L\(!{elem}")]"|\[R\(!{elem}")]"
elementary {elementary_1}|{elementary_2}

%option stack
%x	ALFABET
%x	SYM_DECL
%x	SET_DECL
%x	MT_DECL
%x	TRANS
%x	SET
%x	TRANS_BODY


%%
<INITIAL>;.*$				{;// Din pacate nu prea am ce sa explic la parsare
						  // sper a numele functiilor sunt cat de cat sugestive :) 
						  }

<INITIAL>"alphabet"{os}"::"		BEGIN(ALFABET);
<ALFABET>{os}|,|{newline}		{;}
<ALFABET>{symbol}				{ Parser::add_to_alfabet(yytext); }
<ALFABET>;$					{ BEGIN(INITIAL);}

<INITIAL>{name}{os}=			{ BEGIN(SYM_DECL);
							{ Parser::add_symbol(yytext, SYM_NAME); }}
<SYM_DECL>{os}|newline			{;}
<SYM_DECL>{symbol}				{ Parser::add_symbol(yytext, SYM_VAL); }
<SYM_DECL>;$					{ BEGIN(INITIAL); }

<INITIAL>{name}{os}:=			{ {Parser::add_set(yytext, NEW_SET); }
							BEGIN(SET_DECL);}
<SET_DECL>[{,]|{os}|newline		{;}
<SET_DECL>{symbol}				{ Parser::add_set(yytext, NEW_VAL_TO_SET); }
<SET_DECL>\}{os};$				BEGIN(INITIAL);

<INITIAL>{name}{os}::=			{ BEGIN(MT_DECL);
							{ Parser::NewTM(yytext);}}

<MT_DECL,TRANS_BODY>{name}@			{ Parser::AddNoJumpLabel(yytext);}
<MT_DECL,TRANS_BODY>&{name}			{ Parser::AddJumpLabel(yytext);}
<MT_DECL,TRANS_BODY>{elementary}		{ Parser::AddElementaryTM(yytext);}
<MT_DECL,TRANS_BODY>\[{name}\]		{ Parser::AddGeneralTM(yytext);}
<MT_DECL>;;$						{ BEGIN(INITIAL);
								Parser::EndNewTM(); }
<MT_DECL,TRANS_BODY>\(				{ yy_push_state(TRANS);
								Parser::NewMultiTrans(); }

<TRANS>{name}@				{ Parser::WillStoreVar();}
<TRANS>!					{ Parser::EnableNegMatch();}
<TRANS><{name}>{os}->		{ BEGIN(TRANS_BODY);
						// Deja nu mai am idei pentru nume de functii :|
						Parser::AddTransWithSetFromVar(yytext); }
<TRANS>\{					BEGIN(SET);
<TRANS>\)					{yy_pop_state();
						Parser::EndNewMultiTrans();}

<SET>{os}|,|{newline}		{;}
<SET>{elem}|{name}			{ Parser::AddElemToMatchSet(yytext);}
<SET>\}{os}->{os}			{ BEGIN(TRANS_BODY);
						{Parser::AddTransWithSet();}}
<TRANS_BODY>;$				{ BEGIN(TRANS);
						Parser::EndNewTM();}

<INITIAL,MT_DECL,SET_DECL,TRANS_BODY,SET,TRANS,ALFABET>\n|{os}|\t				{;}
%%


int main(int argc, char *argv[]) {
	yyin = fopen(argv[1], "r");
	yylex();

	Parser::Run(std::string(argv[2]), argv[3]);

	return 0;
}
