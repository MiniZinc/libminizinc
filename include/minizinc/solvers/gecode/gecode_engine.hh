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
  
 /* class FznSpaceIterator {
  protected:
    typedef FznSpace& reference;
    typedef FznSpace* pointer;
    typedef std::vector<FznSpace*>::iterator iterator;
    //Gecode::Support::DynamicStack<Gecode::Search::Sequential::Path::Edge,Gecode::Heap> _ds;
    MiniZinc::Path _path;
    std::vector<FznSpace*>::iterator _it;
    unsigned int _i;
  public:
    FznSpaceIterator() : _i(0) {}
    FznSpaceIterator(const FznSpaceIterator& fi) : _it(fi._it), _i(0) {}
    //FznSpaceIterator(Gecode::Support::DynamicStack<Gecode::Search::Sequential::Path::Edge,Gecode::Heap> ds, 
    FznSpaceIterator(MiniZinc::Path path, iterator& it) : _path(path), _it(it) {      
      while (_i!= path.entries() && !path.getSpace()) {
        ++_it; _i++;
      }
    }
    ~FznSpaceIterator() {}
    
    //iterator begin(void) { return _ds.top(); }
    //iterator end(void) { return _ds.; }
    reference operator*() const { return *static_cast<FznSpace>(*_it); }
    pointer operator->() const { return static_cast<FznSpace>(*_it); }
  }; */
  
  /// subclass of actual path to access the iterative stack
  class Path : public Gecode::Search::Sequential::Path {
  public:
    /// path constructor
    Path(int l) : Gecode::Search::Sequential::Path(l) {} 
    /// get the space at edge \a i in the edge stack; can be NULL
    Gecode::Space* getSpace(unsigned int i) { assert(i < ds.entries()); return ds[i].space(); }
    /// get the number of entries in the edge stack
    int getNbEntries(void) { return ds.entries(); }    
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
    /// post constraints to the search engine
    void postConstraints(std::vector<Call*> cts, GecodeSolverInstance& si);
    /// returns the space (or NULL) at position \a i in the engine dynamic stack
    FznSpace* getSpace(unsigned int i) { return static_cast<FznSpace*>(path.getSpace(i)); }
    /// returns the number of entries in the path (that do not all need to be spaces!)
    unsigned int pathEntries(void) { return path.getNbEntries(); }
    /// Destructor
    ~DFSEngine(void);
  };
  
  /// special meta (wrapper) class to allow additional functionality
  template<template<class> class E, class T>
  class GecodeMeta : E<T> {
  public:
    GecodeMeta(T* s, const Gecode::Search::Options& o) : E<T>(s,o) {} 
    void updateIntBounds(VarDecl* vd, int lb, int ub, GecodeSolverInstance& si) {  E<T>::updateIntBounds(vd,lb,ub,si);  }
    void addVariables(std::vector<VarDecl*> vars, GecodeSolverInstance& si) { E<T>::addVariables(vars, si); }
    void postConstraints(std::vector<Call*> cts, GecodeSolverInstance& si) { E<T>::postConstraints(cts, si); }
    FznSpace* getSpace(unsigned int i) { return E<T>::getSpace(i); }
    unsigned int pathEntries(void) { return E<T>::pathEntries(); }
    FznSpace* next(void) { return E<T>::next(); }
    bool stopped(void) { return E<T>::stopped(); }
  };
  
  template<class T>
  forceinline 
  DFSEngine<T>::DFSEngine(T* s, const Gecode::Search::Options& o)
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
  
  template<class T>
  void
  DFSEngine<T>::postConstraints(std::vector<Call*> cts, GecodeSolverInstance& si) {   
    // iterate over stack and post constraint
    if(path.empty()) 
      return;    
    for(int edge=0; edge<path.getNbEntries(); edge++) {
      T* s = static_cast<T*>(path.getSpace(edge));
      if(s) {
        FznSpace* space = static_cast<FznSpace*>(s);
        si.postConstraints(space,cts);
      }
    }
  }    

}

#endif