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
      ASTStringMap<Expression*>::t _options;

      inline Expression* getParam(const ASTString& name) const;

    public:
      void setIntParam(const ASTString& name,   Expression* e);
      void setFloatParam(const ASTString& name, Expression* e);
      void setBoolParam(const ASTString& name,  Expression* e);
      void setIntParam(const ASTString& name,   long long int e);
      void setFloatParam(const ASTString& name, double e);
      void setBoolParam(const ASTString& name,  bool e);
      long long int getIntParam(const ASTString& name) const;
      double getFloatParam(const ASTString& name) const;
      bool getBoolParam(const ASTString& name) const;
      bool hasParam(const ASTString& name) const;
  };
}

#endif
