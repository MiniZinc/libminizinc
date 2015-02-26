 /* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:     
 *     Andrea RENDL (andrea.rendl@nicta.com.au)
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/solvers/gecode/gecode_engine.hh>

namespace MiniZinc {
  
  void
  MiniZinc::Path::post(Gecode::Space& home) {
    GECODE_ES_FAIL(Gecode::Search::Meta::NoGoodsProp::post(home,*this));
  }
  
  template<class T>
  void 
  DFSEngine<T>::postConstraints(std::vector<Call*> cts, GecodeSolverInstance& si) {
    // iterate over stack and post constraint
    if(path.empty()) 
      return;    
    for(int edge=0; edge<path.getNbEntries(); edge++) {
      T* s = path.getSpace(edge);
      if(s)
        si.postConstraints(cts); // TODO: make sure the constraints are posted on s!!
    }
  }
  
  template<class T>
  void
  DFSEngine<T>::updateIntBounds(VarDecl* vd, int lb, int ub, GecodeSolverInstance& si) {
    // iterate over stack and post constraint
    if(path.empty()) 
      return;    
    for(int edge=0; edge<path.getNbEntries(); edge++) {
      T* s = path.getSpace(edge);
      if(s) {
        FznSpace* space = static_cast<FznSpace*>(s);
        si.updateIntBounds(space, vd,lb,ub);
      }
    }
  }
  
  /*void DFSEngine::addVariable(VarDecl* vd, GecodeSolverInstance& si) {
    if(path.empty())
      return;
    for(int edge=0; edge<path.getNbEntries(); edge++) {
      Gecode::Space* s = path.getSpace(edge);
      if(s)
        si.addVariable;
    }
  }*/
}
