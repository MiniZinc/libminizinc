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
    void print(Expression* e){
      Document* d = expressionToDocument(e);
      print(d);
      delete d;
    }
    void print(Document* d){
      printer->print(d);
      std::cout << *printer << std::endl;
      printer = new PrettyPrinter(80,4,true,true);
    }
    void print(Item* i){
      Document* d = im.map(i);
      print(d);
      delete d;
    }
    void print(Model* m){
      for (unsigned int i = 0; i < m->_items.size(); i++) {
	Document* d = im.map(m->_items[i]);
	print(d);
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
 
};
#endif
