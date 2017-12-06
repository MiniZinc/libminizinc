/* -*- mode: C++; c-basic-offOptions::set: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/options.hh>
#include <minizinc/stl_map_set.hh>

namespace MiniZinc {
  
  Expression* Options::getParam(const std::string& name) const {
    UNORDERED_NAMESPACE::unordered_map<std::string, KeepAlive >::const_iterator it = _options.find(name);
    if(it == _options.end()) {
      std::stringstream ss;
      ss << "Could not find option: \"" << name << "\"." << std::endl;
      throw InternalError(ss.str());
    }
    return (it->second)();
  }
  
  void Options::setIntParam(const std::string& name,   KeepAlive ka) {
    Expression* e = ka();
    if(e && e->type().ispar() && e->type().isint()) {
      _options[name] = ka;
    } else {
      std::stringstream ss;
      ss << "For option: " << name << " expected Par Int, received " << e->type().nonEnumToString() << std::endl;
      throw InternalError(ss.str());
    }
  }
  void Options::setFloatParam(const std::string& name, KeepAlive ka) {
    Expression* e = ka();
    if(e && e->type().ispar() && e->type().isfloat()) {
      _options[name] = ka;
    } else {
      std::stringstream ss;
      ss << "For option: " << name << " expected Par Float, received " << e->type().nonEnumToString() << std::endl;
      throw InternalError(ss.str());
    }
  }
  void Options::setBoolParam(const std::string& name,  KeepAlive ka) {
    Expression* e = ka();
    if(e && e->type().ispar() && e->type().isbool()) {
      _options[name] = ka;
    } else {
      std::stringstream ss;
      ss << "For option: " << name << " expected Par Bool, received " << e->type().nonEnumToString() << std::endl;
      throw InternalError(ss.str());
    }
  }
  void Options::setStringParam(const std::string& name,  KeepAlive ka) {
    Expression* e = ka();
    if(e && e->type().ispar() && e->type().isstring()) {
      _options[name] = ka;
    } else {
      std::stringstream ss;
      ss << "For option: " << name << " expected Par String, received " << e->type().nonEnumToString() << std::endl;
      throw InternalError(ss.str());
    }
  }
  
  void Options::setIntParam(const std::string& name,   long long int e) {
    GCLock lock;
    IntLit* il = IntLit::a(e);
    KeepAlive ka(il);
    
    setIntParam(name, ka);
  };
  void Options::setFloatParam(const std::string& name, double e) {
    GCLock lock;
    FloatLit* fl = FloatLit::a(e);
    KeepAlive ka(fl);
    
    setFloatParam(name, ka);
  }
  void Options::setBoolParam(const std::string& name,  bool e) {
    KeepAlive ka(constants().boollit(e));    
    setBoolParam(name, ka);
  }
  void Options::setStringParam(const std::string& name,  std::string e) {
    GCLock lock;
    StringLit* sl = new StringLit(Location(), e);
    KeepAlive ka(sl);
    
    setStringParam(name, ka);
  }
  
  long long int Options::getIntParam(const std::string& name) const {
    if(IntLit* il = getParam(name)->dyn_cast<IntLit>()) {
      return il->v().toInt();
    } else {
      std::stringstream ss;
      ss << "Option: \"" << name << "\" is not Par Int" << std::endl;
      throw InternalError(ss.str());
    }
  }
  long long int Options::getIntParam(const std::string& name, long long int def) const {
    if (hasParam(name)) {
      if(IntLit* il = getParam(name)->dyn_cast<IntLit>()) {
        return il->v().toInt();
      }
    }
    return def;
  }
  double Options::getFloatParam(const std::string& name) const {
    if(FloatLit* fl = getParam(name)->dyn_cast<FloatLit>()) {
      return fl->v().toDouble();
    } else {
      std::stringstream ss;
      ss << "Option: \"" << name << "\" is not Par Float" << std::endl;
      throw InternalError(ss.str());
    }
  }
  double Options::getFloatParam(const std::string& name, double def) const {
    if (hasParam(name)) {
      if(FloatLit* fl = getParam(name)->dyn_cast<FloatLit>()) {
        return fl->v().toDouble();
      }
    }
    return def;
  }
  bool Options::getBoolParam(const std::string& name) const {
    if(BoolLit* bl = getParam(name)->dyn_cast<BoolLit>()) {
      return bl->v();
    } else {
      std::stringstream ss;
      ss << "Option: \"" << name << "\" is not Par Bool" << std::endl;
      throw InternalError(ss.str());
    }
  }
  bool Options::getBoolParam(const std::string& name, bool def) const {
    if (hasParam(name)) {
      if(BoolLit* bl = getParam(name)->dyn_cast<BoolLit>()) {
        return bl->v();
      }
    }
    return def;
  }
  std::string Options::getStringParam(const std::string& name) const {
    if(StringLit* sl = getParam(name)->dyn_cast<StringLit>()) {
      return sl->v().str();
    } else {
      std::stringstream ss;
      ss << "Option: \"" << name << "\" is not Par String" << std::endl;
      throw InternalError(ss.str());
    }
  }
  std::string Options::getStringParam(const std::string& name, std::string def) const {
    if (hasParam(name)) {
      if(StringLit* sl = getParam(name)->dyn_cast<StringLit>()) {
        return sl->v().str();
      }
    }
    return def;
  }
  bool Options::hasParam(const std::string& name) const {
    return _options.find(name) != _options.end();
  }
  std::ostream& Options::dump(std::ostream& os) {
    for ( auto& it: _options )
      os << it.first << ':' << it.second() << ' ';
    return os;
  }
}
