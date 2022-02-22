// $Id: lyutils.cpp,v 1.1 2021-12-08 12:07:42-08 - - $

#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
using namespace std;

#include "auxlib.h"
#include "lyutils.h"

lex_util lexer;

void yyerror (astree_ptr, const char* message) {
   lexer.lex_error() << ": " << message << endl;
}

lex_util::~lex_util() {
   yylex_destroy();
}

void lex_util::advance() {
   //if (loc.offset == 0) {
   //   cout << ";" << setw(3) << loc.filenr << ":"
   //        << setw(3) << loc.linenr << ": ";
   //}
   //cout << yytext;
   loc.offset += last_yyleng;
   last_yyleng = yyleng;
}

void lex_util::newline() {
   ++loc.linenr;
   loc.offset = 0;
}


void lex_util::include() {
   size_t linenr;
   size_t filename_len = strlen (yytext) + 1;
   unique_ptr<char[]> filename = make_unique<char[]> (filename_len);
   int scan_rc = sscanf (yytext, "# %zu \"%[^\"]\"",
                         &linenr, filename.get());

   if (scan_rc != 2) {
      lex_error() << "invalid directive, ignored: " << yytext << endl;
   }else {
      if (yy_flex_debug) {
         cerr << "--included # " << linenr << " \"" << filename.get()
              << "\"" << endl;
      }
      token_file << "# " << linenr
                 << " \"" << filename.get()
                 << "\"" << endl;

      loc.linenr = linenr - 1;
      loc.filenr = astree::filenames.size();
      astree::filenames.push_back (filename.get());

      
      index_filename_vector++;
   }
}

int lex_util::token (int symbol) {

   token_file << right << setfill(' ')
              << setw(3) << loc.filenr
              << setw(3) << loc.linenr
              << "." << setfill('0') << setw(3) << loc.offset
              << setfill(' ') << setw(5) << symbol
              << setw(2) << "" << left
              << setw(15) << get_parser_yytname(symbol)
              << yytext << right
              << endl;
   
   yylval = astree::make (symbol, loc, yytext);
   return symbol;
}

int lex_util::badtoken (int symbol) {
   lex_error() << ": invalid token (" << yytext << ")" << endl;
   return token (symbol);
}

void lex_util::badchar (unsigned char bad) {
   ostringstream buffer;
   if (isgraph (bad)) buffer << bad;
                 else buffer << "\\" << setfill('0') << setw(3)
                             << unsigned (bad);
   lex_error() << ": invalid source character ("
           << buffer.str() << ")" << endl;
}

void lex_util::lex_fatal_error (const char* message) {
   throw ::fatal_error (message);
}

ostream& lex_util::lex_error() {
   return exec::error() << loc << ": ";
}

void lex_util::open_token_file(const char* token_filename){
   token_file.open (token_filename, ofstream::out | ofstream::trunc);
   //token_file << "Opened...";
}

void lex_util::close_token_file(){
   //token_file << "...Closing";
   token_file.close();
}

void lex_util::open_ast_file(const char* ast_filename){
   ast_file.open (ast_filename, ofstream::out | ofstream::trunc);
}

void lex_util::close_ast_file(){
   ast_file.close();
}

void lex_util::open_symbols_file(const char* symbols_filename){
   symbols_file.open (symbols_filename, ofstream::out | ofstream::trunc);
}

void lex_util::close_symbols_file(){
   symbols_file.close();
}


parse_util::parse_util (const char* oc_filename,
                        bool parse_debug, bool lex_debug):
                        oc_file (oc_filename, lex_debug) {
   yydebug = parse_debug;
   yy_flex_debug = lex_debug;
   yyin = oc_file.get_pipe();
}

void parse_util::parse() {
   int not_ok = yyparse (astree_root);
   if (not_ok) exec::error() << "parse failed.";
   if (yydebug or yy_flex_debug) {
      astree_root->dump_tree();
      astree::dump_filenames();
   }
}

void parse_util::add_all_symbols() {
   astree_root->add_symbols(astree_root->symbols->local_table);

}
