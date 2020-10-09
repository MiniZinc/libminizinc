/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/hash.hh>
#include <minizinc/solvers/nl/nl_file.hh>

/**
 *  A NL File reprensentation.
 *  The purpose of this file is mainly to be a writer.
 *  Most of the information come from 'nlwrite.pdf'
 *  Note:
 *      * a character '#' starts a comment until the end of line.
 *      * A new line is '\n'
 */

using namespace std;

namespace MiniZinc {

/** *** *** *** Helpers *** *** *** **/

/** Create a string representing the name (and unique identifier) from an identifier. */
string NLFile::getVarName(const Id* id) {
  stringstream os;
  if (id->idn() != -1) {
    os << "X_INTRODUCED_" << id->idn() << "_";
  } else if (id->v().size() != 0) {
    os << id->v();
  }
  string name = os.str();
  return name;
}

/** Create a string representing the name (and unique identifier) of a variable from a variable
 * declaration. */
string NLFile::getVarName(const VarDecl& vd) { return getVarName(vd.id()); }

/** Create a string representing the name (and unique identifier) of a constraint from a specific
 * call expression. */
string NLFile::getConstraintName(const Call& c) {
  stringstream os;
  os << c.id() << "_" << static_cast<const void*>(&c);  // use the memory address as unique ID.
  string name = os.str();
  return name;
}

/** Obtain the vector of an array, either from an identifier or an array litteral */
const ArrayLit& NLFile::getArrayLit(const Expression* e) {
  switch (e->eid()) {
    case Expression::E_ID: {
      return getArrayLit(
          e->cast<Id>()->decl()->e());  // Follow the pointer to the expression of the declaration
    }

    case Expression::E_ARRAYLIT: {
      const ArrayLit& al = *e->cast<ArrayLit>();
      return al;
    }

    default:
      should_not_happen("Could not read array from expression.");
  }
}

/** Create a vector of double from a vector containing Expression being IntLit. */
vector<double> NLFile::fromVecInt(const ArrayLit& v_int) {
  vector<double> v = {};
  for (unsigned int i = 0; i < v_int.size(); ++i) {
    double d = static_cast<double>(v_int[i]->cast<IntLit>()->v().toInt());
    v.push_back(d);
  }
  return v;
}

/** Create a vector of double from a vector containing Expression being FloatLit. */
vector<double> NLFile::fromVecFloat(const ArrayLit& v_fp) {
  vector<double> v = {};
  for (unsigned int i = 0; i < v_fp.size(); ++i) {
    double d = v_fp[i]->cast<FloatLit>()->v().toDouble();
    v.push_back(d);
  }
  return v;
}

/** Create a vector of variable names from a vector containing Expression being identifier Id. */
vector<string> NLFile::fromVecId(const ArrayLit& v_id) {
  vector<string> v = {};
  for (unsigned int i = 0; i < v_id.size(); ++i) {
    string s = getVarName(*(v_id[i]->cast<Id>()->decl()));
    v.push_back(s);
  }
  return v;
}

/** *** *** *** Phase 1: collecting data from MZN *** *** *** **/

/** Add a variable declaration in the NL File.
 *  This function pre-analyse the declaration VarDecl, then delegate to addVarDeclInteger or
 * addVarDeclFloat. In flatzinc, arrays always have a rhs: the can always be replaced by their
 * definition (following the pointer starting at the ID) Hence, we do not reproduce arrays in the NL
 * file.
 */
void NLFile::addVarDecl(const VarDecl& vd, const TypeInst& ti, const Expression& rhs) {
  // Get the name
  string name = getVarName(vd);
  // Discriminate according to the type:
  if (ti.isEnum()) {
    should_not_happen("Enum type in the flatzinc");
  } else if (ti.isarray()) {
    DEBUG_MSG("     Definition of array " << name << " is not reproduced in nl.");

    // Look for the annotation "output_array"
    for (ExpressionSetIter it = vd.ann().begin(); it != vd.ann().end(); ++it) {
      Call* c = (*it)->dynamicCast<Call>();
      if (c != nullptr && c->id() == (constants().ann.output_array)) {
        NLArray array;
        array.name = name;
        array.isInteger = ti.type().bt() == Type::BT_INT;

        // Search the 'annotation' array
        const ArrayLit& aa = getArrayLit(c->arg(0));
        for (int i = 0; i < aa.size(); ++i) {
          IntSetVal* r = aa[i]->cast<SetLit>()->isv();
          stringstream ss;
          ss << r->min().toInt() << ".." << r->max().toInt();
          array.dimensions.push_back(ss.str());
        }

        // Search the 'real' array. Items can be an identifier or a litteral.
        const ArrayLit& ra = getArrayLit(&rhs);
        for (int i = 0; i < ra.size(); ++i) {
          NLArray::Item item;

          if (ra[i]->isa<Id>()) {
            item.variable = getVarName(ra[i]->cast<Id>());
          } else if (ra[i]->isa<IntLit>()) {
            assert(array.isInteger);
            item.value = static_cast<double>(ra[i]->cast<IntLit>()->v().toInt());
          } else {
            assert(!array.isInteger);  // Floating point
            item.value = ra[i]->cast<FloatLit>()->v().toDouble();
          }

          array.items.push_back(item);
        }

        outputArrays.push_back(array);

        break;
      }
    }
  } else {
    // Check if the variable needs to be reported
    bool toReport = vd.ann().contains(constants().ann.output_var);
    DEBUG_MSG("     '" << name << "' to be reported? " << toReport);

    // variable declaration
    const Type& type = ti.type();
    const Expression* domain = ti.domain();

    // Check the type: integer or floatin point
    assert(type.isvarint() || type.isvarfloat());
    bool isvarint = type.isvarint();

    // Call the declaration function according to the type
    // Check the domain and convert if not null.
    // Note: we directly jump to the specialized Int/Float set, going through the set literal
    if (isvarint) {
      // Integer
      IntSetVal* isv = nullptr;
      if (domain != nullptr) {
        isv = domain->cast<SetLit>()->isv();
      }
      addVarDeclInteger(name, isv, toReport);
    } else {
      // Floating point
      FloatSetVal* fsv = nullptr;
      if (domain != nullptr) {
        fsv = domain->cast<SetLit>()->fsv();
      }
      addVarDeclFloat(name, fsv, toReport);
    }
  }
}

/** Add an integer variable declaration to the NL File. */
void NLFile::addVarDeclInteger(const string& name, const IntSetVal* isv, bool toReport) {
  // Check that we do not have naming conflict
  assert(variables.find(name) == variables.end());

  // Check the domain.
  NLBound bound;
  if (isv == nullptr) {
    bound = NLBound::makeNoBound();
  } else if (isv->size() == 1) {
    double lb = static_cast<double>(isv->min(0).toInt());
    double ub = static_cast<double>(isv->max(0).toInt());
    bound = NLBound::makeBounded(lb, ub);
  } else {
    should_not_happen("Range: switch on mzn_opt_only_range_domains" << endl);
  }
  // Create the variable and update the NLFile
  NLVar v = NLVar(name, true, toReport, bound);
  variables[name] = v;
}

/** Add a floating point variable declaration to the NL File. */
void NLFile::addVarDeclFloat(const string& name, const FloatSetVal* fsv, bool toReport) {
  // Check that we do not have naming conflict
  assert(variables.find(name) == variables.end());
  // Check the domain.
  NLBound bound;
  if (fsv == nullptr) {
    bound = NLBound::makeNoBound();
  } else if (fsv->size() == 1) {
    double lb = fsv->min(0).toDouble();
    double ub = fsv->max(0).toDouble();
    bound = NLBound::makeBounded(lb, ub);
  } else {
    should_not_happen("Range: switch on mzn_opt_only_range_domains" << std::endl);
  }
  // Create the variable and update the NLFile
  NLVar v = NLVar(name, false, toReport, bound);
  variables[name] = v;
}

// --- --- --- Constraints analysis

/** Dispatcher for constraint analysis. */
void NLFile::analyseConstraint(const Call& c) {
  // ID of the call
  auto id = c.id();
  // Constants for integer builtins
  auto consint = constants().ids.int_;
  // Constants for floating point builtins
  auto consfp = constants().ids.float_;

  // Integer linear predicates
  if (id == consint.lin_eq) {
    consint_lin_eq(c);
  } else if (id == consint.lin_le) {
    consint_lin_le(c);
  } else if (id == consint.lin_ne) {
    consint_lin_ne(c);
  }

  // Integer predicates
  else if (id == consint.le) {
    consint_le(c);
  } else if (id == consint.eq) {
    consint_eq(c);
  } else if (id == consint.ne) {
    consint_ne(c);
  }

  // Integer binary operators
  else if (id == consint.times) {
    consint_times(c);
  } else if (id == consint.div) {
    consint_div(c);
  } else if (id == consint.mod) {
    consint_mod(c);
  } else if (id == consint.plus) {
    consint_plus(c);
  } else if (id == "int_pow") {
    int_pow(c);
  } else if (id == "int_max") {
    int_max(c);
  } else if (id == "int_min") {
    int_min(c);
  }

  // Integer unary operators
  else if (id == "int_abs") {
    int_abs(c);
  }

  // Floating point linear predicates
  else if (id == consfp.lin_eq) {
    consfp_lin_eq(c);
  } else if (id == consfp.lin_le) {
    consfp_lin_le(c);
  } else if (id == consfp.lin_lt) {
    consfp_lin_lt(c);
  } else if (id == consfp.lin_ne) {
    consfp_lin_ne(c);
  }

  // Floating point predicates
  else if (id == consfp.lt) {
    consfp_lt(c);
  } else if (id == consfp.le) {
    consfp_le(c);
  } else if (id == consfp.eq) {
    consfp_eq(c);
  } else if (id == consfp.ne) {
    consfp_ne(c);
  }

  // Floating point binary operators
  else if (id == consfp.plus) {
    consfp_plus(c);
  } else if (id == consfp.minus) {
    consfp_minus(c);
  } else if (id == consfp.div) {
    consfp_div(c);
  } else if (id == consfp.times) {
    consfp_times(c);
  } else if (id == "float_pow") {
    float_pow(c);
  } else if (id == "float_max") {
    float_max(c);
  } else if (id == "float_min") {
    float_min(c);
  }

  // Floating point unary operators
  else if (id == "float_abs") {
    float_abs(c);
  } else if (id == "float_acos") {
    float_acos(c);
  } else if (id == "float_acosh") {
    float_acosh(c);
  } else if (id == "float_asin") {
    float_asin(c);
  } else if (id == "float_asinh") {
    float_asinh(c);
  } else if (id == "float_atan") {
    float_atan(c);
  } else if (id == "float_atanh") {
    float_atanh(c);
  } else if (id == "float_cos") {
    float_cos(c);
  } else if (id == "float_cosh") {
    float_cosh(c);
  } else if (id == "float_exp") {
    float_exp(c);
  } else if (id == "float_ln") {
    float_ln(c);
  } else if (id == "float_log10") {
    float_log10(c);
  } else if (id == "float_log2") {
    float_log2(c);
  } else if (id == "float_sqrt") {
    float_sqrt(c);
  } else if (id == "float_sin") {
    float_sin(c);
  } else if (id == "float_sinh") {
    float_sinh(c);
  } else if (id == "float_tan") {
    float_tan(c);
  } else if (id == "float_tanh") {
    float_tanh(c);
  }

  // Other
  else if (id == "int2float") {
    int2float(c);
  }

  // Domain
  else if (id == consfp.in) {
    should_not_happen("Ignore for now: constraint 'float in    ' not implemented");
  } else if (id == consfp.dom) {
    should_not_happen("Ignore for now: constraint 'float dom   ' not implemented");
  }

  // Grey area
  else if (id == consint.lt) {
    should_not_happen("'int lt'");
  } else if (id == consint.gt) {
    should_not_happen("'int gt'");
  } else if (id == consint.ge) {
    should_not_happen("'int ge'");
  } else if (id == consint.minus) {
    should_not_happen("'int minus'");
  } else if (id == consfp.gt) {
    should_not_happen("float gt'");
  } else if (id == consfp.ge) {
    should_not_happen("float ge'");
  }

  // Not implemented
  else {
    should_not_happen("Builtins " << c.id() << " not implemented or not recognized.");
  }
}

// --- --- --- Helpers

/** Create a token from an expression representing a variable */
NLToken NLFile::getTokenFromVarOrInt(const Expression* e) {
  if (e->type().isPar()) {
    // Constant
    double value = static_cast<double>(e->cast<IntLit>()->v().toInt());
    return NLToken::n(value);
  }  // Variable
  VarDecl& vd = *(e->cast<Id>()->decl());
  string n = getVarName(vd);
  return NLToken::v(n);
}

/** Create a token from an expression representing either a variable or a floating point numeric
 * value. */
NLToken NLFile::getTokenFromVarOrFloat(const Expression* e) {
  if (e->type().isPar()) {
    // Constant
    double value = e->cast<FloatLit>()->v().toDouble();
    return NLToken::n(value);
  }  // Variable
  VarDecl& vd = *(e->cast<Id>()->decl());
  string n = getVarName(vd);
  return NLToken::v(n);
}

/** Create a token from an expression representing either a variable. */
NLToken NLFile::getTokenFromVar(const Expression* e) {
  assert(!e->type().isPar());
  // Variable
  VarDecl& vd = *(e->cast<Id>()->decl());
  string n = getVarName(vd);
  return NLToken::v(n);
}

/** Update an expression graph (only by appending token) with a linear combination
 *  of coefficients and variables. Count as "non linear" for the variables occuring here.
 */
void NLFile::makeSigmaMult(vector<NLToken>& expressionGraph, const vector<double>& coeffs,
                           const vector<string>& vars) {
  assert(coeffs.size() == vars.size());
  assert(coeffs.size() >= 2);

  // Create a sum of products.
  // WARNING: OPSUMLIST needs at least 3 operands! We need a special case for the sum of 2.
  if (coeffs.size() == 2) {
    expressionGraph.push_back(NLToken::o(NLToken::OpCode::OPPLUS));
  } else {
    expressionGraph.push_back(NLToken::mo(NLToken::MOpCode::OPSUMLIST, coeffs.size()));
  }

  // Component of the sum. Simplify multiplication by one.
  for (unsigned int i = 0; i < coeffs.size(); ++i) {
    // Product if coeff !=1
    if (coeffs[i] != 1) {
      expressionGraph.push_back(NLToken::o(NLToken::OpCode::OPMULT));
      expressionGraph.push_back(NLToken::n(coeffs[i]));
    }
    // Set the variable as "non linear"
    expressionGraph.push_back(NLToken::v(vars[i]));
  }
}

// --- --- --- Linear Builders

/** Create a linear constraint [coeffs] *+ [vars] = value. */
void NLFile::linconsEq(const Call& c, const vector<double>& coeffs, const vector<string>& vars,
                       const NLToken& value) {
  // Create the Algebraic Constraint and set the data
  NLAlgCons cons;

  // Get the name of the constraint
  string cname = getConstraintName(c);
  cons.name = cname;

  if (value.isConstant()) {
    // Create the bound of the constraint
    NLBound bound = NLBound::makeEqual(value.numericValue);
    cons.range = bound;
    // No non linear part: leave the expression graph empty.
    // Linear part: set the jacobian
    cons.setJacobian(vars, coeffs, this);
  } else {
    // Create the bound of the constraint = 0 and change the Jacobian to incorporate a -1 on the
    // variable in 'value'
    NLBound bound = NLBound::makeEqual(0);
    cons.range = bound;
    // No non linear part: leave the expression graph empty.
    // Linear part: set the jacobian
    vector<double> coeffs_(coeffs);
    coeffs_.push_back(-1);
    vector<string> vars_(vars);
    vars_.push_back(value.str);
    cons.setJacobian(vars_, coeffs_, this);
  }

  // Add the constraint in our mapping
  constraints[cname] = cons;
}

/** Create a linear constraint [coeffs] *+ [vars] <= value. */
void NLFile::linconsLe(const Call& c, const vector<double>& coeffs, const vector<string>& vars,
                       const NLToken& value) {
  // Create the Algebraic Constraint and set the data
  NLAlgCons cons;

  // Get the name of the constraint
  string cname = getConstraintName(c);
  cons.name = cname;

  if (value.isConstant()) {
    // Create the bound of the constraint
    NLBound bound = NLBound::makeUBBounded(value.numericValue);
    cons.range = bound;
    // No non linear part: leave the expression graph empty.
    // Linear part: set the jacobian
    cons.setJacobian(vars, coeffs, this);
  } else {
    // Create the bound of the constraint = 0 and change the Jacobian to incorporate a -1 on the
    // variable in 'value'
    NLBound bound = NLBound::makeUBBounded(0);
    cons.range = bound;
    // No non linear part: leave the expression graph empty.
    // Linear part: set the jacobian
    vector<double> coeffs_(coeffs);
    coeffs_.push_back(-1);
    vector<string> vars_(vars);
    vars_.push_back(value.str);
    cons.setJacobian(vars_, coeffs_, this);
  }

  // Add the constraint in our mapping
  constraints[cname] = cons;
}

/** Create a linear logical constraint [coeffs] *+ [vars] PREDICATE value.
 *  Use a generic comparison operator.
 *  Warnings:   - Creates a logical constraint
 *              - Only use for conmparisons that cannot be expressed with '=' xor '<='.
 */
void NLFile::linconsPredicate(const Call& c, NLToken::OpCode oc, const vector<double>& coeffs,
                              const vector<string>& vars, const NLToken& value) {
  // Create the Logical Constraint and set the data
  NLLogicalCons cons(logicalConstraints.size());

  // Get the name of the constraint
  string cname = getConstraintName(c);
  cons.name = cname;

  // Create the expression graph (Note: no Jacobian associated with a logical constraint).
  // 1) Push the comparison operator, e.g. "!= operand1 operand2"
  cons.expressionGraph.push_back(NLToken::o(oc));

  // 2) Operand1 := sum of product
  makeSigmaMult(cons.expressionGraph, coeffs, vars);

  // 3) Operand 2 := value.
  cons.expressionGraph.push_back(value);

  // Store the constraint
  logicalConstraints.push_back(cons);
}

// --- --- --- Non Linear Builders
// For predicates, uses 2 variables or literals: x := c.arg(0), y := c.arg(1)
// x PREDICATE y

// For operations, uses 3 variables or literals: x := c.arg(0), y := c.arg(1), and z := c.arg(2).
// x OPERATOR y = z

/** Create a non linear constraint x = y
 *  Use the jacobian and the bound on constraint to translate into x - y = 0
 *  Simply update the bound if one is a constant.
 */
void NLFile::nlconsEq(const Call& c, const NLToken& x, const NLToken& y) {
  if (x.kind != y.kind) {
    if (x.isConstant()) {
      // Update bound on y
      double value = x.numericValue;
      NLVar& v = variables.at(y.str);
      v.bound.updateEq(value);
    } else {
      // Update bound on x
      double value = y.numericValue;
      NLVar& v = variables.at(x.str);
      v.bound.updateEq(value);
    }
  } else if (x.str != y.str) {  // both must be variables anyway.
    assert(x.isVariable() && y.isVariable());
    // Create the Algebraic Constraint and set the data
    NLAlgCons cons;

    // Get the name of the constraint
    string cname = getConstraintName(c);
    cons.name = cname;

    // Create the bound of the constraint: equal 0
    NLBound bound = NLBound::makeEqual(0);
    cons.range = bound;

    // Create the jacobian
    vector<double> coeffs = {1, -1};
    vector<string> vars = {x.str, y.str};
    cons.setJacobian(vars, coeffs, this);

    // Store the constraint
    constraints[cname] = cons;
  }
}

/** Create a non linear constraint x <= y
 *  Use the jacobian and the bound on constraint to translate into x - y <= 0
 *  Simply update the bound if one is a constant.
 */
void NLFile::nlconsLe(const Call& c, const NLToken& x, const NLToken& y) {
  if (x.kind != y.kind) {
    if (x.isConstant()) {
      // Update lower bound on y
      double value = x.numericValue;
      NLVar& v = variables.at(y.str);
      v.bound.updateLB(value);
    } else {
      // Update upper bound on x
      double value = y.numericValue;
      NLVar& v = variables.at(x.str);
      v.bound.updateUB(value);
    }
  } else if (x.str != y.str) {  // both must be variables anyway.
    assert(x.isVariable() && y.isVariable());

    // Create the Algebraic Constraint and set the data
    NLAlgCons cons;

    // Get the name of the constraint
    string cname = getConstraintName(c);
    cons.name = cname;

    // Create the bound of the constraint: <= 0
    NLBound bound = NLBound::makeUBBounded(0);
    cons.range = bound;

    // Create the jacobian
    vector<double> coeffs = {1, -1};
    vector<string> vars = {x.str, y.str};
    cons.setJacobian(vars, coeffs, this);

    // Store the constraint
    constraints[cname] = cons;
  }
}

/** Create a non linear constraint with a predicate: x PREDICATE y
 *  Use a generic comparison operator.
 *  Warnings:   - Creates a logical constraint
 *              - Only use for conmparisons that cannot be expressed with '=' xor '<='.
 */
void NLFile::nlconsPredicate(const Call& c, NLToken::OpCode oc, const NLToken& x,
                             const NLToken& y) {
  // Create the Logical Constraint and set the data
  NLLogicalCons cons(logicalConstraints.size());

  // Get the name of the constraint
  string cname = getConstraintName(c);
  cons.name = cname;

  // Create the expression graph
  cons.expressionGraph.push_back(NLToken::o(oc));
  cons.expressionGraph.push_back(x);
  cons.expressionGraph.push_back(y);

  // Store the constraint
  logicalConstraints.push_back(cons);
}

/** Create a non linear constraint with a binary operator: x OPERATOR y = z */
void NLFile::nlconsOperatorBinary(const Call& c, NLToken::OpCode oc, const NLToken& x,
                                  const NLToken& y, const NLToken& z) {
  // Create the Algebraic Constraint and set the data
  NLAlgCons cons;

  // Get the name of the constraint
  string cname = getConstraintName(c);
  cons.name = cname;

  // If z is a constant, use the bound, else use the jacobian
  if (z.isConstant()) {
    // Create the bound of the constraint with the numeric value of z
    NLBound bound = NLBound::makeEqual(z.numericValue);
    cons.range = bound;
  } else {
    // Else, use a constraint bound = 0 and use the jacobian to substract z from the result.
    // Create the bound of the constraint to 0
    NLBound bound = NLBound::makeEqual(0);
    cons.range = bound;

    vector<double> coeffs = {};
    vector<string> vars = {};

    // If x is a variable different from y (and must be different from z), give it 0 for the linear
    // part
    if (x.isVariable() && x.str != y.str) {
      assert(x.str != z.str);
      coeffs.push_back(0);
      vars.push_back(x.str);
    }
    // Same as above for y.
    if (y.isVariable()) {
      assert(y.str != z.str);
      coeffs.push_back(0);
      vars.push_back(y.str);
    }
    // z is a variable whose value is substracted from the result
    coeffs.push_back(-1);
    vars.push_back(z.str);

    // Finish jacobian
    cons.setJacobian(vars, coeffs, this);
  }

  // Create the expression graph using the operand code 'oc'
  cons.expressionGraph.push_back(NLToken::o(oc));
  cons.expressionGraph.push_back(x);
  cons.expressionGraph.push_back(y);

  // Store the constraint
  constraints[cname] = cons;
}

/** Create a non linear constraint with a binary operator: x OPERATOR y = z.
 *  OPERATOR is now a Multiop, with a count of 2 (so the choice of the method to use depends on the
 * LN implementation) */
void NLFile::nlconsOperatorBinary(const Call& c, NLToken::MOpCode moc, const NLToken& x,
                                  const NLToken& y, const NLToken& z) {
  // Create the Algebraic Constraint and set the data
  NLAlgCons cons;

  // Get the name of the constraint
  string cname = getConstraintName(c);
  cons.name = cname;

  // If z is a constant, use the bound, else use the jacobian
  if (z.isConstant()) {
    // Create the bound of the constraint with the numeric value of z
    NLBound bound = NLBound::makeEqual(z.numericValue);
    cons.range = bound;
  } else {
    // Else, use a constraint bound = 0 and use the jacobian to substract vres from the result.
    // Create the bound of the constraint to 0
    NLBound bound = NLBound::makeEqual(0);
    cons.range = bound;

    vector<double> coeffs = {};
    vector<string> vars = {};

    // If x is a variable different from y (and must be different from z), give it 0 for the linear
    // part
    if (x.isVariable() && x.str != y.str) {
      assert(x.str != z.str);
      coeffs.push_back(0);
      vars.push_back(x.str);
    }
    // Same as above for y.
    if (y.isVariable()) {
      assert(y.str != z.str);
      coeffs.push_back(0);
      vars.push_back(y.str);
    }
    // z is a variable whose value is substracted from the result
    coeffs.push_back(-1);
    vars.push_back(z.str);

    // Finish jacobian
    cons.setJacobian(vars, coeffs, this);
  }

  // Create the expression graph using the operand code 'oc'
  cons.expressionGraph.push_back(NLToken::mo(moc, 2));
  cons.expressionGraph.push_back(x);
  cons.expressionGraph.push_back(y);

  // Store the constraint
  constraints[cname] = cons;
}

/** Create a non linear constraint with an unary operator: OPERATOR x = y */
void NLFile::nlconsOperatorUnary(const Call& c, NLToken::OpCode oc, const NLToken& x,
                                 const NLToken& y) {
  // Create the Algebraic Constraint and set the data
  NLAlgCons cons;

  // Get the name of the constraint
  string cname = getConstraintName(c);
  cons.name = cname;

  // If y is a constant, use the bound, else use the jacobian
  if (y.isConstant()) {
    // Create the bound of the constraint with the numeric value of y
    NLBound bound = NLBound::makeEqual(y.numericValue);
    cons.range = bound;
  } else {
    // Else, use a constraint bound = 0 and use the jacobian to substract y from the result.
    // Create the bound of the constraint to 0
    NLBound bound = NLBound::makeEqual(0);
    cons.range = bound;

    vector<double> coeffs = {};
    vector<string> vars = {};

    // If x is a variable (must be different from y), give it '0' for the linear part
    if (x.isVariable()) {
      assert(x.str != y.str);
      coeffs.push_back(0);
      vars.push_back(x.str);
    }

    // z is a variable whose value is substracted from the result
    coeffs.push_back(-1);
    vars.push_back(y.str);

    // Finish jacobian
    cons.setJacobian(vars, coeffs, this);
  }

  // Create the expression graph using the operand code 'oc'
  cons.expressionGraph.push_back(NLToken::o(oc));
  cons.expressionGraph.push_back(x);

  // Store the constraint
  constraints[cname] = cons;
}

/** Create a non linear constraint, specialized for log2 unary operator: Log2(x) = y */
void NLFile::nlconsOperatorUnaryLog2(const Call& c, const NLToken& x, const NLToken& y) {
  // Create the Algebraic Constraint and set the data
  NLAlgCons cons;

  // Get the name of the constraint
  string cname = getConstraintName(c);
  cons.name = cname;

  // If y is a constant, use the bound, else use the jacobian
  if (y.isConstant()) {
    // Create the bound of the constraint with the numeric value of y
    NLBound bound = NLBound::makeEqual(y.numericValue);
    cons.range = bound;
  } else {
    // Else, use a constraint bound = 0 and use the jacobian to substract vres from the result.
    // Create the bound of the constraint to 0
    NLBound bound = NLBound::makeEqual(0);
    cons.range = bound;

    vector<double> coeffs = {};
    vector<string> vars = {};

    // If x is a variable (must be different from y), give it '0' for the linear part
    if (x.isVariable()) {
      assert(x.str != y.str);
      coeffs.push_back(0);
      vars.push_back(x.str);
    }
    // z is a variable whose value is substracted from the result
    coeffs.push_back(-1);
    vars.push_back(y.str);

    // Finish jacobian
    cons.setJacobian(vars, coeffs, this);
  }

  // Create the expression graph with log2(x) = ln(x)/ln(2)
  cons.expressionGraph.push_back(NLToken::o(NLToken::OpCode::OPDIV));
  cons.expressionGraph.push_back(NLToken::o(NLToken::OpCode::OP_log));
  cons.expressionGraph.push_back(x);
  cons.expressionGraph.push_back(NLToken::o(NLToken::OpCode::OP_log));
  cons.expressionGraph.push_back(NLToken::n(2));

  // Store the constraint
  constraints[cname] = cons;
}

// --- --- --- Integer Linear Constraints

/** Linar constraint: [coeffs] *+ [vars] = value */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::consint_lin_eq(const Call& c) {
  // Get the arguments arg0 (array0 = coeffs), arg1 (array = variables) and arg2 (value)
  vector<double> coeffs = fromVecInt(getArrayLit(c.arg(0)));
  vector<string> vars = fromVecId(getArrayLit(c.arg(1)));
  NLToken value = getTokenFromVarOrInt(c.arg(2));
  // Create the constraint
  linconsEq(c, coeffs, vars, value);
}

/** Linar constraint: [coeffs] *+ [vars] =< value */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::consint_lin_le(const Call& c) {
  // Get the arguments arg0 (array0 = coeffs), arg1 (array = variables) and arg2 (value)
  vector<double> coeffs = fromVecInt(getArrayLit(c.arg(0)));
  vector<string> vars = fromVecId(getArrayLit(c.arg(1)));
  NLToken value = getTokenFromVarOrInt(c.arg(2));
  // Create the constraint
  linconsLe(c, coeffs, vars, value);
}

/** Linar constraint: [coeffs] *+ [vars] != value */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::consint_lin_ne(const Call& c) {
  // Get the arguments arg0 (array0 = coeffs), arg1 (array = variables) and arg2 (value)
  vector<double> coeffs = fromVecInt(getArrayLit(c.arg(0)));
  vector<string> vars = fromVecId(getArrayLit(c.arg(1)));
  NLToken value = getTokenFromVarOrInt(c.arg(2));
  // Create the constraint
  linconsPredicate(c, NLToken::OpCode::NE, coeffs, vars, value);
}

// --- --- --- Integer Non Linear Predicate Constraints

/** Non linear constraint x = y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::consint_eq(const Call& c) {
  nlconsEq(c, getTokenFromVarOrInt(c.arg(0)), getTokenFromVarOrInt(c.arg(1)));
}

/** Non linear constraint x <= y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::consint_le(const Call& c) {
  nlconsLe(c, getTokenFromVarOrInt(c.arg(0)), getTokenFromVarOrInt(c.arg(1)));
}

/** Non linear constraint x != y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::consint_ne(const Call& c) {
  nlconsPredicate(c, NLToken::OpCode::NE, getTokenFromVarOrInt(c.arg(0)),
                  getTokenFromVarOrInt(c.arg(1)));
}

// --- --- --- Integer Non Linear Binary Operator Constraints

/** Non linear constraint x + y = z */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::consint_plus(const Call& c) {
  nlconsOperatorBinary(c, NLToken::OpCode::OPPLUS, getTokenFromVarOrInt(c.arg(0)),
                       getTokenFromVarOrInt(c.arg(1)), getTokenFromVarOrInt(c.arg(2)));
}

/** Non linear constraint x * y = z */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::consint_times(const Call& c) {
  nlconsOperatorBinary(c, NLToken::OpCode::OPMULT, getTokenFromVarOrInt(c.arg(0)),
                       getTokenFromVarOrInt(c.arg(1)), getTokenFromVarOrInt(c.arg(2)));
}

/** Non linear constraint x / y = z */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::consint_div(const Call& c) {
  nlconsOperatorBinary(c, NLToken::OpCode::OPDIV, getTokenFromVarOrInt(c.arg(0)),
                       getTokenFromVarOrInt(c.arg(1)), getTokenFromVarOrInt(c.arg(2)));
}

/** Non linear constraint x mod y = z */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::consint_mod(const Call& c) {
  nlconsOperatorBinary(c, NLToken::OpCode::OPREM, getTokenFromVarOrInt(c.arg(0)),
                       getTokenFromVarOrInt(c.arg(1)), getTokenFromVarOrInt(c.arg(2)));
}

/** Non linear constraint x pow y = z */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::int_pow(const Call& c) {
  nlconsOperatorBinary(c, NLToken::OpCode::OPPOW, getTokenFromVarOrInt(c.arg(0)),
                       getTokenFromVarOrInt(c.arg(1)), getTokenFromVarOrInt(c.arg(2)));
}

/** Non linear constraint max(x, y) = z */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::int_max(const Call& c) {
  nlconsOperatorBinary(c, NLToken::MOpCode::MAXLIST, getTokenFromVarOrInt(c.arg(0)),
                       getTokenFromVarOrInt(c.arg(1)), getTokenFromVarOrInt(c.arg(2)));
}

/** Non linear constraint min(x, y) = z */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::int_min(const Call& c) {
  nlconsOperatorBinary(c, NLToken::MOpCode::MINLIST, getTokenFromVarOrInt(c.arg(0)),
                       getTokenFromVarOrInt(c.arg(1)), getTokenFromVarOrInt(c.arg(2)));
}

// --- --- --- Integer Non Linear Unary Operator Constraints

/** Non linear constraint abs x = y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::int_abs(const Call& c) {
  nlconsOperatorUnary(c, NLToken::OpCode::ABS, getTokenFromVarOrInt(c.arg(0)),
                      getTokenFromVarOrInt(c.arg(1)));
}

// --- --- --- Floating Point Linear Constraints

/** Linar constraint: [coeffs] *+ [vars] = value */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::consfp_lin_eq(const Call& c) {
  // Get the arguments arg0 (array0 = coeffs), arg1 (array = variables) and arg2 (value)
  vector<double> coeffs = fromVecFloat(getArrayLit(c.arg(0)));
  vector<string> vars = fromVecId(getArrayLit(c.arg(1)));
  NLToken value = getTokenFromVarOrFloat(c.arg(2));
  // Create the constraint
  linconsEq(c, coeffs, vars, value);
}

/** Linar constraint: [coeffs] *+ [vars] = value */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::consfp_lin_le(const Call& c) {
  // Get the arguments arg0 (array0 = coeffs), arg1 (array = variables) and arg2 (value)
  vector<double> coeffs = fromVecFloat(getArrayLit(c.arg(0)));
  vector<string> vars = fromVecId(getArrayLit(c.arg(1)));
  NLToken value = getTokenFromVarOrFloat(c.arg(2));
  // Create the constraint
  linconsLe(c, coeffs, vars, value);
}

/** Linar constraint: [coeffs] *+ [vars] != value */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::consfp_lin_ne(const Call& c) {
  // Get the arguments arg0 (array0 = coeffs), arg1 (array = variables) and arg2 (value)
  vector<double> coeffs = fromVecFloat(getArrayLit(c.arg(0)));
  vector<string> vars = fromVecId(getArrayLit(c.arg(1)));
  NLToken value = getTokenFromVarOrFloat(c.arg(2));
  // Create the constraint
  linconsPredicate(c, NLToken::OpCode::NE, coeffs, vars, value);
}

/** Linar constraint: [coeffs] *+ [vars] < value */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::consfp_lin_lt(const Call& c) {
  // Get the arguments arg0 (array0 = coeffs), arg1 (array = variables) and arg2 (value)
  vector<double> coeffs = fromVecFloat(getArrayLit(c.arg(0)));
  vector<string> vars = fromVecId(getArrayLit(c.arg(1)));
  NLToken value = getTokenFromVarOrFloat(c.arg(2));
  // Create the constraint
  linconsPredicate(c, NLToken::OpCode::LT, coeffs, vars, value);
}

// --- --- --- Floating Point Non Linear Predicate Constraints

/** Non linear constraint x = y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::consfp_eq(const Call& c) {
  nlconsEq(c, getTokenFromVarOrFloat(c.arg(0)), getTokenFromVarOrFloat(c.arg(1)));
}

/** Non linear constraint x <= y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::consfp_le(const Call& c) {
  nlconsLe(c, getTokenFromVarOrFloat(c.arg(0)), getTokenFromVarOrFloat(c.arg(1)));
}

/** Non linear constraint x != y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::consfp_ne(const Call& c) {
  nlconsPredicate(c, NLToken::OpCode::NE, getTokenFromVarOrFloat(c.arg(0)),
                  getTokenFromVarOrFloat(c.arg(1)));
}

/** Non linear constraint x < y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::consfp_lt(const Call& c) {
  nlconsPredicate(c, NLToken::OpCode::LT, getTokenFromVarOrFloat(c.arg(0)),
                  getTokenFromVarOrFloat(c.arg(1)));
}

// --- --- --- Floating Point Non Linear Binary Operator Constraints

/** Non linear constraint x + y = z */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::consfp_plus(const Call& c) {
  nlconsOperatorBinary(c, NLToken::OpCode::OPPLUS, getTokenFromVarOrFloat(c.arg(0)),
                       getTokenFromVarOrFloat(c.arg(1)), getTokenFromVarOrFloat(c.arg(2)));
}

/** Non linear constraint x - y = z */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::consfp_minus(const Call& c) {
  nlconsOperatorBinary(c, NLToken::OpCode::OPMINUS, getTokenFromVarOrFloat(c.arg(0)),
                       getTokenFromVarOrFloat(c.arg(1)), getTokenFromVarOrFloat(c.arg(2)));
}

/** Non linear constraint x * y = z */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::consfp_times(const Call& c) {
  nlconsOperatorBinary(c, NLToken::OpCode::OPMULT, getTokenFromVarOrFloat(c.arg(0)),
                       getTokenFromVarOrFloat(c.arg(1)), getTokenFromVarOrFloat(c.arg(2)));
}

/** Non linear constraint x / y = z */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::consfp_div(const Call& c) {
  nlconsOperatorBinary(c, NLToken::OpCode::OPDIV, getTokenFromVarOrFloat(c.arg(0)),
                       getTokenFromVarOrFloat(c.arg(1)), getTokenFromVarOrFloat(c.arg(2)));
}

/** Non linear constraint x mod y = z */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::consfp_mod(const Call& c) {
  nlconsOperatorBinary(c, NLToken::OpCode::OPREM, getTokenFromVarOrFloat(c.arg(0)),
                       getTokenFromVarOrFloat(c.arg(1)), getTokenFromVarOrFloat(c.arg(2)));
}

/** Non linear constraint x pow y = z */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::float_pow(const Call& c) {
  nlconsOperatorBinary(c, NLToken::OpCode::OPPOW, getTokenFromVarOrFloat(c.arg(0)),
                       getTokenFromVarOrFloat(c.arg(1)), getTokenFromVarOrFloat(c.arg(2)));
}

/** Non linear constraint max(x, y) = z */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::float_max(const Call& c) {
  nlconsOperatorBinary(c, NLToken::MOpCode::MAXLIST, getTokenFromVarOrFloat(c.arg(0)),
                       getTokenFromVarOrFloat(c.arg(1)), getTokenFromVarOrFloat(c.arg(2)));
}

/** Non linear constraint min(x, y) = z */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::float_min(const Call& c) {
  nlconsOperatorBinary(c, NLToken::MOpCode::MINLIST, getTokenFromVarOrFloat(c.arg(0)),
                       getTokenFromVarOrFloat(c.arg(1)), getTokenFromVarOrFloat(c.arg(2)));
}

// --- --- --- Floating Point Non Linear Unary operator Constraints

/** Non linear constraint abs x = y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::float_abs(const Call& c) {
  nlconsOperatorUnary(c, NLToken::OpCode::ABS, getTokenFromVarOrFloat(c.arg(0)),
                      getTokenFromVarOrFloat(c.arg(1)));
}

/** Non linear constraint acos x = y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::float_acos(const Call& c) {
  nlconsOperatorUnary(c, NLToken::OpCode::OP_acos, getTokenFromVarOrFloat(c.arg(0)),
                      getTokenFromVarOrFloat(c.arg(1)));
}

/** Non linear constraint acosh x = y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::float_acosh(const Call& c) {
  nlconsOperatorUnary(c, NLToken::OpCode::OP_acosh, getTokenFromVarOrFloat(c.arg(0)),
                      getTokenFromVarOrFloat(c.arg(1)));
}

/** Non linear constraint asin x = y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::float_asin(const Call& c) {
  nlconsOperatorUnary(c, NLToken::OpCode::OP_asin, getTokenFromVarOrFloat(c.arg(0)),
                      getTokenFromVarOrFloat(c.arg(1)));
}

/** Non linear constraint asinh x = y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::float_asinh(const Call& c) {
  nlconsOperatorUnary(c, NLToken::OpCode::OP_asinh, getTokenFromVarOrFloat(c.arg(0)),
                      getTokenFromVarOrFloat(c.arg(1)));
}

/** Non linear constraint atan x = y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::float_atan(const Call& c) {
  nlconsOperatorUnary(c, NLToken::OpCode::OP_atan, getTokenFromVarOrFloat(c.arg(0)),
                      getTokenFromVarOrFloat(c.arg(1)));
}

/** Non linear constraint atanh x = y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::float_atanh(const Call& c) {
  nlconsOperatorUnary(c, NLToken::OpCode::OP_atanh, getTokenFromVarOrFloat(c.arg(0)),
                      getTokenFromVarOrFloat(c.arg(1)));
}

/** Non linear constraint cos x = y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::float_cos(const Call& c) {
  nlconsOperatorUnary(c, NLToken::OpCode::OP_cos, getTokenFromVarOrFloat(c.arg(0)),
                      getTokenFromVarOrFloat(c.arg(1)));
}

/** Non linear constraint cosh x = y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::float_cosh(const Call& c) {
  nlconsOperatorUnary(c, NLToken::OpCode::OP_cosh, getTokenFromVarOrFloat(c.arg(0)),
                      getTokenFromVarOrFloat(c.arg(1)));
}

/** Non linear constraint exp x = y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::float_exp(const Call& c) {
  nlconsOperatorUnary(c, NLToken::OpCode::OP_exp, getTokenFromVarOrFloat(c.arg(0)),
                      getTokenFromVarOrFloat(c.arg(1)));
}

/** Non linear constraint ln x = y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::float_ln(const Call& c) {
  nlconsOperatorUnary(c, NLToken::OpCode::OP_log, getTokenFromVarOrFloat(c.arg(0)),
                      getTokenFromVarOrFloat(c.arg(1)));
}

/** Non linear constraint log10 x = y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::float_log10(const Call& c) {
  nlconsOperatorUnary(c, NLToken::OpCode::OP_log10, getTokenFromVarOrFloat(c.arg(0)),
                      getTokenFromVarOrFloat(c.arg(1)));
}

/** Non linear constraint log2 x = y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::float_log2(const Call& c) {
  nlconsOperatorUnaryLog2(c, getTokenFromVarOrFloat(c.arg(0)), getTokenFromVarOrFloat(c.arg(1)));
}

/** Non linear constraint sqrt x = y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::float_sqrt(const Call& c) {
  nlconsOperatorUnary(c, NLToken::OpCode::OP_sqrt, getTokenFromVarOrFloat(c.arg(0)),
                      getTokenFromVarOrFloat(c.arg(1)));
}

/** Non linear constraint sin x = y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::float_sin(const Call& c) {
  nlconsOperatorUnary(c, NLToken::OpCode::OP_sin, getTokenFromVarOrFloat(c.arg(0)),
                      getTokenFromVarOrFloat(c.arg(1)));
}

/** Non linear constraint sinh x = y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::float_sinh(const Call& c) {
  nlconsOperatorUnary(c, NLToken::OpCode::OP_sinh, getTokenFromVarOrFloat(c.arg(0)),
                      getTokenFromVarOrFloat(c.arg(1)));
}

/** Non linear constraint tan x = y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::float_tan(const Call& c) {
  nlconsOperatorUnary(c, NLToken::OpCode::OP_tan, getTokenFromVarOrFloat(c.arg(0)),
                      getTokenFromVarOrFloat(c.arg(1)));
}

/** Non linear constraint tanh x = y */
// NOLINTNEXTLINE(readability-identifier-naming)
void NLFile::float_tanh(const Call& c) {
  nlconsOperatorUnary(c, NLToken::OpCode::OP_tanh, getTokenFromVarOrFloat(c.arg(0)),
                      getTokenFromVarOrFloat(c.arg(1)));
}

// --- --- --- Other

/** Integer x to floating point y. Constraint x = y translated into x - y = 0.
 *  Simulate a linear constraint [1, -1]+*[x, y] = 0
 */
void NLFile::int2float(const Call& c) {
  vector<double> coeffs = {1, -1};
  vector<string> vars = {};
  vars.push_back(getTokenFromVar(c.arg(0)).str);
  vars.push_back(getTokenFromVar(c.arg(1)).str);
  // Create the constraint
  linconsEq(c, coeffs, vars, NLToken::n(0));
}

// --- --- --- Objective

/** Add a solve goal in the NL File. In our case, we can only have one and only one solve goal. */
void NLFile::addSolve(SolveI::SolveType st, const Expression* e) {
  // We can only have one objective. Prevent override.
  assert(!objective.isDefined());

  switch (st) {
    case SolveI::SolveType::ST_SAT: {
      // Satisfy: implemented by minimizing 0 (print n0 for an empty expression graph)
      objective.minmax = objective.SATISFY;
      break;
    }
    case SolveI::SolveType::ST_MIN: {
      // Maximize an objective represented by a variable
      objective.minmax = objective.MINIMIZE;
      string v = getTokenFromVar(e).str;
      // Use the gradient
      vector<double> coeffs = {1};
      vector<string> vars = {v};
      objective.setGradient(vars, coeffs);
      break;
    }
    case SolveI::SolveType::ST_MAX: {
      // Maximize an objective represented by a variable
      objective.minmax = objective.MAXIMIZE;
      string v = getTokenFromVar(e).str;
      // Use the gradient
      vector<double> coeffs = {1};
      vector<string> vars = {v};
      objective.setGradient(vars, coeffs);
      break;
    }
  }

  // Ensure that the obejctive is now defined.
  assert(objective.isDefined());
}

/* *** *** *** Phase 2: processing *** *** *** */

void NLFile::phase2() {
  // --- --- --- Go over all constraint (algebraic AND logical) and mark non linear variables
  for (auto const& n_c : constraints) {
    for (auto const& tok : n_c.second.expressionGraph) {
      if (tok.isVariable()) {
        variables.at(tok.str).isInNLConstraint = true;
      }
    }
  }

  for (auto const& c : logicalConstraints) {
    for (auto const& tok : c.expressionGraph) {
      if (tok.isVariable()) {
        variables.at(tok.str).isInNLConstraint = true;
      }
    }
  }

  // --- --- --- Go over the objective and mark non linear variables
  for (auto const& tok : objective.expressionGraph) {
    if (tok.isVariable()) {
      variables.at(tok.str).isInNLObjective = true;
    }
  }

  // --- --- --- Variables ordering and indexing
  for (auto const& name_var : variables) {
    const NLVar& v = name_var.second;

    // Accumulate jacobian count
    _jacobianCount += v.jacobianCount;

    // First check non linear variables in BOTH objective and constraint.
    if (v.isInNLObjective && v.isInNLConstraint) {
      if (v.isInteger) {
        vname_nliv_both.push_back(v.name);
      } else {
        vname_nlcv_both.push_back(v.name);
      }
    }
    // Variables in non linear constraint ONLY
    else if (!v.isInNLObjective && v.isInNLConstraint) {
      if (v.isInteger) {
        vname_nliv_cons.push_back(v.name);
      } else {
        vname_nlcv_cons.push_back(v.name);
      }
    }
    // Variables in non linear objective ONLY
    else if (v.isInNLObjective && !v.isInNLConstraint) {
      if (v.isInteger) {
        vname_nliv_obj.push_back(v.name);
      } else {
        vname_nlcv_obj.push_back(v.name);
      }
    }
    // Variables not appearing nonlinearly
    else if (!v.isInNLObjective && !v.isInNLConstraint) {
      if (v.isInteger) {
        vname_liv_all.push_back(v.name);
      } else {
        vname_lcv_all.push_back(v.name);
      }
    }
    // Should not happen
    else {
      should_not_happen("Dispatching variables in phase2");
    }
  }

  // Note:  In the above, we dealt with all 'vname_*' vectors BUT 'vname_larc_all' and
  // 'vname_bv_all'
  //        networks and boolean are not implemented. Nevertheless, we keep the vectors and deal
  //        with them below to ease further implementations.

  vnames.reserve(variables.size());

  vnames.insert(vnames.end(), vname_nlcv_both.begin(), vname_nlcv_both.end());
  vnames.insert(vnames.end(), vname_nliv_both.begin(), vname_nliv_both.end());
  vnames.insert(vnames.end(), vname_nlcv_cons.begin(), vname_nlcv_cons.end());
  vnames.insert(vnames.end(), vname_nliv_cons.begin(), vname_nliv_cons.end());
  vnames.insert(vnames.end(), vname_nlcv_obj.begin(), vname_nlcv_obj.end());
  vnames.insert(vnames.end(), vname_nliv_obj.begin(), vname_nliv_obj.end());
  vnames.insert(vnames.end(), vname_larc_all.begin(), vname_larc_all.end());
  vnames.insert(vnames.end(), vname_lcv_all.begin(), vname_lcv_all.end());
  vnames.insert(vnames.end(), vname_bv_all.begin(), vname_bv_all.end());
  vnames.insert(vnames.end(), vname_liv_all.begin(), vname_liv_all.end());

  // Create the mapping name->index
  for (int i = 0; i < vnames.size(); ++i) {
    variableIndexes[vnames[i]] = i;
  }

  // --- --- --- Constraint ordering, couting, and indexing
  for (auto const& name_cons : constraints) {
    const NLAlgCons& c = name_cons.second;

    // Sort by linearity. We do not have network constraint.
    if (c.isLinear()) {
      cnames_lin_general.push_back(c.name);
    } else {
      cnames_nl_general.push_back(c.name);
    }

    // Count the number of ranges and eqns constraints
    switch (c.range.tag) {
      case NLBound::LB_UB: {
        ++algConsRangeCount;
      }
      case NLBound::EQ: {
        ++algConsEqCount;
      }
      default: {
        break;
      }
    }
  }

  cnames.reserve(constraints.size());
  cnames.insert(cnames.end(), cnames_nl_general.begin(), cnames_nl_general.end());
  cnames.insert(cnames.end(), cnames_nl_network.begin(), cnames_nl_network.end());
  cnames.insert(cnames.end(), cnames_lin_network.begin(), cnames_lin_network.end());
  cnames.insert(cnames.end(), cnames_lin_general.begin(), cnames_lin_general.end());

  // Create the mapping name->index
  for (int i = 0; i < cnames.size(); ++i) {
    constraintIndexes[cnames[i]] = i;
  }
}

// --- --- --- Simple tests

bool NLFile::hasIntegerVars() const {
  return (lvbiCount() + lvciCount() + lvoiCount() + ivCount()) > 0;
}

bool NLFile::hasContinousVars() const { return !hasIntegerVars(); }

// --- --- --- Counts

/** Jacobian count. */
unsigned int NLFile::jacobianCount() const { return _jacobianCount; }

/** Total number of variables. */
unsigned int NLFile::varCount() const { return variables.size(); }

/** Number of variables appearing nonlinearly in constraints. */
unsigned int NLFile::lvcCount() const {
  // Variables in both + variables in constraint only (integer+continuous)
  return lvbCount() + vname_nliv_cons.size() + vname_nlcv_cons.size();
}

/** Number of variables appearing nonlinearly in objectives. */
unsigned int NLFile::lvoCount() const {
  // Variables in both + variables in objective only (integer+continuous)
  return lvbCount() + vname_nliv_obj.size() + vname_nlcv_obj.size();
}

/** Number of variables appearing nonlinearly in both constraints and objectives.*/
unsigned int NLFile::lvbCount() const { return vname_nlcv_both.size() + vname_nliv_both.size(); }

/** Number of integer variables appearing nonlinearly in both constraints and objectives.*/
unsigned int NLFile::lvbiCount() const { return vname_nliv_both.size(); }

/** Number of integer variables appearing nonlinearly in constraints **only**.*/
unsigned int NLFile::lvciCount() const { return vname_nliv_cons.size(); }

/** Number of integer variables appearing nonlinearly in objectives **only**.*/
unsigned int NLFile::lvoiCount() const { return vname_nliv_obj.size(); }

/** Number of linear arcs. Network nor implemented, so always 0.*/
unsigned int NLFile::wvCount() const { return vname_larc_all.size(); }

/** Number of "other" integer variables.*/
unsigned int NLFile::ivCount() const { return vname_liv_all.size(); }

/** Number of binary variables.*/
unsigned int NLFile::bvCount() const { return vname_bv_all.size(); }

/** *** *** *** Printable *** *** *** **/
// Note:  * empty line not allowed
//        * comment only not allowed
ostream& NLFile::printToStream(ostream& os) const {
  // Print the header
  {
    NLHeader header;
    NLHeader::printToStream(os, *this);
  }
  os << endl;

  // Print the unique segments about the variables
  if (varCount() > 1) {
    // Print the 'k' segment Maybe to adjust with the presence of 'J' segments
    os << "k" << (varCount() - 1)
       << "   # Cumulative Sum of non-zero in the jacobian matrix's (nbvar-1) columns." << endl;
    unsigned int acc = 0;
    // Note stop before the last var. Total jacobian count is in the header.
    for (int i = 0; i < varCount() - 1; ++i) {
      string name = vnames[i];
      acc += variables.at(name).jacobianCount;
      os << acc << "   # " << name << endl;
    }

    // Print the 'b' segment
    os << "b   # Bounds on variables (" << varCount() << ")" << endl;
    for (const auto& n : vnames) {
      NLVar v = variables.at(n);
      v.bound.printToStream(os, n);
      os << endl;
    }
  }

  // Print the unique segments about the constraints
  if (!cnames.empty()) {
    // Create the 'r' range segment per constraint
    // For now, it is NOT clear if the network constraint should appear in the range constraint or
    // not. To be determine if later implemented.
    os << "r   # Bounds on algebraic constraint bodies (" << cnames.size() << ")" << endl;
    for (const auto& n : cnames) {
      NLAlgCons c = constraints.at(n);
      c.range.printToStream(os, n);
      os << endl;
    }
  }

  // Print the Algebraic Constraints
  for (const auto& n : cnames) {
    NLAlgCons c = constraints.at(n);
    c.printToStream(os, *this);
  }

  // Print the Logical constraint
  for (const auto& lc : logicalConstraints) {
    lc.printToStream(os, *this);
  }

  // Print the objective
  objective.printToStream(os, *this);
  return os;
}

}  // namespace MiniZinc
