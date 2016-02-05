/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was ! distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <minizinc/solns2out.h>
#include <fstream>

using namespace std;
using namespace MiniZinc;


void Solns2Out::printHelp(ostream& os)
{
  os
  << std::endl
  << "Solution output options:" << std::endl
  << "  -o <file>, --output-to-file <file>\n    Filename for generated output." << std::endl
  << "  --no-output-comments\n    Do not print comments in the FlatZinc solution stream." << std::endl
  << "  --output-time\n    Print timing information in the FlatZinc solution stream." << std::endl
  << "  --no-flush-output\n    Don't flush output stream after every line." << std::endl
  << "  -i <n>, --ignore-lines <n>, --ignore-leading-lines <n>\n    Ignore the first <n> lines in the FlatZinc solution stream." << std::endl
  << "  --soln-sep <s>, --soln-separator <s>, --solution-separator <s>\n    Specify the string printed after each solution.\n    The default is to use the same as FlatZinc,\n    \"----------\"." << std::endl
  << "  --soln-comma <s>, --solution-comma <s>\n    Specify the string used to separate solutions.\n    The default is the empty string." << std::endl
  << "  --unsat-msg <msg>, --unsatisfiable-msg <msg>\n    Specify the message to print if the model instance is\n    unsatisfiable.\n    The default is to print \"=====UNSATISFIABLE=====\"." << std::endl
  << "  --unbounded-msg <msg>\n    Specify the message to print if the objective of the\n    model instance is unbounded.\n    The default is to print \"=====UNBOUNDED=====\"." << std::endl
  << "  --unsatorunbnd-msg <msg>\n    Specify the message to print if the objective of the\n    model instance is unsat or unbounded.\n    The default is to print \"=====UNSATorUNBOUNDED=====\"." << std::endl
  << "  --unknown-msg <msg>\n    Specify the message to print if search terminates without\n    any result.  The default is to print \"=====UNKNOWN=====\"." << std::endl
  << "  --error-msg <msg>\n    Specify the message to print if error occurs.\n    The default is to print \"=====ERROR=====\"." << std::endl
  << "  --search-complete-msg <msg>\n    Specify the message to print if search terminates having\n    explored the entire search space.\n    The default is to print \"==========\"." << std::endl
  << "  -c, --canonicalize\n    Canonicalize the output solution stream.\n"
     "         Note that this option prevents incremental printing of solutions."
  << "  --output-non-canonical <file>\n    Immediate solution output file in case of canonicalization.\n"
  << "  --number-output <n>\n    Maximal number of different solutions printed." << std::endl
  ;
}

bool Solns2Out::processOption(int& i, const int argc, const char** argv)
{
  CLOParser cop( i, argc, argv );
  
  if ( cop.getOption( "-o --output-to-file", &_opt.flag_output_file) ) {
  } else if ( cop.getOption( "--no-flush-output" ) ) {
    _opt.flag_output_flush = false;
  } else if ( cop.getOption( "--no-output-comments" ) ) {
    _opt.flag_output_comments = false;
  } else if ( cop.getOption( "-i --ignore-lines --ignore-leading-lines", &_opt.flag_ignore_lines ) ) {
  } else if ( cop.getOption( "--output-time" ) ) {
    _opt.flag_output_time = true;
  } else if ( cop.getOption( "--soln-sep --soln-separator --solution-separator", &_opt.solution_separator ) ) {
  } else if ( cop.getOption( "--soln-comma --solution-comma", &_opt.solution_comma ) ) {
  } else if ( cop.getOption( "--unsat-msg --unsatisfiable-msg", &_opt.unsatisfiable_msg ) ) {
  } else if ( cop.getOption( "--unbounded-msg", &_opt.unbounded_msg ) ) {
  } else if ( cop.getOption( "--unsatorunbnd-msg", &_opt.unsatorunbnd_msg ) ) {
  } else if ( cop.getOption( "--unknown-msg", &_opt.unknown_msg ) ) {
  } else if ( cop.getOption( "--error-msg", &_opt.error_msg ) ) {
  } else if ( cop.getOption( "--search-complete-msg", &_opt.search_complete_msg ) ) {
  } else if ( cop.getOption( "-c, --canonicalize") ) {
    _opt.flag_canonicalize, true;
  } else if ( cop.getOption( "--output-non-canonical", &_opt.flag_output_noncanonical) ) {
  } else if ( cop.getOption( "--number-output", &_opt.flag_number_output ) ) {
  } else {
    return false;
  }
  return true;
}

bool Solns2Out::parseOzn(string& fileOzn)
{
  std::vector<string> filenames( 1, fileOzn );
  // If set before:
  
  if (_opt.std_lib_dir.empty())
    if (char* MZNSTDLIBDIR = getenv("MZN_STDLIB_DIR")) {
      _opt.std_lib_dir = string(MZNSTDLIBDIR);
    }

  if (_opt.std_lib_dir.empty()) {
    std::string mypath = FileUtils::progpath();
    if (!mypath.empty()) {
      if (FileUtils::file_exists(mypath+"/share/minizinc/std/builtins.mzn")) {
        _opt.std_lib_dir = mypath+"/share/minizinc";
      } else if (FileUtils::file_exists(mypath+"/../share/minizinc/std/builtins.mzn")) {
        _opt.std_lib_dir = mypath+"/../share/minizinc";
      } else if (FileUtils::file_exists(mypath+"/../../share/minizinc/std/builtins.mzn")) {
        _opt.std_lib_dir = mypath+"/../../share/minizinc";
      }
    }
  }

  if (_opt.std_lib_dir.empty()) {
    std::cerr << "Error: Solns2Out: unknown minizinc standard library directory.\n"
      << "Specify --stdlib-dir on the command line or set the\n"
      << "MZN_STDLIB_DIR environment variable.\n";
    std::exit(EXIT_FAILURE);
  }

  includePaths.push_back(_opt.std_lib_dir+"/std/");

  for (unsigned int i=0; i<includePaths.size(); i++) {
    if (!FileUtils::directory_exists(includePaths[i])) {
      std::cerr << "Cannot access include directory " << includePaths[i] << "\n";
      std::exit(EXIT_FAILURE);
    }
  }

  {
    if (pOutput = parse(filenames, std::vector<std::string>(), includePaths, false, false, false,
                               std::cerr)) {
      std::vector<TypeError> typeErrors;
      pEnv = new Env(pOutput);
      MZN_ASSERT_HARD_MSG( pEnv, "Solns2Out: could not allocate Env" );
      pEnv_guard.reset( pEnv );
      MiniZinc::typecheck(*pEnv,pOutput,typeErrors);
      MiniZinc::registerBuiltins(*pEnv,pOutput);
      return true;
    }
  }
  
  return false;
}

void Solns2Out::createOutputMap() {
  for (unsigned int i=0; i<getModel()->size(); i++) {
    if (VarDeclI* vdi = (*getModel())[i]->dyn_cast<VarDeclI>()) {
      declmap.insert(pair<ASTString,DE>(vdi->e()->id()->v(),DE(vdi->e(),vdi->e()->e())));
    } else if (OutputI* oi = (*getModel())[i]->dyn_cast<OutputI>()) {
      outputExpr = oi->e();
    }
  }
}

Solns2Out::DE& Solns2Out::findOutputVar( ASTString id ) {
  if ( declmap.empty() )
    createOutputMap();
  auto it = declmap.find( id );
  MZN_ASSERT_HARD_MSG( declmap.end()!=it,
                       "Error: unexpected id in output: " << id );
  return it->second;
}

void Solns2Out::restoreDefaults() {
  for (unsigned int i=0; i<getModel()->size(); i++) {
    if (VarDeclI* vdi = (*getModel())[i]->dyn_cast<VarDeclI>()) {
      auto& de = findOutputVar(vdi->e()->id()->v());
      vdi->e()->e(de.second());
      vdi->e()->evaluated(false);
    }
  }
}

void Solns2Out::parseAssignments(string& solution) {
  unique_ptr<Model> sm(
    parseFromString(solution, "solution received from solver", includePaths, true, false, false, cerr) );
  MZN_ASSERT_HARD_MSG( sm.get(), "Solns2Out: could not parse solution" );
  solution = "";
  for (unsigned int i=0; i<sm->size(); i++) {
    if (AssignI* ai = (*sm)[i]->dyn_cast<AssignI>()) {
      auto& de = findOutputVar(ai->id());
      ai->e()->type(de.first->type());
      ai->decl(de.first);
      typecheck(*pEnv, getModel(), ai);
      if (Call* c = ai->e()->dyn_cast<Call>()) {
        // This is an arrayXd call, make sure we get the right builtin
        assert(c->args()[c->args().size()-1]->isa<ArrayLit>());
        for (unsigned int i=0; i<c->args().size(); i++)
          c->args()[i]->type(Type::parsetint());
        c->args()[c->args().size()-1]->type(de.first->type());
        c->decl(getModel()->matchFn(pEnv->envi(), c));
      }
      de.first->e(ai->e());
    }
  }
}

bool Solns2Out::evalOutput() {
  ostringstream oss;
  if (!__evalOutput( oss, false ))
    return false;
  auto res = sSolsCanon.insert( oss.str() );
  if ( !res.second )            // repeated solution
    return true;
  if ( _opt.flag_canonicalize ) {
    if ( ofs_non_canon.good() && ofs_non_canon.is_open() ) {
      ofs_non_canon << oss.str();
      if ( _opt.flag_output_flush )
        ofs_non_canon.flush();
    }
  } else {
    if ( _opt.solution_comma.size() && sSolsCanon.size()>1 )
      getOutput() << _opt.solution_comma << '\n';
    getOutput() << oss.str();
    if ( _opt.flag_output_flush )
      getOutput().flush();
  }
  restoreDefaults();
  return true;
}

bool Solns2Out::__evalOutput( ostream& fout, bool flag_output_flush ) {
  if ( 0!=outputExpr ) {
//     GCLock lock;
//     ArrayLit* al = eval_array_lit(pEnv->envi(),outputExpr);
//     std::string os;
//     for (unsigned int i=0; i<al->v().size(); i++) {
//       std::string s = eval_string(pEnv->envi(),al->v()[i]);
//       if (!s.empty()) {
//         os = s;
//         fout << os;
//         if (flag_output_flush)
//           fout.flush();
//       }
//     }
//     if (!os.empty() && os[os.size()-1] != '\n') {
//       fout << '\n';
//       if (flag_output_flush)
//         fout.flush();
//     }
    pEnv->envi().evalOutput( fout );
  }
  fout << comments;
  fout << _opt.solution_separator << '\n';
  if (flag_output_flush)
    fout.flush();
  comments = "";
  return true;
}

bool Solns2Out::evalStatus( SolverInstance::Status status ) {
  if ( _opt.flag_canonicalize )
    __evalOutputFinal( _opt.flag_output_flush );
  __evalStatusMsg( status );
  fStatusPrinted = 1;
  return true;
}

bool Solns2Out::__evalOutputFinal( bool ) {
  /// Print the canonical list
  for ( auto& sol : sSolsCanon ) {
    if ( _opt.solution_comma.size() && &sol != &*sSolsCanon.begin() )
      getOutput() << _opt.solution_comma << '\n';
    getOutput() << sol;
  }
  return true;
}

bool Solns2Out::__evalStatusMsg( SolverInstance::Status status ) {
  std::map<SolverInstance::Status, string> stat2msg;
  stat2msg[ SolverInstance::OPT ] = _opt.search_complete_msg;
  stat2msg[ SolverInstance::UNSAT ] = _opt.unsatisfiable_msg;
  stat2msg[ SolverInstance::UNBND ] = _opt.unbounded_msg;
  stat2msg[ SolverInstance::UNSATorUNBND ] = _opt.unsatorunbnd_msg;
  stat2msg[ SolverInstance::UNKNOWN ] = _opt.unknown_msg;
  stat2msg[ SolverInstance::ERROR ] = _opt.error_msg;
  auto it=stat2msg.find(status);
  if ( stat2msg.end()!=it ) {
    getOutput() << it->second << '\n';
    getOutput() << comments;
    if ( _opt.flag_output_flush )
      getOutput().flush();
    Solns2Out::status = status;
  }
  else {
    getOutput() << comments;
    if ( _opt.flag_output_flush )
      getOutput().flush();
    MZN_ASSERT_HARD_MSG( SolverInstance::SAT==status,    // which is ignored
                         "Solns2Out: undefined solution status code " << status );
    Solns2Out::status = SolverInstance::SAT;
  }
  comments = "";
}

void Solns2Out::init() {
  /// Main output file
  if ( 0==pOut ) {
    if ( _opt.flag_output_file.size() ) {
      ofs.open( _opt.flag_output_file );
      if ( ofs ) {
        pOut=&ofs;
      } else
        checkIOStatus( false, _opt.flag_output_file, 0);
    }
    pOut = &cout;
  }
  /// Non-canonical output
  if ( _opt.flag_canonicalize && _opt.flag_output_noncanonical.size() ) {
    ofs_non_canon.open( _opt.flag_output_noncanonical );
    checkIOStatus( ofs_non_canon.good(), _opt.flag_output_file, 0);
  }
  /// Assume all options are set before
  nLinesIgnore = _opt.flag_ignore_lines;
}

ostream& Solns2Out::getOutput() {
  return ( 0!=pOut && pOut->good() ) ? *pOut : cout;
}

bool Solns2Out::feedRawDataChunk(const char* data) {
  istringstream solstream( data );
  while (solstream.good()) {
    string line;
    getline(solstream, line);
    if ( nLinesIgnore > 0 ) {
      --nLinesIgnore;
      continue;
    }
    if ( mapInputStatus.empty() )
      createInputMap();
    auto it = mapInputStatus.find( line );
    if ( mapInputStatus.end()!=it ) {
      if ( SolverInstance::SAT==it->second ) {
        parseAssignments( solution );
        evalOutput();
      } else {
        evalStatus( it->second );
      }
      if (_opt.flag_output_time)
        getOutput() << "% time elapsed: " << stoptime(starttime) << "\n";
    } else {
      solution += line + '\n';
      size_t comment_pos = line.find('%');
      if (comment_pos != string::npos) {
        comments += line.substr(comment_pos);
        comments += "\n";
      }
    }
  }
  return true;
}

void Solns2Out::createInputMap() {
  mapInputStatus[ _opt.search_complete_msg_00 ] = SolverInstance::OPT;
  mapInputStatus[ _opt.solution_separator_00 ] = SolverInstance::SAT;
  mapInputStatus[ _opt.unsatisfiable_msg_00 ] = SolverInstance::UNSAT;
  mapInputStatus[ _opt.unbounded_msg_00 ] = SolverInstance::UNBND;
  mapInputStatus[ _opt.unsatorunbnd_msg_00 ] = SolverInstance::UNSATorUNBND;
  mapInputStatus[ _opt.unknown_msg_00 ] = SolverInstance::UNKNOWN;
  mapInputStatus[ _opt.error_msg ] = SolverInstance::ERROR;
}

void Solns2Out::printStatistics(ostream&)
{
}
