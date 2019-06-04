/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_SOLNS2OUT_H__
#define __MINIZINC_SOLNS2OUT_H__

#include <string>
#include <vector>
#include <set>
#include <ctime>
#include <memory>
#include <iomanip>
#include <unordered_map>

#include <minizinc/model.hh>
#include <minizinc/parser.hh>
#include <minizinc/typecheck.hh>
#include <minizinc/astexception.hh>

#include <minizinc/flatten.hh>
#include <minizinc/flatten_internal.hh>  // temp., TODO
#include <minizinc/optimize.hh>
#include <minizinc/builtins.hh>
#include <minizinc/utils.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/solver_instance.hh>

namespace MiniZinc {
  
  /// Class handling fzn solver's output
  /// could facilitate exhange of raw/final outputs in a portfolio
  class Solns2Out {
  protected:
    std::unique_ptr<Env> pEnv_guard;
    Env* pEnv=0;
    Model* pOutput=0;

    typedef std::pair<VarDecl*, KeepAlive> DE;
    std::unordered_map<std::string, DE> declmap;
    Expression* outputExpr = NULL;
    std::string checkerModel;
    bool fNewSol2Print = false;     // should be set for evalOutput to work
    
  public:
    std::string solution;
    std::string comments;
    int nLinesIgnore = 0;
    
    struct Options {
      std::string flag_output_file;
      bool flag_output_comments = true;
      bool flag_output_flush = true;
      bool flag_output_time = false;
      int flag_ignore_lines = 0;
      bool flag_unique = 1;
      bool flag_canonicalize = 0;
      bool flag_standaloneSolns2Out = false;
      std::string flag_output_noncanonical;
      std::string flag_output_raw;
      int flag_number_output = -1;
      /// Default values, also used for input
      const char* const solution_separator_00 = "----------";
      const char* const unsatisfiable_msg_00  = "=====UNSATISFIABLE=====";
      const char* const unbounded_msg_00      = "=====UNBOUNDED=====";
      const char* const unsatorunbnd_msg_00   = "=====UNSATorUNBOUNDED=====";
      const char* const unknown_msg_00        = "=====UNKNOWN=====";
      const char* const error_msg_00          = "=====ERROR=====";
      const char* const search_complete_msg_00= "==========";
      /// Output values
      std::string solution_separator = solution_separator_00;
      std::string solution_comma     = "";
      std::string unsatisfiable_msg  = unsatisfiable_msg_00;
      std::string unbounded_msg      = unbounded_msg_00;
      std::string unsatorunbnd_msg   = unsatorunbnd_msg_00;
      std::string unknown_msg        = unknown_msg_00;
      std::string error_msg          = error_msg_00;
      std::string search_complete_msg= search_complete_msg_00;
    } _opt;

  public:
     ~Solns2Out();
    Solns2Out(std::ostream& os, std::ostream& log, const std::string& stdlibDir);
    
     bool processOption(int& i, std::vector<std::string>& argv);
     void printHelp(std::ostream& );

    /// The output model (~.ozn) can be passed in 1 way in this base class:
    /// passing Env* containing output()
     bool initFromEnv(Env* pE);

    /// Then, variable assignments can be passed either as text
    /// or put directly into envi()->output() ( latter done externally
    /// by e.g. SolverInstance::assignSolutionToOutput() )
    /// In the 1st case, (part of) the assignment text is passed as follows,
    /// original end-of-lines need to be there as well
     bool feedRawDataChunk( const char* );
    
    SolverInstance::Status status = SolverInstance::UNKNOWN;
    bool fStatusPrinted = false;
    /// Should be called when entering new solution into the output model.
    /// Default assignSolutionToOutput() does it by using findOutputVar().
    void declNewOutput();

    /// This can be used by assignSolutionToOutput()    
    DE& findOutputVar( ASTString );
    
    /// In the other case,
    /// the evaluation procedures print output/status to os
    /// returning false means need to stop (error/ too many solutions)
    /// Solution validation here   TODO
    /// Note that --canonicalize delays output
    /// until ... exit, eof,  ??   TODO
    /// These functions should only be called explicitly
    /// from SolverInstance
     bool evalOutput( const std::string& s_ExtraInfo = "" );
    /// This means the solver exits
     bool evalStatus(SolverInstance::Status status);

     void printStatistics(std::ostream& );
    
     Env* getEnv() const { return pEnv; }
     Model* getModel() const { assert(getEnv()->output()); return getEnv()->output(); }
     /// Get the primary output stream
     /// First call restores stdout
     std::ostream& getOutput();
     /// Get the secondary output stream
     std::ostream& getLog();

  private:
    Timer starttime;

    std::unique_ptr<std::ostream> pOut;  // file output
    std::unique_ptr<std::ostream> pOfs_non_canon;
    std::unique_ptr<std::ostream> pOfs_raw;
    int nSolns = 0;
    std::set<std::string> sSolsCanon;
    std::string line_part;   // non-finished line from last chunk

    /// Initialise from ozn file
    void initFromOzn(const std::string& filename);
  protected:
    std::ostream& os;
    std::ostream& log;
    std::vector<std::string> includePaths;
    std::string stdlibDir;
    
    // Basically open output
     void init();
    void createOutputMap();
    std::map<std::string, SolverInstance::Status> mapInputStatus;
    void createInputMap();
    void restoreDefaults();
    /// Parsing fznsolver's complete raw text output
    void parseAssignments( std::string& );
    /// Checking solution against checker model
    void checkSolution( std::ostream& os );
     bool __evalOutput( std::ostream& os );
     bool __evalOutputFinal( bool flag_flush );
     bool __evalStatusMsg(SolverInstance::Status status);
    
  };

  // Passthrough Solns2Out class
  class Solns2Log {
  private:
    std::ostream& _log;
    std::ostream& _err_log;
  public:
    Solns2Log(std::ostream& log, std::ostream& errLog) : _log(log), _err_log(errLog) {}
    bool feedRawDataChunk(const char* data) { _log << data << std::flush; return true; }
    std::ostream& getLog(void) { return _err_log; }
  };
  

}

#endif  // __MINIZINC_SOLNS2OUT_H__

