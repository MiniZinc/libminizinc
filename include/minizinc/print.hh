/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_PRINT_HH__
#define __MINIZINC_PRINT_HH__

#include <minizinc/model.hh>
#include <printer/Document.h>
#include <printer/PrettyPrinter.h>

namespace MiniZinc {
  Document* expressionToDocument(const Expression* e);
  void print(std::ostream& os, Model* m);
  void printDoc(std::ostream& os, Model* m);
  class ItemDocumentMapper;

  class ItemDocumentMapper {
  public:
    typedef Document* ret;
    ret mapIncludeI(const IncludeI& ii) {
      std::ostringstream oss;
      oss << "include \"" << ii._f.str() << "\";";
      return new StringDocument(oss.str());
    }
    ret mapVarDeclI(const VarDeclI& vi) {
      DocumentList* dl = new DocumentList("", " ", ";");
      dl->addDocumentToList(expressionToDocument(vi._e));
      return dl;
    }
    ret mapAssignI(const AssignI& ai) {
      DocumentList* dl = new DocumentList("", " = ", ";");
      dl->addStringToList(ai._id.str());
      dl->addDocumentToList(expressionToDocument(ai._e));
      return dl;
    }
    ret mapConstraintI(const ConstraintI& ci) {
      DocumentList* dl = new DocumentList("constraint ", " ", ";");
      dl->addDocumentToList(expressionToDocument(ci._e));
      return dl;
    }
    ret mapSolveI(const SolveI& si) {
      DocumentList* dl = new DocumentList("", "", ";");
      dl->addStringToList("solve");
      if (si._ann)
	dl->addDocumentToList(expressionToDocument(si._ann));
      switch (si._st) {
      case SolveI::ST_SAT:
	dl->addStringToList(" satisfy");
	break;
      case SolveI::ST_MIN:
	dl->addStringToList(" minimize ");
	dl->addDocumentToList(expressionToDocument(si._e));
	break;
      case SolveI::ST_MAX:
	dl->addStringToList(" maximize ");
	dl->addDocumentToList(expressionToDocument(si._e));
	break;
      }
      return dl;
    }
    ret mapOutputI(const OutputI& oi) {
      DocumentList* dl = new DocumentList("output ", " ", ";");
      dl->addDocumentToList(expressionToDocument(oi._e));
      return dl;
    }
    ret mapFunctionI(const FunctionI& fi) {
      DocumentList* dl;
      if (fi._ti->_type.isann() && fi._e == NULL) {
	dl = new DocumentList("annotation ", " ", ";", false);
      } else if (fi._ti->_type == Type::parbool()) {
	dl = new DocumentList("test ", "", ";", false);
      } else if (fi._ti->_type == Type::varbool()) {
	dl = new DocumentList("predicate ", "", ";", false);
      } else {
	dl = new DocumentList("function ", "", ";", false);
	dl->addDocumentToList(expressionToDocument(fi._ti));
	dl->addStringToList(": ");
      }
      dl->addStringToList(fi._id.str());
      if (!fi._params->empty()) {
	DocumentList* params = new DocumentList("(", ", ", ")");
	for (unsigned int i = 0; i < fi._params->size(); i++) {
	  DocumentList* par = new DocumentList("", "", "");
	  par->setUnbreakable(true);
	  par->addDocumentToList(expressionToDocument((*fi._params)[i]));
	  params->addDocumentToList(par);
	}
	dl->addDocumentToList(params);
      }
      if (fi._ann) {
	dl->addDocumentToList(expressionToDocument(fi._ann));
      }
      if (fi._e) {
	dl->addStringToList(" = ");
	dl->addBreakPoint();
	dl->addDocumentToList(expressionToDocument(fi._e));
      }

      return dl;
    }
  };

template<class T>
  class ItemMapper {
  protected:
    T& _t;
  public:
    ItemMapper(T& t) :
      _t(t) {
    }
    typename T::ret map(Item* i) {
      switch (i->_iid) {
      case Item::II_INC:
	return _t.mapIncludeI(*i->cast<IncludeI>());
      case Item::II_VD:
	return _t.mapVarDeclI(*i->cast<VarDeclI>());
      case Item::II_ASN:
	return _t.mapAssignI(*i->cast<AssignI>());
      case Item::II_CON:
	return _t.mapConstraintI(*i->cast<ConstraintI>());
      case Item::II_SOL:
	return _t.mapSolveI(*i->cast<SolveI>());
      case Item::II_OUT:
	return _t.mapOutputI(*i->cast<OutputI>());
      case Item::II_FUN:
	return _t.mapFunctionI(*i->cast<FunctionI>());
      default:
	assert(false);
	break;
      }
      return NULL;
    }
  };
  template<class T> class ItemMapper;
  
}

#endif
