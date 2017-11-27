/*
 *  Main authors:
 *     Kevin Leo <kevin.leo@monash.edu>
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef __MINIZINC_GECODE_FZN_SPACE_HH__
#define __MINIZINC_GECODE_FZN_SPACE_HH__

#include <gecode/kernel.hh>
#include <gecode/int.hh>
#include <gecode/driver.hh>

#ifdef GECODE_HAS_SET_VARS
#include <gecode/set.hh>
#endif

#ifdef GECODE_HAS_FLOAT_VARS
#include <gecode/float.hh>
#endif

#include <minizinc/solver_instance_base.hh>

namespace MiniZinc { 
  
 class FznSpace : public Gecode::Space {
  public:
    /// The integer variables
    std::vector<Gecode::IntVar> iv;
    /// The introduced integer variables
    Gecode::IntVarArray iv_aux;
    /// Indicates whether an integer variable is introduced by mzn2fzn
    std::vector<bool> iv_introduced;    
    /// Indicates whether an integer variable is defined
    std::vector<bool> iv_defined;    
    /// The Boolean variables
    std::vector<Gecode::BoolVar> bv;
    /// The introduced Boolean variables
    Gecode::BoolVarArray bv_aux;
    /// Indicates whether a Boolean variable is introduced by mzn2fzn
    std::vector<bool> bv_introduced;
    /// Indicates whether a Boolean variable is defined
    std::vector<bool> bv_defined;
#ifdef GECODE_HAS_SET_VARS
    /// The set variables
    std::vector<Gecode::SetVar> sv;
    /// The introduced set variables
    Gecode::SetVarArray sv_aux;
    /// Indicates whether a set variable is introduced by mzn2fzn
    std::vector<bool> sv_introduced;
        /// Indicates whether a set variable is introduced by mzn2fzn
    std::vector<bool> sv_defined;
#endif
#ifdef GECODE_HAS_FLOAT_VARS
    /// The float variables
    std::vector<Gecode::FloatVar> fv;
    /// The introduced float variables
    Gecode::FloatVarArray fv_aux;
    /// Indicates whether a float variable is introduced by mzn2fzn
    std::vector<bool> fv_introduced;
    /// Indicates whether a float variable is defined
    std::vector<bool> fv_defined;
#endif 
    /// Indicates if the objective variable is integer (float otherwise)
    bool _optVarIsInt;
    /// Index of the variable to optimize 
    int _optVarIdx;    
    /// Whether the introduced variables still need to be copied
    bool _copyAuxVars;  
    /// solve type (SAT, MIN or MAX)
    MiniZinc::SolveI::SolveType _solveType;
    
    /// copy constructor
    FznSpace(bool share, FznSpace&);
    /// standard constructor
    FznSpace(void) : _optVarIsInt(true), _optVarIdx(-1), _copyAuxVars(true) {};
    ~FznSpace(void) {} 
            
    /// get the index of the Boolean variable in bv; return -1 if not exists
    int getBoolAliasIndex(Gecode::BoolVar bvar) {
      for(unsigned int i=0; i<bv.size(); i++) {        
        if(bv[i].same(bvar)) { 
          // std::cout << "DEBUG: settings bool alias of variable to index " << i << std::endl;
          return i;          
        }            
      }
      return -1; // we should have found the boolvar in bv
    }
  
  protected:       
    /// Implement optimization
    virtual void constrain(const Space& s);
    /// Copy function
    virtual Gecode::Space* copy(bool share);
  };
  
}
#endif
