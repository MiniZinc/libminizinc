/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was ! distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* A basic mzn2fzn wrapper, can be used as a plugin
 */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <minizinc/flattener.hh>
#include <minizinc/pathfileprinter.hh>
#include <fstream>

#ifdef HAS_GECODE
#include <minizinc/solvers/gecode_solverinstance.hh>
#endif

using namespace std;
using namespace MiniZinc;

void Flattener::printVersion(ostream& os)
{
  os << "MiniZinc to FlatZinc converter, version "
     << MZN_VERSION_MAJOR << "." << MZN_VERSION_MINOR << "." << MZN_VERSION_PATCH;
  if (!std::string(MZN_BUILD_REF).empty()) {
     os << ", build " << MZN_BUILD_REF;
  }
  os << std::endl;
  os << "Copyright (C) 2014-" << string(__DATE__).substr(7, 4)
     << " Monash University, NICTA, Data61" << std::endl;
}

void Flattener::printHelp(ostream& os)
{
  os
  << std::endl
  << "Flattener input options:" << std::endl
  << "  --ignore-stdlib\n    Ignore the standard libraries stdlib.mzn and builtins.mzn" << std::endl
  << "  --instance-check-only\n    Check the model instance (including data) for errors, but do not\n    convert to FlatZinc." << std::endl
  << "  -e, --model-check-only\n    Check the model (without requiring data) for errors, but do not\n    convert to FlatZinc." << std::endl
  << "  --model-interface-only\n    Only extract parameters and output variables." << std::endl
  << "  --model-types-only\n    Only output variable (enum) type information." << std::endl
  << "  --no-optimize\n    Do not optimize the FlatZinc" << std::endl
  << "  --no-chain-compression\n    Do not simplify chains of implication constraints." << std::endl
  << "  -d <file>, --data <file>\n    File named <file> contains data used by the model." << std::endl
  << "  -D <data>, --cmdline-data <data>\n    Include the given data assignment in the model." << std::endl
  << "  --stdlib-dir <dir>\n    Path to MiniZinc standard library directory" << std::endl
  << "  -G <dir>, --globals-dir <dir>, --mzn-globals-dir <dir>\n    Search for included globals in <stdlib>/<dir>." << std::endl
  << "  -, --input-from-stdin\n    Read problem from standard input" << std::endl
  << "  -I <dir>, --search-dir <dir>\n    Additionally search for included files in <dir>." << std::endl
  << "  -D \"fMIPdomains=true\"\n    Switch on MIPDomain Unification" << std::endl
  << "  --MIPDMaxIntvEE <n>\n    MIPD: max integer domain subinterval length to enforce equality encoding, default " << opt_MIPDmaxIntvEE << std::endl
  << "  --MIPDMaxDensEE <n>\n    MIPD: max domain cardinality to N subintervals ratio\n    to enforce equality encoding, default " << opt_MIPDmaxDensEE << ", either condition triggers" << std::endl
  << "  --only-range-domains\n    When no MIPdomains: all domains contiguous, holes replaced by inequalities" << std::endl
  << "  --allow-multiple-assignments\n    Allow multiple assignments to the same variable (e.g. in dzn)" << std::endl
  << "  --no-half-reifications\n    Only use fully reified constraints, even when a half reified constraint is defined." << std::endl
  << "  --compile-solution-checker <file>.mzc.mzn\n    Compile solution checker model" << std::endl
  << std::endl
  << "Flattener two-pass options:" << std::endl
  << "  --two-pass\n    Flatten twice to make better flattening decisions for the target" << std::endl
#ifdef HAS_GECODE
  << "  --use-gecode\n    Perform root-node-propagation with Gecode (adds --two-pass)" << std::endl
  << "  --shave\n    Probe bounds of all variables at the root node (adds --use-gecode)" << std::endl
  << "  --sac\n    Probe values of all variables at the root node (adds --use-gecode)" << std::endl
  << "  --pre-passes <n>\n    Number of times to apply shave/sac pass (0 = fixed-point, 1 = default)" << std::endl
#endif
  << "  -O<n>\n    Two-pass optimisation levels:" << std::endl
  << "    -O0:    Disable optimize (--no-optimize)  -O1:    Single pass (default)" << std::endl
  << "    -O2:    Same as: --two-pass"
#ifdef HAS_GECODE
  <<                                "               -O3:    Same as: --use-gecode" << std::endl
  << "    -O4:    Same as: --shave                  -O5:    Same as: --sac" << std::endl
#else
  << "\n    -O3,4,5:    Disabled [Requires MiniZinc with built-in Gecode support]" << std::endl
#endif
  << "  -g\n    Debug mode: Forces -O0 and records all domain changes as constraints instead of applying them" << std::endl
  << std::endl;
  os
  << "Flattener output options:" << std::endl
  << "  --no-output-ozn, -O-\n    Do not output ozn file" << std::endl
  << "  --output-base <name>\n    Base name for output files" << std::endl
  << ( fOutputByDefault ? "  -o <file>, --fzn <file>, --output-to-file <file>, --output-fzn-to-file <file>\n"
       : "  --fzn <file>, --output-fzn-to-file <file>\n" )
  << "    Filename for generated FlatZinc output" << std::endl
  << "  -O, --ozn, --output-ozn-to-file <file>\n    Filename for model output specification (-O- for none)" << std::endl
  << "  --keep-paths\n    Don't remove path annotations from FlatZinc" << std::endl
  << "  --output-paths\n    Output a symbol table (.paths file)" << std::endl
  << "  --output-paths-to-file <file>\n    Output a symbol table (.paths file) to <file>" << std::endl
  << "  --output-to-stdout, --output-fzn-to-stdout\n    Print generated FlatZinc to standard output" << std::endl
  << "  --output-ozn-to-stdout\n    Print model output specification to standard output" << std::endl
  << "  --output-paths-to-stdout\n    Output symbol table to standard output" << std::endl
  << "  --output-mode <item|dzn|json>\n    Create output according to output item (default), or output compatible\n    with dzn or json format" << std::endl
  << "  --output-objective\n    Print value of objective function in dzn or json output" << std::endl
  << "  --output-output-item\n    Print the output item as a string in the dzn or json output" << std::endl
  << "  -Werror\n    Turn warnings into errors" << std::endl
  ;
}

bool Flattener::processOption(int& i, std::vector<std::string>& argv)
{
  CLOParser cop( i, argv );
  string buffer;
  
  if ( cop.getOption( "-I --search-dir", &buffer ) ) {
    includePaths.push_back(buffer+string("/"));
  } else if ( cop.getOption( "--ignore-stdlib" ) ) {
    flag_ignoreStdlib = true;
  } else if ( cop.getOption( "--no-typecheck") ) {
    flag_typecheck = false;
  } else if ( cop.getOption( "--instance-check-only") ) {
    flag_instance_check_only = true;
  } else if ( cop.getOption( "-e --model-check-only") ) {
    flag_model_check_only = true;
  } else if ( cop.getOption( "--model-interface-only") ) {
    flag_model_interface_only = true;
  } else if ( cop.getOption( "--model-types-only") ) {
    flag_model_types_only = true;
  } else if ( cop.getOption( "-v --verbose") ) {
    flag_verbose = true;
  } else if (string(argv[i])==string("--newfzn")) {
    flag_newfzn = true;
  } else if ( cop.getOption( "--no-optimize --no-optimise") ) {
    flag_optimize = false;
  } else if ( cop.getOption( "--no-chain-compression") ) {
    flag_chain_compression = false;
  } else if ( cop.getOption( "--no-output-ozn -O-") ) {
    flag_no_output_ozn = true;
  } else if ( cop.getOption( "--output-base", &flag_output_base ) ) {
  } else if ( cop.getOption(
    fOutputByDefault ?
      "-o --fzn --output-to-file --output-fzn-to-file"
      : "--fzn --output-fzn-to-file", &flag_output_fzn) ) {
  } else if ( cop.getOption( "--output-paths-to-file", &flag_output_paths) ) {
    fopts.collect_mzn_paths = true;
  } else if ( cop.getOption( "--output-paths") ) {
    fopts.collect_mzn_paths = true;
  } else if ( cop.getOption( "--output-to-stdout --output-fzn-to-stdout" ) ) {
    flag_output_fzn_stdout = true;
  } else if ( cop.getOption( "--output-ozn-to-stdout" ) ) {
    flag_output_ozn_stdout = true;
  } else if ( cop.getOption( "--output-paths-to-stdout" ) ) {
    fopts.collect_mzn_paths = true;
    flag_output_paths_stdout = true;
  } else if ( cop.getOption( "--output-mode", &buffer ) ) {
    if (buffer == "dzn") {
      flag_output_mode = FlatteningOptions::OUTPUT_DZN;
    } else if (buffer == "json") {
      flag_output_mode = FlatteningOptions::OUTPUT_JSON;
    } else if (buffer == "item") {
      flag_output_mode = FlatteningOptions::OUTPUT_ITEM;
    } else {
      return false;
    }
  } else if ( cop.getOption( "--output-objective" ) ) {
    flag_output_objective = true;
  } else if ( cop.getOption( "--output-output-item" ) ) {
    flag_output_output_item = true;
  } else if ( cop.getOption( "- --input-from-stdin" ) ) {
      flag_stdinInput = true;
  } else if ( cop.getOption( "-d --data", &buffer ) ) {
    if ( buffer.length()<=4 ||
         buffer.substr(buffer.length()-4,string::npos) != ".dzn")
      return false;
    datafiles.push_back(buffer);
  } else if ( cop.getOption( "--stdlib-dir", &std_lib_dir ) ) {
  } else if ( cop.getOption( "-G --globals-dir --mzn-globals-dir", &globals_dir ) ) {
  } else if ( cop.getOption( "-D --cmdline-data", &buffer)) {
    datafiles.push_back("cmd:/"+buffer);
  } else if ( cop.getOption( "--allow-unbounded-vars" ) ) {
    flag_allow_unbounded_vars = true;
  } else if ( cop.getOption( "--only-range-domains" ) ) {
    flag_only_range_domains = true;
  } else if ( cop.getOption( "--no-MIPdomains" ) ) {   // internal
    flag_noMIPdomains = true;
  } else if ( cop.getOption( "--MIPDMaxIntvEE", &opt_MIPDmaxIntvEE ) ) {
  } else if ( cop.getOption( "--MIPDMaxDensEE", &opt_MIPDmaxDensEE ) ) {
  } else if ( cop.getOption( "-Werror" ) ) {
    flag_werror = true;
  } else if (string(argv[i])=="--use-gecode") {
#ifdef HAS_GECODE
    flag_two_pass = true;
    flag_gecode = true;
#else
    log << "warning: Gecode not available. Ignoring '--use-gecode'\n";
#endif
  } else if (string(argv[i])=="--sac") {
#ifdef HAS_GECODE
    flag_two_pass = true;
    flag_gecode = true;
    flag_sac = true;
#else
    log << "warning: Gecode not available. Ignoring '--sac'\n";
#endif

  } else if (string(argv[i])=="--shave") {
#ifdef HAS_GECODE
    flag_two_pass = true;
    flag_gecode = true;
    flag_shave = true;
#else
    log << "warning: Gecode not available. Ignoring '--shave'\n";
#endif
  } else if (string(argv[i])=="--two-pass") {
    flag_two_pass = true;
  } else if (string(argv[i])=="--npass") {
    i++;
    if (i==argv.size()) return false;
    log << "warning: --npass option is deprecated --two-pass\n";
    int passes = atoi(argv[i].c_str());
    if(passes == 1) flag_two_pass = false;
    else if(passes == 2) flag_two_pass = true;
  } else if (string(argv[i])=="--pre-passes") {
    i++;
    if (i==argv.size()) return false;
    int passes = atoi(argv[i].c_str());
    if(passes >= 0) {
      flag_pre_passes = static_cast<unsigned int>(passes);
    }
  } else if (string(argv[i])=="-O0") {
    flag_optimize = false;
  } else if (string(argv[i])=="-O1") {
    // Default settings
  } else if (string(argv[i])=="-O2") {
    flag_two_pass = true;
#ifdef HAS_GECODE
  } else if (string(argv[i])=="-O3") {
    flag_two_pass = true;
    flag_gecode = true;
  } else if (string(argv[i])=="-O4") {
    flag_two_pass = true;
    flag_gecode = true;
    flag_shave = true;
  } else if (string(argv[i])=="-O5") {
    flag_two_pass = true;
    flag_gecode = true;
    flag_sac = true;
#else
  } else if (string(argv[i])=="-O3" || string(argv[i])=="-O4" || string(argv[i])=="-O5") {
    log << "% Warning: This compiler does not have Gecode builtin, cannot process -O3,-O4,-O5.\n";
    return false;
#endif
    // ozn options must be after the -O<n> optimisation options
  } else if ( cop.getOption( "-O --ozn --output-ozn-to-file", &flag_output_ozn) ) {
  } else if (string(argv[i])=="-g") {
    flag_optimize = false;
    flag_two_pass = false;
    flag_gecode = false;
    flag_shave = false;
    flag_sac = false;
    fopts.record_domain_changes = true;
  } else if (string(argv[i])=="--keep-paths") {
    flag_keep_mzn_paths = true;
    fopts.collect_mzn_paths = true;
  } else if (string(argv[i])=="--only-toplevel-presolve") {
    fopts.only_toplevel_paths = true;
  } else if ( cop.getOption( "--allow-multiple-assignments" ) ) {
    flag_allow_multi_assign = true;
  } else if ( cop.getOption( "--no-half-reifications" ) ) {
    fopts.enable_imp = false;
  } else if (string(argv[i])=="--input-is-flatzinc") {
    is_flatzinc = true;
  } else if ( cop.getOption( "--compile-solution-checker", &buffer) ) {
    if (buffer.length()>=8 && buffer.substr(buffer.length()-8,string::npos) == ".mzc.mzn") {
      flag_compile_solution_check_model = true;
      flag_model_check_only = true;
      filenames.push_back(buffer);
    } else {
      log << "Error: solution checker model must have extension .mzc.mzn" << std::endl;
      return false;
    }
  } else {
    std::string input_file(argv[i]);
    if (input_file.length()<=4) {
      return false;
    }
    size_t last_dot = input_file.find_last_of('.');
    if (last_dot == string::npos) {
      return false;
    }
    std::string extension = input_file.substr(last_dot,string::npos);
    if ( extension == ".mzc" || (input_file.length()>=8 && input_file.substr(input_file.length()-8,string::npos) == ".mzc.mzn") ) {
      flag_solution_check_model = input_file;
    } else if (extension == ".mzn" || extension == ".fzn") {
      if ( extension == ".fzn" ) {
        is_flatzinc = true;
        if ( fOutputByDefault )        // mzn2fzn mode
          return false;
      }
      filenames.push_back(input_file);
    } else if (extension == ".dzn" || extension == ".json") {
      datafiles.push_back(input_file);
    } else {
      if ( fOutputByDefault )
        log << "Error: cannot handle file extension " << extension << "." << std::endl;
      return false;
    }
  }
  return true;
}

Flattener::Flattener(std::ostream& os_, std::ostream& log_, const std::string& stdlibDir)
  : os(os_), log(log_), std_lib_dir(stdlibDir) {}

Flattener::~Flattener()
{
  if (pEnv.get()) {      // ??? TODO
    if(is_flatzinc) {
      pEnv->swap();
    }
  }
}

Env* Flattener::multiPassFlatten(const vector<unique_ptr<Pass> >& passes) {
  Env& e = *getEnv();

  Env* pre_env = &e;
  size_t npasses = passes.size();
  pre_env->envi().final_pass_no = static_cast<unsigned int>(npasses);
  Timer starttime;
  bool verbose = false;
  for(unsigned int i=0; i<passes.size(); i++) {
    pre_env->envi().current_pass_no = i;
    if(verbose)
      log << "Start pass " << i << ":\n";

    Env* out_env = passes[i]->run(pre_env,log);
    if(out_env == nullptr) return nullptr;
    if(pre_env != &e && pre_env != out_env) {
      delete pre_env;
    }
    pre_env = out_env;

    if(verbose)
      log << "Finish pass " << i << ": " << starttime.stoptime() << "\n";
  }

  return pre_env;
}

class FlattenTimeout {
public:
  FlattenTimeout(unsigned long long int t) {
    GC::setTimeout(t);
  }
  ~FlattenTimeout(void) {
    GC::setTimeout(0);
  }
};

void Flattener::flatten(const std::string& modelString, const std::string& modelName)
{
  FlattenTimeout flatten_timeout(fopts.timeout);
  Timer flatten_time;
  starttime.reset();
  
  if (flag_verbose)
    printVersion(log);

  if (filenames.empty() && !flag_solution_check_model.empty()) {
    // Compile solution check model as if it were a normal model
    filenames.push_back(flag_solution_check_model);
    flag_solution_check_model = "";
  }
  
  if ( filenames.empty() && !flag_stdinInput && modelString.empty() ) {
    throw Error( "Error: no model file given." );
  }

  if (std_lib_dir=="") {
    throw Error("Error: unknown minizinc standard library directory.\n"
      "Specify --stdlib-dir on the command line or set the\n"
      "MZN_STDLIB_DIR environment variable.");
  }

  if (globals_dir != "") {
    includePaths.insert(includePaths.begin(), std_lib_dir+"/"+globals_dir+"/");
  }
  includePaths.push_back(std_lib_dir+"/std/");

  for (unsigned int i=0; i<includePaths.size(); i++) {
    if (!FileUtils::directory_exists(includePaths[i])) {
      throw Error("Cannot access include directory " + includePaths[i]);
    }
  }

  if (flag_output_base == "") {
    if (filenames.empty()) {
      flag_output_base = "mznout";
    } else {
      flag_output_base = filenames[0].substr(0,filenames[0].length()-4);
    }
  }
  
  if ( filenames.end() !=
      find( filenames.begin(), filenames.end(), flag_output_fzn ) ||
       datafiles.end() !=
      find( datafiles.begin(), datafiles.end(), flag_output_fzn ) ) {
    log << "  WARNING: fzn filename '" << flag_output_fzn
      << "' matches an input file, ignoring." << endl;
    flag_output_fzn = "";
  }
  if ( filenames.end() !=
      find( filenames.begin(), filenames.end(), flag_output_ozn ) ||
       datafiles.end() !=
      find( datafiles.begin(), datafiles.end(), flag_output_ozn ) ) {
    log << "  WARNING: ozn filename '" << flag_output_ozn
      << "' matches an input file, ignoring." << endl;
    flag_output_ozn = "";
  }
  
  if (fOutputByDefault) {
    if (flag_output_fzn == "") {
      flag_output_fzn = flag_output_base+".fzn";
    }
    if (flag_output_paths == "" && fopts.collect_mzn_paths) {
      flag_output_paths = flag_output_base+".paths";
    }
    if (flag_output_ozn == "" && ! flag_no_output_ozn) {
      flag_output_ozn = flag_output_base+".ozn";
    }
  }

  {
    std::stringstream errstream;

    Model* m;
    pEnv.reset(new Env(NULL,os,log));
    Env* env = getEnv();

    if (!flag_compile_solution_check_model && !flag_solution_check_model.empty()) {
      // Extract variables to check from solution check model
      if (flag_verbose)
        log << "Parsing solution checker model " << flag_solution_check_model << " ..." << endl;
      bool isCompressedChecker = flag_solution_check_model.size() >= 4 && flag_solution_check_model.substr(flag_solution_check_model.size()-4)==".mzc";
      std::vector<std::string> smm_model({flag_solution_check_model});
      Model* smm = parse(*env, smm_model, datafiles, "", "", includePaths, flag_ignoreStdlib, false, flag_verbose, errstream);
      if (flag_verbose)
        log << " done parsing (" << starttime.stoptime() << ")" << std::endl;
      if (smm) {
        log << errstream.str();
        errstream.str("");
        std::ostringstream smm_oss;
        Printer p(smm_oss,0,false);
        p.print(smm);
        Env smm_env(smm);
        GCLock lock;
        vector<TypeError> typeErrors;
        try {
          MiniZinc::typecheck(smm_env, smm, typeErrors, true, false, true);
          if (typeErrors.size() > 0) {
            if (!isCompressedChecker) {
              for (unsigned int i=0; i<typeErrors.size(); i++) {
                if (flag_verbose)
                  log << std::endl;
                log << typeErrors[i].loc() << ":" << std::endl;
                log << typeErrors[i].what() << ": " << typeErrors[i].msg() << std::endl;
              }
            }
            throw Error("multiple type errors");
          }
          for (unsigned int i=0; i<smm->size(); i++) {
            if (VarDeclI* vdi = (*smm)[i]->dyn_cast<VarDeclI>()) {
              if (vdi->e()->e()==NULL)
                env->envi().checkVars.push_back(vdi->e());
            }
          }
          smm->compact();
          std::string smm_compressed = FileUtils::encodeBase64(FileUtils::deflateString(smm_oss.str()));
          TypeInst* ti = new TypeInst(Location().introduce(),Type::parstring(),NULL);
          VarDecl* checkString = new VarDecl(Location().introduce(),ti,ASTString("_mzn_solution_checker"),new StringLit(Location().introduce(),smm_compressed));
          VarDeclI* checkStringI = new VarDeclI(Location().introduce(), checkString);
          env->output()->addItem(checkStringI);
        } catch (TypeError& e) {
          if (isCompressedChecker) {
            log << "Warning: type error in solution checker model\n";
          } else {
            throw;
          }
        }
      } else {
        if (isCompressedChecker) {
          log << "Warning: syntax error in solution checker model\n";
        } else {
          log << errstream.str();
          throw Error("parse error");
        }
      }
    }

    if (flag_compile_solution_check_model) {
      if (!modelString.empty()) {
        throw Error("Cannot compile solution checker model with additional model inputs.");
      }
      if (flag_stdinInput) {
        throw Error("Cannot compile solution checker model with additional model from standard input.");
      }
      if (filenames.size() != 1) {
        throw Error("Cannot compile solution checker model with more than one model given.");
      }
    }
    
    if (!flag_solution_check_model.empty() && filenames.size()==0) {
      throw Error("Cannot run solution checker without model.");
    }
    
    std::string modelText = modelString;
    if (flag_stdinInput) {
      std::string input = std::string(istreambuf_iterator<char>(std::cin), istreambuf_iterator<char>());
      modelText += input;
    }
    
    if (flag_verbose) {
      log << "Parsing file(s) ";
      for ( int i=0; i<filenames.size(); ++i )
        log << (i==0 ? "" : ", '") << filenames[i] << '\'';
      for ( const auto& sFln: datafiles )
        log << ", '" << sFln << '\'';
      log << " ..." << std::endl;
    }
    errstream.str("");
    m = parse(*env, filenames, datafiles, modelText, modelName.empty() ? "stdin" : modelName, includePaths, flag_ignoreStdlib, false, flag_verbose, errstream);
    if (globals_dir != "") {
      includePaths.erase(includePaths.begin());
    }
    if (m==NULL)
      throw Error(errstream.str());
    log << errstream.str();
    env->model(m);
    if (flag_typecheck) {
      if (flag_verbose)
        log << " done parsing (" << starttime.stoptime() << ")" << std::endl;

      if (flag_instance_check_only || flag_model_check_only ||
          flag_model_interface_only || flag_model_types_only ) {
        std::ostringstream compiledSolutionCheckModel;
        if (flag_compile_solution_check_model) {
          Printer p(compiledSolutionCheckModel,0);
          p.print(m);
        }
        GCLock lock;
        vector<TypeError> typeErrors;
        MiniZinc::typecheck(*env, m, typeErrors, flag_model_types_only || flag_model_interface_only || flag_model_check_only, flag_allow_multi_assign);
        if (typeErrors.size() > 0) {
          for (unsigned int i=0; i<typeErrors.size(); i++) {
            if (flag_verbose)
              log << std::endl;
            log << typeErrors[i].loc() << ":" << std::endl;
            log << typeErrors[i].what() << ": " << typeErrors[i].msg() << std::endl;
          }
          throw Error("multiple type errors");
        }
        if (flag_model_interface_only) {
          MiniZinc::output_model_interface(*env, m, os);
        }
        if (flag_model_types_only) {
          MiniZinc::output_model_variable_types(*env, m, os);
        }
        if (flag_compile_solution_check_model) {
          std::string mzc(FileUtils::deflateString(compiledSolutionCheckModel.str()));
          mzc = FileUtils::encodeBase64(mzc);
          std::string mzc_filename = filenames[0].substr(0,filenames[0].size()-4);
          if (flag_verbose)
            log << "Write solution checker to " << mzc_filename << "\n";
          std::ofstream mzc_f(mzc_filename);
          mzc_f << mzc;
          mzc_f.close();
        }
        status = SolverInstance::NONE;
      } else {
        if (is_flatzinc) {
          GCLock lock;
          vector<TypeError> typeErrors;
          MiniZinc::typecheck(*env, m, typeErrors, flag_model_check_only || flag_model_interface_only, flag_allow_multi_assign, true);
          if (typeErrors.size() > 0) {
            for (unsigned int i=0; i<typeErrors.size(); i++) {
              if (flag_verbose)
                log << std::endl;
              log << typeErrors[i].loc() << ":" << std::endl;
              log << typeErrors[i].what() << ": " << typeErrors[i].msg() << std::endl;
            }
            throw Error("multiple type errors");
          }
          MiniZinc::registerBuiltins(*env);
          env->swap();
          populateOutput(*env);
        } else {
          if (flag_verbose)
            log << "Flattening ...";

          fopts.onlyRangeDomains = flag_only_range_domains;
          fopts.verbose = flag_verbose;
          fopts.outputMode = flag_output_mode;
          fopts.outputObjective = flag_output_objective;
          fopts.outputOutputItem = flag_output_output_item;
#ifdef HAS_GECODE
          GecodeOptions gopts;
          gopts.only_range_domains = flag_only_range_domains;
          gopts.sac = flag_sac;
          gopts.allow_unbounded_vars = flag_allow_unbounded_vars;
          gopts.shave = flag_shave;
          gopts.printStatistics =  flag_statistics;
          gopts.pre_passes = flag_pre_passes;
#endif
          FlatteningOptions pass_opts = fopts;
          CompilePassFlags cfs;
          cfs.noMIPdomains = flag_noMIPdomains;
          cfs.verbose      = flag_verbose;
          cfs.statistics   = flag_statistics;
          cfs.optimize     = flag_optimize;
          cfs.chain_compression = flag_chain_compression;
          cfs.newfzn       = flag_newfzn;
          cfs.werror       = flag_werror;
          cfs.model_check_only = flag_model_check_only;
          cfs.model_interface_only  = flag_model_interface_only;
          cfs.allow_multi_assign    = flag_allow_multi_assign;

          std::vector<unique_ptr<Pass> > managed_passes;
          
          if(flag_two_pass) {
            std::string library = std_lib_dir + (flag_gecode ? "/gecode_presolver/" : "/std/");
            bool differentLibrary = (library!=std_lib_dir+"/"+globals_dir+"/");
            managed_passes.emplace_back(new CompilePass(env, pass_opts, cfs,
                                                        library, includePaths,  true, differentLibrary));
#ifdef HAS_GECODE
            if(flag_gecode)
              managed_passes.emplace_back(new GecodePass(&gopts));
#endif
          }
          managed_passes.emplace_back(new CompilePass(env, fopts, cfs,
                                                      std_lib_dir+"/"+globals_dir+"/",
                                                      includePaths, flag_two_pass, false));

          Env* out_env = multiPassFlatten(managed_passes);
          if(out_env == nullptr) exit(EXIT_FAILURE);

          if(out_env != env) {
            pEnv.reset(out_env);
          }
          env = out_env;
          if (flag_verbose)
            log << " done (" << starttime.stoptime() << "),"
                << " max stack depth " << env->maxCallStack() << std::endl;
        }

        if (flag_statistics) {
          FlatModelStatistics stats = statistics(*env);
          os << "% Generated FlatZinc statistics:\n";

          os << "%%%mzn-stat: paths=" << env->envi().getPathMap().size() << endl;

          if (stats.n_bool_vars) { os << "%%%mzn-stat: flatBoolVars=" << stats.n_bool_vars << endl; }
          if (stats.n_int_vars) { os << "%%%mzn-stat: flatIntVars=" << stats.n_int_vars << endl; }
          if (stats.n_float_vars) { os << "%%%mzn-stat: flatFloatVars=" << stats.n_float_vars << endl; }
          if (stats.n_set_vars) { os << "%%%mzn-stat: flatSetVars=" << stats.n_set_vars << endl; }

          if (stats.n_bool_ct) { os << "%%%mzn-stat: flatBoolConstraints=" << stats.n_bool_ct << endl; }
          if (stats.n_int_ct) { os << "%%%mzn-stat: flatIntConstraints=" << stats.n_int_ct << endl; }
          if (stats.n_float_ct) { os << "%%%mzn-stat: flatFloatConstraints=" << stats.n_float_ct << endl; }
          if (stats.n_set_ct) { os << "%%%mzn-stat: flatSetConstraints=" << stats.n_set_ct << endl; }

          if (stats.n_reif_ct) { os << "%%%mzn-stat: evaluatedReifiedConstraints=" << stats.n_reif_ct << endl; }
          if (stats.n_imp_ct) { os << "%%%mzn-stat: evaluatedHalfReifiedConstraints=" << stats.n_imp_ct << endl; }

          if (stats.n_imp_del) { os << "%%%mzn-stat: eliminatedImplications=" << stats.n_imp_del << endl; }
          if (stats.n_lin_del) { os << "%%%mzn-stat: eliminatedLinearConstraints=" << stats.n_lin_del << endl; }

          /// Objective / SAT. These messages are used by mzn-test.py.
          SolveI* solveItem = env->flat()->solveItem();
          if (solveItem->st() != SolveI::SolveType::ST_SAT) {
            if (solveItem->st() == SolveI::SolveType::ST_MAX) {
              os << "%%%mzn-stat: method=\"maximize\"" << endl;
            } else {
              os << "%%%mzn-stat: method=\"minimize\"" << endl;
            }
          } else {
            os << "%%%mzn-stat: method=\"satisfy\"" << endl;
          }

          os << "%%%mzn-stat: flatTime=" << flatten_time.s() << endl;
          os << "%%%mzn-stat-end" << endl << endl;
        }

        if (flag_output_paths_stdout) {
          if (flag_verbose)
            log << "Printing Paths to stdout ..." << std::endl;
          PathFilePrinter pfp(os, env->envi());
          pfp.print(env->flat());
          if (flag_verbose)
            log << " done (" << starttime.stoptime() << ")" << std::endl;
        } else if (flag_output_paths != "") {
          if (flag_verbose)
            log << "Printing Paths to '"
            << flag_output_paths << "' ..." << std::flush;
          std::ofstream ofs;
          ofs.open(flag_output_paths.c_str(), ios::out);
          checkIOStatus (ofs.good(), " I/O error: cannot open fzn output file. ");
          PathFilePrinter pfp(ofs, env->envi());
          pfp.print(env->flat());
          checkIOStatus (ofs.good(), " I/O error: cannot write fzn output file. ");
          ofs.close();
          if (flag_verbose)
            log << " done (" << starttime.stoptime() << ")" << std::endl;
        }

        if ( (fopts.collect_mzn_paths || flag_two_pass) && !flag_keep_mzn_paths) {
          class RemovePathAnnotations : public ItemVisitor {
          public:
            void removePath(Annotation& a) const {
              a.removeCall(constants().ann.mzn_path);
            }
            void vVarDeclI(VarDeclI* vdi) const { removePath(vdi->e()->ann()); }
            void vConstraintI(ConstraintI* ci) const { removePath(ci->e()->ann()); }
            void vSolveI(SolveI* si) const {
              removePath(si->ann());
              if(Expression* e = si->e()) removePath(e->ann());
            }
          } removePaths;
          iterItems<RemovePathAnnotations>(removePaths, env->flat());
        }

        if (flag_output_fzn_stdout) {
          if (flag_verbose)
            log << "Printing FlatZinc to stdout ..." << std::endl;
          Printer p(os,0);
          p.print(env->flat());
          if (flag_verbose)
            log << " done (" << starttime.stoptime() << ")" << std::endl;
        } else if(flag_output_fzn != "") {
          if (flag_verbose)
            log << "Printing FlatZinc to '"
            << flag_output_fzn << "' ..." << std::flush;
          std::ofstream ofs;
          ofs.open(flag_output_fzn.c_str(), ios::out);
          checkIOStatus (ofs.good(), " I/O error: cannot open fzn output file. ");
          Printer p(ofs,0);
          p.print(env->flat());
          checkIOStatus (ofs.good(), " I/O error: cannot write fzn output file. ");
          ofs.close();
          if (flag_verbose)
            log << " done (" << starttime.stoptime() << ")" << std::endl;
        }
        if (!flag_no_output_ozn) {
          if (flag_output_ozn_stdout) {
            if (flag_verbose)
              log << "Printing .ozn to stdout ..." << std::endl;
            Printer p(os,0);
            p.print(env->output());
            if (flag_verbose)
              log << " done (" << starttime.stoptime() << ")" << std::endl;
          } else if (flag_output_ozn != "") {
            if (flag_verbose)
              log << "Printing .ozn to '"
              << flag_output_ozn << "' ..." << std::flush;
            std::ofstream ofs;
            ofs.open(flag_output_ozn.c_str(), std::ios::out);
            checkIOStatus (ofs.good(), " I/O error: cannot open ozn output file. ");
            Printer p(ofs,0);
            p.print(env->output());
            checkIOStatus (ofs.good(), " I/O error: cannot write ozn output file. ");
            ofs.close();
            if (flag_verbose)
              log << " done (" << starttime.stoptime() << ")" << std::endl;
          }
        }
      }
    } else { // !flag_typecheck
      Printer p(os);
      p.print(m);
    }
  }
  
  if (getEnv()->envi().failed()) {
    status = SolverInstance::UNSAT;
  }
  
  if (flag_verbose) {
    size_t mem = GC::maxMem();
    if (mem < 1024)
      log << "Maximum memory " << mem << " bytes";
    else if (mem < 1024*1024)
      log << "Maximum memory " << mem/1024 << " Kbytes";
    else
      log << "Maximum memory " << mem/(1024*1024) << " Mbytes";
    log << "." << std::endl;
  }
}

void Flattener::printStatistics(ostream&)
{
}
