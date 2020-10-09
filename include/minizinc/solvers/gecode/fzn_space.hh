/*
 *  Main authors:
 *     Kevin Leo <kevin.leo@monash.edu>
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/solver_instance_base.hh>

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/kernel.hh>
#ifdef GECODE_HAS_SET_VARS
#include <gecode/set.hh>
#endif
#ifdef GECODE_HAS_FLOAT_VARS
#include <gecode/float.hh>
#endif
#undef ERROR

namespace MiniZinc {

class FznSpace : public Gecode::Space {
public:
  /// The integer variables
  std::vector<Gecode::IntVar> iv;
  /// The introduced integer variables
  Gecode::IntVarArray ivAux;
  /// Indicates whether an integer variable is introduced by mzn2fzn
  std::vector<bool> ivIntroduced;
  /// Indicates whether an integer variable is defined
  std::vector<bool> ivDefined;
  /// The Boolean variables
  std::vector<Gecode::BoolVar> bv;
  /// The introduced Boolean variables
  Gecode::BoolVarArray bvAux;
  /// Indicates whether a Boolean variable is introduced by mzn2fzn
  std::vector<bool> bvIntroduced;
  /// Indicates whether a Boolean variable is defined
  std::vector<bool> bvDefined;
#ifdef GECODE_HAS_SET_VARS
  /// The set variables
  std::vector<Gecode::SetVar> sv;
  /// The introduced set variables
  Gecode::SetVarArray svAux;
  /// Indicates whether a set variable is introduced by mzn2fzn
  std::vector<bool> svIntroduced;
  /// Indicates whether a set variable is introduced by mzn2fzn
  std::vector<bool> svDefined;
#endif
#ifdef GECODE_HAS_FLOAT_VARS
  /// The float variables
  std::vector<Gecode::FloatVar> fv;
  /// The introduced float variables
  Gecode::FloatVarArray fvAux;
  /// Indicates whether a float variable is introduced by mzn2fzn
  std::vector<bool> fvIntroduced;
  /// Indicates whether a float variable is defined
  std::vector<bool> fvDefined;
#endif
  /// Indicates if the objective variable is integer (float otherwise)
  bool optVarIsInt;
  /// Index of the variable to optimize
  int optVarIdx;
  /// Whether the introduced variables still need to be copied
  bool copyAuxVars;
  /// solve type (SAT, MIN or MAX)
  MiniZinc::SolveI::SolveType solveType;

  /// copy constructor
  FznSpace(FznSpace& f);
  /// standard constructor
  FznSpace() : optVarIsInt(true), optVarIdx(-1), copyAuxVars(true) {}
  ~FznSpace() override {}

  /// get the index of the Boolean variable in bv; return -1 if not exists
  int getBoolAliasIndex(const Gecode::BoolVar& bvar) {
    for (unsigned int i = 0; i < bv.size(); i++) {
      if (bv[i].varimp() == bvar.varimp()) {
        // std::cout << "DEBUG: settings bool alias of variable to index " << i << std::endl;
        return static_cast<int>(i);
      }
    }
    return -1;  // we should have found the boolvar in bv
  }

protected:
  /// Implement optimization
  void constrain(const Space& s) override;
  /// Copy function
  Gecode::Space* copy() override;
};

}  // namespace MiniZinc
