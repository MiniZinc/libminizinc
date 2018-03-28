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

using namespace std;
using namespace MiniZinc;

void Flattener::printVersion(ostream& os)
{
  os << "MiniZinc to FlatZinc converter, version "
     << MZN_VERSION_MAJOR << "." << MZN_VERSION_MINOR << "." << MZN_VERSION_PATCH << std::endl;
  os << "Copyright (C) 2014-" << string(__DATE__).substr(7, 4)
     << "   Monash University, NICTA, Data61" << std::endl;
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
  << "  --no-optimize\n    Do not optimize the FlatZinc" << std::endl
  << "  -d <file>, --data <file>\n    File named <file> contains data used by the model." << std::endl
  << "  -D <data>, --cmdline-data <data>\n    Include the given data assignment in the model." << std::endl
  << "  --stdlib-dir <dir>\n    Path to MiniZinc standard library directory" << std::endl
  << "  -G --globals-dir --mzn-globals-dir <dir>\n    Search for included globals in <stdlib>/<dir>." << std::endl
  << "  - --input-from-stdin\n    Read problem from standard input" << std::endl
  << "  -I --search-dir\n    Additionally search for included files in <dir>." << std::endl
  << "  -D \"fMIPdomains=false\"\n    No domain unification for MIP" << std::endl
  << "  --MIPDMaxIntvEE <n>\n    Max integer domain subinterval length to enforce equality encoding, default " << opt_MIPDmaxIntvEE << std::endl
  << "  --MIPDMaxDensEE <n>\n    Max domain cardinality to N subintervals ratio\n    to enforce equality encoding, default " << opt_MIPDmaxDensEE << ", either condition triggers" << std::endl
  << "  --only-range-domains\n    When no MIPdomains: all domains contiguous, holes replaced by inequalities" << std::endl
  << "  --allow-multiple-assignments\n    Allow multiple assignments to the same variable (e.g. in dzn)" << std::endl
  << std::endl
  << "Flattener two-pass options:" << std::endl
  << "  --two-pass\n    Flatten twice to make better flattening decisions for the target" << std::endl
#ifdef HAS_GECODE
  << "  --use-gecode\n    Perform root-node-propagation with Gecode (adds --two-pass)" << std::endl
  << "  --shave\n    Probe bounds of all variables at the root node (adds --use-gecode)" << std::endl
  << "  --sac\n    Probe values of all variables at the root node (adds --use-gecode)" << std::endl
  << "  --pre-passes <n>\n    Number of times to apply shave/sac pass (0 = fixed-point, 1 = default)" << std::endl
#endif
  << "  Two-pass optimisation levels:  -O0:    Disable optimize (--no-optimize)" << std::endl
  << "    -O1:    Single pass (default)"
#ifdef HAS_GECODE
  <<                                  "  -O2:    Same as: --use-gecode" << std::endl
  << "    -O3:    Same as: --shave       -O4:    Same as: --sac" << std::endl
#else
  << "\n    -O2,3,4:    Disabled [Requires MiniZinc with Gecode support]" << std::endl
#endif
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
  << "  --solution-checker <file>.mzc\n    Create output suitable for solution checking" << std::endl
  << "  -Werror\n    Turn warnings into errors" << std::endl
  ;
}

bool Flattener::processOption(int& i, const int argc, const char** argv)
{
  CLOParser cop( i, argc, argv );
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
  } else if ( cop.getOption( "-v --verbose") ) {
    flag_verbose = true;
  } else if (string(argv[i])==string("--newfzn")) {
    flag_newfzn = true;
  } else if ( cop.getOption( "--no-optimize --no-optimise") ) {
    flag_optimize = false;
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
      goto error;
    }
  } else if ( cop.getOption( "--output-objective" ) ) {
    flag_output_objective = true;
  } else if ( cop.getOption( "--solution-checker", &buffer ) ) {
    if (buffer.length()<8 || buffer.substr(buffer.length()-8,string::npos) != ".mzc.mzn")
      if (buffer.length()<=4 || buffer.substr(buffer.length()-4,string::npos) != ".mzc")
        goto error;
    flag_solution_check_model = buffer;
  } else if ( cop.getOption( "- --input-from-stdin" ) ) {
      if (datafiles.size() > 0 || filenames.size() > 0)
        goto error;
      flag_stdinInput = true;
  } else if ( cop.getOption( "-d --data", &buffer ) ) {
    if (flag_stdinInput)
      goto error;
    if ( buffer.length()<=4 ||
         buffer.substr(buffer.length()-4,string::npos) != ".dzn")
      goto error;
    datafiles.push_back(buffer);
  } else if ( cop.getOption( "--stdlib-dir", &std_lib_dir ) ) {
  } else if ( cop.getOption( "-G --globals-dir --mzn-globals-dir", &globals_dir ) ) {
  } else if ( cop.getOption( "-D --cmdline-data", &buffer)) {
    if (flag_stdinInput)
      goto error;
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
    if (i==argc) goto error;
    log << "warning: --npass option is deprecated --two-pass\n";
    int passes = atoi(argv[i]);
    if(passes == 1) flag_two_pass = false;
    else if(passes == 2) flag_two_pass = true;
  } else if (string(argv[i])=="--pre-passes") {
    i++;
    if (i==argc) goto error;
    int passes = atoi(argv[i]);
    if(passes >= 0) {
      flag_pre_passes = static_cast<unsigned int>(passes);
    }
  } else if (string(argv[i])=="-O0") {
    flag_optimize = false;
  } else if (string(argv[i])=="-O1") {
    // Default settings
#ifdef HAS_GECODE
  } else if (string(argv[i])=="-O2") {
    flag_two_pass = true;
    flag_gecode = true;
  } else if (string(argv[i])=="-O3") {
    flag_two_pass = true;
    flag_gecode = true;
    flag_shave = true;
  } else if (string(argv[i])=="-O4") {
    flag_two_pass = true;
    flag_gecode = true;
    flag_sac = true;
#else
  } else if (string(argv[i])=="-O2" || string(argv[i])=="-O3" || string(argv[i])=="-O4") {
    log << "% Warning: This compiler does not have Gecode builtin, cannot process -O2,-O3,-O4.\n";
    goto error;
#endif
    // ozn options must be after the -O<n> optimisation options
  } else if ( cop.getOption( "-O --ozn --output-ozn-to-file", &flag_output_ozn) ) {
  } else if (string(argv[i])=="--keep-paths") {
    flag_keep_mzn_paths = true;
    fopts.collect_mzn_paths = true;
  } else if (string(argv[i])=="--only-toplevel-presolve") {
    fopts.only_toplevel_paths = true;
  } else if ( cop.getOption( "--allow-multiple-assignments" ) ) {
    flag_allow_multi_assign = true;
  } else {
    if (flag_stdinInput || '-'==*argv[i])   // unknown option
      goto error;
    std::string input_file(argv[i]);
    if (input_file.length()<=4) {
      // std::cerr << "Error: cannot handle file " << input_file << "." << std::endl;
      goto error;
    }
    size_t last_dot = input_file.find_last_of('.');
    if (last_dot == string::npos) {
      goto error;
    }
    std::string extension = input_file.substr(last_dot,string::npos);
    if (extension == ".mzn" || extension ==  ".mzc" || extension == ".fzn") {
      if ( extension == ".fzn" ) {
        is_flatzinc = true;
        if ( fOutputByDefault )        // mzn2fzn mode
          goto error;
      }
      filenames.push_back(input_file);
    } else if (extension == ".dzn" || extension == ".json") {
      datafiles.push_back(input_file);
    } else {
      if ( fOutputByDefault )
        log << "Error: cannot handle file extension " << extension << "." << std::endl;
      goto error;
    }
  }
  return true;
error:
  return false;
}

Flattener::Flattener(std::ostream& os_, std::ostream& log_, bool fOutputByDef_)
  : os(os_), log(log_), fOutputByDefault(fOutputByDef_)
{
  if (char* MZNSTDLIBDIR = getenv("MZN_STDLIB_DIR")) {
    std_lib_dir = string(MZNSTDLIBDIR);
  }
}

Flattener::~Flattener()
{
  if (pEnv.get()) {      // ??? TODO
    if(is_flatzinc) {
      pEnv->swap();
    }
    delete pEnv->model();
  }
}

Env* Flattener::multiPassFlatten(const vector<unique_ptr<Pass> >& passes) {
  Env& e = *getEnv();

  Env* pre_env = &e;
  size_t npasses = passes.size();
  pre_env->envi().final_pass_no = static_cast<unsigned int>(npasses);
  Timer lasttime;
  bool verbose = false;
  for(unsigned int i=0; i<passes.size(); i++) {
    pre_env->envi().current_pass_no = i;
    if(verbose)
      log << "Start pass " << i << ":\n";

    Env* out_env = passes[i]->run(pre_env,log);
    if(out_env == nullptr) return nullptr;
    if(pre_env != &e && pre_env != out_env) {
      delete pre_env->model();
      delete pre_env;
    }
    pre_env = out_env;

    if(verbose)
      log << "Finish pass " << i << ": " << stoptime(lasttime) << "\n";
  }

  return pre_env;
}

void Flattener::flatten(const std::string& modelString)
{
  starttime01 = std::clock();
  lasttime = starttime01;
  
  if (flag_verbose)
    printVersion(log);

  // controlled from redefs and command line:
//   if (beginswith(globals_dir, "linear")) {
//     flag_only_range_domains = true;
//     if (flag_verbose)
//       cerr << "Assuming a linear programming-based solver (only_range_domains)." << endl;
//   }

  if ( filenames.empty() && !flag_stdinInput && modelString.empty() ) {
    throw Error( "Error: no model file given." );
  }

  if (std_lib_dir=="") {
    std_lib_dir = FileUtils::share_directory();
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
    if (flag_stdinInput || !modelString.empty()) {
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
    pEnv.reset(new Env());
    Env* env = getEnv();

    if (!flag_solution_check_model.empty()) {
      // Extract variables to check from solution check model
      if (flag_verbose)
        log << "Parsing solution checker model ..." << endl;
      std::vector<std::string> smm_model({flag_solution_check_model});
      Model* smm = parse(*env, smm_model, datafiles, includePaths, flag_ignoreStdlib, false, flag_verbose, errstream);
      if (flag_verbose)
        log << " done parsing (" << stoptime(lasttime) << ")" << std::endl;
      if (smm) {
        Env smm_env(smm);
        GCLock lock;
        vector<TypeError> typeErrors;
        MiniZinc::typecheck(smm_env, smm, typeErrors, true, false);
        if (typeErrors.size() > 0) {
          for (unsigned int i=0; i<typeErrors.size(); i++) {
            if (flag_verbose)
              log << std::endl;
            log << typeErrors[i].loc() << ":" << std::endl;
            log << typeErrors[i].what() << ": " << typeErrors[i].msg() << std::endl;
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
        std::ostringstream smm_oss;
        Printer p(smm_oss,0,false);
        p.print(smm);
        std::string smm_compressed = FileUtils::encodeBase64(FileUtils::deflateString(smm_oss.str()));
        TypeInst* ti = new TypeInst(Location().introduce(),Type::parstring(),NULL);
        VarDecl* checkString = new VarDecl(Location().introduce(),ti,ASTString("_mzn_solution_checker"),new StringLit(Location().introduce(),smm_compressed));
        VarDeclI* checkStringI = new VarDeclI(Location().introduce(), checkString);
        env->output()->addItem(checkStringI);
      }
    }

    if (!modelString.empty()) {
      if (flag_verbose)
        log << "Parsing model string ..." << endl;
      std::vector<SyntaxError> se;
      m = parseFromString(modelString, "stdin", includePaths, flag_ignoreStdlib, false, flag_verbose, errstream, se);
    } else if (flag_stdinInput) {
      if (flag_verbose)
        log << "Parsing standard input ..." << endl;
      std::string input = std::string(istreambuf_iterator<char>(std::cin), istreambuf_iterator<char>());
      std::vector<SyntaxError> se;
      m = parseFromString(input, "stdin", includePaths, flag_ignoreStdlib, false, flag_verbose, errstream, se);
    } else {
      if (flag_verbose) {
        MZN_ASSERT_HARD_MSG( filenames.size(), "at least one model file needed" );
        log << "Parsing file(s) '" << filenames[0] << '\'';
        for ( int i=1; i<filenames.size(); ++i )
          log << ", '" << filenames[i] << '\'';
        for ( const auto& sFln: datafiles )
          log << ", '" << sFln << '\'';
        log << " ..." << std::endl;
      }
      m = parse(*env, filenames, datafiles, includePaths, flag_ignoreStdlib, false, flag_verbose, errstream);
      if (globals_dir != "") {
        includePaths.erase(includePaths.begin());
      }
    }
    if (m==NULL)
      throw Error(errstream.str());
    
    env->model(m);
    if (flag_typecheck) {
      if (flag_verbose)
        log << " done parsing (" << stoptime(lasttime) << ")" << std::endl;

      if (flag_instance_check_only || flag_model_check_only || flag_model_interface_only) {
        GCLock lock;
        vector<TypeError> typeErrors;
        MiniZinc::typecheck(*env, m, typeErrors, flag_model_interface_only || flag_model_check_only, flag_allow_multi_assign);
        if (typeErrors.size() > 0) {
          for (unsigned int i=0; i<typeErrors.size(); i++) {
            if (flag_verbose)
              log << std::endl;
            log << typeErrors[i].loc() << ":" << std::endl;
            log << typeErrors[i].what() << ": " << typeErrors[i].msg() << std::endl;
          }
          throw Error("multiple type errors");
        }
      }
      
      if (flag_model_interface_only) {
        MiniZinc::output_model_interface(*env, m, os);
      }
      
      if (!flag_instance_check_only && !flag_model_check_only && !flag_model_interface_only) {
        if (is_flatzinc) {
          GCLock lock;
          vector<TypeError> typeErrors;
          MiniZinc::typecheck(*env, m, typeErrors, flag_model_check_only || flag_model_interface_only, flag_allow_multi_assign);
          if (typeErrors.size() > 0) {
            for (unsigned int i=0; i<typeErrors.size(); i++) {
              if (flag_verbose)
                log << std::endl;
              log << typeErrors[i].loc() << ":" << std::endl;
              log << typeErrors[i].what() << ": " << typeErrors[i].msg() << std::endl;
            }
            throw Error("multiple type errors");
          }
          MiniZinc::registerBuiltins(*env, m);
          env->swap();
          populateOutput(*env);
        } else {
          if (flag_verbose)
            log << "Flattening ...";

          fopts.onlyRangeDomains = flag_only_range_domains;
          fopts.verbose = flag_verbose;
          fopts.outputMode = flag_output_mode;
#ifdef HAS_GECODE
          Options gopts;
          gopts.setBoolParam(std::string("only-range-domains"), flag_only_range_domains);
          gopts.setBoolParam(std::string("sac"),       flag_sac);
          gopts.setBoolParam(std::string("allow_unbounded_vars"), flag_allow_unbounded_vars);
          gopts.setBoolParam(std::string("shave"),     flag_shave);
          gopts.setBoolParam(std::string("print_stats"),     flag_statistics);
          gopts.setIntParam(std::string("pre_passes"), flag_pre_passes);
#endif
          FlatteningOptions pass_opts = fopts;
          CompilePassFlags cfs;
          cfs.noMIPdomains = flag_noMIPdomains;
          cfs.verbose      = flag_verbose;
          cfs.statistics   = flag_statistics;
          cfs.optimize     = flag_optimize;
          cfs.newfzn       = flag_newfzn;
          cfs.werror       = flag_werror;
          cfs.model_check_only = flag_model_check_only;
          cfs.model_interface_only  = flag_model_interface_only;
          cfs.allow_multi_assign    = flag_allow_multi_assign;

          std::vector<unique_ptr<Pass> > managed_passes;

          if(flag_two_pass) {
            std::string library = std_lib_dir + (flag_gecode ? "/gecode/" : "/std/");
            managed_passes.emplace_back(new CompilePass(env, pass_opts, cfs,
                                                        library, includePaths,  true));
#ifdef HAS_GECODE
            if(flag_gecode)
              managed_passes.emplace_back(new GecodePass(gopts));
#endif
          }
          managed_passes.emplace_back(new CompilePass(env, fopts, cfs,
                                                      std_lib_dir+"/"+globals_dir+"/",
                                                      includePaths, flag_two_pass));

          fopts.outputObjective = flag_output_objective;
          Env* out_env = multiPassFlatten(managed_passes);
          if(out_env == nullptr) exit(EXIT_FAILURE);

          if(out_env != env) {
            delete env->model();
            pEnv.reset(out_env);
          }
          env = out_env;
          if (flag_verbose)
            log << " done (" << stoptime(lasttime) << "),"
                << " max stack depth " << env->maxCallStack() << std::endl;
        }

        if (flag_statistics) {
          FlatModelStatistics stats = statistics(*env);
          log << "Generated FlatZinc statistics:\n";

          log << "Paths: ";
          log << env->envi().getPathMap().size() << std::endl;

          log << "Variables: ";
          HadOne ho;
          log << ho(stats.n_bool_vars, " bool");
          log << ho(stats.n_int_vars, " int");
          log << ho(stats.n_float_vars, " float");
          log << ho(stats.n_set_vars, " set");
          if (!ho)
            log << "none";
          log << "\n";
          ho.reset();
          log << "Constraints: ";
          log << ho(stats.n_bool_ct, " bool");
          log << ho(stats.n_int_ct, " int");
          log << ho(stats.n_float_ct, " float");
          log << ho(stats.n_set_ct, " set");
          if (!ho)
            log << "none";
          log << "\n";
          /// Objective / SAT. These messages are used by mzn-test.py.
          SolveI* solveItem = env->flat()->solveItem();
          if (solveItem->st() != SolveI::SolveType::ST_SAT) {
            if (solveItem->st() == SolveI::SolveType::ST_MAX) {
              log << "    This is a maximization problem." << endl;
            } else {
              log << "    This is a minimization problem." << endl;
            }
          } else {
            log << "    This is a satisfiability problem." << endl;
          }
        }

        if (flag_output_paths_stdout) {
          if (flag_verbose)
            log << "Printing Paths to stdout ..." << std::endl;
          PathFilePrinter pfp(os, env->envi());
          pfp.print(env->flat());
          if (flag_verbose)
            log << " done (" << stoptime(lasttime) << ")" << std::endl;
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
            log << " done (" << stoptime(lasttime) << ")" << std::endl;
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
            log << " done (" << stoptime(lasttime) << ")" << std::endl;
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
            log << " done (" << stoptime(lasttime) << ")" << std::endl;
        }
        if (!flag_no_output_ozn) {
          if (flag_output_ozn_stdout) {
            if (flag_verbose)
              log << "Printing .ozn to stdout ..." << std::endl;
            Printer p(os,0);
            p.print(env->output());
            if (flag_verbose)
              log << " done (" << stoptime(lasttime) << ")" << std::endl;
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
              log << " done (" << stoptime(lasttime) << ")" << std::endl;
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
