// * -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was ! distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* This is the main file for a mzn-cplex solver using a unified
 * linearization module && a flexible flattener-to-solver interface
 */

#ifdef _MSC_VER 
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <memory>
#include <chrono>

using namespace std;

#include <minizinc/solvers/MIP/MIP_solverinstance.hh>

using namespace MiniZinc;

SolverFactory* MiniZinc::SolverFactory::createF_MIP() {
  return new MIP_SolverFactory;
}

string MIP_SolverFactory::getVersion()
{
  string v = "  MIP solver plugin, compiled  " __DATE__ ", using: "
    + MIP_WrapperFactory::getVersion();
  return v;
}


MIP_solver::Variable MIP_solverinstance::exprToVar(Expression* arg) {
  if (Id* ident = arg->dyn_cast<Id>()) {
    return _variableMap.get(ident->decl()->id());
  } else
    return mip_wrap->addLitVar( exprToConst( arg ) );
}

void MIP_solverinstance::exprToVarArray(Expression* arg, vector<VarId> &vars) {
  ArrayLit* al = eval_array_lit(getEnv()->envi(), arg);
  vars.clear();
  vars.reserve(al->v().size());
  for (unsigned int i=0; i<al->v().size(); i++)
    vars.push_back(exprToVar(al->v()[i]));
}

double MIP_solverinstance::exprToConst(Expression* e) {
    if (IntLit* il = e->dyn_cast<IntLit>()) {
      return ( il->v().toInt() );
    } else if (FloatLit* fl = e->dyn_cast<FloatLit>()) {
      return ( fl->v().toDouble() );
    } else if (BoolLit* bl = e->dyn_cast<BoolLit>()) {
      return ( bl->v() );
    } else {
      throw InternalError("Expected a numeric/bool literal");
    }
}

void MIP_solverinstance::exprToArray(Expression* arg, vector<double> &vals) {
  ArrayLit* al = eval_array_lit(getEnv()->envi(), arg);
  vals.clear();
  vals.reserve(al->v().size());
  for (unsigned int i=0; i<al->v().size(); i++) {
    vals.push_back( exprToConst( al->v()[i] ) );
  }
}

namespace SCIPConstraints {

  bool CheckAnnUserCut(const Call* call) {
    if(!call->ann().isEmpty()) {
      if(call->ann().contains(constants().ann.user_cut)) {
        return true;
      }
    }
    return false;
  }
  bool CheckAnnLazyConstraint(const Call* call) {
    if(!call->ann().isEmpty()) {
      if(call->ann().contains(constants().ann.lazy_constraint)) {
        return true;
      }
    }
    return false;
  }
  int GetMaskConsType(const Call* call) {
    int mask=0;
      const bool fUC = CheckAnnUserCut(call);
      const bool fLC = CheckAnnLazyConstraint(call);
      if (fUC) {
        mask |= MIP_wrapper::MaskConsType_Usercut;
      }
      if (fLC) {
        mask |= MIP_wrapper::MaskConsType_Lazy;
      }
      if (!fUC && !fLC)
        mask |= MIP_wrapper::MaskConsType_Normal;
      return mask;
//       return MIP_wrapper::MaskConsType_Normal;    // recognition fails
  }

  void p_lin(SolverInstanceBase& si, const Call* call, MIP_wrapper::LinConType lt) {
    MIP_solverinstance& gi = dynamic_cast<MIP_solverinstance&>( si );
    Env& _env = gi.env();
    ASTExprVec<Expression> args = call->args();
//     ArrayLit* al = eval_array_lit(_env.envi(), args[0]);
//     int nvars = al->v().size();
    vector<double> coefs;
//     gi.exprToArray(args[0], coefs);
    vector<MIP_solverinstance::VarId> vars;
//     gi.exprToVarArray(args[1], vars);
    IntVal ires;
    FloatVal fres;

    double rhs;
    if(args[2]->type().isint()) {
      ires = eval_int(_env.envi(), args[2]);
      rhs = ires.toInt();
    } else if(args[2]->type().isfloat()) {
      fres = eval_float(_env.envi(), args[2]);
      rhs = fres.toDouble();
    } else {
      throw InternalError("p_lin: rhs unknown type");
    }

    /// Process coefs & vars together to eliminate literals (problem with Gurobi's updatemodel()'s)
    ArrayLit* alC = eval_array_lit(_env.envi(), args[0]);
    coefs.reserve(alC->v().size());
    ArrayLit* alV = eval_array_lit(_env.envi(), args[1]);
    vars.reserve(alV->v().size());
    for (unsigned int i=0; i<alV->v().size(); i++) {
      const double dCoef = gi.exprToConst( alC->v()[i] );
      if (Id* ident = alV->v()[i]->dyn_cast<Id>()) {
        coefs.push_back( dCoef );
        vars.push_back( gi.exprToVar( ident ) );
      } else
        rhs -= dCoef*gi.exprToConst( alV->v()[i] );
    }
    assert(coefs.size() == vars.size());

    /// Check feas-ty
    if ( coefs.empty() ) {
      if ( (MIP_wrapper::LinConType::EQ==lt && 1e-5 < fabs( rhs ))
        || (MIP_wrapper::LinConType::LQ==lt && -1e-5 > ( rhs ))
        || (MIP_wrapper::LinConType::GQ==lt && 1e-5 < ( rhs ))
      ) {
        si._status = SolverInstance::UNSAT;
        if ( gi.getMIPWrapper()->fVerbose )
          cerr << "  Constraint '" << *call
            << "' seems infeasible: simplified to 0 (rel) " << rhs
            << endl;
      }
    } else {
      // See if the solver adds indexation itself: no.
      std::stringstream ss;
      ss << "p_lin_" << (gi.getMIPWrapper()->nAddedRows++);
      gi.getMIPWrapper()->addRow(coefs.size(), &vars[0], &coefs[0], lt, rhs,
                                GetMaskConsType(call), ss.str());
    }
  }

  void p_int_lin_le(SolverInstanceBase& si, const Call* call) {
    p_lin(si, call, MIP_wrapper::LQ);
  }
  void p_int_lin_eq(SolverInstanceBase& si, const Call* call) {
    p_lin(si, call, MIP_wrapper::EQ);
  }
  void p_float_lin_le(SolverInstanceBase& si, const Call* call) {
    p_lin(si, call, MIP_wrapper::LQ);
  }
  void p_float_lin_eq(SolverInstanceBase& si, const Call* call) {
    p_lin(si, call, MIP_wrapper::EQ);
  }

  // The non-_lin constraints happen in a failed model || in a non-optimized one:
   void p_non_lin(SolverInstanceBase& si, const Call* call, MIP_wrapper::LinConType nCmp) {
      MIP_solverinstance& gi = dynamic_cast<MIP_solverinstance&>( si );
      ASTExprVec<Expression> args = call->args();
      vector<double> coefs;
      vector<MIP_solver::Variable> vars;
      double rhs = 0.0;
      if ( args[0]->isa<Id>() ) {
        coefs.push_back( 1.0 );
        vars.push_back( gi.exprToVar(args[0]) );
      } else
        rhs -= gi.exprToConst(args[0]);
      if ( args[1]->isa<Id>() ) {
        coefs.push_back( -1.0 );
        vars.push_back( gi.exprToVar(args[1]) );
      } else
        rhs += gi.exprToConst(args[1]);
      /// Check feas-ty
      if ( coefs.empty() ) {
        if ( (MIP_wrapper::LinConType::EQ==nCmp && 1e-5 < fabs( rhs ))
          || (MIP_wrapper::LinConType::LQ==nCmp && -1e-5 > ( rhs ))
          || (MIP_wrapper::LinConType::GQ==nCmp && 1e-5 < ( rhs ))
        ) {
          si._status = SolverInstance::UNSAT;
          if ( gi.getMIPWrapper()->fVerbose )
            cerr << "  Constraint '" << *call
              << "' seems infeasible: simplified to 0 (rel) " << rhs
              << endl;
        }
      } else {
        std::stringstream ss;
        ss << "p_eq_" << (gi.getMIPWrapper()->nAddedRows++);
        gi.getMIPWrapper()->addRow(vars.size(), &vars[0], &coefs[0], nCmp, rhs,
                                GetMaskConsType(call), ss.str());
      }
    }
   void p_eq(SolverInstanceBase& si, const Call* call) {
     p_non_lin( si, call, MIP_wrapper::EQ );
   }
   void p_le(SolverInstanceBase& si, const Call* call) {
     p_non_lin( si, call, MIP_wrapper::LQ );
   }
   
  /// The XBZ cut generator
  void p_XBZ_cutgen(SolverInstanceBase& si, const Call* call) {
    MIP_solverinstance& gi = dynamic_cast<MIP_solverinstance&>( si );
    Env& _env = gi.env();
    
//     auto pCG = make_unique<XBZCutGen>();
    unique_ptr<XBZCutGen> pCG( new XBZCutGen( gi.getMIPWrapper() ) );
    
    ASTExprVec<Expression> args = call->args();
    assert( args.size()==3 );
    gi.exprToVarArray(args[0], pCG->varX);
    gi.exprToVarArray(args[1], pCG->varB);
    assert(pCG->varX.size() == pCG->varB.size());
    pCG->varZ = gi.exprToVar(args[2]);
//     cout << "  NEXT_CUTGEN" << endl;
//     pCG->print( cout );
    
    gi.registerCutGenerator( move( pCG ) );
  }
}

void MIP_solverinstance::registerConstraints() {
  _constraintRegistry.add(ASTString("int2float"),    SCIPConstraints::p_eq);
  _constraintRegistry.add(ASTString("bool_eq"),      SCIPConstraints::p_eq);   // for inconsistency reported in fzn
  _constraintRegistry.add(ASTString("int_eq"),       SCIPConstraints::p_eq);
  _constraintRegistry.add(ASTString("int_le"),       SCIPConstraints::p_le);
  _constraintRegistry.add(ASTString("int_lin_eq"),   SCIPConstraints::p_int_lin_eq);
  _constraintRegistry.add(ASTString("int_lin_le"),   SCIPConstraints::p_int_lin_le);
//   _constraintRegistry.add(ASTString("int_plus"),     SCIPConstraints::p_plus);
//   _constraintRegistry.add(ASTString("bool2int"),     SCIPConstraints::p_eq);
  _constraintRegistry.add(ASTString("float_eq"),     SCIPConstraints::p_eq);
  _constraintRegistry.add(ASTString("float_le"),     SCIPConstraints::p_le);
  _constraintRegistry.add(ASTString("float_lin_eq"), SCIPConstraints::p_float_lin_eq);
  _constraintRegistry.add(ASTString("float_lin_le"), SCIPConstraints::p_float_lin_le);
//   _constraintRegistry.add(ASTString("float_plus"),   SCIPConstraints::p_plus);
  
  _constraintRegistry.add(ASTString("array_var_float_element__XBZ_lb__cutgen"),
                          SCIPConstraints::p_XBZ_cutgen);
  
}

void MIP_solverinstance::printStatistics(ostream& os, bool fLegend)
{
  auto nn = std::chrono::system_clock::now();
  auto n_c = std::chrono::system_clock::to_time_t( nn );
    {
      std::ios oldState(nullptr);
      oldState.copyfmt(std::cout);
      os.precision(12);
      os << "  % MIP Status: " << mip_wrap->getStatusName() << endl;
      if (fLegend)
        os << "  % obj, bound, CPU_time, nodes (left): ";
      os << mip_wrap->getObjValue() << ",  ";
      os << mip_wrap->getBestBound() << ",  ";
      os.setf( ios::fixed );
      os.precision( 3 );
      os << mip_wrap->getCPUTime() << ",  ";
      os << mip_wrap->getNNodes();
      if (mip_wrap->getNOpen())
        os << " ( " << mip_wrap->getNOpen() << " )";
      os << "    " << std::ctime( &n_c );
      //  ctime already adds EOL.     os << endl;
      os.copyfmt( oldState );
    }
}


void HandleSolutionCallback(const MIP_wrapper::Output& out, void* pp) {
  // multi-threading? TODO
  MIP_solverinstance* pSI = (MIP_solverinstance*)( pp );
  assert(pSI);
  /// Not for -a:
//   if (fabs(pSI->lastIncumbent - out.objVal) > 1e-12*(1.0 + fabs(out.objVal))) {
    pSI->lastIncumbent = out.objVal;
  
  try {     /// Sometimes the intermediate output is wrong, especially in SCIP
    pSI->printSolution();            // The solution in [out] is not used  TODO 
  } catch (const Exception& e) {
    std::cerr << std::endl;
    std::cerr << "  Error when evaluating an intermediate solution:  " << e.what() << ": " << e.msg() << std::endl;
  }
  catch (const exception& e) {
    std::cerr << std::endl;
    std::cerr << "  Error when evaluating an intermediate solution:  " << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << std::endl;
    std::cerr << "  Error when evaluating an intermediate solution:  " << "  UNKNOWN EXCEPTION." << std::endl;
  }
//   }
}

void HandleCutCallback(const MIP_wrapper::Output& out, MIP_wrapper::CutInput& in,
                       void* pp, bool fMIPSol) {
  // multi-threading? TODO
  MIP_solverinstance* pSI = (MIP_solverinstance*)( pp );
  assert(pSI);
  assert(&out);
  assert(&in);
  pSI->genCuts( out, in, fMIPSol );
}



SolverInstance::Status MIP_solverinstance::solve(void) {
  SolveI* solveItem = getEnv()->flat()->solveItem();
  if (solveItem->st() != SolveI::SolveType::ST_SAT) {
    if (solveItem->st() == SolveI::SolveType::ST_MAX) {
      getMIPWrapper()->setObjSense(1);
      getMIPWrapper()->setProbType(1);
      if (mip_wrap->fVerbose)
        cerr << "    MIP_solverinstance: this is a MAXimization problem." << endl;
    } else {
      getMIPWrapper()->setObjSense(-1);
      getMIPWrapper()->setProbType(-1);
      if (mip_wrap->fVerbose)
        cerr << "    MIP_solverinstance: this is a MINimization problem." << endl;
    }
    if (mip_wrap->fVerbose) {
      cerr << "    MIP_solverinstance: bounds for the objective function: "
        << dObjVarLB << ", " << dObjVarUB << endl;
    }
  } else {
    getMIPWrapper()->setProbType(0);
    if (mip_wrap->fVerbose)
      cerr << "    MIP_solverinstance: this is a SATisfiability problem." << endl;
  }
  
  
  lastIncumbent = 1e200;                  // for callbacks
  MIP_wrapper::Status sw;
  if ( SolverInstance::UNSAT == _status )     // already deduced - exit now
    return _status;
  if ( getMIPWrapper()->getNCols() ) {     // If any variables, we need to run solver just to get values?
    getMIPWrapper()->provideSolutionCallback(HandleSolutionCallback, this);
    if ( cutGenerators.size() )  // only then, can modify presolve
      getMIPWrapper()->provideCutCallback(HandleCutCallback, this);
    getMIPWrapper()->solve();
  //   printStatistics(cout, 1);   MznSolver does this (if it wants)
    sw = getMIPWrapper()->getStatus();
  } else {
    if ( mip_wrap->fVerbose )
      cerr << "  MIP_solverinstance: no constraints - skipping actual solution phase." << endl;
    sw = MIP_wrapper::Status::OPT;
  }
  SolverInstance::Status s = SolverInstance::UNKNOWN;
  switch(sw) {
    case MIP_wrapper::Status::OPT:
      if ( SolveI::SolveType::ST_SAT != getEnv()->flat()->solveItem()->st() ) {
        s = SolverInstance::OPT;
      } else {
        s = SolverInstance::SAT;    // For SAT problems, just say SAT unless we know it's complete
      }
      break;
    case MIP_wrapper::Status::SAT:
      s = SolverInstance::SAT;
      break;
    case MIP_wrapper::Status::UNSAT:
      s = SolverInstance::UNSAT;
      break;
    case MIP_wrapper::Status::UNBND:
      s = SolverInstance::UNBND;
      break;
    case MIP_wrapper::Status::UNSATorUNBND:
      s = SolverInstance::UNSATorUNBND;
      break;
    case MIP_wrapper::Status::UNKNOWN:
      s = SolverInstance::UNKNOWN;
      break;
    default:
      s = SolverInstance::ERROR;
  }
  return s;
}

void MIP_solverinstance::processFlatZinc(void) {
  /// last-minute solver params
  mip_wrap->fVerbose = (getOptions().getBoolParam(constants().opts.verbose.str(), false));

  SolveI* solveItem = getEnv()->flat()->solveItem();
  VarDecl* objVd = NULL;

  if (solveItem->st() != SolveI::SolveType::ST_SAT) {
    if(Id* id = solveItem->e()->dyn_cast<Id>()) {
      objVd = id->decl();
    } else {
      std::cerr << "Objective must be Id: " << solveItem->e() << std::endl;
      throw InternalError("Objective must be Id");
    }
  }

  for (VarDeclIterator it = getEnv()->flat()->begin_vardecls(); it != getEnv()->flat()->end_vardecls(); ++it) {
    if (it->removed()) {
      continue;
    }
    VarDecl* vd = it->e();
    if(!vd->ann().isEmpty()) {
      if(vd->ann().containsCall(constants().ann.output_array.aststr()) ||
          vd->ann().contains(constants().ann.output_var)
        ) {
        _varsWithOutput.push_back(vd);
//         std::cerr << (*vd);
//         if ( vd->e() )
//           cerr << " = " << (*vd->e());
//         cerr << endl;
      }
    }
    if (vd->type().dim() == 0 && it->e()->type().isvar() && !it->removed()) {
      MiniZinc::TypeInst* ti = it->e()->ti();
      MIP_wrapper::VarType vType = MIP_wrapper::VarType::REAL;     // fInt = false;
      if (ti->type().isvarint() || ti->type().isint())
        vType = MIP_wrapper::VarType::INT;
      else if (ti->type().isvarbool() || ti->type().isbool()) {
        vType = MIP_wrapper::VarType::BINARY;
      } else if (ti->type().isvarfloat() || ti->type().isfloat()) {
      } else {
        std::stringstream ssm;
        ssm << "This type of var is ! handled by MIP: " << *it << std::endl;
        ssm << "  VarDecl flags (ti, bt, st, ot): "
          << ti->type().ti()
          << ti->type().bt()
          << ti->type().st()
          << ti->type().ot()
          << ", dim == " << ti->type().dim()
          << endl;
        throw InternalError(ssm.str());
      }
      double lb=0.0, ub=1.0;  // for bool
      if (ti->domain()) {
        if (MIP_wrapper::VarType::REAL == vType) {
          FloatBounds fb = compute_float_bounds(getEnv()->envi(), it->e()->id());
          if (fb.valid) {
            lb = fb.l.toDouble();
            ub = fb.u.toDouble();
          } else {
            lb = 1.0;
            ub = 0.0;
          }
        } else if (MIP_wrapper::VarType::INT == vType) {
          IntBounds ib = compute_int_bounds(getEnv()->envi(), it->e()->id());
          if (ib.valid) {  // Normally should be
            lb = ib.l.toInt();
            ub = ib.u.toInt();
          } else {
            lb = 1;
            ub = 0;
          }
        } 
      } else if (MIP_wrapper::VarType::BINARY != vType) {
        lb = -getMIPWrapper()->getInfBound();  // if just 1 bound inf, using MZN's default?  TODO
        ub = -lb;
      }

//       IntSetVal* dom = eval_intset(env,vdi->e()->ti()->domain());
//       if (dom->size() > 1)
//         throw runtime_error("MIP_solverinstance: domains with holes ! supported, use --MIPdomains");

      VarId res;
      Id* id = it->e()->id();
      MZN_ASSERT_HARD( id == id->decl()->id() );   // Assume all unified
      MZN_ASSERT_HARD( it->e() == id->decl() );    // Assume all unified
      double obj = vd==objVd ? 1.0 : 0.0;
      auto decl00 = follow_id_to_decl( it->e() );
      MZN_ASSERT_HARD ( decl00->isa<VarDecl>() );
      {
        auto vd00 = decl00->dyn_cast<VarDecl>();
        if ( 0!=vd00->e() ) {
          // Should be a const
          auto dRHS = exprToConst( vd00->e() );
          lb = max( lb, dRHS );
          ub = min( ub, dRHS );
        }
        if ( it->e()!=vd00 ) {    // A different vardecl
          res = exprToVar( vd00->id() );                 // Assume FZN is sorted.
          MZN_ASSERT_HARD( !getMIPWrapper()->fPhase1Over ); // Still can change colUB, colObj
          /// Tighten the ini-expr's bounds
          lb = getMIPWrapper()->colLB.at( res ) = max( getMIPWrapper()->colLB.at( res ), lb );
          ub = getMIPWrapper()->colUB.at( res ) = min( getMIPWrapper()->colUB.at( res ), ub );
          if ( 0.0!=obj ) {
            getMIPWrapper()->colObj.at( res ) = obj;
          }
        } else {
          res = getMIPWrapper()->addVar(obj, lb, ub, vType, id->str().c_str());
        }
      }
      /// Test infeasibility
      if ( lb>ub ) {
        _status = SolverInstance::UNSAT;
        if ( getMIPWrapper()->fVerbose )
          cerr << "  VarDecl '" << *(it->e())
            << "' seems infeasible: computed bounds [" << lb << ", " << ub << ']'
            << endl;
      }
      if ( 0.0!=obj ) {
        dObjVarLB = lb;
        dObjVarUB = ub;
        getMIPWrapper()->output.nObjVarIndex = res;
        if ( getMIPWrapper()->fVerbose )
          cerr << "  MIP: objective variable index (0-based): " << res << endl;
      }
      _variableMap.insert(id, res);
      assert( res == _variableMap.get(id) );
    }
  }
  if (mip_wrap->fVerbose && mip_wrap->sLitValues.size())
    cerr << "  MIP_solverinstance: during Phase 1,  "
      << mip_wrap->nLitVars << " literals with "
      << mip_wrap-> sLitValues.size() << " values used." << endl;
  if (! getMIPWrapper()->fPhase1Over)
    getMIPWrapper()->addPhase1Vars(); 

  if (mip_wrap->fVerbose)
    cerr << "  MIP_solverinstance: adding constraints..." << flush;
  
  for (ConstraintIterator it = getEnv()->flat()->begin_constraints(); it != getEnv()->flat()->end_constraints(); ++it) {
    if (!it->removed()) {
      if (Call* c = it->e()->dyn_cast<Call>()) {
        _constraintRegistry.post(c);
      }
    }
  }

  if (mip_wrap->fVerbose)
    cerr << " done, " << mip_wrap->getNRows() << " rows && "
    << mip_wrap->getNCols() << " columns in total." << endl;
  if (mip_wrap->fVerbose && mip_wrap->sLitValues.size())
    cerr << "  MIP_solverinstance: overall,  "
      << mip_wrap->nLitVars << " literals with "
      << mip_wrap-> sLitValues.size() << " values used." << endl;
}  // processFlatZinc

Expression* MIP_solverinstance::getSolutionValue(Id* id) {
  id = id->decl()->id();

  if(id->type().isvar()) {
    MIP_solver::Variable var = exprToVar(id);
    double val = getMIPWrapper()->getValues()[var];
    switch (id->type().bt()) {
      case Type::BT_INT: return IntLit::a(round_to_longlong(val));
      case Type::BT_FLOAT: return FloatLit::a(val);
      case Type::BT_BOOL: return new BoolLit(Location(), round_to_longlong(val));
      default: return NULL;
    }
  } else {
    return id->decl()->e();
  }
}

void MIP_solverinstance::genCuts(const MIP_wrapper::Output& slvOut,
                                 MIP_wrapper::CutInput& cutsIn, bool fMIPSol) {
  for ( auto& pCG : cutGenerators ) {
    if ( !fMIPSol || pCG->getMask()&MIP_wrapper::MaskConsType_Lazy )
      pCG->generate( slvOut, cutsIn );
  }
  /// Select some most violated? TODO
}

void XBZCutGen::generate(const MIP_wrapper::Output& slvOut, MIP_wrapper::CutInput& cutsIn) {
  assert( pMIP );
  const int n = varX.size();
  assert( n==varB.size() );
  MIP_wrapper::CutDef cut( MIP_wrapper::GQ, MIP_wrapper::MaskConsType_Usercut );
  cut.addVar( varZ, -1.0 );
  for ( int i=0; i<n; ++i ) {
    const int ix = varX[ i ];
    const int ib = varB[ i ];
    assert( ix>=0 && ix<slvOut.nCols );
    assert( ib>=0 && ib<slvOut.nCols );
    const double theXi = slvOut.x[ ix ];
    const double theBi = slvOut.x[ ib ];
    const double LBXi = pMIP->colLB[ ix ];
    const double UBXi = pMIP->colUB[ ix ];  // tighter bounds from presolve?  TODO
    bool fi = ( theXi + LBXi * ( theBi - 1.0 ) - UBXi * theBi < 0.0 );
    if ( fi ) {
      cut.addVar( ix, 1.0 );
      cut.addVar( ib, LBXi );
      cut.rhs += LBXi;
    } else {
      cut.addVar( ib, UBXi );
    }
  }
  double dViol = cut.computeViol( slvOut.x, slvOut.nCols );
  if ( dViol > 0.01 ) {   // ?? PARAM?  TODO
    cutsIn.push_back( cut );
    cerr << " vi" << dViol << flush;
//     cout << cut.rmatind.size() << ' '
//       << cut.rhs << "  cutlen, rhs. (Sense fixed to GQ) " << endl;
//     for ( int i=0; i<cut.rmatind.size(); ++i )
//       cout << cut.rmatind[i] << ' ';
//     cout << endl;
//     for ( int i=0; i<cut.rmatind.size(); ++i )
//       cout << cut.rmatval[i] << ' ';
//     cout << endl;
  }
}

void XBZCutGen::print( ostream& os )
{
  os
    << varZ << '\n'
    << varX.size() << '\n';
  for ( int i=0; i<varX.size(); ++i )
    os << varX[i] << ' ';
  os << endl;
  for ( int i=0; i<varB.size(); ++i )
    os << varB[i] << ' ';
  os << endl;
}
