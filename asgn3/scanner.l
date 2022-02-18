/* $Id: scanner.l,v 1.3 2021-12-08 12:23:51-08 - - $ */

%{

#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wunused-function"

#include "lyutils.h"

#define YY_USER_ACTION  { lexer.advance(); }
#define YY_FATAL_ERROR(msg) { lexer.lex_fatal_error (msg); }

%}

%option 8bit
%option debug
%option nobackup
%option nodefault
%option noinput
%option nounput
%option noyywrap
%option warn



DIGIT           [0-9]
HEXDIGIT        [a-fA-F0-9]
INTEGER         ({DIGIT}+|0x{HEXDIGIT}+)
MANTISSA        ({DIGIT}+\.?{DIGIT}*|\.{DIGIT}+)
EXPONENT        ([Ee][+-]?{DIGIT}+)
NUMBER          ({MANTISSA}{EXPONENT}?)
LETTER          [a-zA-Z_]
IDENT           (({LETTER}|_)({LETTER}|{DIGIT}|_)*)
CHAR            ('([^\\'\n]|\\[\\'"0nt])')
STRING          (\"([^\\"\n]|\\[\\'"0nt])*\")

BADOCTAL        (0[8-9A-Fa-f]+)

BADNUMBER       ({MANTISSA}[Ee][+-]?)
BADIDENT        ([0-9_a-zA-Z]+)



%%

^"#".*\n        { lexer.include(); lexer.newline(); }
[ \t]+          { }
\n              { lexer.newline(); }

"+"             { return lexer.token ('+'); }
"-"             { return lexer.token ('-'); }
"*"             { return lexer.token ('*'); }
"/"             { return lexer.token ('/'); }
"%"             { return lexer.token ('%'); }
"="             { return lexer.token ('='); }
","             { return lexer.token (','); }
";"             { return lexer.token (';'); }




"("             { return lexer.token ('('); }
")"             { return lexer.token (')'); }
"["             { return lexer.token ('['); }
"]"             { return lexer.token (']'); }
"{"             { return lexer.token ('{'); }
"}"             { return lexer.token ('}'); }

"=="            { return lexer.token (TOK_EQ); }
"!="            { return lexer.token (TOK_NE); }
"<"             { return lexer.token (TOK_LT); }
"<="            { return lexer.token (TOK_LE); }
">"             { return lexer.token (TOK_GT); }
">="            { return lexer.token (TOK_GE); }
"->"            { return lexer.token (TOK_ARROW); }

"not"           { return lexer.token (TOK_NOT); }
"ptr"           { return lexer.token (TOK_PTR); }
"void"          { return lexer.token (TOK_VOID); }
"int"           { return lexer.token (TOK_INT); }
"string"        { return lexer.token (TOK_STRING); }
"if"            { return lexer.token (TOK_IF); }
"else"          { return lexer.token (TOK_ELSE); }
"while"         { return lexer.token (TOK_WHILE); }
"return"        { return lexer.token (TOK_RETURN); }
"struct"        { return lexer.token (TOK_STRUCT); }
"nullptr"       { return lexer.token (TOK_NULLPTR); }
"array"         { return lexer.token (TOK_ARRAY); }
"alloc"         { return lexer.token (TOK_ALLOC); }

{BADOCTAL}      { return lexer.badtoken (TOK_INTCON); }

{CHAR}          { return lexer.token (TOK_CHARCON); }
{STRING}        { return lexer.token (TOK_STRINGCON); }
{INTEGER}       { return lexer.token (TOK_INTCON); }
{IDENT}         { return lexer.token (TOK_IDENT); }

{BADNUMBER}     { return lexer.badtoken (TOK_INTCON); }
{BADIDENT}      { return lexer.badtoken (TOK_IDENT); }


.               { lexer.badchar (*yytext); }

%%

