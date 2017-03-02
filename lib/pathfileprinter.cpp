/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/prettyprinter.hh>
#include <minizinc/pathfileprinter.hh>

#include <minizinc/model.hh>
#include <minizinc/flatten_internal.hh>

#include <sstream>

namespace MiniZinc {

  PathFilePrinter::PathFilePrinter(std::ostream& o, EnvI& envi, bool rem) : os(o), ei(envi), remove_paths(rem), constraint_index(0) {};

  void PathFilePrinter::addBetterName(Id* id, std::string name, std::string path, bool overwrite = false) {
    std::string oname;
    std::string opath;

    NameMap::iterator it = betternames.find(id);
    if(it!=betternames.end()) {
      oname = it->second.first;
      opath = it->second.second;
    }

    if(name != "" && (overwrite || oname == ""))
      oname = name;
    if(path != "" && (overwrite || opath == ""))
      opath = path;

    betternames[id] = NamePair(oname, opath);
  }

  std::string path2name(std::string path, bool ignore_array = true) {
    std::stringstream name;

    int idpos = path.rfind("id:");
    if(idpos > -1) {
      idpos += 3;
      int semi = path.find(";", idpos);
      if(semi > -1) {
        // Variable name
        name << path.substr(idpos, semi-idpos);

        // Check for array
        int dim = 0;
        int ilpos = semi-idpos;
        do {
          ilpos = path.find("il:", ilpos);
          if(ilpos > -1) {
            ilpos += 3;
            semi = path.find(";", ilpos);
            if(semi > -1) {
              if(dim == 0) name << "[";
              else name << ",";
              name << path.substr(ilpos, semi-ilpos);
              dim ++;
            }
          }
        } while(ilpos > -1);

        if(dim > 0) name << "?]";

        // Check for anon
        if(path.find(":anon") != -1 || path.find("=") != -1) {
          name.str("");
          name.clear();
        }
      }
    }

    return name.str();
  }

  void PathFilePrinter::print(Model* m) {
    // Build map
    for(VarDeclIterator vdit = m->begin_vardecls(); vdit != m->end_vardecls(); ++vdit) {
      VarDecl* e = vdit->e();
      for(ExpressionSetIter it = e->ann().begin(); it != e->ann().end(); ++it) {
        if(Call* ca = (*it)->dyn_cast<Call>()) {
          ASTString cid = ca->id();
          if(cid == constants().ann.output_array) {
            if(ArrayLit* rhs = e->e()->dyn_cast<ArrayLit>()) {
              ASTExprVec<Expression> elems = rhs->v();
              for(unsigned int ind=0; ind<elems.size(); ind++) {
                if(Id* id = elems[ind]->dyn_cast<Id>()) {
                  std::stringstream bettername;
                  bettername << *e->id() << "[";

                  // Array of sets
                  ASTExprVec<Expression> dimsets = ca->args()[0]->cast<ArrayLit>()->v();
                  std::vector<IntVal> dims(dimsets.size(), 1);
                  for(unsigned int i=0; i<dimsets.size(); i++) {
                    SetLit* sl = dimsets[i]->cast<SetLit>();
                    dims[i] = sl->isv()->card();
                  }
                  std::vector<IntVal> dimspan(dims.size(), 1);
                  for(unsigned int i=0; i<dims.size(); i++)
                    for(unsigned int j=i+1; j<dims.size(); j++)
                      dimspan[i] *= dims[j];

                  IntVal curind = ind;
                  for(unsigned int i=0; i<dims.size()-1; i++) {
                    IntVal thisind = curind / dimspan[i];
                    curind -= thisind * dimspan[i];
                    bettername << dimsets[i]->cast<SetLit>()->isv()->min() + thisind << ",";
                  }
                  bettername << dimsets[dimsets.size()-1]->cast<SetLit>()->isv()->min() + curind << "]";

                  addBetterName(id, bettername.str(), "", true);
                }
              }
            }
          } else if(ca->id() == constants().ann.mzn_path) {
            StringLit* sl = ca->args()[0]->cast<StringLit>();
            addBetterName(e->id(), path2name(sl->v().str()), sl->v().str());
            //if(remove_paths)
            //  e->ann().removeCall(constants().ann.mzn_path);
          }
        }
      }

    }

    // Print values
    for(Item* item : *m) {
      print(item);
    }

  }

  void PathFilePrinter::print(Item* item) {
    if(VarDeclI* vdi = item->dyn_cast<VarDeclI>()) {
      Id* id = vdi->e()->id();
      NamePair np = betternames[id];
      if(np.first != "" || np.second != "") {
        os << *id << "\t";

        { // Name
          if(np.first == "") {
            os << *id << "\t";
          } else {
            std::string name = np.first;
            os << name;
            if(name.find("?") != -1)
              os <<"(" << *id << ")";
            os << "\t";
          }
        }

        { // Path
          os << np.second << std::endl;
        }
      }
    } else if (ConstraintI* ci = item->dyn_cast<ConstraintI>()) {
      StringLit* sl = NULL;
      Call* e = ci->e()->cast<Call>();
      for(ExpressionSetIter it = e->ann().begin(); it != e->ann().end(); ++it) {
        if(Call* ca = (*it)->dyn_cast<Call>()) {
          ASTString cid = ca->id();
          if(ca->id() == constants().ann.mzn_path) {
            sl = ca->args()[0]->cast<StringLit>();
          }
        }
      }

      {
        os << constraint_index << "\t";
        os << constraint_index << "\t";
        if (sl) {
          os << sl->v();
        } else {
          os << "";
        }
        os << std::endl;
      }
      constraint_index++;
    }
  }
}
