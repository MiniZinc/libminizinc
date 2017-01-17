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

  PathFilePrinter::PathFilePrinter(std::ostream& o, EnvI& envi, bool rem) : os(o), ei(envi), remove_paths(rem) {};

  void PathFilePrinter::print(Model* m) {
    for(Item* item : *m) {
      print(item);
    }

  }

  void PathFilePrinter::print(Item* item) {
    if(VarDeclI* vdi = item->dyn_cast<VarDeclI>()) {
      VarDecl* e = vdi->e();
      for(ExpressionSetIter it = e->ann().begin(); it != e->ann().end(); ++it) {
        if(Call* ca = (*it)->dyn_cast<Call>()) {
          if(ca->id() == constants().ann.mzn_path) {
            StringLit* sl = ca->args()[0]->cast<StringLit>();
            os << *vdi->e()->id() << "\t" << *sl << "\n";
            //if(remove_paths)
            //  e->ann().removeCall(constants().ann.mzn_path);
            return;
          }
        }
      }
    }
  }


}
