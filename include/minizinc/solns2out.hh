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

#include <minizinc/astexception.hh>
#include <minizinc/builtins.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/flatten.hh>
#include <minizinc/flatten_internal.hh>  // temp., TODO
#include <minizinc/model.hh>
#include <minizinc/optimize.hh>
#include <minizinc/parser.hh>
#include <minizinc/solver_instance.hh>
#include <minizinc/typecheck.hh>
#include <minizinc/utils.hh>

#include <ctime>
#include <iomanip>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace MiniZinc {

/// Class handling fzn solver's output
/// could facilitate exhange of raw/final outputs in a portfolio
class Solns2Out {
protected:
  std::unique_ptr<Env> _envGuard;
  Env* _env = nullptr;
  Model* _outputModel = nullptr;

  typedef std::pair<VarDecl*, KeepAlive> DE;
  ManagedASTStringMap<DE> _declmap;
  Expression* _outputExpr = nullptr;
  std::string _checkerModel;
  std::string _statisticsCheckerModel;
  bool _fNewSol2Print = false;  // should be set for evalOutput to work

public:
  std::string solution;
  std::string comments;
  int nLinesIgnore = 0;

  struct Options {
    std::string flagOutputFile;
    bool flagOutputComments = true;
    bool flagOutputFlush = true;
    bool flagOutputTime = false;
    int flagIgnoreLines = 0;
    bool flagUnique = true;
    bool flagCanonicalize = false;
    bool flagStandaloneSolns2Out = false;
    std::string flagOutputNoncanonical;
    std::string flagOutputRaw;
    int flagNumberOutput = -1;
    /// Default values, also used for input
    const char* const solutionSeparatorDef = "----------";
    const char* const unsatisfiableMsgDef = "=====UNSATISFIABLE=====";
    const char* const unboundedMsgDef = "=====UNBOUNDED=====";
    const char* const unsatorunbndMsgDef = "=====UNSATorUNBOUNDED=====";
    const char* const unknownMsgDef = "=====UNKNOWN=====";
    const char* const errorMsgDef = "=====ERROR=====";
    const char* const searchCompleteMsgDef = "==========";
    /// Output values
    std::string solutionSeparator = solutionSeparatorDef;
    std::string solutionComma;
    std::string unsatisfiableMsg = unsatisfiableMsgDef;
    std::string unboundedMsg = unboundedMsgDef;
    std::string unsatorunbndMsg = unsatorunbndMsgDef;
    std::string unknownMsg = unknownMsgDef;
    std::string errorMsg = errorMsgDef;
    std::string searchCompleteMsg = searchCompleteMsgDef;
  } opt;

  struct Statistics {
    unsigned long long nSolns = 0;
    unsigned long long nFails = 0;
    unsigned long long nNodes = 0;
  } stats;

  ~Solns2Out();
  Solns2Out(std::ostream& os, std::ostream& log, std::string stdlibDir);

  bool processOption(int& i, std::vector<std::string>& argv,
                     const std::string& workingDir = std::string());
  static void printHelp(std::ostream& os);

  /// The output model (~.ozn) can be passed in 1 way in this base class:
  /// passing Env* containing output()
  bool initFromEnv(Env* pE);

  /// Then, variable assignments can be passed either as text
  /// or put directly into envi()->output() ( latter done externally
  /// by e.g. SolverInstance::assignSolutionToOutput() )
  /// In the 1st case, (part of) the assignment text is passed as follows,
  /// original end-of-lines need to be there as well
  bool feedRawDataChunk(const char* data);

  SolverInstance::Status status = SolverInstance::UNKNOWN;
  bool fStatusPrinted = false;
  /// Should be called when entering new solution into the output model.
  /// Default assignSolutionToOutput() does it by using findOutputVar().
  void declNewOutput();

  /// This can be used by assignSolutionToOutput()
  DE& findOutputVar(const ASTString& name);

  /// In the other case,
  /// the evaluation procedures print output/status to os
  /// returning false means need to stop (error/ too many solutions)
  /// Solution validation here   TODO
  /// Note that --canonicalize delays output
  /// until ... exit, eof,  ??   TODO
  /// These functions should only be called explicitly
  /// from SolverInstance
  bool evalOutput(const std::string& s_ExtraInfo = "");
  /// This means the solver exits
  bool evalStatus(SolverInstance::Status status);

  void printStatistics(std::ostream& os);

  Env* getEnv() const { return _env; }
  Model* getModel() const {
    assert(getEnv()->output());
    return getEnv()->output();
  }
  /// Get the primary output stream
  /// First call restores stdout
  std::ostream& getOutput();
  /// Get the secondary output stream
  std::ostream& getLog();

private:
  Timer _starttime;

  std::unique_ptr<std::ostream> _outStream;  // file output
  std::unique_ptr<std::ostream> _outStreamNonCanon;
  std::unique_ptr<std::ostream> _outStreamRaw;
  std::set<std::string> _sSolsCanon;
  std::string _linePart;  // non-finished line from last chunk

  /// Initialise from ozn file
  void initFromOzn(const std::string& filename);

protected:
  std::ostream& _os;
  std::ostream& _log;
  std::vector<std::string> _includePaths;
  std::string _stdlibDir;

  // Basically open output
  void init();
  std::map<std::string, SolverInstance::Status> _mapInputStatus;
  void createInputMap();
  void restoreDefaults();
  /// Parsing fznsolver's complete raw text output
  void parseAssignments(std::string& solution);
  /// Checking solution against checker model
  void checkSolution(std::ostream& os);
  void checkStatistics(std::ostream& os);
  bool evalOutputInternal(std::ostream& fout);
  bool evalOutputFinalInternal(bool flag_flush);
  bool evalStatusMsg(SolverInstance::Status status);
};

// Passthrough Solns2Out class
class Solns2Log {
private:
  std::ostream& _log;
  std::ostream& _errLog;

public:
  Solns2Log(std::ostream& log, std::ostream& errLog) : _log(log), _errLog(errLog) {}
  bool feedRawDataChunk(const char* data) {
    _log << data << std::flush;
    return true;
  }
  std::ostream& getLog() { return _errLog; }
};

}  // namespace MiniZinc
