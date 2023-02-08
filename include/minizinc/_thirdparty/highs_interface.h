#include <cstdint>

typedef int HighsInt;
typedef unsigned int HighsUInt;

const HighsInt kHighsMaximumStringLength = 512;

const HighsInt kHighsStatusError = -1;
const HighsInt kHighsStatusOk = 0;
const HighsInt kHighsStatusWarning = 1;

const HighsInt kHighsVarTypeContinuous = 0;
const HighsInt kHighsVarTypeInteger = 1;
const HighsInt kHighsVarTypeSemiContinuous = 2;
const HighsInt kHighsVarTypeSemiInteger = 3;
const HighsInt kHighsVarTypeImplicitInteger = 4;

const HighsInt kHighsOptionTypeBool = 0;
const HighsInt kHighsOptionTypeInt = 1;
const HighsInt kHighsOptionTypeDouble = 2;
const HighsInt kHighsOptionTypeString = 3;

const HighsInt kHighsInfoTypeInt64 = -1;
const HighsInt kHighsInfoTypeInt = 1;
const HighsInt kHighsInfoTypeDouble = 2;

const HighsInt kHighsObjSenseMinimize = 1;
const HighsInt kHighsObjSenseMaximize = -1;

const HighsInt kHighsMatrixFormatColwise = 1;
const HighsInt kHighsMatrixFormatRowwise = 2;

const HighsInt kHighsHessianFormatTriangular = 1;
const HighsInt kHighsHessianFormatSquare = 2;

const HighsInt kHighsSolutionStatusNone = 0;
const HighsInt kHighsSolutionStatusInfeasible = 1;
const HighsInt kHighsSolutionStatusFeasible = 2;

const HighsInt kHighsBasisValidityInvalid = 0;
const HighsInt kHighsBasisValidityValid = 1;

const HighsInt kHighsPresolveStatusNotPresolved = -1;
const HighsInt kHighsPresolveStatusNotReduced = 0;
const HighsInt kHighsPresolveStatusInfeasible = 1;
const HighsInt kHighsPresolveStatusUnboundedOrInfeasible = 2;
const HighsInt kHighsPresolveStatusReduced = 3;
const HighsInt kHighsPresolveStatusReducedToEmpty = 4;
const HighsInt kHighsPresolveStatusTimeout = 5;
const HighsInt kHighsPresolveStatusNullError = 6;
const HighsInt kHighsPresolveStatusOptionsError = 7;

const HighsInt kHighsModelStatusNotset = 0;
const HighsInt kHighsModelStatusLoadError = 1;
const HighsInt kHighsModelStatusModelError = 2;
const HighsInt kHighsModelStatusPresolveError = 3;
const HighsInt kHighsModelStatusSolveError = 4;
const HighsInt kHighsModelStatusPostsolveError = 5;
const HighsInt kHighsModelStatusModelEmpty = 6;
const HighsInt kHighsModelStatusOptimal = 7;
const HighsInt kHighsModelStatusInfeasible = 8;
const HighsInt kHighsModelStatusUnboundedOrInfeasible = 9;
const HighsInt kHighsModelStatusUnbounded = 10;
const HighsInt kHighsModelStatusObjectiveBound = 11;
const HighsInt kHighsModelStatusObjectiveTarget = 12;
const HighsInt kHighsModelStatusTimeLimit = 13;
const HighsInt kHighsModelStatusIterationLimit = 14;
const HighsInt kHighsModelStatusUnknown = 15;
const HighsInt kHighsModelStatusSolutionLimit = 16;
const HighsInt kHighsModelStatusInterrupt = 17;

const HighsInt kHighsBasisStatusLower = 0;
const HighsInt kHighsBasisStatusBasic = 1;
const HighsInt kHighsBasisStatusUpper = 2;
const HighsInt kHighsBasisStatusZero = 3;
const HighsInt kHighsBasisStatusNonbasic = 4;

const HighsInt kHighsCallbackLogging = 0;
const HighsInt kHighsCallbackSimplexInterrupt = 1;
const HighsInt kHighsCallbackIpmInterrupt = 2;
const HighsInt kHighsCallbackMipImprovingSolution = 3;
const HighsInt kHighsCallbackMipLogging = 4;
const HighsInt kHighsCallbackMipInterrupt = 5;

struct HighsCallbackDataOut {
  int log_type;
  double running_time;
  HighsInt simplex_iteration_count;
  HighsInt ipm_iteration_count;
  double objective_function_value;
  int64_t mip_node_count;
  double mip_primal_bound;
  double mip_dual_bound;
  double mip_gap;
  double* mip_solution;
};

struct HighsCallbackDataIn {
  int user_interrupt;
};
