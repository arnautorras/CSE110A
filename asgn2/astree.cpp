// $Id: astree.cpp,v 1.1 2021-12-08 12:07:42-08 - - $

#include <cassert>
#include <iomanip>
#include <iostream>
#include <string.h>

using namespace std;

#include "astree.h"
#include "lyutils.h"

unordered_set<string> astree::lextable;
vector<string> astree::filenames {"/"};

ostream& operator<< (ostream& out, const location& loc) {
   return out << loc.filenr
              << "." << loc.linenr
              << "." << loc.offset;
}

astree::astree (int symbol_, const location& loc_, const char* info):
                symbol (symbol_), loc (loc_) {
   auto handle = lextable.emplace (info);
   lexinfo = &(*handle.first);
   if (yydebug) cerr << "::astree: " << *this << endl;
}

astree::~astree() {
   if (yydebug) cerr << "::~astree: " << *this << endl;
   for (astree_ptr child: children){
      erase (child);
   }
}

void astree::adopt (astree_ptr child1,
                    astree_ptr child2,
                    astree_ptr child3,
                    astree_ptr child4) {
   if (child1 != nullptr) children.push_back (child1);
   if (child2 != nullptr) children.push_back (child2);
   if (child3 != nullptr) children.push_back (child3);
   if (child4 != nullptr) children.push_back (child4);
}

void astree::adopt_as (astree_ptr child, int symbol_) {
   symbol = symbol_;
   adopt (child);
}

void astree::define_as (int symbol_){
   symbol = symbol_;
}


void astree::dump_tree (int depth) {
   cerr << "::astree: " << setw (depth * 3) << "" << *this << endl;
   for (astree_ptr child: children) child->dump_tree (depth + 1);
}

void astree::print_tree (ostream& out, int depth) {

   for (int i=0; i<depth; ++i) { out << "|\t";}
   out <<  *this << endl;
   for (astree_ptr child: children) child->print_tree (out, depth + 1);
}

void astree::dump_filenames() {
   for (size_t index = 0; index < filenames.size(); ++index) {
      cerr << "::astree::cppfile[" << index << "]: \"" 
           << filenames[index] << "\"" << endl;
   }
}   

void astree::print_symbol_value (astree_ptr tree) {
   if (tree == nullptr) cerr << "nullptr";
                   else cerr << *tree;
}

ostream& operator<< (ostream& out, const astree& tree) {
   const char *tname = get_parser_yytname(tree.symbol);
   if (strstr (tname, "TOK_") == tname) tname += 4;
   out << tname << " \"" << *tree.lexinfo << "\" " << tree.loc;
   return out;
}

astree_ptr astree::make (int symbol, const location& loc,
                         const char* lexinfo) {
   return new astree (symbol, loc, lexinfo);
}


// To-do : ASGN2 : Determine whether this is causing memory issues.
// (Should the tree's children be erased before the tree itself?)
void astree::erase (astree_ptr& tree) {
   delete tree;
   tree = nullptr;
}

