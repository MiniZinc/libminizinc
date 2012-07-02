#ifndef __MINIZINC_PRINT_HH__
#define __MINIZINC_PRINT_HH__

#include <minizinc/model.hh>
#include <printer/document/Document.h>
#include <printer/printer/PrettyPrinter.h>

namespace MiniZinc {
  
void print(std::ostream& os, Model* m);
void printDoc(std::ostream& os, Model* m);
  
}

#endif
