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
    BoolLit* lit_true;
    /// Variable bound to true
    VarDecl* var_true;
    /// Literal false
    BoolLit* lit_false;
    /// Variable bound to false
    VarDecl* var_false;
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
    /// Identifiers for Boolean contexts
    struct {
      Id* root;
      Id* pos;
      Id* neg;
      Id* mix;
    } ctx;
    /// Constructor
    Constants(void);
    /// Return shared BoolLit
    BoolLit* boollit(bool b) {
      return b ? lit_true : lit_false;
    }
  };
  
  /// Return static instance
  Constants& constants(void);

  /// Flatten model \a m
  Model* flatten(Model* m);

  /// Translate \a m into old FlatZinc syntax
  void oldflatzinc(Model* m);
  
}

#endif
