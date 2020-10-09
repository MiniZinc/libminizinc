/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * Main author: Matthieu Herrmann, Monash University, Melbourne, Australia. 2019
 */

#pragma once

#include <minizinc/ast.hh>
#include <minizinc/astvec.hh>
#include <minizinc/solvers/nl/nl_components.hh>

#include <map>
#include <ostream>
#include <set>
#include <string>

// This files declare data-structure describing the various components of a nl files.
// A nl files is composed of two main parts: a header and a list of segments.
// The header contains statistics and meta information about the model.
// The segments allow to describe the model, i.e. the variables, the constraints and objectives.
// The order of the segments and, when relevant, the order of their content
// (e.g. a segment declaring a list of variables) matters.

/** NL File.
 *  Good to know:
 *      * We use string as variable unique identifier.
 *        Given a MZN variable declaration (can be obtain from a MZN variable), the 'getVarName'
 * helper produces the string.
 *      * In our case, we only have one 'solve' per file.
 *      * NL file use double everywhere. Hence, even with dealing with integer variable, we store
 * the information with double.
 *
 */

namespace MiniZinc {

// --- --- --- NL Files
class NLFile {
public:
  /* *** *** *** Helpers *** *** *** */

  /** Create a string representing the name (and unique identifier) from an identifier. */
  static std::string getVarName(const Id* id);

  /** Create a string representing the name (and unique identifier) of a variable from a variable
   * declaration. */
  static std::string getVarName(const VarDecl& vd);

  /** Create a string representing the name (and unique identifier) of a constraint from a specific
   * call expression. */
  static std::string getConstraintName(const Call& c);

  /** Extract an array literal from an expression. */
  static const ArrayLit& getArrayLit(const Expression* e);

  /** Create a vector of double from a vector containing Expression being integer literal IntLit. */
  static std::vector<double> fromVecInt(const ArrayLit& v_int);

  /** Create a vector of double from a vector containing Expression being float literal FloatLit. */
  static std::vector<double> fromVecFloat(const ArrayLit& v_fp);

  /** Create a vector of variable names from a vector containing Expression being identifier Id. */
  static std::vector<std::string> fromVecId(const ArrayLit& v_id);

  /* *** *** *** Phase 1: collecting data from MZN *** *** *** */

  // Variables collection, identified by name
  // Needs ordering, see phase 2
  std::map<std::string, NLVar> variables = {};

  // Algebraic constraints collection, identified by name
  // Needs ordering, see phase 2
  std::map<std::string, NLAlgCons> constraints = {};

  // Logical constraints do not need ordering:
  std::vector<NLLogicalCons> logicalConstraints = {};

  // Objective field. Only one, so we do not need ordering.
  NLObjective objective = {};

  // Output arrays
  std::vector<NLArray> outputArrays = {};

  /** Add a solve goal in the NL File. In our case, we can only have one and only one solve goal. */
  void addSolve(SolveI::SolveType st, const Expression* e);

  /** Add a variable declaration in the NL File.
   *  This function pre-analyse the declaration VarDecl, then delegate to addVarDeclInteger or
   * addVarDeclFloat. Analyse a variable declaration 'vd' of type 'ti' with an 'rhs'. The variable
   * declaration gives us access to the variable name while the type allows us to discriminate
   * between integer, floating point value and arrays. Array are ignored (not declared): if we
   * encouter an array in a constraint, we can find the array through the variable (ot it is a
   * litteral). Notes:  - We use -Glinear, so we do not have boolean.
   *          - This will change TODO keep checking comment and code consistency.
   *
   * RHS is for arrays: it contains the definition of the array.
   *
   * The type also gives us the domain, which can be:
   *  NULL:       no restriction over the variable
   *  SetLit:     Gives us a lower and upper bound
   *  If a variable is bounded only on one side, then the domain is NULL and the bound is expressed
   * through a constraint.
   */
  void addVarDecl(const VarDecl& vd, const TypeInst& ti, const Expression& rhs);

  /** Add an integer variable declaration to the NL File. */
  void addVarDeclInteger(const std::string& name, const IntSetVal* isv, bool toReport);

  /** Add a floating point variable declaration to the NL File. */
  void addVarDeclFloat(const std::string& name, const FloatSetVal* fsv, bool toReport);

  // --- --- --- Constraints analysis

  /** Add a constraint to the NL File.
   * This method is a dispatcher for all the other constraints methods below. */
  void analyseConstraint(const Call& c);

  // --- --- --- Helpers

  /** Create a token from an expression representing a variable.
   * ONLY USE FOR CONSTRAINT, NOT OBJECTIVES! (UPDATE VARIABLES FLAG FOR CONSTRAINTS)
   */
  static NLToken getTokenFromVar(const Expression* e);

  /** Create a token from an expression representing either a variable or an integer numeric value.
   * ONLY USE FOR CONSTRAINT, NOT OBJECTIVES!
   */
  static NLToken getTokenFromVarOrInt(const Expression* e);

  /** Create a token from an expression representing either a variable or a floating point numeric
   * value. ONLY USE FOR CONSTRAINT, NOT OBJECTIVES!
   */
  static NLToken getTokenFromVarOrFloat(const Expression* e);

  /** Update an expression graph (only by appending token) with a linear combination
   *  of coefficients and variables.
   *  ONLY USE FOR CONSTRAINTS, NOT OBJECTIVES!
   */
  static void makeSigmaMult(std::vector<NLToken>& expressionGraph,
                            const std::vector<double>& coeffs,
                            const std::vector<std::string>& vars);

  // --- --- --- Linear Builders
  // Use an array of literals 'coeffs' := c.arg(0), an array of variables 'vars' := c.arg(1),
  // and a variable or literal 'value' := c.arg(2).
  // [coeffs] and value are fixed (no variable allowed).
  // The call is needed to create the name. However, the extraction of the coefficients and the
  // value is left to the calling function as this could be use with both integer and floating point
  // (we only have floating point in NL)

  /** Create a linear constraint [coeffs] *+ [vars] = value. */
  void linconsEq(const Call& c, const std::vector<double>& coeffs,
                 const std::vector<std::string>& vars, const NLToken& value);

  /** Create a linear constraint [coeffs] *+ [vars] <= value. */
  void linconsLe(const Call& c, const std::vector<double>& coeffs,
                 const std::vector<std::string>& vars, const NLToken& value);

  /** Create a linear logical constraint [coeffs] *+ [vars] PREDICATE value.
   *  Use a generic comparison operator.
   *  Warnings:   - Creates a logical constraint
   *              - Only use for conmparisons that cannot be expressed with '=' xor '<='.
   */
  void linconsPredicate(const Call& c, NLToken::OpCode oc, const std::vector<double>& coeffs,
                        const std::vector<std::string>& vars, const NLToken& value);

  // --- --- --- Non Linear Builders
  // For predicates, uses 2 variables or literals: x := c.arg(0), y := c.arg(1)
  // x PREDICATE y

  // For unary operations, uses 2 variables or literals: x := c.arg(0), y := c.arg(1)
  // OPEARTOR x = y

  // For binary operations, uses 3 variables or literals: x := c.arg(0), y := c.arg(1), and z :=
  // c.arg(2). x OPERATOR y = z

  /** Create a non linear constraint x = y
   *  Use the jacobian and the bound on constraint to translate into x - y = 0
   *  Simply update the bound if one is a constant.
   */
  void nlconsEq(const Call& c, const NLToken& x, const NLToken& y);

  /** Create a non linear constraint x <= y
   *  Use the jacobian and the bound on constraint to translate into x - y <= 0
   *  Simply update the bound if one is a constant.
   */
  void nlconsLe(const Call& c, const NLToken& x, const NLToken& y);

  /** Create a non linear constraint with a predicate: x PREDICATE y
   *  Use a generic comparison operator.
   *  Warnings:   - Creates a logical constraint
   *              - Only use for conmparisons that cannot be expressed with '=' xor '<='.
   */
  void nlconsPredicate(const Call& c, NLToken::OpCode oc, const NLToken& x, const NLToken& y);

  /** Create a non linear constraint with a binary operator: x OPERATOR y = z */
  void nlconsOperatorBinary(const Call& c, NLToken::OpCode oc, const NLToken& x, const NLToken& y,
                            const NLToken& z);

  /** Create a non linear constraint with a binary operator: x OPERATOR y = z.
   *  OPERATOR is now a Multiop, with a count of 2 (so the choice of the method to use depends on
   * the LN implementation) */
  void nlconsOperatorBinary(const Call& c, NLToken::MOpCode moc, const NLToken& x, const NLToken& y,
                            const NLToken& z);

  /** Create a non linear constraint with an unary operator: OPERATOR x = y */
  void nlconsOperatorUnary(const Call& c, NLToken::OpCode oc, const NLToken& x, const NLToken& y);

  /** Create a non linear constraint, specialized for log2 unary operator: Log2(x) = y */
  void nlconsOperatorUnaryLog2(const Call& c, const NLToken& x, const NLToken& y);

  // --- --- --- Integer Linear Constraints

  /** Linar constraint: [coeffs] *+ [vars] = value */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void consint_lin_eq(const Call& c);

  /** Linar constraint: [coeffs] *+ [vars] =< value */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void consint_lin_le(const Call& c);

  /** Linar constraint: [coeffs] *+ [vars] != value */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void consint_lin_ne(const Call& c);

  // --- --- --- Integer Non Linear Predicate Constraints

  /** Non linear constraint x = y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void consint_eq(const Call& c);

  /** Non linear constraint x <= y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void consint_le(const Call& c);

  /** Non linear constraint x != y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void consint_ne(const Call& c);

  // --- --- --- Integer Non Linear Binary Operator Constraints

  /** Non linear constraint x + y = z */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void consint_plus(const Call& c);

  /** Non linear constraint x * y = z */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void consint_times(const Call& c);

  /** Non linear constraint x / y = z */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void consint_div(const Call& c);

  /** Non linear constraint x mod y = z */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void consint_mod(const Call& c);

  /** Non linear constraint x pow y = z */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void int_pow(const Call& c);

  /** Non linear constraint max(x, y) = z */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void int_max(const Call& c);

  /** Non linear constraint min(x, y) = z */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void int_min(const Call& c);

  // --- --- --- Integer Non Linear Unary Operator Constraints

  // NOLINTNEXTLINE(readability-identifier-naming)
  void int_abs(const Call& c);

  // --- --- --- Floating Point Linear Constraints

  /** Linar constraint: [coeffs] *+ [vars] = value */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void consfp_lin_eq(const Call& c);

  /** Linar constraint: [coeffs] *+ [vars] = value */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void consfp_lin_le(const Call& c);

  /** Linar constraint: [coeffs] *+ [vars] != value */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void consfp_lin_ne(const Call& c);

  /** Linar constraint: [coeffs] *+ [vars] < value */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void consfp_lin_lt(const Call& c);

  // --- --- --- Floating Point Non Linear Predicate Constraints

  /** Non linear constraint x = y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void consfp_eq(const Call& c);

  /** Non linear constraint x <= y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void consfp_le(const Call& c);

  /** Non linear constraint x != y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void consfp_ne(const Call& c);

  /** Non linear constraint x < y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void consfp_lt(const Call& c);

  // --- --- --- Floating Point Non Linear Binary Operator Constraints

  /** Non linear constraint x + y = z */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void consfp_plus(const Call& c);

  /** Non linear constraint x - y = z */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void consfp_minus(const Call& c);

  /** Non linear constraint x * y = z */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void consfp_times(const Call& c);

  /** Non linear constraint x / y = z */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void consfp_div(const Call& c);

  /** Non linear constraint x mod y = z */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void consfp_mod(const Call& c);

  /** Non linear constraint x pow y = z */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void float_pow(const Call& c);

  /** Non linear constraint max(x, y) = z */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void float_max(const Call& c);

  /** Non linear constraint min(x, y) = z */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void float_min(const Call& c);

  // --- --- --- Floating Point Non Linear Unary Operator Constraints

  /** Non linear constraint abs x = y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void float_abs(const Call& c);

  /** Non linear constraint acos x = y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void float_acos(const Call& c);

  /** Non linear constraint acosh x = y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void float_acosh(const Call& c);

  /** Non linear constraint asin x = y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void float_asin(const Call& c);

  /** Non linear constraint asinh x = y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void float_asinh(const Call& c);

  /** Non linear constraint atan x = y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void float_atan(const Call& c);

  /** Non linear constraint atanh x = y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void float_atanh(const Call& c);

  /** Non linear constraint cos x = y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void float_cos(const Call& c);

  /** Non linear constraint cosh x = y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void float_cosh(const Call& c);

  /** Non linear constraint exp x = y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void float_exp(const Call& c);

  /** Non linear constraint ln x = y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void float_ln(const Call& c);

  /** Non linear constraint log10 x = y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void float_log10(const Call& c);

  /** Non linear constraint log2 x = y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void float_log2(const Call& c);

  /** Non linear constraint sqrt x = y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void float_sqrt(const Call& c);

  /** Non linear constraint sin x = y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void float_sin(const Call& c);

  /** Non linear constraint sinh x = y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void float_sinh(const Call& c);

  /** Non linear constraint tan x = y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void float_tan(const Call& c);

  /** Non linear constraint tanh x = y */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void float_tanh(const Call& c);

  // --- --- --- Other

  /** Integer x to floating point y. Constraint x = y translated into x - y = 0. */
  // NOLINTNEXTLINE(readability-identifier-naming)
  void int2float(const Call& c);

  /* *** *** *** Phase 2: processing *** *** *** */

  void phase2();

  // Ordering of variables according to "hooking your solver"
  /*  Meaning of the names (total, then by appearance order in the tables below)
          n_var               total number of variables
          nlvc                number of variables appearing nonlinearly in constraints
          nlvo                number of variables appearing nonlinearly in objectives
          nwv                 number of linear arcs
          niv                 number of "other" integer variables
          nbv                 number of binary variables


      Order of variables (yes, the way things are counted is... "special".)
      Category            Count
      --- --- --- --- |   --- --- --- --- ---
      nonlinear           max(nlvc, nlvo)                                 // See below for order on
     non linear variables linear arcs         nwv                                             // Not
     implemented other linear        n_var − (max {nlvc, nlvo} + niv + nbv + nwv)    // Linear
     Continuous binary              nbv                                             // Booleans
      other integer       niv                                             // Linear Integer



      Order of non linear variables (see 'nonlinear' above)
      Meaning of the names:
          nlvb            number of variables appearing nonlinearly in both constraints and
     objectives nlvbi           number of integer variables appearing nonlinearly in both
     constraints and objectives nlvc            number of variables appearing nonlinearly in
     constraints nlvci           number of integer variables appearing nonlinearly in constraints
     **only** nlvo            number of variables appearing nonlinearly in objectives nlvoi number
     of integer variables appearing nonlinearly in objectives **only**

      Category                                                Count
      --- --- --- --- --- --- --- --- --- --- --- --- --- |   --- --- --- --- ---
      Continuous in BOTH an objective AND a constraint    |   nlvb - nlvbi
      Integer, in BOTH an objective AND a constraint      |   nlvbi
      Continuous, in constraints only                     |   nlvc − (nlvb + nlvci)
      Integer, in constraints only                        |   nlvci
      Continous, in objectives only                       |   nlvo − (nlvc + nlvoi)
      Integer, in objectives only                         |   nlvoi
  */

  /** Non Linear Continuous Variables in BOTH an objective and a constraint. */
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<std::string> vname_nlcv_both = {};

  /** Non Linear Integer Variables in BOTH an objective and a constraint. */
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<std::string> vname_nliv_both = {};

  /** Non Linear Continuous Variables in CONStraints only. */
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<std::string> vname_nlcv_cons = {};

  /** Non Linear Integer Variables in CONStraints only. */
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<std::string> vname_nliv_cons = {};

  /** Non Linear Continuous Variables in OBJectives only. */
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<std::string> vname_nlcv_obj = {};

  /** Non Linear Integer Variables in OBJectives only. */
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<std::string> vname_nliv_obj = {};

  /** Linear arcs. (Network not implemented) */
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<std::string> vname_larc_all = {};

  /** Linear Continuous Variables (ALL of them). */
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<std::string> vname_lcv_all = {};

  /** Binary Variables (ALL of them). */
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<std::string> vname_bv_all = {};

  /** Linear Integer Variables (ALL of them). */
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<std::string> vname_liv_all = {};

  /** Contained all ordered variable names. Mapping variable index -> variable name */
  std::vector<std::string> vnames = {};

  /** Mapping variable name -> variable index */
  std::map<std::string, int> variableIndexes = {};

  // --- --- --- Simple tests

  bool hasIntegerVars() const;

  bool hasContinousVars() const;

  // --- --- --- Variables  counts

  // When the phase 2 is done, all the following counts should be available.
  // taken from "hooking your solver" and used in the above explanatios

  /** Total number of variables. */
  unsigned int varCount() const;

  /** Number of variables appearing nonlinearly in constraints. */
  unsigned int lvcCount() const;

  /** Number of variables appearing nonlinearly in objectives. */
  unsigned int lvoCount() const;

  /** Number of variables appearing nonlinearly in both constraints and objectives.*/
  unsigned int lvbCount() const;

  /** Number of integer variables appearing nonlinearly in both constraints and objectives.*/
  unsigned int lvbiCount() const;

  /** Number of integer variables appearing nonlinearly in constraints **only**.*/
  unsigned int lvciCount() const;

  /** Number of integer variables appearing nonlinearly in objectives **only**.*/
  unsigned int lvoiCount() const;

  /** Number of linear arcs .*/
  unsigned int wvCount() const;

  /** Number of "other" integer variables.*/
  unsigned int ivCount() const;

  /** Number of binary variables.*/
  unsigned int bvCount() const;

  /** Accumulation of Jacobian counts. */
  unsigned int jacobianCount() const;

  // Ordering of constraints according to "hooking your solver"
  /*  Meaning of the names:
          n_con       Total number of constraint
          nlc         Number of nonlinear general constraint, including network constraint
          nlnc        Number of nonlinear network constraint
          lnc         Number of linear network constraint

      Order of constraints:
      Category                Count
      --- --- --- --- --- |   --- --- --- --- ---
      Nonlinear general       nlc - nlnc
      Nonlinear network       nlnc
      Linear network          lnc
      Linear general          n_con - (nlc + lnc)
  */

  /** Nonlinear general constraints. */
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<std::string> cnames_nl_general = {};

  /** Nonlinear network constraints. */
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<std::string> cnames_nl_network = {};

  /** Linear network constraints. */
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<std::string> cnames_lin_network = {};

  /** Linear general constraints. */
  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<std::string> cnames_lin_general = {};

  /** Contained all ordered algebraic (and network if they were implemented) constraints names.
   *  Mapping constraint index -> constraint name
   */
  std::vector<std::string> cnames = {};

  /** Mapping constraint name -> contraint index */
  std::map<std::string, int> constraintIndexes = {};

  // Count of algebraic constraints:
  // The header needs to know how many range algebraic constraints and equality algebraic
  // constraints we have.
  /** Number of range algebraic constraints */
  int algConsRangeCount = 0;
  /** equality algebraic constraints */
  int algConsEqCount = 0;

  /* *** *** *** Constructor *** *** *** */

  NLFile() = default;

  /* *** *** *** Printable *** *** *** */

  /** Print the NLFile on a stream.
   *  Note: this is not the 'Printable' interface as we do not pass any nl_file (that would be
   * 'this') as a reference.
   */
  std::ostream& printToStream(std::ostream& o) const;

private:
  unsigned int _jacobianCount = 0;
};

}  // End of NameSpace MiniZinc
