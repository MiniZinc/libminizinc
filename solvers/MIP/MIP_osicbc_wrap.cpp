// * -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <minizinc/solvers/MIP/MIP_osicbc_wrap.hh>
#include <minizinc/utils.hh>

#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <coin/CbcConfig.h>
#include <coin/CbcEventHandler.hpp>
#include <coin/CbcSolver.hpp>
#include <coin/CglCutGenerator.hpp>
#include <coin/CglPreProcess.hpp>
#include <coin/ClpConfig.h>
#include <coin/CoinSignal.hpp>

using namespace std;

#define WANT_SOLUTION

string MIPosicbcWrapper::getDescription(FactoryOptions& factoryOpt,
                                        MiniZinc::SolverInstanceBase::Options* /*opt*/) {
  string v = "MIP wrapper for COIN-BC ";
  v += CBC_VERSION;  // E.g., 2.9 stable or 2.9.7 latest release
  v += ",  using CLP ";
  v += CLP_VERSION;
  v += "  Compiled  " __DATE__ "  " __TIME__;
  return v;
}

string MIPosicbcWrapper::getVersion(FactoryOptions& factoryOpt,
                                    MiniZinc::SolverInstanceBase::Options* /*opt*/) {
  return string(CBC_VERSION) + "/" + string(CLP_VERSION);
}

string MIPosicbcWrapper::getId() { return "coin-bc"; }

string MIPosicbcWrapper::getName() { return "COIN-BC"; }

vector<string> MIPosicbcWrapper::getTags() {
  return {"mip", "float", "api", "osicbc", "coinbc", "cbc"};
}

vector<string> MIPosicbcWrapper::getStdFlags() { return {"-i", "-p", "-s", "-v"}; }

void MIPosicbcWrapper::Options::printHelp(ostream& os) {
  os << "COIN-BC MIP wrapper options:"
     << std::endl
     // -s                  print statistics
     //            << "  --readParam <file>  read OSICBC parameters from file
     //               << "--writeParam <file> write OSICBC parameters to file
     //               << "--tuneParam         instruct OSICBC to tune parameters instead of solving
     << "  --cbcArgs, --cbcFlags, --cbc-flags, --backend-flags \"args\"\n"
        "    command-line args passed to callCbc, e.g., \"-cuts off -preprocess off -passc 1\"."
     << std::endl
     << "  --cbcArg, --cbcFlag, --cbc-flag, --backend-flag \"args\"\n"
        "    same as above but with a single flag."
     << std::endl
     //  \"-preprocess off\" recommended in 2.9.6
     << "  --writeModel <file>" << endl
     << "    write model to <file> (.mps)" << std::endl
     << "  -i\n    print intermediate solutions for optimization problems\n"
        "    (not from FeasPump. Can be slow.)"
     << std::endl
     << "  -p <N>, --parallel <N>\n    use N threads, default: 1. CBC should be configured with "
        "--enable-cbc-parallel"
     << std::endl
     //   << "--nomippresolve     disable MIP presolving   NOT IMPL" << std::endl
     << "  --solver-time-limit <N>\n    stop search after N milliseconds"
     << std::endl
     //   << "--workmem <N>       maximal amount of RAM used, MB" << std::endl
     //   << "--readParam <file>  read OSICBC parameters from file" << std::endl
     //   << "--writeParam <file> write OSICBC parameters to file" << std::endl
     //   << "--tuneParam         instruct OSICBC to tune parameters instead of solving   NOT IMPL"

     << "  --absGap <n>\n    absolute gap |primal-dual| to stop" << std::endl
     << "  --relGap <n>\n    relative gap |primal-dual|/<solver-dep> to stop. Default 1e-8, set <0 "
        "to use backend's default"
     << std::endl
     << "  --intTol <n>\n    integrality tolerance for a variable. Default 1e-8"
     << std::endl
     //   << "--objDiff <n>       objective function discretization. Default 1.0" << std::endl

     << std::endl;
}

bool MIPosicbcWrapper::Options::processOption(int& i, std::vector<std::string>& argv,
                                              const std::string& workingDir) {
  MiniZinc::CLOParser cop(i, argv);
  std::string buffer;
  if (cop.get("-i")) {
    flagIntermediate = true;
  } else if (string(argv[i]) == "-f") {  // NOLINT: Allow repeated empty if
    //     std::cerr << "  Flag -f: ignoring fixed strategy anyway." << std::endl;
  } else if (cop.get("--writeModel", &buffer)) {
    sExportModel = MiniZinc::FileUtils::file_path(buffer, workingDir);
  } else if (cop.get("-p --parallel", &nThreads)) {
    // Parsed by referenced
  } else if (cop.get("--solver-time-limit", &nTimeout)) {  // NOLINT: Allow repeated empty if
    // Parsed by referenced
  } else if (cop.get("--workmem", &nWorkMemLimit)) {  // NOLINT: Allow repeated empty if
    // Parsed by referenced
  } else if (cop.get("--readParam", &sReadParams)) {  // NOLINT: Allow repeated empty if
    // Parsed by referenced
  } else if (cop.get("--writeParam", &sWriteParams)) {  // NOLINT: Allow repeated empty if
    // Parsed by referenced
  } else if (cop.get("--cbcArgs --cbcFlags --cbc-flags --solver-flags --backend-flags", &buffer)) {
    auto cmdLine = MiniZinc::FileUtils::parse_cmd_line(buffer);
    for (auto& s : cmdLine) {
      cbcCmdOptions.push_back(s);
    }
  } else if (cop.get("--cbcArg --cbcFlag --cbc-flag --solver-flag --backend-flag", &buffer)) {
    cbcCmdOptions.push_back(buffer);
  } else if (cop.get("--absGap", &absGap)) {  // NOLINT: Allow repeated empty if
    // Parsed by referenced
  } else if (cop.get("--relGap", &relGap)) {  // NOLINT: Allow repeated empty if
    // Parsed by referenced
  } else if (cop.get("--intTol", &intTol)) {  // NOLINT: Allow repeated empty if
    // Parsed by referenced
    //   } else if ( cop.get( "--objDiff", &objDiff ) ) {
  } else {
    return false;
  }
  return true;
}

namespace {
void remove_chars(std::string& s, const std::string& cs) {
  for (char c : cs) {
    auto i = s.find(c);
    while (i != std::string::npos) {
      s.erase(i, 1);
      i = s.find(c);
    }
  }
}
}  // namespace

std::vector<MiniZinc::SolverConfig::ExtraFlag> MIPosicbcWrapper::getExtraFlags(
    FactoryOptions& factoryOpt) {
  OsiClpSolverInterface osi;
  CbcModel model(osi);
  CbcSolverUsefulData info;
  CbcMain0(model, info);

  std::vector<MiniZinc::SolverConfig::ExtraFlag> res;
  res.reserve(info.parameters_.size());

  for (auto param : info.parameters_) {
    auto name = param.name();
    if (name == "?" || name == "???" || name == "allCommands" || name == "moreSpecialOptions" ||
        name == "moreTune" || name == "mipOptions" || name == "moreMipOptions" ||
        name == "more2MipOptions") {
      continue;
    }

    // strip braces from name
    remove_chars(name, "()");
    auto desc = param.shortHelp();
    auto t = param.type();
    MiniZinc::SolverConfig::ExtraFlag::FlagType param_type;
    std::vector<std::string> param_range;
    std::string param_default;
    if (t <= 100) {
      param_type = MiniZinc::SolverConfig::ExtraFlag::FlagType::T_FLOAT;
      param_range.push_back(std::to_string(param.lowerDoubleValue()));
      param_range.push_back(std::to_string(param.upperDoubleValue()));
      param_default = std::to_string(param.doubleParameter(model));
    } else if (t <= 200) {
      param_type = MiniZinc::SolverConfig::ExtraFlag::FlagType::T_INT;
      param_range.push_back(std::to_string(param.lowerIntValue()));
      param_range.push_back(std::to_string(param.upperIntValue()));
      param_default = std::to_string(param.intParameter(model));
    } else if (t <= 400) {
      auto allowed = param.definedKeywords();
      if (allowed.size() == 2 && (allowed[0] == "on" && allowed[1] == "off" ||
                                  allowed[0] == "off" && allowed[1] == "on")) {
        param_type = MiniZinc::SolverConfig::ExtraFlag::FlagType::T_BOOL;
      } else {
        param_type = MiniZinc::SolverConfig::ExtraFlag::FlagType::T_STRING;
      }
      for (auto v : allowed) {
        remove_chars(v, "!?");
        param_range.push_back(v);
      }
      param_default = param.currentOption();
      remove_chars(param_default, "!?");
    } else {
      // action, not parameter, so skip
      continue;
    }

    res.emplace_back("--cbc-" + name, desc, param_type, param_range, param_default);
  }

  return res;
}

void MIPosicbcWrapper::wrapAssert(bool cond, const string& msg, bool fTerm) {
  if (!cond) {
    //       strcpy(_osicbcBuffer, "[NO ERROR STRING GIVEN]");
    //       CBCgeterrorstring (env, status, _osicbcBuffer);
    string msgAll = ("  MIPosicbcWrapper runtime error:  " + msg + "  " + _osicbcBuffer);
    cerr << msgAll << endl;
    if (fTerm) {
      cerr << "TERMINATING." << endl;
      throw runtime_error(msgAll);
    }
  }
}

void MIPosicbcWrapper::doAddVars(size_t n, double* obj, double* lb, double* ub,
                                 MIPWrapper::VarType* vt, string* names) {
  /// Convert var types:
  //   vector<char> ctype(n);
  //   vector<char*> pcNames(n);
  CoinPackedVector cpv;
  vector<CoinPackedVectorBase*> pCpv(n, &cpv);
  _osi.addCols(n, pCpv.data(), lb, ub, obj);  // setting integer & names later
  //   status = CBCnewcols (env, lp, n, obj, lb, ub, &ctype[0], &pcNames[0]);
  //   wrapAssert( !status,  "Failed to declare variables." );
}

void MIPosicbcWrapper::addRow(int nnz, int* rmatind, double* rmatval, MIPWrapper::LinConType sense,
                              double rhs, int mask, const string& rowName) {
  /// Convert var types:
  double rlb = rhs;
  double rub = rhs;
  switch (sense) {
    case LQ:
      rlb = -_osi.getInfinity();
      break;
    case EQ:
      break;
    case GQ:
      rub = _osi.getInfinity();
      break;
    default:
      throw runtime_error("  MIPWrapper: unknown constraint type");
  }
  // ignoring mask for now.  TODO
  // 1-by-1 too slow:
  //   try {
  //     CoinPackedVector cpv(nnz, rmatind, rmatval);
  //     _osi.addRow(cpv, rlb, rub);
  //   } catch (const CoinError& err) {
  //     cerr << "  COIN-OR Error: " << err.message() << endl;
  //     throw runtime_error(err.message());
  //   }
  /// Segfault:
  //   rowStarts.push_back(columns.size());
  //   columns.insert(columns.end(), rmatind, rmatind + nnz);
  //   element.insert(element.end(), rmatval, rmatval + nnz);
  _rows.emplace_back(nnz, rmatind, rmatval);
  _rowlb.push_back(rlb);
  _rowub.push_back(rub);
}

bool MIPosicbcWrapper::addWarmStart(const std::vector<VarId>& vars,
                                    const std::vector<double>& vals) {
  assert(vars.size() == vals.size());
  static_assert(sizeof(VarId) == sizeof(int), "VarId should be (u)int currently");
  for (int i = 0; i < vars.size(); ++i) {
    _warmstart[vars[i]] = vals[i];
  }
  return true;
}

/// SolutionCallback ------------------------------------------------------------------------
/// OSICBC ensures thread-safety?? TODO
/// Event handling copied from examples/interrupt.cpp, Cbc 2.9.8 rev 2272

/************************************************************************

This main program shows how to take advantage of the standalone cbc in your program,
while still making major modifications.
This is very like driver4 but allows interrupts in clp for faster stopping
It would be up to user to clean up output as stopping in Clp seems to
give correct results but can think it is stopping in an odd way.
To make cleaner would need more events defined (in Cbc AND Clp)
First it reads in an integer model from an mps file
Then it initializes the integer model with cbc defaults
Then it calls CbcMain1 passing all parameters apart from first but with callBack to modify stuff
Finally it prints solution

************************************************************************/
/* Meaning of whereFrom:
   1 after initial solve by dualsimplex etc
   2 after preprocessing
   3 just before branchAndBound (so user can override)
   4 just after branchAndBound (before postprocessing)
   5 after postprocessing
*/
/* Meaning of model status is as normal
   status
      -1 before branchAndBound
      0 finished - check isProvenOptimal or isProvenInfeasible to see if solution found
      (or check value of best solution)
      1 stopped - on maxnodes, maxsols, maxtime
      2 difficulties so run was abandoned
      (5 event user programmed event occurred)

      cbc secondary status of problem
        -1 unset (status_ will also be -1)
  0 search completed with solution
  1 linear relaxation not feasible (or worse than cutoff)
  2 stopped on gap
  3 stopped on nodes
  4 stopped on time
  5 stopped on user event
  6 stopped on solutions
  7 linear relaxation unbounded

   but initially check if status is 0 and secondary status is 1 -> infeasible
   or you can check solver status.
*/
/* Return non-zero to return quickly */
// static int callBack(CbcModel * model, int whereFrom)
//{
//  int returnCode=0;
//  switch (whereFrom) {
//  case 1:
//  case 2:
//    if (!model->status()&&model->secondaryStatus())
//      returnCode=1;
//    break;
//  case 3:
//    {
//      //CbcCompareUser compare;
//      //model->setNodeComparison(compare);
//    }
//    break;
//  case 4:
//    // If not good enough could skip postprocessing
//    break;
//  case 5:
//    break;
//  default:
//    abort();
//  }
//  return returnCode;
//}
static int cancelAsap = 0;
/*
  0 - not yet in Cbc
  1 - in Cbc with new signal handler
  2 - ending Cbc
*/
static int statusOfCbc = 0;
static CoinSighandler_t saveSignal = static_cast<CoinSighandler_t>(nullptr);

extern "C" {
static void
#if defined(_MSC_VER)
    __cdecl
#endif  // _MSC_VER
    signal_handler(int /*whichSignal*/) {
  cancelAsap = 3;
}
}
/** This is so user can trap events and do useful stuff.

    CbcModel model_ is available as well as anything else you care
    to pass in
*/

struct EventUserInfo {
  MIPWrapper::CBUserInfo* pCbui = nullptr;
  CglPreProcess* pPP = nullptr;
};

extern CglPreProcess* cbcPreProcessPointer;
class MyEventHandler3 : public CbcEventHandler {
public:
  /**@name Overrides */
  //@{
  CbcAction event(CbcEvent whichEvent) override;
  //@}

  /**@name Constructors, destructor etc*/
  //@{
  /** Default constructor. */
  MyEventHandler3(EventUserInfo& u_);
  /// Constructor with pointer to model (redundant as setEventHandler does)
  MyEventHandler3(CbcModel* model, EventUserInfo& u_);
  /** Destructor */
  ~MyEventHandler3() override;
  /** The copy constructor. */
  MyEventHandler3(const MyEventHandler3& rhs);
  /// Assignment
  MyEventHandler3& operator=(const MyEventHandler3& rhs);
  /// Clone
  CbcEventHandler* clone() const override;
  //@}

protected:
  // data goes here
  EventUserInfo _ui;
  double _bestSolutionValue = DBL_MAX;  // always min
};
//-------------------------------------------------------------------
// Default Constructor
//-------------------------------------------------------------------
MyEventHandler3::MyEventHandler3(EventUserInfo& u_) : _ui(u_) { assert(0); }

//-------------------------------------------------------------------
// Copy constructor
//-------------------------------------------------------------------
MyEventHandler3::MyEventHandler3(const MyEventHandler3& rhs) : CbcEventHandler(rhs), _ui(rhs._ui) {}

// Constructor with pointer to model
MyEventHandler3::MyEventHandler3(CbcModel* model, EventUserInfo& u_)
    : CbcEventHandler(model), _ui(u_) {}

//-------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------
MyEventHandler3::~MyEventHandler3() {}

//----------------------------------------------------------------
// Assignment operator
//-------------------------------------------------------------------
MyEventHandler3& MyEventHandler3::operator=(const MyEventHandler3& rhs) {
  if (this != &rhs) {
    CbcEventHandler::operator=(rhs);
  }
  _ui = rhs._ui;
  return *this;
}
//-------------------------------------------------------------------
// Clone
//-------------------------------------------------------------------
CbcEventHandler* MyEventHandler3::clone() const { return new MyEventHandler3(*this); }

CbcEventHandler::CbcAction MyEventHandler3::event(CbcEvent whichEvent) {
  if (statusOfCbc == 0) {
    // override signal handler
    // register signal handler
    saveSignal = signal(SIGINT, signal_handler);
    statusOfCbc = 1;
  }
  if ((cancelAsap & 2) != 0) {
    //     printf("Cbc got cancel\n");
    // switch off Clp cancel
    cancelAsap &= 2;
    return stop;
  }
  // If in sub tree carry on
  if (model_->parentModel() == nullptr) {
    if (whichEvent == endSearch && statusOfCbc == 1) {
      // switch off cancel
      cancelAsap = 0;
      // restore signal handler
      signal(SIGINT, saveSignal);
      statusOfCbc = 2;
    }
    if (whichEvent == solution || whichEvent == heuristicSolution) {
      // John Forrest  27.2.16:
      // check not duplicate
      if (model_->getObjValue() < _bestSolutionValue) {
        _bestSolutionValue = model_->getObjValue();
        // If preprocessing was done solution will be to processed model
        //       int numberColumns = model_->getNumCols();
        const double* bestSolution = model_->bestSolution();
        assert(bestSolution);
        //       printf("value of solution is %g\n",model_->getObjValue());

        // Trying to obtain solution for the original model:
        assert(model_ && model_->solver());
        // double objOffset=0;
        // model_->solver()->getDblParam(OsiObjOffset, objOffset);
        double objVal =
            (model_->getObjValue());  //- objOffset);   John Forrest suggested to remove, 17.11.17
        double bestBnd = (model_->getBestPossibleObjValue());  //- objOffset);
        if (nullptr != cbcPreProcessPointer) {
          if (OsiSolverInterface* cbcPreOrig = cbcPreProcessPointer->originalModel()) {
            objVal *= cbcPreOrig->getObjSense();
            bestBnd *= cbcPreOrig->getObjSense();
          }
        } else {
          objVal *= model_->getObjSense();
          bestBnd *= model_->getObjSense();
        }
        OsiSolverInterface* origModel = nullptr;
        if (nullptr != cbcPreProcessPointer && nullptr != model_->continuousSolver()) {
          OsiSolverInterface* solver = (model_->continuousSolver()->clone());
          //       ? model_->continuousSolver()->clone()
          //       : model_->continuousSolver()->clone();
          int numberColumns = solver->getNumCols();
          for (int i = 0; i < numberColumns; i++) {
            if (solver->isInteger(i)) {
              solver->setColLower(i, bestSolution[i]);
              solver->setColUpper(i, bestSolution[i]);
            }
          }
          solver->resolve();
          cbcPreProcessPointer->postProcess(*solver, 0);
          delete solver;
          origModel = cbcPreProcessPointer->originalModel();
          _ui.pCbui->pOutput->x = origModel->getColSolution();
        } else {
          origModel = model_->solver();
          _ui.pCbui->pOutput->x = bestSolution;
        }
        if (_ui.pCbui->fVerb) {
          cerr << " % OBJ VAL RAW: " << model_->getObjValue() << "  OBJ VAL ORIG(?): " << objVal
               << " % BND RAW: " << model_->getBestPossibleObjValue() << "  BND ORIG(?): "
               << bestBnd
               //         << "  &prepro: " << cbcPreProcessPointer
               //         << "  &model_._solver(): " << model_->solver()
               << "  orig NCols: " << _ui.pCbui->pOutput->nCols
               << "  prepro NCols:  " << model_->getNumCols();
        }
        assert(origModel->getNumCols() == _ui.pCbui->pOutput->nCols);
        if (_ui.pCbui->fVerb) {
          if (_ui.pCbui->pOutput->nObjVarIndex >= 0) {
            cerr << "  objVAR: " << _ui.pCbui->pOutput->x[_ui.pCbui->pOutput->nObjVarIndex];
          }
          cerr << endl;
        }
        _ui.pCbui->pOutput->objVal = objVal;
        //         origModel->getObjValue();
        _ui.pCbui->pOutput->status = MIPWrapper::SAT;
        _ui.pCbui->pOutput->statusName = "feasible from a callback";
        _ui.pCbui->pOutput->bestBound = bestBnd;
        _ui.pCbui->pOutput->dWallTime =
            std::chrono::duration<double>(std::chrono::steady_clock::now() -
                                          _ui.pCbui->pOutput->dWallTime0)
                .count();
        _ui.pCbui->pOutput->dCPUTime = model_->getCurrentSeconds();
        _ui.pCbui->pOutput->nNodes = model_->getNodeCount();
        _ui.pCbui->pOutput->nOpenNodes = -1;  // model_->getNodeCount2();

        /// Call the user function:
        if (_ui.pCbui->solcbfn != nullptr) {
          (*(_ui.pCbui->solcbfn))(*(_ui.pCbui->pOutput), _ui.pCbui->psi);
          _ui.pCbui->printed = true;
        }
        return noAction;  // carry on
      }
      return noAction;  // carry on
    }
    return noAction;
  }
  return noAction;  // carry on
}

/** This is so user can trap events and do useful stuff.

    ClpSimplex model_ is available as well as anything else you care
    to pass in
*/
class MyEventHandler4 : public ClpEventHandler {
public:
  /**@name Overrides */
  //@{
  int event(Event whichEvent) override;
  //@}

  /**@name Constructors, destructor etc*/
  //@{
  /** Default constructor. */
  MyEventHandler4();
  /// Constructor with pointer to model (redundant as setEventHandler does)
  MyEventHandler4(ClpSimplex* model);
  /** Destructor */
  ~MyEventHandler4() override;
  /** The copy constructor. */
  MyEventHandler4(const MyEventHandler4& rhs);
  /// Assignment
  MyEventHandler4& operator=(const MyEventHandler4& rhs);
  /// Clone
  ClpEventHandler* clone() const override;
  //@}

protected:
  // data goes here
};
//-------------------------------------------------------------------
// Default Constructor
//-------------------------------------------------------------------
MyEventHandler4::MyEventHandler4() {}

//-------------------------------------------------------------------
// Copy constructor
//-------------------------------------------------------------------
MyEventHandler4::MyEventHandler4(const MyEventHandler4& rhs) : ClpEventHandler(rhs) {}

// Constructor with pointer to model
MyEventHandler4::MyEventHandler4(ClpSimplex* model) : ClpEventHandler(model) {}

//-------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------
MyEventHandler4::~MyEventHandler4() {}

//----------------------------------------------------------------
// Assignment operator
//-------------------------------------------------------------------
MyEventHandler4& MyEventHandler4::operator=(const MyEventHandler4& rhs) {
  if (this != &rhs) {
    ClpEventHandler::operator=(rhs);
  }
  return *this;
}
//-------------------------------------------------------------------
// Clone
//-------------------------------------------------------------------
ClpEventHandler* MyEventHandler4::clone() const { return new MyEventHandler4(*this); }

int MyEventHandler4::event(Event whichEvent) {
  if ((cancelAsap & 1) != 0) {
    //     printf("Clp got cancel\n");
    return 5;
  }
  return -1;
}
// end SolutionCallback ---------------------------------------------------------------------

MIPosicbcWrapper::Status MIPosicbcWrapper::convertStatus(CbcModel* pModel) {
  Status s = Status::UNKNOWN;
  /* Converting the status. */
  if (pModel->isProvenOptimal()) {
    s = Status::OPT;
    output.statusName = "Optimal";
    //        wrapAssert(_osi., "Optimality reported but pool empty?", false);
  } else if (pModel->isProvenInfeasible()) {
    s = Status::UNSAT;
    output.statusName = "Infeasible";
  } else if (pModel->isProvenDualInfeasible()) {
    s = Status::UNBND;
    output.statusName = "Dual infeasible";
    //        s = Status::UNSATorUNBND;
  } else if  // wrong: (pModel->getColSolution())
      (fabs(pModel->getObjValue()) < 1e50) {
    s = Status::SAT;
    output.statusName = "Feasible";
  } else if (pModel->isAbandoned()) {  // AFTER feas-ty
    s = Status::ERROR_STATUS;
    output.statusName = "Abandoned";
  } else {
    s = Status::UNKNOWN;
    output.statusName = "Unknown";
  }
  return s;
}

MIPosicbcWrapper::Status MIPosicbcWrapper::convertStatus() {
  Status s = Status::UNKNOWN;
  /* Converting the status. */
  if (_osi.isProvenOptimal()) {
    s = Status::OPT;
    output.statusName = "Optimal";
    //        wrapAssert(_osi., "Optimality reported but pool empty?", false);
  } else if (_osi.isProvenPrimalInfeasible()) {
    s = Status::UNSAT;
    output.statusName = "Infeasible";
  } else if (_osi.isProvenDualInfeasible()) {
    s = Status::UNBND;
    output.statusName = "Dual infeasible";
    //        s = Status::UNSATorUNBND;
  } else if (_osi.isAbandoned()) {
    s = Status::ERROR_STATUS;
    output.statusName = "Abandoned";
  } else if  // wrong: (pModel->getColSolution())
      (fabs(_osi.getObjValue()) < _osi.getInfinity()) {
    s = Status::SAT;
    output.statusName = "Feasible";
    cout << " getSolverObjValue(as minim) == " << _osi.getObjValue() << endl;
  } else {
    s = Status::UNKNOWN;
    output.statusName = "Unknown";
  }
  return s;
}

void MIPosicbcWrapper::solve() {  // Move into ancestor?
  try {
    /// Not using CoinPackedMatrix any more, so need to add all constraints at once:
    /// But this gives segf:
    //     _osi.addRows(rowStarts.size(), rowStarts.data(),
    //                 columns.data(), element.data(), rowlb.data(), rowub.data());
    /// So:
    MIPWrapper::addPhase1Vars();  // only now
    if (fVerbose) {
      cerr << "  MIPosicbcWrapper: adding constraints physically..." << flush;
    }
    vector<CoinPackedVectorBase*> pRows(_rowlb.size());
    for (int i = 0; i < _rowlb.size(); ++i) {
      pRows[i] = &_rows[i];
    }
    _osi.addRows(_rowlb.size(), pRows.data(), _rowlb.data(), _rowub.data());
    //     rowStarts.clear();
    //     columns.clear();
    //     element.clear();
    pRows.clear();
    _rows.clear();
    _rowlb.clear();
    _rowub.clear();
    if (fVerbose) {
      cerr << " done." << endl;
    }
    /////////////// Last-minute solver options //////////////////
    //       osi->loadProblem(*matrix,
    {
      std::vector<VarId> integer_vars;
      for (unsigned int i = 0; i < colObj.size(); i++) {
        if (REAL != colTypes[i]
            //           && is_used[i]
        ) {
          integer_vars.push_back(i);
        }
      }
      _osi.setInteger(integer_vars.data(), integer_vars.size());
    }
    if (!_options->sExportModel.empty()) {
      // Not implemented for OsiClp:
      //       _osi.setColNames(colNames, 0, colObj.size(), 0);
      vector<const char*> colN(colObj.size());
      for (int j = 0; j < colNames.size(); ++j) {
        colN[j] = colNames[j].c_str();
      }
      _osi.writeMpsNative(_options->sExportModel.c_str(), nullptr, colN.data());
    }

    // Tell solver to return fast if presolve or initial solve infeasible
    _osi.getModelPtr()->setMoreSpecialOptions(3);
    // allow Clp to handle interrupts
    MyEventHandler4 clpEventHandler;
    _osi.getModelPtr()->passInEventHandler(&clpEventHandler);

    /* switch on/off output to the screen */
    class NullCoinMessageHandler : public CoinMessageHandler {
      int print() override { return 0; }
      void checkSeverity() override {}
    } nullHandler;

    //     CbcSolver control(osi);
    //     // initialize   ???????
    //     control.fillValuesInSolver();
    //     CbcModel * pModel = control.model();
    if (fVerbose) {
      cerr << " Model creation..." << endl;
    }

    // #define MZN_USE_CbcSolver  -- not linked rev2274
    /// FOR WARMSTART
    for (const auto& vv : _warmstart) {
      _osi.setColName(vv.first, colNames[vv.first]);
    }
#ifdef MZN_USE_CbcSolver
    CbcSolver control(osi);
    // initialize
    control.fillValuesInSolver();
    CbcModel& model = *control.model();
#else
    CbcModel model(_osi);
#endif
    //     CbcSolver control(osi);
    //     control.solve();
    if (_options->absGap >= 0.0) {
      model.setAllowableGap(_options->absGap);
    }
    if (_options->relGap >= 0.0) {
      model.setAllowableFractionGap(_options->relGap);
    }
    if (_options->intTol >= 0.0) {
      model.setIntegerTolerance(_options->intTol);
    }
    //     model.setCutoffIncrement( objDiff );

    /// WARMSTART
    {
      std::vector<std::pair<std::string, double> > mipstart;
      for (const auto& vv : _warmstart) {
        mipstart.emplace_back(colNames[vv.first], vv.second);
      }
      _warmstart.clear();
      model.setMIPStart(mipstart);
    }

    CoinMessageHandler msgStderr(stderr);

    class StderrCoinMessageHandler : public CoinMessageHandler {
      int print() override {
        cerr << messageBuffer_ << endl;
        return 0;
      }
      void checkSeverity() override {}
    } stderrHandler;

    if (fVerbose) {
      //        _osi.messageHandler()->setLogLevel(1);
      //        _osi.getModelPtr()->setLogLevel(1);
      //        _osi.getRealSolverPtr()->messageHandler()->setLogLevel(0);
      // DOES NOT WORK:                                                     TODO
      //        model.passInMessageHandler( &stderrHandler );
      msgStderr.setLogLevel(0, 1);
      model.passInMessageHandler(&msgStderr);
      //        model.setLogLevel(1);
      //        model.solver()->messageHandler()->setLogLevel(0);
    } else {
      model.passInMessageHandler(&nullHandler);
      model.messageHandler()->setLogLevel(0);
      model.setLogLevel(0);
      model.solver()->setHintParam(OsiDoReducePrint, true, OsiHintTry);
      //       _osi.passInMessageHandler(&nullHandler);
      //       _osi.messageHandler()->setLogLevel(0);
      //       _osi.setHintParam(OsiDoReducePrint, true, OsiHintTry);
    }

    if (_options->nTimeout != 0) {
      //       _osi.setMaximumSeconds(nTimeout);
      model.setMaximumSeconds(static_cast<double>(_options->nTimeout) / 1000.0);
    }

    /// TODO
    //     if(all_solutions && obj.getImpl()) {
    //       IloNum lastObjVal = (obj.getSense() == IloObjective::Minimize ) ?
    //       _iloosicbc->use(SolutionCallback(_iloenv, lastObjVal, *this));
    // Turn off OSICBC logging

    /// Solution callback
    output.nCols = colObj.size();
    //    x.resize(output.nCols);
    //    output.x = &x[0];

    if (_options->flagIntermediate && (cbui.solcbfn != nullptr)) {
      // Event handler. Should be after CbcMain0()?
      EventUserInfo ui;
      ui.pCbui = &cbui;
      //      ui.pPP = 0;
      MyEventHandler3 eventHandler(&model, ui);
      model.passInEventHandler(&eventHandler);
    }

    /// Cuts needed
    if (cbui.cutcbfn != nullptr) {
      /// This class is passed to CBC to organize cut callbacks
      /// We need original solutions here (combinatorial cuts)
      class CutCallback : public CglCutGenerator {
        MIPWrapper::CBUserInfo& _cbui;

      public:
        CutCallback(MIPWrapper::CBUserInfo& ui) : _cbui(ui) {}
        CglCutGenerator* clone() const override { return new CutCallback(_cbui); }
        /// Make sure this overrides but we might need to compile this with old CBC as well
        static bool needsOriginalModel() /*override*/ { return true; }
        void generateCuts(const OsiSolverInterface& si, OsiCuts& cs,
                          const CglTreeInfo info = CglTreeInfo()) override {
          _cbui.pOutput->nCols = si.getNumCols();
          MZN_ASSERT_HARD_MSG(
              _cbui.pOutput->nCols == ((MIPWrapper*)(_cbui.wrapper))->colNames.size(),
              "CBC cut callback: current model is different? Ncols="
                  << _cbui.pOutput->nCols << ", originally "
                  << ((MIPWrapper*)(_cbui.wrapper))->colNames.size()
                  << ". If you have an old version of CBC, to use combinatorial cuts"
                     " run with --cbcArgs '-preprocess off'");
          _cbui.pOutput->x = si.getColSolution();  // change the pointer?
          MIPWrapper::CutInput cuts;
          _cbui.cutcbfn(*_cbui.pOutput, cuts, _cbui.psi,
                        (info.options & 128) != 0);  // options&128: integer candidate
          for (const auto& cut : cuts) {             // Convert cut sense
            OsiRowCut rc;
            switch (cut.sense) {
              case LQ:
                rc.setUb(cut.rhs);
                break;
              case GQ:
                rc.setLb(cut.rhs);
                break;
              default:
                assert(EQ == cut.sense);
                rc.setLb(cut.rhs);
                rc.setUb(cut.rhs);
            }
            rc.setRow(cut.rmatind.size(), cut.rmatind.data(), cut.rmatval.data());
            cs.insertIfNotDuplicate(rc);
          }
        }
      };
      CutCallback ccb(cbui);
      model.addCutGenerator(&ccb, 10, "MZN_cuts", true, true);  // also at solution
    }

    // Process extra flags options
    for (const auto& it : _options->extraParams) {
      _options->cbcCmdOptions.push_back(it.first.substr(5));
      _options->cbcCmdOptions.push_back(it.second);
    }

    if (1 < _options->nThreads) {
      _options->cbcCmdOptions.emplace_back("-threads");
      ostringstream oss;
      oss << _options->nThreads;
      _options->cbcCmdOptions.push_back(oss.str());
    }
    _options->cbcCmdOptions.emplace_back("-solve");
    _options->cbcCmdOptions.emplace_back("-quit");

    auto cbc_argc = _options->cbcCmdOptions.size() + 1;
    std::vector<const char*> cbc_argv;
    cbc_argv.reserve(cbc_argc);
    cbc_argv.push_back("cbc");
    for (const auto& arg : _options->cbcCmdOptions) {
      cbc_argv.push_back(arg.c_str());
    }

    cbui.pOutput->dWallTime0 = output.dWallTime0 = std::chrono::steady_clock::now();
    output.dCPUTime = clock();

    /* OLD: Optimize the problem and obtain solution. */
    //       model.branchAndBound();
    //       _osi.branchAndBound();

    /// TAKEN FORM DRIVER3.CPP, seems to use most features:
//      CbcMain0(model);
//      CbcCbcParamUtils::setCbcModelDefaults(model) ;
//       const char * argv2[]={"mzn-cbc","-solve","-quit"};
//        CbcMain1(3,argv2,model);
#ifdef MZN_USE_CbcSolver
    if (fVerbose)
      cerr << "  Calling control.solve() with options '" << options->cbcCmdOptions << "'..."
           << endl;
    control.solve(options->cbcCmdOptions.c_str(), 1);
#else
#define MZN_USE_callCbc1
#ifdef MZN_USE_callCbc1
    if (fVerbose) {
      cerr << "  Calling CbcMain with command 'cbc";
      for (const auto& arg : _options->cbcCmdOptions) {
        cerr << " " << arg;
      }
      cerr << "'..." << endl;
    }
    CbcMain(cbc_argc, &cbc_argv[0], model);
    // callCbc(_options->cbcCmdOptions, model);
//     callCbc1(cbcCmdOptions, model, callBack);
// What is callBack() for?    TODO
#else
    CbcMain0(model);
    // should be here?
    //   // Event handler
    //    EventUserInfo ui;
    //    MyEventHandler3 eventHandler( &model, ui );
    //    model.passInEventHandler(&eventHandler);
    /* Now go into code for standalone solver
       Could copy arguments and add -quit at end to be safe
       but this will do
    */
    vector<string> argvS;
    MiniZinc::split(cbcCmdOptions, argvS);
    vector<const char*> argv;
    MiniZinc::vec_string2vec_pchar(argvS, argv);
    if (fVerbose) cerr << "  Calling CbcMain1 with options '" << cbcCmdOptions << "'..." << endl;
    CbcMain1(argv.size(), argv.data(), model, callBack);
#endif
#endif

    output.dWallTime =
        std::chrono::duration<double>(std::chrono::steady_clock::now() - output.dWallTime0).count();
    output.dCPUTime = (clock() - output.dCPUTime) / CLOCKS_PER_SEC;

    output.status = convertStatus(&model);
    //    output.status = convertStatus();

    /// Continuing to fill the output object:
    if (Status::OPT == output.status || Status::SAT == output.status) {
      output.objVal = model.getObjValue();
      //      output.objVal = _osi.getObjValue();

      /* The size of the problem should be obtained by asking OSICBC what
          the actual size is, rather than using what was passed to CBCcopylp.
          cur_numrows and cur_numcols store the current number of rows and
          columns, respectively.  */   // ?????????????? TODO

      int cur_numcols = model.getNumCols();
      //       int cur_numcols = _osi.getNumCols ();
      assert(cur_numcols == colObj.size());

      wrapAssert(model.getColSolution() != nullptr, "Failed to get variable values.");
      _x.assign(model.getColSolution(), model.getColSolution() + cur_numcols);  // ColSolution();
      output.x = _x.data();
      //       output.x = _osi.getColSolution();
      if ((cbui.solcbfn != nullptr) && (!_options->flagIntermediate || !cbui.printed)) {
        cbui.solcbfn(output, cbui.psi);
      }
    }
    output.bestBound = model.getBestPossibleObjValue();
    //    output.bestBound = -1;
    output.nNodes = model.getNodeCount();
    //    output.nNodes = _osi.getNodeCount();
    output.nOpenNodes = -1;

  } catch (CoinError& err) {
    err.print(true);
  }
}

void MIPosicbcWrapper::setObjSense(int s) { _osi.setObjSense(-s); }

/*

try the following for example:

CbcMain0(model);
const char * argv2[]={"driver4","-cuts","off" ,"-preprocess","off","-passc","1","-solve","-quit"};
CbcMain1(9,argv2,model);

you can add any feature you want to argv2 ...

if you want to add cuts yourself, or heuristics, do the following:

  OsiSolverInterface *solver2 = osi;
  CglPreProcess *process = new CglPreProcess;
  solver2 = process->preProcess(*solver,false,2);

    CbcModel model1(*solver2);

    model1.initialSolve();

  //==============================================

  CglProbing generator1;
  generator1.setUsingObjective(true);
  generator1.setMaxPass(1);
  generator1.setMaxPassRoot(5);
  generator1.setMaxProbe(10);
  generator1.setMaxProbeRoot(1000);
  generator1.setMaxLook(50);
  generator1.setMaxLookRoot(500);
  generator1.setMaxElements(200);
  generator1.setRowCuts(3);

  CglGomory generator2;
  generator2.setLimit(300);

  CglKnapsackCover generator3;

  CglRedSplit generator4;
  generator4.setLimit(200);

  CglClique generator5;
  generator5.setStarCliqueReport(false);
  generator5.setRowCliqueReport(false);

  CglMixedIntegerRounding2 mixedGen;
  CglFlowCover flowGen;

  CglGMI cut1;
  CglMixedIntegerRounding2 cut2;
  CglOddHole cut3;
  CglSimpleRounding cut4;
  CglResidualCapacity cut5;
  CglTwomir cut6;
  CglZeroHalf cut7;

  model1.addCutGenerator(&generator1,-1,"Probing");
  model1.addCutGenerator(&generator2,-1,"Gomory");
  model1.addCutGenerator(&generator3,-1,"Knapsack");
  model1.addCutGenerator(&generator4,-1,"RedSplit");
  model1.addCutGenerator(&generator5,-1,"Clique");
  model1.addCutGenerator(&flowGen,-1,"FlowCover");
  model1.addCutGenerator(&mixedGen,-1,"MixedIntegerRounding");
  model1.addCutGenerator(&cut1,-1,"GMI");
  model1.addCutGenerator(&cut2,-1,"MixedIntegerRounding2");
  model1.addCutGenerator(&cut3,-1,"OddHole");
  model1.addCutGenerator(&cut4,-1,"SimpleRounding");
  model1.addCutGenerator(&cut5,-1,"ResidualCapacity");
  model1.addCutGenerator(&cut6,-1,"Twomir");
  model1.addCutGenerator(&cut7,-1,"ZeroHalf");



  CbcRounding heuristic1(model1);
  CbcHeuristicLocal heuristic2(model1);


 model1.addHeuristic(&heuristic1);
 model1.addHeuristic(&heuristic2);




    model1.setMaximumCutPassesAtRoot(50);
    model1.setMaximumCutPasses(1000);



  model1.branchAndBound();


  OsiSolverInterface * solver3;

  process->postProcess(*model1.solver());

  solver3 = solver;

 or, use the default strategy:

CbcStrategyDefault strategy(5);
model1.setStrategy(strategy);






On Sun, Oct 11, 2015 at 8:38 PM, Gleb Belov <gleb.belov@monash.edu> wrote:

    Hi,

    I am trying to call Cbc 2.9.6 from my program. When using the tutorial-style approach

    OsiClpSolverInterface osi;
    osi.add .......
    CbcModel model(osi);
    model.branchAndBound();

    there seem to be no cuts and other stuff applied. When using the method from the examples,

    CbcMain0(model);
    const char * argv2[]={"driver4","-solve","-quit"};
    CbcMain1(3,argv2,model);

    there are cuts applied, but obviously different (less aggressive) to the standalone Cbc
executable. I also tried CbcSolver class but its method solve() is not found by the linker. So what
is the 'standard' way of using the 'default' add-ons?

    Moreover. The attached example crashes both in the standalone Cbc and in the CbcCmain0/1 variant
after a few minutes.

    Thanks

    _______________________________________________
    Cbc mailing list
    Cbc@list.coin-or.org
    http://list.coin-or.org/mailman/listinfo/cbc




Hi, what is currently good way to have a solution callback in Cbc? the
interrupt example shows 2 ways, don't know which is right.

Moreover, it says that the solution would be given for the preprocessed
model. Is it possible to produce one for the original? Is it possible to
call other functions from inside, such as number of nodes, dual bound?

Thanks

From john.forrest at fastercoin.com  Thu Oct  8 10:34:15 2015
From: john.forrest at fastercoin.com (John Forrest)
Date: Thu, 8 Oct 2015 15:34:15 +0100
Subject: [Cbc] Solution callbacks
In-Reply-To: <5615F778.9020601@monash.edu>
References: <5615F778.9020601@monash.edu>
Message-ID: <56167EE7.6000607@fastercoin.com>

Gleb,

On 08/10/15 05:56, Gleb Belov wrote:
> Hi, what is currently good way to have a solution callback in Cbc? the
> interrupt example shows 2 ways, don't know which is right.
>

It is the event handling code you would be using.
> Moreover, it says that the solution would be given for the
> preprocessed model. Is it possible to produce one for the original?

At present no.  In principle not difficult.  First the callback function
would have to be modified to get passed the CglPreProcess object -
easy.  Then in event handler you could make a copy of object and
postsolve (you need a copy as postsolve deletes data).
> Is it possible to call other functions from inside, such as number of
> nodes, dual bound?

Yes - you have CbcModel * model_ so things like that are available (or
could easily be made available)

>
> Thanks
>

John Forrest


 */
