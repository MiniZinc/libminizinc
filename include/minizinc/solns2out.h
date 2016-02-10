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

using namespace std;

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
    unique_ptr<Env> pEnv_guard;
    Env* pEnv=0;
    Model* pOutput=0;

    typedef pair<VarDecl*, KeepAlive> DE;
    ASTStringMap<DE>::t declmap;
    Expression* outputExpr = NULL;
    bool fNewSol2Print = false;     // should be set for evalOutput to work
    
  public:
    string solution;
    string comments;
    int nLinesIgnore = 0;
    
    struct Options {
      string flag_output_file;
      bool flag_output_comments = true;
      bool flag_output_flush = true;
      bool flag_output_time = false;
      int flag_ignore_lines = 0;
      bool flag_canonicalize = 0;
      string flag_output_noncanonical;
      string flag_output_raw;
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
      string solution_separator = solution_separator_00;
      string solution_comma     = "";
      string unsatisfiable_msg  = unsatisfiable_msg_00;
      string unbounded_msg      = unbounded_msg_00;
      string unsatorunbnd_msg   = unsatorunbnd_msg_00;
      string unknown_msg        = unknown_msg_00;
      string error_msg          = error_msg_00;
      string search_complete_msg= search_complete_msg_00;
    } _opt;
    
  public:
    virtual ~Solns2Out();
    
    virtual bool processOption(int& i, const int argc, const char** argv);
    virtual void printHelp(ostream& );
    
    /// The output model (~.ozn) can be passed in 1 way in this base class:
    /// passing Env* containing output()
    virtual bool initFromEnv(Env* pE);
    
    /// Then, variable assignments can be passed either as text
    /// or put directly into envi()->output() ( latter done externally
    /// by e.g. SolverInstance::assignSolutionToOutput() )
    /// In the 1st case, (part of) the assignment text is passed as follows
    virtual bool feedRawDataChunk( const char* );
    
    SolverInstance::Status status = SolverInstance::UNKNOWN;
    bool fStatusPrinted = false;
    /// Should be called when entering new solution into the output model.
    /// Default assignSolutionToOutput() does it by using find findOutputVar().
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
    virtual bool evalOutput();
    /// This means the solver exits
    virtual bool evalStatus(SolverInstance::Status status);

    virtual void printStatistics(ostream& );
    
    virtual Env* getEnv() const { assert(pEnv); return pEnv; }
    virtual Model* getModel() const { assert(getEnv()->output()); return getEnv()->output(); }
    
  private:
    Timer starttime;

    unique_ptr<ostream> pOut;  // file output
    unique_ptr<ostream> pOfs_non_canon;
    unique_ptr<ostream> pOfs_raw;
    set<string> sSolsCanon;
    string line_part;   // non-finished line from last chunk

  protected:
    std::vector<string> includePaths;
    
    // Basically open output
    virtual void init();
    void createOutputMap();
    std::map<string, SolverInstance::Status> mapInputStatus;
    void createInputMap();
    void restoreDefaults();
    /// Parsing fznsolver's complete raw text output
    void parseAssignments( string& );
    
    virtual bool __evalOutput(ostream& os, bool flag_flush);
    virtual bool __evalOutputFinal( bool flag_flush );
    virtual bool __evalStatusMsg(SolverInstance::Status status);
    
    virtual ostream& getOutput();
  };

}

#endif  // __MINIZINC_SOLNS2OUT_H__

