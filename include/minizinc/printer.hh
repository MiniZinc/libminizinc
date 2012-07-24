#ifndef __PRINTER_H
#define __PRINTER_H

#include <minizinc/print.hh>
#include <minizinc/ast.hh>

namespace MiniZinc {
  class Printer {

  private:

    ItemDocumentMapper ism;
    ItemMapper<ItemDocumentMapper> im;
    PrettyPrinter* printer;
    
    Printer () : im(ism) {
      printer =  new PrettyPrinter(80, 4, true, true);
    }
    ~Printer () {
      delete printer;
    }

  public:
    static Printer* _singleton;
    static Printer* getInstance (){
      if (NULL == _singleton){
	_singleton =  new Printer();
      }
      return _singleton;
    }
    
    void print(Expression* e, std::ostream& os = std::cout){
      Document* d = expressionToDocument(e);
      print(d,os);
      delete d;
    }
    void print(Document* d, std::ostream& os = std::cout){
      printer->print(d);
      os << *printer;
      printer = new PrettyPrinter(80,4,true,true);
    }
    void print(Item* i, std::ostream& os = std::cout){
      Document* d = im.map(i);
      print(d,os);
      delete d;
    }
    void print(Model* m, std::ostream& os = std::cout){
      for (unsigned int i = 0; i < m->_items.size(); i++) {
	Document* d = im.map(m->_items[i]);
	print(d,os);
	delete d;
      }
    }
    static void kill ()
    {
      if (NULL != _singleton)
	{
	  delete _singleton;
	  _singleton = NULL;
	}
    } 
  };
 

  inline
  std::ostream& operator<<(std::ostream& os, Expression* e){
    Printer::getInstance()->print(e,os);
    return os;
  }
  inline
  std::ostream& operator<<(std::ostream& os, Document* e){
    Printer::getInstance()->print(e,os);
    return os;
  }
  inline
  std::ostream& operator<<(std::ostream& os, Item* e){
    Printer::getInstance()->print(e,os);
    return os;
  }
  inline
  std::ostream& operator<<(std::ostream& os, Model* e){
    Printer::getInstance()->print(e,os);
    return os;
  }
};
#endif
