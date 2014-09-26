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
    
    inline
    Expression* getParam(ASTString name) {
      IdMap<Expression*>::iterator it = _options.find(name);
      if(it == _options.end()) {
	std::stringstream ss;
	ss << "Could not find option: \"" << name << "\"." << std::endl;
	throw InternalError(ss.str());
      }
      return *it;
    }
  public:
    void setIntParam(ASTString name,   Expression* e) {
      if(e->type().ispar() && e->type().isint()) {
	_options[name] = e;
      } else {
	std::stringstream ss;
	ss << "For option: " << name << " expected Par Int, received " << e->type() << std::endl;
	throw InternalError(ss.str());
      }
    }
    void setFloatParam(ASTString name, Expression* e) {
      if(e->type().ispar() && e->type().isfloat()) {
	_options[name] = e;
      } else {
	std::stringstream ss;
	ss << "For option: " << name << " expected Par Float, received " << e->type() << std::endl;
	throw InternalError(ss.str());
      }
    }
    void setBoolParam(ASTString name,  Expression* e) {
      if(e->type().ispar() && e->type().isbool()) {
	_options[name] = e;
      } else {
	std::stringstream ss;
	ss << "For option: " << name << " expected Par Bool, received " << e->type() << std::endl;
	throw InternalError(ss.str());
      }
    }
    
    void setIntParam(ASTString name,   long long int e) {
      GCLock lock;
      IntLit* il = new IntLit(Location(), new IntVal(e));
      KeepAlive ka(il);

      setIntParam(name, il);
    };
    void setFloatParam(ASTString name, double e) {
      GCLock lock;
      FloatLit* fl = new FloatLit(Location(), e);
      KeepAlive ka(fl);
      
      setFloatParam(name, fl);
    }
    void setBoolParam(ASTString name,  bool e) {
      GCLock lock;
      BoolLit* bl = new BoolLit(Location(), e);
      KeepAlive ka(bl);
      
      setBoolParam(name, bl);
    }
    
    long long int getIntParam(ASTString name) {
      if(IntLit* il = getParam(name)->dyn_cast<IntLit>()) {
	return il->v();
      } else {
	std::stringstream ss;
	ss << "Option: \"" << name << "\" is not Par Int" << std::endl;
	throw InternalError(ss.str());
      }
    }
    double getFloatParam(ASTString name) {
      if(FloatLit* fl = getParam(name)->dyn_cast<FloatLit>()) {
	return fl->v();
      } else {
	std::stringstream ss;
	ss << "Option: \"" << name << "\" is not Par Float" << std::endl;
	throw InternalError(ss.str());
      }
    }
    bool getBoolParam(ASTString name) {
      if(BoolLit* bl = getParam(name)->dyn_cast<BoolLit>()) {
	return bl->v();
      } else {
	std::stringstream ss;
	ss << "Option: \"" << name << "\" is not Par Bool" << std::endl;
	throw InternalError(ss.str());
      }
    }
  };
}

#endif