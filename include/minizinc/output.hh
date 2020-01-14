/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_OUTPUT_HH__
#define __MINIZINC_OUTPUT_HH__

#include <minizinc/flatten_internal.hh>

namespace MiniZinc {

  /// Remove all output annotations from \a vd
  void removeIsOutput(VarDecl* vd);
  
  /// Copy output item to FlatZinc model
  void copyOutput(EnvI& e);
  
  /// Copy all dependent variable declarations
  void outputVarDecls(EnvI& env, Item* ci, Expression* e);
  
  /// Create initial output model
  void createOutput(EnvI& e, std::vector<VarDecl*>& deletedFlatVarDecls,
                    FlatteningOptions::OutputMode outputMode, bool outputObjective, bool includeOutputItem, bool hasChecker);
  /// Finalise output model after flattening is complete
  void finaliseOutput(EnvI& e, std::vector<VarDecl*>& deletedFlatVarDecls);
  
  /// Remove all links to variables in flat model from output model in \a env
  void cleanupOutput(EnvI& env);

  ArrayLit* createJSONOutput(EnvI& env, bool outputObjective, bool includeOutputItem, bool hasChecker);
  
}

#endif
