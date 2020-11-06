/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/MIPdomains.hh>
#include <minizinc/astexception.hh>
#include <minizinc/builtins.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/flatten.hh>
#include <minizinc/flatten_internal.hh>  // temp., TODO
#include <minizinc/model.hh>
#include <minizinc/optimize.hh>
#include <minizinc/parser.hh>
#include <minizinc/passes/compile_pass.hh>
#include <minizinc/solver_instance.hh>
#include <minizinc/timer.hh>
#include <minizinc/typecheck.hh>
#include <minizinc/utils.hh>

#include <ctime>
#include <iomanip>
#include <memory>
#include <string>
#include <vector>
#ifdef HAS_GECODE
#include <minizinc/passes/gecode_pass.hh>
#endif

namespace MiniZinc {

class Flattener {
private:
  std::unique_ptr<Env> _pEnv;
  std::ostream& _os;
  std::ostream& _log;

public:
  Flattener(std::ostream& os, std::ostream& log, std::string stdlibDir);
  ~Flattener();
  bool processOption(int& i, std::vector<std::string>& argv,
                     const std::string& workingDir = std::string());
  static void printVersion(std::ostream& os);
  void printHelp(std::ostream& os) const;

  void flatten(const std::string& modelString = std::string(),
               const std::string& modelName = std::string("stdin"));
  void printStatistics(std::ostream& os);

  void setFlagVerbose(bool f) { _flags.verbose = f; }
  bool getFlagVerbose() const { return _flags.verbose; }
  void setFlagStatistics(bool f) { _flags.statistics = f; }
  bool getFlagStatistics() const { return _flags.statistics; }
  void setFlagTimelimit(unsigned long long int t) { _fopts.timeout = t; }
  unsigned long long int getFlagTimelimit() const { return _fopts.timeout; }
  void setFlagOutputByDefault(bool f) { _fOutputByDefault = f; }
  Env* getEnv() const {
    assert(_pEnv.get());
    return _pEnv.get();
  }
  bool hasInputFiles() const {
    return !_filenames.empty() || _flags.stdinInput || !_flagSolutionCheckModel.empty();
  }

  SolverInstance::Status status = SolverInstance::UNKNOWN;

private:
  Env* multiPassFlatten(const std::vector<std::unique_ptr<Pass> >& passes);

  bool _fOutputByDefault = false;  // if the class is used in mzn2fzn, write .fzn+.ozn by default
  std::vector<std::string> _filenames;
  std::vector<std::string> _datafiles;
  std::vector<std::string> _includePaths;
  bool _isFlatzinc = false;

  struct {
    bool typecheck = true;
    bool verbose = false;
    bool newfzn = false;
    bool optimize = true;
    bool chainCompression = true;
    bool werror = false;
    bool onlyRangeDomains = false;
    bool allowUnboundedVars = false;
    bool noMIPdomains = false;
    bool statistics = false;
    bool stdinInput = false;
    bool allowMultiAssign = false;
    bool gecode = false;
    bool twoPass = false;
    bool sac = false;
    bool shave = false;
    bool noOutputOzn = false;
    bool keepMznPaths = false;
    bool outputFznStdout = false;
    bool outputOznStdout = false;
    bool outputPathsStdout = false;
    bool instanceCheckOnly = false;
    bool modelCheckOnly = false;
    bool modelInterfaceOnly = false;
    bool modelTypesOnly = false;
    bool outputObjective = false;
    bool outputOutputItem = false;
    bool compileSolutionCheckModel = false;
  } _flags;

  int _optMIPDmaxIntvEE = 0;
  double _optMIPDmaxDensEE = 0.0;

  unsigned int _flagPrePasses = 1;

  std::string _stdLibDir;
  std::string _globalsDir;

  std::string _flagOutputBase;
  std::string _flagOutputFzn;
  std::string _flagOutputOzn;
  std::string _flagOutputPaths;
  FlatteningOptions::OutputMode _flagOutputMode = FlatteningOptions::OUTPUT_ITEM;
  std::string _flagSolutionCheckModel;
  FlatteningOptions _fopts;

  Timer _starttime;
};

}  // namespace MiniZinc
