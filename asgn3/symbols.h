// $Id: lyutils.h,v 1.3 2021-12-20 13:07:53-08 - - $

#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <iostream>
#include <unordered_map>
#include <string>
#include <unordered_set>
#include <vector>
#include <bitset>

using namespace std;

enum attr {
   VOID,
   INT,
   NULLPTR_T,
   STRING,
   STRUCT,
   ARRAY, 
   FUNCTION,
   VARIABLE,
   FIELD,
   TYPEID,
   PARAM,
   LOCAL,
   LVAL,
   CONST,
   VREG,  
   VADDR,
   BITSET_SIZE
};

using attr_bitset = bitset<attr::BITSET_SIZE>;

using symbol_table = unordered_map<string, struct symbols*>;

using symbol_entry = symbol_table::value_type;

using symbols_ptr = struct symbols*;


struct location {
   size_t filenr = 0;
   size_t linenr = 0;
   size_t offset = 0;
};

class symbols {
   private:
      symbols();
   public:

      attr_bitset attributes;                         /* Symbol attributes */

      size_t sequence;                                /* For parameters and local variables,
                                                         their declaration sequence number,
                                                         starting from 0, 1, etc. For all
                                                         global names, 0.*/

      symbol_table* fields;                           /* A pointer to the field table if this symbol
                                                         is a struct (if the typeid attribute is present).
                                                         Else nullptr. */

      symbol_table* local_table;                      /* A pointer to the field table if this symbol
                                                         is a function */

      location lloc;                                  /* Location where this symbol was declared.*/

      size_t block_nr;                                /* The block number to which this symbol belongs.
                                                         Block 0 is the global block, and positive increasing
                                                         sequential numbers are assigned to functions. */

      vector<symbols_ptr> parameters;                 /* A symbol node pointer which points at the parameter list.
                                                         For a function, points at a vector of parameters.
                                                         Else nullptr. */

      static void erase (symbols_ptr&);
      symbols (const symbols&) = delete;
      symbols& operator= (const symbols&) = delete;
      ~symbols();
      void print_attributes (ostream& out);
      void print_symbols (ostream& out,int depth = 0);
      static symbols_ptr make ();
};

ostream& operator<< (ostream&, const symbols&);

#endif

