/* -*- mode: C++; c-basic-offOptions::set: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors: 
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/cli.hh>
//#include <minizinc/stl_map_set.hh>

namespace MiniZinc {
  
  void CLIOptions::setStringParam(const std::string& name, KeepAlive ka) {
    Expression* e = ka();
    if(e && e->type().ispar() && e->type().isstring()) {
      _options[name] = e;
    } else {
      std::stringstream ss;
      ss << "For option: " << name << " expected Par String, received " << e->type().toString() << std::endl;
      throw InternalError(ss.str());
    }
  }
  
  void CLIOptions::setStringParam(const std::string& name, std::string& s) {
    GCLock lock;
    StringLit* sl = new StringLit(Location(), s);
    KeepAlive ka(sl);
    
    setStringParam(name, ka);
  }
  
  std::string CLIOptions::getStringParam(const std::string& name) const {
    if(hasParam(name)) {
      if(StringLit* sl = getParam(name)->dyn_cast<StringLit>()) {
        return sl->v().str();
      }
    }    
    std::stringstream ss;
    ss << "Option: \"" << name << "\" does not exist or is not Par String" << std::endl;
    throw InternalError(ss.str());    
  }
  
  std::string CLIOptions::getStringParam(const std::string& name, std::string& def) const {
    if(hasParam(name)) {
      if(StringLit* sl = getParam(name)->dyn_cast<StringLit>()) {
        return sl->v().str();
      }
    } 
    return def;
  }
  
  void CLIOptions::setStringVectorParam(const std::string& name, KeepAlive ka) {
   Expression* e = ka();
    if(ArrayLit* al = e->dyn_cast<ArrayLit>()) {
      ASTExprVec<Expression> vec = al->v();
      if(vec.size() > 0 && vec[0]->type().ispar() && vec[0]->type().isstring()) {
        _options[name] = e;
        return;
      }
    }
    std::stringstream ss;
    ss << "For option: " << name << " expected Par String vector, received " << e->type().toString() << std::endl;
    throw InternalError(ss.str());
  }
  
  void CLIOptions::setStringVectorParam(const std::string& name, const std::vector<std::string>& v) {
    GCLock lock; 
    std::vector<Expression*> vs;
    for(unsigned int i=0; i<v.size(); i++)       
      vs.push_back(new StringLit(Location(),v[i]));
    ASTExprVec<Expression> vec(vs);
    ArrayLit* al = new ArrayLit(Location(), vec);
    KeepAlive ka(al);
    
    setStringVectorParam(name,ka);
  }
  
  std::vector<std::string> CLIOptions::getStringVectorParam(const std::string& name) const {
    if(hasParam(name)) {
      if(ArrayLit* al = getParam(name)->dyn_cast<ArrayLit>()) {    
        ASTExprVec<Expression> vec = al->v();
        if(vec.size() > 0 && vec[0]->type().isstring()) {
          std::vector<std::string> v;
          for(unsigned int i=0; i<vec.size(); i++)
            v.push_back(vec[i]->cast<StringLit>()->v().str());
          return v;
        }
      }
    }
    std::stringstream ss;
    ss << "Option: \"" << name << "\" does not exist or is not a String Vector" << std::endl;
    throw InternalError(ss.str());  
  }
  
  std::vector<std::string> CLIOptions::getStringVectorParam(const std::string& name, std::vector<std::string>& def) const {
    if(hasParam(name)) {
      if(ArrayLit* al = getParam(name)->dyn_cast<ArrayLit>()) {    
        ASTExprVec<Expression> vec = al->v();
        if(vec.size() > 0 && vec[0]->type().isstring()) {
          std::vector<std::string> v;
          for(unsigned int i=0; i<vec.size(); i++)
            v.push_back(vec[i]->cast<StringLit>()->v().str());
          return v;
        }
      }
    }
    return def;
  }  
}