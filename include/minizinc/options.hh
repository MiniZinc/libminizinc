/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_OPTIONS_HH__
#define __MINIZINC_OPTIONS_HH__

#include <minizinc/hash.hh>

namespace MiniZinc {
  class Options {
    protected:
      IdMap<Expression*> _options;

      inline Expression* getParam(ASTString name);

    public:
      void setIntParam(ASTString name,   Expression* e);
      void setFloatParam(ASTString name, Expression* e);
      void setBoolParam(ASTString name,  Expression* e);
      void setIntParam(ASTString name,   long long int e);
      void setFloatParam(ASTString name, double e);
      void setBoolParam(ASTString name,  bool e);
      long long int getIntParam(ASTString name);
      double getFloatParam(ASTString name);
      bool getBoolParam(ASTString name);
  };
}

#endif
