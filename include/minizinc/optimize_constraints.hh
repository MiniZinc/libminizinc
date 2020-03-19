/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/hash.hh>
#include <minizinc/flatten_internal.hh>

namespace MiniZinc {
  
  class OptimizeRegistry {
  public:
    enum ConstraintStatus { CS_NONE, CS_OK, CS_FAILED, CS_ENTAILED, CS_REWRITE };
    typedef ConstraintStatus (*optimizer) (EnvI& env, Item* i, Call* c, Expression*& rewrite);
  protected:
    ASTStringMap<optimizer>::t _m;
  public:
    
    void reg(const ASTString& call, optimizer);
    ConstraintStatus process(EnvI& env, Item* i, Call* c, Expression*& rewrite);
    
    static OptimizeRegistry& registry(void);
  };
  
}
