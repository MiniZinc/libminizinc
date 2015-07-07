/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
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
      UNORDERED_NAMESPACE::unordered_map<std::string, KeepAlive> _options;

      inline Expression* getParam(const std::string& name) const;

    public:
      void setIntParam(const std::string& name,   KeepAlive e);
      void setFloatParam(const std::string& name, KeepAlive e);
      void setBoolParam(const std::string& name,  KeepAlive e);
      void setIntParam(const std::string& name,   long long int e);
      void setFloatParam(const std::string& name, double e);
      void setBoolParam(const std::string& name,  bool e);
      long long int getIntParam(const std::string& name) const;
      long long int getIntParam(const std::string& name, long long int def) const;
      double getFloatParam(const std::string& name) const;
      double getFloatParam(const std::string& name, double def) const;
      bool getBoolParam(const std::string& name) const;
      bool getBoolParam(const std::string& name, bool def) const;
      bool hasParam(const std::string& name) const;
  };
  
  // Options for the command line interface
  class CLIOptions : public Options {
  public:
    void setStringParam(const std::string& name, KeepAlive e);
    void setStringParam(const std::string& name, std::string& e);
    std::string getStringParam(const std::string& name) const;
    std::string getStringParam(const std::string& name, std::string& def) const; 
    
    // NOTE: setting string vector parameters is very expensive since they are converted into ArrayLits and back
    void setStringVectorParam(const std::string& name, KeepAlive e);
    void setStringVectorParam(const std::string& name, const std::vector<std::string>& e);
    std::vector<std::string> getStringVectorParam(const std::string& name) const;
    std::vector<std::string> getStringVectorParam(const std::string& name, std::vector<std::string>& def) const; 
  };
  
  /// class for each CLI option
  class CLIOption {
  protected:
    /// option name as given in the command line, e.g. \"--help\"
    const std::string _name;
    /// the number of arguments of the option
    int _nbArgs;
    /// the arg string can begin with the name, e.g. -I/home/user/mydir/
    bool _beginsWith;
    /// the value the option has been set to via the command line
    KeepAlive _value;
    //// default value
    KeepAlive _def;
  public:
    CLIOption(const std::string& name, int nbArgs, bool beginsWith, KeepAlive def) : 
    _name(name), _nbArgs(nbArgs), _beginsWith(beginsWith), _def(def) {}
    
    bool setValue(std::string v);
    bool setValue(int v);
    bool setValue(bool v);
    bool setValue(float f);
    bool setValue(std::vector<std::string> v);
  };
}

#endif
