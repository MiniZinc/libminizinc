  /* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:     
 *     Andrea RENDL (andrea.rendl@nicta.com.au)
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
// TODO: add Gecode disclaimer

#ifndef __GECODE_ENGINE__
#define __GECODE_ENGINE__

#include <gecode/search.hh>
#include <gecode/search/sequential/path.hh>
#include <gecode/search/support.hh>
#include <gecode/search/worker.hh>
#include <gecode/support.hh>

#include <minizinc/solvers/gecode_solverinstance.hh>
#include <minizinc/ast.hh>
  
   
namespace MiniZinc {  
   
  // forward declaration to tackle circular declaration through gecode_interface.hh
  class GecodeInterface;
  
  /// subclass of actual path to access the iterative stack
  class Path : public Gecode::Search::Sequential::Path {
  public:
    /// path constructor
    Path(int l) : Gecode::Search::Sequential::Path(l) {} 
    /// get the space at edge \a i in the edge stack
    Gecode::Space* getSpace(int i) { assert(i >=0 && i < ds.entries()); return ds[i].space(); }
    /// get the number of entries in the edge stack
    int getNbEntries(void) { return ds.entries(); }
  };
  
  
  /// iterative DFS class that allows to add constraints along the path      
  class DFSEngine  : public Gecode::Search::Worker {
  private:
    /// Search options
    Gecode::Search::Options opt;    
    /// Current space being explored
    Gecode::Space* cur;
    /// Distance until next clone
    unsigned int d;
  protected:
    /// Current path in search tree
    MiniZinc::Path path;
  public:
    /// Initialize for space \a s with options \a o
    DFSEngine(Gecode::Space* s, const Gecode::Search::Options& o);
    /// %Search for next solution
    Gecode::Space* next(void);
    /// Return statistics
    Statistics statistics(void) const;
    /// Reset engine to restart at space \a s
    void reset(Gecode::Space* s);
    /// Return no-goods
    Gecode::NoGoods& nogoods(void);
    /// post constraints to the search engine
    void postConstraints(std::vector<Call*> cts, GecodeSolverInstance& si);
    /// adds variable to the engine
    // void addVariable(VarDecl* vd, GecodeSolverInstance& si);
    /// update the integer bounds of the given variable to the tighter bounds (lb..ub)
    void updateIntBounds(VarDecl* vd, int lb, int ub, GecodeSolverInstance& si);
    /// Destructor
    ~DFSEngine(void);
  };

  forceinline 
  DFSEngine::DFSEngine(Gecode::Space* s, const Gecode::Search::Options& o)
    : opt(o), path(static_cast<int>(opt.nogoods_limit)), d(0) {
    if ((s == NULL) || (s->status(*this) == Gecode::SS_FAILED)) {
      fail++;
      cur = NULL;
      if (!opt.clone)
        delete s;
    } else {
      cur = snapshot(s,opt);
    }
  }

  forceinline void
  DFSEngine::reset(Gecode::Space* s) {
    delete cur;
    this->path.reset();
    d = 0;
    if ((s == NULL) || (s->status(*this) == Gecode::SS_FAILED)) {
      cur = NULL;
    } else {
      cur = s;
    }
    Worker::reset();
  }

  forceinline Gecode::NoGoods&
  DFSEngine::nogoods(void) {
    return path;
  }

  forceinline Gecode::Space*
  DFSEngine::next(void) {
    start();
    while (true) {
      while (cur) {
        if (stop(opt))
          return NULL;
        node++;
        switch (cur->status(*this)) {
          case Gecode::SS_FAILED:
          fail++;
          delete cur;
          cur = NULL;
          break;
          case Gecode::SS_SOLVED:
          {
            // Deletes all pending branchers
            (void) cur->choice();
            Gecode::Space* s = cur;
            cur = NULL;
            return s;
          }
          case Gecode::SS_BRANCH:
          {
            Gecode::Space* c;
            if ((d == 0) || (d >= opt.c_d)) {
              c = cur->clone();
              d = 1;
            } else {
              c = NULL;
              d++;
            }
            const Gecode::Choice* ch = path.push(*this,cur,c);
            cur->commit(*ch,0);
            break;
          }
        default:
          GECODE_NEVER;
        }
      }
      do {
        if (!path.next())
          return NULL;
        cur = path.recompute(d,opt.a_d,*this);
      } while (cur == NULL);
    }
    GECODE_NEVER;
    return NULL;
  }

  forceinline Gecode::Search::Statistics
  DFSEngine::statistics(void) const {
    return *this;
  }

  forceinline 
  DFSEngine::~DFSEngine(void) {
    delete cur;
    path.reset();
  }


}

#endif