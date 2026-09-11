/// @file
/// FZnSO — the shared types of the solver interface.
///
/// A *solver* implements the `fznso_<name>_...` entry points (see
/// `fznso_solver_template.c`); an *application* loads a solver library and calls
/// them. The types below are used by both sides.
///
/// Four rules hold throughout, and so are not repeated on every declaration:
///
///  - **Handles are opaque.** `FznsoModel`, `FznsoValue`, `FznsoSolution`,
///    `FznsoAnnotation` and `FznsoSolver` mean something only to the side that
///    created one. Never inspect or dereference a handle — it need not address
///    memory at all. Reach its contents through the `methods` table of the
///    matching `...Ref` struct.
///  - **Accessors have preconditions.** Read `kind`, or the relevant length,
///    first and call only what the answer permits. Nothing checks for you.
///  - **Nothing transfers ownership.** Every pointer, string and handle is
///    borrowed for a stated window. Copy anything that must outlive it.
///  - **The version must match exactly.** A loader rejects a solver whose
///    `fznso_<name>_abi_version` differs from its own.
///
/// Each rule, and the exact window every kind of borrow is valid for, is
/// specified at
/// <https://fznso.minizinc.dev/spec/lifetimes-and-ownership/>.

#ifndef fznso_types_h
#define fznso_types_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * The FZnSO ABI version this crate defines.
 *
 * A solver exports it as `fznso_<name>_abi_version`, and a loader rejects any
 * solver whose reported version differs from its own.
 */
#define FZNSO_ABI_VERSION 1

/**
 * Representation of the base type of a value.
 */
typedef enum FznsoTypeBase {
  /**
   * Boolean type
   */
  FznsoTypeBaseBool,
  /**
   * Integer numeric type
   */
  FznsoTypeBaseInt,
  /**
   * Floating point numeric type
   */
  FznsoTypeBaseFloat,
  /**
   * Character string type
   */
  FznsoTypeBaseString,
} FznsoTypeBase;

/**
 * Enumerated type used to mark the kind of FznsoValue. This is used to
 * determine which "get" method in FznsoValueMethods is safe to call.
 */
typedef enum FznsoValueKind {
  /**
   * No value is available.
   */
  FznsoValueAbsent,
  /**
   * The value is available using FznsoValueMethods::as_decision.
   */
  FznsoValueDecision,
  /**
   * The value is available using FznsoValueMethods::as_constraint.
   */
  FznsoValueConstraint,
  /**
   * The value is available using FznsoValueMethods::as_bool.
   */
  FznsoValueBool,
  /**
   * The value is available using FznsoValueMethods::as_int.
   */
  FznsoValueInt,
  /**
   * The value is available using FznsoValueMethods::as_float.
   */
  FznsoValueFloat,
  /**
   * The value is available using FznsoValueMethods::as_string.
   */
  FznsoValueString,
  /**
   * Sets of integers are represented using a range list. The number of
   * ranges is available using FznsoValueMethods::len, and the ranges can
   * be accessed using FznsoValueMethods::get_range_int.
   */
  FznsoValueSetInt,
  /**
   * Sets of floats are represented using a range list. The number of
   * ranges is available using FznsoValueMethods::len, and the ranges can
   * be accessed using FznsoValueMethods::get_range_float.
   */
  FznsoValueSetFloat,
  /**
   * The length of the list can be accessed using
   * FznsoValueMethods::len, and elements in the list can be
   * accessed using FznsoValueMethods::get_element
   */
  FznsoValueList,
} FznsoValueKind;

/**
 * The status returned by `fznso_solver_run`, indicating whether the solver
 * completed its search.
 */
typedef enum FznsoStatus {
  /**
   * The solver explored the full search space and yielded all relevant
   * solutions.
   */
  FznsoComplete,
  /**
   * The solver did not explore the full search space due to a timeout or
   * other termination condition. Additional (better) solutions might be
   * possible.
   */
  FznsoIncomplete,
  /**
   * An error occurred during the solver's execution.
   *
   * `fznso_<name>_solver_read_error` can be used to retrieve the error
   * message.
   */
  FznsoError,
} FznsoStatus;

/**
 * The handle of an annotation on a decision, constraint or objective.
 *
 * Opaque: only the side that created it may interpret it. Read it through
 * FznsoAnnotationRef, which pairs it with the callbacks that can.
 */
typedef void FznsoAnnotation;

/**
 * A borrowed UTF-8 string, passed across the interface as a pointer/length
 * pair.
 *
 * The bytes are **not** required to be null-terminated; consumers must use
 * `len` to determine the string extent. This matches the convention used by
 * FznsoValueMethods::as_string, and allows implementations to hand out
 * slices of strings they already hold without copying them into a
 * null-terminated buffer.
 *
 * A null `ptr` signals the absence of a string, which is used where a name or
 * identifier is optional (e.g. FznsoModelMethods::decision_name).
 */
typedef struct FznsoStr {
  /**
   * Pointer to the first UTF-8 byte of the string, or null if absent.
   */
  const char *ptr;
  /**
   * The number of bytes in the string, **not** counting any trailing null
   * byte.
   */
  size_t len;
} FznsoStr;

/**
 * Representation of a type to signal and check whether an argument takes the
 * correct type.
 */
typedef struct FznsoType {
  /**
   * Whether the type is a list of values.
   */
  bool list_of;
  /**
   * Whether the argument can be or contain decision variables (represented
   * as decision indexes).
   */
  bool decision;
  /**
   * Whether expected type is an set of values of the base type.
   */
  bool set_of;
  /**
   * Whether the expected type is optional (and can take the value of
   * FznsoValueKind::FznsoValueAbsent).
   */
  bool opt;
  /**
   * The expected base type of the argument.
   */
  enum FznsoTypeBase base;
} FznsoType;

/**
 * Representation of a type of constraint, discerned by its identifier and the
 * types of its arguments.
 */
typedef struct FznsoConstraintType {
  /**
   * The identifier of the constraint type.
   */
  struct FznsoStr ident;
  /**
   * The number of expected arguments for the constraint type.
   */
  size_t arg_len;
  /**
   * The types of the expected arguments for the constraint type.
   */
  const struct FznsoType *arg_types;
} FznsoConstraintType;

/**
 * A list of FznsoConstraintTypes.
 *
 * This type is for example used to return from `fznso_<name>_constraint_list`.
 */
typedef struct FznsoConstraintList {
  /**
   * The number of elements in the `constraints` array.
   */
  size_t len;
  /**
   * An array of constraint types.
   */
  const struct FznsoConstraintType *constraints;
} FznsoConstraintList;

/**
 * An inclusive `[min, max]` float range, returned by
 * FznsoValueMethods::get_range_float.
 */
typedef struct FznsoFloatRange {
  /**
   * The inclusive lower bound.
   */
  double min;
  /**
   * The inclusive upper bound.
   */
  double max;
} FznsoFloatRange;

/**
 * An inclusive `[min, max]` integer range, returned by
 * FznsoValueMethods::get_range_int.
 */
typedef struct FznsoIntRange {
  /**
   * The inclusive lower bound.
   */
  int64_t min;
  /**
   * The inclusive upper bound.
   */
  int64_t max;
} FznsoIntRange;

/**
 * The handle of a model instance.
 *
 * Opaque: only the side that created it may interpret it. Read it through
 * FznsoModelRef, which pairs it with the callbacks that can.
 */
typedef void FznsoModel;

/**
 * Wrapper type for the indexes that represent decision variables in the model.
 */
typedef size_t FznsoDecisionIdx;

/**
 * The handle of a value: a constraint argument, a variable domain, a solution
 * assignment, an option or a statistic.
 *
 * Opaque: only the side that created it may interpret it. Read it through
 * FznsoValueRef, which pairs it with the callbacks that can.
 *
 * This is the handle least likely to be a pointer at all: a small payload such
 * as a decision index is normally stored *in* the pointer word rather than
 * behind it, and the absent value carries a null. Dispatch on
 * FznsoValueMethods::kind and use the accessor it permits; never
 * dereference the handle itself.
 */
typedef void FznsoValue;

/**
 * Wrapper type for the indexes that represent constraints in the model.
 */
typedef size_t FznsoConstraintIdx;

/**
 * The callbacks that read a FznsoValue.
 *
 * FznsoValueMethods::kind decides which of the others are meaningful; each
 * states the precondition it needs. A nested value or string that these return
 * borrows from the value it came from.
 */
typedef struct FznsoValueMethods {
  /**
   * Function callback that returns the kind of the value.
   */
  enum FznsoValueKind (*kind)(const FznsoValue *);
  /**
   * Function callback that returns the length of the value.
   *
   * In case FznsoValueMethods::kind returns
   * FznsoValueKind::FznsoValueString, the length is the number of bytes
   * in the string **not** counting any trailing null byte.  The pointer
   * returned by FznsoValueMethods::as_string is not required to be null-
   * terminated; callers must use this length to determine the string extent.
   *
   * In case FznsoValueMethods::kind returns
   * FznsoValueKind::FznsoValueList, the length is the number of elements
   * in the list, accessible using FznsoValueMethods::get_element.
   *
   * In case FznsoValueMethods::kind returns
   * FznsoValueKind::FznsoValueSetInt or
   * FznsoValueKind::FznsoValueSetFloat, the length is the number of
   * ranges in the set, accessible using
   * FznsoValueMethods::get_range_int or
   * FznsoValueMethods::get_range_float.
   *
   * Requires FznsoValueMethods::kind to return one of
   * FznsoValueKind::FznsoValueString,
   * FznsoValueKind::FznsoValueList,
   * FznsoValueKind::FznsoValueSetInt, or
   * FznsoValueKind::FznsoValueSetFloat.
   */
  size_t (*len)(const FznsoValue *);
  /**
   * Function callback that returns the decision variable index contained in
   * the value.
   *
   * Requires FznsoValueMethods::kind to return
   * FznsoValueKind::FznsoValueDecision.
   */
  FznsoDecisionIdx (*as_decision)(const FznsoValue *);
  /**
   * Function callback that returns the constraint index contained in the
   * value.
   *
   * Requires FznsoValueMethods::kind to return
   * FznsoValueKind::FznsoValueConstraint.
   */
  FznsoConstraintIdx (*as_constraint)(const FznsoValue *);
  /**
   * Function callback that returns the integer value contained in the value.
   *
   * Requires FznsoValueMethods::kind to return
   * FznsoValueKind::FznsoValueInt.
   */
  int64_t (*as_int)(const FznsoValue *);
  /**
   * Function callback that returns the floating point value contained in the
   * value.
   *
   * Requires FznsoValueMethods::kind to return
   * FznsoValueKind::FznsoValueFloat.
   */
  double (*as_float)(const FznsoValue *);
  /**
   * Function callback that returns a pointer to the UTF-8 bytes of a string
   * value.  The pointer is valid for FznsoValueMethods::len bytes and is
   * **not** required to be null-terminated (a trailing null byte may or may
   * not be present).
   *
   * Requires FznsoValueMethods::kind to return
   * FznsoValueKind::FznsoValueString.
   */
  const char *(*as_string)(const FznsoValue *);
  /**
   * Function callback that returns the Boolean value contained in the value.
   *
   * Requires FznsoValueMethods::kind to return
   * FznsoValueKind::FznsoValueBool.
   */
  bool (*as_bool)(const FznsoValue *);
  /**
   * Function callback that returns a range from a range list representing
   * the integer set contained in the value.
   *
   * Requires FznsoValueMethods::kind to return
   * FznsoValueKind::FznsoValueSetInt, and `index` to be below
   * FznsoValueMethods::len.
   */
  struct FznsoIntRange (*get_range_int)(const FznsoValue *, size_t index);
  /**
   * Function callback that returns a range from a range list representing
   * the floating point set contained in the value.
   *
   * Requires FznsoValueMethods::kind to return
   * FznsoValueKind::FznsoValueSetFloat, and `index` to be below
   * FznsoValueMethods::len.
   */
  struct FznsoFloatRange (*get_range_float)(const FznsoValue *, size_t index);
  /**
   * Function callback that returns an element from the list contained in the
   * value.
   *
   * Requires FznsoValueMethods::kind to return
   * FznsoValueKind::FznsoValueList, and `index` to be below
   * FznsoValueMethods::len.
   */
  struct FznsoValueRef (*get_element)(const FznsoValue *, size_t index);
} FznsoValueMethods;

/**
 * A value together with the callbacks that read it.
 */
typedef struct FznsoValueRef {
  /**
   * The data pointer to be the first argument of `get_value`.
   */
  const FznsoValue * data;
  /**
   * Reference to the structure containing the function callbacks used to
   * interact with the value.
   */
  const struct FznsoValueMethods *methods;
} FznsoValueRef;

/**
 * The callbacks that read a FznsoAnnotation.
 */
typedef struct FznsoAnnotationMethods {
  /**
   * Retrieve the identifier of an annotation.
   *
   * The returned string can be assumed to have the same lifetime as the
   * annotation reference and must be valid UTF8.
   */
  struct FznsoStr (*ident)(const FznsoAnnotation * ann);
  /**
   * Retrieve the number of arguments of an annotation.
   */
  size_t (*argument_len)(const FznsoAnnotation * ann);
  /**
   * Retrieve the value of the annotation's argument at the given index.
   *
   * The `index` argument must be less than the value returned by
   * `annotation_argument_len` for the given annotation.
   */
  struct FznsoValueRef (*argument)(const FznsoAnnotation * ann, size_t index);
} FznsoAnnotationMethods;

/**
 * An annotation together with the callbacks that read it.
 */
typedef struct FznsoAnnotationRef {
  /**
   * The data pointer of the annotation.
   */
  const FznsoAnnotation * data;
  /**
   * Reference to the structure containing the function callbacks used to
   * interact with the annotation.
   */
  const struct FznsoAnnotationMethods *methods;
} FznsoAnnotationRef;

/**
 * The callbacks that read a FznsoModel.
 *
 * Every index argument has a matching length callback, and must be below it.
 * Everything these return (e.g. values, strings, and annotations) borrows from
 * the model, and so is valid for as long as the model handle itself.
 */
typedef struct FznsoModelMethods {
  /**
   * Returns the current number of model layers currently contained in the
   * model.
   *
   * Layers are how an application adds and retracts groups of decision
   * variables and constraints between runs; see Self::layer_permanent
   * for the ones that can never be retracted.
   */
  size_t (*layer_len)(const FznsoModel * model);
  /**
   * Returns the number of layers that have been unchanged since the last
   * call to `fznso_<name>_solver_run`.
   *
   * Unchanged layers must be consecutive starting from layer 0.
   */
  size_t (*layer_unchanged)(const FznsoModel * model);
  /**
   * Returns the number of permanent layers in the model.
   *
   * Permanent layers are consecutive, starting from layer 0, and can never
   * be retracted, so a solver may assume their number only ever grows.
   *
   * Permanent layers can, however, still be found to be redundant, allowing
   * the solver to remove the decision variables and constraints in these
   * layers, if convenient. This is signaled using
   * Self::layer_redundant_len and Self::layer_redundant_index.
   */
  size_t (*layer_permanent)(const FznsoModel * model);
  /**
   * Returns the number of permanent layers that have been marked as
   * redundant.
   *
   * The solver can remove the decision variables and constraints in these
   * layers, if convenient. Once a layer is marked as redundant, it can
   * forever be considered redundant. As such, the solver can assume that
   * the number of redundant layers only ever increases.
   */
  size_t (*layer_redundant_len)(const FznsoModel * model);
  /**
   * Returns the index of the n-th permanent layer that has been marked as
   * redundant.
   *
   * The `n` argument must be less than the value returned by
   * Self::layer_redundant_len.
   */
  size_t (*layer_redundant_index)(const FznsoModel * model, size_t n);
  /**
   * Retrieve the number of decisions currently contained in the model.
   */
  size_t (*decision_len)(const FznsoModel * model);
  /**
   * Retrieve the exclusive upper bound on the decision indexes belonging to
   * layers `0..=layer`.
   *
   * Layer `layer` therefore owns the decisions
   * `decision_layer_end(layer-1)..decision_layer_end(layer)`, taking the
   * bound below layer 0 to be 0. It equals `decision_layer_end(layer-1)`
   * when the layer adds no decisions, and Self::decision_len for the
   * last layer.
   */
  size_t (*decision_layer_end)(const FznsoModel * model, size_t layer);
  /**
   * Retrieve the type of the given decision variable.
   *
   * This is what the type of value the decision must take, as opposed to
   * Self::decision_domain, which says which specific values it may take.
   */
  struct FznsoType (*decision_type)(const FznsoModel * model, FznsoDecisionIdx decision);
  /**
   * Retrieve the domain of the given decision.
   *
   * Note that the value might have FznsoValueKind::FznsoValueAbsent
   * if the decision variable does not have an explicit domain.
   */
  struct FznsoValueRef (*decision_domain)(const FznsoModel * model, FznsoDecisionIdx decision);
  /**
   * Retrieve the name of a decision variable, if it exists.
   *
   * Note that names are only available for debugging purposes. Decisions are
   * identified using their index in the model. If the decision variable does
   * not have a name, the returned string has a null `ptr`.
   */
  struct FznsoStr (*decision_name)(const FznsoModel * model, FznsoDecisionIdx decision);
  /**
   * Check whether the decision variable is defined by a constraint.
   */
  bool (*decision_defined)(const FznsoModel * model, FznsoDecisionIdx decision);
  /**
   * Check whether the application may ask a solution for this decision
   * variable's value.
   *
   * A solver must give every such variable a value in every solution it
   * reports, and is free to leave the others open.
   */
  bool (*decision_in_solution)(const FznsoModel * model, FznsoDecisionIdx decision);
  /**
   * Retrieve the number of annotations on a decision variable.
   */
  size_t (*decision_annotation_len)(const FznsoModel * model, FznsoDecisionIdx decision);
  /**
   * Retrieve the annotation on a decision variable at the given index.
   *
   * Requires `index` to be less than Self::decision_annotation_len for
   * that decision variable.
   */
  struct FznsoAnnotationRef (*decision_annotation)(const FznsoModel * model,
                                                   FznsoDecisionIdx decision,
                                                   size_t index);
  /**
   * Retrieve the number of constraints currently contained in the model.
   */
  size_t (*constraint_len)(const FznsoModel * model);
  /**
   * Retrieve the exclusive upper bound on the constraint indexes belonging
   * to layers `0..=layer`.
   *
   * Layer `layer` therefore owns the constraints
   * `constraint_layer_end(layer-1)..constraint_layer_end(layer)`, taking
   * the bound below layer 0 to be 0. It equals
   * `constraint_layer_end(layer-1)` when the layer adds no constraints, and
   * Self::constraint_len for the last layer.
   */
  size_t (*constraint_layer_end)(const FznsoModel * model, size_t layer);
  /**
   * Retrieve the identifier of a constraint.
   *
   * The returned string can be assumed to have the same lifetime as the
   * model reference and must be valid UTF8.
   */
  struct FznsoStr (*constraint_ident)(const FznsoModel * model, FznsoConstraintIdx constraint);
  /**
   * Retrieve the number of arguments of a constraint.
   */
  size_t (*constraint_argument_len)(const FznsoModel * model, FznsoConstraintIdx constraint);
  /**
   * Retrieve the value of the constraint's argument at the given index.
   *
   * The `index` argument must be less than the value returned by
   * `constraint_argument_len` for the given constraint.
   */
  struct FznsoValueRef (*constraint_argument)(const FznsoModel * model,
                                              FznsoConstraintIdx constraint,
                                              size_t index);
  /**
   * Check whether the decision variable is functionally defined by a
   * constraint.
   *
   * This function returns a FznsoValue that is either
   * FznsoValueKind::FznsoValueDecision if it defined a decision variable
   * or FznsoValueKind::FznsoValueAbsent otherwise.
   */
  struct FznsoValueRef (*constraint_defines)(const FznsoModel * model, FznsoConstraintIdx constraint);
  /**
   * Retrieve the number of annotations on a constraint.
   */
  size_t (*constraint_annotation_len)(const FznsoModel * model, FznsoConstraintIdx constraint);
  /**
   * Retrieve the annotation on a constraint at the given index.
   *
   * The `index` argument must be less than the value returned by
   * `constraint_annotation_len` for the given constraint.
   */
  struct FznsoAnnotationRef (*constraint_annotation)(const FznsoModel * model,
                                                     FznsoConstraintIdx constraint,
                                                     size_t index);
  /**
   * Request the identifier of the type of objective strategy to be used when
   * solving the model.
   *
   * Note that the returned string has a null `ptr` if the model does not
   * have an objective strategy.
   */
  struct FznsoStr (*objective_ident)(const FznsoModel * model);
  /**
   * Retrieve the argument of the objective strategy.
   */
  struct FznsoValueRef (*objective_arg)(const FznsoModel * model);
  /**
   * Retrieve the number of annotations on an objective
   */
  size_t (*objective_annotation_len)(const FznsoModel * model);
  /**
   * Retrieve the annotation on the objective at the given index.
   *
   * The `index` argument must be less than the value returned by
   * `objective_annotation_len`.
   */
  struct FznsoAnnotationRef (*objective_annotation)(const FznsoModel * model, size_t index);
} FznsoModelMethods;

/**
 * A model together with the callbacks that read it.
 */
typedef struct FznsoModelRef {
  /**
   * The handle to the data of the model instance.
   */
  const FznsoModel * data;
  /**
   * Reference to the structure containing the function callbacks used to
   * interact with the model.
   */
  const struct FznsoModelMethods *methods;
} FznsoModelRef;

/**
 * Representation of a type of objective strategies, discerned by its
 * identifier and the type of its argument.
 */
typedef struct FznsoObjective {
  /**
   * The identifier of the objective type.
   */
  struct FznsoStr ident;
  /**
   * The type of the expected argument for the constraint type.
   */
  struct FznsoType arg_type;
} FznsoObjective;

/**
 * A list of FznsoObjectives.
 *
 * This type is, for example, used to return from
 * `fznso_<name>_objective_list`.
 */
typedef struct FznsoObjectiveList {
  /**
   * The number of elements in the `objectives` array.
   */
  size_t len;
  /**
   * An array of objective strategies.
   */
  const struct FznsoObjective *objectives;
} FznsoObjectiveList;

/**
 * The definition of an option that is available to be set for the solver.
 */
typedef struct FznsoOption {
  /**
   * The identifier used to set the option or get the current value of the
   * option.
   */
  struct FznsoStr ident;
  /**
   * The type of value that is expected for this option.
   */
  struct FznsoType arg_ty;
  /**
   * The default value for this option.
   */
  struct FznsoValueRef arg_def;
} FznsoOption;

/**
 * A list of FznsoOptions.
 *
 * This type is, for example, used to return from `fznso_<name>_option_list`.
 */
typedef struct FznsoOptionList {
  /**
   * The number of elements in the `options` array.
   */
  size_t len;
  /**
   * An array of option definitions.
   */
  const struct FznsoOption *options;
} FznsoOptionList;

/**
 * The handle of a solution reported by a solver.
 *
 * Opaque: only the side that created it may interpret it. Read it through
 * FznsoSolutionRef, which pairs it with the callbacks that can.
 */
typedef void FznsoSolution;

/**
 * The callbacks that read a FznsoSolution.
 *
 * The values these return borrow from the solution, so they last only as long
 * as it does (i.e. for the duration of the `on_solution` callback that
 * delivered it). Copy out anything that has to outlive the call.
 */
typedef struct FznsoSolutionMethods {
  /**
   * Function callback to retrieve the value assigned to a decision variable
   * in the solution.
   */
  struct FznsoValueRef (*value)(const FznsoSolution * data, size_t decision_index);
  /**
   * Function callback to retrieve the statistical information made available
   * by the solver about the search process so far.
   */
  struct FznsoValueRef (*statistic)(const FznsoSolution * data, struct FznsoStr ident);
} FznsoSolutionMethods;

/**
 * A solution together with the callbacks that read it.
 */
typedef struct FznsoSolutionRef {
  /**
   * The data pointer to be used as the first argument in the function
   * callbacks provided by the `methods` field.
   */
  const FznsoSolution * data;
  /**
   * Reference to the structure containing the function callbacks used to
   * interact with the solution.
   */
  const struct FznsoSolutionMethods *methods;
} FznsoSolutionRef;

/**
 * The handle of a solver instance, as returned by `fznso_solver_create`.
 *
 * Opaque: only the solver library may interpret it. An application passes it
 * back to the library's entry points and releases it with
 * `fznso_solver_free`.
 */
typedef void FznsoSolver;

/**
 * The definition of statistical information that is made available by the
 * solver.
 *
 * The `solution` and `solver` flags say *where* the statistic can be read
 * from, and are independent: a statistic may be available from a solution,
 * from the solver instance, or from both. At least one of them must be set —
 * a statistic with neither could be read from nowhere.
 */
typedef struct FznsoStatistic {
  /**
   * The identifier used to retrieve the statistical information from the
   * solver or a solution.
   */
  struct FznsoStr ident;
  /**
   * The type of value that is expected for this statistic.
   */
  struct FznsoType ty;
  /**
   * Whether the statistic is available from a solution, read with
   * FznsoSolutionMethods::statistic.
   */
  bool solution;
  /**
   * Whether the statistic is available from the solver instance, read with
   * `fznso_solver_statistic`.
   */
  bool solver;
} FznsoStatistic;

/**
 * A list of FznsoStatistics.
 *
 * This type is, for example, used to return from
 * `fznso_<name>_statistic_list`.
 */
typedef struct FznsoStatisticList {
  /**
   * The number of elements in the `stats` array.
   */
  size_t len;
  /**
   * An array of statistic definitions.
   */
  const struct FznsoStatistic *stats;
} FznsoStatisticList;

/**
 * A list of FznsoTypes.
 *
 * This type is for example used to return from `fznso_<name>_decision_list`.
 */
typedef struct FznsoTypeList {
  /**
   * The number of elements in the `types` array.
   */
  size_t len;
  /**
   * An array of decision-variable types.
   */
  const struct FznsoType *types;
} FznsoTypeList;

#endif  /* fznso_types_h */
