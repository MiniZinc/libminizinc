/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

//#include <assert.h>

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

/*  A NL File is composed of a header and several segments.
Adding items in the nl file is done through adding segment (or adding item in a segment).
As for the header, segment are printable.
Segment are identified by a prefix, which should be one of (taken from table 13 in 'writing nl
files'): F   imported function description S   suffix values V   defined variable definition (must
precede V,C,L,O segments where used) (yes, I know, "V must preceed V"...) C   algebraic constraint
body L   logical constraint expression O   objective function d   dual initial guess x   primal
initial guess r   bounds on algebraic constraint bodies (“ranges”), can only appears once b   bounds
on variable, can only appears once k   Jacobian column counts (must precede all J segments) J
Jacobian sparsity, linear terms G   Gradient sparsity, linear terms
*/

namespace MiniZinc {

// --- --- --- Tooling

/** Exception when translating
 *  Code mostly taken from https://www.softwariness.com/articles/assertions-in-cpp/
 */
class NLException : public std::exception {
public:
  // --- --- --- Fields
  const char* expression;
  const char* file;
  int line;
  std::string message;
  std::string report;

  /** Exception constructor. Use with the macro assert/should_not_happen.
   * If not, WARNING: stream must be a std::ostringstream&
   * We only use a ostreamé so we can use the standard "<<" operator, which is returning a ostream&
   */
  NLException(const char* expression, const char* file, int line, std::ostream& stream)
      : expression(expression), file(file), line(line) {
    message = static_cast<std::ostringstream&>(stream).str();
    std::ostringstream outputStream;

    if (expression == nullptr) {
      outputStream << "Something should not have happen in file '" << file << "' line " << line
                   << ". Message:" << std::endl;
      if (!message.empty()) {
        outputStream << message << std::endl;
      } else {
        outputStream << "No message provided..." << std::endl;
      }
    } else {
      std::string expressionString = expression;
      if (expressionString == "false" || expressionString == "0" || expressionString == "FALSE") {
        outputStream << "Unreachable code assertion";
      } else {
        outputStream << "Assertion '" << expression << "'";
      }
      outputStream << " failed in file '" << file << "' line " << line << std::endl;
    }
    outputStream << "Note: the NL component is still in development!" << std::endl;
    report = outputStream.str();
  }

  /** Exception interface */
  const char* what() const noexcept override { return report.c_str(); }

  ~NLException() noexcept override = default;
};

#ifdef assert
#undef assert
#endif

/** Should not happen macro */
#define should_not_happen(MESSAGE)                           \
  do {                                                       \
    ostringstream oss;                                       \
    oss << MESSAGE; /* NOLINT(bugprone-macro-parentheses) */ \
    throw NLException(NULL, __FILE__, __LINE__, oss);        \
  } while (false)

/* CMake febug build flag: double negation... because... ? */
#ifndef NDEBUG
#define DEBUG_MSG(STR)                                                                   \
  do {                                                                                   \
    std::cerr << "%[NL DEBUG] " << STR << endl; /* NOLINT(bugprone-macro-parentheses) */ \
  } while (false)
#define assert(EXPRESSION)                                     \
  do {                                                         \
    if (!(EXPRESSION)) {                                       \
      ostringstream oss;                                       \
      throw NLException(#EXPRESSION, __FILE__, __LINE__, oss); \
    }                                                          \
  } while (false)
#else
#define DEBUG_MSG(STR) \
  do {                 \
  } while (false)
#define assert(EXPRESSION) \
  do {                     \
  } while (false)
#endif

// --- --- --- Components

// Declaration
class NLFile;

/** A Bound.
 *  A bound can represent various constraint on a variable or a constraint.
 *  Because it apply to both variables and constraints, we keep it general enough.
 *  Note that the targeted variable or constraint is implicitely represented by its position in the
 final NL File.
 *  As a result, this information does not appear here.
 *  Bounds are used in the 'b' and 'r' segments.

   # Text       # Starting the segment  # Variable          # Tag in enum NLS_Bounditem::Bound
   0 1.1 3.4    # 1.1 =< V =< 3.4       First variable      LB_UB
   1 2.5        # V =< 2.5              Second variable     UB
   2 7          # 7 =< V                etc...              LB
   3            # no constraint                             NONE
   4 9.4        # V = 9.4                                   EQ

 * Notes:   - bound values are stored as 'double', even for integer variables.
 *          - we do not make that class Printable as it is better "printed" with a name for the
 targeted variable/constraint
 */
class NLBound {
public:
  /** Bound kind. Declaration matches specification above. */
  enum Bound { LB_UB = 0, UB = 1, LB = 2, NONE = 3, EQ = 4 };

  /** *** *** *** Fields *** *** *** **/
  Bound tag = NONE;
  double lb = 0;
  double ub = 0;

  /** *** *** *** Constructors & helpers *** *** *** **/
  NLBound() = default;
  NLBound(Bound tag, double lb, double ub);

  static NLBound makeBounded(double lb, double ub);
  static NLBound makeUBBounded(double ub);
  static NLBound makeLBBounded(double lb);
  static NLBound makeNoBound();
  static NLBound makeEqual(double val);

  /** *** *** *** Update the lower or upper bound *** *** *** **/
  // Note: this method are "additive only": we cannot use them to remove a a bound.
  void updateLB(double new_lb);
  void updateUB(double new_ub);
  void updateEq(double new_eq);

  /** *** *** *** Printing Methods *** *** *** **/

  /** Print the bound with a comment containing the name of the variable/constraint. */
  std::ostream& printToStream(std::ostream& o, const std::string& vname) const;

  /** Printing with 'body' as the name of the variable/constraint. */
  std::ostream& printToStream(std::ostream& o) const;
};

/** A Declared variable.
 *  A variable is identified by its name, which is supposed to be unique in the MZN representation.
 *  In an NL file, variables are identified by their index. However, those index are dependent on
 * the variable ordering, which can only be known once all variables are known. Hence, the
 * computation of the index can only be achieved at a later stage. A variable is always associated
 * to a bound, even if none are specified (See LNBound above)/
 */
class NLVar {
public:
  /** Variable name. */
  std::string name;

  /** Is the variable an integer variable? Else is a floating point variable. */
  bool isInteger = false;

  /** Is this variable flagged to be reported? */
  bool toReport = false;

  /** Is the variable appearing in a nonlinear constraint (including logical constraint, L segment).
   */
  bool isInNLConstraint = false;

  /** Is the variable appearing non linearly in the objective? */
  bool isInNLObjective = false;

  /** Number of occurrences in Jacobian. */
  unsigned int jacobianCount = 0;

  /** The bound over this variable.
   *  Used when producing the unique 'b' segment of the NL file.
   */
  NLBound bound;

  /* *** *** *** Constructors *** *** *** */
  NLVar() = default;

  /** Constructor with declare time information */
  NLVar(std::string name, bool isInteger, bool to_report, NLBound bound)
      : name(std::move(name)), isInteger(isInteger), toReport(to_report), bound(bound) {}

  /** Copy constructor, with update on bound */
  NLVar copyWithBound(NLBound bound) const;
};

/** A NLArray:
 * We do not use "real" array.
 * This type only serves when sending the result back to minizinc
 */
class NLArray {
public:
  /** Array item; if the string is empty, use the value. */
  class Item {
  public:
    std::string variable;
    double value;
  };

  /** Array name */
  std::string name;

  /** Dimensions part, e.g. array2d( '0..4', '0..5' [ .... ]) */
  std::vector<std::string> dimensions;

  /** Related variables */
  std::vector<Item> items;

  /** Is this an array or integers or floats? */
  bool isInteger = false;
};

/** A token from an 'expression graph'.
 *  An expression graph is express in Polish Prefix Notation: operator followed by operand.
 *  A token represent an operator or an operand.
 *  See the definition of the various enum.
 */
class NLToken {
public:
  /** Kind of token. */
  enum Kind {
    NUMERIC,   // "n42.42"     a numeric constant, double
    VARIABLE,  // "v4"         reference to a decision variable 0<= i < nb_vars (see header) or a
               // defined variable for i>=nb_vars
    STRING,    // "h11:some string"    Probably unused in our case.
    FUNCALL,   // "f0 3"               Call a defined function (index 0, 3 args). Probably unused in
               // our case.
    OP,        // "o5"                 An operation defined by its operation code
    MOP  // "o7\n3"              Operator with multiple operand. The number of operands (3) is on
         // the next line ("\n")?
  };

  /** Opcode for operator with a fixed number of operands. */
  enum OpCode {
    OPPLUS = 0,
    OPMINUS = 1,
    OPMULT = 2,
    OPDIV = 3,
    OPREM = 4,
    OPPOW = 5,
    OPLESS = 6,
    FLOOR = 13,
    CEIL = 14,
    ABS = 15,
    OPUMINUS = 16,
    OPOR = 20,
    OPAND = 21,
    LT = 22,
    LE = 23,
    EQ = 24,
    GE = 28,
    GT = 29,
    NE = 30,
    OPNOT = 34,
    OPIFnl = 35,
    OP_tanh = 37,
    OP_tan = 38,
    OP_sqrt = 39,
    OP_sinh = 40,
    OP_sin = 41,
    OP_log10 = 42,
    OP_log = 43,
    OP_exp = 44,
    OP_cosh = 45,
    OP_cos = 46,
    OP_atanh = 47,
    OP_atan2 = 48,
    OP_atan = 49,
    OP_asinh = 50,
    OP_asin = 51,
    OP_acosh = 52,
    OP_acos = 53,
    OPintDIV = 55,
    OPprecision = 56,
    OPround = 57,
    OPtrunc = 58,
    OPATLEAST = 62,
    OPATMOST = 63,
    OPPLTERM = 64,
    OPIFSYM = 65,
    OPEXACTLY = 66,
    OPNOTATLEAST = 67,
    OPNOTATMOST = 68,
    OPNOTEXACTLY = 69,
    OPIMPELSE = 72,
    OP_IFF = 73,
    OPSOMESAME = 75,
    OP1POW = 76,
    OP2POW = 77,
    OPCPOW = 78,
    OPFUNCALL = 79,
    OPNUM = 80,
    OPHOL = 81,
    OPVARVAL = 82,
    N_OPS = 83,
  };

  /** Opcodes for operand taking multiple arguments. */
  enum MOpCode {
    MINLIST = 11,
    MAXLIST = 12,
    OPSUMLIST = 54,
    OPCOUNT = 59,
    OPNUMBEROF = 60,
    OPNUMBEROFs = 61,
    ANDLIST = 70,
    ORLIST = 71,
    OPALLDIFF = 74,
  };

  /** Obtain the name of an operator from its opcode. */
  static const char* getName(OpCode oc);

  /** Obtain the name of an operator (with multiple operands) from its opcode. */
  static const char* getName(MOpCode moc);

  /* *** *** *** Fields *** *** *** */

  Kind kind;
  double numericValue;  // if kind==NUMERIC
  int argCount;         // if kind==FUNCALL or kind==MOP
  std::string
      str;      // if kind==STRING or kind=VARIABLE (variable name) or kind=FUNCALL (function name)
  OpCode oc;    // if kind==OP
  MOpCode moc;  // if kind==MOP

  /* *** *** *** Constructor and helpers *** *** *** */

  NLToken() = default;

  static NLToken n(double value);

  static NLToken v(std::string vname);

  static NLToken o(OpCode opc);

  static NLToken mo(MOpCode mopc, int nb);

  /* *** *** *** Query *** *** *** */
  bool isVariable() const;

  bool isConstant() const;

  /* *** *** *** Printable *** *** *** */

  std::ostream& printToStream(std::ostream& o, const NLFile& nl_file) const;
};

/** A algebraic constraint.
 *  Contains both a linear and a non linear part.
 *  We do not handle network constraints.
 */
class NLAlgCons {
public:
  /** Constraint name, also acts as identifier. */
  std::string name;

  /** Bound on the algebraic constraint.
   *  Used when producing the unique r of the NL file.
   */
  NLBound range;

  /** Expression graph, used for the non linear part.
   *  Used to produce a new, standalone, C segment.
   *  If the expression graph is empty (linear constraint), produce the expression graph 'n0'
   */
  std::vector<NLToken> expressionGraph = {};

  /** Jacobian, used for the linear part. Identify a variable by its name and associate a
   * coefficent. Used to produce a new, standalone, J segment.
   */
  std::vector<std::pair<std::string, double>> jacobian = {};

  /** Method to build the var_coeff vector.
   *  The NLFile is used to access the variables through their name in order to increase their
   * jacobian count.
   */
  void setJacobian(const std::vector<std::string>& vnames, const std::vector<double>& coeffs,
                   NLFile* nl_file);

  /* *** *** *** Helpers *** *** *** */

  /** A constraint is considered linear if the expressionGraph is empty. */
  bool isLinear() const;

  /* *** *** *** Printable *** *** *** */

  std::ostream& printToStream(std::ostream& o, const NLFile& nl_file) const;
};

/** A logical constraint.
 *  Contains only a non linear part.
 *  We do not handle network constraints.
 *  Logical constraint stands on their own and do not need any identifier.
 *  However, for consistency sake, we still keep their name.
 */
class NLLogicalCons {
public:
  /** Constraint name, also acts as identifier. */
  std::string name;

  /** Index */
  int index = -1;

  /** Expression graph, used for the non linear part.
   *  Used to produce a new, standalone, L segment.
   *  If the expression graph is empty (linear constraint), produce the expression graph 'n0'
   */
  std::vector<NLToken> expressionGraph = {};

  /* *** *** *** Constructor *** *** *** */
  NLLogicalCons(int idx) : index(idx) {}

  /* *** *** *** Printable *** *** *** */

  std::ostream& printToStream(std::ostream& o, const NLFile& nl_file) const;
};

/** The header. */
class NLHeader {
public:
  /* *** *** *** Printable *** *** *** */

  static std::ostream& printToStream(std::ostream& o, const NLFile& nl_file);
};

/** An Objective
 * In an NL file, we can have several of those.
 * However, in flatzinc, only one is allowed, so we only have one.
 * Note that in NL, we do not have a "satisfy" objective, only a minimize or maximize one.
 * We translate the "satisfy" with "minimize n0".
 */
class NLObjective {
public:
  enum MinMax {
    UNDEF = -2,
    SATISFY = -1,
    MINIMIZE = 0,
    MAXIMIZE = 1,
  };

  /* *** *** *** Fields *** *** *** */
  MinMax minmax = UNDEF;
  std::vector<NLToken> expressionGraph = {};  // If empty, produce a 'n0' when printing

  /* *** *** *** Gradient *** *** *** */

  /** Gradient, used for the linear part. Identify a variable by its name and associate a
   * coefficent. Used to produce a new, standalone, G segment.
   */
  std::vector<std::pair<std::string, double>> gradient = {};

  /** Method to build the var_coeff vector. */
  void setGradient(const std::vector<std::string>& vnames, const std::vector<double>& coeffs);

  int gradientCount() const;

  /* *** *** *** Helpers *** *** *** */
  bool isDefined() const;

  bool isLinear() const;

  bool isOptimisation() const;

  /* *** *** *** Constructor *** *** *** */
  NLObjective() = default;

  /* *** *** *** Printable *** *** *** */
  std::ostream& printToStream(std::ostream& o, const NLFile& nl_file) const;
};

}  // namespace MiniZinc
