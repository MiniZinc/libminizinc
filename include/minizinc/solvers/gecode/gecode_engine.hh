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
  
    /// search engine for standard Gecode search, wrapper class for engine
  class GecodeEngine {
  public:
    virtual FznSpace* next(void) = 0;
    virtual bool stopped(void) = 0;    
    virtual ~GecodeEngine(void) {}
  };
  
   /// custom search engine for search combinators, wrapper class for engine
  class CustomEngine : public GecodeEngine {
  public:
    virtual FznSpace* next(void) = 0;
    virtual bool stopped(void) = 0;
    virtual void updateIntBounds(VarDecl* vd, int lb, int ub, GecodeSolverInstance& si_) = 0;
    virtual void addVariables(std::vector<VarDecl*> vars, GecodeSolverInstance& si) = 0; 
    /// returns the space (or NULL) at position \a i in the engine dynamic stack
    virtual FznSpace* getSpace(unsigned int i) = 0;
    /// returns the number of entries in the path (that do not all need to be spaces!)
    virtual unsigned int pathEntries(void) = 0;
    virtual ~CustomEngine(void) {}
  };
  
    /// meta-engine that inherits from GecodeEngine
  template<template<class> class E,
           template<template<class> class,class> class Meta>
  class MetaEngine : public GecodeEngine {
    Meta<E,FznSpace> e;
  public:
    MetaEngine(FznSpace* s, Gecode::Search::Options& o) : e(s,o) {}
    virtual FznSpace* next(void) { return e.next(); }
    virtual bool stopped(void) { return e.stopped(); }    
  };
  
  /// meta-engine that inherits from CustomEngine
  template<template<class> class E,
           template<template<class> class,class> class Meta>
  class CustomMetaEngine : public CustomEngine {
    Meta<E,FznSpace> e;
  public:
    CustomMetaEngine(FznSpace* s, Gecode::Search::Options& o) : e(s,o) {}
    virtual FznSpace* next(void) { return e.next(); }
    virtual bool stopped(void) { return e.stopped(); }
    virtual void updateIntBounds(VarDecl* vd, int lb, int ub, GecodeSolverInstance& si) { e.updateIntBounds(vd,lb,ub,si); }
    virtual void addVariables(std::vector<VarDecl*> vars, GecodeSolverInstance& si) { e.addVariables(vars, si); }   
    virtual FznSpace* getSpace(unsigned int i) { return e.getSpace(i); }
    virtual unsigned int pathEntries(void) { return e.pathEntries(); }
  };
  
  /// special meta (wrapper) class to allow additional functionality
  template<template<class> class E, class T>
  class GecodeMeta : E<T> {
  public:
    GecodeMeta(T* s, const Gecode::Search::Options& o) : E<T>(s,o) {} 
    void updateIntBounds(VarDecl* vd, int lb, int ub, GecodeSolverInstance& si) {  E<T>::updateIntBounds(vd,lb,ub,si);  }
    void addVariables(std::vector<VarDecl*> vars, GecodeSolverInstance& si) { E<T>::addVariables(vars, si); }    
    FznSpace* getSpace(unsigned int i) { return E<T>::getSpace(i); }
    unsigned int pathEntries(void) { return E<T>::pathEntries(); }
    FznSpace* next(void) { return E<T>::next(); }
    bool stopped(void) { return E<T>::stopped(); }
  };  
    
  
  /// subclass of actual path to access the iterative stack
  class Path : public Gecode::Search::Sequential::Path {
  public:
    /// path constructor
    Path(int l) : Gecode::Search::Sequential::Path(l) {} 
    /// get the space at edge \a i in the edge stack; can be NULL
    Gecode::Space* getSpace(unsigned int i) { assert(i < ds.entries()); return ds[i].space(); }
    /// get the number of entries in the edge stack
    int getNbEntries(void) { return ds.entries(); }    

    Gecode::Space*
    recompute(unsigned int& d, unsigned int a_d, Gecode::Search::Worker& stat) {
      assert(!ds.empty());
      // Recompute space according to path
      // Also say distance to copy (d == 0) requires immediate copying
      
      // Check for LAO
      if ((ds.top().space() != NULL) && ds.top().rightmost()) {
        Gecode::Space* s = ds.top().space();
        s->commit(*ds.top().choice(),ds.top().alt());
        assert(ds.entries()-1 == lc());
        ds.top().space(NULL);
        // Mark as reusable
        if (ds.entries() > ngdl())
          ds.top().next();
        d = 0;
        return s;
      }
      // General case for recomputation
      int l = lc();             // Position of last clone
      int n = ds.entries();     // Number of stack entries
      // New distance, if no adaptive recomputation
      d = static_cast<unsigned int>(n - l);
      
      Gecode::Space* s = ds[l].space(); // Last clone
      
      // The space on the stack could be failed now as an additional
      // constraint might have been added.
      if (s->status(stat) == Gecode::SS_FAILED) {
        // s does not need deletion as it is on the stack (unwind does this)
        stat.fail++;
        unwind(l);
        return NULL;
      }
      // It is important to replace the space on the stack with the
      // copy: a copy might be much smaller due to flushed caches
      // of propagators
      Gecode::Space* c = s->clone();
      ds[l].space(c);
      
      if (d < a_d) {
        // No adaptive recomputation
        for (int i=l; i<n; i++)
          commit(s,i);
      } else {
        int m = l + static_cast<int>(d >> 1); // Middle between copy and top
        int i = l;            // To iterate over all entries
        // Recompute up to middle
        for (; i<m; i++ )
          commit(s,i);
        // Skip over all rightmost branches
        for (; (i<n) && ds[i].rightmost(); i++)
          commit(s,i);
        // Is there any point to make a copy?
        if (i<n-1) {
          // Propagate to fixpoint
          Gecode::SpaceStatus ss = s->status(stat);
          /*
           * Again, the space might already propagate to failure
           *
           * This can be for two reasons:
           *  - constrain is true, so we fail
           *  - the space has weakly monotonic propagators
           */
          if (ss == Gecode::SS_FAILED) {
            // s must be deleted as it is not on the stack
            delete s;
            stat.fail++;
            unwind(i);
            return NULL;
          }
          ds[i].space(s->clone());
          d = static_cast<unsigned int>(n-i);
        }
        // Finally do the remaining commits
        for (; i<n; i++)
          commit(s,i);
      }
      return s;
    }
    
    virtual void post(Gecode::Space& home) const;
  };
  
  /// iterative DFS class that allows to add constraints along the path
  template<class T>
  class DFSEngine  : public Gecode::Search::Worker, public Gecode::EngineBase {
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
    DFSEngine(T* s, const Gecode::Search::Options& o);
    /// %Search for next solution
    T* next(void);
    /// Return statistics
    Statistics statistics(void) const;
    /// Reset engine to restart at space \a s
    void reset(T* s);
    /// Return no-goods
    Gecode::NoGoods& nogoods(void);    
    /// adds variable to the engine
    // void addVariable(VarDecl* vd, GecodeSolverInstance& si);
    /// update the integer bounds of the given variable to the tighter bounds (lb..ub)
    void updateIntBounds(VarDecl* vd, int lb, int ub, GecodeSolverInstance& si);
    /// add variables to the search engine
    void addVariables(std::vector<VarDecl*> vars, GecodeSolverInstance& si);
    /// returns the space (or NULL) at position \a i in the engine dynamic stack
    FznSpace* getSpace(unsigned int i) { return static_cast<FznSpace*>(path.getSpace(i)); }
    /// returns the number of entries in the path (that do not all need to be spaces!)
    unsigned int pathEntries(void) { return path.getNbEntries(); }
    /// Destructor
    ~DFSEngine(void);
  };
  
  template<class T>
  forceinline 
  DFSEngine<T>::DFSEngine(T* s, const Gecode::Search::Options& o)
    : opt(o), d(0), path(static_cast<int>(opt.nogoods_limit)) {
    if ((s == NULL) || (s->status(*this) == Gecode::SS_FAILED)) {
      fail++;
      cur = NULL;
      if (!opt.clone)
        delete s;
    } else {
      cur = snapshot(s,opt);
    }
  }

  template<class T>
  forceinline void  
  DFSEngine<T>::reset(T* s) {
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

  template<class T>
  forceinline Gecode::NoGoods&
  DFSEngine<T>::nogoods(void) {
    return path;
  }

  template<class T>
  forceinline T*
  DFSEngine<T>::next(void) {
    start();
    while (true) {
      if (stop(opt))
        return NULL;
      while (cur == NULL) {
        if (path.empty())
          return NULL;
        cur = path.recompute(d,opt.a_d,*this);
        if (cur != NULL)
          break;
        path.next();
      }
      node++;
      switch (cur->status(*this)) {
        case Gecode::SS_FAILED:
          fail++;
          delete cur;
          cur = NULL;
          path.next();
          break;
        case Gecode::SS_SOLVED:
        {
          // Deletes all pending branchers
          (void) cur->choice();
          Gecode::Space* s = cur;
          cur = NULL;
          path.next();
          return static_cast<T*>(s);
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
    GECODE_NEVER;
    return NULL;
  }

  template<class T>
  forceinline Gecode::Search::Statistics
  DFSEngine<T>::statistics(void) const {
    return *this;
  }

  template<class T>
  forceinline 
  DFSEngine<T>::~DFSEngine(void) {
    delete cur;
    path.reset();
  }
  
  template<class T>
  void
  DFSEngine<T>::updateIntBounds(VarDecl* vd, int lb, int ub, GecodeSolverInstance& si) {   
    // iterate over stack and post constraint
    if(path.empty()) 
      return;    
    for(int edge=0; edge<path.getNbEntries(); edge++) {
      T* s = static_cast<T*>(path.getSpace(edge));
      if(s) {
        FznSpace* space = static_cast<FznSpace*>(s);       
        si.updateIntBounds(space, vd,lb,ub);
      }
    }
  }
  
  template<class T>
  void
  DFSEngine<T>::addVariables(std::vector<VarDecl*> vars, GecodeSolverInstance& si) {   
    // iterate over stack and post constraint
    if(path.empty()) 
      return;    
    for(int edge=0; edge<path.getNbEntries(); edge++) {
      T* s = static_cast<T*>(path.getSpace(edge));
      if(s) {
        FznSpace* space = static_cast<FznSpace*>(s);
        si.addVariables(space, vars);
      }
    }
  }
  
}

#endif