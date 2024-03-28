/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip.dekker@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/plugin.hh>
#include <minizinc/solver_config.hh>
#include <minizinc/solver_instance_base.hh>
#include <minizinc/solvers/MIP/MIP_wrap.hh>

#ifdef HIGHS_PLUGIN
#include <minizinc/_thirdparty/highs_interface.h>
#else
#include <interfaces/highs_c_api.h>
#endif

#include <memory>
#include <sstream>

class HiGHSPlugin {
public:
  HiGHSPlugin(const std::string& dll);

  // NOLINTNEXTLINE(readability-identifier-naming)
  void* (*Highs_create)();
  // NOLINTNEXTLINE(readability-identifier-naming)
  void (*Highs_destroy)(void* highs);
  // NOLINTNEXTLINE(readability-identifier-naming)
  const char* (*Highs_version)();
  // NOLINTNEXTLINE(readability-identifier-naming)
  double (*Highs_getInfinity)(const void* highs);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_getNumOptions)(const void* highs);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_getOptionName)(const void* highs, const HighsInt index, char** name);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_getOptionType)(const void* highs, const char* option, HighsInt* type);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_getBoolOptionValues)(const void* highs, const char* option,
                                        HighsInt* current_value, HighsInt* default_value);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_getIntOptionValues)(const void* highs, const char* option,
                                       HighsInt* current_value, HighsInt* min_value,
                                       HighsInt* max_value, HighsInt* default_value);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_getDoubleOptionValues)(const void* highs, const char* option,
                                          double* current_value, double* min_value,
                                          double* max_value, double* default_value);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_getStringOptionValues)(const void* highs, const char* option,
                                          char* current_value, char* default_value);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_setIntOptionValue)(void* highs, const char* option, const HighsInt value);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_setDoubleOptionValue)(void* highs, const char* option, const double value);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_setStringOptionValue)(void* highs, const char* option, const char* value);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_setBoolOptionValue)(void* highs, const char* option, const HighsInt value);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_getNumRow)(const void* highs);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_addRow)(void* highs, const double lower, const double upper,
                           const HighsInt num_new_nz, const HighsInt* index, const double* value);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_getNumCol)(const void* highs);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_addCols)(void* highs, const HighsInt num_new_col, const double* costs,
                            const double* lower, const double* upper, const HighsInt num_new_nz,
                            const HighsInt* starts, const HighsInt* index, const double* value);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_changeColsIntegralityByRange)(void* highs, const HighsInt from_col,
                                                 const HighsInt to_col,
                                                 const HighsInt* integrality);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_changeObjectiveSense)(void* highs, const HighsInt sense);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_changeColBounds)(void* highs, const HighsInt col, const double lower,
                                    const double upper);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_writeModel)(void* highs, const char* filename);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_run)(void* highs);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_getModelStatus)(const void* highs);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_getSolution)(const void* highs, double* col_value, double* col_dual,
                                // NOLINTNEXTLINE(readability-identifier-naming)
                                double* row_value, double* row_dual);
  // NOLINTNEXTLINE(readability-identifier-naming)
  double (*Highs_getObjectiveValue)(const void* highs);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_getIntInfoValue)(const void* highs, const char* info, int* value);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_getInt64InfoValue)(const void* highs, const char* info, int64_t* value);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_getDoubleInfoValue)(const void* highs, const char* info, double* value);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_setCallback)(void* highs,
                                void (*user_callback)(const int, const char*, const void*, void*,
                                                      void*),
                                void* user_callback_data);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_startCallback)(void* highs, const int callback_type);
  // NOLINTNEXTLINE(readability-identifier-naming)
  HighsInt (*Highs_stopCallback)(void* highs, const int callback_type);

private:
#ifdef HIGHS_PLUGIN
  std::unique_ptr<MiniZinc::Plugin> _inner;
#endif
};

class MIPHiGHSWrapper : public MIPWrapper {
protected:
  void* _highs;
  std::vector<double> _x;

  static void checkHiGHSReturn(HighsInt stat, const std::string& message) {
    if (stat == kHighsStatusError) {
      std::ostringstream ss;
      ss << "Highs ERROR: " << message;
      throw std::runtime_error(ss.str());
    }
  }

public:
  class FactoryOptions {
  public:
    bool processOption(int& i, std::vector<std::string>& argv, const std::string& workingDir);

    std::string highsDll;
  };

  class Options : public MiniZinc::SolverInstanceBase::Options {
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
  FactoryOptions& _factoryOptions;
  Options* _options = nullptr;
  HiGHSPlugin* _plugin = nullptr;

public:
  MIPHiGHSWrapper(FactoryOptions& factoryOpt, Options* opt)
      : _factoryOptions(factoryOpt), _options(opt) {
    _plugin = new HiGHSPlugin(factoryOpt.highsDll);
    _highs = _plugin->Highs_create();
    checkHiGHSReturn(
        _plugin->Highs_setBoolOptionValue(_highs, "log_to_console", static_cast<HighsInt>(false)),
        "Unable to set verbosity");
  }
  ~MIPHiGHSWrapper() override {
    _plugin->Highs_destroy(_highs);
    delete _plugin;
  }

  static std::string getId() { return "highs"; };
  static std::string getName() { return "HiGHS"; };
  static std::string getVersion(FactoryOptions& factoryOpt,
                                MiniZinc::SolverInstanceBase::Options* opt = nullptr);
  static std::string getDescription(FactoryOptions& factoryOpt,
                                    MiniZinc::SolverInstanceBase::Options* opt = nullptr);
  static std::vector<std::string> getStdFlags() {
    return {
        "-i", "-p", "-r", "-s", "-v",
    };
  };
  static std::vector<std::string> getRequiredFlags(FactoryOptions& factoryOpt);
  static std::vector<std::string> getFactoryFlags() { return {"--highs-dll"}; };
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
    auto highsSense = s < 0 ? kHighsObjSenseMinimize : kHighsObjSenseMaximize;
    _plugin->Highs_changeObjectiveSense(_highs, highsSense);
  };
  // Get solver infinity value
  double getInfBound() override { return _plugin->Highs_getInfinity(_highs); };
  // Get number of solver variables (matrix columns)
  int getNCols() override { return _plugin->Highs_getNumCol(_highs); };
  // Get number of linear constraints (matrix rows)
  int getNRows() override { return _plugin->Highs_getNumRow(_highs); }

  // Method to optimize the current MIP program
  void solve() override;

  // Change variable bounds
  void setVarBounds(int iVar, double lb, double ub) override {
    checkHiGHSReturn(_plugin->Highs_changeColBounds(_highs, iVar, lb, ub),
                     "unable to set variable bounds");
  }

protected:
  // Convert HiGHSModelStatus to MIPWrapper internal status
  MIPWrapper::Status convertStatus(HighsInt model_status);
  // Set HiGHS internal options based on the command line flags given to the solver interface
  void setOptions();

  static void callback(int callback_type, const char* message, const void* data_out, void* data_in,
                       void* user_callback_data);
};
