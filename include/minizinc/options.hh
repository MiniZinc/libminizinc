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
      std::unordered_map<std::string, KeepAlive> _options;

      Expression* getParam(const std::string& name) const;

    public:
      void setIntParam(const std::string& name,   KeepAlive e);
      void setFloatParam(const std::string& name, KeepAlive e);
      void setBoolParam(const std::string& name,  KeepAlive e);
      void setStringParam(const std::string& name,  KeepAlive e);
      void setIntParam(const std::string& name,   long long int e);
      void setFloatParam(const std::string& name, double e);
      void setBoolParam(const std::string& name,  bool e);
      void setStringParam(const std::string& name, std::string e);
      long long int getIntParam(const std::string& name) const;
      long long int getIntParam(const std::string& name, long long int def) const;
      double getFloatParam(const std::string& name) const;
      double getFloatParam(const std::string& name, double def) const;
      bool getBoolParam(const std::string& name) const;
      bool getBoolParam(const std::string& name, bool def) const;
      std::string getStringParam(const std::string& name) const;
      std::string getStringParam(const std::string& name, std::string def) const;
      bool hasParam(const std::string& name) const;
      std::ostream& dump(std::ostream& os);
  };
}

#endif
