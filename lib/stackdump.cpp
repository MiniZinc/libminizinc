/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 *     Jason Nguyen <jason.nguyen@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/stackdump.hh>
#include <minizinc/typecheck.hh>

namespace MiniZinc {
StackDump::StackDump(EnvI& env) {
  // Make sure the call stack items are kept alive
  for (auto it = env.callStack.rbegin(); it != env.callStack.rend(); it++) {
    bool isCompIter = it->tag;
    if (!it->e->loc().isIntroduced() && !isCompIter && it->e->isa<Id>()) {
      // Stop at the inner-most ID
      // TODO: Is this actually necessary?
      if (_stack.empty()) {
        _stack.emplace_back(it->e, isCompIter);
      }
      break;
    }
    _stack.emplace_back(it->e, isCompIter);
  }
}

void StackDump::print(std::ostream& os) const {
  if (_stack.empty()) {
    return;
  }

  if (_stack.size() == 1 && _stack[0].first->isa<Id>()) {
    Expression* e = _stack[0].first;
    if (!e->loc().isIntroduced()) {
      os << e->loc().toString() << std::endl;
      os << "  in variable declaration " << *e << std::endl;
    }
    return;
  }

  ASTString curloc_f;
  long long int curloc_l = -1;

  for (auto it = _stack.rbegin(); it != _stack.rend(); it++) {
    EnvI::CallStackEntry entry(it->first, it->second);

    Expression* e = entry.e;
    bool isCompIter = entry.tag;
    ASTString newloc_f = e->loc().filename();
    if (e->loc().isIntroduced()) {
      continue;
    }
    auto newloc_l = static_cast<long long int>(e->loc().firstLine());
    if (newloc_f != curloc_f || newloc_l != curloc_l) {
      os << e->loc().toString() << std::endl;
      curloc_f = newloc_f;
      curloc_l = newloc_l;
    }
    if (isCompIter) {
      os << "    with ";
    } else {
      os << "  in ";
    }
    switch (e->eid()) {
      case Expression::E_INTLIT:
        os << "integer literal" << std::endl;
        break;
      case Expression::E_FLOATLIT:
        os << "float literal" << std::endl;
        break;
      case Expression::E_SETLIT:
        os << "set literal" << std::endl;
        break;
      case Expression::E_BOOLLIT:
        os << "bool literal" << std::endl;
        break;
      case Expression::E_STRINGLIT:
        os << "string literal" << std::endl;
        break;
      case Expression::E_ID:
        if (isCompIter) {
          if ((e->cast<Id>()->decl()->e() != nullptr) &&
              e->cast<Id>()->decl()->e()->type().isPar()) {
            os << *e << " = " << *e->cast<Id>()->decl()->e() << std::endl;
          } else {
            os << *e << " = <expression>" << std::endl;
          }
        } else {
          os << "identifier" << *e << std::endl;
        }
        break;
      case Expression::E_ANON:
        os << "anonymous variable" << std::endl;
        break;
      case Expression::E_ARRAYLIT:
        os << "array literal" << std::endl;
        break;
      case Expression::E_ARRAYACCESS:
        os << "array access" << std::endl;
        break;
      case Expression::E_FIELDACCESS:
        os << "field access" << std::endl;
        break;
      case Expression::E_COMP: {
        const Comprehension* cmp = e->cast<Comprehension>();
        if (cmp->set()) {
          os << "set ";
        } else {
          os << "array ";
        }
        os << "comprehension expression" << std::endl;
      } break;
      case Expression::E_ITE:
        os << "if-then-else expression" << std::endl;
        break;
      case Expression::E_BINOP:
        os << "binary " << e->cast<BinOp>()->opToString() << " operator expression" << std::endl;
        break;
      case Expression::E_UNOP:
        os << "unary " << e->cast<UnOp>()->opToString() << " operator expression" << std::endl;
        break;
      case Expression::E_CALL:
        os << "call '" << demonomorphise_identifier(e->cast<Call>()->id()) << "'" << std::endl;
        break;
      case Expression::E_VARDECL: {
        GCLock lock;
        os << "variable declaration for '" << e->cast<VarDecl>()->id()->str() << "'" << std::endl;
      } break;
      case Expression::E_LET:
        os << "let expression" << std::endl;
        break;
      case Expression::E_TI:
        os << "type-inst expression" << std::endl;
        break;
      case Expression::E_TIID:
        os << "type identifier" << std::endl;
        break;
      default:
        assert(false);
        os << "unknown expression (internal error)" << std::endl;
        break;
    }
  }
}

void StackDump::json(std::ostream& os) const {
  if (_stack.empty()) {
    os << "[]";
    return;
  }

  if (_stack.size() == 1 && _stack[0].first->isa<Id>()) {
    os << "[";
    Expression* e = _stack[0].first;
    if (!e->loc().isIntroduced()) {
      os << "{\"location\": " << e->loc().toJSON()
         << ", \"isCompIter\": false, \"description\": \"variable declaration\"}";
    }
    os << "]";
    return;
  }

  ASTString curloc_f;
  long long int curloc_l = -1;
  bool first = true;

  os << "[";

  for (auto it = _stack.rbegin(); it != _stack.rend(); it++) {
    EnvI::CallStackEntry entry(it->first, it->second);

    Expression* e = entry.e;
    bool isCompIter = entry.tag;
    ASTString newloc_f = e->loc().filename();
    if (e->loc().isIntroduced()) {
      continue;
    }
    if (first) {
      first = false;
    } else {
      os << ", ";
    }
    os << "{\"location\": " << e->loc().toJSON()
       << ", \"isCompIter\": " << (isCompIter ? "true" : "false") << ", \"description\": \"";
    std::stringstream ss;
    switch (e->eid()) {
      case Expression::E_INTLIT:
        ss << "integer literal";
        break;
      case Expression::E_FLOATLIT:
        ss << "float literal";
        break;
      case Expression::E_SETLIT:
        ss << "set literal";
        break;
      case Expression::E_BOOLLIT:
        ss << "bool literal";
        break;
      case Expression::E_STRINGLIT:
        ss << "string literal";
        break;
      case Expression::E_ID:
        if (isCompIter) {
          if ((e->cast<Id>()->decl()->e() != nullptr) &&
              e->cast<Id>()->decl()->e()->type().isPar()) {
            ss << *e << " = " << *e->cast<Id>()->decl()->e();
          } else {
            ss << *e << " = <expression>";
          }
        } else {
          ss << "identifier" << *e;
        }
        break;
      case Expression::E_ANON:
        ss << "anonymous variable";
        break;
      case Expression::E_ARRAYLIT:
        ss << "array literal";
        break;
      case Expression::E_ARRAYACCESS:
        ss << "array access";
        break;
      case Expression::E_COMP: {
        const Comprehension* cmp = e->cast<Comprehension>();
        if (cmp->set()) {
          ss << "set ";
        } else {
          ss << "array ";
        }
        ss << "comprehension expression";
      } break;
      case Expression::E_ITE:
        ss << "if-then-else expression";
        break;
      case Expression::E_BINOP:
        ss << "binary " << e->cast<BinOp>()->opToString() << " operator expression";
        break;
      case Expression::E_UNOP:
        ss << "unary " << e->cast<UnOp>()->opToString() << " operator expression";
        break;
      case Expression::E_CALL:
        ss << "call '" << demonomorphise_identifier(e->cast<Call>()->id()) << "'";
        break;
      case Expression::E_VARDECL: {
        GCLock lock;
        ss << "variable declaration for '" << e->cast<VarDecl>()->id()->str() << "'";
      } break;
      case Expression::E_LET:
        ss << "let expression";
        break;
      case Expression::E_TI:
        ss << "type-inst expression";
        break;
      case Expression::E_TIID:
        ss << "type identifier";
        break;
      default:
        assert(false);
        ss << "unknown expression";
        break;
    }
    os << Printer::escapeStringLit(ss.str()) << "\"}";
  }
  os << "]";
}
}  // namespace MiniZinc
