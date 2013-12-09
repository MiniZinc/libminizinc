/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_FLATTEN_HH__
#define __MINIZINC_FLATTEN_HH__

#include <minizinc/model.hh>

namespace MiniZinc {

  /// Statically allocated constants
  class Constants {
  private:
    /// Model used as a garbage collection root set
    Model* m;
  public:
    /// Literal true
    BoolLit* lt;
    /// Variable bound to true
    VarDecl* t;
    /// Literal false
    BoolLit* lf;
    /// Variable bound to false
    VarDecl* f;
    /// Identifiers for builtins
    struct {
      ASTString forall;
      ASTString exists;
      ASTString bool2int;
      ASTString sum;
      ASTString lin_exp;
      ASTString bool_eq;
      ASTString bool_clause;
    } ids;
    /// Constructor
    Constants(void);
  };
  
  /// Return static instance
  Constants& constants(void);

  /// Flatten model \a m
  Model* flatten(Model* m);

  /// Translate \a m into old FlatZinc syntax
  void oldflatzinc(Model* m);
  
}

#endif
