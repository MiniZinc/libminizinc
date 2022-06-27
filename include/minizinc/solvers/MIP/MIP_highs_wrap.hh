/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip.dekker@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/solver_config.hh>
#include <minizinc/solver_instance_base.hh>
#include <minizinc/solvers/MIP/MIP_wrap.hh>

#include <Highs.h>
#include <sstream>

using namespace MiniZinc;

class MIPHiGHSWrapper : public MIPWrapper {
protected:
  Highs _highs;

  static void checkHiGHSReturn(HighsStatus stat, const std::string& message) {
    if (stat == HighsStatus::kError) {
      std::ostringstream ss;
      ss << "Highs ERROR: " << message;
      throw std::runtime_error(ss.str());
    }
  }

public:
  class FactoryOptions {
  public:
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    bool processOption(int& i, std::vector<std::string>& argv, const std::string& workingDir) {
      return false;
    }
  };

  class Options : public SolverInstanceBase::Options {
  public:
    int nThreads = 1;
    std::string sExportModel;
    int nTimeout = 0;

    bool flagIntermediate = false;
    double absGap = -1;
    double relGap = 1e-8;
    double intTol = 1e-8;
    int randSeed = -1;

    std::unordered_map<std::string, std::string> extraParams;

    bool processOption(int& i, std::vector<std::string>& argv,
                       const std::string& workingDir = std::string());
    static void printHelp(std::ostream& os);
  };

private:
  Options* _options = nullptr;

public:
  MIPHiGHSWrapper(FactoryOptions& factoryOpt, Options* opt) : _options(opt) {}
  ~MIPHiGHSWrapper() override {}

  static std::string getId() { return "highs"; };
  static std::string getName() { return "HiGHS"; };
  static std::string getVersion(FactoryOptions& factoryOpt,
                                MiniZinc::SolverInstanceBase::Options* opt = nullptr);
  static std::string getDescription(FactoryOptions& factoryOpt,
                                    MiniZinc::SolverInstanceBase::Options* opt = nullptr);
  static std::vector<std::string> getStdFlags() {
    return {
        "-f", "-i", "-p", "-r", "-s", "-v",
    };
  };
  static std::vector<std::string> getRequiredFlags(FactoryOptions& factoryOpt) { return {}; };
  static std::vector<std::string> getFactoryFlags() { return {}; };
  static std::vector<std::string> getTags() { return {"mip", "float", "api", "highs"}; };

  static std::vector<MiniZinc::SolverConfig::ExtraFlag> getExtraFlags(FactoryOptions& factoryOpt);

  /// Add new variables to the solver
  void doAddVars(size_t n, double* obj, double* lb, double* ub, VarType* vt,
                 std::string* names) override;
  /// Add a linear constraint
  void addRow(int nnz, int* rmatind, double* rmatval, LinConType sense, double rhs,
              int mask = MaskConsType_Normal, const std::string& rowName = "") override;
  /// Set objective type
  void setObjSense(int s) override {
    assert(s == -1 || s == +1);
    ObjSense highsSense = s < 0 ? ObjSense::kMinimize : ObjSense::kMaximize;
    _highs.changeObjectiveSense(highsSense);
  };
  // Get solver infinity value
  double getInfBound() override { return _highs.getInfinity(); };
  // Get number of solver variables (matrix columns)
  int getNCols() override { return _highs.getNumCol(); };
  // Get number of linear constraints (matrix rows)
  int getNRows() override { return _highs.getNumRow(); }

  // Method to optimize the current MIP program
  void solve() override;

  // Change variable bounds
  void setVarBounds(int iVar, double lb, double ub) override {
    checkHiGHSReturn(_highs.changeColBounds(iVar, lb, ub), "unable to set variable bounds");
  }

protected:
  // Convert HiGHSModelStatus to MIPWrapper internal status
  MIPWrapper::Status convertStatus(const HighsModelStatus& model_status) const;
  // Set HiGHS internal options based on the command line flags given to the solver interface
  void setOptions();
  // Synchronise "output" object with the current solver status
  void setOutput();
};