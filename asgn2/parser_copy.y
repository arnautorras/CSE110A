%{
// $Id: parser.y,v 1.4 2022-01-11 22:46:28-08 - - $

#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wfree-nonheap-object"

#include "lyutils.h"
#include "astree.h"

#define ERASE(PTR) { delete (PTR); }

#define NEW_TOKEN(TMP,TOKEN,LOC,LEX)\
   {\
      location new_loc {.filenr=LOC.filenr, \
                        .linenr=LOC.linenr, \
                        .offset=LOC.offset};\
      TMP = astree::make(TOKEN,new_loc,LEX);\
   }

void show (astree_ptr ptr) {
   if (yydebug && ptr) ptr->print_tree (cerr);
}


%}

%parse-param {YYSTYPE astree_root}

%debug
%defines
%error-verbose
%token-table
%verbose

/* %destructor { ERASE($$); } <> --- Causing memory errors ---*/
%printer { astree::print_symbol_value ($$); } <>

%token TOK_VOID TOK_INT TOK_STRING
%token TOK_IF TOK_ELSE TOK_WHILE TOK_RETURN TOK_STRUCT
%token TOK_NULLPTR TOK_ARRAY TOK_ARROW TOK_ALLOC TOK_PTR
%token TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE TOK_NOT
%token TOK_IDENT TOK_INTCON TOK_CHARCON TOK_STRINGCON
%token TOK_ROOT TOK_BLOCK TOK_CALL TOK_FUNCT TOK_TYPE_ID
%token TOK_POS TOK_NEG

%right  TOK_IF TOK_ELSE
%right  '='
%right  TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE 
%left   '+' '-'
%left   '*' '/' '%'
%right  '^'
%right  TOK_POS TOK_NEG
%right  TOK_ARROW
%start  program

%%

program     : program structdef  { $1->adopt ($2); $$ = $1; }
            | program function   { $1->adopt ($2); $$ = $1; }
            | program statement  { $1->adopt ($2); $$ = $1; }
            | program error '}'   
                                 {
                                    ERASE ($3);
                                    $1->adopt($2);
                                    $$ = $1;
                                 }
            | program error ';'
                                 {
                                    ERASE ($3);
                                    $1->adopt($2);
                                    $$ = $1;
                                 }
            | %empty             { $$ = astree_root; }
            ;

function    : returntype TOK_IDENT parameters block
                  {
                     astree_ptr tmp;
                     NEW_TOKEN(tmp,TOK_FUNCT,$1->loc,"");
                     tmp->adopt($1,$2,$3,$4);
                     $$ = tmp;
                  }
            ;

vardecl     : muldecl ';'  { ERASE($2); $$ = $1; }
            ;

muldecl     : muldecl '=' expr
                  {
                     $2->adopt($1,$3);
                     $$ = $2;
                  }
            | type TOK_IDENT
                  {
                     astree_ptr tmp;
                     NEW_TOKEN(tmp,TOK_TYPE_ID,$1->loc,"TYPE_ID");
                     tmp->adopt($1,$2);
                     $$ = tmp;
                  }
            ;

returntype  : plaintype          { $$ = $1; }
            | TOK_VOID           { $$ = $1; }
            ;

structdef  : structhead '}' ';'  { ERASE($3); ERASE ($2); $$ = $1;}
            ;

structhead  : structhead type TOK_IDENT ';'
                           {
                              ERASE ($3);
                              $1->adopt($2);
                              $$ = $1;
                           }
            | TOK_STRUCT TOK_IDENT '{'
                           {
                              $1->adopt($2,$3);
                              $$ = $1;
                           }
            ;


statement   : vardecl      { $$ = $1; }
            | block        { $$ = $1; }
            | while        { $$ = $1; }
            | ifelse       { $$ = $1; }
            | return       { $$ = $1; }
            | expr ';'     { ERASE ($2); $$ = $1; }
            ;

while       : TOK_WHILE '(' expr ')' statement
                           {
                              ERASE ($2); ERASE ($4);
                              $1->adopt($3,$5);
                              $$ = $1;
                           }
            ;


ifelse      : ifstmt elsestmt               { $1->adopt($2); $$ = $1; }
            | ifstmt %prec elsestmt         { $$ = $1; }
            | ifstmt                        { $$ = $1; }
            ;

ifstmt      : TOK_IF '(' expr ')' statement
                           {
                              ERASE ($2); ERASE ($4);
                              $1->adopt($3,$5);
                              $$ = $1;
                           }
            ;

elsestmt    : TOK_ELSE statement    { $1->adopt($2); $$ = $1; }
            ;

block       : blockhead '}'         { ERASE ($2); $$ = $1;}
            | ';'                               { $$ = $1; }
            ;

blockhead   : blockhead statement
                           {
                              $1->adopt_as($2,TOK_BLOCK);
                              $$ = $1; 
                           }
            | '{'          { $$ = $1; }
            ;

parameters  : paramhead ')'         { ERASE ($2); $$ = $1; }
            | '(' ')'               { ERASE ($2); $$ = $1; }
            ;
paramhead   : paramhead ',' type TOK_IDENT
                           {
                              ERASE ($2);
                              $1->adopt($3,$4);
                              $$ = $1;
                           }
            | '(' type TOK_IDENT
                           {
                              $1->adopt($2);
                              $$ = $1;
                           }
            ;


type        : plaintype    { $$ = $1; }
            | array        { $$ = $1; }
            ;

array       : TOK_ARRAY '<' plaintype '>'
                           {
                              ERASE ($2); ERASE ($4);
                              $1->adopt($3);
                              $$ = $1;
                           }
            ;

plaintype   : TOK_INT                              { $$ = $1; }
            | TOK_STRING                           { $$ = $1; }
            | TOK_PTR '<' TOK_STRUCT TOK_IDENT '>'
                           {
                              ERASE ($2); ERASE ($3); ERASE ($5);
                              $1->adopt($4);
                              $$ = $1;
                           }
            ;

expr        : expr '+' expr            { $2->adopt($1,$3); $$ = $2; }
            | expr '-' expr            { $2->adopt($1,$3); $$ = $2; }
            | expr '*' expr            { $2->adopt($1,$3); $$ = $2; }
            | expr '/' expr            { $2->adopt($1,$3); $$ = $2; }
            | '+' expr %prec TOK_POS
                                       {
                                          $2->adopt_as($1,TOK_POS);
                                          $$ = $1;
                                       }  
            | '-' expr %prec TOK_NEG   
                                       {
                                          $2->adopt_as($1,TOK_NEG);
                                          $$ = $1;
                                       } 
            | TOK_NOT expr             { $2->adopt($1); $$ = $1; }
            | expr TOK_EQ expr         { $2->adopt($1,$3); $$ = $2; }
            | expr TOK_NE expr         { $2->adopt($1,$3); $$ = $2; }
            | expr TOK_LT expr         { $2->adopt($1,$3); $$ = $2; }
            | expr TOK_LE expr         { $2->adopt($1,$3); $$ = $2; }
            | expr TOK_GT expr         { $2->adopt($1,$3); $$ = $2; }
            | expr TOK_GE expr         { $2->adopt($1,$3); $$ = $2; }
            | allocator                { $$ = $1; }
            | call                     { $$ = $1; }
            | '(' expr ')'             
                                       { 
                                          ERASE ($1); ERASE ($3);
                                          $$ = $2;
                                       }
            | variable                 { $$ = $1; }
            | constant                 { $$ = $1; }
            ;

allocator   : TOK_ALLOC '<' TOK_STRING '>' '(' expr ')'
                           {  
                              ERASE ($2); ERASE ($4);
                              ERASE ($5); ERASE ($7);
                              $1->adopt($3,$6);
                              $$ = $1;
                           }
            | TOK_ALLOC '<' TOK_STRUCT TOK_IDENT '>' '(' ')'
                           {  
                              ERASE ($2); ERASE ($3);
                              ERASE ($5); ERASE ($6);
                              ERASE ($7);
                              $1->adopt($4);
                              $$ = $1;
                           }
            | TOK_ALLOC '<' array '>' '(' expr ')'
                           {  
                              ERASE ($2); ERASE ($4);
                              ERASE ($5); ERASE ($7);
                              $1->adopt($3,$6);
                              $$ = $1;
                           }
            ;

call        : callhead ')'
                           { ERASE ($2); $$ = $1; }
            | TOK_IDENT '(' ')'
                           {
                              ERASE ($3);
                              $2->adopt_as($1,TOK_CALL);
                              $$ = $2;
                           }
            ;

callhead    : callhead ',' expr
                           {
                              ERASE ($2);
                              $1->adopt($3);
                              $$ = $1;
                           }
            | TOK_IDENT '(' expr
                           {
                              $2->adopt_as($1,TOK_CALL);
                              $2->adopt($3); $$ = $2;
                           }
            ;

variable    : TOK_IDENT    { $$ = $1; }
            | expr '[' expr ']'           
                           {
                              ERASE ($4);
                              $2->adopt($1,$3);
                              $$ = $2;
                           }
            | expr TOK_ARROW TOK_IDENT
                           {
                              $1->adopt($2,$3);
                              $$ = $1;
                           }
            ;

constant    : TOK_INTCON      { $$ = $1; }
            | TOK_CHARCON     { $$ = $1; }
            | TOK_STRINGCON   { $$ = $1; }
            | TOK_NULLPTR     { $$ = $1; }
            ;

return      : TOK_RETURN expr ';'
                              {
                                 ERASE ($3);
                                 $1->adopt($2);
                                 $$ = $1;
                              }
            | TOK_RETURN ';'  {
                                 ERASE ($2);
                                 $$ = $1;
                              }
            ;



%%


const char* get_parser_yytname (int symbol) {
   return yytname [YYTRANSLATE (symbol)];
}


