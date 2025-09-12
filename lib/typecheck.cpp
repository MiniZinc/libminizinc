/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/astexception.hh>
#include <minizinc/astiterator.hh>
#include <minizinc/aststring.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/hash.hh>
#include <minizinc/output.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/typecheck.hh>

#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>

namespace MiniZinc {

Scopes::Scopes() { _s.emplace_back(ST_TOPLEVEL); }

void Scopes::add(EnvI& env, VarDecl* vd) {
  if (!_s.back().toplevel() && vd->ti()->isEnum() && (vd->e() != nullptr)) {
    throw TypeError(env, Expression::loc(vd), "enums are only allowed at top level");
  }
  if (vd->id()->idn() == -1 && vd->id()->v().empty()) {
    return;
  }
  // If the current scope is ST_INNER, check if vd shadows another
  // declaration from the same functional or toplevel scope
  if (_s.back().st == ST_INNER) {
    assert(_s.size() > 1);  // at least toplevel scope above
    for (int i = static_cast<int>(_s.size()) - 2; i >= 0; i--) {
      auto previous = _s[i].m.find(vd->id());
      if (previous != _s[i].m.end()) {
        std::ostringstream oss;
        unsigned int earlier_l = Expression::loc(previous->second->id()).firstLine();
        unsigned int earlier_c = Expression::loc(previous->second->id()).firstColumn();
        oss << "variable `" << *vd->id() << "` shadows variable with the same name in line "
            << earlier_l << "." << earlier_c;
        env.addWarning(Expression::loc(vd), oss.str(), false);
        break;
      }
      if (_s[i].st != ST_INNER) {
        break;
      }
    }
  }

  auto vdi = _s.back().m.find(vd->id());
  if (vdi == _s.back().m.end()) {
    _s.back().m.insert(vd->id(), vd);
  } else if (vd->id()->idn() >= -1) {
    GCLock lock;
    std::ostringstream ss;
    ss << "identifier `" << vd->id()->str() << "' already defined";
    throw TypeError(env, Expression::loc(vd), ss.str());
  }
}

void Scopes::pushToplevel() { _s.emplace_back(ST_TOPLEVEL); }

void Scopes::pushFun() { _s.emplace_back(ST_FUN); }

void Scopes::push() { _s.emplace_back(ST_INNER); }

void Scopes::pop() { _s.pop_back(); }

VarDecl* Scopes::find(Id* ident) {
  int cur = static_cast<int>(_s.size()) - 1;
  for (;;) {
    auto vdi = _s[cur].m.find(ident);
    if (vdi == _s[cur].m.end()) {
      if (_s[cur].toplevel()) {
        if (cur > 0) {
          cur = 0;
        } else {
          return nullptr;
        }
      } else {
        cur--;
      }
    } else {
      return vdi->second;
    }
  }
}

VarDecl* Scopes::findSimilar(Id* ident) {
  VarDecl* mostSimilar = nullptr;
  int cur = static_cast<int>(_s.size()) - 1;
  int minEdits = 3;
  for (;;) {
    for (auto decls : _s[cur].m) {
      int edits = ident->levenshteinDistance(decls.first);
      if (edits < minEdits && std::abs(static_cast<int>(ident->v().size()) -
                                       static_cast<int>(decls.first->v().size())) <= 3) {
        minEdits = edits;
        mostSimilar = decls.second;
      }
    }
    if (_s[cur].toplevel()) {
      if (cur > 0) {
        cur = 0;
      } else {
        break;
      }
    } else {
      cur--;
    }
  }
  return mostSimilar;
}

class VarDeclCmp {
private:
  std::unordered_map<VarDecl*, int>& _pos;

public:
  VarDeclCmp(std::unordered_map<VarDecl*, int>& pos) : _pos(pos) {}
  bool operator()(Expression* e0, Expression* e1) {
    if (auto* vd0 = Expression::dynamicCast<VarDecl>(e0)) {
      if (auto* vd1 = Expression::dynamicCast<VarDecl>(e1)) {
        return _pos[vd0] < _pos[vd1];
      }
      return true;
    }
    return false;
  }
};
class ItemCmp {
private:
  std::unordered_map<VarDecl*, int>& _pos;

public:
  ItemCmp(std::unordered_map<VarDecl*, int>& pos) : _pos(pos) {}
  bool operator()(Item* i0, Item* i1) {
    if (auto* vd0 = i0->cast<VarDeclI>()) {
      if (auto* vd1 = i1->cast<VarDeclI>()) {
        return _pos[vd0->e()] < _pos[vd1->e()];
      }
      return true;
    }
    return false;
  }
};

// Create all required mapping functions for a new enum
// (mapping enum identifiers to strings, and mapping between different enums)
void create_enum_mapper(EnvI& env, Model* m, unsigned int enumId, VarDecl* vd, Model* enumItems,
                        IdMap<bool>& needToString, std::vector<Call*>& enumConstructorSetTypes) {
  GCLock lock;

  Id* ident = vd->id();

  if (vd->e() == nullptr) {
    // Enum without right hand side (may be supplied later in an assignment
    // item, or we may be runnint in --model-interface-only mode).
    // Need to create stub function declarations, so that the type checker
    // is happy.
    Type tx = Type::parint();
    tx.ot(Type::OT_OPTIONAL);
    auto* ti_aa = new TypeInst(Location().introduce(), tx);
    auto* vd_aa = new VarDecl(Location().introduce(), ti_aa, env.genId());
    vd_aa->toplevel(false);

    auto* ti_ab = new TypeInst(Location().introduce(), Type::parbool());
    auto* vd_ab = new VarDecl(Location().introduce(), ti_ab, env.genId());
    vd_ab->toplevel(false);

    auto* ti_aj = new TypeInst(Location().introduce(), Type::parbool());
    auto* vd_aj = new VarDecl(Location().introduce(), ti_aj, env.genId());
    vd_aj->toplevel(false);

    auto* ti_fi = new TypeInst(Location().introduce(), Type::parstring());
    std::vector<VarDecl*> fi_params(3);
    fi_params[0] = vd_aa;
    fi_params[1] = vd_ab;
    fi_params[2] = vd_aj;
    auto* fi = new FunctionI(Location().introduce(),
                             ASTString(create_enum_to_string_name(ident, "_toString_")), ti_fi,
                             fi_params, nullptr);
    enumItems->addItem(fi);

    return;
  }

  std::vector<Expression*> stack = {vd->e()};
  std::vector<Expression*> parts;
  while (!stack.empty()) {
    Expression* vde = stack.back();
    stack.pop_back();
    Call* c = Expression::dynamicCast<Call>(vde);
    auto* al = Expression::dynamicCast<ArrayLit>(vde);
    if (Expression::isa<SetLit>(vde)) {
      parts.push_back(vde);
    } else if ((al != nullptr) || ((c != nullptr) && c->id() == env.constants.ids.anon_enum &&
                                   c->argCount() == 1 && Expression::isa<ArrayLit>(c->arg(0)))) {
      if (c != nullptr) {
        al = Expression::cast<ArrayLit>(c->arg(0));
      }
      std::vector<Expression*> enumIds(al->size());
      for (unsigned int i = 0; i < al->size(); i++) {
        if (Id* eid = Expression::dynamicCast<Id>((*al)[i])) {
          enumIds[i] = eid;
        } else {
          std::ostringstream ss;
          ss << "invalid initialisation for enum `" << ident->v() << "'";
          throw TypeError(env, Expression::loc(vd->e()), ss.str());
        }
      }
      parts.push_back(new SetLit(Expression::loc(vd->e()), enumIds));
    } else if (c != nullptr) {
      if (c->id() == env.constants.ids.enumFromConstructors) {
        if (c->argCount() != 1 || !Expression::isa<ArrayLit>(c->arg(0))) {
          throw TypeError(env, Expression::loc(c),
                          "enumFromConstructors used with incorrect argument type (only supports "
                          "array literals)");
        }
        auto* al = Expression::cast<ArrayLit>(c->arg(0));
        for (unsigned int i = 0; i < al->size(); i++) {
          parts.push_back((*al)[i]);
        }
      } else {
        parts.push_back(c);
      }
    } else if (auto* binop = Expression::dynamicCast<BinOp>(vde)) {
      if (binop->op() != BinOpType::BOT_PLUSPLUS) {
        throw TypeError(
            env, Expression::loc(vde),
            std::string("invalid initialisation for enum `") + ident->v().c_str() + "'");
      }
      stack.push_back(binop->rhs());
      stack.push_back(binop->lhs());
    } else {
      throw TypeError(env, Expression::loc(vd->e()),
                      std::string("invalid initialisation for enum `") + ident->v().c_str() + "'");
    }
  }

  std::vector<Expression*> partCardinality;
  for (unsigned int p = 0; p < parts.size(); p++) {
    if (auto* sl = Expression::dynamicCast<SetLit>(parts[p])) {
      Expression* prevCardinality = partCardinality.empty() ? nullptr : partCardinality.back();
      for (unsigned int i = 0; i < sl->v().size(); i++) {
        if (!Expression::isa<Id>(sl->v()[i])) {
          throw TypeError(
              env, Expression::locDefault(sl->v()[i], sl),
              std::string("invalid initialisation for enum `") + ident->v().c_str() + "'");
        }
        auto* ti_id = new TypeInst(Expression::loc(sl->v()[i]), Type::parenum(enumId));

        std::vector<Expression*> toEnumArgs(2);
        toEnumArgs[0] = vd->id();
        if (prevCardinality == nullptr) {
          toEnumArgs[1] = IntLit::a(i + 1);
        } else {
          toEnumArgs[1] =
              new BinOp(Location().introduce(), prevCardinality, BOT_PLUS, IntLit::a(i + 1));
        }
        Call* toEnum = Call::a(Expression::loc(sl->v()[i]), ASTString("to_enum"), toEnumArgs);
        auto* vd_id = new VarDecl(Expression::loc(ti_id), ti_id,
                                  Expression::cast<Id>(sl->v()[i])->str(), toEnum);
        auto* vdi_id = VarDeclI::a(Expression::loc(vd_id), vd_id);
        ASTString str = Expression::cast<Id>(sl->v()[i])->str();
        env.reverseEnum[str] = vdi_id;
        enumItems->addItem(vdi_id);
        if (i == sl->v().size() - 1) {
          // remember the last identifier
          partCardinality.push_back(toEnumArgs[1]);
        }
      }

      std::string name =
          create_enum_to_string_name(ident, "_enum_to_string_" + std::to_string(p) + "_");
      std::vector<Expression*> al_args(sl->v().size());
      for (unsigned int i = 0; i < sl->v().size(); i++) {
        auto str = Expression::cast<Id>(sl->v()[i])->str();
        al_args[i] = new StringLit(Location().introduce(), str);
        /// TODO: reimplement reverseEnum with a symbol table into the model (so you can evalPar an
        /// expression)
      }
      auto* al = new ArrayLit(Location().introduce(), al_args);

      std::vector<TypeInst*> ranges(1);
      ranges[0] = new TypeInst(Location().introduce(), Type::parint());
      auto* ti = new TypeInst(Location().introduce(), Type::parstring(1));
      ti->setRanges(ranges);
      auto* vd_enumToString = new VarDecl(Location().introduce(), ti, name, al);
      enumItems->addItem(VarDeclI::a(Location().introduce(), vd_enumToString));

      Type tx = Type::parint();
      tx.ot(Type::OT_OPTIONAL);
      auto* ti_aa = new TypeInst(Location().introduce(), tx);
      auto* vd_aa = new VarDecl(Location().introduce(), ti_aa, env.genId());
      vd_aa->toplevel(false);
      auto* ti_ab = new TypeInst(Location().introduce(), Type::parbool());
      auto* vd_ab = new VarDecl(Location().introduce(), ti_ab, env.genId());
      vd_ab->toplevel(false);
      auto* ti_aj = new TypeInst(Location().introduce(), Type::parbool());
      auto* vd_aj = new VarDecl(Location().introduce(), ti_aj, env.genId());
      vd_aj->toplevel(false);
      auto* ti_fi = new TypeInst(Location().introduce(), Type::parstring());
      std::vector<VarDecl*> fi_params(3);
      fi_params[0] = vd_aa;
      fi_params[1] = vd_ab;
      fi_params[2] = vd_aj;

      std::vector<Expression*> deopt_args(1);
      deopt_args[0] = vd_aa->id();
      Call* deopt = Call::a(Location().introduce(), env.constants.ids.deopt, deopt_args);
      Call* occurs = Call::a(Location().introduce(), env.constants.ids.occurs, deopt_args);
      std::vector<Expression*> aa_args(1);
      if (prevCardinality == nullptr) {
        aa_args[0] = deopt;
      } else {
        aa_args[0] = new BinOp(Location().introduce(), deopt, BOT_MINUS, prevCardinality);
      }
      auto* aa = new ArrayAccess(Location().introduce(), vd_enumToString->id(), aa_args);

      auto* sl_absent = new StringLit(Location().introduce(), "<>");

      ITE* if_absent = new ITE(
          Location().introduce(),
          {vd_aj->id(), new StringLit(Location().introduce(), ASTString("null"))}, sl_absent);

      auto* json_e_quote = new StringLit(Location().introduce(), ASTString("{\"e\":"));
      auto* json_e_quote_end = new StringLit(Location().introduce(), ASTString("}"));
      auto* quote_aa = new BinOp(Location().introduce(), json_e_quote, BOT_PLUSPLUS,
                                 Call::a(Location().introduce(), env.constants.ids.show, {aa}));
      auto* quote_aa2 = new BinOp(Location().introduce(), quote_aa, BOT_PLUSPLUS, json_e_quote_end);

      Call* quote_dzn = Call::a(Location().introduce(), ASTString("showDznId"), {aa});

      std::vector<Expression*> ite_ifelse(2);
      ite_ifelse[0] = occurs;
      ite_ifelse[1] =
          new ITE(Location().introduce(), {vd_ab->id(), quote_dzn, vd_aj->id(), quote_aa2}, aa);

      ITE* ite = new ITE(Location().introduce(), ite_ifelse, if_absent);

      std::string toString = "_toString_";
      if (parts.size() > 1) {
        toString += std::to_string(p) + "_";
      }

      auto* fi = new FunctionI(Location().introduce(),
                               ASTString(create_enum_to_string_name(ident, toString)), ti_fi,
                               fi_params, ite);
      enumItems->addItem(fi);
    } else if (Call* c = Expression::dynamicCast<Call>(parts[p])) {
      enumConstructorSetTypes.push_back(c);
      if (c->id() == env.constants.ids.anon_enum || c->id() == env.constants.ids.anon_enum_set) {
        Type tx = Type::parint();
        tx.ot(Type::OT_OPTIONAL);
        auto* ti_aa = new TypeInst(Location().introduce(), tx);
        auto* vd_aa = new VarDecl(Location().introduce(), ti_aa, env.genId());
        vd_aa->toplevel(false);

        auto* ti_ab = new TypeInst(Location().introduce(), Type::parbool());
        auto* vd_ab = new VarDecl(Location().introduce(), ti_ab, env.genId());
        vd_ab->toplevel(false);

        auto* ti_aj = new TypeInst(Location().introduce(), Type::parbool());
        auto* vd_aj = new VarDecl(Location().introduce(), ti_aj, env.genId());
        vd_aj->toplevel(false);

        std::vector<Expression*> deopt_args(1);
        deopt_args[0] = vd_aa->id();
        Call* deopt = Call::a(Location().introduce(), env.constants.ids.deopt, deopt_args);
        Call* if_absent = Call::a(Location().introduce(), env.constants.ids.absent, deopt_args);
        auto* sl_absent_dzn = new StringLit(Location().introduce(), "<>");
        ITE* sl_absent = new ITE(
            Location().introduce(),
            {vd_aj->id(), new StringLit(Location().introduce(), ASTString("null"))}, sl_absent_dzn);

        auto* sl_dzn = new StringLit(Location().introduce(), ASTString(std::string("to_enum(") +
                                                                       ident->str().c_str() + ","));

        std::vector<Expression*> showIntArgs(1);
        Expression* enumCard;
        if (c->id() == env.constants.ids.anon_enum) {
          enumCard = c->arg(0);
        } else {
          enumCard = Call::a(Location().introduce(), env.constants.ids.card, {c->arg(0)});
        }
        if (partCardinality.empty()) {
          showIntArgs[0] = deopt;
          partCardinality.push_back(enumCard);
        } else {
          showIntArgs[0] =
              new BinOp(Location().introduce(), partCardinality.back(), BOT_PLUS, deopt);
          partCardinality.push_back(
              new BinOp(Location().introduce(), partCardinality.back(), BOT_PLUS, enumCard));
        }

        Call* showInt = Call::a(Location().introduce(), env.constants.ids.show, showIntArgs);
        auto* construct_string_dzn =
            new BinOp(Location().introduce(), sl_dzn, BOT_PLUSPLUS, showInt);
        auto* closing_bracket = new StringLit(Location().introduce(), ASTString(")"));
        auto* construct_string_dzn_2 =
            new BinOp(Location().introduce(), construct_string_dzn, BOT_PLUSPLUS, closing_bracket);

        auto* sl = new StringLit(Location().introduce(),
                                 ASTString("to_enum(" + std::string(ident->str().c_str()) + ","));
        auto* construct_string0 = new BinOp(Location().introduce(), sl, BOT_PLUSPLUS, showInt);
        auto* construct_string = new BinOp(Location().introduce(), construct_string0, BOT_PLUSPLUS,
                                           new StringLit(Location().introduce(), ")"));

        auto* json_e_quote = new StringLit(Location().introduce(), ASTString("{\"e\":\""));
        auto* json_e_quote_mid = new StringLit(Location().introduce(), ASTString("\", \"i\":"));
        auto* json_e_quote_end = new StringLit(Location().introduce(), ASTString("}"));
        auto* construct_string_json = new BinOp(
            Location().introduce(), json_e_quote, BOT_PLUSPLUS,
            new StringLit(Location().introduce(), Printer::escapeStringLit(ident->str())));
        auto* construct_string_json_1a = new BinOp(Location().introduce(), construct_string_json,
                                                   BOT_PLUSPLUS, json_e_quote_mid);
        auto* construct_string_json_1b =
            new BinOp(Location().introduce(), construct_string_json_1a, BOT_PLUSPLUS, showInt);
        auto* construct_string_json_2 = new BinOp(Location().introduce(), construct_string_json_1b,
                                                  BOT_PLUSPLUS, json_e_quote_end);

        std::vector<Expression*> if_then(6);
        if_then[0] = if_absent;
        if_then[1] = sl_absent;
        if_then[2] = vd_ab->id();
        if_then[3] = construct_string_dzn_2;
        if_then[4] = vd_aj->id();
        if_then[5] = construct_string_json_2;
        ITE* ite = new ITE(Location().introduce(), if_then, construct_string);

        auto* ti_fi = new TypeInst(Location().introduce(), Type::parstring());
        std::vector<VarDecl*> fi_params(3);
        fi_params[0] = vd_aa;
        fi_params[1] = vd_ab;
        fi_params[2] = vd_aj;
        std::string toString = "_toString_";
        if (parts.size() > 1) {
          toString += std::to_string(p) + "_";
        }

        auto* fi = new FunctionI(Location().introduce(),
                                 ASTString(create_enum_to_string_name(ident, toString)), ti_fi,
                                 fi_params, ite);
        enumItems->addItem(fi);
      } else {
        // This is an enum constructor C(E)

        if (c->argCount() != 1) {
          throw TypeError(env, Expression::loc(c), "enum constructors must have a single argument");
        }

        auto* constructorArgId = Expression::dynamicCast<Id>(c->arg(0));
        if (constructorArgId == nullptr) {
          // expression is not an identifer, create new VarDecl for the argument
          std::ostringstream constructorArgIdent;
          constructorArgIdent << "_constrId_" << p << "_" << *ident;
          Call* enumOf = Call::a(Location().introduce(), ASTString("enum_of"), {c->arg(0)});
          Type t;
          t.st(Type::ST_SET);
          auto* constructorArgVdTi = new TypeInst(Location().introduce(), t, enumOf);
          auto* constructorArgVd = new VarDecl(Location().introduce(), constructorArgVdTi,
                                               constructorArgIdent.str(), c->arg(0));

          enumItems->addItem(VarDeclI::a(Location().introduce(), constructorArgVd));
          constructorArgId = constructorArgVd->id();
        }

        // Compute minimum-1 of constructor argument
        Id* constructorArgMin;
        {
          auto* min =
              Call::a(Location().introduce(), ASTString("mzn_min_or_0"), {constructorArgId});
          Expression* prevCard = partCardinality.empty() ? IntLit::a(0) : partCardinality.back();
          auto* minMinusOne =
              new BinOp(Location().introduce(), prevCard, BOT_MINUS,
                        new BinOp(Location().introduce(), min, BOT_MINUS, IntLit::a(1)));
          std::ostringstream constructorArgMinIdent;
          constructorArgMinIdent << "_constrMin_" << p << "_" << *ident;
          auto* constructorArgMinVd = new VarDecl(
              Location().introduce(), new TypeInst(Location().introduce(), Type::parint(), nullptr),
              constructorArgMinIdent.str(), minMinusOne);
          enumItems->addItem(VarDeclI::a(Location().introduce(), constructorArgMinVd));
          constructorArgMin = constructorArgMinVd->id();
        }

        // Generate (both par and var versions):
        /*
         function X: C(E: x) =
             if mzn_set_is_contiguous(E) then
               to_enum(X,partCardinality.back()+x)
             else
               (let { any: mx = set_sparse_inverse(E) } in
               to_enum(X,partCardinality.back()+mx[x]))::mzn_evaluate_once
             endif ::mzn_evaluate_once
         function opt X: C(opt E: x) = if occurs(x) then C(deopt(x)) else to_enum(x,<>) endif
         function set of X: C(set of E: x) = { C(i) | i in x }
         function set of X: C() = C(E);

         function E: C⁻¹(X: x) =
           if mzn_set_is_contiguous(E) then
             to_enum(E,x-partCardinality.back())
           else
             (let { any: mx = set2array(E) } in
             to_enum(X,mx[x-partCardinality.back()]))::mzn_evaluate_once
           endif ::mzn_evaluate_once

         function opt E: C⁻¹(opt X: x) = if occurs(x) then C⁻¹(deopt(x)) else to_enum(x,<>) endif
         function set of E: C⁻¹(set of X: x) = { C⁻¹(i) | i in x }
         */

        {
          Type Xt(Type::parint());
          Xt.st(Type::ST_SET);
          Xt.typeId(enumId);
          auto* Cfn_ti = new TypeInst(Location().introduce(), Xt);
          ASTString Cfn_id = c->id();
          auto* inv = Call::a(Location().introduce(), Cfn_id, {constructorArgId});
          auto* Cfn = new FunctionI(Location().introduce(), Cfn_id, Cfn_ti, {}, inv);
          enumItems->addItem(Cfn);
        }

        for (Type baseType : {Type::parint(), Type::varint()}) {
          {
            Type Xt(baseType);
            Xt.typeId(enumId);
            auto* Cfn_ti = new TypeInst(Location().introduce(), Xt);
            Type argT;
            argT.ti(baseType.ti());
            auto* Cfn_x_ti = new TypeInst(Location().introduce(), argT, constructorArgId);
            auto* vd_x = new VarDecl(Location().introduce(), Cfn_x_ti, env.genId());
            vd_x->toplevel(false);

            auto* isContiguous = Call::a(Location().introduce(), ASTString("mzn_set_is_contiguous"),
                                         {constructorArgId});

            Expression* realX =
                new BinOp(Location().introduce(), constructorArgMin, BOT_PLUS, vd_x->id());
            auto* Cfn_then = Call::a(Location().introduce(), "to_enum", {vd->id(), realX});

            auto* mxti = new TypeInst(Location().introduce(), Type::mkAny());
            auto* sparse_inv = Call::a(Location().introduce(), ASTString("set_to_sparse_inverse"),
                                       {constructorArgId});
            auto* mx = new VarDecl(Location().introduce(), mxti, env.genId(), sparse_inv);

            auto* mxx = new ArrayAccess(Location().introduce(), mx->id(), {vd_x->id()});
            Expression* realMx =
                new BinOp(Location().introduce(), constructorArgMin, BOT_PLUS, mxx);
            auto* let_body = Call::a(Location().introduce(), "to_enum", {vd->id(), realMx});

            auto* Cfn_else = new Let(Location().introduce(), {mx}, let_body);
            Expression::addAnnotation(Cfn_else, env.constants.ann.mzn_evaluate_once);

            auto* ite = new ITE(Location().introduce(), {isContiguous, Cfn_then}, Cfn_else);
            Expression::addAnnotation(ite, env.constants.ann.mzn_evaluate_once);

            ASTString Cfn_id = c->id();
            auto* Cfn = new FunctionI(Location().introduce(), Cfn_id, Cfn_ti, {vd_x}, ite);
            env.reverseEnum[Cfn_id] = Cfn;
            enumItems->addItem(Cfn);
          }
          {
            Type Xt(baseType);
            Xt.ot(Type::OT_OPTIONAL);
            Xt.typeId(enumId);
            auto* Cfn_ti = new TypeInst(Location().introduce(), Xt);
            Type argT;
            argT.ti(baseType.ti());
            argT.ot(Type::OT_OPTIONAL);
            auto* Cfn_x_ti = new TypeInst(Location().introduce(), argT, constructorArgId);
            auto* vd_x = new VarDecl(Location().introduce(), Cfn_x_ti, env.genId());
            ASTString Cfn_id = c->id();
            vd_x->toplevel(false);
            auto* occurs = Call::a(Location().introduce(), env.constants.ids.occurs, {vd_x->id()});
            auto* deopt = Call::a(Location().introduce(), env.constants.ids.deopt, {vd_x->id()});
            auto* inv = Call::a(Location().introduce(), Cfn_id, {deopt});
            auto* toEnumAbsent =
                Call::a(Location().introduce(), "to_enum", {vd->id(), env.constants.absent});
            auto* ite = new ITE(Location().introduce(), {occurs, inv}, toEnumAbsent);
            auto* Cfn = new FunctionI(Location().introduce(), Cfn_id, Cfn_ti, {vd_x}, ite);
            enumItems->addItem(Cfn);
          }
          {
            Type Xt(baseType);
            Xt.st(Type::ST_SET);
            Xt.typeId(enumId);
            auto* Cfn_ti = new TypeInst(Location().introduce(), Xt);
            Type argT;
            argT.ti(baseType.ti());
            argT.st(Type::ST_SET);
            auto* Cfn_x_ti = new TypeInst(Location().introduce(), argT, constructorArgId);
            auto* vd_x = new VarDecl(Location().introduce(), Cfn_x_ti, env.genId());
            ASTString Cfn_id = c->id();
            vd_x->toplevel(false);
            auto* s_ti = new TypeInst(Location().introduce(), Type::parint());
            auto* s = new VarDecl(Location().introduce(), s_ti, env.genId());
            s->toplevel(false);
            auto* inv = Call::a(Location().introduce(), Cfn_id, {s->id()});
            Generator gen({s}, vd_x->id(), nullptr);
            Generators gens;
            gens.g = {gen};
            auto* comprehension = new Comprehension(Location().introduce(), inv, gens, true);
            auto* Cfn =
                new FunctionI(Location().introduce(), Cfn_id, Cfn_ti, {vd_x}, comprehension);
            enumItems->addItem(Cfn);
          }
          {
            Type rT;
            rT.ti(baseType.ti());
            rT.typeId(constructorArgId->type().typeId());
            auto* toEfn_ti = new TypeInst(Location().introduce(), rT, constructorArgId);
            Type Xt(baseType);
            Xt.typeId(enumId);
            auto* toEfn_x_ti = new TypeInst(Location().introduce(), Xt, vd->id());
            auto* vd_x = new VarDecl(Location().introduce(), toEfn_x_ti, env.genId());
            vd_x->toplevel(false);

            auto* isContiguous = Call::a(Location().introduce(), ASTString("mzn_set_is_contiguous"),
                                         {constructorArgId});

            Expression* realX =
                new BinOp(Location().introduce(), vd_x->id(), BOT_MINUS, constructorArgMin);
            auto* toEfn_then =
                Call::a(Location().introduce(), "to_enum", {constructorArgId, realX});

            auto* mxti = new TypeInst(Location().introduce(), Type::mkAny());
            auto* sparse_inv =
                Call::a(Location().introduce(), ASTString("set2array"), {constructorArgId});
            auto* mx = new VarDecl(Location().introduce(), mxti, env.genId(), sparse_inv);

            Expression* realMx;
            if (partCardinality.empty()) {
              realMx = vd_x->id();
            } else {
              realMx =
                  new BinOp(Location().introduce(), vd_x->id(), BOT_MINUS, partCardinality.back());
            }
            auto* mxx = new ArrayAccess(Location().introduce(), mx->id(), {realMx});
            auto* let_body = Call::a(Location().introduce(), "to_enum", {constructorArgId, mxx});

            auto* toEfn_else = new Let(Location().introduce(), {mx}, let_body);
            Expression::addAnnotation(toEfn_else, env.constants.ann.mzn_evaluate_once);

            auto* ite = new ITE(Location().introduce(), {isContiguous, toEfn_then}, toEfn_else);
            Expression::addAnnotation(ite, env.constants.ann.mzn_evaluate_once);

            ASTString Cinv_id(std::string(c->id().c_str()) + "⁻¹");
            auto* toEfn = new FunctionI(Location().introduce(), Cinv_id, toEfn_ti, {vd_x}, ite);
            enumItems->addItem(toEfn);
          }
          {
            Type rt;
            rt.ti(baseType.ti());
            rt.ot(Type::OT_OPTIONAL);
            auto* Cfn_ti = new TypeInst(Location().introduce(), rt, constructorArgId);
            Type argT(baseType);
            argT.ot(Type::OT_OPTIONAL);
            argT.typeId(enumId);
            auto* Cfn_x_ti = new TypeInst(Location().introduce(), argT, vd->id());
            auto* vd_x = new VarDecl(Location().introduce(), Cfn_x_ti, env.genId());
            ASTString Cinv_id(std::string(c->id().c_str()) + "⁻¹");
            vd_x->toplevel(false);
            auto* occurs = Call::a(Location().introduce(), env.constants.ids.occurs, {vd_x->id()});
            auto* deopt = Call::a(Location().introduce(), env.constants.ids.deopt, {vd_x->id()});
            auto* inv = Call::a(Location().introduce(), Cinv_id, {deopt});
            auto* toEnumAbsent = Call::a(Location().introduce(), "to_enum",
                                         {constructorArgId, env.constants.absent});
            auto* ite = new ITE(Location().introduce(), {occurs, inv}, toEnumAbsent);
            auto* Cfn = new FunctionI(Location().introduce(), Cinv_id, Cfn_ti, {vd_x}, ite);
            enumItems->addItem(Cfn);
          }
          {
            Type Xt;
            Xt.ti(baseType.ti());
            Xt.st(Type::ST_SET);
            auto* Cfn_ti = new TypeInst(Location().introduce(), Xt, constructorArgId);
            Type argT(baseType);
            argT.st(Type::ST_SET);
            argT.typeId(enumId);
            auto* Cfn_x_ti = new TypeInst(Location().introduce(), argT, vd->id());
            auto* vd_x = new VarDecl(Location().introduce(), Cfn_x_ti, env.genId());
            vd_x->toplevel(false);
            ASTString Cinv_id(std::string(c->id().c_str()) + "⁻¹");
            auto* s_ti = new TypeInst(Location().introduce(), Type::parint());
            auto* s = new VarDecl(Location().introduce(), s_ti, env.genId());
            s->toplevel(false);
            auto* inv = Call::a(Location().introduce(), Cinv_id, {s->id()});
            Generator gen({s}, vd_x->id(), nullptr);
            Generators gens;
            gens.g = {gen};
            auto* comprehension = new Comprehension(Location().introduce(), inv, gens, true);
            auto* Cfn =
                new FunctionI(Location().introduce(), Cinv_id, Cfn_ti, {vd_x}, comprehension);
            enumItems->addItem(Cfn);
          }
        }

        /*
         function string: _toString_p_X(opt X: x, bool: b, bool: json) =
           if absent(x) then "<>" else
           if json then "{ \"c\": \"C\", \"e\":" else "C(" endif
           ++_toString_E(to_enum(E,deopt(x)),b,json)
           ++ if json then "}" else ")" endif
           endif
         */

        {
          Type tx = Type::parint();
          tx.typeId(enumId);
          tx.ot(Type::OT_OPTIONAL);
          auto* ti_aa = new TypeInst(Location().introduce(), tx, vd->id());
          auto* vd_aa = new VarDecl(Location().introduce(), ti_aa, env.genId());
          vd_aa->toplevel(false);

          auto* ti_ab = new TypeInst(Location().introduce(), Type::parbool());
          auto* vd_ab = new VarDecl(Location().introduce(), ti_ab, env.genId());
          vd_ab->toplevel(false);

          auto* ti_aj = new TypeInst(Location().introduce(), Type::parbool());
          auto* vd_aj = new VarDecl(Location().introduce(), ti_aj, env.genId());
          vd_aj->toplevel(false);

          std::string Cinv_id(std::string(c->id().c_str()) + "⁻¹");
          Call* invCall = Call::a(Location().introduce(), Cinv_id, {vd_aa->id()});

          Call* if_absent = Call::a(Location().introduce(), "absent", {vd_aa->id()});
          auto* sl_absent_dzn = new StringLit(Location().introduce(), "<>");
          ITE* sl_absent =
              new ITE(Location().introduce(),
                      {vd_aj->id(), new StringLit(Location().introduce(), ASTString("null"))},
                      sl_absent_dzn);

          needToString.insert(constructorArgId, true);
          Call* toString = Call::a(Location().introduce(),
                                   create_enum_to_string_name(constructorArgId, "_toString_"),
                                   {invCall, vd_ab->id(), vd_aj->id()});
          auto* c_quoted = Call::a(Location().introduce(), "showDznId",
                                   {new StringLit(Location().introduce(), c->id())});
          auto* c_ident = new ITE(Location().introduce(), {vd_ab->id(), c_quoted},
                                  new StringLit(Location().introduce(), c->id()));
          auto* openOther = new BinOp(Location().introduce(), c_ident, BOT_PLUSPLUS,
                                      new StringLit(Location().introduce(), "("));
          auto* openJson =
              new StringLit(Location().introduce(),
                            "{ \"c\" : \"" + Printer::escapeStringLit(c->id()) + "\", \"e\" : ");
          ITE* openConstr = new ITE(Location().introduce(), {vd_aj->id(), openJson}, openOther);
          auto* closeJson = new StringLit(Location().introduce(), "}");
          auto* closeOther = new StringLit(Location().introduce(), ")");
          ITE* closeConstr = new ITE(Location().introduce(), {vd_aj->id(), closeJson}, closeOther);

          auto* concat1 = new BinOp(Location().introduce(), openConstr, BOT_PLUSPLUS, toString);
          auto* concat2 = new BinOp(Location().introduce(), concat1, BOT_PLUSPLUS, closeConstr);

          ITE* ite = new ITE(Location().introduce(), {if_absent, sl_absent}, concat2);
          auto* ti_fi = new TypeInst(Location().introduce(), Type::parstring());
          std::vector<VarDecl*> fi_params(3);
          fi_params[0] = vd_aa;
          fi_params[1] = vd_ab;
          fi_params[2] = vd_aj;
          std::string XtoString = "_toString_";
          if (parts.size() > 1) {
            XtoString += std::to_string(p) + "_";
          }

          auto* fi = new FunctionI(Location().introduce(),
                                   ASTString(create_enum_to_string_name(ident, XtoString)), ti_fi,
                                   fi_params, ite);
          enumItems->addItem(fi);
        }

        Call* cardE = Call::a(Location().introduce(), "card", {constructorArgId});
        if (partCardinality.empty()) {
          partCardinality.push_back(cardE);
        } else {
          partCardinality.push_back(
              new BinOp(Location().introduce(), partCardinality.back(), BOT_PLUS, cardE));
        }
      }
    } else {
      assert(false);
    }
  }

  // Create set literal for overall enum
  Expression* upperBound;
  if (!partCardinality.empty()) {
    upperBound = partCardinality.back();
  } else {
    // For empty enums, just create 1..0.
    upperBound = IntLit::a(0);
  }
  auto* rhs = new BinOp(Expression::loc(vd), IntLit::a(1), BOT_DOTDOT, upperBound);
  vd->e(rhs);

  if (parts.size() > 1) {
    Type tx = Type::parint();
    tx.ot(Type::OT_OPTIONAL);
    tx.typeId(enumId);
    auto* ti_aa = new TypeInst(Location().introduce(), tx, vd->id());
    auto* vd_aa = new VarDecl(Location().introduce(), ti_aa, env.genId());
    vd_aa->toplevel(false);

    auto* ti_ab = new TypeInst(Location().introduce(), Type::parbool());
    auto* vd_ab = new VarDecl(Location().introduce(), ti_ab, env.genId());
    vd_ab->toplevel(false);

    auto* ti_aj = new TypeInst(Location().introduce(), Type::parbool());
    auto* vd_aj = new VarDecl(Location().introduce(), ti_aj, env.genId());
    vd_aj->toplevel(false);

    std::vector<Expression*> deopt_args(1);
    deopt_args[0] = vd_aa->id();
    Call* deopt = Call::a(Location().introduce(), env.constants.ids.deopt, deopt_args);
    Call* if_absent = Call::a(Location().introduce(), "absent", deopt_args);
    auto* sl_absent_dzn = new StringLit(Location().introduce(), "<>");
    ITE* sl_absent = new ITE(
        Location().introduce(),
        {vd_aj->id(), new StringLit(Location().introduce(), ASTString("null"))}, sl_absent_dzn);

    std::vector<Expression*> ite_cases_a;
    Expression* ite_cases_else;
    for (unsigned int i = 0; i < parts.size(); i++) {
      std::string toString = "_toString_" + std::to_string(i) + "_";
      Call* c = Call::a(Location().introduce(), create_enum_to_string_name(ident, toString),
                        {vd_aa->id(), vd_ab->id(), vd_aj->id()});
      if (i < parts.size() - 1) {
        auto* bo = new BinOp(Location().introduce(), deopt, BOT_LQ, partCardinality[i]);
        ite_cases_a.push_back(bo);
        ite_cases_a.push_back(c);
      } else {
        ite_cases_else = c;
      }
    }

    ITE* ite_cases = new ITE(Location().introduce(), ite_cases_a, ite_cases_else);

    ITE* ite = new ITE(Location().introduce(), {if_absent, sl_absent}, ite_cases);

    auto* ti_fi = new TypeInst(Location().introduce(), Type::parstring());
    std::vector<VarDecl*> fi_params(3);
    fi_params[0] = vd_aa;
    fi_params[1] = vd_ab;
    fi_params[2] = vd_aj;
    auto* fi = new FunctionI(Location().introduce(),
                             ASTString(create_enum_to_string_name(ident, "_toString_")), ti_fi,
                             fi_params, ite);
    enumItems->addItem(fi);

    /*
     function string: _toString_ENUM(opt Foo: x, bool: b, bool: json) =
       if occurs(x) then
         if deopt(x)<=partCardinality[1] then _toString_1_ENUM(x,b,json)
         elseif deopt(x)<=partCardinality[2] then _toString_2_ENUM(x,b,json)
         ...
         endif
       else "<>" endif
     */
  }

  {
    /*

     function _toString_ENUM(array[$U] of opt Foo: x, bool: b, bool: json) =
       let {
         array[int] of opt ENUM: xx = array1d(x)
       } in "[" ++ join(", ", [ _toString_ENUM(xx[i],b,json) | i in index_set(xx) ]) ++ "]";

     */

    TIId* tiid = new TIId(Location().introduce(), "U");
    auto* ti_range = new TypeInst(Location().introduce(), Type::parint(), tiid);
    std::vector<TypeInst*> ranges(1);
    ranges[0] = ti_range;

    Type tx = Type::parint(-1);
    tx.ot(Type::OT_OPTIONAL);
    auto* x_ti = new TypeInst(Location().introduce(), tx, ranges, ident);
    auto* vd_x = new VarDecl(Location().introduce(), x_ti, env.genId());
    vd_x->toplevel(false);

    auto* b_ti = new TypeInst(Location().introduce(), Type::parbool());
    auto* vd_b = new VarDecl(Location().introduce(), b_ti, env.genId());
    vd_b->toplevel(false);

    auto* j_ti = new TypeInst(Location().introduce(), Type::parbool());
    auto* vd_j = new VarDecl(Location().introduce(), j_ti, env.genId());
    vd_j->toplevel(false);

    auto* xx_range = new TypeInst(Location().introduce(), Type::parint(), nullptr);
    std::vector<TypeInst*> xx_ranges(1);
    xx_ranges[0] = xx_range;
    auto* xx_ti = new TypeInst(Location().introduce(), tx, xx_ranges, ident);

    std::vector<Expression*> array1dArgs(1);
    array1dArgs[0] = vd_x->id();
    Call* array1dCall = Call::a(Location().introduce(), env.constants.ids.array1d, array1dArgs);

    auto* vd_xx = new VarDecl(Location().introduce(), xx_ti, env.genId(), array1dCall);
    vd_xx->toplevel(false);

    auto* idx_i_ti = new TypeInst(Location().introduce(), Type::parint());
    auto* idx_i = new VarDecl(Location().introduce(), idx_i_ti, env.genId());
    idx_i->toplevel(false);

    std::vector<Expression*> aa_xxi_idx(1);
    aa_xxi_idx[0] = idx_i->id();
    auto* aa_xxi = new ArrayAccess(Location().introduce(), vd_xx->id(), aa_xxi_idx);

    std::vector<Expression*> _toString_ENUMArgs(3);
    _toString_ENUMArgs[0] = aa_xxi;
    _toString_ENUMArgs[1] = vd_b->id();
    _toString_ENUMArgs[2] = vd_j->id();
    Call* _toString_ENUM =
        Call::a(Location().introduce(), create_enum_to_string_name(ident, "_toString_"),
                _toString_ENUMArgs);

    std::vector<Expression*> index_set_xx_args(1);
    index_set_xx_args[0] = vd_xx->id();
    Call* index_set_xx = Call::a(Location().introduce(), "index_set", index_set_xx_args);
    std::vector<VarDecl*> gen_exps(1);
    gen_exps[0] = idx_i;
    Generator gen(gen_exps, index_set_xx, nullptr);

    Generators generators;
    generators.g.push_back(gen);
    auto* comp = new Comprehension(Location().introduce(), _toString_ENUM, generators, false);

    std::vector<Expression*> join_args(2);
    join_args[0] = new StringLit(Location().introduce(), ", ");
    join_args[1] = comp;
    Call* join = Call::a(Location().introduce(), "join", join_args);

    auto* sl_open = new StringLit(Location().introduce(), "[");
    auto* bopp0 = new BinOp(Location().introduce(), sl_open, BOT_PLUSPLUS, join);
    auto* sl_close = new StringLit(Location().introduce(), "]");
    auto* bopp1 = new BinOp(Location().introduce(), bopp0, BOT_PLUSPLUS, sl_close);

    std::vector<Expression*> let_args(1);
    let_args[0] = vd_xx;
    Let* let = new Let(Location().introduce(), let_args, bopp1);

    auto* ti_fi = new TypeInst(Location().introduce(), Type::parstring());
    std::vector<VarDecl*> fi_params(3);
    fi_params[0] = vd_x;
    fi_params[1] = vd_b;
    fi_params[2] = vd_j;
    auto* fi = new FunctionI(Location().introduce(),
                             ASTString(create_enum_to_string_name(ident, "_toString_")), ti_fi,
                             fi_params, let);
    enumItems->addItem(fi);
  }

  {
    /*

     function _toString_ENUM(opt set of ENUM: x, bool: b, bool: json) =
       if absent(x) then
         "<>"
       else
         "{" ++ join(", ", [ _toString_ENUM(i,b,json) | i in deopt(x) ]) ++ "}"
       endif

     */

    Type argType = Type::parsetenum(ident->type().typeId());
    argType.ot(Type::OT_OPTIONAL);
    auto* x_ti = new TypeInst(Location().introduce(), argType, ident);
    auto* vd_x = new VarDecl(Location().introduce(), x_ti, env.genId());
    vd_x->toplevel(false);

    auto* b_ti = new TypeInst(Location().introduce(), Type::parbool());
    auto* vd_b = new VarDecl(Location().introduce(), b_ti, env.genId());
    vd_b->toplevel(false);

    auto* j_ti = new TypeInst(Location().introduce(), Type::parbool());
    auto* vd_j = new VarDecl(Location().introduce(), j_ti, env.genId());
    vd_j->toplevel(false);

    std::vector<Expression*> deopt_args(1);
    deopt_args[0] = vd_x->id();
    Call* deopt = Call::a(Location().introduce(), env.constants.ids.deopt, deopt_args);
    Call* if_absent = Call::a(Location().introduce(), "absent", deopt_args);
    auto* sl_absent_dzn = new StringLit(Location().introduce(), "<>");
    ITE* sl_absent = new ITE(Location().introduce(),
                             {vd_j->id(), new StringLit(Location().introduce(), ASTString("null"))},
                             sl_absent_dzn);

    auto* idx_i_ti = new TypeInst(Location().introduce(), Type::parint());
    auto* idx_i = new VarDecl(Location().introduce(), idx_i_ti, env.genId());
    idx_i->toplevel(false);

    std::vector<Expression*> _toString_ENUMArgs(3);
    _toString_ENUMArgs[0] = idx_i->id();
    _toString_ENUMArgs[1] = vd_b->id();
    _toString_ENUMArgs[2] = vd_j->id();
    Call* _toString_ENUM =
        Call::a(Location().introduce(), create_enum_to_string_name(ident, "_toString_"),
                _toString_ENUMArgs);

    std::vector<VarDecl*> gen_exps(1);
    gen_exps[0] = idx_i;
    Generator gen(gen_exps, deopt, nullptr);

    Generators generators;
    generators.g.push_back(gen);
    auto* comp = new Comprehension(Location().introduce(), _toString_ENUM, generators, false);

    std::vector<Expression*> join_args(2);
    join_args[0] = new StringLit(Location().introduce(), ", ");
    join_args[1] = comp;
    Call* join = Call::a(Location().introduce(), "join", join_args);

    ITE* json_set =
        new ITE(Location().introduce(),
                {vd_j->id(), new StringLit(Location().introduce(), ASTString("\"set\":["))},
                new StringLit(Location().introduce(), ASTString("")));
    ITE* json_set_close = new ITE(
        Location().introduce(), {vd_j->id(), new StringLit(Location().introduce(), ASTString("]"))},
        new StringLit(Location().introduce(), ASTString("")));

    auto* sl_open = new StringLit(Location().introduce(), "{");
    auto* bopp0 = new BinOp(Location().introduce(), sl_open, BOT_PLUSPLUS, json_set);
    auto* bopp1 = new BinOp(Location().introduce(), bopp0, BOT_PLUSPLUS, join);
    auto* bopp2 = new BinOp(Location().introduce(), bopp1, BOT_PLUSPLUS, json_set_close);
    auto* sl_close = new StringLit(Location().introduce(), "}");
    auto* bopp3 = new BinOp(Location().introduce(), bopp2, BOT_PLUSPLUS, sl_close);
    ITE* ite = new ITE(Location().introduce(), {if_absent, sl_absent}, bopp3);

    auto* ti_fi = new TypeInst(Location().introduce(), Type::parstring());
    std::vector<VarDecl*> fi_params(3);
    fi_params[0] = vd_x;
    fi_params[1] = vd_b;
    fi_params[2] = vd_j;
    auto* fi = new FunctionI(Location().introduce(),
                             ASTString(create_enum_to_string_name(ident, "_toString_")), ti_fi,
                             fi_params, ite);
    enumItems->addItem(fi);
  }

  {
    /*

     function _toString_ENUM(array[$U] of opt set of ENUM: x, bool: b, bool: json) =
       "[" ++ join(", ", [ _toString_ENUM(x_i,b,json) | x_i in x ]) ++ "]";

     */

    TIId* tiid = new TIId(Location().introduce(), "U");
    auto* ti_range = new TypeInst(Location().introduce(), Type::parint(), tiid);
    std::vector<TypeInst*> ranges(1);
    ranges[0] = ti_range;

    Type tx = Type::parsetint(-1);
    tx.ot(Type::OT_OPTIONAL);
    auto* x_ti = new TypeInst(Location().introduce(), tx, ranges, ident);
    auto* vd_x = new VarDecl(Location().introduce(), x_ti, env.genId());
    vd_x->toplevel(false);

    auto* b_ti = new TypeInst(Location().introduce(), Type::parbool());
    auto* vd_b = new VarDecl(Location().introduce(), b_ti, env.genId());
    vd_b->toplevel(false);

    auto* j_ti = new TypeInst(Location().introduce(), Type::parbool());
    auto* vd_j = new VarDecl(Location().introduce(), j_ti, env.genId());
    vd_j->toplevel(false);

    auto* idx_xi_ti = new TypeInst(Location().introduce(), tx);
    auto* idx_xi = new VarDecl(Location().introduce(), idx_xi_ti, env.genId());
    idx_xi->toplevel(false);

    std::vector<Expression*> _toString_ENUMArgs(3);
    _toString_ENUMArgs[0] = idx_xi->id();
    _toString_ENUMArgs[1] = vd_b->id();
    _toString_ENUMArgs[2] = vd_j->id();
    Call* _toString_ENUM =
        Call::a(Location().introduce(), create_enum_to_string_name(ident, "_toString_"),
                _toString_ENUMArgs);

    std::vector<VarDecl*> gen_exps(1);
    gen_exps[0] = idx_xi;
    Generator gen(gen_exps, vd_x->id(), nullptr);

    Generators generators;
    generators.g.push_back(gen);
    auto* comp = new Comprehension(Location().introduce(), _toString_ENUM, generators, false);

    std::vector<Expression*> join_args(2);
    join_args[0] = new StringLit(Location().introduce(), ", ");
    join_args[1] = comp;
    Call* join = Call::a(Location().introduce(), "join", join_args);

    auto* sl_open = new StringLit(Location().introduce(), "[");
    auto* bopp0 = new BinOp(Location().introduce(), sl_open, BOT_PLUSPLUS, join);
    auto* sl_close = new StringLit(Location().introduce(), "]");
    auto* bopp1 = new BinOp(Location().introduce(), bopp0, BOT_PLUSPLUS, sl_close);

    auto* ti_fi = new TypeInst(Location().introduce(), Type::parstring());
    std::vector<VarDecl*> fi_params(3);
    fi_params[0] = vd_x;
    fi_params[1] = vd_b;
    fi_params[2] = vd_j;
    auto* fi = new FunctionI(Location().introduce(),
                             ASTString(create_enum_to_string_name(ident, "_toString_")), ti_fi,
                             fi_params, bopp1);
    enumItems->addItem(fi);
  }
}

void TopoSorter::add(EnvI& env, VarDeclI* vdi, bool handleEnums, Model* enumItems) {
  VarDecl* vd = vdi->e();
  if (handleEnums && vd->ti() != nullptr && vd->ti()->isEnum()) {
    unsigned int enumId = env.registerEnum(vdi);
    Type vdt = vd->type();
    vdt.typeId(enumId);
    vd->ti()->type(vdt);
    vd->type(vdt);

    create_enum_mapper(env, model, enumId, vd, enumItems, needToString, enumConstructorSetTypes);
  }
  scopes.add(env, vd);
}

VarDecl* TopoSorter::get(EnvI& env, const ASTString& id_v, const Location& loc) {
  GCLock lock;
  Id* ident = new Id(Location(), id_v, nullptr);
  VarDecl* decl = scopes.find(ident);
  if (decl == nullptr) {
    std::ostringstream ss;
    ss << "undefined identifier `" << ident->str() << "'";
    VarDecl* similar = scopes.findSimilar(ident);
    if (similar != nullptr) {
      ss << ", did you mean `" << *similar->id() << "'?";
    }
    throw TypeError(env, loc, ss.str());
  }
  return decl;
}

VarDecl* TopoSorter::checkId(EnvI& env, Id* ident, const Location& loc) {
  VarDecl* decl = scopes.find(ident);
  if (decl == nullptr) {
    std::ostringstream ss;
    ss << "undefined identifier `" << ident->str() << "'";
    VarDecl* similar = scopes.findSimilar(ident);
    if (similar != nullptr) {
      ss << ", did you mean `" << *similar->id() << "'?";
    }
    throw TypeError(env, loc, ss.str());
  }
  auto pi = pos.find(decl);
  if (pi == pos.end()) {
    // new id
    scopes.pushToplevel();
    run(env, decl);
    scopes.pop();
  } else {
    // previously seen, check if circular
    if (pi->second == -1) {
      std::ostringstream ss;
      ss << "circular definition of `" << ident->str() << "'";
      throw TypeError(env, loc, ss.str());
    }
  }
  return decl;
}

void TopoSorter::run(EnvI& env, Expression* e) {
  if (e == nullptr) {
    return;
  }
  if (Expression::eid(e) != Expression::E_VARDECL) {
    for (auto it = Expression::ann(e).begin(); it != Expression::ann(e).end(); ++it) {
      run(env, *it);
    }
  }
  switch (Expression::eid(e)) {
    case Expression::E_INTLIT:
    case Expression::E_FLOATLIT:
    case Expression::E_BOOLLIT:
    case Expression::E_STRINGLIT:
    case Expression::E_ANON:
      break;
    case Expression::E_SETLIT: {
      auto* sl = Expression::cast<SetLit>(e);
      if (sl->isv() == nullptr && sl->fsv() == nullptr) {
        for (unsigned int i = 0; i < sl->v().size(); i++) {
          run(env, sl->v()[i]);
        }
      }
    } break;
    case Expression::E_ID: {
      if (e != env.constants.absent) {
        VarDecl* vd = checkId(env, Expression::cast<Id>(e), Expression::loc(e));
        Expression::cast<Id>(e)->decl(vd);
      }
    } break;
    case Expression::E_ARRAYLIT: {
      auto* al = Expression::cast<ArrayLit>(e);
      for (unsigned int i = 0; i < al->size(); i++) {
        run(env, (*al)[i]);
      }
    } break;
    case Expression::E_ARRAYACCESS: {
      auto* ae = Expression::cast<ArrayAccess>(e);
      run(env, ae->v());
      for (unsigned int i = 0; i < ae->idx().size(); i++) {
        run(env, ae->idx()[i]);
      }
    } break;
    case Expression::E_FIELDACCESS: {
      auto* fa = Expression::cast<FieldAccess>(e);
      run(env, fa->v());
      // IGNORE fa->field(), must be IntLit or field identifier (checked later)
    } break;
    case Expression::E_COMP: {
      auto* ce = Expression::cast<Comprehension>(e);
      scopes.push();
      for (unsigned int i = 0; i < ce->numberOfGenerators(); i++) {
        run(env, ce->in(i));
        for (unsigned int j = 0; j < ce->numberOfDecls(i); j++) {
          run(env, ce->decl(i, j));
          scopes.add(env, ce->decl(i, j));
        }
        if (ce->where(i) != nullptr) {
          run(env, ce->where(i));
        }
      }
      run(env, ce->e());
      scopes.pop();
    } break;
    case Expression::E_ITE: {
      ITE* ite = Expression::cast<ITE>(e);
      for (unsigned int i = 0; i < ite->size(); i++) {
        run(env, ite->ifExpr(i));
        run(env, ite->thenExpr(i));
      }
      run(env, ite->elseExpr());
    } break;
    case Expression::E_BINOP: {
      auto* be = Expression::cast<BinOp>(e);
      std::vector<Expression*> todo;
      todo.push_back(be->lhs());
      todo.push_back(be->rhs());
      while (!todo.empty()) {
        Expression* be = todo.back();
        todo.pop_back();
        if (auto* e_bo = Expression::dynamicCast<BinOp>(be)) {
          todo.push_back(e_bo->lhs());
          todo.push_back(e_bo->rhs());
          for (ExpressionSetIter it = Expression::ann(e_bo).begin();
               it != Expression::ann(e_bo).end(); ++it) {
            run(env, *it);
          }
        } else {
          run(env, be);
        }
      }
    } break;
    case Expression::E_UNOP: {
      UnOp* ue = Expression::cast<UnOp>(e);
      run(env, ue->e());
    } break;
    case Expression::E_CALL: {
      Call* ce = Expression::cast<Call>(e);
      for (unsigned int i = 0; i < ce->argCount(); i++) {
        run(env, ce->arg(i));
      }
    } break;
    case Expression::E_VARDECL: {
      auto* ve = Expression::cast<VarDecl>(e);
      auto pi = pos.find(ve);
      if (pi == pos.end()) {
        pos.insert(std::pair<VarDecl*, int>(ve, -1));
        run(env, ve->ti());
        run(env, ve->e());
        for (auto it = Expression::ann(e).begin(); it != Expression::ann(e).end(); ++it) {
          run(env, *it);
        }
        ve->payload(static_cast<int>(decls.size()));
        decls.emplace_back(ve);
        pi = pos.find(ve);
        pi->second = static_cast<int>(decls.size()) - 1;
      } else {
        assert(pi->second != -1);
      }
    } break;
    case Expression::E_TI: {
      auto* ti = Expression::cast<TypeInst>(e);
      for (unsigned int i = 0; i < ti->ranges().size(); i++) {
        run(env, ti->ranges()[i]);
      }
      run(env, ti->domain());
    } break;
    case Expression::E_TIID:
      break;
    case Expression::E_LET: {
      Let* let = Expression::cast<Let>(e);
      scopes.push();
      for (unsigned int i = 0; i < let->let().size(); i++) {
        run(env, let->let()[i]);
        if (auto* vd = Expression::dynamicCast<VarDecl>(let->let()[i])) {
          scopes.add(env, vd);
        }
      }
      run(env, let->in());
      VarDeclCmp poscmp(pos);
      std::stable_sort(let->let().begin(), let->let().end(), poscmp);
      for (unsigned int i = 0, j = 0; i < let->let().size(); i++) {
        if (auto* vd = Expression::dynamicCast<VarDecl>(let->let()[i])) {
          let->letOrig()[j++] = vd->e();
          for (unsigned int k = 0; k < vd->ti()->ranges().size(); k++) {
            let->letOrig()[j++] = vd->ti()->ranges()[k]->domain();
          }
        }
      }
      scopes.pop();
    } break;
  }
  if (env.ignoreUnknownIds) {
    std::vector<Expression*> toDelete;
    for (ExpressionSetIter it = Expression::ann(e).begin(); it != Expression::ann(e).end(); ++it) {
      try {
        run(env, *it);
      } catch (TypeError&) {
        toDelete.push_back(*it);
      }
      for (Expression* de : toDelete) {
        Expression::ann(e).remove(de);
      }
    }
  } else {
    for (ExpressionSetIter it = Expression::ann(e).begin(); it != Expression::ann(e).end(); ++it) {
      run(env, *it);
    }
  }
}

KeepAlive add_coercion(EnvI& env, Model* m, Expression* e0, const Location& loc_default,
                       const Type& funarg_t) {
  GCLock lock;
  Expression* e = e0;

  if (env.warnImplicitEnum2Int &&
      (funarg_t.bt() == Type::BT_INT || funarg_t.bt() == Type::BT_FLOAT) &&
      funarg_t.typeId() == 0 && !Expression::loc(e).isIntroduced() &&
      Expression::type(e).bt() == Type::BT_INT && Expression::type(e).elemType(env).typeId() != 0) {
    // Implicit coercion of enum to int is deprecated
    env.addWarning(Expression::locDefault(e, loc_default),
                   "Implicit coercion of enums to integers is deprecated and will be removed in a "
                   "future version.\n"
                   "Use the explicit coercion function enum2int instead.");
  }
  if (Expression::isa<Call>(e)) {
    auto* c = Expression::cast<Call>(e);
    if (c->id() == env.constants.ids.enum2int || c->id() == env.constants.ids.index2int) {
      // Remove call to enum2int/index2int
      assert(c->argCount() == 1);
      e = c->arg(0);
    } else if (c->id() == env.constants.ids.to_enum_internal) {
      // Remove call to to_enum_internal
      assert(c->argCount() == 2);
      e = c->arg(1);
    }
  }

  if (Expression::type(e).istuple() && funarg_t.dim() != 0) {
    // Check if this is an array of array type, wrapped in a tuple
    auto* tupleType = env.getTupleType(Expression::type(e));
    if (tupleType->size() == 2 && (*tupleType)[1].isunknown()) {
      // Yes: insert field access
      GCLock lock;
      e = new FieldAccess(Expression::loc(e).introduce(), e, IntLit::a(1));
      Expression::type(e, (*tupleType)[0]);
    }
  }

  if (Expression::isa<ArrayAccess>(e) && Expression::type(e).dim() > 0) {
    auto* aa = Expression::cast<ArrayAccess>(e);
    // Turn ArrayAccess into a slicing operation
    std::vector<Expression*> args;
    args.push_back(aa->v());
    args.push_back(nullptr);
    std::vector<Expression*> slice;
    for (unsigned int i = 0; i < aa->idx().size(); i++) {
      if (Expression::type(aa->idx()[i]).isSet()) {
        bool needIdxSet = true;
        bool needInter = true;
        Call* openIntervalCall = Expression::dynamicCast<Call>(aa->idx()[i]);
        if (openIntervalCall != nullptr) {
          if (openIntervalCall->argCount() == 0 &&
              (openIntervalCall->id() == "'..<'" || openIntervalCall->id() == "'<..'" ||
               openIntervalCall->id() == "'<..<'")) {
            needInter = false;
          } else {
            openIntervalCall = nullptr;
          }
        } else if (auto* sl = Expression::dynamicCast<SetLit>(aa->idx()[i])) {
          if ((sl->isv() != nullptr) && sl->isv()->size() == 1) {
            if (sl->isv()->min().isFinite() && sl->isv()->max().isFinite()) {
              args.push_back(sl);
              needIdxSet = false;
            } else if (sl->isv()->min() == -IntVal::infinity() &&
                       sl->isv()->max() == IntVal::infinity()) {
              needInter = false;
            }
          }
        }
        if (needIdxSet) {
          std::ostringstream oss;
          oss << "index_set";
          if (aa->idx().size() > 1) {
            oss << "_" << (i + 1) << "of" << aa->idx().size();
          }
          std::vector<Expression*> origIdxsetArgs(1);
          origIdxsetArgs[0] = aa->v();
          Call* origIdxset =
              Call::a(Expression::loc(aa->v()), ASTString(oss.str()), origIdxsetArgs);
          FunctionI* fi = m->matchFn(env, origIdxset, false);
          if (fi == nullptr) {
            throw TypeError(env, Expression::locDefault(e, loc_default),
                            "missing builtin " + oss.str());
          }
          origIdxset->type(fi->rtype(env, origIdxsetArgs, nullptr, false));
          origIdxset->decl(fi);
          if (needInter) {
            auto* inter =
                new BinOp(Expression::loc(aa->idx()[i]), aa->idx()[i], BOT_INTERSECT, origIdxset);
            inter->type(Type::parsetint());
            args.push_back(inter);
          } else if (openIntervalCall != nullptr) {
            auto* newOpenIntervalCall =
                Call::a(Expression::loc(openIntervalCall), openIntervalCall->id(), {origIdxset});
            FunctionI* nfi = m->matchFn(env, newOpenIntervalCall, false);
            if (nfi == nullptr) {
              throw TypeError(env, Expression::locDefault(e, loc_default),
                              "missing builtin " + std::string(openIntervalCall->id().c_str()));
            }
            newOpenIntervalCall->type(nfi->rtype(env, {origIdxset}, nullptr, false));
            newOpenIntervalCall->decl(nfi);
            slice.push_back(newOpenIntervalCall);
            args.push_back(newOpenIntervalCall);
          } else {
            args.push_back(origIdxset);
          }
        }
        if (openIntervalCall == nullptr) {
          slice.push_back(aa->idx()[i]);
        }
      } else {
        Expression* slice_set;
        Expression* idx = aa->idx()[i];
        if (!Expression::isa<Id>(idx) && !Expression::isa<IntLit>(idx)) {
          auto* ti = new TypeInst(Location().introduce(), Expression::type(idx), nullptr);
          auto* vd = new VarDecl(Location().introduce(), ti, env.genId(), idx);
          auto* bo = new BinOp(Expression::loc(aa->idx()[i]), vd->id(), BOT_DOTDOT, vd->id());
          bo->type(Type::parsetint());
          slice_set = new Let(Location().introduce(), {vd}, bo);
        } else {
          slice_set = new BinOp(Expression::loc(aa->idx()[i]), idx, BOT_DOTDOT, idx);
        }
        Expression::type(slice_set, Type::parsetint());
        slice.push_back(slice_set);
      }
    }
    auto* a_slice = new ArrayLit(Expression::loc(e), slice);
    a_slice->type(Type::parsetint(1));
    args[1] = a_slice;
    std::ostringstream oss;
    oss << "slice_" << (args.size() - 2) << "d";
    Call* c = Call::a(Expression::loc(e), ASTString(oss.str()), args);
    FunctionI* fi = m->matchFn(env, c, false);
    if (fi == nullptr) {
      throw TypeError(env, Expression::locDefault(e, loc_default), "missing builtin " + oss.str());
    }
    c->type(fi->rtype(env, args, nullptr, false));
    c->decl(fi);
    e = c;
  }

  auto getStructType = [&](StructType* current, std::vector<Type>& pt) {
    Type tt = funarg_t;
    unsigned int nId;
    if (Expression::type(e).bt() == Type::BT_TUPLE) {
      nId = env.registerTupleType(pt);
    } else {
      assert(Expression::type(e).bt() == Type::BT_RECORD);
      nId = env.registerRecordType(static_cast<RecordType*>(current), pt);
    }
    tt.typeId(nId);
    return tt;
  };

  Expression* e_coerced = e;
  if (Expression::type(e).structBT() && Expression::type(e).bt() == funarg_t.bt() &&
      Expression::type(e).dim() == funarg_t.dim()) {
    StructType* current = env.getStructType(Expression::type(e));
    StructType* intended = env.getStructType(funarg_t);
    if (intended->size() == current->size()) {
      // Directly add coercions in Array Literals
      if (auto* al = Expression::dynamicCast<ArrayLit>(e)) {
        std::vector<Expression*> elem(al->size());
        ArrayLit* c_al = nullptr;
        if (Expression::type(e).dim() > 0) {
          // Array of tuples (coerce each tuple individually)
          Type elemTy = funarg_t.elemType(env);
          for (unsigned int i = 0; i < al->size(); i++) {
            elem[i] = add_coercion(env, m, (*al)[i], Expression::loc(al), elemTy)();
          }
          std::vector<std::pair<int, int>> dims(al->dims());
          for (unsigned int i = 0; i < al->dims(); i++) {
            dims[i] = {al->min(i), al->max(i)};
          }
          c_al = new ArrayLit(Expression::loc(al).introduce(), elem, dims);
          Type coercedTy = funarg_t;
          if (!elem.empty()) {
            auto common = Type::bot();
            for (auto* el : elem) {
              common = Type::commonType(env, common, Expression::type(el));
            }
            assert(!common.isunknown());
            coercedTy = common;
          }
          c_al->type(Type::arrType(env, Expression::type(e), coercedTy));
        } else {
          assert(al->isTuple());
          std::vector<Type> pt(current->size());
          bool allPar = true;
          for (unsigned int i = 0; i < al->size(); i++) {
            Type elemTy = (*intended)[i];
            elem[i] = add_coercion(env, m, (*al)[i], Expression::loc(al), elemTy)();
            pt[i] = Expression::type(elem[i]);
            if (pt[i].isvar()) {
              allPar = false;
            }
          }
          if (current->size() > al->size()) {
            // Nested array wrapper
            pt[al->size()] = Type();
          }
          c_al = ArrayLit::constructTuple(Expression::loc(al).introduce(), elem);
          Type st = getStructType(current, pt);
          if (allPar) {
            st.mkPar(env);
          }
          c_al->type(st);
        }
        e_coerced = c_al;
      }
    }
  }

  auto sameBT = [&]() {
    return Expression::type(e).bt() == funarg_t.bt() &&
           (Expression::type(e).bt() != Type::BT_TUPLE ||
            env.getTupleType(Expression::type(e))->matchesBT(env, *env.getTupleType(funarg_t))) &&
           (Expression::type(e).bt() != Type::BT_RECORD ||
            env.getRecordType(Expression::type(e))->matchesBT(env, *env.getRecordType(funarg_t)));
  };
  if (Expression::type(e).dim() == funarg_t.dim() &&
      (funarg_t.bt() == Type::BT_BOT || funarg_t.bt() == Type::BT_TOP ||
       Expression::type(e).bt() == Type::BT_BOT || sameBT())) {
    return e_coerced;
  }
  e = e_coerced;
  Call* c = nullptr;
  if (Expression::type(e).isSet() && funarg_t.dim() != 0) {
    if (Expression::type(e).isvar()) {
      throw TypeError(env, Expression::locDefault(e, loc_default),
                      "cannot coerce var set into array");
    }
    if (Expression::type(e).isOpt()) {
      throw TypeError(env, Expression::locDefault(e, loc_default),
                      "cannot coerce opt set into array");
    }
    if (funarg_t.dim() > 1) {
      std::stringstream ss;
      ss << "cannot coerce set into " << funarg_t.dim() << "-dimensional array";
      throw TypeError(env, Expression::locDefault(e, loc_default), ss.str());
    }
    std::vector<Expression*> set2a_args(1);
    set2a_args[0] = e;
    Call* set2a = Call::a(Expression::loc(e), ASTString("set2array"), set2a_args);
    FunctionI* fi = m->matchFn(env, set2a, false);
    if (fi != nullptr) {
      set2a->type(fi->rtype(env, set2a_args, nullptr, false));
      set2a->decl(fi);
      e = set2a;
    }
  }
  if (funarg_t.bt() == Type::BT_TOP || sameBT() || Expression::type(e).bt() == Type::BT_BOT) {
    return e;
  }
  if (Expression::type(e).structBT() && Expression::type(e).bt() == funarg_t.bt() &&
      Expression::type(e).dim() == funarg_t.dim()) {
    StructType* current = env.getStructType(Expression::type(e));
    StructType* intended = env.getStructType(funarg_t);
    if (intended->size() == current->size()) {
      // Directly add coercions in Array Literals
      assert(!Expression::isa<ArrayLit>(e));
      // Create (bounded) identifier for expression if not available
      std::vector<Expression*> let_bindings;
      Expression* ident = e;
      if (!Expression::isa<Id>(ident)) {
        auto* vd =
            new VarDecl(Expression::loc(e),
                        new TypeInst(Expression::loc(e).introduce(), Expression::type(e)), 1, e);
        vd->ti()->setStructDomain(env, Expression::type(e));
        vd->toplevel(false);
        vd->type(Expression::type(e));
        let_bindings.push_back(vd);
        ident = vd->id();
      }
      Expression* ret;
      if (Expression::type(e).dim() > 0) {
        Type tyElem = Expression::type(e).elemType(env);
        Type ty1d = Type::arrType(env, Type::partop(1), Expression::type(e));
        // Expressions that has array of tuple type
        auto* array1d = Call::a(Expression::loc(e).introduce(), env.constants.ids.array1d, {ident});
        array1d->type(ty1d);
        array1d->decl(m->matchFn(env, array1d, false, true));
        auto* vd_array1d = new VarDecl(
            Expression::loc(e), new TypeInst(Expression::loc(e).introduce(), ty1d), 2, array1d);
        vd_array1d->ti()->setStructDomain(env, ty1d);
        vd_array1d->toplevel(false);
        let_bindings.push_back(vd_array1d);

        auto* index_set = Call::a(Expression::loc(e).introduce(), "index_set", {vd_array1d->id()});
        index_set->decl(m->matchFn(env, index_set, false, true));
        index_set->type(Type::parsetint());

        auto* vd_it = new VarDecl(Location().introduce(),
                                  new TypeInst(Expression::loc(e).introduce(), tyElem), 3);
        vd_it->toplevel(false);
        Generator gen({vd_it}, index_set, nullptr);
        Generators gens;
        gens.g = {gen};

        auto* aa = new ArrayAccess(Expression::loc(e).introduce(), vd_array1d->id(), {vd_it->id()});
        aa->type(tyElem);
        Expression* elem = add_coercion(env, m, aa, Expression::loc(aa), funarg_t.elemType(env))();
        auto* comprehension = new Comprehension(Location().introduce(), elem, gens, true);
        comprehension->type(Type::arrType(env, Type::partop(1), Expression::type(elem)));

        auto* arrayXd = Call::a(Expression::loc(e).introduce(), env.constants.ids.arrayXd,
                                {ident, comprehension});
        arrayXd->type(Type::arrType(env, Expression::type(e), Expression::type(elem)));
        arrayXd->decl(m->matchFn(env, arrayXd, false, true));
        ret = arrayXd;
      } else {
        // Expression that has tuple type
        std::vector<Expression*> collect(intended->size());
        std::vector<Type> pt(intended->size());
        for (unsigned int i = 0; i < collect.size(); i++) {
          collect[i] = new FieldAccess(Expression::loc(e).introduce(), ident, IntLit::a(i + 1));
          Expression::type(collect[i], (*current)[i]);
          collect[i] = add_coercion(env, m, collect[i], loc_default, (*intended)[i])();
          pt[i] = Expression::type(collect[i]);
        }
        auto* c_al = ArrayLit::constructTuple(Expression::loc(e).introduce(), collect);
        c_al->type(getStructType(current, pt));
        ret = c_al;
      }
      if (!let_bindings.empty()) {
        auto* let = new Let(Expression::loc(e).introduce(), let_bindings, ret);
        let->type(Expression::type(ret));
        ret = let;
      }
      return ret;
    }
  }
  std::vector<Expression*> args(1);
  args[0] = e;
  if (Expression::type(e).bt() == Type::BT_BOOL) {
    if (funarg_t.bt() == Type::BT_INT) {
      c = Call::a(Expression::loc(e), env.constants.ids.bool2int, args);
    } else if (funarg_t.bt() == Type::BT_FLOAT) {
      c = Call::a(Expression::loc(e), env.constants.ids.bool2float, args);
    }
  } else if (Expression::type(e).bt() == Type::BT_INT) {
    if (funarg_t.bt() == Type::BT_FLOAT) {
      c = Call::a(Expression::loc(e), env.constants.ids.int2float, args);
    }
  }
  if (c != nullptr) {
    FunctionI* fi = m->matchFn(env, c, false);
    if (fi != nullptr) {
      Type ct = fi->rtype(env, args, nullptr, false);
      ct.cv(Expression::type(e).cv() || ct.cv());
      c->type(ct);
      c->decl(fi);
      KeepAlive ka(c);
      return ka;
    }
  }
  throw TypeError(env, Expression::locDefault(e, loc_default),
                  "cannot determine coercion from type " + Expression::type(e).toString(env) +
                      " to type " + funarg_t.toString(env));
}
KeepAlive add_coercion(EnvI& env, Model* m, Expression* e, Expression* e_default,
                       const Type& funarg_t) {
  return add_coercion(env, m, e, Expression::loc(e_default), funarg_t);
}
KeepAlive add_coercion(EnvI& env, Model* m, Expression* e, Expression* e_default,
                       Expression* funarg) {
  return add_coercion(env, m, e, Expression::loc(e_default), Expression::type(funarg));
}
KeepAlive add_coercion(EnvI& env, Model* m, Expression* e, const Location& loc_default,
                       Expression* funarg) {
  return add_coercion(env, m, e, loc_default, Expression::type(funarg));
}

template <bool ignoreVarDecl>
class Typer {
private:
  EnvI& _env;
  Model* _model;
  std::vector<TypeError>& _typeErrors;
  bool _isFlatZinc;

public:
  std::unordered_set<VarDecl*> anyInLet;
  Typer(EnvI& env, Model* model, std::vector<TypeError>& typeErrors, bool isFlatZinc)
      : _env(env), _model(model), _typeErrors(typeErrors), _isFlatZinc(isFlatZinc) {}
  /// Check annotations when expression is finished
  void exit(Expression* e) {
    for (ExpressionSetIter it = Expression::ann(e).begin(); it != Expression::ann(e).end(); ++it) {
      if (!Expression::type(*it).isAnn()) {
        throw TypeError(_env, Expression::loc(*it),
                        "expected annotation, got `" + Expression::type(*it).toString(_env) + "'");
      }
    }
  }
  bool enter(Expression* /*e*/) { return true; }
  /// Visit integer literal
  void vIntLit(const IntLit* /*i*/) {}
  /// Visit floating point literal
  void vFloatLit(const FloatLit* /*f*/) {}
  /// Visit Boolean literal
  void vBoolLit(const BoolLit* /*b*/) {}
  /// Visit set literal
  void vSetLit(SetLit* sl) {
    Type ty;
    ty.st(Type::ST_SET);
    if (sl->isv() != nullptr) {
      ty.bt(Type::BT_INT);
      ty.typeId(sl->type().typeId());
      sl->type(ty);
      return;
    }
    if (sl->fsv() != nullptr) {
      ty.bt(Type::BT_FLOAT);
      sl->type(ty);
      return;
    }
    unsigned int enumId = sl->v().empty() ? 0 : Expression::type(sl->v()[0]).typeId();
    for (unsigned int i = 0; i < sl->v().size(); i++) {
      Type vi_t = Expression::type(sl->v()[i]);
      vi_t.ot(Type::OT_PRESENT);
      if (sl->v()[i] == _env.constants.absent) {
        continue;
      }
      if (vi_t.dim() > 0) {
        throw TypeError(_env, Expression::loc(sl->v()[i]), "set literals cannot contain arrays");
      }
      if (vi_t.st() == Type::ST_SET) {
        throw TypeError(_env, Expression::loc(sl->v()[i]), "set literals cannot contain sets");
      }
      if (vi_t.structBT()) {
        throw TypeError(
            _env, Expression::loc(sl->v()[i]),
            std::string("set literals cannot contain ") +
                (vi_t.bt() == Type::BT_TUPLE ? std::string("tuples") : std::string("records")));
      }
      if (vi_t.bt() == Type::BT_ANN) {
        throw TypeError(_env, Expression::loc(sl->v()[i]),
                        "set literals cannot contain annotations");
      }
      if (vi_t.bt() == Type::BT_STRING) {
        throw TypeError(_env, Expression::loc(sl->v()[i]), "set literals cannot contain strings");
      }
      if (vi_t.isvar()) {
        ty.ti(Type::TI_VAR);
      }
      if (vi_t.cv()) {
        ty.cv(true);
      }
      if (enumId != vi_t.typeId()) {
        enumId = 0;
      }
      if (!Type::btSubtype(_env, vi_t, ty, true)) {
        if (ty.bt() == Type::BT_UNKNOWN || Type::btSubtype(_env, ty, vi_t, true)) {
          ty.bt(vi_t.bt());
        } else {
          throw TypeError(_env, Expression::loc(sl), "non-uniform set literal");
        }
      }
    }
    ty.typeId(enumId);
    if (ty.bt() == Type::BT_UNKNOWN) {
      ty.bt(Type::BT_BOT);
    } else {
      if (ty.isvar() && ty.bt() != Type::BT_INT) {
        if (ty.bt() == Type::BT_BOOL) {
          ty.bt(Type::BT_INT);
        } else {
          throw TypeError(_env, Expression::loc(sl),
                          "cannot coerce set literal element to var int");
        }
      }
      for (unsigned int i = 0; i < sl->v().size(); i++) {
        sl->v()[i] = add_coercion(_env, _model, sl->v()[i], sl, ty)();
      }
    }
    sl->type(ty);
  }
  /// Visit string literal
  void vStringLit(const StringLit* /*sl*/) {}
  /// Visit identifier
  void vId(Id* ident) {
    if (ident != _env.constants.absent && !ident->decl()->isTypeAlias()) {
      if (ident->decl()->type().isunknown()) {
        ident->decl()->type(ident->decl()->ti()->type());
      }
      assert(!ident->decl()->type().isunknown());
      ident->type(ident->decl()->type());
    }
  }
  /// Visit anonymous variable
  void vAnonVar(const AnonVar* /*v*/) {}
  /// Visit array literal
  void vArrayLit(ArrayLit* al) {
    if (al->isTuple()) {
      if (al->type().isrecord()) {
        if (al->type().typeId() != 0) {
          auto* rt = _env.getRecordType(al->type());
          std::vector<Type> fields(al->size());
          for (unsigned int i = 0; i < al->size(); i++) {
            fields[i] = Expression::type((*al)[i]);
          }
          auto ty = al->type();
          ty.typeId(_env.registerRecordType(rt, fields));
          al->type(ty);
        } else {
          _env.registerRecordType(al);
        }
      } else if (al->type().typeId() != Type::COMP_INDEX) {
        _env.registerTupleType(al);
      }
      for (unsigned int i = 0; i < al->size(); i++) {
        auto* vi = Expression::dynamicCast<AnonVar>((*al)[i]);
        if (vi != nullptr) {
          std::stringstream ss;
          ss << "anonymous variables in " << (al->type().isrecord() ? "record" : "tuple")
             << " literals are not currently supported";
          throw TypeError(_env, Expression::loc(vi), ss.str());
        }
      }
      return;
    }
    Type elemTy = Type::bot();
    std::vector<AnonVar*> anons;
    bool haveInferredType = false;
    for (unsigned int i = 0; i < al->size(); i++) {
      Expression* vi = (*al)[i];
      if (Expression::type(vi).dim() > 0) {
        GCLock lock;
        auto* wrapper = ArrayLit::constructTuple(Expression::loc(vi).introduce(), {vi});
        auto wrapper_t = _env.registerTupleType({Expression::type(vi), Type()});
        Type tt = Type::tuple();
        tt.typeId(wrapper_t);
        Expression::type(wrapper, tt);
        al->set(i, wrapper);
        vi = wrapper;
      }
      auto* av = Expression::dynamicCast<AnonVar>(vi);
      if (av == nullptr) {
        auto common = Type::commonType(_env, elemTy, Expression::type(vi));
        if (common.isunknown()) {
          std::stringstream ss;
          ss << "non-uniform array literal. Expected `" << elemTy.toString(_env) << "' but got `"
             << Expression::type(vi).toString(_env) << "'.";
          throw TypeError(_env, Expression::locDefault(vi, al), ss.str());
        }
        elemTy = common;
      } else {
        anons.push_back(av);
      }
    }

    if (!anons.empty()) {
      if (elemTy.isbot()) {
        throw TypeError(_env, Expression::loc(al),
                        "array literal must contain at least one non-anonymous variable");
      }
      if (!elemTy.isVarifiable(_env)) {
        std::stringstream ss;
        ss << "anonymous variable of type `" << elemTy.toString(_env) << "' not allowed";
        throw TypeError(_env, Expression::loc(anons[0]), ss.str());
      }
      if (elemTy.structBT()) {
        // TODO: Fix flattening of struct type anon vars and remove this
        std::stringstream ss;
        ss << (elemTy.istuple() ? "tuple" : "record")
           << " type anonymous variables are not yet supported";
        throw TypeError(_env, Expression::loc(anons[0]), ss.str());
      }
      elemTy.mkVar(_env);
      for (auto& anon : anons) {
        anon->type(elemTy);
      }
    }

    for (unsigned int i = 0; i < al->size(); i++) {
      al->set(i, add_coercion(_env, _model, (*al)[i], al, elemTy)());
    }

    auto arrType = Type::arrType(_env, Type::parint(static_cast<int>(al->dims())), elemTy);
    al->type(arrType);
  }
  /// Visit array access
  void vArrayAccess(ArrayAccess* aa) {
    if (Expression::type(aa->v()).dim() == 0) {
      if (Expression::type(aa->v()).st() == Type::ST_SET) {
        Type plainTy = Expression::type(aa->v());
        plainTy.st(Type::ST_PLAIN);
        Type tv = Type::arrType(_env, Type::partop(1), plainTy);
        aa->v(add_coercion(_env, _model, aa->v(), aa, tv)());
      } else if (Expression::type(aa->v()).bt() == Type::BT_TUPLE) {
        assert(Expression::type(aa->v()).typeId() != 0);
        TupleType* tt = _env.getTupleType(Expression::type(aa->v()));
        if (tt->size() != 2 || !(*tt)[1].isunknown()) {
          assert((*tt)[0].dim() != 0);
          std::ostringstream oss;
          oss << "array access attempted on expression of type `"
              << Expression::type(aa->v()).toString(_env) << "'";
          throw TypeError(_env, Expression::loc(aa->v()), oss.str());
        }
        Type resultType = (*tt)[0];
        aa->v(add_coercion(_env, _model, aa->v(), aa, resultType)());
      } else {
        std::ostringstream oss;
        oss << "array access attempted on expression of type `"
            << Expression::type(aa->v()).toString(_env) << "'";
        throw TypeError(_env, Expression::loc(aa->v()), oss.str());
      }
    } else if (Expression::isa<ArrayAccess>(aa->v()) || Expression::isa<Call>(aa->v())) {
      aa->v(add_coercion(_env, _model, aa->v(), aa, Expression::type(aa->v()))());
    }
    if (Expression::type(aa->v()).dim() != aa->idx().size()) {
      std::ostringstream oss;
      oss << Expression::type(aa->v()).dim() << "-dimensional array accessed with "
          << aa->idx().size() << (aa->idx().size() == 1 ? " expression" : " expressions");
      throw TypeError(_env, Expression::loc(aa->v()), oss.str());
    }
    Type tt = Expression::type(aa->v());
    std::vector<unsigned int> arrayEnumIds;
    if (tt.typeId() != 0) {
      arrayEnumIds = _env.getArrayEnum(tt.typeId());
      std::vector<unsigned int> newArrayEnumids;

      for (unsigned int i = 0; i < arrayEnumIds.size() - 1; i++) {
        Expression* aai = aa->idx()[i];
        // Check if index is slice operator, and convert to correct enum type
        if (auto* aai_sl = Expression::dynamicCast<SetLit>(aai)) {
          if (IntSetVal* aai_isv = aai_sl->isv()) {
            if (aai_isv->min() == -IntVal::infinity() && aai_isv->max() == IntVal::infinity()) {
              Type aai_sl_t = aai_sl->type();
              aai_sl_t.typeId(arrayEnumIds[i]);
              aai_sl->type(aai_sl_t);
            }
          }
        } else if (auto* aai_bo = Expression::dynamicCast<BinOp>(aai)) {
          if (aai_bo->op() == BOT_DOTDOT) {
            Type aai_bo_t = aai_bo->type();
            if (auto* il = Expression::dynamicCast<IntLit>(aai_bo->lhs())) {
              if (IntLit::v(il) == -IntVal::infinity()) {
                // Expression is ..X, so result gets enum type of X
                aai_bo_t.typeId(Expression::type(aai_bo->rhs()).typeId());
              }
            } else if (auto* il = Expression::dynamicCast<IntLit>(aai_bo->rhs())) {
              if (IntLit::v(il) == IntVal::infinity()) {
                // Expression is X.., so result gets enum type of X
                aai_bo_t.typeId(Expression::type(aai_bo->lhs()).typeId());
              }
            }
            aai_bo->type(aai_bo_t);
          }
        } else if (auto* aai_c = Expression::dynamicCast<Call>(aai)) {
          if (aai_c->argCount() == 0 &&
              (aai_c->id() == "'..<'" || aai_c->id() == "'<..'" || aai_c->id() == "'<..<'")) {
            Type aai_c_t = aai_c->type();
            aai_c_t.typeId(arrayEnumIds[i]);
            aai_c->type(aai_c_t);
          }
        }
        if (Expression::type(aai).isSet()) {
          newArrayEnumids.push_back(arrayEnumIds[i]);
        }

        if (arrayEnumIds[i] != 0) {
          if (Expression::type(aa->idx()[i]).typeId() != arrayEnumIds[i]) {
            std::ostringstream oss;
            oss << "array index ";
            if (aa->idx().size() > 1) {
              oss << (i + 1) << " ";
            }
            oss << "must be `" << _env.getEnum(arrayEnumIds[i])->e()->id()->str() << "', but is `"
                << Expression::type(aa->idx()[i]).toString(_env) << "'";
            throw TypeError(_env, Expression::loc(aa), oss.str());
          }
        }
      }
      if (newArrayEnumids.empty()) {
        tt.typeId(arrayEnumIds[arrayEnumIds.size() - 1]);
      } else {
        newArrayEnumids.push_back(arrayEnumIds[arrayEnumIds.size() - 1]);
        int newEnumId = _env.registerArrayEnum(newArrayEnumids);
        tt.typeId(newEnumId);
      }
    }
    int n_dimensions = 0;
    bool isVarAccess = false;
    bool isSlice = false;
    bool containsArray =
        tt.structBT() && _env.getStructType(Expression::type(aa->v()))->containsArray(_env);
    for (unsigned int i = 0; i < aa->idx().size(); i++) {
      Expression* aai = aa->idx()[i];
      if (Expression::isa<AnonVar>(aai)) {
        Expression::type(aai, Type::varint());
      }
      if ((Expression::type(aai).bt() != Type::BT_INT &&
           Expression::type(aai).bt() != Type::BT_BOOL) ||
          Expression::type(aai).dim() != 0) {
        throw TypeError(_env, Expression::loc(aa),
                        "array index must be `int' or `set of int', but is `" +
                            Expression::type(aai).toString(_env) + "'");
      }
      if (Expression::type(aai).isSet()) {
        if (isVarAccess || Expression::type(aai).isvar()) {
          throw TypeError(_env, Expression::loc(aa),
                          "array slicing with variable range or index not supported");
        }
        isSlice = true;
        Type t = Type::varsetint();
        if (!arrayEnumIds.empty()) {
          t.typeId(arrayEnumIds[i]);
        }
        aa->idx()[i] = add_coercion(_env, _model, aai, aa, t)();
        n_dimensions++;
      } else {
        Type t = Type::varint();
        if (!arrayEnumIds.empty()) {
          t.typeId(arrayEnumIds[i]);
        }
        aa->idx()[i] = add_coercion(_env, _model, aai, aa, t)();
      }

      if (Expression::type(aai).isOpt()) {
        if (tt.structBT()) {
          std::ostringstream oss;
          oss << "array access using an optional index is not supported for array of "
              << (tt.bt() == Type::BT_TUPLE ? "tuple" : "record") << "s.";
          throw TypeError(_env, Expression::loc(aai), oss.str());
        }
        tt.ot(Type::OT_OPTIONAL);
      }
      unsigned int typeId = tt.typeId();
      tt.typeId(0);
      tt.dim(n_dimensions);
      tt.typeId(typeId);
      if (Expression::type(aai).isvar()) {
        isVarAccess = true;
        if (isSlice) {
          throw TypeError(_env, Expression::loc(aa),
                          "array slicing with variable range or index not supported");
        }
        tt.mkVar(_env);
        if (tt.contains(_env, [](Type t) {
              return t.bt() == Type::BT_ANN || t.bt() == Type::BT_STRING ||
                     (t.st() == Type::ST_SET &&
                      ((t.bt() != Type::BT_INT && t.bt() != Type::BT_TOP) || t.isOpt()));
            })) {
          std::ostringstream oss;
          oss << "array access using a variable is not supported for array of "
              << Expression::type(aa->v()).elemType(_env).toString(_env) << ".";
          throw TypeError(_env, Expression::loc(aai), oss.str());
        }
        if (containsArray) {
          std::ostringstream oss;
          oss << "array access using a variable is not supported for arrays which contain other "
                 "arrays";
          throw TypeError(_env, Expression::loc(aai), oss.str());
        }
      }
      if (Expression::type(aai).cv()) {
        tt.cv(true);
      }
    }
    aa->type(tt);
  }
  /// Visit field access
  void vFieldAccess(FieldAccess* fa) {
    if (!Expression::type(fa->v()).istuple() && !Expression::type(fa->v()).isrecord()) {
      std::ostringstream oss;
      oss << "field access attempted on expression of type `"
          << Expression::type(fa->v()).toString(_env) << "'";
      throw TypeError(_env, Expression::loc(fa), oss.str());
    }
    if (Expression::type(fa->v()).istuple()) {
      if (!Expression::isa<IntLit>(fa->field())) {
        throw TypeError(_env, Expression::loc(fa),
                        "field access of a tuple must use an integer literal");
      }
      assert(Expression::type(fa->v()).typeId() != 0);
      TupleType* tt = _env.getTupleType(Expression::type(fa->v()));
      if (tt->size() == 2 && (*tt)[1].isunknown() && !Expression::loc(fa).isIntroduced()) {
        std::ostringstream oss;
        oss << "field access attempted on expression of type `"
            << Expression::type(fa->v()).toString(_env) << "'";
        throw TypeError(_env, Expression::loc(fa), oss.str());
      }
      IntVal i = IntLit::v(Expression::cast<IntLit>(fa->field()));
      if (!i.isFinite() || i < 1 || i.toInt() > tt->size()) {
        std::ostringstream oss;
        oss << "unable to access field " << i << " of an expression of type `"
            << Expression::type(fa->v()).toString(_env) << "'. Its fields are between 1 and "
            << tt->size() << ".";
        throw TypeError(_env, Expression::loc(fa), oss.str());
      }
      Type ty((*tt)[static_cast<unsigned int>(i.toInt()) - 1]);
      assert((!ty.cv()) || Expression::type(fa->v()).cv());
      ty.cv(Expression::type(fa->v()).cv());
      fa->type(ty);
    } else {
      // Check if field exists
      assert(Expression::type(fa->v()).isrecord());
      unsigned int loc;
      RecordType* rt = _env.getRecordType(Expression::type(fa->v()));
      if (!Expression::isa<Id>(fa->field())) {
        if (fa->type().bt() == Type::BT_UNKNOWN) {
          throw TypeError(_env, Expression::loc(fa),
                          "field access of a record must use a field identifier");
        }
        loc =
            static_cast<unsigned int>(IntLit::v(Expression::cast<IntLit>(fa->field())).toInt()) - 1;
      } else {
        ASTString name = Expression::cast<Id>(fa->field())->str();
        auto find = rt->findField(name);
        if (!find.first) {
          std::ostringstream oss;
          oss << "expression of type `" << Expression::type(fa->v()).toString(_env)
              << "' does not have a field named `" << name << "'.";
          throw TypeError(_env, Expression::loc(fa), oss.str());
        }
        loc = find.second;
        // Replace Id with IntLit
        IntLit* nf = IntLit::a(static_cast<long long>(loc + 1));
        fa->field(nf);
      }
      // Set overall expression type
      Type ty((*rt)[loc]);
      assert((!ty.cv()) || Expression::type(fa->v()).cv());
      ty.cv(Expression::type(fa->v()).cv());
      fa->type(ty);
    }
  }
  /// Visit array comprehension
  void vComprehension(Comprehension* c) {
    Expression* c_e = c->e();
    auto* indexTuple = Expression::dynamicCast<ArrayLit>(c->e());
    if (indexTuple != nullptr &&
        (!indexTuple->isTuple() || indexTuple->type().typeId() != Type::COMP_INDEX)) {
      indexTuple = nullptr;
    }
    if (indexTuple == nullptr) {
      if (Expression::isa<Call>(c_e)) {
        // Ensure enum2int and similar get removed if necessary
        c->e(add_coercion(_env, _model, c_e, c, Expression::type(c_e))());
      }
    } else {
      for (unsigned int i = 0; i < indexTuple->size(); i++) {
        auto* elem = (*indexTuple)[i];
        if (Expression::isa<Call>(elem)) {
          // Ensure enum2int and similar get removed if necessary
          indexTuple->set(i, add_coercion(_env, _model, elem, c, Expression::type(elem))());
        }
      }
      c_e = (*indexTuple)[indexTuple->size() - 1];
    }
    Type tt = Expression::type(c_e);

    if (tt.dim() > 0) {
      GCLock lock;
      auto* wrapper = ArrayLit::constructTuple(Expression::loc(c_e).introduce(), {c_e});
      auto wrapper_t = _env.registerTupleType({tt, Type()});
      tt = Type::tuple();
      tt.typeId(wrapper_t);
      Expression::type(wrapper, tt);
      c->e(wrapper);
      c_e = wrapper;
    }

    typedef std::unordered_map<VarDecl*, std::pair<unsigned int, unsigned int>> genMap_t;
    typedef std::unordered_map<VarDecl*, std::vector<Expression*>> whereMap_t;
    genMap_t generatorMap;
    whereMap_t whereMap;
    unsigned int declCount = 0;

    for (unsigned int i = 0; i < c->numberOfGenerators(); i++) {
      for (unsigned int j = 0; j < c->numberOfDecls(i); j++) {
        generatorMap[c->decl(i, j)] = std::pair<unsigned int, unsigned int>(i, declCount++);
        whereMap[c->decl(i, j)] = std::vector<Expression*>();
      }
      Expression* g_in = c->in(i);
      if (g_in != nullptr) {
        const Type& ty_in = Expression::type(g_in);
        if (ty_in == Type::varsetint()) {
          if (tt.structBT()) {
            std::ostringstream oss;
            oss << "variable set generator not supported for "
                << (tt.bt() == Type::BT_TUPLE ? "tuple" : "record") << "-valued comprehension";
            throw TypeError(_env, Expression::loc(g_in), oss.str());
          }
          if (!c->set()) {
            tt.ot(Type::OT_OPTIONAL);
          }
          tt.mkVar(_env);
        }
        if (ty_in.cv()) {
          tt.cv(true);
        }
        if (c->where(i) != nullptr) {
          if (Expression::type(c->where(i)) == Type::varbool()) {
            if (tt.structBT()) {
              std::ostringstream oss;
              oss << "variable where clause for "
                  << (tt.bt() == Type::BT_TUPLE ? "tuple" : "record")
                  << "-valued comprehension not supported";
              throw TypeError(_env, Expression::loc(c->where(i)), oss.str());
            }
            if (!c->set()) {
              if (Expression::type(c_e).isSet()) {
                throw TypeError(_env, Expression::loc(c->where(i)),
                                "variable where clause not allowed in set-valued comprehension");
              }
              tt.ot(Type::OT_OPTIONAL);
            }
            tt.mkVar(_env);
          } else if (Expression::type(c->where(i)) != Type::parbool()) {
            throw TypeError(_env, Expression::locDefault(c->where(i), c),
                            "where clause must be bool, but is `" +
                                Expression::type(c->where(i)).toString(_env) + "'");
          }
          if (Expression::type(c->where(i)).cv()) {
            tt.cv(true);
          }

          // Try to move parts of the where clause to earlier generators
          std::vector<Expression*> wherePartsStack;
          std::vector<Expression*> whereParts;
          wherePartsStack.push_back(c->where(i));
          while (!wherePartsStack.empty()) {
            Expression* e = wherePartsStack.back();
            wherePartsStack.pop_back();
            if (auto* bo = Expression::dynamicCast<BinOp>(e)) {
              if (bo->op() == BOT_AND) {
                wherePartsStack.push_back(bo->rhs());
                wherePartsStack.push_back(bo->lhs());
              } else {
                whereParts.push_back(e);
              }
            } else {
              whereParts.push_back(e);
            }
          }

          for (auto* wp : whereParts) {
            class FindLatestGen : public EVisitor {
            public:
              unsigned int declIndex;
              VarDecl* decl;
              const genMap_t& generatorMap;
              Comprehension* comp;
              FindLatestGen(const genMap_t& generatorMap0, Comprehension* comp0)
                  : generatorMap(generatorMap0), comp(comp0) {
                for (unsigned int i = 0; i < comp->numberOfGenerators(); i++) {
                  if (comp->in(i) != nullptr) {
                    declIndex = i;
                    decl = comp->decl(i, 0);
                    break;
                  }
                }
              }
              void vId(const Id* ident) {
                auto it = generatorMap.find(ident->decl());
                if (it != generatorMap.end() && it->second.second > declIndex) {
                  declIndex = it->second.second;
                  decl = ident->decl();
                  auto gen = it->second.first;
                  while (comp->in(gen) == nullptr && gen < comp->numberOfGenerators() - 1) {
                    declIndex++;
                    gen++;
                    decl = comp->decl(gen, 0);
                  }
                }
              }
            } flg(generatorMap, c);
            top_down(flg, wp);
            whereMap[flg.decl].push_back(wp);
          }
        }
      } else {
        assert(c->where(i) != nullptr);
        whereMap[c->decl(i, 0)].push_back(c->where(i));
      }
    }

    {
      GCLock lock;
      Generators generators;
      for (unsigned int i = 0; i < c->numberOfGenerators(); i++) {
        std::vector<VarDecl*> decls;
        for (unsigned int j = 0; j < c->numberOfDecls(i); j++) {
          decls.push_back(c->decl(i, j));
          KeepAlive c_in = c->in(i) != nullptr
                               ? add_coercion(_env, _model, c->in(i), c, Expression::type(c->in(i)))
                               : nullptr;
          auto* decl_ti = c->decl(i, j)->ti();
          if (decl_ti->type().structBT() && decl_ti->domain() == nullptr) {
            decl_ti->setStructDomain(_env, decl_ti->type());
          }
          if (!whereMap[c->decl(i, j)].empty()) {
            // need a generator for all the decls up to this point
            Expression* whereExpr = whereMap[c->decl(i, j)][0];
            whereExpr = add_coercion(_env, _model, whereExpr, c, Expression::type(whereExpr))();
            for (unsigned int k = 1; k < whereMap[c->decl(i, j)].size(); k++) {
              GCLock lock;
              auto* bo =
                  new BinOp(Location().introduce(), whereExpr, BOT_AND, whereMap[c->decl(i, j)][k]);
              Type bo_t = Expression::type(whereMap[c->decl(i, j)][k]).isPar() &&
                                  Expression::type(whereExpr).isPar()
                              ? Type::parbool()
                              : Type::varbool();
              if (Expression::type(whereMap[c->decl(i, j)][k]).cv() ||
                  Expression::type(whereExpr).cv()) {
                bo_t.cv(true);
              }
              bo->type(bo_t);
              whereExpr = bo;
            }
            generators.g.emplace_back(decls, c_in(), whereExpr);
            decls.clear();
          } else if (j == c->numberOfDecls(i) - 1) {
            generators.g.emplace_back(decls, c_in(), nullptr);
            decls.clear();
          }
        }
      }
      c->init(c->e(), generators);
    }

    if (c->set()) {
      if (Expression::type(c_e).dim() != 0 || Expression::type(c_e).st() == Type::ST_SET ||
          Expression::type(c_e).structBT()) {
        throw TypeError(_env, Expression::loc(c_e),
                        "set comprehension expression must be scalar, but is `" +
                            Expression::type(c_e).toString(_env) + "'");
      }
      tt.st(Type::ST_SET);
      if (tt.isvar()) {
        Type var_int = Type::varint();
        var_int.typeId(tt.typeId());
        c->e(add_coercion(_env, _model, c->e(), c, var_int)());
        tt.bt(Type::BT_INT);
      }
    } else {
      std::vector<unsigned int> enumIds;
      bool hadEnums = false;
      unsigned int typeId = tt.typeId();
      tt.typeId(0);
      if (indexTuple != nullptr) {
        tt.dim(static_cast<int>(indexTuple->size()) - 1);
        for (unsigned int i = 0; i < indexTuple->size() - 1; i++) {
          if (!Expression::type((*indexTuple)[i]).isPar()) {
            throw TypeError(_env, Expression::loc((*indexTuple)[i]), "index is not par");
          }
          if (!Expression::type((*indexTuple)[i]).isint()) {
            throw TypeError(_env, Expression::locDefault((*indexTuple)[i], indexTuple),
                            "index is not int or enumerated type");
          }
          unsigned int e = Expression::type((*indexTuple)[i]).typeId();
          enumIds.push_back(e);
          if (e != 0) {
            hadEnums = true;
          }
        }
      } else {
        tt.dim(1);
        enumIds.push_back(0);
      }
      if (hadEnums || typeId != 0) {
        enumIds.push_back(typeId);
        tt.typeId(_env.registerArrayEnum(enumIds));
      }
    }
    if (tt.isvar()) {
      if (tt.bt() == Type::BT_ANN || tt.bt() == Type::BT_STRING ||
          (tt.st() == Type::ST_SET && (tt.bt() != Type::BT_INT || tt.isOpt()))) {
        throw TypeError(_env, Expression::loc(c),
                        "invalid type for comprehension: `" + tt.toString(_env) + "'");
      }
    }
    c->type(tt);
  }
  /// Visit array comprehension generator
  void vComprehensionGenerator(Comprehension* c, int gen_i) {
    Expression* g_in = c->in(gen_i);
    if (g_in == nullptr) {
      // This is an "assignment generator" (i = expr)
      assert(c->where(gen_i) != nullptr);
      assert(c->numberOfDecls(gen_i) == 1);
      const Type& ty_where = Expression::type(c->where(gen_i));
      c->decl(gen_i, 0)->type(ty_where);
      c->decl(gen_i, 0)->ti()->type(ty_where);
      if (ty_where.structBT()) {
        c->decl(gen_i, 0)->ti()->setStructDomain(_env, ty_where);
      } else if (ty_where.dim() > 0) {
        GCLock lock;
        std::vector<TypeInst*> ranges(ty_where.dim());
        for (int i = 0; i < ty_where.dim(); i++) {
          ranges[i] = new TypeInst(Location().introduce(), Type::parint());
        }
        c->decl(gen_i, 0)->ti()->setRanges(ranges);
      }
    } else {
      const Type& ty_in = Expression::type(g_in);
      if (ty_in != Type::varsetint() && ty_in != Type::parsetint() && ty_in.dim() == 0) {
        if (!ty_in.isSet() || ty_in.bt() != Type::BT_BOT) {
          throw TypeError(
              _env, Expression::locDefault(g_in, c),
              "generator expression must be (par or var) set of int or array, but is `" +
                  ty_in.toString(_env) + "'");
        }
      }
      Type ty_id;
      if (ty_in.dim() == 0) {
        FunctionI* set2iter =
            _env.model->matchFn(_env, _env.constants.ids.set2iter, {ty_in}, false);
        if (ty_in.isvar() && set2iter != nullptr && set2iter->e() != nullptr) {
          ty_id = Type::varint();
          ty_id.typeId(ty_in.typeId());
        } else {
          ty_id = Type::parint();
          ty_id.typeId(ty_in.typeId());
        }
      } else {
        ty_id = ty_in;
        ty_id.typeId(0);
        ty_id.dim(0);
        if (ty_in.typeId() != 0) {
          const std::vector<unsigned int>& enumIds = _env.getArrayEnum(ty_in.typeId());
          ty_id.typeId(enumIds.back());
        }
      }
      for (unsigned int j = 0; j < c->numberOfDecls(gen_i); j++) {
        c->decl(gen_i, j)->type(ty_id);
        c->decl(gen_i, j)->ti()->type(ty_id);
      }
    }
  }
  /// Visit if-then-else
  void vITE(ITE* ite) {
    // Set return type to else type or, in case of no else, unknown
    Type tret = Type::bot();
    std::vector<AnonVar*> anons;
    bool varcond = false;
    for (unsigned int i = 0; i < ite->size(); i++) {
      Expression* eif = ite->ifExpr(i);
      Expression* ethen = ite->thenExpr(i);
      varcond = varcond || (Expression::type(eif) == Type::varbool());
      if (Expression::type(eif) != Type::parbool() && Expression::type(eif) != Type::varbool()) {
        throw TypeError(_env, Expression::locDefault(eif, ite),
                        "expected bool conditional expression, got `" +
                            Expression::type(eif).toString(_env) + "'");
      }
      if (Expression::type(eif).cv()) {
        tret.cv(true);
      }

      if (auto* av = Expression::dynamicCast<AnonVar>(ethen)) {
        anons.push_back(av);
      } else {
        Type common = Type::commonType(_env, tret, Expression::type(ethen));
        if (common.isunknown()) {
          std::stringstream ss;
          ss << "incompatible branch in if-then-else. Expected `" << tret.toString(_env)
             << "' but got `" << Expression::type(ethen).toString(_env) << "'.";
          throw TypeError(_env, Expression::locDefault(ethen, ite), ss.str());
        }
        tret = common;
      }
    }

    if (ite->elseExpr() == nullptr) {
      // this is an "if <cond> then <expr> endif" so the <expr> must be bool, string, ann or array
      if (tret.isbool()) {
        ite->elseExpr(_env.constants.literalTrue);
      } else if (tret.isstring()) {
        GCLock lock;
        ite->elseExpr(new StringLit(Expression::loc(ite).introduce(), ""));
      } else if (tret.isAnn()) {
        ite->elseExpr(_env.constants.ann.empty_annotation);
      } else if (tret.dim() > 0) {
        GCLock lock;
        ite->elseExpr(new ArrayLit(Expression::loc(ite).introduce(), std::vector<Expression*>()));
        Expression::type(ite->elseExpr(), tret);
      } else {
        throw TypeError(_env, Expression::loc(ite),
                        std::string("conditional without `else' branch must have bool, string, "
                                    "ann, or array type, ") +
                            "but `then' branch has type `" + tret.toString(_env) + "'");
      }
    } else {
      if (auto* av = Expression::dynamicCast<AnonVar>(ite->elseExpr())) {
        anons.push_back(av);
      } else {
        Type common = Type::commonType(_env, tret, Expression::type(ite->elseExpr()));
        if (common.isunknown()) {
          std::stringstream ss;
          ss << "incompatible branch in if-then-else. Expected `" << tret.toString(_env)
             << "' but got `" << Expression::type(ite->elseExpr()).toString(_env) << "'.";
          throw TypeError(_env, Expression::locDefault(ite->elseExpr(), ite), ss.str());
        }
        tret = common;
      }
    }

    if (!anons.empty()) {
      if (tret.isbot()) {
        throw TypeError(_env, Expression::loc(anons[0]),
                        "if-then-else must have at least one non-anonymous branch");
      }
      if (!tret.isVarifiable(_env)) {
        throw TypeError(_env, Expression::loc(anons[0]),
                        "anonymous variable of type `" + tret.toString(_env) + "' is not allowed");
      }
      if (tret.structBT()) {
        // TODO: Fix flattening of struct type anon vars and remove this
        std::stringstream ss;
        ss << (tret.istuple() ? "tuple" : "record")
           << " type anonymous variables are not yet supported";
        throw TypeError(_env, Expression::loc(anons[0]), ss.str());
      }
      tret.mkVar(_env);
      for (auto& anon : anons) {
        anon->type(tret);
      }
    }
    if (varcond) {
      if (!tret.isVarifiable(_env)) {
        throw TypeError(
            _env, Expression::loc(ite),
            "if-then-else with var condition cannot have type `" + tret.toString(_env) + "'");
      }
      tret.mkVar(_env);
    }
    for (unsigned int i = 0; i < ite->size(); i++) {
      ite->thenExpr(i, add_coercion(_env, _model, ite->thenExpr(i), ite, tret)());
    }
    ite->elseExpr(add_coercion(_env, _model, ite->elseExpr(), ite, tret)());
    ite->type(tret);
  }
  /// Visit binary operator
  void vBinOp(BinOp* bop) {
    std::vector<Expression*> args(2);
    args[0] = bop->lhs();
    args[1] = bop->rhs();
    if (bop->op() == BOT_PLUSPLUS && (Expression::isa<TypeInst>(bop->lhs()) ||
                                      (Expression::isa<Id>(bop->lhs()) &&
                                       Expression::cast<Id>(bop->lhs())->decl()->isTypeAlias()))) {
      // Special case: concatenating type expressions

      // Check whether the rhs is also a type expression
      if (!(Expression::isa<TypeInst>(bop->rhs()) ||
            (Expression::isa<Id>(bop->rhs()) &&
             Expression::cast<Id>(bop->rhs())->decl()->isTypeAlias()))) {
        std::ostringstream ss;
        ss << "operator application for `" << bop->opToString()
           << "' cannot combine type expression`" << bop->lhs() << "' with expression `"
           << bop->rhs() << "'.";
        throw TypeError(_env, Expression::loc(bop), ss.str());
      }

      // Resolve potential type aliases
      if (auto* alias = Expression::dynamicCast<Id>(bop->lhs())) {
        bop->lhs(Expression::cast<TypeInst>(alias->decl()->e()));
      }
      if (auto* alias = Expression::dynamicCast<Id>(bop->rhs())) {
        bop->rhs(Expression::cast<TypeInst>(alias->decl()->e()));
      }

      Type lhsT = Expression::type(bop->lhs());
      Type rhsT = Expression::type(bop->rhs());
      // Check whether type expressions can be correctly combined
      if (!lhsT.structBT()) {
        std::ostringstream ss;
        ss << "operator application for `" << bop->opToString() << "' is not allowed on the `"
           << lhsT.toString(_env) << "' type instance.";
        throw TypeError(_env, Expression::loc(bop), ss.str());
      }
      if (!rhsT.structBT()) {
        std::ostringstream ss;
        ss << "operator application for `" << bop->opToString() << "' is not allowed on the `"
           << rhsT.toString(_env) << "' type instance.";
        throw TypeError(_env, Expression::loc(bop), ss.str());
      }
      if (lhsT.bt() != rhsT.bt()) {
        std::ostringstream ss;
        ss << "operator application for `" << bop->opToString() << "' cannot combine type `"
           << lhsT.toString(_env) << "' with typ `" << rhsT.toString(_env) << "'.";
        throw TypeError(_env, Expression::loc(bop), ss.str());
      }

      // Note: BinOp is resolved during typechecking of TypeInst
    } else if (bop->op() == BOT_PLUSPLUS && _env.getTransparentType(bop->lhs()).structBT() &&
               _env.getTransparentType(bop->lhs()).bt() ==
                   _env.getTransparentType(bop->rhs()).bt() &&
               _env.getTransparentType(bop->lhs()).dim() == 0 &&
               _env.getTransparentType(bop->rhs()).dim() == 0) {
      // Special case: concatenating tuples or records
      Type lhsT = Expression::type(bop->lhs());
      Type rhsT = Expression::type(bop->rhs());
      if (lhsT.isrecord()) {
        bop->type(_env.mergeRecord(lhsT, rhsT, Expression::loc(bop)));
      } else {
        assert(lhsT.istuple());
        bop->type(_env.concatTuple(lhsT, rhsT));
      }
    } else if (FunctionI* fi = _model->matchFn(_env, bop->opToString(), args, true)) {
      bop->lhs(add_coercion(_env, _model, bop->lhs(), bop, fi->argtype(_env, args, 0))());
      bop->rhs(add_coercion(_env, _model, bop->rhs(), bop, fi->argtype(_env, args, 1))());
      args[0] = bop->lhs();
      args[1] = bop->rhs();
      Type ty = fi->rtype(_env, args, bop, true);
      ty.cv(Expression::type(bop->lhs()).cv() || Expression::type(bop->rhs()).cv() || ty.cv());
      bop->type(ty);

      if (fi->e() != nullptr) {
        bop->decl(fi);
      } else {
        bop->decl(nullptr);
      }

      if (Expression::type(bop->lhs()).isint() && Expression::type(bop->rhs()).isint() &&
          Expression::type(bop->lhs()).isPresent() && Expression::type(bop->rhs()).isPresent() &&
          (bop->op() == BOT_EQ || bop->op() == BOT_GQ || bop->op() == BOT_GR ||
           bop->op() == BOT_NQ || bop->op() == BOT_LE || bop->op() == BOT_LQ)) {
        Call* call = Expression::dynamicCast<Call>(bop->lhs());
        Expression* rhs = bop->rhs();
        BinOpType bot = bop->op();
        if (call == nullptr) {
          call = Expression::dynamicCast<Call>(bop->rhs());
          rhs = bop->lhs();
          switch (bop->op()) {
            case BOT_LQ:
              bot = BOT_GQ;
              break;
            case BOT_LE:
              bot = BOT_GR;
              break;
            case BOT_GQ:
              bot = BOT_LQ;
              break;
            case BOT_GR:
              bot = BOT_LE;
              break;
            default:
              break;
          }
        }
        if ((call != nullptr) &&
            (call->id() == _env.constants.ids.count || call->id() == _env.constants.ids.sum) &&
            call->type().isvar()) {
          if (call->argCount() == 1 && Expression::isa<Comprehension>(call->arg(0))) {
            auto* comp = Expression::cast<Comprehension>(call->arg(0));
            auto* inner_bo = Expression::dynamicCast<BinOp>(comp->e());
            if (inner_bo != nullptr) {
              if (inner_bo->op() == BOT_EQ && Expression::type(inner_bo->lhs()).isint() &&
                  !Expression::type(inner_bo->lhs()).isOpt() &&
                  !Expression::type(inner_bo->rhs()).isOpt()) {
                Expression* generated = inner_bo->lhs();
                Expression* comparedTo = inner_bo->rhs();
                if (comp->containsBoundVariable(comparedTo)) {
                  if (comp->containsBoundVariable(generated)) {
                    comparedTo = nullptr;
                  } else {
                    std::swap(generated, comparedTo);
                  }
                }
                if (comparedTo != nullptr) {
                  GCLock lock;
                  ASTString cid;
                  switch (bot) {
                    case BOT_EQ:
                      cid = ASTString("count_eq");
                      break;
                    case BOT_GQ:
                      cid = ASTString("count_leq");
                      break;
                    case BOT_GR:
                      cid = ASTString("count_lt");
                      break;
                    case BOT_LQ:
                      cid = ASTString("count_geq");
                      break;
                    case BOT_LE:
                      cid = ASTString("count_gt");
                      break;
                    case BOT_NQ:
                      cid = ASTString("count_neq");
                      break;
                    default:
                      assert(false);
                  }

                  comp->e(generated);
                  Type ct = comp->type();
                  ct.bt(Expression::type(generated).bt());
                  comp->type(ct);

                  std::vector<Expression*> args({comp, comparedTo, rhs});
                  FunctionI* newCall_decl = _model->matchFn(_env, cid, args, true);
                  if (newCall_decl == nullptr) {
                    std::ostringstream ss;
                    ss << "could not replace binary operator by call to " << cid;
                    throw InternalError(ss.str());
                  }
                  Call* newCall = bop->morph(cid, args);
                  newCall->decl(newCall_decl);
                }
              }
            }
          } else if (call->argCount() == 2 && Expression::type(call->arg(0)).isIntArray() &&
                     Expression::type(call->arg(1)).isint()) {
            GCLock lock;
            ASTString cid;
            switch (bot) {
              case BOT_EQ:
                cid = ASTString("count_eq");
                break;
              case BOT_GQ:
                cid = ASTString("count_leq");
                break;
              case BOT_GR:
                cid = ASTString("count_lt");
                break;
              case BOT_LQ:
                cid = ASTString("count_geq");
                break;
              case BOT_LE:
                cid = ASTString("count_gt");
                break;
              case BOT_NQ:
                cid = ASTString("count_neq");
                break;
              default:
                assert(false);
            }
            std::vector<Expression*> args({call->arg(0), call->arg(1), rhs});
            FunctionI* newCall_decl = _model->matchFn(_env, cid, args, true);
            if (newCall_decl == nullptr) {
              std::ostringstream ss;
              ss << "could not replace binary operator by call to " << cid;
              throw InternalError(ss.str());
            }
            Call* newCall = bop->morph(cid, args);
            newCall->decl(newCall_decl);
          }
        }
      }
    } else {
      std::ostringstream ss;
      ss << "type error in operator application for `" << bop->opToString()
         << "'. No matching operator found with left-hand side type `"
         << Expression::type(bop->lhs()).toString(_env) << "' and right-hand side type `"
         << Expression::type(bop->rhs()).toString(_env) << "'";
      throw TypeError(_env, Expression::loc(bop), ss.str());
    }
  }
  /// Visit unary operator
  void vUnOp(UnOp* uop) {
    std::vector<Expression*> args(1);
    args[0] = uop->e();
    if (FunctionI* fi = _model->matchFn(_env, uop->opToString(), args, true)) {
      uop->e(add_coercion(_env, _model, uop->e(), uop, fi->argtype(_env, args, 0))());
      args[0] = uop->e();
      Type ty = fi->rtype(_env, args, uop, true);
      ty.cv(Expression::type(uop->e()).cv() || ty.cv());
      uop->type(ty);
      if (fi->e() != nullptr) {
        uop->decl(fi);
      }
    } else {
      std::ostringstream ss;
      ss << "type error in operator application for `" << uop->opToString()
         << "'. No matching operator found with type `" << Expression::type(uop->e()).toString(_env)
         << "'";
      throw TypeError(_env, Expression::loc(uop), ss.str());
    }
  }

  /// Visit call
  void vCall(Call* call) {
    std::vector<Expression*> args(call->argCount());
    for (auto i = static_cast<unsigned int>(args.size()); (i--) != 0U;) {
      args[i] = call->arg(i);
    }
    FunctionI* fi = _model->matchFn(_env, call, true, true);

    if (fi != nullptr && fi->id() == _env.constants.ids.symmetry_breaking_constraint &&
        fi->paramCount() == 1 && fi->param(0)->type().isbool()) {
      GCLock lock;
      call->id(_env.constants.ids.mzn_symmetry_breaking_constraint);
      fi = _model->matchFn(_env, call, true, true);
    } else if (fi != nullptr &&
               (fi->id() == _env.constants.ids.redundant_constraint ||
                fi->id() == _env.constants.ids.implied_constraint) &&
               fi->paramCount() == 1 && fi->param(0)->type().isbool()) {
      GCLock lock;
      call->id(_env.constants.ids.mzn_redundant_constraint);
      fi = _model->matchFn(_env, call, true, true);
    }

    // Rewrite multi-dimensional search into internal version to flatten arrays
    if (!_isFlatZinc && fi != nullptr && fi->e() == nullptr && fi->paramCount() >= 1) {
      Type p1 = fi->param(0)->type();
      if (p1.dim() != 0) {
        ASTString cid = call->id();
        ASTString nid;
        if (cid == _env.constants.ann.bool_search) {
          nid = _env.constants.ann.bool_search_internal;
        } else if (cid == _env.constants.ann.float_search) {
          nid = _env.constants.ann.float_search_internal;
        } else if (cid == _env.constants.ann.int_search) {
          nid = _env.constants.ann.int_search_internal;
        } else if (cid == _env.constants.ann.seq_search) {
          nid = _env.constants.ann.seq_search_internal;
        } else if (cid == _env.constants.ann.set_search) {
          nid = _env.constants.ann.set_search_internal;
        } else if (cid == _env.constants.ann.warm_start) {
          nid = _env.constants.ann.warm_start_internal;
        } else if (cid == _env.constants.ann.warm_start_array) {
          nid = _env.constants.ann.warm_start_array_internal;
        }

        if (!nid.empty() &&
            !Expression::ann(call).contains(_env.constants.ann.mzn_internal_representation)) {
          call->id(nid);
          fi = _model->matchFn(_env, call, true, true);
        }
      }
    }

    if ((fi->e() != nullptr) && Expression::isa<Call>(fi->e())) {
      Call* next_call = Expression::cast<Call>(fi->e());
      if ((next_call->decl() != nullptr) && next_call->argCount() == fi->paramCount() &&
          _model->sameOverloading(_env, args, fi, next_call->decl())) {
        bool macro = true;
        for (unsigned int i = 0; i < fi->paramCount(); i++) {
          if (!Expression::equal(next_call->arg(i), fi->param(i)->id())) {
            macro = false;
            break;
          }
        }
        if (macro) {
          // Call is not a macro if it has a reification implementation
          GCLock lock;
          ASTString reif_id = _env.reifyId(fi->id());
          std::vector<Type> tt(fi->paramCount() + 1);
          for (unsigned int i = 0; i < fi->paramCount(); i++) {
            tt[i] = fi->param(i)->type();
          }
          tt[fi->paramCount()] = Type::varbool();

          macro = _model->matchFn(_env, reif_id, tt, true) == nullptr;
        }
        if (macro) {
          call->decl(next_call->decl());
          for (ExpressionSetIter esi = Expression::ann(next_call).begin();
               esi != Expression::ann(next_call).end(); ++esi) {
            Expression::addAnnotation(call, *esi);
          }
          call->rehash();
          fi = next_call->decl();
        }
      }
    }

    bool cv = false;
    for (unsigned int i = 0; i < args.size(); i++) {
      if (auto* c = Expression::dynamicCast<Comprehension>(call->arg(i))) {
        GCLock lock;
        Expression* c_e = c->e();
        ArrayLit* indexTuple = nullptr;
        if (Expression::isa<ArrayLit>(c_e) && Expression::cast<ArrayLit>(c_e)->isTuple() &&
            Expression::type(c_e).typeId() == Type::COMP_INDEX) {
          indexTuple = Expression::cast<ArrayLit>(c_e);
          c_e = (*indexTuple)[indexTuple->size() - 1];
        }
        Type t_before = Expression::type(c_e);
        Type t = fi->argtype(_env, args, i).elemType(_env);
        // Call to enum2int would have been removed when processing the comprehension,
        // so don't warn here
        bool warnImplicitEnum2Int = _env.warnImplicitEnum2Int;
        _env.warnImplicitEnum2Int = false;
        c_e = add_coercion(_env, _model, c_e, c, t)();
        _env.warnImplicitEnum2Int = warnImplicitEnum2Int;
        Type t_after = Expression::type(c_e);
        if (t_before != t_after) {
          if (indexTuple != nullptr) {
            std::vector<Expression*> indexes(indexTuple->size());
            for (unsigned int i = 0; i < indexTuple->size() - 1; i++) {
              indexes[i] = (*indexTuple)[i];
            }
            indexes[indexTuple->size() - 1] = c_e;
            c_e = ArrayLit::constructTuple(Expression::loc(indexTuple), indexes);
          }
          c->e(c_e);
          Type ct = c->type();
          ct.bt(t_after.bt());
          c->type(ct);
        }
      } else {
        args[i] = add_coercion(_env, _model, call->arg(i), call, fi->argtype(_env, args, i))();
        call->arg(i, args[i]);
      }
      cv = cv || Expression::type(args[i]).cv();
    }
    if (call->id() == _env.constants.ids.enumOf) {
      auto enumId = Expression::type(call->arg(0)).typeId();
      if (enumId != 0 && Expression::type(call->arg(0)).dim() != 0) {
        const auto& enumIds = _env.getArrayEnum(enumId);
        enumId = enumIds[enumIds.size() - 1];
      }
      if (enumId != 0) {
        call->id(_env.constants.ids.enumOfInternal);
        VarDecl* enumDecl = _env.getEnum(enumId)->e();
        call->arg(0, enumDecl->id());
        fi = _model->matchFn(_env, call, false, true);
      }
    }

    // Set type and decl
    Type ty = fi->rtype(_env, args, call, true);
    ty.cv(cv || ty.cv());
    call->type(ty);

    if (Call* deprecated = fi->ann().getCall(_env.constants.ann.mzn_deprecated)) {
      // rewrite this call into a call to mzn_deprecate(..., e)
      GCLock lock;
      std::vector<Expression*> params(call->argCount());
      for (unsigned int i = 0; i < params.size(); i++) {
        params[i] = call->arg(i);
      }
      Call* origCall = Call::a(Expression::loc(call), call->id(), params);
      origCall->type(ty);
      origCall->decl(fi);
      call->id(_env.constants.ids.mzn_deprecate);
      std::vector<Expression*> args(
          {new StringLit(Location(), fi->id()), deprecated->arg(0), deprecated->arg(1), origCall});
      call->args(args);
      FunctionI* deprecated_fi = _model->matchFn(_env, call, false, true);
      call->decl(deprecated_fi);
    } else {
      call->decl(fi);
    }
  }
  /// Visit let
  void vLet(Let* let) {
    bool cv = false;
    bool isVar = false;
    std::vector<Expression*> letOrig;
    for (unsigned int i = 0; i < let->let().size(); i++) {
      Expression* li = let->let()[i];
      cv = cv || Expression::type(li).cv();
      if (auto* vdi = Expression::dynamicCast<VarDecl>(li)) {
        if (vdi->e() == nullptr && vdi->type().isSet() && vdi->type().isvar() &&
            vdi->ti()->domain() == nullptr) {
          std::ostringstream ss;
          ss << "set element type for `" << vdi->id()->str() << "' is not finite";
          _typeErrors.emplace_back(_env, Expression::loc(vdi), ss.str());
        }
        if (vdi->type().isPar() && (vdi->e() == nullptr)) {
          std::ostringstream ss;
          ss << "let variable `" << vdi->id()->v() << "' must be initialised";
          throw TypeError(_env, Expression::loc(vdi), ss.str());
        }
        if (vdi->ti()->hasTiVariable()) {
          std::ostringstream ss;
          ss << "type-inst variables not allowed in type-inst for let variable `"
             << vdi->id()->str() << "'";
          _typeErrors.emplace_back(_env, Expression::loc(vdi), ss.str());
        }
        letOrig.push_back(vdi->e());
        for (unsigned int k = 0; k < vdi->ti()->ranges().size(); k++) {
          letOrig.push_back(vdi->ti()->ranges()[k]->domain());
        }
      } else {
        if (!_env.isSubtype(Expression::type(let->let()[i]), Type::varbool(), true)) {
          const Location* errLoc = &Expression::loc(let->let()[i]);
          if (errLoc->isNonAlloc()) {
            errLoc = &Expression::loc(let);
          }
          _typeErrors.emplace_back(_env, *errLoc,
                                   "invalid type of constraint, expected `" +
                                       Type::varbool().toString(_env) + "', actual `" +
                                       Expression::type(let->let()[i]).toString(_env) + "'");
        }
      }
      isVar |= Expression::type(li).isvar();
    }
    {
      GCLock lock;
      let->setLetOrig(ASTExprVec<Expression>(letOrig));
    }
    let->in(add_coercion(_env, _model, let->in(), let, Expression::type(let->in()))());
    Type ty = Expression::type(let->in());
    ty.cv(cv || ty.cv());
    if (isVar && ty.bt() == Type::BT_BOOL && ty.dim() == 0) {
      ty.mkVar(_env);
    }
    let->type(ty);
  }
  /// Visit variable declaration
  void vVarDecl(VarDecl* vd) {
    if (vd->isTypeAlias()) {
      // Resolve any aliases in the alias definition
      bool resolved = Expression::cast<TypeInst>(vd->e())->resolveAlias(_env);
      if (resolved) {
        // Recheck the TypeInst (might now contain a problem)
        vTypeInst(Expression::cast<TypeInst>(vd->e()));
      }
      return;
    }
    vd->type(vd->ti()->type());
    if (ignoreVarDecl) {
      if (vd->e() != nullptr) {
        Type vdt = vd->ti()->type();
        Type vet = _env.getTransparentType(vd->e());
        if (!vdt.any() && vdt.typeId() != 0 && vdt.dim() > 0 &&
            (Expression::isa<ArrayLit>(vd->e()) || Expression::isa<Comprehension>(vd->e()) ||
             (Expression::isa<BinOp>(vd->e()) &&
              Expression::cast<BinOp>(vd->e())->op() == BOT_PLUSPLUS))) {
          // Special case: index sets of array literals and comprehensions automatically
          // coerce to any enum index set
          const std::vector<unsigned int>& enumIds = _env.getArrayEnum(vdt.typeId());
          if (enumIds[enumIds.size() - 1] == 0) {
            vdt.typeId(0);
          } else {
            std::vector<unsigned int> nEnumIds(enumIds.size());
            for (unsigned int i = 0; i < nEnumIds.size() - 1; i++) {
              nEnumIds[i] = 0;
            }
            nEnumIds[nEnumIds.size() - 1] = enumIds[enumIds.size() - 1];
            vdt.typeId(_env.registerArrayEnum(nEnumIds));
          }
        } else if (vd->ti()->isEnum() && Expression::isa<Call>(vd->e())) {
          if (Expression::cast<Call>(vd->e())->id() == _env.constants.ids.anon_enum) {
            vet.typeId(vdt.typeId());
          }
        }
        if (vd->type().any() || vd->type().isunknown()) {
          if (vd->type().any()) {
            anyInLet.insert(vd);
            if (vet.isbot()) {
              const Location& loc = Expression::loc(vd->e()).isNonAlloc()
                                        ? Expression::loc(vd)
                                        : Expression::loc(vd->e());
              _typeErrors.emplace_back(_env, loc, "Cannot infer type for declaration");
            }
          }
          vd->ti()->type(vet);
          vd->type(vet);
          if (vdt.any()) {
            if (vet.structBT()) {
              vd->ti()->setStructDomain(_env, vet);
            } else if (vet.dim() > 0) {
              GCLock lock;
              std::vector<TypeInst*> ranges(vet.dim());
              for (int i = 0; i < vet.dim(); i++) {
                ranges[i] = new TypeInst(Location().introduce(), Type::parint());
              }
              vd->ti()->setRanges(ranges);
            }
          }
        } else if (vet.isbot() && vet.dim() != 0 && vdt.dim() > 1 &&
                   (vet.dim() == 1 || vet.dim() == vdt.dim()) &&
                   Expression::isa<ArrayLit>(vd->e()) &&
                   Expression::cast<ArrayLit>(vd->e())->empty()) {
          // Replace [] with empty array literal of the correct dimensions
          GCLock lock;
          std::vector<Expression*> args;
          args.reserve(vdt.dim() + 1);
          for (auto* r : vd->ti()->ranges()) {
            if (r->domain() != nullptr) {
              args.push_back(r->domain());
              r->domain(nullptr);
            } else {
              args.push_back(new SetLit(Location().introduce(), IntSetVal::a()));
            }
          }
          args.push_back(vd->e());
          const auto& ident = _env.constants.ids.arrayNd(vd->type().dim());
          auto* call = Call::a(Location().introduce(), ident, args);
          call->type(vd->type());
          call->decl(_env.model->matchFn(_env, call, false));
          vd->e(call);
        } else if (!_env.isSubtype(vet, vdt, true)) {
          if (vd->ti()->isEnum() && vet == Type::parsetint()) {
            // let's ignore this for now (TODO: add an annotation to make sure only
            // compiler-generated ones are accepted)
          } else {
            const Location& loc = Expression::loc(vd->e()).isNonAlloc() ? Expression::loc(vd)
                                                                        : Expression::loc(vd->e());
            std::ostringstream ss;
            ss << "initialisation value for `" << vd->id()->str()
               << "' has invalid type-inst: expected `" << vd->ti()->type().toString(_env)
               << "', actual `" << Expression::type(vd->e()).toString(_env) << "'";
            _typeErrors.emplace_back(_env, loc, ss.str());
          }
        }
        vd->e(add_coercion(_env, _model, vd->e(), vd, vd->ti()->type())());
        vet = Expression::type(vd->e());
        if (vd->type().dim() > 0) {
          if (vet.typeId() != 0) {
            // check if the VarDecl has _ as index sets and copy correct enum information
            const std::vector<unsigned int>& enumIds = _env.getArrayEnum(vet.typeId());
            std::vector<unsigned int> vdEnumIds(vd->type().dim() + 1, 0);
            if (vd->type().typeId() != 0) {
              vdEnumIds = _env.getArrayEnum(vd->type().typeId());
            }
            bool hadAnonVar = false;
            for (unsigned int i = 0; i < vd->ti()->ranges().size(); i++) {
              auto* av = Expression::dynamicCast<AnonVar>(vd->ti()->ranges()[i]->domain());
              if (av != nullptr) {
                if (enumIds[i] != vdEnumIds[i]) {
                  vdEnumIds[i] = enumIds[i];
                  hadAnonVar = true;
                }
                vd->ti()->ranges()[i]->domain(nullptr);
              }
            }
            if (hadAnonVar) {
              int arrayEnumId = _env.registerArrayEnum(vdEnumIds);
              Type t = vd->type();
              t.typeId(arrayEnumId);
              vd->ti()->type(t);
              vd->type(t);
            }
          } else {
            // remove all _ in array index sets
            for (unsigned int i = 0; i < vd->ti()->ranges().size(); i++) {
              auto* av = Expression::dynamicCast<AnonVar>(vd->ti()->ranges()[i]->domain());
              if (av != nullptr) {
                vd->ti()->ranges()[i]->domain(nullptr);
              }
            }
          }
        }
      } else {
        assert(!vd->type().isunknown());
      }
      // Check that annotations are type correct if they have an annotated_expression argument
      for (auto* e : Expression::ann(vd)) {
        std::vector<Expression*> addAnnArgs;
        ASTString addAnnId;
        if (auto* ident = Expression::dynamicCast<Id>(e)) {
          if (Expression::ann(ident->decl())
                  .containsCall(_env.constants.ann.mzn_add_annotated_expression)) {
            addAnnArgs = {vd->id()};
            addAnnId = ident->str();
          }
        } else if (auto* c = Expression::dynamicCast<Call>(e)) {
          if (c->decl()->ann().containsCall(_env.constants.ann.mzn_add_annotated_expression)) {
            Call* addAnnExp =
                c->decl()->ann().getCall(_env.constants.ann.mzn_add_annotated_expression);
            auto annotatedExpressionIdx =
                static_cast<unsigned int>(eval_int(_env, addAnnExp->arg(0)).toInt());
            addAnnArgs.resize(c->argCount() + 1);
            for (unsigned int i = 0, j = 0; i < c->argCount(); i++) {
              if (j == annotatedExpressionIdx) {
                addAnnArgs[j++] = vd->id();
              }
              addAnnArgs[j++] = c->arg(i);
            }
            if (annotatedExpressionIdx == c->argCount()) {
              addAnnArgs[c->argCount()] = vd->id();
            }
            addAnnId = c->id();
          }
        }
        if (!addAnnArgs.empty()) {
          GCLock lock;
          Call* nc = Call::a(Expression::loc(e), addAnnId, addAnnArgs);
          FunctionI* fi = _model->matchFn(_env, nc, true, true);
        }
      }
    }
  }
  /// Visit type inst
  void vTypeInst(TypeInst* ti) {
    /// Resolve type aliasing
    ti->resolveAlias(_env);
    Type tt = ti->type();
    bool needsArrayType = false;
    // !ti->ranges().empty() && (ti->domain() != nullptr) && ti->domain()->type().typeId() != 0;
    if (!ti->ranges().empty()) {
      bool foundTIId = false;
      for (unsigned int i = 0; i < ti->ranges().size(); i++) {
        TypeInst* ri = ti->ranges()[i];
        assert(ri != nullptr);
        if (ri->type().cv()) {
          tt.cv(true);
        }
        if (ri->type().typeId() != 0) {
          needsArrayType = true;
        }
        if (ri->type() == Type::top()) {
          //            if (foundTIId) {
          //              throw TypeError(_env,Expression::loc(ri),
          //                "only one type-inst variable allowed in array index");
          //            } else {
          foundTIId = true;
          //            }
        } else if (ri->type() != Type::parint()) {
          assert(Expression::isa<TypeInst>(ri));
          auto* riti = Expression::cast<TypeInst>(ri);
          if (riti->domain() != nullptr) {
            throw TypeError(_env, Expression::loc(ri),
                            "array index set expression has invalid type, expected `set of int', "
                            "actual `set of " +
                                ri->type().toString(_env) + "'");
          }
          throw TypeError(_env, Expression::loc(ri),
                          "cannot use `" + ri->type().toString(_env) +
                              "' as array index set (did you mean `int'?)");
        }
      }
      unsigned int tId = tt.typeId();
      tt.typeId(0);
      tt.dim(foundTIId ? -1 : static_cast<int>(ti->ranges().size()));
      tt.typeId(tId);
    }
    if ((ti->domain() != nullptr) && Expression::type(ti->domain()).cv()) {
      tt.cv(true);
    }
    if (ti->domain() != nullptr) {
      if (Expression::type(ti->domain()).typeId() != 0) {
        needsArrayType = needsArrayType || !ti->ranges().empty();
      }
      if (ti->concatDomain(_env)) {
        tt = ti->type();
        // Concat domain has now added a typeId when combining domains
        needsArrayType = needsArrayType || !ti->ranges().empty();
      } else if (ti->type().bt() == Type::BT_TUPLE) {
        if (tt.isOpt()) {
          throw TypeError(_env, Expression::loc(ti), "opt tuples are not allowed");
        }
        needsArrayType = false;  // will be registered by registerTupleType
        // Register and cononicalise tuple type
        ti->canonicaliseStruct(_env);
        tt = ti->type();
      } else if (ti->type().bt() == Type::BT_RECORD) {
        if (tt.isOpt()) {
          throw TypeError(_env, Expression::loc(ti), "opt records are not allowed");
        }
        needsArrayType = false;  // will be registered by registerRecordType
        // Register and cononicalise record type
        ti->canonicaliseStruct(_env);
        tt = ti->type();
      } else if (TIId* tiid = Expression::dynamicCast<TIId>(ti->domain())) {
        if (tiid->isEnum()) {
          tt.bt(Type::BT_INT);
        }
      } else if (Expression::isa<AnonVar>(ti->domain())) {
        tt.bt(Type::BT_INT);
      } else {
        if (Expression::type(ti->domain()).ti() != Type::TI_PAR ||
            Expression::type(ti->domain()).st() != Type::ST_SET) {
          throw TypeError(_env,
                          Expression::loc(ti->domain()).isNonAlloc()
                              ? Expression::loc(ti)
                              : Expression::loc(ti->domain()),
                          "type-inst must be par set but is `" +
                              Expression::type(ti->domain()).toString(_env) + "'");
        }
        if (Expression::type(ti->domain()).dim() != 0) {
          throw TypeError(_env, Expression::loc(ti->domain()), "type-inst cannot be an array");
        }
      }
    }
    if (tt.isunknown() && (ti->domain() != nullptr)) {
      assert(ti->domain());
      switch (Expression::type(ti->domain()).bt()) {
        case Type::BT_INT:
        case Type::BT_FLOAT:
          break;
        case Type::BT_BOT: {
          Type tidt = Expression::type(ti->domain());
          tidt.bt(Type::BT_INT);
          Expression::type(ti->domain(), tidt);
        } break;
        default:
          throw TypeError(_env, Expression::loc(ti->domain()), "type-inst must be int or float");
      }
      tt.bt(Expression::type(ti->domain()).bt());
      tt.typeId(Expression::type(ti->domain()).typeId());
    } else {
      //        assert(ti->domain()==NULL || ti->domain()->isa<TIId>());
    }
    if (needsArrayType) {
      assert(tt.dim() != 0);
      std::vector<unsigned int> typeIds(ti->ranges().size() + 1);
      for (unsigned int i = 0; i < ti->ranges().size(); i++) {
        typeIds[i] = ti->ranges()[i]->type().typeId();
      }
      typeIds[ti->ranges().size()] =
          ti->domain() != nullptr ? Expression::type(ti->domain()).typeId() : 0;
      int arrayTypeId = _env.registerArrayEnum(typeIds);
      tt.typeId(arrayTypeId);
    }

    if (tt.st() == Type::ST_SET && tt.ti() == Type::TI_VAR &&
        ((tt.bt() != Type::BT_INT && tt.bt() != Type::BT_TOP) || tt.isOpt())) {
      throw TypeError(_env, Expression::loc(ti),
                      "var set element types other than `int' not allowed");
    }
    if (tt.isvar() && (tt.bt() == Type::BT_ANN || tt.bt() == Type::BT_STRING ||
                       (tt.isOpt() && tt.st() == Type::ST_SET))) {
      throw TypeError(_env, Expression::loc(ti),
                      "invalid type of variable declaration: `" + tt.toString(_env) + "'");
    }

    ti->type(tt);
  }
  void vTIId(TIId* id) {}
};

void create_par_versions(Env& env, Model* m, BottomUpIterator<Typer<true>>& bottomUpTyper) {
  // Create a par version of each function that returns par and
  // that has a body that can be made par
  std::unique_ptr<Model> nonPolyFns(new Model);
  std::vector<FunctionI*> fns;
  std::unordered_map<FunctionI*, std::pair<bool, std::vector<FunctionI*>>> fnsToMakePar;
  for (auto& f : m->functions()) {
    if (f.id() == env.envi().constants.ids.mzn_reverse_map_var ||
        f.ti()->type().bt() == Type::BT_ANN) {
      continue;
    }
    if (f.e() == nullptr) {
      if (f.isPolymorphic() || f.ti()->type().isPar()) {
        // Builtin should be able to be called from a par function
        fnsToMakePar.insert({&f, {true, std::vector<FunctionI*>()}});
      } else {
        continue;
      }
    } else if (f.isPolymorphic()) {
      // Don't try to make polymorphic function par, just do this to the concrete instances
      continue;
    }
    nonPolyFns->registerFn(env.envi(), &f);
    fns.push_back(&f);
  }
  nonPolyFns->sortFn(env.envi());
  for (auto* f : fns) {
    bool alreadyPar = Expression::type(f->ti()).isPar();
    if (alreadyPar) {
      for (unsigned int i = 0; i < f->paramCount(); i++) {
        if (f->param(i)->type().isvar()) {
          alreadyPar = false;
          break;
        }
      }
    }
    if (alreadyPar) {
      fnsToMakePar.insert({f, {true, std::vector<FunctionI*>()}});
      continue;
    }
    // create par version of parameter types
    std::vector<Type> tv(f->paramCount());
    for (unsigned int i = 0; i < f->paramCount(); i++) {
      Type t = f->param(i)->type();
      t.mkPar(env.envi());
      t.cv(false);
      tv[i] = t;
    }
    // check if specialised par version of function already exists
    FunctionI* fi_par = nonPolyFns->matchFn(env.envi(), f->id(), tv, false);
    alreadyPar = fi_par->ti()->type().isPar();
    if (alreadyPar) {
      for (unsigned int i = 0; i < fi_par->paramCount(); i++) {
        if (fi_par->param(i)->type().isvar()) {
          alreadyPar = false;
          break;
        }
      }
    }
    if (alreadyPar) {
      fnsToMakePar.insert({fi_par, {true, std::vector<FunctionI*>()}});
      continue;
    }
    if (fi_par->e() == nullptr) {
      continue;
    }
    // check if body of f doesn't contain any free variables in lets,
    // all calls in the body have par versions available,
    // and all toplevel identifiers used in the body of f are par
    class CheckParBody : public EVisitor {
    public:
      EnvI& env;
      Model* m;
      CheckParBody(EnvI& env0, Model* m0) : env(env0), m(m0) {}
      bool isPar = true;
      std::vector<FunctionI*> deps;
      bool enter(Expression* e) const {
        // if we have already found a var, don't continue
        return isPar;
      }
      void vId(const Id* ident) {
        if (ident->decl() != nullptr && (ident->type().isvar() || ident->type().cv()) &&
            ident->decl()->toplevel()) {
          isPar = false;
        }
      }
      void vLet(const Let* let) {
        // check if any of the declared variables does not have a RHS
        for (auto* e : let->let()) {
          if (auto* vd = Expression::dynamicCast<VarDecl>(e)) {
            if (vd->e() == nullptr) {
              isPar = false;
              break;
            }
          }
        }
      }
      void vCall(const Call* c) {
        if (!c->type().isAnn()) {
          FunctionI* decl = c->decl();
          // create par version of parameter types
          std::vector<Type> tv;
          for (unsigned int i = 0; i < decl->paramCount(); i++) {
            Type t = decl->param(i)->type();
            t.mkPar(env);
            tv.push_back(t);
          }
          FunctionI* decl_par = m->matchFn(env, decl->id(), tv, false);
          if (decl_par == nullptr) {
            isPar = false;
            return;
          }
          deps.push_back(decl_par);
        }
      }
    } cpb(env.envi(), &*nonPolyFns);
    top_down(cpb, fi_par->e());
    if (cpb.isPar) {
      fnsToMakePar.insert({fi_par, {false, cpb.deps}});
    }
  }

  // Repeatedly remove functions whose dependencies cannot be made par
  bool didRemove;
  do {
    didRemove = false;
    std::vector<FunctionI*> toRemove;
    for (auto& p : fnsToMakePar) {
      for (auto* dep : p.second.second) {
        if (fnsToMakePar.find(dep) == fnsToMakePar.end()) {
          toRemove.push_back(p.first);
        }
      }
    }
    if (!toRemove.empty()) {
      didRemove = true;
      for (auto* p : toRemove) {
        fnsToMakePar.erase(p);
      }
    }
  } while (didRemove);

  // Create par versions of remaining functions
  if (!fnsToMakePar.empty()) {
    // First step: copy and register functions
    std::vector<FunctionI*> parFunctions;
    CopyMap parCopyMap;
    // Step 1a: enter all global declarations into copy map
    class EnterGlobalDecls : public EVisitor {
    public:
      CopyMap& cm;
      EnterGlobalDecls(CopyMap& cm0) : cm(cm0) {}
      void vId(Id* ident) {
        if (ident->decl() != nullptr && ident->decl()->toplevel()) {
          cm.insert(ident->decl(), ident->decl());
        }
      }
    } _egd(parCopyMap);
    for (auto& p : fnsToMakePar) {
      if (!p.second.first) {
        for (unsigned int i = 0; i < p.first->paramCount(); i++) {
          top_down(_egd, p.first->param(i));
        }
        if (p.first->capturedAnnotationsVar() != nullptr) {
          top_down(_egd, p.first->capturedAnnotationsVar());
        }
        for (ExpressionSetIter i = p.first->ann().begin(); i != p.first->ann().end(); ++i) {
          top_down(_egd, *i);
        }
        top_down(_egd, p.first->e());
      }
    }

    // Step 1b: copy functions
    for (auto& p : fnsToMakePar) {
      if (!p.second.first) {
        GCLock lock;
        auto* cp = copy(env.envi(), parCopyMap, p.first)->cast<FunctionI>();
        for (unsigned int i = 0; i < cp->paramCount(); i++) {
          VarDecl* v = cp->param(i);
          v->ti()->mkPar(env.envi());
          v->type(v->ti()->type());
        }
        cp->ti()->mkPar(env.envi());
        bool didRegister = m->registerFn(env.envi(), cp, true, false);
        if (didRegister) {
          m->addItem(cp);
          parFunctions.push_back(cp);
        } else {
          // A function with par parameters already exists.
          // This can only happen this can be made par, but the return type was given as var.
          // Therefore, we can just set the return type to par.
          assert(p.first->ti()->type().isvar());
          p.first->ti()->mkPar(env.envi());
          parFunctions.push_back(p.first);
        }
      }
    }

    // Second step: make function bodies par
    // (needs to happen in a separate second step so that
    //  matchFn will find the correct par function from first step)
    class MakeFnPar : public EVisitor {
    public:
      EnvI& env;
      Model* m;
      MakeFnPar(EnvI& env0, Model* m0) : env(env0), m(m0) {}
      bool enter(Expression* e) {
        Type t(Expression::type(e));
        t.mkPar(env);
        t.cv(false);
        Expression::type(e, t);
        return true;
      }
      void vCall(Call* c) {
        FunctionI* decl = m->matchFn(env, c, false);
        c->decl(decl);
      }
      void vBinOp(BinOp* bo) {
        if (bo->decl() != nullptr) {
          std::vector<Type> ta(2);
          ta[0] = Expression::type(bo->lhs());
          ta[1] = Expression::type(bo->rhs());
          FunctionI* decl = m->matchFn(env, bo->opToString(), ta, false);
          bo->decl(decl);
        }
      }
      void vUnOp(UnOp* uo) {
        if (uo->decl() != nullptr) {
          std::vector<Type> ta(1);
          ta[0] = Expression::type(uo->e());
          FunctionI* decl = m->matchFn(env, uo->opToString(), ta, false);
          uo->decl(decl);
        }
      }
    } _mfp(env.envi(), m);

    for (auto* p : parFunctions) {
      bottom_up(_mfp, p->e());
    }

    for (auto* p : parFunctions) {
      // type-check body again, to enable rewriting of calls like "show"
      bottomUpTyper.run(p->e());
      assert(Expression::type(p->e()).isPar());
    }
  }
}

void typecheck(Env& env, Model* origModel, std::vector<TypeError>& typeErrors,
               bool ignoreUndefinedParameters, bool allowMultiAssignment, bool isFlatZinc) {
  auto isChecker =
      origModel->filename().endsWith(".mzc") || origModel->filename().endsWith(".mzc.mzn");

  Model* m;
  if (!isFlatZinc && origModel == env.model()) {
    // Combine all items into single model
    auto* combinedModel = new Model;
    class Combiner : public ItemVisitor {
    public:
      Model* m;
      Combiner(Model* m0) : m(m0) {}
      bool enter(Item* i) const {
        if (!i->isa<IncludeI>()) {
          m->addItem(i);
        }
        return true;
      }
    } _combiner(combinedModel);
    iter_items(_combiner, origModel);
    env.envi().originalModel = origModel;
    env.envi().model = combinedModel;
    m = combinedModel;
  } else {
    m = origModel;
  }

  // Topological sorting
  IdMap<bool> needToString;
  std::vector<Call*> enumConstructorSetTypes;
  TopoSorter ts(m, needToString, enumConstructorSetTypes);

  std::vector<FunctionI*> functionItems;
  std::vector<AssignI*> assignItems;
  std::unique_ptr<Model> annotatedExpressionItems(new Model);
  auto* enumItems = new Model;

  class TSVFuns : public ItemVisitor {
  public:
    EnvI& env;
    Model* model;
    Model& toAdd;
    std::vector<FunctionI*>& fis;
    std::vector<TypeError>& typeErrors;
    ASTStringSet reifiedAnnotationIds;
    TSVFuns(EnvI& env0, Model* model0, std::vector<FunctionI*>& fis0, Model& toAdd0,
            std::vector<TypeError>& typeErrors0)
        : env(env0), model(model0), fis(fis0), toAdd(toAdd0), typeErrors(typeErrors0) {}
    void vFunctionI(FunctionI* i) {
      fis.push_back(i);
      // check if one of the arguments is annotated with ::annotated_expression
      unsigned int reifiedAnnotationIdx = 0;
      bool foundReifiedAnnotation = false;
      for (unsigned int j = 0; j < i->paramCount(); j++) {
        Expression* param = i->param(j);
        for (auto* ii : Expression::ann(param)) {
          if (Expression::isa<Id>(ii) &&
              Expression::cast<Id>(ii)->v() == env.constants.ann.annotated_expression->v()) {
            if (j != 0) {
              typeErrors.emplace_back(
                  env, Expression::loc(param),
                  "only the first argument can be annotated with annotated_expression");
            }
            reifiedAnnotationIdx = j;
            foundReifiedAnnotation = true;
          }
        }
      }
      if (foundReifiedAnnotation) {
        GCLock lock;
        if (i->paramCount() == 1) {
          // turn into atomic annotation
          if (reifiedAnnotationIds.find(i->id()) == reifiedAnnotationIds.end()) {
            auto* ti = new TypeInst(Location().introduce(), Type::ann());
            auto* vd = new VarDecl(Location().introduce(), ti, i->id());
            Expression::ann(vd).add(Call::a(Location().introduce(),
                                            env.constants.ann.mzn_add_annotated_expression,
                                            {IntLit::a(0)}));
            toAdd.addItem(VarDeclI::a(Location().introduce(), vd));
            reifiedAnnotationIds.insert(i->id());
          }
        } else {
          // turn into annotation function with one argument less
          std::vector<VarDecl*> newParams(i->paramCount() - 1);
          int j = 0;
          for (unsigned int k = 0; k < i->paramCount(); k++) {
            if (k != reifiedAnnotationIdx) {
              newParams[j++] = Expression::cast<VarDecl>(copy(env, i->param(k)));
            }
          }
          auto* fi = new FunctionI(Location().introduce(), i->id(), i->ti(), newParams);
          fi->ann().add(Call::a(Location().introduce(),
                                env.constants.ann.mzn_add_annotated_expression,
                                {IntLit::a(reifiedAnnotationIdx)}));
          toAdd.addItem(fi);
          fis.push_back(fi);
        }
      }
    }
  } _tsvf(env.envi(), m, functionItems, *annotatedExpressionItems, typeErrors);
  iter_items(_tsvf, m);
  for (auto* it : *annotatedExpressionItems) {
    m->addItem(it);  // Add the new items now that we've finished iterating
  }

  class TSV0 : public ItemVisitor {
  public:
    EnvI& env;
    TopoSorter& ts;
    Model* model;
    bool hadSolveItem;
    std::vector<AssignI*>& ais;
    VarDeclI* objective;
    Model* objectiveModel;
    Model* enumis;
    bool isFlatZinc;
    bool isChecker;
    std::vector<TypeError>& typeErrors;
    TSV0(EnvI& env0, TopoSorter& ts0, Model* model0, std::vector<AssignI*>& ais0, Model* enumis0,
         bool isFlatZinc0, bool isChecker0, std::vector<TypeError>& typeErrors0)
        : env(env0),
          ts(ts0),
          model(model0),
          hadSolveItem(false),
          ais(ais0),
          objective(nullptr),
          objectiveModel(new Model),
          enumis(enumis0),
          isFlatZinc(isFlatZinc0),
          isChecker(isChecker0),
          typeErrors(typeErrors0) {}
    ~TSV0() { delete objectiveModel; }
    void vAssignI(AssignI* i) { ais.push_back(i); }
    void vVarDeclI(VarDeclI* i) {
      ts.add(env, i, true, enumis);
      // initialise new identifier counter to be larger than existing identifier
      if (i->e()->id()->idn() >= 0) {
        env.minId(i->e()->id()->idn());
      } else if (i->e()->id()->v().beginsWith("X_INTRODUCED_") && i->e()->id()->v().endsWith("_")) {
        std::string numId = i->e()->id()->v().substr(std::string("X_INTRODUCED_").size());
        if (!numId.empty()) {
          numId = numId.substr(0, numId.size() - 1);
          if (!numId.empty()) {
            int vId = -1;
            try {
              vId = std::stoi(numId);
            } catch (std::exception&) {
              vId = -1;
            }
            if (vId >= 0) {
              env.minId(vId);
            }
          }
        }
      }
    }
    void vSolveI(SolveI* si) {
      if (hadSolveItem) {
        typeErrors.emplace_back(env, si->loc(), "Only one solve item allowed");
        return;
      }
      hadSolveItem = true;
      if (!isFlatZinc && (si->e() != nullptr)) {
        GCLock lock;
        auto* ti = new TypeInst(Location().introduce(), Type());
        VarDecl* obj;
        if (!isChecker) {
          obj = new VarDecl(Expression::loc(si->e()).introduce(), ti, "_objective", si->e());
        } else {
          obj =
              new VarDecl(Expression::loc(si->e()).introduce(), ti, "_checker_objective", si->e());
        }
        si->e(obj->id());
        Expression::addAnnotation(
            obj, si->st() == SolveI::ST_MAX ? env.constants.ctx.pos : env.constants.ctx.neg);
        objective = VarDeclI::a(si->loc().introduce(), obj);
        objectiveModel->addItem(objective);
      }
    }
  } _tsv0(env.envi(), ts, m, assignItems, enumItems, isFlatZinc, isChecker, typeErrors);
  iter_items(_tsv0, m);
  if (_tsv0.objective != nullptr) {
    m->addItem(_tsv0.objective);
    ts.add(env.envi(), _tsv0.objective, true, enumItems);
  }

  for (unsigned int i = 0; i < enumItems->size(); i++) {
    if (auto* ai = (*enumItems)[i]->dynamicCast<AssignI>()) {
      assignItems.push_back(ai);
    } else if (auto* vdi = (*enumItems)[i]->dynamicCast<VarDeclI>()) {
      m->addItem(vdi);
      ts.add(env.envi(), vdi, false, enumItems);
    } else if (auto* fi = (*enumItems)[i]->dynamicCast<FunctionI>()) {
      m->addItem(fi);
      functionItems.push_back(fi);
    } else if (auto* ci = (*enumItems)[i]->dynamicCast<ConstraintI>()) {
      m->addItem(ci);
    }
  }

  auto* enumItems2 = new Model;

  for (auto* ai : assignItems) {
    VarDecl* vd = nullptr;
    if (env.envi().ignoreUnknownIds) {
      try {
        vd = ts.get(env.envi(), ai->id(), ai->loc());
      } catch (TypeError&) {
        vd = nullptr;
      }
    } else {
      vd = ts.get(env.envi(), ai->id(), ai->loc());
    }
    if (vd != nullptr) {
      if (vd->e() != nullptr) {
        if (allowMultiAssignment) {
          GCLock lock;
          m->addItem(new ConstraintI(
              ai->loc(),
              new BinOp(ai->loc(), new Id(Location().introduce(), ai->id(), vd), BOT_EQ, ai->e())));
        } else {
          typeErrors.emplace_back(env.envi(), ai->loc(),
                                  "multiple assignment to the same variable");
        }
      } else {
        vd->e(ai->e());
        Expression::addAnnotation(vd, Constants::constants().ann.rhs_from_assignment);
        if (vd->ti()->isEnum()) {
          create_enum_mapper(env.envi(), m, vd->ti()->type().typeId(), vd, enumItems2, needToString,
                             enumConstructorSetTypes);
        }
      }
    }
    ai->remove();
  }

  for (auto& i : *enumItems2) {
    m->addItem(i);
    if (auto* vdi = i->dynamicCast<VarDeclI>()) {
      ts.add(env.envi(), vdi, false, enumItems);
    } else if (auto* fi = i->dynamicCast<FunctionI>()) {
      functionItems.push_back(fi);
    }
  }

  ASTStringMap<std::vector<FunctionI*>> overload_map;
  for (auto* functionItem : functionItems) {
    bool fullyKnown = true;
    if (functionItem->ti()->type().isunknown() ||
        functionItem->ti()->type().structBT() && functionItem->ti()->type().typeId() == 0) {
      fullyKnown = false;
    } else {
      for (unsigned int i = 0; i < functionItem->paramCount(); i++) {
        if (functionItem->param(i)->type().isunknown() ||
            functionItem->param(i)->type().structBT() &&
                functionItem->param(i)->type().typeId() == 0) {
          fullyKnown = false;
          break;
        }
      }
    }

    if (fullyKnown) {
      m->registerFn(env.envi(), functionItem);
      m->sortFn(env.envi(), functionItem);
    } else {
      auto it = overload_map.find(functionItem->id());
      if (it == overload_map.end()) {
        overload_map.insert({functionItem->id(), {functionItem}});
      } else {
        it->second.push_back(functionItem);
      }
    }
  }

  for (auto& nts : needToString) {
    ASTString nts_id(create_enum_to_string_name(nts.first, "_toString_"));
    if (!env.model()->fnExists(env.envi(), nts_id) &&
        overload_map.find(nts_id) == overload_map.end()) {
      GCLock lock;
      // Assumption: any _toString_ function that hasn't been generated by now
      // is for a set of int, rather than an enum. So generate a generic _toString_
      // function here:
      // function string: _to_String_<nts_id>(opt int: x, bool: b, bool: json) = show(i);
      Type tx = Type::parint();
      tx.ot(Type::OT_OPTIONAL);
      auto* ti_aa = new TypeInst(Location().introduce(), tx, new TIId(Location(), "$E"));
      auto* vd_aa = new VarDecl(Location().introduce(), ti_aa, env.envi().genId());
      vd_aa->toplevel(false);

      auto* ti_ab = new TypeInst(Location().introduce(), Type::parbool());
      auto* vd_ab = new VarDecl(Location().introduce(), ti_ab, env.envi().genId());
      vd_ab->toplevel(false);

      auto* ti_aj = new TypeInst(Location().introduce(), Type::parbool());
      auto* vd_aj = new VarDecl(Location().introduce(), ti_aj, env.envi().genId());
      vd_aj->toplevel(false);

      auto* ti_fi = new TypeInst(Location().introduce(), Type::parstring());
      std::vector<VarDecl*> fi_params(3);
      fi_params[0] = vd_aa;
      fi_params[1] = vd_ab;
      fi_params[2] = vd_aj;

      Call* body = Call::a(Location().introduce(), Constants::constants().ids.show, {vd_aa->id()});
      auto* fi = new FunctionI(Location().introduce(), nts_id, ti_fi, fi_params, body);
      m->addItem(fi);
      functionItems.push_back(fi);
      m->registerFn(env.envi(), fi);
      m->sortFn(env.envi(), fi);
    }
  }

  delete enumItems;
  delete enumItems2;

  class TSV1 : public ItemVisitor {
  public:
    EnvI& env;
    TopoSorter& ts;
    TSV1(EnvI& env0, TopoSorter& ts0) : env(env0), ts(ts0) {}
    void vVarDeclI(VarDeclI* i) { ts.run(env, i->e()); }
    void vAssignI(AssignI* i) {}
    void vConstraintI(ConstraintI* i) { ts.run(env, i->e()); }
    void vSolveI(SolveI* i) {
      for (ExpressionSetIter it = i->ann().begin(); it != i->ann().end(); ++it) {
        ts.run(env, *it);
      }
      ts.run(env, i->e());
    }
    void vOutputI(OutputI* i) {
      for (ExpressionSetIter it = i->ann().begin(); it != i->ann().end(); ++it) {
        ts.run(env, *it);
      }
      ts.run(env, i->e());
    }
    void vFunctionI(FunctionI* fi) {
      ts.run(env, fi->ti());
      for (unsigned int i = 0; i < fi->paramCount(); i++) {
        ts.run(env, fi->param(i));
      }
      ts.run(env, fi->capturedAnnotationsVar());
      for (ExpressionSetIter it = fi->ann().begin(); it != fi->ann().end(); ++it) {
        ts.run(env, *it);
      }
      ts.scopes.pushFun();
      for (unsigned int i = 0; i < fi->paramCount(); i++) {
        ts.scopes.add(env, fi->param(i));
      }
      if (fi->capturedAnnotationsVar() != nullptr) {
        ts.scopes.add(env, fi->capturedAnnotationsVar());
      }
      ts.run(env, fi->e());
      ts.scopes.pop();
    }
  } _tsv1(env.envi(), ts);
  iter_items(_tsv1, m);

  {
    struct SortByPayload {
      bool operator()(Item* i0, Item* i1) {
        if (i0->isa<IncludeI>()) {
          return !i1->isa<IncludeI>();
        }
        if (auto* vdi0 = i0->dynamicCast<VarDeclI>()) {
          if (auto* vdi1 = i1->dynamicCast<VarDeclI>()) {
            return vdi0->e()->payload() < vdi1->e()->payload();
          }
          return !i1->isa<IncludeI>();
        }
        return false;
      }
    } _sbp;

    std::stable_sort(m->begin(), m->end(), _sbp);
  }

  // The first phase of type checking needs to check and infer types
  // for all "left-hand sides" (LHS). This includes type-insts in variable
  // declarations, as well as the signatures of all functions
  // (type-insts of their parameters and the return type-inst).

  // Since all those type-insts may refer to other declarations
  // (both to other variables, and to functions), this needs to be done
  // in topological order.

  // The algorithm has a stack of items (vardecls and functions) that
  // need to be LHS-typed. For each item, we first push all items they
  // refer to, and process those. Then we can type the item itself.

  class PushUndefinedDeclsStackItem {
  private:
    Item* _item;
    bool _marked;

  public:
    PushUndefinedDeclsStackItem(Item* item) : _item(item), _marked(false) {}
    Item* item() { return _item; }
    const Item* item() const { return _item; }
    bool marked() const { return _marked; }
    void mark() {
      assert(!_marked);
      _marked = true;
    }
  };

  /// This visitor finds declarations that have undefined type and pushes them onto the stack
  class PushUndefinedDeclsV : public EVisitor {
  private:
    EnvI& _env;
    Model* _m;
    std::vector<PushUndefinedDeclsStackItem>& _stack;
    std::unordered_set<Item*>& _seenItems;
    ASTStringMap<std::vector<FunctionI*>>& _overloadMap;

  public:
    PushUndefinedDeclsV(EnvI& env, Model* m, std::vector<PushUndefinedDeclsStackItem>& stack,
                        std::unordered_set<Item*>& seenItems,
                        ASTStringMap<std::vector<FunctionI*>>& overloadMap)
        : _env(env), _m(m), _stack(stack), _seenItems(seenItems), _overloadMap(overloadMap) {}

    void pushItemAndCheckCycle(Item* item) {
      if (_seenItems.find(item) != _seenItems.end()) {
        std::ostringstream ss;
        ss << "circular definition of `";
        if (auto* fi = Item::dynamicCast<FunctionI>(item)) {
          ss << fi->id();
        } else {
          auto* vd = Item::cast<VarDeclI>(item)->e();
          ss << vd->id()->str();
        }
        ss << "'";
        throw TypeError(_env, item->loc(), ss.str());
      }
      _stack.emplace_back(item);
    }

    void vId(Id* ident) {
      auto* decl = ident->decl();
      if (decl->type().isunknown() &&
          (!decl->isTypeAlias() || Expression::type(decl->e()).isunknown()) &&
          (!decl->type().structBT() || decl->type().typeId() == 0)) {
        pushItemAndCheckCycle(decl->item());
      }
    }
    void vCall(Call* call) {
      auto* decl = call->decl();
      if (decl == nullptr) {
        std::vector<FunctionI*> potentialOverloads;
        auto it = _overloadMap.find(call->id());
        if (it != _overloadMap.end()) {
          for (auto* fi : it->second) {
            if (fi->paramCount() == call->argCount()) {
              potentialOverloads.push_back(fi);
            }
          }
        }
        for (auto* decl : potentialOverloads) {
          if (decl->ti()->type().isunknown() ||
              (decl->ti()->type().structBT() && decl->ti()->type().typeId() == 0)) {
            pushItemAndCheckCycle(decl);
          } else {
            for (unsigned int i = 0; i < decl->paramCount(); i++) {
              if (decl->param(i)->type().isunknown() ||
                  (decl->param(i)->type().structBT() && decl->param(i)->type().typeId() == 0)) {
                pushItemAndCheckCycle(decl);
                break;
              }
            }
          }
        }
      } else {
        if (decl->ti()->type().isunknown() ||
            (decl->ti()->type().structBT() && decl->ti()->type().typeId() == 0)) {
          pushItemAndCheckCycle(decl);
        } else {
          for (unsigned int i = 0; i < decl->paramCount(); i++) {
            if (decl->param(i)->type().isunknown() ||
                (decl->param(i)->type().structBT() && decl->param(i)->type().typeId() == 0)) {
              pushItemAndCheckCycle(decl);
              break;
            }
          }
        }
      }
    }
  };

  class TypeUndefinedDecls {
  private:
    EnvI& _env;
    std::vector<PushUndefinedDeclsStackItem> _stack;
    std::unordered_set<Item*>& _seenItems;
    ASTStringMap<std::vector<FunctionI*>>& _overloadMap;
    Model* _m;
    Typer<false> _typer;
    BottomUpIterator<Typer<false>> _bottomUpTyper;
    PushUndefinedDeclsV _pushUndefinedDeclsV;
    BottomUpIterator<PushUndefinedDeclsV> _pushUndefinedDecls;

  public:
    TypeUndefinedDecls(EnvI& env, Model* m, std::unordered_set<Item*>& seenItems,
                       ASTStringMap<std::vector<FunctionI*>>& overloadMap,
                       std::vector<TypeError>& typeErrors)
        : _env(env),
          _m(m),
          _seenItems(seenItems),
          _overloadMap(overloadMap),
          _typer(env, m, typeErrors, false),
          _bottomUpTyper(_typer),
          _pushUndefinedDeclsV(_env, _m, _stack, seenItems, overloadMap),
          _pushUndefinedDecls(_pushUndefinedDeclsV) {}

    void run(Item* rootItem) {
      if (_seenItems.find(rootItem) == _seenItems.end()) {
        _stack.emplace_back(rootItem);
        while (!_stack.empty()) {
          auto& top = _stack.back();
          if (top.marked()) {
            // All dependencies have been processed, now type this item
            if (auto* fi = Item::dynamicCast<FunctionI>(top.item())) {
              _bottomUpTyper.run(fi->ti());
              for (unsigned int j = 0; j < fi->paramCount(); j++) {
                _bottomUpTyper.run(fi->param(j));
              }
              if (fi->capturedAnnotationsVar() != nullptr) {
                _bottomUpTyper.run(fi->capturedAnnotationsVar());
              }

              _m->registerFn(_env, fi);
              _m->sortFn(_env, fi);
            } else {
              auto* decl = Item::cast<VarDeclI>(top.item())->e();
              _bottomUpTyper.run(decl->ti());
              if (decl->isTypeAlias()) {
                _bottomUpTyper.run(decl->e());
              }
              _typer.vVarDecl(decl);
            }
            _stack.pop_back();
          } else {
            top.mark();
            _seenItems.insert(top.item());
            if (auto* fi = Item::dynamicCast<FunctionI>(top.item())) {
              _pushUndefinedDecls.run(fi->ti());
              for (unsigned int j = 0; j < fi->paramCount(); j++) {
                _pushUndefinedDecls.run(fi->param(j));
              }
              if (fi->capturedAnnotationsVar() != nullptr) {
                _pushUndefinedDecls.run(fi->capturedAnnotationsVar());
              }
            } else {
              auto* vd = Item::cast<VarDeclI>(top.item())->e();
              _pushUndefinedDecls.run(vd->ti());
            }
          }
        }
      }
    }
  };

  {
    std::unordered_set<Item*> seen_items;
    TypeUndefinedDecls typeUndefinedDecls(env.envi(), m, seen_items, overload_map, typeErrors);
    for (auto& declKA : ts.decls) {
      auto* decl_item = Expression::cast<VarDecl>(declKA())->item();
      decl_item->e()->payload(0);
      if (decl_item->e()->toplevel()) {
        typeUndefinedDecls.run(decl_item);
      }
    }
    for (auto* functionItem : functionItems) {
      typeUndefinedDecls.run(functionItem);
    }
  }

  Typer<true> ty(env.envi(), m, typeErrors, isFlatZinc);
  {
    BottomUpIterator<Typer<true>> bottomUpTyper(ty);

    for (auto* c : enumConstructorSetTypes) {
      bottomUpTyper.run(c->arg(0));
      if (c->id() == env.envi().constants.ids.anon_enum) {
        if (Expression::type(c->arg(0)) != Type::parint()) {
          throw TypeError(env.envi(), Expression::loc(c->arg(0)),
                          "anonymous enum initializer must be of type `int', but is `" +
                              Expression::type(c->arg(0)).toString(env.envi()) + "'");
        }
      } else if (c->id() == env.envi().constants.ids.anon_enum_set) {
        if (!Expression::type(c->arg(0)).isSubtypeOf(env.envi(), Type::parsetint(), false)) {
          throw TypeError(env.envi(), Expression::loc(c->arg(0)),
                          "anonymous enum initializer must be of type `set of int', but is `" +
                              Expression::type(c->arg(0)).toString(env.envi()) + "'");
        }
      }
    }

    class TSV2 : public ItemVisitor {
    private:
      EnvI& _env;
      Model* _m;
      BottomUpIterator<Typer<true>>& _bottomUpTyper;
      std::vector<TypeError>& _typeErrors;

    public:
      TSV2(EnvI& env0, Model* m0, BottomUpIterator<Typer<true>>& b,
           std::vector<TypeError>& typeErrors)
          : _env(env0), _m(m0), _bottomUpTyper(b), _typeErrors(typeErrors) {}
      void vVarDeclI(VarDeclI* i) {
        _bottomUpTyper.run(i->e());
        if (i->e()->ti() != nullptr && i->e()->ti()->hasTiVariable()) {
          std::ostringstream ss;
          ss << "type-inst variables not allowed in type-inst for `" << i->e()->id()->str() << "'";
          _typeErrors.emplace_back(_env, Expression::loc(i->e()), ss.str());
        }
        VarDecl* vdi = i->e();
        if (vdi->e() == nullptr && vdi->type().isSet() && vdi->type().isvar() &&
            vdi->ti()->domain() == nullptr) {
          std::ostringstream ss;
          ss << "set element type for `" << vdi->id()->str() << "' is not finite";
          _typeErrors.emplace_back(_env, Expression::loc(vdi), ss.str());
        }
        if (Expression::ann(i->e()).contains(Constants::constants().ann.output_only)) {
          if (vdi->e() == nullptr) {
            _typeErrors.emplace_back(
                _env, Expression::loc(vdi),
                "variables annotated with ::output_only must have a right hand side");
          } else if (Expression::type(vdi->e()).isvar()) {
            _typeErrors.emplace_back(_env, Expression::loc(vdi),
                                     "variables annotated with ::output_only must be par");
          }
        }
      }
      void vAssignI(AssignI* i) {
        _bottomUpTyper.run(i->e());
        if (!_env.isSubtype(Expression::type(i->e()), i->decl()->ti()->type(), true)) {
          std::ostringstream ss;
          ss << "assignment value for `" << i->decl()->id()->str()
             << "' has invalid type-inst: expected `" << i->decl()->ti()->type().toString(_env)
             << "', actual `" << Expression::type(i->e()).toString(_env) << "'";
          _typeErrors.emplace_back(_env, i->loc(), ss.str());
          // Assign to "true" constant to avoid generating further errors that the parameter
          // is undefined
          i->decl()->e(Constants::constants().literalTrue);
        }
      }
      void vConstraintI(ConstraintI* i) {
        _bottomUpTyper.run(i->e());
        i->e(add_coercion(_env, _env.model, i->e(), i->loc(), Type::varbool())());
        if (!_env.isSubtype(Expression::type(i->e()), Type::varbool(), true)) {
          _typeErrors.emplace_back(_env, i->loc(),
                                   "invalid type of constraint, expected `" +
                                       Type::varbool().toString(_env) + "', actual `" +
                                       Expression::type(i->e()).toString(_env) + "'");
        }
      }
      void vSolveI(SolveI* i) {
        for (ExpressionSetIter it = i->ann().begin(); it != i->ann().end(); ++it) {
          _bottomUpTyper.run(*it);
          if (!Expression::type(*it).isAnn()) {
            _typeErrors.emplace_back(
                _env, Expression::loc(*it),
                "expected annotation, got `" + Expression::type(*it).toString(_env) + "'");
          }
        }
        _bottomUpTyper.run(i->e());
        if (i->e() != nullptr) {
          Type et = Expression::type(i->e());
          if (et.isbool()) {
            Type target_t = Type::varint();
            if (et.isOpt()) {
              target_t.ot(Type::OT_OPTIONAL);
            }
            i->e(add_coercion(_env, _env.model, i->e(), i->loc(), target_t)());
          }

          bool needOptCoercion = et.isOpt() && et.isint();
          if (needOptCoercion) {
            et.ot(Type::OT_PRESENT);
          }

          if (!(_env.isSubtype(et, Type::varint(), true) ||
                _env.isSubtype(et, Type::varfloat(), true))) {
            _typeErrors.emplace_back(_env, Expression::loc(i->e()),
                                     "objective has invalid type, expected int or float, actual `" +
                                         et.toString(_env) + "'");
          }

          if (needOptCoercion) {
            GCLock lock;
            std::vector<Expression*> args(2);
            args[0] = i->e();
            args[1] = _env.constants.boollit(i->st() == SolveI::ST_MAX);
            Call* c = Call::a(Location().introduce(), ASTString("objective_deopt_"), args);
            c->decl(_env.model->matchFn(_env, c, false));
            assert(c->decl());
            c->type(et);
            i->e(c);
          }
        }
      }
      void vOutputI(OutputI* i) {
        for (ExpressionSetIter it = i->ann().begin(); it != i->ann().end(); ++it) {
          _bottomUpTyper.run(*it);
          if (!Expression::type(*it).isAnn()) {
            _typeErrors.emplace_back(
                _env, Expression::loc(*it),
                "expected annotation, got `" + Expression::type(*it).toString(_env) + "'");
          }
        }
        _bottomUpTyper.run(i->e());
        if (i->ann().isEmpty() && Expression::type(i->e()) != Type::parstring() &&
            Expression::type(i->e()) != Type::ann() &&
            Expression::type(i->e()) != Type::parstring(1) &&
            Expression::type(i->e()) != Type::bot(1)) {
          _typeErrors.emplace_back(_env, Expression::loc(i->e()),
                                   "invalid type in output item, expected `" +
                                       Type::parstring(1).toString(_env) + "', actual `" +
                                       Expression::type(i->e()).toString(_env) + "'");
        }
      }
      void vFunctionI(FunctionI* fi) {
        for (ExpressionSetIter it = fi->ann().begin(); it != fi->ann().end(); ++it) {
          _bottomUpTyper.run(*it);
          if (!Expression::type(*it).isAnn()) {
            _typeErrors.emplace_back(
                _env, Expression::loc(*it),
                "expected annotation, got `" + Expression::type(*it).toString(_env) + "'");
          }
        }
        _bottomUpTyper.run(fi->ti());
        // Check that type-inst variables are used consistently
        enum TIVarType { TIVAR_INDEX, TIVAR_DOMAIN };
        ASTStringMap<TIVarType> ti_map;
        std::function<void(TypeInst * ti, TIVarType t)> checkTIId;
        checkTIId = [&ti_map, this, &checkTIId](TypeInst* ti, TIVarType t) {
          if (TIId* tiid = Expression::dynamicCast<TIId>(ti->domain())) {
            if (!tiid->isEnum()) {
              auto lookup = ti_map.insert({tiid->v(), t});
              if (!lookup.second && lookup.first->second != t) {
                std::ostringstream ss;
                ss << "type-inst variable $" << tiid->v()
                   << " used in both array and non-array position";
                _typeErrors.emplace_back(_env, Expression::loc(tiid), ss.str());
              }
            } else {
              ti_map.insert({tiid->v(), t});
            }
          } else if (ti->type().structBT()) {
            auto* al = Expression::cast<ArrayLit>(ti->domain());
            for (unsigned int i = 0; i < al->size(); i++) {
              checkTIId(Expression::cast<TypeInst>((*al)[i]), t);
            }
          }
          for (unsigned int j = 0; j < ti->ranges().size(); j++) {
            checkTIId(ti->ranges()[j], TIVAR_INDEX);
          }
        };
        bool allParamsPar = true;
        for (unsigned int i = 0; i < fi->paramCount(); i++) {
          allParamsPar = allParamsPar && fi->param(i)->type().isPar();
          checkTIId(fi->param(i)->ti(), TIVAR_DOMAIN);
        }
        std::function<void(TypeInst * ti, TIVarType t)> checkRetTIId;
        checkRetTIId = [&ti_map, this, &checkRetTIId](TypeInst* ti, TIVarType t) {
          if (TIId* tiid = Expression::dynamicCast<TIId>(ti->domain())) {
            auto it = ti_map.find(tiid->v());
            if (it == ti_map.end()) {
              std::ostringstream ss;
              ss << "type-inst variable $" << tiid->v()
                 << " used in return type but not defined in argument list";
              _typeErrors.emplace_back(_env, Expression::loc(tiid), ss.str());
            } else if (!tiid->isEnum() && it->second != t) {
              std::ostringstream ss;
              ss << "type-inst variable $" << tiid->v()
                 << " used in both array and non-array position";
              _typeErrors.emplace_back(_env, Expression::loc(tiid), ss.str());
            }
          } else if (ti->type().structBT()) {
            auto* al = Expression::cast<ArrayLit>(ti->domain());
            for (unsigned int i = 0; i < al->size(); i++) {
              checkRetTIId(Expression::cast<TypeInst>((*al)[i]), t);
            }
          }
          for (unsigned int j = 0; j < ti->ranges().size(); j++) {
            checkRetTIId(ti->ranges()[j], TIVAR_INDEX);
          }
        };
        checkRetTIId(fi->ti(), TIVAR_DOMAIN);

        if (fi->ann().contains(_env.constants.ann.promise_commutative)) {
          bool valid = fi->paramCount() > 0;
          if (fi->paramCount() == 1) {
            valid = fi->param(0)->type().dim() != 0;
          } else if (fi->paramCount() > 1) {
            for (unsigned int i = 1; i < fi->paramCount(); ++i) {
              if (fi->param(i)->type() != fi->param(0)->type()) {
                valid = false;
                break;
              }
            }
          }
          if (!valid) {
            std::ostringstream ss;
            ss << "the types of the paramaters for the function `" << fi->id()
               << "', declared as `promise_commutative' are expected to all be the same, or be a "
                  "single array type.";
            _typeErrors.emplace_back(_env, fi->loc(), ss.str());
          }
        }

        _bottomUpTyper.run(fi->e());
        if ((fi->e() != nullptr) &&
            !_env.isSubtype(Expression::type(fi->e()), fi->ti()->type(), true)) {
          _typeErrors.emplace_back(
              _env, Expression::loc(fi->e()),
              "return type of function does not match body, declared type is `" +
                  fi->ti()->type().toString(_env) + "', body type is `" +
                  Expression::type(fi->e()).toString(_env) + "'");
        }
        if (fi->e() != nullptr) {
          fi->e(add_coercion(_env, _m, fi->e(), fi->loc(), fi->ti()->type())());
        }
      }
    } _tsv2(env.envi(), m, bottomUpTyper, typeErrors);
    iter_items(_tsv2, m);
  }

  class TSV3 : public ItemVisitor {
  public:
    EnvI& env;
    Model* m;
    OutputI* outputItem;
    TSV3(EnvI& env0, Model* m0) : env(env0), m(m0), outputItem(nullptr) {}
    void vAssignI(AssignI* i) {
      i->decl()->e(add_coercion(env, m, i->e(), i->loc(), i->decl()->type())());
    }
    static void vVarDeclI(VarDeclI* i) {
      if (i->e()->isTypeAlias()) {
        i->remove();  // no longer required
      }
    }
  } _tsv3(env.envi(), m);
  if (typeErrors.empty()) {
    iter_items(_tsv3, m);
  }

  // Specialisation of parametric functions
  if (!isFlatZinc) {
    BottomUpIterator<Typer<true>> bottomUpTyper(ty);

    class ConcreteTyper : public TyperFn {
    private:
      Typer<true>& _ty;
      BottomUpIterator<Typer<true>>& _bottomUpTyper;

    public:
      ConcreteTyper(Typer<true>& ty, BottomUpIterator<Typer<true>>& i)
          : _ty(ty), _bottomUpTyper(i) {}
      void retype(EnvI& env, FunctionI* fi) override { _bottomUpTyper.run(fi->e()); }
      void reset(EnvI& env, FunctionI* fi) override {
        class ResetAnyInLet : public EVisitor {
        private:
          EnvI& _env;
          Typer<true>& _ty;

        public:
          ResetAnyInLet(EnvI& env, Typer<true>& ty) : _env(env), _ty(ty) {}
          void vLet(Let* let) {
            for (auto* l : let->let()) {
              if (auto* vd = Expression::dynamicCast<VarDecl>(l)) {
                if (_ty.anyInLet.find(vd) != _ty.anyInLet.end()) {
                  vd->type(Type::mkAny());
                  vd->ti()->type(Type::mkAny());
                  vd->ti()->domain(nullptr);
                }
              }
            }
          }
        } ra(env, _ty);
        bottom_up(ra, fi->e());
      }
    } concreteTyper(ty, bottomUpTyper);
    type_specialise(env, m, concreteTyper);
    create_par_versions(env, m, bottomUpTyper);
  }

  try {
    m->checkFnOverloading(env.envi());
  } catch (TypeError& e) {
    typeErrors.push_back(e);
  }

  for (auto& declKA : ts.decls) {
    auto* decl = Expression::cast<VarDecl>(declKA());
    if (decl->isTypeAlias()) {
      continue;
    }
    if (decl->toplevel() && decl->type().isPar() && !decl->type().isAnn() && decl->e() == nullptr) {
      if (decl->type().isOpt() && decl->type().dim() == 0) {
        decl->e(Constants::constants().absent);
        Expression::addAnnotation(decl, Constants::constants().ann.mzn_was_undefined);
      } else if (!ignoreUndefinedParameters) {
        std::ostringstream ss;
        ss << "  symbol error: variable `" << decl->id()->str()
           << "' must be defined (did you forget to specify a data file?)";
        typeErrors.emplace_back(env.envi(), Expression::loc(decl), ss.str());
      }
    }
    if (decl->ti()->isEnum()) {
      decl->ti()->setIsEnum(false);
      Type vdt = decl->ti()->type();
      vdt.typeId(0);
      decl->ti()->type(vdt);
    }
  }

  for (auto vd_k : env.envi().checkVars) {
    try {
      VarDecl* vd;
      try {
        vd = ts.get(env.envi(), Expression::cast<VarDecl>(vd_k())->id()->str(),
                    Expression::loc(Expression::cast<VarDecl>(vd_k())));
      } catch (TypeError&) {
        if (Expression::cast<VarDecl>(vd_k())->type().isvar()) {
          continue;  // var can be undefined
        }
        throw;
      }
      Expression::addAnnotation(vd, Constants::constants().ann.mzn_check_var);
      if (vd->type().typeId() != 0 && vd->type().bt() == Type::BT_INT) {
        GCLock lock;
        std::vector<unsigned int> enumIds({vd->type().typeId()});
        if (vd->type().dim() > 0) {
          enumIds = env.envi().getArrayEnum(vd->type().typeId());
        }
        std::vector<Expression*> enumIds_a(enumIds.size());
        for (unsigned int i = 0; i < enumIds.size(); i++) {
          if (enumIds[i] != 0) {
            enumIds_a[i] = env.envi().getEnum(enumIds[i])->e()->id();
          } else {
            enumIds_a[i] = new SetLit(Location().introduce(), std::vector<Expression*>());
          }
        }
        auto* enumIds_al = new ArrayLit(Location().introduce(), enumIds_a);
        enumIds_al->type(Type::parsetint(1));
        std::vector<Expression*> args({enumIds_al});
        Call* checkEnum =
            Call::a(Location().introduce(), Constants::constants().ann.mzn_check_enum_var, args);
        checkEnum->type(Type::ann());
        checkEnum->decl(env.envi().model->matchFn(env.envi(), checkEnum, false));
        Expression::addAnnotation(vd, checkEnum);
      }
      Type vdktype = Expression::type(vd_k());
      vdktype.mkVar(env.envi());
      if (!Expression::type(vd_k()).isSubtypeOf(env.envi(), vd->type(), false)) {
        std::ostringstream ss;
        ss << "Solution checker requires `" << vd->id()->str() << "' to be of type `"
           << vdktype.toString(env.envi()) << "'";
        typeErrors.emplace_back(env.envi(), Expression::loc(vd), ss.str());
      }
    } catch (TypeError& e) {
      typeErrors.emplace_back(env.envi(), e.loc(),
                              e.msg() + " (required by solution checker model)");
    }
  }

  if (isFlatZinc) {
    for (auto* it : *annotatedExpressionItems) {
      // We needed these to do typechecking but we can't keep them because this is a FlatZinc file
      it->remove();
    }
  }
}

void typecheck(Env& env, Model* m, AssignI* ai) {
  std::vector<TypeError> typeErrors;
  Typer<true> ty(env.envi(), m, typeErrors, false);
  BottomUpIterator<Typer<true>> bottomUpTyper(ty);
  bottomUpTyper.run(ai->e());
  if (!typeErrors.empty()) {
    throw MultipleErrors<TypeError>(typeErrors);
  }
  if (!env.envi().isSubtype(Expression::type(ai->e()), ai->decl()->ti()->type(), true)) {
    std::ostringstream ss;
    ss << "assignment value for `" << ai->decl()->id()->str()
       << "' has invalid type-inst: expected `" << ai->decl()->ti()->type().toString(env.envi())
       << "', actual `" << Expression::type(ai->e()).toString(env.envi()) << "'";
    throw TypeError(env.envi(), Expression::loc(ai->e()), ss.str());
  }
}

void output_var_desc_json(Env& env, TypeInst* ti, std::ostream& os, bool extra = false) {
  os << "{";
  os << "\"type\" : ";
  switch (ti->type().bt()) {
    case Type::BT_INT:
      os << "\"int\"";
      break;
    case Type::BT_BOOL:
      os << "\"bool\"";
      break;
    case Type::BT_FLOAT:
      os << "\"float\"";
      break;
    case Type::BT_STRING:
      os << "\"string\"";
      break;
    case Type::BT_ANN:
      os << "\"ann\"";
      break;
    case Type::BT_TUPLE: {
      os << "\"tuple\"";
      break;
    }
    case Type::BT_RECORD: {
      os << "\"record\"";
      break;
    }
    default:
      os << "\"?\"";
      break;
  }
  if (ti->type().ot() == Type::OT_OPTIONAL) {
    os << ", \"optional\" : true";
  }
  if (ti->type().st() == Type::ST_SET) {
    os << ", \"set\" : true";
  }

  const auto tuple_types = [&]() {
    auto* dom = Expression::cast<ArrayLit>(ti->domain());
    os << ", \"field_types\" : [";
    for (unsigned int i = 0; i < dom->size(); ++i) {
      output_var_desc_json(env, Expression::cast<TypeInst>((*dom)[i]), os, extra);
      if (i < dom->size() - 1) {
        os << ", ";
      }
    }
    os << "]";
  };
  const auto record_types = [&]() {
    auto* dom = Expression::cast<ArrayLit>(ti->domain());
    auto* rt = env.envi().getRecordType(ti->type());
    os << ", \"field_types\" : {";
    for (unsigned int i = 0; i < dom->size(); ++i) {
      os << "\"" << Printer::escapeStringLit(rt->fieldName(i)) << "\": ";
      output_var_desc_json(env, Expression::cast<TypeInst>((*dom)[i]), os, extra);
      if (i < dom->size() - 1) {
        os << ", ";
      }
    }
    os << "}";
  };
  if (ti->type().dim() > 0) {
    os << ", \"dim\" : " << ti->type().dim();

    if (extra) {
      os << ", \"dims\" : [";
      bool had_dim = false;
      ASTExprVec<TypeInst> ranges = ti->ranges();
      for (auto& range : ranges) {
        if (range->type().typeId() > 0) {
          os << (had_dim ? "," : "") << "\""
             << *env.envi().getEnum(range->type().typeId())->e()->id() << "\"";
        } else {
          os << (had_dim ? "," : "") << "\"int\"";
        }
        had_dim = true;
      }
      os << "]";

      if (ti->type().typeId() > 0) {
        const std::vector<unsigned int>& typeIds = env.envi().getArrayEnum(ti->type().typeId());
        if (typeIds.back() > 0) {
          if (ti->type().bt() == Type::BT_TUPLE) {
            tuple_types();
          } else if (ti->type().bt() == Type::BT_RECORD) {
            record_types();
          } else {
            assert(ti->type().bt() == Type::BT_INT);
            os << ", \"enum_type\" : \"" << *env.envi().getEnum(typeIds.back())->e()->id() << "\"";
          }
        }
      }
    }
  } else {
    if (extra) {
      if (ti->type().typeId() > 0) {
        if (ti->type().bt() == Type::BT_TUPLE) {
          tuple_types();
        } else if (ti->type().bt() == Type::BT_RECORD) {
          record_types();
        } else {
          assert(ti->type().bt() == Type::BT_INT);
          os << ", \"enum_type\" : \"" << *env.envi().getEnum(ti->type().typeId())->e()->id()
             << "\"";
        }
      }
    }
  }
  os << "}";
}

void output_var_desc_json(Env& env, VarDecl* vd, std::ostream& os, bool extra = false) {
  os << "\"" << Printer::escapeStringLit(vd->id()->str()) << "\": ";
  output_var_desc_json(env, vd->ti(), os, extra);
}

void output_model_variable_types(Env& env, Model* m, std::ostream& os,
                                 const std::vector<std::string>& skipDirs) {
  class VInfVisitor : public ItemVisitor {
  public:
    Env& env;
    const std::vector<std::string>& skipDirs;
    bool hadVar;
    bool hadEnum;
    std::ostringstream ossVars;
    std::ostringstream ossEnums;
    VInfVisitor(Env& env0, const std::vector<std::string>& skipDirs0)
        : env(env0), skipDirs(skipDirs0), hadVar(false), hadEnum(false) {}
    bool enter(Item* i) {
      if (auto* ii = i->dynamicCast<IncludeI>()) {
        std::string prefix =
            ii->m()->filepath().substr(0, ii->m()->filepath().size() - ii->f().size());
        for (const auto& skip_dir : skipDirs) {
          if (prefix.substr(0, skip_dir.size()) == skip_dir) {
            return false;
          }
        }
      }
      return true;
    }
    void vVarDeclI(VarDeclI* vdi) {
      if (!vdi->e()->type().isAnn() && !vdi->e()->ti()->isEnum()) {
        if (hadVar) {
          ossVars << ",\n";
        }
        output_var_desc_json(env, vdi->e(), ossVars, true);
        hadVar = true;
      } else if (vdi->e()->type().st() == Type::ST_SET && vdi->e()->type().typeId() != 0 &&
                 !vdi->e()->type().isAnn()) {
        if (hadEnum) {
          ossEnums << ", ";
        }
        ossEnums << "\"" << *env.envi().getEnum(vdi->e()->type().typeId())->e()->id() << "\"";
        hadEnum = true;
      }
    }
  } _vinf(env, skipDirs);
  iter_items(_vinf, m);
  os << "{\"var_types\": {";
  os << "\n  \"vars\": {\n" << _vinf.ossVars.str() << "\n  },";
  os << "\n  \"enums\": [" << _vinf.ossEnums.str() << "]\n";
  os << "}}\n";
}

std::set<std::string> model_globals(Model* m, const std::vector<std::string>& skipDirs) {
  class IterGlobals : public EVisitor {
  public:
    const std::vector<std::string>& skipDirs;
    std::set<std::string> globals;
    IterGlobals(const std::vector<std::string>& skipDirs0) : skipDirs(skipDirs0) {}

    void vCall(const Call* c) {
      if (c->decl() != nullptr && !c->decl()->fromStdLib()) {
        // Globals are not from the standard library (i.e., stdlib.mzn), but included from
        // the standard library path
        ASTString filename = c->decl()->loc().filename();
        if (!filename.empty()) {
          const auto filedir = FileUtils::file_path(FileUtils::dir_name(filename.c_str()));
          for (const auto& skip_dir : skipDirs) {
            const auto& comp_dir = FileUtils::dir_name(skip_dir);
            if (filedir.substr(0, comp_dir.size()) == comp_dir) {
              globals.insert(demonomorphise_identifier(c->id()));
              break;
            }
          }
        }
      }
    }

  } ig(skipDirs);

  class GlobalsVisitor : public ItemVisitor {
  public:
    const std::vector<std::string>& skipDirs;
    IterGlobals& ig;

    GlobalsVisitor(IterGlobals& ig0, const std::vector<std::string>& skipDirs0)
        : ig(ig0), skipDirs(skipDirs0) {}

    bool enter(Item* i) {
      if (auto* ii = i->dynamicCast<IncludeI>()) {
        std::string prefix =
            ii->m()->filepath().substr(0, ii->m()->filepath().size() - ii->f().size());
        for (const auto& skip_dir : skipDirs) {
          if (prefix.substr(0, skip_dir.size()) == skip_dir) {
            return false;
          }
        }
      }
      return true;
    }
    void vVarDeclI(VarDeclI* vdi) { top_down(ig, vdi->e()); }
    void vAssignI(AssignI* ai) { top_down(ig, ai->e()); }
    void vConstraintI(ConstraintI* ci) { top_down(ig, ci->e()); }
    void vSolveI(SolveI* si) {
      if (si->e() != nullptr) {
        top_down(ig, si->e());
      }
    }
    void vOutputI(OutputI* oi) { top_down(ig, oi->e()); }
    void vFunctionI(FunctionI* fi) {
      if (fi->e() != nullptr) {
        top_down(ig, fi->e());
      }
    }

  } gv(ig, skipDirs);
  iter_items(gv, m);
  return ig.globals;
}

void output_model_interface(Env& env, Model* m, std::ostream& os,
                            const std::vector<std::string>& skipDirs) {
  class IfcVisitor : public ItemVisitor {
  public:
    Env& env;
    const std::vector<std::string>& skipDirs;
    bool hadInput;
    bool hadIncludedFiles;
    bool hadAddToOutput = false;
    bool hasOutputItem = false;
    std::ostringstream ossInput;
    std::ostringstream ossIncludedFiles;
    std::string method;
    IfcVisitor(Env& env0, const std::vector<std::string>& skipDirs0)
        : env(env0), skipDirs(skipDirs0), hadInput(false), hadIncludedFiles(false), method("sat") {}
    bool enter(Item* i) {
      if (auto* ii = i->dynamicCast<IncludeI>()) {
        std::string prefix =
            ii->m()->filepath().substr(0, ii->m()->filepath().size() - ii->f().size());
        for (const auto& skip_dir : skipDirs) {
          if (prefix.substr(0, skip_dir.size()) == skip_dir) {
            return false;
          }
        }
        if (hadIncludedFiles) {
          ossIncludedFiles << ", ";
        }
        ossIncludedFiles << "\"" << Printer::escapeStringLit(ii->m()->filepath()) << "\"";
        hadIncludedFiles = true;
      }
      return true;
    }
    void vVarDeclI(VarDeclI* vdi) {
      VarDecl* vd = vdi->e();
      if (vd->type().isPar() && !vd->type().isAnn() &&
          (vd->e() == nullptr ||
           (vd->e() == Constants::constants().absent &&
            Expression::ann(vd).contains(Constants::constants().ann.mzn_was_undefined)))) {
        if (hadInput) {
          ossInput << ", ";
        }
        output_var_desc_json(env, vd, ossInput);
        hadInput = true;
      }
    }
    void vSolveI(SolveI* si) {
      switch (si->st()) {
        case SolveI::ST_MIN:
          method = "min";
          break;
        case SolveI::ST_MAX:
          method = "max";
          break;
        case SolveI::ST_SAT:
          method = "sat";
          break;
      }
    }
    void vOutputI(OutputI* oi) { hasOutputItem = true; }
  } _ifc(env, skipDirs);
  iter_items(_ifc, m);

  bool hadOutput = false;
  std::ostringstream ossOutput;
  process_toplevel_output_vars(env.envi());
  for (auto& it : env.envi().outputVars) {
    auto* vd = Expression::cast<VarDecl>(it());
    if (vd->id()->str() == "_objective" || vd->id()->str() == "_checker_objective") {
      // Never include
      continue;
    }
    if (hadOutput) {
      ossOutput << ", ";
    }
    output_var_desc_json(env, vd, ossOutput);
    hadOutput = true;
  }

  os << "{\"type\": \"interface\", \"input\": {" << _ifc.ossInput.str() << "}, \"output\": {"
     << ossOutput.str() << "}";
  os << ", \"method\": \"";
  os << _ifc.method;
  os << "\"";
  os << ", \"has_output_item\": " << (_ifc.hasOutputItem ? "true" : "false");
  os << ", \"included_files\": [" << _ifc.ossIncludedFiles.str() << "]";
  os << ", \"globals\": [";
  bool first = true;
  for (const auto& g : model_globals(m, skipDirs)) {
    os << (first ? "    " : ", ") << "\"" << g << "\"";
    first = false;
  }
  os << "]";

  os << "}\n";
}

std::string create_enum_to_string_name(Id* ident, const std::string& prefix) {
  std::ostringstream ss;
  ss << prefix << *ident;
  return ss.str();
}

}  // namespace MiniZinc
