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

int astree::next_block = 1; 

ostream& operator<< (ostream& out, const location& loc) {
   return out << "("
              << loc.filenr
              << "." << loc.linenr
              << "." << loc.offset
              << ")";
}

astree::astree (int symbol_,
                const location& loc_,
                const char* info):
                     symbol (symbol_),
                     loc (loc_),
                     symbols(symbols::make()){
   auto handle = lextable.emplace (info);
   lexinfo = &(*handle.first);
   if (yydebug) cerr << "::astree: " << *this << endl;
   symbols->attributes = 0;
   symbols->block_nr = 0;
   //symbols->symbol_table = nullptr; To-do : uncomment?

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
/*
void astree::print_symbols (ostream& out,
                            int depth,
                            int param_or_local) {

   for (int i=0; i<depth; ++i) { out << "\t";}

   for (const auto & [ key, value ] : symbols) {
      if (param_or_local == 1)
      else if (param_or_local == 2)

      for (symbols_ptr field : value->fields) {
         for (int i=0; i<depth+1; ++i) { out << "\t";}
      }
      value->parameters->print_symbols(out,depth+1,1);
      value->local_table->print_symbols(out,depth+1,2);
   }
}
*/

attr astree::match_symbol_to_attribute(){
   switch(symbol){
      case TOK_INT: 
         return attr::INT;
      case TOK_PTR: 
         return attr::STRUCT;
      case TOK_STRING: 
         return attr::STRING;
      case TOK_ARRAY: 
         return attr::ARRAY;
   }
   // To-do : fix
   return attr::VOID;
}



void astree::add_symbols(symbol_table* curr_symtable, int g_block_nr){
   symbols->block_nr = g_block_nr;
   switch(symbol){
      case TOK_ROOT :
         symbols->block_nr = 0;
         for (astree_ptr child: children)
         {
            child->add_symbols(curr_symtable,symbols->block_nr);
         }
         break;
      case TOK_FUNCT :
         symbols->block_nr = next_block;
         next_block++;
         for (astree_ptr child: children)
         {
            child->symbols->block_nr = symbols->block_nr;
            switch(child->symbol){
               case TOK_TYPE_ID :
                  for (astree_ptr gchild: child->children)
                  {
                     gchild->symbols->block_nr = symbols->block_nr;
                     switch(gchild->symbol){
                        case TOK_IDENT :
                           (*curr_symtable)[*(gchild->lexinfo)]
                              = symbols;
                           break;
                        default :
                           symbols->attributes.set(
                              gchild->match_symbol_to_attribute());
                     }
                  }
                  child->add_symbols(symbols->local_table,symbols->block_nr);
                  break;
               case TOK_PARAM :
                  //To-do : add param sequence count
                  for (astree_ptr gchild: child->children)
                  {
                     gchild->symbols->block_nr = symbols->block_nr;
                     gchild->add_symbols(symbols->local_table,
                                         child->symbols->block_nr);
                     gchild->symbols->attributes.set(attr::PARAM);
                     symbols->parameters.push_back(gchild->symbols);
                  }
                  break;
               case TOK_BLOCK :
                  //To-do : add local sequence count
                  for (astree_ptr gchild: child->children)
                  {
                     gchild->symbols->block_nr = symbols->block_nr;
                     gchild->add_symbols(symbols->local_table,
                                         child->symbols->block_nr);
                     gchild->symbols->attributes.set(attr::LOCAL);
                  }
                  break;

            }
         }
         break; 
      case TOK_STRUCT :
         symbols->attributes.set(attr::STRUCT);
         symbols->lloc = loc;
         for (astree_ptr child: children)
         {
            switch(child->symbol){
               case TOK_IDENT :
                  (*curr_symtable)[*(child->lexinfo)] = symbols;
                  break;
               case TOK_TYPE_ID :
                  child->add_symbols(symbols->fields,
                                     symbols->block_nr);
                  break;
            }
         }
         break;
      case TOK_TYPE_ID :
         for (astree_ptr child: children){
            child->add_symbols(curr_symtable,symbols->block_nr);
            switch(child->symbol){
               case TOK_IDENT :
                  if(curr_symtable != nullptr){
                     (*curr_symtable)[*(child->lexinfo)] = symbols;
                  }
                  break;
               //To-do : finish implementing ?
               default :
                  symbols->attributes.set(
                     child->match_symbol_to_attribute());
               
            }
         }
         break;

      default : 
         symbols->block_nr = g_block_nr;
         for (astree_ptr child: children){
            child->add_symbols(curr_symtable,symbols->block_nr);
         }
   }
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
   out << tname << " \"" << *tree.lexinfo << "\" "
       << tree.loc  << " {"
       << tree.symbols->block_nr << "}";
   tree.symbols->print_attributes(out);
   if(tree.symbol==TOK_IDENT){
      out << " " << tree.symbols->lloc;
   }

   return out;
}

// ostream& operator<< (ostream& out, const symbols& symbols) {
//            << symbols->block_nr << "}"<< endl;
//   //tree.symbols->print_attributes(out);
//    if(tree.symbol==TOK_IDENT){
//       out << " "<< tree.symbols->lloc;
//    }

//    return out;
// }

astree_ptr astree::make (int symbol, const location& loc,
                         const char* lexinfo) {
   return new astree (symbol, loc, lexinfo);
}


// To-do : ASGN2 : Determine whether this is causing memory issues.
// (Should the tree's children be erased before the tree itself?)
void astree::erase (astree_ptr& tree) {
   delete tree->symbols;
   delete tree;
   tree = nullptr;
}

