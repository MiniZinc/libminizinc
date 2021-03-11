#include <minizinc/solvers/MIP/MIP_solverinstance.hh>

#include <chrono>
#include <queue>

namespace MiniZinc {

template <class MIPWrapper>
MIPSolverFactory<MIPWrapper>::MIPSolverFactory() : _factoryOptions() {
  for (auto& flag : MIPWrapper::getFactoryFlags()) {
    get_global_solver_registry()->addFactoryFlag(flag, this);
  }
}

template <class MIPWrapper>
bool MIPSolverFactory<MIPWrapper>::processFactoryOption(int& i, std::vector<std::string>& argv,
                                                        const std::string& workingDir) {
  return _factoryOptions.processOption(i, argv, workingDir);
}

template <class MIPWrapper>
void MIPSolverFactory<MIPWrapper>::factoryOptionsFinished() {
  _extraFlags = MIPWrapper::getExtraFlags(_factoryOptions);
  SolverConfig sc(getId(), MIPWrapper::getVersion(_factoryOptions, nullptr));
  sc.name(MIPWrapper::getName());
  sc.mznlib(MIPWrapper::getMznLib());
  sc.mznlibVersion(1);
  sc.supportsMzn(true);
  sc.description(MIPWrapper::getDescription(_factoryOptions, nullptr));
  sc.requiredFlags(MIPWrapper::getRequiredFlags(_factoryOptions));
  sc.tags(MIPWrapper::getTags());
  sc.stdFlags(MIPWrapper::getStdFlags());
  sc.extraFlags(_extraFlags);
  SolverConfigs::registerBuiltinSolver(sc);
}

template <class MIPWrapper>
bool MIPSolverFactory<MIPWrapper>::processOption(SolverInstanceBase::Options* opt, int& i,
                                                 std::vector<std::string>& argv,
                                                 const std::string& workingDir) {
  CLOParser cop(i, argv);
  auto& options = static_cast<typename MIPWrapper::Options&>(*opt);

  if (cop.get("-v --verbose-solving")) {
    options.verbose = true;
    return true;
  }
  if (cop.get("-s --solver-statistics")) {
    options.printStatistics = true;
    return true;
  }
  if (options.processOption(i, argv, workingDir)) {
    return true;
  }

  // Add any command line extra flags
  for (const auto& flag : _extraFlags) {
    if (flag.flagType == SolverConfig::ExtraFlag::FlagType::T_BOOL && flag.range.empty() &&
        cop.get(flag.flag.c_str())) {
      options.extraParams.emplace(flag.flag, "true");
      return true;
    }

    std::string buffer;
    if (cop.get(flag.flag.c_str(), &buffer)) {
      if (flag.validate(buffer)) {
        options.extraParams.emplace(flag.flag, buffer);
        return true;
      }
      return false;
    }
  }

  return false;
}

template <class MIPWrapper>
std::string MIPSolverFactory<MIPWrapper>::getDescription(SolverInstanceBase::Options* opt) {
  std::string v = "MIP solver plugin, compiled " __DATE__ ", using: " +
                  MIPWrapper::getDescription(_factoryOptions, opt);
  return v;
}

template <class MIPWrapper>
std::string MIPSolverFactory<MIPWrapper>::getVersion(SolverInstanceBase::Options* opt) {
  return MIPWrapper::getVersion(_factoryOptions, opt);
}

template <class MIPWrapper>
std::string MIPSolverFactory<MIPWrapper>::getId() {
  return "org.minizinc.mip." + MIPWrapper::getId();
}

template <class MIPWrapper>
MIPSolver::Variable MIPSolverinstance<MIPWrapper>::exprToVar(Expression* arg) {
  if (Id* ident = arg->dynamicCast<Id>()) {
    return _variableMap.get(ident->decl()->id());
  }
  return _mipWrapper->addLitVar(exprToConst(arg));
}

template <class MIPWrapper>
void MIPSolverinstance<MIPWrapper>::exprToVarArray(Expression* arg, std::vector<VarId>& vars) {
  ArrayLit* al = eval_array_lit(getEnv()->envi(), arg);
  vars.clear();
  vars.reserve(al->size());
  for (unsigned int i = 0; i < al->size(); i++) {
    vars.push_back(exprToVar((*al)[i]));
  }
}

template <class MIPWrapper>
std::pair<double, bool> MIPSolverinstance<MIPWrapper>::exprToConstEasy(Expression* e) {
  std::pair<double, bool> res{0.0, true};
  if (auto* il = e->dynamicCast<IntLit>()) {
    res.first = (static_cast<double>(il->v().toInt()));
  } else if (auto* fl = e->dynamicCast<FloatLit>()) {
    res.first = (fl->v().toDouble());
  } else if (auto* bl = e->dynamicCast<BoolLit>()) {
    res.first = static_cast<double>(bl->v());
  } else {
    res.second = false;
  }
  return res;
}

template <class MIPWrapper>
double MIPSolverinstance<MIPWrapper>::exprToConst(Expression* e) {
  const auto e2ce = exprToConstEasy(e);
  if (!e2ce.second) {
    std::ostringstream oss;
    oss << "ExprToConst: expected a numeric/bool literal, getting " << *e;
    throw InternalError(oss.str());
  }
  return e2ce.first;
}

template <class MIPWrapper>
void MIPSolverinstance<MIPWrapper>::exprToArray(Expression* arg, std::vector<double>& vals) {
  ArrayLit* al = eval_array_lit(getEnv()->envi(), arg);
  vals.clear();
  vals.reserve(al->size());
  for (unsigned int i = 0; i < al->size(); i++) {
    vals.push_back(exprToConst((*al)[i]));
  }
}

template <class MIPWrapper>
void MIPSolverinstance<MIPWrapper>::processSearchAnnotations(const Annotation& ann) {
  if (getMIPWrapper()->getFreeSearch() == MIPWrapper::SearchType::FREE_SEARCH) {
    return;
  }
  std::vector<Expression*> flattenedAnns;
  flattenSearchAnnotations(ann, flattenedAnns);
  std::vector<MIPSolverinstance::VarId> vars;
  std::vector<int> aPri;  // priorities

  /// Annotations that may be useful for custom search strategies in e.g. SCIP
  std::deque<std::string> variableSelection;
  std::deque<std::string> valueSelection;

  int nArrayAnns = 0;
  auto priority = flattenedAnns.size();  // Variables at front get highest pri
  for (const auto& annExpression : flattenedAnns) {
    /// Skip expressions that are not meaningful or we cannot process
    if (!annExpression->isa<Call>()) {
      continue;
    }

    Call* annotation = annExpression->cast<Call>();
    const auto annotation_type = annotation->id();
    if (annotation_type != "int_search" && annotation_type != "float_search") {
      continue;
    }

    if ((annotation->argCount() == 0U) ||
        nullptr == eval_array_lit(_env.envi(), annotation->arg(0))) {
      std::cerr << "  SEARCH ANN: '" << (*annotation) << "'  is unknown. " << std::endl;
      continue;
    }

    /// Save the variable selection and the value selection strategies, indexed on priority.
    /// Rules are ordered by ascending priorities, i.e. rules with lower priorities are at the front
    /// so that we can index them by priority.
    const auto cVarSel = annotation->arg(1)->cast<Id>()->str();
    const auto cValSel = annotation->arg(2)->cast<Id>()->str();
    variableSelection.push_front(cVarSel.c_str());
    valueSelection.push_front(cValSel.c_str());

    ++nArrayAnns;

    /// Take the variables and append them with set prioirty.
    std::vector<MIPSolverinstance::VarId> annVars;
    exprToVarArray(annotation->arg(0), annVars);
    aPri.insert(aPri.end(), annVars.size(), --priority);
    std::move(annVars.begin(), annVars.end(), std::back_inserter(vars));
  }

  if (vars.empty()) {
    return;
  }

  if (getMIPWrapper()->getFreeSearch() == MIPWrapper::SearchType::UNIFORM_SEARCH) {
    std::fill(aPri.begin(), aPri.end(), 1);
    /// It is an error here to use variableSelection / valueSelection since
    /// we can't index them anymore. Makes no sense to use them for uniform search anyway.
    variableSelection.clear();
    valueSelection.clear();
  } else {
    /// Subtract offset of remaining priority so that priorities start at 0,
    /// so that we can index variableSelection and valueSelection by priority.
    std::transform(aPri.cbegin(), aPri.cend(), aPri.begin(),
                   [priority](const int p) { return p - priority; });
  }

  // Try adding to solver
  const auto successfullyAddedAnnotations = getMIPWrapper()->addSearch(vars, aPri);
  if (!successfullyAddedAnnotations) {
    std::cerr << "\nWARNING: MIP backend seems to ignore search strategy." << std::endl;
  } else {
    std::cerr << "  MIP: added " << vars.size() << " variable branching priorities from "
              << nArrayAnns << " arrays." << std::endl;
  }
}

template <class MIPWrapper>
void MIPSolverinstance<MIPWrapper>::processWarmstartAnnotations(const Annotation& ann) {
  int nVal = 0;
  for (ExpressionSetIter i = ann.begin(); i != ann.end(); ++i) {
    Expression* e = *i;
    if (e->isa<Call>()) {
      Call* c = e->cast<Call>();
      if (c->id() == "warm_start_array" || c->id() == "seq_search") {
        auto* anns = c->arg(0)->cast<ArrayLit>();
        for (unsigned int i = 0; i < anns->size(); i++) {
          Annotation subann;
          subann.add((*anns)[i]);
          processWarmstartAnnotations(subann);
        }
      } else if (c->id() == "warm_start") {
        MZN_ASSERT_HARD_MSG(c->argCount() >= 2, "ERROR: warm_start needs 2 array args");
        std::vector<double> coefs;
        std::vector<MIPSolverinstance::VarId> vars;

        /// Process coefs & vars together to eliminate literals (problem with Gurobi's
        /// updatemodel()'s)
        ArrayLit* alC = eval_array_lit(_env.envi(), c->arg(1));
        MZN_ASSERT_HARD_MSG(nullptr != alC, "ERROR: warm_start needs 2 array args");
        coefs.reserve(alC->size());
        ArrayLit* alV = eval_array_lit(_env.envi(), c->arg(0));
        MZN_ASSERT_HARD_MSG(nullptr != alV, "ERROR: warm_start needs 2 array args");
        vars.reserve(alV->size());
        for (unsigned int i = 0; i < alV->size() && i < alC->size(); i++) {
          const auto e2c = exprToConstEasy((*alC)[i]);
          /// Check if it is not an opt int etc. and a proper variable
          if (e2c.second) {
            if (Id* ident = (*alV)[i]->dynamicCast<Id>()) {
              coefs.push_back(e2c.first);
              vars.push_back(exprToVar(ident));
            }  // else ignore
          }
        }
        assert(coefs.size() == vars.size());
        nVal += static_cast<int>(coefs.size());
        if (!coefs.empty() && !getMIPWrapper()->addWarmStart(vars, coefs)) {
          std::cerr << "\nWARNING: MIP backend seems to ignore warm starts" << std::endl;
          return;
        }
      }
    }
  }
  if (nVal && getMIPWrapper()->fVerbose) {
    std::cerr << "  MIP: added " << nVal << " MIPstart values..." << std::flush;
  }
}

template <class MIPWrapper>
void MIPSolverinstance<MIPWrapper>::processMultipleObjectives(const Annotation& ann) {
  MultipleObjectives mo;
  flattenMultipleObjectives(ann, mo);
  if (mo.size() != 0U) {
    typename MIPWrapper::MultipleObjectives mo_mip;
    for (const auto& obj : mo.getObjectives()) {
      mo_mip.add({exprToVar(obj.getVariable()), obj.getWeight()});
    }
    if (!getMIPWrapper()->defineMultipleObjectives(mo_mip)) {
      getEnv()->envi().addWarning("Solver backend does not support multiple objectives.");
    }
    if (getMIPWrapper()->fVerbose) {
      std::cerr << "  MIP: added " << mo.size() << " objectives." << std::endl;
    }
  }
}

template <class MIPWrapper>
void MIPSolverinstance<MIPWrapper>::processFlatZinc() {
  _mipWrapper->fVerbose = _options->verbose;

  SolveI* solveItem = getEnv()->flat()->solveItem();
  VarDecl* objVd = nullptr;

  if (solveItem->st() != SolveI::SolveType::ST_SAT) {
    if (Id* id = solveItem->e()->dynamicCast<Id>()) {
      objVd = id->decl();
    } else {
      std::cerr << "Objective must be Id: " << solveItem->e() << std::endl;
      throw InternalError("Objective must be Id");
    }
  }

  for (VarDeclIterator it = getEnv()->flat()->vardecls().begin();
       it != getEnv()->flat()->vardecls().end(); ++it) {
    if (it->removed()) {
      continue;
    }
    VarDecl* vd = it->e();
    if (!vd->ann().isEmpty()) {
      if (vd->ann().containsCall(constants().ann.output_array) ||
          vd->ann().contains(constants().ann.output_var)) {
        _varsWithOutput.push_back(vd);
        //         std::cerr << (*vd);
        //         if ( vd->e() )
        //           cerr << " = " << (*vd->e());
        //         cerr << endl;
      }
    }
    if (vd->type().dim() == 0 && it->e()->type().isvar() && !it->removed()) {
      MiniZinc::TypeInst* ti = it->e()->ti();
      typename MIPWrapper::VarType vType = MIPWrapper::VarType::REAL;  // fInt = false;
      if (ti->type().isvarint() || ti->type().isint()) {
        vType = MIPWrapper::VarType::INT;
      } else if (ti->type().isvarbool() || ti->type().isbool()) {
        vType = MIPWrapper::VarType::BINARY;
      } else if (!(ti->type().isvarfloat() || ti->type().isfloat())) {
        std::stringstream ssm;
        ssm << "This type of var is not handled by MIP: " << *it << std::endl;
        ssm << "  VarDecl flags (ti, bt, st, ot): " << ti->type().ti() << ti->type().bt()
            << ti->type().st() << ti->type().ot() << ", dim == " << ti->type().dim()
            << "\nRemove the variable or add a constraint so it is redefined." << std::endl;
        throw InternalError(ssm.str());
      }
      double lb = 0.0;
      double ub = 1.0;  // for bool
      if (ti->domain() != nullptr) {
        if (MIPWrapper::VarType::REAL == vType) {
          FloatBounds fb = compute_float_bounds(getEnv()->envi(), it->e()->id());
          if (fb.valid) {
            lb = fb.l.toDouble();
            ub = fb.u.toDouble();
          } else {
            lb = 1.0;
            ub = 0.0;
          }
        } else if (MIPWrapper::VarType::INT == vType) {
          IntBounds ib = compute_int_bounds(getEnv()->envi(), it->e()->id());
          if (ib.valid) {  // Normally should be
            lb = static_cast<double>(ib.l.toInt());
            ub = static_cast<double>(ib.u.toInt());
          } else {
            lb = 1;
            ub = 0;
          }
        }
      } else if (MIPWrapper::VarType::BINARY != vType) {
        lb = -getMIPWrapper()->getInfBound();  // if just 1 bound inf, using MZN's default?  TODO
        ub = -lb;
      }

      //       IntSetVal* dom = eval_intset(env,vdi->e()->ti()->domain());
      //       if (dom->size() > 1)
      //         throw runtime_error("MIPSolverinstance: domains with holes ! supported, use
      //         --MIPdomains");

      VarId res;
      Id* id = it->e()->id();
      MZN_ASSERT_HARD(id == id->decl()->id());  // Assume all unified
      MZN_ASSERT_HARD(it->e() == id->decl());   // Assume all unified
      double obj = vd == objVd ? 1.0 : 0.0;
      auto* decl00 = follow_id_to_decl(it->e());
      MZN_ASSERT_HARD(decl00->isa<VarDecl>());
      {
        auto* vd00 = decl00->dynamicCast<VarDecl>();
        if (nullptr != vd00->e()) {
          // Should be a const
          auto dRHS = exprToConst(vd00->e());
          lb = std::max(lb, dRHS);
          ub = std::min(ub, dRHS);
        }
        if (it->e() != vd00) {                             // A different vardecl
          res = exprToVar(vd00->id());                     // Assume FZN is sorted.
          MZN_ASSERT_HARD(!getMIPWrapper()->fPhase1Over);  // Still can change colUB, colObj
          /// Tighten the ini-expr's bounds
          lb = getMIPWrapper()->colLB.at(res) = std::max(getMIPWrapper()->colLB.at(res), lb);
          ub = getMIPWrapper()->colUB.at(res) = std::min(getMIPWrapper()->colUB.at(res), ub);
          if (0.0 != obj) {
            getMIPWrapper()->colObj.at(res) = obj;
          }
        } else {
          res = getMIPWrapper()->addVar(obj, lb, ub, vType, id->str().c_str());
        }
      }
      /// Test infeasibility
      if (lb > ub) {
        _status = SolverInstance::UNSAT;
        if (getMIPWrapper()->fVerbose) {
          std::cerr << "  VarDecl '" << *(it->e()) << "' seems infeasible: computed bounds [" << lb
                    << ", " << ub << ']' << std::endl;
        }
      }
      if (0.0 != obj) {
        dObjVarLB = lb;
        dObjVarUB = ub;
        getMIPWrapper()->output.nObjVarIndex = res;
        if (getMIPWrapper()->fVerbose) {
          std::cerr << "  MIP: objective variable index (0-based): " << res << std::endl;
        }
      }
      _variableMap.insert(id, res);
      assert(res == _variableMap.get(id));
    }
  }
  if (_mipWrapper->fVerbose && (!_mipWrapper->sLitValues.empty())) {
    std::cerr << "  MIPSolverinstance: during Phase 1,  " << _mipWrapper->nLitVars
              << " literals with " << _mipWrapper->sLitValues.size() << " values used."
              << std::endl;
  }
  if (!getMIPWrapper()->fPhase1Over) {
    getMIPWrapper()->addPhase1Vars();
  }

  if (_mipWrapper->fVerbose) {
    std::cerr << "  MIPSolverinstance: adding constraints..." << std::flush;
  }

  for (ConstraintIterator it = getEnv()->flat()->constraints().begin();
       it != getEnv()->flat()->constraints().end(); ++it) {
    if (!it->removed()) {
      if (Call* c = it->e()->dynamicCast<Call>()) {
        _constraintRegistry.post(c);
      }
    }
  }

  if (_mipWrapper->fVerbose) {
    std::cerr << " done, " << _mipWrapper->getNRows() << " rows && " << _mipWrapper->getNCols()
              << " columns in total.";
    if (_mipWrapper->nIndicatorConstr != 0) {
      std::cerr << "  " << _mipWrapper->nIndicatorConstr << " indicator constraints." << std::endl;
    }
    std::cerr << std::endl;
    if (!_mipWrapper->sLitValues.empty()) {
      std::cerr << "  MIPSolverinstance: overall,  " << _mipWrapper->nLitVars << " literals with "
                << _mipWrapper->sLitValues.size() << " values used." << std::endl;
    }
  }

  processSearchAnnotations(solveItem->ann());

  processWarmstartAnnotations(solveItem->ann());

  processMultipleObjectives(solveItem->ann());
}  // processFlatZinc

template <class MIPWrapper>
Expression* MIPSolverinstance<MIPWrapper>::getSolutionValue(Id* id) {
  id = id->decl()->id();

  if (id->type().isvar()) {
    MIPSolver::Variable var = exprToVar(id);
    double val = getMIPWrapper()->getValues()[var];
    switch (id->type().bt()) {
      case Type::BT_INT:
        return IntLit::a(round_to_longlong(val));
      case Type::BT_FLOAT:
        return FloatLit::a(val);
      case Type::BT_BOOL:
        return new BoolLit(Location(), round_to_longlong(val) != 0);
      default:
        return nullptr;
    }
  } else {
    return id->decl()->e();
  }
}

template <class MIPWrapper>
void MIPSolverinstance<MIPWrapper>::genCuts(const typename MIPWrapper::Output& slvOut,
                                            typename MIPWrapper::CutInput& cutsIn, bool fMIPSol) {
  for (auto& pCG : _cutGenerators) {
    if (!fMIPSol || ((pCG->getMask() & MIPWrapper::MaskConsType_Lazy) != 0)) {
      pCG->generate(slvOut, cutsIn);
    }
  }
  /// Select some most violated? TODO
}

template <class MIPWrapper>
void MIPSolverinstance<MIPWrapper>::printStatisticsLine(bool fLegend) {
  //   auto nn = std::chrono::system_clock::now();
  //   auto n_c = std::chrono::system_clock::to_time_t( nn );
  {
    std::ios oldState(nullptr);
    oldState.copyfmt(_log);
    _log.precision(12);
    _log << "  % MIP Status: " << _mipWrapper->getStatusName() << std::endl;
    if (fLegend) {
      _log << "  % obj, bound, time wall/CPU, nodes (left): ";
    }
    _log << _mipWrapper->getObjValue() << ",  ";
    _log << _mipWrapper->getBestBound() << ",  ";
    _log.setf(std::ios::fixed);
    _log.precision(1);
    _log << _mipWrapper->getWallTimeElapsed() << "/";
    _log << _mipWrapper->getCPUTime() << ",  ";
    _log << _mipWrapper->getNNodes();
    if (_mipWrapper->getNOpen() != 0) {
      _log << " ( " << _mipWrapper->getNOpen() << " )";
    }
    //       _log << "    " << std::ctime( &n_c );
    //  ctime already adds EOL.     os << endl;
    _log << std::endl;
    _log.copyfmt(oldState);
  }
}

template <class MIPWrapper>
void MIPSolverinstance<MIPWrapper>::printStatistics() {
  //   auto nn = std::chrono::system_clock::now();
  //   auto n_c = std::chrono::system_clock::to_time_t( nn );
  {
    EnvI& env = getEnv()->envi();

    std::ios oldState(nullptr);
    oldState.copyfmt(env.outstream);
    env.outstream.precision(12);
    env.outstream << "%%%mzn-stat: objective=" << _mipWrapper->getObjValue() << std::endl;
    ;
    env.outstream << "%%%mzn-stat: objectiveBound=" << _mipWrapper->getBestBound() << std::endl;
    ;
    env.outstream << "%%%mzn-stat: nodes=" << _mipWrapper->getNNodes() << std::endl;
    ;
    if (_mipWrapper->getNOpen() != 0) {
      env.outstream << "%%%mzn-stat: openNodes=" << _mipWrapper->getNOpen() << std::endl;
    };
    env.outstream.setf(std::ios::fixed);
    env.outstream.precision(4);
    env.outstream << "%%%mzn-stat: solveTime=" << _mipWrapper->getCPUTime() << std::endl;
    ;
    env.outstream.copyfmt(oldState);

    env.outstream << "%%%mzn-stat-end" << std::endl;
  }
}

template <class MIPWrapper>
void handle_solution_callback(const typename MIPWrapper::Output& out, void* pp) {
  // multi-threading? TODO
  auto* pSI = static_cast<MIPSolverinstance<MIPWrapper>*>(pp);
  assert(pSI);
  /// Not for -a:
  //   if (fabs(pSI->lastIncumbent - out.objVal) > 1e-12*(1.0 + fabs(out.objVal))) {
  pSI->lastIncumbent = out.objVal;

  try {                    /// Sometimes the intermediate output is wrong, especially in SCIP
    pSI->printSolution();  // The solution in [out] is not used  TODO
  } catch (const Exception& e) {
    std::cerr << std::endl;
    std::cerr << "  Error when evaluating an intermediate solution:  " << e.what() << ": "
              << e.msg() << std::endl;
  } catch (const std::exception& e) {
    std::cerr << std::endl;
    std::cerr << "  Error when evaluating an intermediate solution:  " << e.what() << std::endl;
  } catch (...) {
    std::cerr << std::endl;
    std::cerr << "  Error when evaluating an intermediate solution:  "
              << "  UNKNOWN EXCEPTION." << std::endl;
  }
  //   }
}

template <class MIPWrapper>
void handle_cut_callback(const typename MIPWrapper::Output& out, typename MIPWrapper::CutInput& in,
                         void* pp, bool fMIPSol) {
  // multi-threading? TODO
  auto* pSI = static_cast<MIPSolverinstance<MIPWrapper>*>(pp);
  assert(pSI);
  assert(&out);
  assert(&in);
  pSI->genCuts(out, in, fMIPSol);
}

template <class MIPWrapper>
SolverInstance::Status MIPSolverinstance<MIPWrapper>::solve() {
  SolveI* solveItem = getEnv()->flat()->solveItem();
  int nProbType = 0;
  if (solveItem->st() != SolveI::SolveType::ST_SAT) {
    if (solveItem->st() == SolveI::SolveType::ST_MAX) {
      getMIPWrapper()->setObjSense(1);
      getMIPWrapper()->setProbType(1);
      nProbType = 1;
      if (_mipWrapper->fVerbose) {
        std::cerr << "    MIPSolverinstance: this is a MAXimization problem." << std::endl;
      }
    } else {
      getMIPWrapper()->setObjSense(-1);
      getMIPWrapper()->setProbType(-1);
      nProbType = -1;
      if (_mipWrapper->fVerbose) {
        std::cerr << "    MIPSolverinstance: this is a MINimization problem." << std::endl;
      }
    }
    if (_mipWrapper->fVerbose) {
      std::cerr << "    MIPSolverinstance: bounds for the objective function: " << dObjVarLB << ", "
                << dObjVarUB << std::endl;
    }
  } else {
    getMIPWrapper()->setProbType(0);
    if (_mipWrapper->fVerbose) {
      std::cerr << "    MIPSolverinstance: this is a SATisfiability problem." << std::endl;
    }
  }

  lastIncumbent = 1e200;  // for callbacks
  typename MIPWrapper::Status sw;
  if (SolverInstance::UNSAT == _status) {  // already deduced - exit now
    return _status;
  }
  if (getMIPWrapper()->getNCols()) {  // If any variables, we need to run solver just to get values?
    getMIPWrapper()->provideSolutionCallback(handle_solution_callback<MIPWrapper>, this);
    if (!_cutGenerators.empty()) {  // only then, can modify presolve
      getMIPWrapper()->provideCutCallback(handle_cut_callback<MIPWrapper>, this);
    }
    ////////////// clean up envi /////////////////
    {
      /// Removing for now - need access to output variables  TODO
      //       cleanupForNonincrementalSolving();
      if (GC::locked() && _mipWrapper->fVerbose) {
        std::cerr << "WARNING: GC is locked before SolverInstance::solve()! Wasting memory."
                  << std::endl;
      }
      // GCLock lock;
      GC::trigger();
    }
    getMIPWrapper()->solve();
    //   printStatistics(cout, 1);   MznSolver does this (if it wants)
    sw = getMIPWrapper()->getStatus();
  } else {
    if (_mipWrapper->fVerbose) {
      std::cerr << "  MIPSolverinstance: no constraints - skipping actual solution phase."
                << std::endl;
    }
    sw = MIPWrapper::Status::OPT;
    printSolution();
  }
  SolverInstance::Status s = SolverInstance::UNKNOWN;
  switch (sw) {
    case MIPWrapper::Status::OPT:
      if (0 != nProbType) {
        s = SolverInstance::OPT;
      } else {
        s = SolverInstance::SAT;  // For SAT problems, just say SAT unless we know it's complete
      }
      break;
    case MIPWrapper::Status::SAT:
      s = SolverInstance::SAT;
      break;
    case MIPWrapper::Status::UNSAT:
      s = SolverInstance::UNSAT;
      break;
    case MIPWrapper::Status::UNBND:
      s = SolverInstance::UNBND;
      break;
    case MIPWrapper::Status::UNSATorUNBND:
      s = SolverInstance::UNSATorUNBND;
      break;
    case MIPWrapper::Status::UNKNOWN:
      s = SolverInstance::UNKNOWN;
      break;
    default:
      s = SolverInstance::ERROR;
  }
  _pS2Out->stats.nNodes = _mipWrapper->getNNodes();
  return s;
}

namespace SCIPConstraints {

bool check_ann_user_cut(const Call* call);
bool check_ann_lazy_constraint(const Call* call);
int get_mask_cons_type(const Call* call);

/// Create constraint name
/// Input: a prefix, a counter, and the original call.
/// If the call has a path annotation, that is used,
/// otherwise pfx << cnt.
inline std::string make_constraint_name(const char* pfx, int cnt,
                                        const Expression* cOrig = nullptr) {
  Call* mznp;
  std::ostringstream ss;
  if (nullptr != cOrig && ((mznp = cOrig->ann().getCall(constants().ann.mzn_path)) != nullptr)) {
    assert(1 == mznp->argCount());
    auto* strp = mznp->arg(0)->dynamicCast<StringLit>();
    assert(strp);
    ss << strp->v().substr(0, 255);  // Gurobi 8.1 has <=255 characters
  } else {
    ss << pfx << cnt;
  }
  return ss.str();
}

/// Gurobi 8.1.0 complains about duplicates, CPLEX 12.8.0 just ignores repeats
/// An example for duplicated indices was on 72a9b64f with two floats equated
template <class Idx>
void remove_duplicates(std::vector<Idx>& rmi, std::vector<double>& rmv) {
  std::unordered_map<Idx, double> linExp;
  for (int i = rmi.size(); i--;) {
    linExp[rmi[i]] += rmv[i];
  }
  if (rmi.size() == linExp.size()) {
    return;
  }
  rmi.resize(linExp.size());
  rmv.resize(linExp.size());
  int i = 0;
  for (const auto& iv : linExp) {
    rmi[i] = iv.first;
    rmv[i] = iv.second;
    ++i;
  }
}

template <class MIPWrapper>
void p_lin(SolverInstanceBase& si, const Call* call, typename MIPWrapper::LinConType lt) {
  auto& gi = dynamic_cast<MIPSolverinstance<MIPWrapper>&>(si);
  Env& _env = gi.env();
  //     ArrayLit* al = eval_array_lit(_env.envi(), args[0]);
  //     int nvars = al->v().size();
  std::vector<double> coefs;
  //     gi.exprToArray(args[0], coefs);
  std::vector<typename MIPSolverinstance<MIPWrapper>::VarId> vars;
  //     gi.exprToVarArray(args[1], vars);
  IntVal ires;
  FloatVal fres;

  double rhs;
  if (call->arg(2)->type().isint()) {
    ires = eval_int(_env.envi(), call->arg(2));
    rhs = static_cast<double>(ires.toInt());
  } else if (call->arg(2)->type().isfloat()) {
    fres = eval_float(_env.envi(), call->arg(2));
    rhs = fres.toDouble();
  } else {
    throw InternalError("p_lin: rhs unknown type");
  }

  /// Process coefs & vars together to eliminate literals (problem with Gurobi's updatemodel()'s)
  ArrayLit* alC = eval_array_lit(_env.envi(), call->arg(0));
  coefs.reserve(alC->size());
  ArrayLit* alV = eval_array_lit(_env.envi(), call->arg(1));
  vars.reserve(alV->size());
  for (unsigned int i = 0; i < alV->size(); i++) {
    const double dCoef = gi.exprToConst((*alC)[i]);
    if (Id* ident = (*alV)[i]->dynamicCast<Id>()) {
      coefs.push_back(dCoef);
      vars.push_back(gi.exprToVar(ident));
    } else {
      rhs -= dCoef * gi.exprToConst((*alV)[i]);
    }
  }
  assert(coefs.size() == vars.size());

  /// Check feas-ty
  if (coefs.empty()) {
    if ((MIPWrapper::LinConType::EQ == lt && 1e-5 < fabs(rhs)) ||
        (MIPWrapper::LinConType::LQ == lt && -1e-5 > (rhs)) ||
        (MIPWrapper::LinConType::GQ == lt && 1e-5 < (rhs))) {
      si.setStatus(SolverInstance::UNSAT);
      if (gi.getMIPWrapper()->fVerbose) {
        std::cerr << "  Constraint '" << *call << "' seems infeasible: simplified to 0 (rel) "
                  << rhs << std::endl;
      }
    }
  } else {
    remove_duplicates(vars, coefs);
    // See if the solver adds indexation itself: no.
    gi.getMIPWrapper()->addRow(
        static_cast<int>(coefs.size()), &vars[0], &coefs[0], lt, rhs, get_mask_cons_type(call),
        make_constraint_name("p_lin_", (gi.getMIPWrapper()->nAddedRows++), call));
  }
}

template <class MIPWrapper>
void p_int_lin_le(SolverInstanceBase& si, const Call* call) {
  p_lin<MIPWrapper>(si, call, MIPWrapper::LQ);
}
template <class MIPWrapper>
void p_int_lin_eq(SolverInstanceBase& si, const Call* call) {
  p_lin<MIPWrapper>(si, call, MIPWrapper::EQ);
}
template <class MIPWrapper>
void p_float_lin_le(SolverInstanceBase& si, const Call* call) {
  p_lin<MIPWrapper>(si, call, MIPWrapper::LQ);
}
template <class MIPWrapper>
void p_float_lin_eq(SolverInstanceBase& si, const Call* call) {
  p_lin<MIPWrapper>(si, call, MIPWrapper::EQ);
}

// The non-_lin constraints happen in a failed model || in a non-optimized one:
template <class MIPWrapper>
void p_non_lin(SolverInstanceBase& si, const Call* call, typename MIPWrapper::LinConType nCmp) {
  auto& gi = dynamic_cast<MIPSolverinstance<MIPWrapper>&>(si);
  std::vector<double> coefs;
  std::vector<MIPSolver::Variable> vars;
  double rhs = 0.0;
  if (call->arg(0)->isa<Id>()) {
    coefs.push_back(1.0);
    vars.push_back(gi.exprToVar(call->arg(0)));
  } else {
    rhs -= gi.exprToConst(call->arg(0));
  }
  if (call->arg(1)->isa<Id>()) {
    coefs.push_back(-1.0);
    vars.push_back(gi.exprToVar(call->arg(1)));
  } else {
    rhs += gi.exprToConst(call->arg(1));
  }
  /// Check feas-ty
  if (coefs.empty()) {
    if ((MIPWrapper::LinConType::EQ == nCmp && 1e-5 < fabs(rhs)) ||
        (MIPWrapper::LinConType::LQ == nCmp && -1e-5 > (rhs)) ||
        (MIPWrapper::LinConType::GQ == nCmp && 1e-5 < (rhs))) {
      si.setStatus(SolverInstance::UNSAT);
      if (gi.getMIPWrapper()->fVerbose) {
        std::cerr << "  Constraint '" << *call << "' seems infeasible: simplified to 0 (rel) "
                  << rhs << std::endl;
      }
    }
  } else {
    remove_duplicates(vars, coefs);
    gi.getMIPWrapper()->addRow(
        static_cast<int>(vars.size()), &vars[0], &coefs[0], nCmp, rhs, get_mask_cons_type(call),
        make_constraint_name("p_eq_", (gi.getMIPWrapper()->nAddedRows++), call));
  }
}
template <class MIPWrapper>
void p_eq(SolverInstanceBase& si, const Call* call) {
  p_non_lin<MIPWrapper>(si, call, MIPWrapper::EQ);
}
template <class MIPWrapper>
void p_le(SolverInstanceBase& si, const Call* call) {
  p_non_lin<MIPWrapper>(si, call, MIPWrapper::LQ);
}

/// var1<=0 if var2==0
template <class MIPWrapper>
void p_indicator_le0_if0(SolverInstanceBase& si, const Call* call) {
  auto& gi = dynamic_cast<MIPSolverinstance<MIPWrapper>&>(si);
  /// Looking at the bounded variable and the flag
  bool f1const = 0;
  bool f2const = 0;
  double val1;
  double val2;
  MIPSolver::Variable var1;
  MIPSolver::Variable var2;
  if (call->arg(0)->isa<Id>()) {
    var1 = gi.exprToVar(call->arg(0));
  } else {
    f1const = 1;
    val1 = gi.exprToConst(call->arg(0));
  }
  if (call->arg(1)->isa<Id>()) {
    var2 = gi.exprToVar(call->arg(1));
  } else {
    f2const = 1;
    val2 = gi.exprToConst(call->arg(1));
  }
  /// Check feas-ty. 1e-6 ?????????????   TODO
  if (f1const && f2const) {
    if (val1 > 1e-6 && val2 < 1e-6) {
      si.setStatus(SolverInstance::UNSAT);
      if (gi.getMIPWrapper()->fVerbose) {
        std::cerr << "  Constraint '" << *call << "' seems infeasible: " << val2 << "==0 -> "
                  << val1 << "<=0" << std::endl;
      }
    }
  } else if (f1const) {
    if (val1 > 1e-6) {  // so  var2==1
      gi.getMIPWrapper()->setVarBounds(var2, 1.0, 1.0);
    }
  } else if (f2const) {
    if (val2 < 1e-6) {  // so  var1<=0
      gi.getMIPWrapper()->setVarUB(var1, 0.0);
    }
  } else {
    double coef = 1.0;
    gi.getMIPWrapper()->addIndicatorConstraint(
        var2, 0, 1, &var1, &coef, MIPWrapper::LinConType::LQ, 0.0,
        make_constraint_name("p_ind_", (gi.getMIPWrapper()->nAddedRows++), call));
    ++gi.getMIPWrapper()->nIndicatorConstr;
  }
}

/// var1==var2 if var3==1
template <class MIPWrapper>
void p_indicator_eq_if1(SolverInstanceBase& si, const Call* call) {
  auto& gi = dynamic_cast<MIPSolverinstance<MIPWrapper>&>(si);
  std::vector<double> coefs;
  std::vector<MIPSolver::Variable> vars;
  double rhs = 0.0;
  /// Looking at the bounded variables and the flag
  bool f1const = 0;
  bool f2const = 0;
  bool fBconst = 0;
  double val1;
  double val2;
  double valB;
  MIPSolver::Variable var1;
  MIPSolver::Variable var2;
  MIPSolver::Variable varB;
  if (call->arg(0)->isa<Id>()) {
    var1 = gi.exprToVar(call->arg(0));
    coefs.push_back(1.0);
    vars.push_back(var1);
  } else {
    f1const = 1;
    val1 = gi.exprToConst(call->arg(0));
    rhs -= val1;
  }
  if (call->arg(1)->isa<Id>()) {
    var2 = gi.exprToVar(call->arg(1));
    coefs.push_back(-1.0);
    vars.push_back(var2);
  } else {
    f2const = 1;
    val2 = gi.exprToConst(call->arg(1));
    rhs += val2;
  }
  if (call->arg(2)->isa<Id>()) {
    varB = gi.exprToVar(call->arg(2));
  } else {
    fBconst = 1;
    valB = gi.exprToConst(call->arg(2));
  }
  /// Check feas-ty. 1e-6 ?????????????   TODO
  if (f1const && f2const && fBconst) {
    if (fabs(val1 - val2) > 1e-6 && val2 > 0.999999) {
      si.setStatus(SolverInstance::UNSAT);
      if (gi.getMIPWrapper()->fVerbose) {
        std::cerr << "  Constraint '" << *call << "' seems infeasible: " << valB << "==0 -> "
                  << val1 << "==" << val2 << std::endl;
      }
    }
  } else if (f1const && f2const) {
    if (fabs(val1 - val2) > 1e-6) {  // so  varB=0
      gi.getMIPWrapper()->setVarBounds(varB, 0.0, 0.0);
    }
  } else if (fBconst) {
    if (val2 > 0.999999) {  // so  var1<=0
      remove_duplicates(vars, coefs);
      gi.getMIPWrapper()->addRow(
          static_cast<int>(vars.size()), &vars[0], &coefs[0], MIPWrapper::LinConType::EQ, rhs,
          MIPWrapper::MaskConsType_Normal,
          make_constraint_name("p_eq_", (gi.getMIPWrapper()->nAddedRows++), call));
    }
  } else {
    std::ostringstream ss;
    ss << "p_ind_" << (gi.getMIPWrapper()->nAddedRows++);
    gi.getMIPWrapper()->addIndicatorConstraint(
        varB, 1, static_cast<int>(coefs.size()), vars.data(), coefs.data(),
        MIPWrapper::LinConType::EQ, rhs,
        make_constraint_name("p_ind_", (gi.getMIPWrapper()->nAddedRows++), call));
    ++gi.getMIPWrapper()->nIndicatorConstr;
  }
}

/// Cumulative
template <class MIPWrapper>
void p_cumulative(SolverInstanceBase& si, const Call* call) {
  auto& gi = dynamic_cast<MIPSolverinstance<MIPWrapper>&>(si);

  assert(call->argCount() == 4);

  std::vector<MIPSolver::Variable> startTimes;
  gi.exprToVarArray(call->arg(0), startTimes);
  std::vector<double> durations;
  std::vector<double> demands;
  gi.exprToArray(call->arg(1), durations);
  gi.exprToArray(call->arg(2), demands);
  double b = gi.exprToConst(call->arg(3));

  gi.getMIPWrapper()->addCumulative(
      startTimes.size(), startTimes.data(), durations.data(), demands.data(), b,
      make_constraint_name("p_cumulative_", (gi.getMIPWrapper()->nAddedRows++), call));
}

template <class MIPWrapper>
void p_lex_lesseq_binary(SolverInstanceBase& si, const Call* call) {
  auto& gi = dynamic_cast<MIPSolverinstance<MIPWrapper>&>(si);

  assert(call->argCount() == 3);

  std::vector<MIPSolver::Variable> vec1;
  std::vector<MIPSolver::Variable> vec2;
  gi.exprToVarArray(call->arg(0), vec1);
  gi.exprToVarArray(call->arg(1), vec2);
  auto isModelCons = gi.exprToConst(call->arg(2));
  MZN_ASSERT_HARD(vec1.size() == vec2.size());

  gi.getMIPWrapper()->addLexLesseq(
      vec1.size(), vec1.data(), vec2.data(), (bool)isModelCons,
      make_constraint_name("p_lex_lesseq__orbisack_", (gi.getMIPWrapper()->nAddedRows++), call));
}

template <class MIPWrapper>
void p_lex_chain_lesseq_binary(SolverInstanceBase& si, const Call* call) {
  auto& gi = dynamic_cast<MIPSolverinstance<MIPWrapper>&>(si);

  assert(call->argCount() == 5);

  std::vector<MIPSolver::Variable> vars;
  gi.exprToVarArray(call->arg(0), vars);
  auto m = gi.exprToConst(call->arg(1));
  auto orbitopeType = gi.exprToConst(call->arg(2));
  auto resolveprop = gi.exprToConst(call->arg(3));
  auto isModelCons = gi.exprToConst(call->arg(4));

  gi.getMIPWrapper()->addLexChainLesseq(
      m, vars.size() / m, vars.data(), orbitopeType, (bool)resolveprop, (bool)isModelCons,
      make_constraint_name("p_lex_lesseq__orbisack_", (gi.getMIPWrapper()->nAddedRows++), call));
}

/// The XBZ cut generator
template <class MIPWrapper>
void p_xbz_cutgen(SolverInstanceBase& si, const Call* call) {
  auto& gi = dynamic_cast<MIPSolverinstance<MIPWrapper>&>(si);

  //     auto pCG = make_unique<XBZCutGen>();
  std::unique_ptr<XBZCutGen> pCG(new XBZCutGen(gi.getMIPWrapper()));

  assert(call->argCount() == 3);
  gi.exprToVarArray(call->arg(0), pCG->varX);
  gi.exprToVarArray(call->arg(1), pCG->varB);
  assert(pCG->varX.size() == pCG->varB.size());
  pCG->varZ = gi.exprToVar(call->arg(2));
  //     cout << "  NEXT_CUTGEN" << endl;
  //     pCG->print( cout );

  gi.registerCutGenerator(move(pCG));
}

/// Initialize the SEC cut generator
template <class MIPWrapper>
void p_sec_cutgen(SolverInstanceBase& si, const Call* call) {
  auto& gi = dynamic_cast<MIPSolverinstance<MIPWrapper>&>(si);

  std::unique_ptr<SECCutGen> pCG(new SECCutGen(gi.getMIPWrapper()));

  assert(call->argCount() == 1);
  gi.exprToVarArray(call->arg(0), pCG->varXij);  // WHAT ABOUT CONSTANTS?
  const double dN = sqrt(pCG->varXij.size());
  MZN_ASSERT_HARD(fabs(dN - round(dN)) < 1e-6);  // should be a square matrix
  pCG->nN = static_cast<int>(round(dN));
  const auto sVld = pCG->validate();
  MZN_ASSERT_HARD_MSG(sVld.empty(), "ERROR(s): " << sVld);
  //     cout << "  NEXT_CUTGEN" << endl;
  //     pCG->print( cout );

  gi.registerCutGenerator(move(pCG));
}

/// SCIP's bound disj
template <class MIPWrapper>
void p_bounds_disj(SolverInstanceBase& si, const Call* call) {
  auto& gi = dynamic_cast<MIPSolverinstance<MIPWrapper>&>(si);
  assert(6 == call->argCount());
  std::vector<double> fUB;
  std::vector<double> fUBF;
  std::vector<double> bnd;
  std::vector<double> bndF;
  std::vector<MIPSolver::Variable> vars;
  std::vector<MIPSolver::Variable> varsF;
  gi.exprToArray(call->arg(0), fUB);
  gi.exprToArray(call->arg(3), fUBF);
  gi.exprToArray(call->arg(1), bnd);
  gi.exprToArray(call->arg(4), bndF);
  gi.exprToVarArray(call->arg(2), vars);
  gi.exprToVarArray(call->arg(5), varsF);
  double coef = 1.0;
  gi.getMIPWrapper()->addBoundsDisj(
      fUB.size(), fUB.data(), bnd.data(), vars.data(), fUBF.size(), fUBF.data(), bndF.data(),
      varsF.data(),
      make_constraint_name("p_bounds_disj_", (gi.getMIPWrapper()->nAddedRows++), call));
}

template <class MIPWrapper>
void p_array_minimum(SolverInstanceBase& si, const Call* call) {
  auto& gi = dynamic_cast<MIPSolverinstance<MIPWrapper>&>(si);
  assert(2 == call->argCount());
  auto res = gi.exprToVar(call->arg(0));
  std::vector<MIPSolver::Variable> args;
  gi.exprToVarArray(call->arg(1), args);
  gi.getMIPWrapper()->addMinimum(
      res, args.size(), args.data(),
      make_constraint_name("p_minimum_", (gi.getMIPWrapper()->nAddedRows++), call));
}

/// fzn_[int/float]_times
template <class MIPWrapper>
void p_times(SolverInstanceBase& si, const Call* call) {
  auto& gi = dynamic_cast<MIPSolverinstance<MIPWrapper>&>(si);
  assert(3 == call->argCount());
  auto x = gi.exprToVar(call->arg(0));
  auto y = gi.exprToVar(call->arg(1));
  auto z = gi.exprToVar(call->arg(2));
  gi.getMIPWrapper()->addTimes(
      x, y, z, make_constraint_name("p_times_", (gi.getMIPWrapper()->nAddedRows++), call));
}

}  // namespace SCIPConstraints

template <class MIPWrapper>
void MIPSolverinstance<MIPWrapper>::registerConstraints() {
  GCLock lock;
  _constraintRegistry.add("int2float", SCIPConstraints::p_eq<MIPWrapper>);
  _constraintRegistry.add("bool_eq",
                          SCIPConstraints::p_eq<MIPWrapper>);  // for inconsistency reported in fzn
  _constraintRegistry.add("int_eq", SCIPConstraints::p_eq<MIPWrapper>);
  _constraintRegistry.add("int_le", SCIPConstraints::p_le<MIPWrapper>);
  _constraintRegistry.add("int_lin_eq", SCIPConstraints::p_int_lin_eq<MIPWrapper>);
  _constraintRegistry.add("int_lin_le", SCIPConstraints::p_int_lin_le<MIPWrapper>);
  //   _constraintRegistry.add("int_plus",     SCIPConstraints::p_plus<MIPWrapper>);
  //   _constraintRegistry.add("bool2int",     SCIPConstraints::p_eq<MIPWrapper>);
  _constraintRegistry.add("float_eq", SCIPConstraints::p_eq<MIPWrapper>);
  _constraintRegistry.add("float_le", SCIPConstraints::p_le<MIPWrapper>);
  _constraintRegistry.add("float_lin_eq", SCIPConstraints::p_float_lin_eq<MIPWrapper>);
  _constraintRegistry.add("float_lin_le", SCIPConstraints::p_float_lin_le<MIPWrapper>);
  //   _constraintRegistry.add("float_plus",   SCIPConstraints::p_plus<MIPWrapper>);

  /// XBZ cut generator
  _constraintRegistry.add("array_var_float_element__XBZ_lb__cutgen",
                          SCIPConstraints::p_xbz_cutgen<MIPWrapper>);
  _constraintRegistry.add("circuit__SECcuts", SCIPConstraints::p_sec_cutgen<MIPWrapper>);

  //////////////// GLOBALS / GENERAL CONSTRAINTS /////////////////////////////////////////
  /// Indicators, if supported by the solver
  _constraintRegistry.add("aux_int_le_zero_if_0__IND",
                          SCIPConstraints::p_indicator_le0_if0<MIPWrapper>);
  _constraintRegistry.add("aux_float_le_zero_if_0__IND",
                          SCIPConstraints::p_indicator_le0_if0<MIPWrapper>);
  _constraintRegistry.add("aux_float_eq_if_1__IND",
                          SCIPConstraints::p_indicator_eq_if1<MIPWrapper>);

  _constraintRegistry.add("fzn_cumulative_fixed_d_r", SCIPConstraints::p_cumulative<MIPWrapper>);

  _constraintRegistry.add("fzn_lex_lesseq__orbisack",
                          SCIPConstraints::p_lex_lesseq_binary<MIPWrapper>);

  _constraintRegistry.add("fzn_lex_chain_lesseq__orbitope",
                          SCIPConstraints::p_lex_chain_lesseq_binary<MIPWrapper>);

  _constraintRegistry.add("bounds_disj", SCIPConstraints::p_bounds_disj<MIPWrapper>);

  _constraintRegistry.add("fzn_array_float_minimum", SCIPConstraints::p_array_minimum<MIPWrapper>);

  _constraintRegistry.add("fzn_int_times", SCIPConstraints::p_times<MIPWrapper>);
  _constraintRegistry.add("fzn_float_times", SCIPConstraints::p_times<MIPWrapper>);
}

}  // namespace MiniZinc
