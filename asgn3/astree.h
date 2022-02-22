#ifndef ASTREE_H
#define ASTREE_H

#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

#include "auxlib.h"
#include "symbols.h"

using astree_ptr = struct astree*;

ostream& operator<< (ostream&, const location&);

class astree {
   private:
      attr match_symbol_to_attribute();

      astree (int symbol, const location&, const char* lexinfo);
      static unordered_set<string> lextable;
      static int next_block;

   public:
      int symbol;                  // token code
      location loc;                // source location
      symbols_ptr symbols;
      const string* lexinfo;       // pointer to lexical information
      vector<astree_ptr> children; // children of this n-way node
      static vector<string> filenames;

      astree (const astree&) = delete;
      astree& operator= (const astree&) = delete;
      ~astree();

      static void dump_filenames();
      static void print_symbol_value (astree_ptr);
      static astree_ptr make (int symbol, const location&,
                              const char* lexinfo);
      static void erase (astree_ptr&);

   
      void adopt (astree_ptr child1,
                  astree_ptr child2 = nullptr,
                  astree_ptr child3 = nullptr,
                  astree_ptr child4 = nullptr);
      void adopt_as (astree_ptr child, int symbol);
      void define_as (int symbol);
      void dump_tree (int depth = 0);
      void print_tree (ostream& out, int depth = 0);
      void print_symbols (ostream& out);


      void add_symbols(symbol_table* curr_symtable, int g_block_nr = 0);

};

ostream& operator<< (ostream&, const astree&);
// ostream& operator<< (ostream&, const symbols&);

#endif

