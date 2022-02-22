#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
using namespace std;
#include "symbols.h"

symbols::symbols () {
   fields = new symbol_table();
   local_table = new symbol_table();

}

symbols::~symbols() {
   //delete fields;
   //delete local_table;
   for (symbols_ptr param: parameters){
      erase (param);
   }
}

void symbols::print_attributes (ostream& out) {
   if(attributes.test(attr::VOID))
      out << " void";
   else if(attributes.test(attr::INT))
      out << " int";
   else if(attributes.test(attr::NULLPTR_T))
      out << " nullptr";
   else if(attributes.test(attr::STRING))
      out << " string";
   else if(attributes.test(attr::STRING))
      out << " struct";

   if(attributes.test(attr::ARRAY))
      out << " array";

   if(attributes.test(attr::FUNCTION))
      out << " function";
   else if(attributes.test(attr::VARIABLE))
      out << " variable";
   else if(attributes.test(attr::FIELD))
      out << " field";
   else if(attributes.test(attr::TYPEID))
      out << " typeid";
   else if(attributes.test(attr::PARAM))
      out << " param";
   else if(attributes.test(attr::LOCAL))
      out << " local";

   if(attributes.test(attr::LVAL))
      out << " lval";

   if(attributes.test(attr::CONST))
      out << " const";

   if(attributes.test(attr::VREG))
      out << " vreg";
   else if(attributes.test(attr::VADDR))
      out << " vaddr";

}


void symbols::print_symbols (ostream& out,
                            int depth) {

   for (const auto & [ key, value ] : *fields) {
      for (int i=0; i<depth; ++i) { out << "\t";}
      out << key << " " << *value;
      value->print_attributes(out);
      out << " field " << value->sequence << endl;;
   }

   for (symbols_ptr param : parameters) {
      for (int i=0; i<depth; ++i) { out << "\t";}
      out << *param;
      param->print_attributes(out);
      out << " param " << param->sequence << endl;
   }


   for (const auto & [ key, value ] : *local_table) {
      for (int i=0; i<depth; ++i) { out << "\t";}
      out << key << " " << *value;
      if(depth != 0) out << " local " << value->sequence;
      out << endl;
      value->print_symbols(out,depth+1);
   }
}

symbols_ptr symbols::make () {
   return new symbols ();
}

void symbols::erase (symbols_ptr& symbols) {
   delete symbols->fields;
   delete symbols;
   symbols = nullptr;
}

ostream& operator<< (ostream& out, const symbols& sym) {
   //To-do : Find way to name parameter
   out << "("
       << sym.lloc.filenr
       << "." << sym.lloc.linenr
       << "." << sym.lloc.offset
       << ") "
       << " {" << sym.block_nr << "}";
   return out;
}


// ostream& operator<< (ostream& out, const symbols& symbols) {
//    cout << key << " " << symbols->lloc << " {"
//            << symbols->block_nr << "}"<< endl;
//    tree.symbols->print_attributes(out);
//    if(tree.symbol==TOK_IDENT){
//       out << " "<< tree.symbols->lloc;
//    }

//    return out;
// }
