/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/solvers/nl/nl_components.hh>
#include <minizinc/solvers/nl/nl_file.hh>

#include <utility>

using namespace std;

namespace MiniZinc {

/* *** *** *** NLBound *** *** *** */

// Constructors
NLBound::NLBound(Bound tag, double lb, double ub) : tag(tag), lb(lb), ub(ub) {}

NLBound NLBound::makeBounded(double lb, double ub) {
  if (lb == ub) {
    return makeEqual(lb);
  }
  assert(lb < ub);
  return NLBound(LB_UB, lb, ub);
}

NLBound NLBound::makeUBBounded(double ub) { return NLBound(UB, 0, ub); }

NLBound NLBound::makeLBBounded(double lb) { return NLBound(LB, lb, 0); }

NLBound NLBound::makeNoBound() { return NLBound(NONE, 0, 0); }

NLBound NLBound::makeEqual(double val) { return NLBound(EQ, val, val); }

/** Update the lower bound */
void NLBound::updateLB(double new_lb) {
  switch (tag) {
    // LB <= var <= UB. Same tag
    case NLBound::LB_UB: {
      assert(new_lb <= ub);
      if (new_lb > lb) {
        lb = new_lb;
      }
      break;
    }
    // var <= UB. Update tag
    case NLBound::UB: {
      assert(new_lb <= ub);
      tag = LB_UB;
      lb = new_lb;
      break;
    }
    // LB <= var. Same tag
    case NLBound::LB: {
      if (new_lb > lb) {
        lb = new_lb;
      }
      break;
    }
    // No bound. Update tag
    case NLBound::NONE: {
      tag = LB;
      lb = new_lb;
      break;
    }
    // LB = var = UB. Should not happen
    case NLBound::EQ: {
      should_not_happen(
          "Updating a bound already set to \"equals\". We only allow tightening update.");
    }
  }
}

/** Update the upper bound */
void NLBound::updateUB(double new_ub) {
  switch (tag) {
    // LB <= var <= UB. Same tag
    case NLBound::LB_UB: {
      assert(lb <= new_ub);
      if (new_ub < ub) {
        ub = new_ub;
      }
      break;
    }
    // var <= UB. Same tag
    case NLBound::UB: {
      if (new_ub < ub) {
        ub = new_ub;
      }
      break;
    }
    // LB <= var. Update tag
    case NLBound::LB: {
      assert(lb <= new_ub);
      tag = LB_UB;
      ub = new_ub;
      break;
    }
    // No bound. Update tag
    case NLBound::NONE: {
      tag = UB;
      ub = new_ub;
      break;
    }
    // LB = var = UB. Should not happen
    case NLBound::EQ: {
      should_not_happen(
          "Updating a bound already set to \"equals\". We only allow tightening update.");
    }
  }
}

/** Update equal bound */
void NLBound::updateEq(double new_eq) {
  switch (tag) {
    // LB <= var <= UB. Update tag
    case NLBound::LB_UB: {
      assert(lb <= new_eq && new_eq <= ub);
      tag = EQ;
      lb = new_eq;
      ub = new_eq;
    }
    // var <= UB. Update tag
    case NLBound::UB: {
      assert(new_eq <= ub);
      tag = EQ;
      lb = new_eq;
      ub = new_eq;
    }
    // LB <= var. Update tag
    case NLBound::LB: {
      assert(lb <= new_eq);
      tag = EQ;
      lb = new_eq;
      ub = new_eq;
    }
    // No bound. Update tag
    case NLBound::NONE: {
      tag = EQ;
      lb = new_eq;
      ub = new_eq;
    }
    // LB = var = UB. Can happen only if we "update" with the same bound.
    case NLBound::EQ: {
      assert(lb == new_eq);
    }
  }
}

/** Printing with a name. */
ostream& NLBound::printToStream(ostream& os, const string& vname) const {
  switch (tag) {
    case LB_UB: {
      os << "0 " << lb << " " << ub << "   # " << lb << " =< " << vname << " =< " << ub;
      break;
    }
    case UB: {
      os << "1 " << ub << "   # " << vname << " =< " << ub;
      break;
    }
    case LB: {
      os << "2 " << lb << "   # " << lb << " =< " << vname;
      break;
    }
    case NONE: {
      os << "3"
         << "   # No constraint";
      break;
    }
    case EQ: {
      os << "4 " << lb << "   # " << vname << " = " << lb;
      break;
    }
  }
  return os;
}

/** Default printing */
ostream& NLBound::printToStream(ostream& os) const {
  printToStream(os, "body");
  return os;
}

/* *** *** *** NLVar *** *** *** */

NLVar NLVar::copyWithBound(NLBound bound) const {
  NLVar v = NLVar(*this);  // copy constructor
  v.bound = bound;
  return v;
}

/* *** *** *** NLArray *** *** *** */

/* *** *** *** NLToken *** *** *** */

const char* NLToken::getName(OpCode oc) {
  switch (oc) {
    case OpCode::OPPLUS:
      return "OPPLUS";
    case OpCode::OPMINUS:
      return "OPMINUS";
    case OpCode::OPMULT:
      return "OPMULT";
    case OpCode::OPDIV:
      return "OPDIV";
    case OpCode::OPREM:
      return "OPREM";
    case OpCode::OPPOW:
      return "OPPOW";
    case OpCode::OPLESS:
      return "OPLESS";
    case OpCode::FLOOR:
      return "FLOOR";
    case OpCode::CEIL:
      return "CEIL";
    case OpCode::ABS:
      return "ABS";
    case OpCode::OPUMINUS:
      return "OPUMINUS";
    case OpCode::OPOR:
      return "OPOR";
    case OpCode::OPAND:
      return "OPAND";
    case OpCode::LT:
      return "LT";
    case OpCode::LE:
      return "LE";
    case OpCode::EQ:
      return "EQ";
    case OpCode::GE:
      return "GE";
    case OpCode::GT:
      return "GT";
    case OpCode::NE:
      return "NE";
    case OpCode::OPNOT:
      return "OPNOT";
    case OpCode::OPIFnl:
      return "OPIFnl";
    case OpCode::OP_tanh:
      return "OP_tanh";
    case OpCode::OP_tan:
      return "OP_tan";
    case OpCode::OP_sqrt:
      return "OP_sqrt";
    case OpCode::OP_sinh:
      return "OP_sinh";
    case OpCode::OP_sin:
      return "OP_sin";
    case OpCode::OP_log10:
      return "OP_log10";
    case OpCode::OP_log:
      return "OP_log";
    case OpCode::OP_exp:
      return "OP_exp";
    case OpCode::OP_cosh:
      return "OP_cosh";
    case OpCode::OP_cos:
      return "OP_cos";
    case OpCode::OP_atanh:
      return "OP_atanh";
    case OpCode::OP_atan2:
      return "OP_atan2";
    case OpCode::OP_atan:
      return "OP_atan";
    case OpCode::OP_asinh:
      return "OP_asinh";
    case OpCode::OP_asin:
      return "OP_asin";
    case OpCode::OP_acosh:
      return "OP_acosh";
    case OpCode::OP_acos:
      return "OP_acos";
    case OpCode::OPintDIV:
      return "OPintDIV";
    case OpCode::OPprecision:
      return "OPprecision";
    case OpCode::OPround:
      return "OPround";
    case OpCode::OPtrunc:
      return "OPtrunc";
    case OpCode::OPATLEAST:
      return "OPATLEAST";
    case OpCode::OPATMOST:
      return "OPATMOST";
    case OpCode::OPPLTERM:
      return "OPPLTERM";
    case OpCode::OPIFSYM:
      return "OPIFSYM";
    case OpCode::OPEXACTLY:
      return "OPEXACTLY";
    case OpCode::OPNOTATLEAST:
      return "OPNOTATLEAST";
    case OpCode::OPNOTATMOST:
      return "OPNOTATMOST";
    case OpCode::OPNOTEXACTLY:
      return "OPNOTEXACTLY";
    case OpCode::OPIMPELSE:
      return "OPIMPELSE";
    case OpCode::OP_IFF:
      return "OP_IFF";
    case OpCode::OPSOMESAME:
      return "OPSOMESAME";
    case OpCode::OP1POW:
      return "OP1POW";
    case OpCode::OP2POW:
      return "OP2POW";
    case OpCode::OPCPOW:
      return "OPCPOW";
    case OpCode::OPFUNCALL:
      return "OPFUNCALL";
    case OpCode::OPNUM:
      return "OPNUM";
    case OpCode::OPHOL:
      return "OPHOL";
    case OpCode::OPVARVAL:
      return "OPVARVAL";
    case OpCode::N_OPS:
      return "N_OPS";
    default:
      assert(false);
      return nullptr;
  }
};

const char* NLToken::getName(MOpCode moc) {
  switch (moc) {
    case MOpCode::MINLIST:
      return "MINLIST";
    case MOpCode::MAXLIST:
      return "MAXLIST";
    case MOpCode::OPSUMLIST:
      return "OPSUMLIST";
    case MOpCode::OPCOUNT:
      return "OPCOUNT";
    case MOpCode::OPNUMBEROF:
      return "OPNUMBEROF";
    case MOpCode::OPNUMBEROFs:
      return "OPNUMBEROFs";
    case MOpCode::ANDLIST:
      return "ANDLIST";
    case MOpCode::ORLIST:
      return "ORLIST";
    case MOpCode::OPALLDIFF:
      return "OPALLDIFF";
    default:
      assert(false);
      return nullptr;
  }
}

NLToken NLToken::n(double value) {
  NLToken tok;
  tok.kind = Kind::NUMERIC;
  tok.numericValue = value;
  return tok;
}

NLToken NLToken::v(string vname) {
  NLToken tok;
  tok.kind = Kind::VARIABLE;
  tok.str = std::move(vname);
  return tok;
}

NLToken NLToken::o(OpCode opc) {
  NLToken tok;
  tok.kind = Kind::OP;
  tok.oc = opc;
  return tok;
}

NLToken NLToken::mo(MOpCode mopc, int nb) {
  NLToken tok;
  tok.kind = Kind::MOP;
  tok.moc = mopc;
  tok.argCount = nb;
  return tok;
}

bool NLToken::isVariable() const { return kind == VARIABLE; }

bool NLToken::isConstant() const { return kind == NUMERIC; }

ostream& NLToken::printToStream(ostream& os, const NLFile& nl_file) const {
  switch (kind) {
    case Kind::NUMERIC: {
      os << "n" << numericValue;
      break;
    }

    case Kind::VARIABLE: {
      os << "v" << nl_file.variableIndexes.at(str) << " # " << str;
      break;
    }

    case Kind::STRING: {
      should_not_happen("NL string token (Kind::STRING) not implemented");
    }

    case Kind::FUNCALL: {
      should_not_happen("NL function call token (Kind::FUNCALL) not implemented");
    }

    case Kind::OP: {
      os << "o" << oc << " # " << getName(oc);
      break;
    }

    case Kind::MOP: {
      os << "o" << moc << " # " << getName(moc) << " " << argCount << endl;
      os << argCount;
      break;
    }

    default:
      should_not_happen("Unknown token kind: " << kind);
  }

  return os;
}

/* *** *** *** NLAlgCons *** *** *** */

/** Method to build the var_coeff vector. */
void NLAlgCons::setJacobian(const vector<string>& vnames, const vector<double>& coeffs,
                            NLFile* nl_file) {
  assert(vnames.size() == coeffs.size());
  for (int i = 0; i < vnames.size(); ++i) {
    string vn = vnames[i];
    nl_file->variables.at(vn).jacobianCount++;
    jacobian.emplace_back(vn, coeffs[i]);
  }
}

/** A constraint is considered linear if the expressionGraph is empty. */
bool NLAlgCons::isLinear() const { return expressionGraph.empty(); }

/** Printing. */
ostream& NLAlgCons::printToStream(ostream& os, const NLFile& nl_file) const {
  int idx = nl_file.constraintIndexes.at(name);

  // Print the 'C' segment: if no expression graph, print "n0".
  os << "C" << idx << "   # Non linear part of " << name << endl;
  if (expressionGraph.empty()) {
    os << "n0   # No non linear part coded as the value '0'" << endl;
  } else {
    for (const auto& t : expressionGraph) {
      t.printToStream(os, nl_file);
      os << endl;
    }
  }

  // Print the 'J' segment if present.
  if (!jacobian.empty()) {
    os << "J" << idx << " " << jacobian.size() << "   # Linear part of " << name << endl;
    for (const auto& vn_coef : jacobian) {
      os << nl_file.variableIndexes.at(vn_coef.first) << " " << vn_coef.second << "   # "
         << vn_coef.first << endl;
    }
  }

  return os;
}

/* *** *** *** NLLogicalCons *** *** *** */

/** Printing. */
ostream& NLLogicalCons::printToStream(ostream& os, const NLFile& nl_file) const {
  os << "L" << index << "   # Logical constraint " << name << endl;
  for (const auto& t : expressionGraph) {
    t.printToStream(os, nl_file);
    os << endl;
  }

  return os;
}

/* *** *** *** NLHeader *** *** *** */

/** Printing the header.
 *  The header is composed of then lines that we describe as we proceed.
 *  A '#' starts a comment until the end of the line. However, it cannot be a line on its own!*/
ostream& NLHeader::printToStream(ostream& os, const NLFile& nl_file) {
  // 1st line:
  // 'g': file will be in text format
  // other numbers: as given in the doc (no other explanation...)
  os << "g3 1 1 0" << endl;

  // 2nd line:
  os << nl_file.variables.size() << " "  // Total number of variables
     << nl_file.constraints.size()
     << " "       // Total number of algebraic constraint (including 'range' and 'eq')
     << 1 << " "  // Always 1 objective
     << nl_file.algConsRangeCount << " "          // Number of algebraic range constraints
     << nl_file.algConsEqCount << " "             // Number of algebraic eq constraints
     << nl_file.logicalConstraints.size() << " "  // Number of logical constraints
     << "# Total nb of:  variables,  algebraic constraints,  objectives,  ranges,  eqs,  logical "
        "constraints"
     << endl;

  // 3rd line: Nonlinear and complementary information
  os << nl_file.cnames_nl_general.size() << " "        // Non linear constraints
     << (nl_file.objective.isLinear() ? 0 : 1) << " "  // Non linear objective
     << "# Nb of nonlinear constraints,  nonlinar objectives." << endl;
  /* This was found in the online source of the ASL parser, but is not produce in our ampl tests.
   * If needed, should be put on the same line
  << nb_complementarity_linear_conditions << " "
  << nb_complementarity_nonlinear_conditions << " "
  << nb_complementarity_items_double_inequalities << " "
  << nb_complemented_vars_non_zero_lowerbound << " "
  << "Nb of complementary: linear & nonlinear conditions, double inequalities, vars with non-0 lower
  bound."
  << endl;
  */

  // 4th line: Network constraints
  os << nl_file.cnames_nl_network.size() << " "   // Number of nonlinear network constraints
     << nl_file.cnames_lin_network.size() << " "  // Number of linear network constraints
     << "# Nb of network constraints: nonlinear,  linear." << endl;

  // 5th line: nonlinear variables:
  os << nl_file.lvcCount() << " "  // Nb of nonlinear vars in constraints
     << nl_file.lvoCount() << " "  // Nb of nonlinear vars in objectives
     << nl_file.lvbCount() << " "  // Nb of nonlinear vars in both
     << "# Nb of non linear vars in:  constraints,  objectives,  both." << endl;

  // 6th line:
  os << nl_file.wvCount() << " "  // Nb of linear network vars
     << "0"
     << " "  // Nb of functions. Not Implemented
     << "0 1 "
     << "# Nb of: linear network vars,  functions. Floating point arithmetic mode (TEXT == 0). "
        "Flag: if 1, add .sol suffixe."
     << endl;

  // 7th line: discrete variables
  os << nl_file.bvCount() << " "    // Nb of linear binary vars
     << nl_file.ivCount() << " "    // Nb of linear integer vars
     << nl_file.lvbiCount() << " "  // Nb of nonlinear integer vars in both
     << nl_file.lvciCount() << " "  // Nb of nonlinear integer vars in constraints only
     << nl_file.lvoiCount() << " "  // Nb of nonlinear integer vars in objectives only
     << "# Nb of linear vars: binary, integer (non binary). "
     << "Nb of nonlinear integer vars in: both,  constraints only,  objectives only." << endl;

  // 8th line: non zeros
  os << nl_file.jacobianCount() << " "            // Nb of nonzero in jacobian
     << nl_file.objective.gradientCount() << " "  // Nb of nonzero in gradient
     << "# Nb of non zeros in: jacobian, objective gradients." << endl;

  // 9th line: name length. Our tests always produce 0...
  os << "0"
     << " "  // Max constraint name length (??)
     << "0"
     << " "  // Max var name length (??)
     << "# Longest name among: contraints' name, vars' name." << endl;

  // 10th line: common expressions. Not enough infor for now...
  os << "0"
     << " "  // Nb common exprs in both
     << "0"
     << " "  // Nb common exprs in constraints only
     << "0"
     << " "  // Nb common exprs in objectives only
     << "0"
     << " "  // Nb common exprs in single constraint only
     << "0"
     << " "  // Nb common exprs in single objectives only
     << "# Nb of common expressions in: both, constraints only, objectives only, single "
        "constraint, single objective.";

  return os;
}

/* *** *** *** NLObjective *** *** *** */

/** Gradient count. */
int NLObjective::gradientCount() const { return gradient.size(); }

/** Set the gradient. */
void NLObjective::setGradient(const vector<string>& vnames, const vector<double>& coeffs) {
  assert(vnames.size() == coeffs.size());
  for (int i = 0; i < vnames.size(); ++i) {
    string vn = vnames[i];
    gradient.emplace_back(vn, coeffs[i]);
  }
}

/** A objective is considered as linear if its expression graph is non empty. */
bool NLObjective::isDefined() const { return minmax != UNDEF; }

bool NLObjective::isLinear() const { return expressionGraph.empty(); }

bool NLObjective::isOptimisation() const { return minmax >= MINIMIZE; }

/** Printing. */
ostream& NLObjective::printToStream(ostream& os, const NLFile& nl_file) const {
  if (minmax != UNDEF) {
    if (minmax == SATISFY) {
      os << "O0 0   # Satisfy objectif implemented as 'minimize 0'" << endl;
      os << "n0" << endl;
    } else {
      os << "O0 " << minmax << "   # Objectif (0: minimize, 1: maximize)" << endl;
      if (expressionGraph.empty()) {
        os << "n0  # No expression graph" << endl;
      } else {
        for (const auto& tok : expressionGraph) {
          tok.printToStream(os, nl_file) << endl;
        }
      }
      // Print gradient
      if (!gradient.empty()) {
        os << "G0 " << gradient.size() << "   # Objective Linear part" << endl;
        for (const auto& vn_coef : gradient) {
          os << nl_file.variableIndexes.at(vn_coef.first) << " " << vn_coef.second << "   # "
             << vn_coef.first << endl;
        }
      }
    }
  }

  return os;
}
}  // namespace MiniZinc
