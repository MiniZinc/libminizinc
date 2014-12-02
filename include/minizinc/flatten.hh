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
#include <minizinc/astexception.hh>

namespace MiniZinc {

  class EnvI;
  
  /// Environment for flattening
  class Env {
  private:
    EnvI* e;
  public:
    Env(Model* m);
    ~Env(void);
    
    Model* model(void);
    Model* flat(void);
    Model* output(void);
    EnvI& envi(void);
    std::ostream& dumpErrorStack(std::ostream& os);
    const std::vector<std::string>& warnings(void);
    std::ostream& evalOutput(std::ostream& os);
  };

  /// Exception thrown for errors during flattening
  class FlatteningError : public LocationException {
  public:
    FlatteningError(EnvI& env, const Location& loc, const std::string& msg);
    ~FlatteningError(void) throw() {}
    virtual const char* what(void) const throw() {
      return "MiniZinc: flattening error";
    }
  };

  /// Options for the flattener
  struct FlatteningOptions {
    /// Variables support only ranges, convert holes into != constraints
    bool onlyRangeDomains;
    /// Keep output in resulting flat model
    bool keepOutputInFzn;
    /// Record reason for variable introductions
    bool collectVarPaths;
    /// Check path table for tighter domains for introduced variables.
    bool useVarPaths;
    /// Don't record paths including let expressions
    bool topLevelPathsOnly;
    /// Default constructor
    FlatteningOptions(void) : onlyRangeDomains(false), keepOutputInFzn(false), collectVarPaths(false), useVarPaths(false), topLevelPathsOnly(false) {}
  };

  class Pass {
    private:
      FlatteningOptions fopts;

    public:
      Pass(FlatteningOptions& opts) : fopts(opts) {};
      virtual std::string getLibrary() = 0;
      FlatteningOptions& getFlatteningOptions() {return fopts;};
      virtual void run(Env& env) = 0;
      virtual ~Pass() {};
  };

  class CompilePass : public Pass {
    private:
      std::string library;

    public:
      CompilePass(FlatteningOptions& opts, std::string globals_library) : Pass(opts), library(globals_library) {}
      std::string getLibrary() { return library; }
      void run(Env& env) { };
      ~CompilePass() {};
  };

  /// Flatten model \a m several times and record information in its env
  void multiPassFlatten(Env& m, std::vector<std::string>& includePaths, std::vector<Pass*>& passes);
  
  /// Flatten model \a m
  void flatten(Env& m, FlatteningOptions opt = FlatteningOptions());

  /// Translate \a m into old FlatZinc syntax
  void oldflatzinc(Env& m);
  
}

#endif
