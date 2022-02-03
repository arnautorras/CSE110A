// $Id: main.cpp,v 1.1 2021-12-08 12:07:42-08 - - $

#include <cstring>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include <libgen.h>
#include <unistd.h>

#include "astree.h"
#include "auxlib.h"
#include "debug.h"
#include "lyutils.h"

struct usage_error: exception {
   usage_error() {
      exec::status (EXIT_FAILURE);
   }
};

struct options {
   int lex_debug {0};
   int parse_debug {0};
   int string_defines {0}; //

   const char* oc_filename {nullptr};
   const char* token_filename {nullptr};
   options (int argc, char** argv);
};

options::options (int argc, char** argv) {
   opterr = 0;
   for(;;) {
      int opt = getopt (argc, argv, "@:d:ly");
      if (opt == EOF) break;
      switch (opt) {
         case '@': debugflags::setflags (optarg); break;
         case 'l': lex_debug = 1; break;
         case 'y': parse_debug = 1; break;
         case 'd': string_defines = 1; break; // To-do : (Asgn2)
         default: exec::error() << "invalid option ("
                        << char (optopt) << ")" << endl;
      }
   }
   if (optind + 1 < argc) throw usage_error();
   if(optind == argc){
      oc_filename = "-";
      token_filename = "-";
   }else{
      oc_filename = argv[optind];
      int oc_filename_len = strlen(oc_filename);

      if(strcmp(&oc_filename[oc_filename_len-3],".oc")!=0){
         exec::error() << "invalid file type ("
                        << &oc_filename[oc_filename_len-3]
                        << ")" << endl;
      }
      char *strArray = new char[oc_filename_len+5];
      strcpy(strArray,oc_filename);
      strcpy(&strArray[oc_filename_len-3],".tokens");
      strArray[oc_filename_len+4] = '\0';
      token_filename = strArray;
   }
}


int main (int argc, char** argv) {
   ios_base::sync_with_stdio (true);
   exec::name (argv[0]);
   try {
      options opts (argc, argv);      
      lexer.open(opts.token_filename);
      parse_util parser (opts.oc_filename,
                 opts.parse_debug, opts.lex_debug);
      // To-do :Inclide string_degines and argument
      //to pass it to cpp_pipe (asgn2)
      parser.parse();
      // To-do : On asgn3, add code to handle symbol table
      lexer.close();
   }catch (usage_error&) {
      cerr << "Usage: " << exec::name() << " [-lyd@] [program]" << endl;
   }catch (fatal_error& reason) {
      exec::error() << reason.what() << endl;
   }



   return exec::status();
}

