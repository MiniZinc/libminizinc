/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/astexception.hh>
#include <minizinc/astiterator.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/hash.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/typecheck.hh>

#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>

namespace MiniZinc {

Scopes::Scopes() { _s.emplace_back(ST_TOPLEVEL); }

void Scopes::add(EnvI& env, VarDecl* vd) {
  if (!_s.back().toplevel() && vd->ti()->isEnum() && (vd->e() != nullptr)) {
    throw TypeError(env, vd->loc(), "enums are only allowed at top level");
  }
  if (vd->id()->idn() == -1 && vd->id()->v() == "") {
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
        ASTString warnloc_f = vd->loc().filename();
        unsigned int warnloc_l = vd->id()->loc().firstLine();
        unsigned int warnloc_c = vd->id()->loc().firstColumn();
        unsigned int earlier_l = previous->second->id()->loc().firstLine();
        unsigned int earlier_c = previous->second->id()->loc().firstColumn();
        oss << "\n  " << warnloc_f << ":" << warnloc_l << "." << warnloc_c << ":\n";
        oss << "  Variable `" << *vd->id() << "' shadows variable with the same name in line "
            << earlier_l << "." << earlier_c;
        env.addWarning(oss.str());
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
  } else {
    std::ostringstream ss;
    ss << "identifier `" << vd->id()->str() << "' already defined";
    throw TypeError(env, vd->loc(), ss.str());
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
void create_enum_mapper(EnvI& env, Model* m, unsigned int enumId, VarDecl* vd, Model* enumItems) {
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
    auto* vd_aa = new VarDecl(Location().introduce(), ti_aa, "x");
    vd_aa->toplevel(false);

    auto* ti_ab = new TypeInst(Location().introduce(), Type::parbool());
    auto* vd_ab = new VarDecl(Location().introduce(), ti_ab, "b");
    vd_ab->toplevel(false);

    auto* ti_aj = new TypeInst(Location().introduce(), Type::parbool());
    auto* vd_aj = new VarDecl(Location().introduce(), ti_aj, "json");
    vd_aj->toplevel(false);

    auto* ti_fi = new TypeInst(Location().introduce(), Type::parstring());
    std::vector<VarDecl*> fi_params(3);
    fi_params[0] = vd_aa;
    fi_params[1] = vd_ab;
    fi_params[2] = vd_aj;
    auto* fi =
        new FunctionI(Location().introduce(), create_enum_to_string_name(ident, "_toString_"),
                      ti_fi, fi_params, nullptr);
    enumItems->addItem(fi);

    return;
  }

  Call* c = vd->e()->dynamicCast<Call>();
  auto* al = vd->e()->dynamicCast<ArrayLit>();

  std::vector<Expression*> parts;
  if (vd->e()->isa<SetLit>()) {
    parts.push_back(vd->e());
  } else if ((al != nullptr) || ((c != nullptr) && c->id() == "anon_enum" && c->argCount() == 1 &&
                                 c->arg(0)->isa<ArrayLit>())) {
    if (c != nullptr) {
      al = c->arg(0)->cast<ArrayLit>();
    }
    std::vector<Expression*> enumIds(al->size());
    for (unsigned int i = 0; i < al->size(); i++) {
      if (Id* eid = (*al)[i]->dynamicCast<Id>()) {
        enumIds[i] = eid;
      } else {
        std::ostringstream ss;
        ss << "invalid initialisation for enum `" << ident->v() << "'";
        throw TypeError(env, vd->e()->loc(), ss.str());
      }
    }
    parts.push_back(new SetLit(vd->e()->loc(), enumIds));
  } else if (c != nullptr) {
    if (c->id() == "enumFromConstructors") {
      if (c->argCount() != 1 || !c->arg(0)->isa<ArrayLit>()) {
        throw TypeError(env, c->loc(),
                        "enumFromConstructors used with incorrect argument type (only supports "
                        "array literals)");
      }
      auto* al = c->arg(0)->cast<ArrayLit>();
      for (unsigned int i = 0; i < al->size(); i++) {
        parts.push_back((*al)[i]);
      }
    } else {
      parts.push_back(c);
    }
  } else {
    throw TypeError(env, vd->e()->loc(),
                    std::string("invalid initialisation for enum `") + ident->v().c_str() + "'");
  }

  std::vector<Expression*> partCardinality;
  for (unsigned int p = 0; p < parts.size(); p++) {
    if (auto* sl = parts[p]->dynamicCast<SetLit>()) {
      for (unsigned int i = 0; i < sl->v().size(); i++) {
        if (!sl->v()[i]->isa<Id>()) {
          throw TypeError(
              env, sl->v()[i]->loc(),
              std::string("invalid initialisation for enum `") + ident->v().c_str() + "'");
        }
        auto* ti_id = new TypeInst(sl->v()[i]->loc(), Type::parenum(enumId));

        std::vector<Expression*> toEnumArgs(2);
        toEnumArgs[0] = vd->id();
        if (partCardinality.empty()) {
          toEnumArgs[1] = IntLit::a(i + 1);
        } else {
          toEnumArgs[1] =
              new BinOp(Location().introduce(), partCardinality.back(), BOT_PLUS, IntLit::a(i + 1));
        }
        Call* toEnum = new Call(sl->v()[i]->loc(), ASTString("to_enum"), toEnumArgs);
        auto* vd_id = new VarDecl(ti_id->loc(), ti_id, sl->v()[i]->cast<Id>()->str(), toEnum);
        auto* vdi_id = new VarDeclI(vd_id->loc(), vd_id);
        std::string str(sl->v()[i]->cast<Id>()->str().c_str());
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
        std::string str(sl->v()[i]->cast<Id>()->str().c_str());
        if (str.size() >= 2 && str[0] == '\'' && str[str.size() - 1] == '\'') {
          al_args[i] =
              new StringLit(Location().introduce(), ASTString(str.substr(1, str.size() - 2)));
        } else {
          al_args[i] = new StringLit(Location().introduce(), ASTString(str));
        }
        /// TODO: reimplement reverseEnum with a symbol table into the model (so you can evalPar an
        /// expression)
      }
      auto* al = new ArrayLit(Location().introduce(), al_args);

      std::vector<TypeInst*> ranges(1);
      ranges[0] = new TypeInst(Location().introduce(), Type::parint());
      auto* ti = new TypeInst(Location().introduce(), Type::parstring(1));
      ti->setRanges(ranges);
      auto* vd_enumToString = new VarDecl(Location().introduce(), ti, name, al);
      enumItems->addItem(new VarDeclI(Location().introduce(), vd_enumToString));

      Type tx = Type::parint();
      tx.ot(Type::OT_OPTIONAL);
      auto* ti_aa = new TypeInst(Location().introduce(), tx);
      auto* vd_aa = new VarDecl(Location().introduce(), ti_aa, "x");
      vd_aa->toplevel(false);
      auto* ti_ab = new TypeInst(Location().introduce(), Type::parbool());
      auto* vd_ab = new VarDecl(Location().introduce(), ti_ab, "b");
      vd_ab->toplevel(false);
      auto* ti_aj = new TypeInst(Location().introduce(), Type::parbool());
      auto* vd_aj = new VarDecl(Location().introduce(), ti_aj, "json");
      vd_aj->toplevel(false);
      auto* ti_fi = new TypeInst(Location().introduce(), Type::parstring());
      std::vector<VarDecl*> fi_params(3);
      fi_params[0] = vd_aa;
      fi_params[1] = vd_ab;
      fi_params[2] = vd_aj;

      std::vector<Expression*> deopt_args(1);
      deopt_args[0] = vd_aa->id();
      Call* deopt = new Call(Location().introduce(), "deopt", deopt_args);
      Call* occurs = new Call(Location().introduce(), "occurs", deopt_args);
      std::vector<Expression*> aa_args(1);
      aa_args[0] = deopt;
      auto* aa = new ArrayAccess(Location().introduce(), vd_enumToString->id(), aa_args);

      auto* sl_absent = new StringLit(Location().introduce(), "<>");

      ITE* if_absent = new ITE(
          Location().introduce(),
          {vd_aj->id(), new StringLit(Location().introduce(), ASTString("null"))}, sl_absent);

      auto* json_e_quote = new StringLit(Location().introduce(), ASTString("{\"e\":\""));
      auto* json_e_quote_end = new StringLit(Location().introduce(), ASTString("\"}"));
      auto* quote_aa = new BinOp(Location().introduce(), json_e_quote, BOT_PLUSPLUS, aa);
      auto* quote_aa2 = new BinOp(Location().introduce(), quote_aa, BOT_PLUSPLUS, json_e_quote_end);

      Call* quote_dzn = new Call(Location().introduce(), ASTString("showDznId"), {aa});

      std::vector<Expression*> ite_ifelse(2);
      ite_ifelse[0] = occurs;
      ite_ifelse[1] =
          new ITE(Location().introduce(), {vd_ab->id(), quote_dzn, vd_aj->id(), quote_aa2}, aa);

      ITE* ite = new ITE(Location().introduce(), ite_ifelse, if_absent);

      std::string toString = "_toString_";
      if (parts.size() > 1) {
        toString += std::to_string(p) + "_";
      }

      auto* fi = new FunctionI(Location().introduce(), create_enum_to_string_name(ident, toString),
                               ti_fi, fi_params, ite);
      enumItems->addItem(fi);
    } else if (Call* c = parts[p]->dynamicCast<Call>()) {
      if (c->id() == "anon_enum") {
        Type tx = Type::parint();
        tx.ot(Type::OT_OPTIONAL);
        auto* ti_aa = new TypeInst(Location().introduce(), tx);
        auto* vd_aa = new VarDecl(Location().introduce(), ti_aa, "x");
        vd_aa->toplevel(false);

        auto* ti_ab = new TypeInst(Location().introduce(), Type::parbool());
        auto* vd_ab = new VarDecl(Location().introduce(), ti_ab, "b");
        vd_ab->toplevel(false);

        auto* ti_aj = new TypeInst(Location().introduce(), Type::parbool());
        auto* vd_aj = new VarDecl(Location().introduce(), ti_aj, "json");
        vd_aj->toplevel(false);

        std::vector<Expression*> deopt_args(1);
        deopt_args[0] = vd_aa->id();
        Call* deopt = new Call(Location().introduce(), "deopt", deopt_args);
        Call* if_absent = new Call(Location().introduce(), "absent", deopt_args);
        auto* sl_absent_dzn = new StringLit(Location().introduce(), "<>");
        ITE* sl_absent = new ITE(
            Location().introduce(),
            {vd_aj->id(), new StringLit(Location().introduce(), ASTString("null"))}, sl_absent_dzn);

        auto* sl_dzn = new StringLit(Location().introduce(), ASTString(std::string("to_enum(") +
                                                                       ident->str().c_str() + ","));

        std::vector<Expression*> showIntArgs(1);
        if (partCardinality.empty()) {
          showIntArgs[0] = deopt;
          partCardinality.push_back(c->arg(0));
        } else {
          showIntArgs[0] =
              new BinOp(Location().introduce(), partCardinality.back(), BOT_PLUS, deopt);
          partCardinality.push_back(
              new BinOp(Location().introduce(), partCardinality.back(), BOT_PLUS, c->arg(0)));
        }

        Call* showInt = new Call(Location().introduce(), constants().ids.show, showIntArgs);
        auto* construct_string_dzn =
            new BinOp(Location().introduce(), sl_dzn, BOT_PLUSPLUS, showInt);
        auto* closing_bracket = new StringLit(Location().introduce(), ASTString(")"));
        auto* construct_string_dzn_2 =
            new BinOp(Location().introduce(), construct_string_dzn, BOT_PLUSPLUS, closing_bracket);

        auto* sl = new StringLit(Location().introduce(),
                                 ASTString(std::string(ident->str().c_str()) + "_"));
        auto* construct_string = new BinOp(Location().introduce(), sl, BOT_PLUSPLUS, showInt);

        auto* json_e_quote = new StringLit(Location().introduce(), ASTString("{\"e\":\""));
        auto* json_e_quote_mid = new StringLit(Location().introduce(), ASTString("\", \"i\":"));
        auto* json_e_quote_end = new StringLit(Location().introduce(), ASTString("}"));
        auto* construct_string_json =
            new BinOp(Location().introduce(), json_e_quote, BOT_PLUSPLUS,
                      new StringLit(Location().introduce(), ident->str()));
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

        auto* fi =
            new FunctionI(Location().introduce(), create_enum_to_string_name(ident, toString),
                          ti_fi, fi_params, ite);
        enumItems->addItem(fi);
      } else {
        // This is an enum constructor C(E)

        if (c->argCount() != 1 || !c->arg(0)->isa<Id>()) {
          throw TypeError(env, c->loc(),
                          "enum constructors must have a single identifier as argument");
        }
        Id* otherEnumId = c->arg(0)->cast<Id>();

        // Generate:
        /*
         function X: C(E: x) = to_enum(X,partCardinality.back()+x)
         function var X: C(var E: x) = to_enum(X,partCardinality.back()+x)
         function opt X: C(opt E: x) = if occurs(x) then C(deopt(x)) else to_enum(x,<>) endif
         function var opt X: C(var opt E: x) = if occurs(x) then C(deopt(x)) else to_enum(x,<>)
         endif
         function set of X: C(set of E: x) = { C(i) | i in x }
         function var set of X: C(var set of E: x) = { C(i) | i in x }
         */
        {
          Type Xt = Type::parint();
          Xt.enumId(enumId);
          auto* Cfn_ti = new TypeInst(Location().introduce(), Xt);
          auto* Cfn_x_ti = new TypeInst(Location().introduce(), Type(), otherEnumId);
          auto* vd_x = new VarDecl(Location().introduce(), Cfn_x_ti, "x");
          vd_x->toplevel(false);
          Expression* realX;
          if (partCardinality.empty()) {
            realX = vd_x->id();
          } else {
            realX = new BinOp(Location().introduce(), partCardinality.back(), BOT_PLUS, vd_x->id());
          }
          auto* Cfn_body = new Call(Location().introduce(), "to_enum", {vd->id(), realX});

          std::string Cfn_id(c->id().c_str());
          auto* Cfn = new FunctionI(Location().introduce(), Cfn_id, Cfn_ti, {vd_x}, Cfn_body);
          env.reverseEnum[Cfn_id] = Cfn;
          enumItems->addItem(Cfn);
        }
        {
          Type Xt = Type::varint();
          Xt.enumId(enumId);
          auto* Cfn_ti = new TypeInst(Location().introduce(), Xt);
          Type argT;
          argT.ti(Type::TI_VAR);
          auto* Cfn_x_ti = new TypeInst(Location().introduce(), argT, otherEnumId);
          auto* vd_x = new VarDecl(Location().introduce(), Cfn_x_ti, "x");
          vd_x->toplevel(false);
          Expression* realX;
          if (partCardinality.empty()) {
            realX = vd_x->id();
          } else {
            realX = new BinOp(Location().introduce(), partCardinality.back(), BOT_PLUS, vd_x->id());
          }
          auto* Cfn_body = new Call(Location().introduce(), "to_enum", {vd->id(), realX});

          std::string Cfn_id(c->id().c_str());
          auto* Cfn = new FunctionI(Location().introduce(), Cfn_id, Cfn_ti, {vd_x}, Cfn_body);
          enumItems->addItem(Cfn);
        }
        {
          Type Xt = Type::parint();
          Xt.ot(Type::OT_OPTIONAL);
          Xt.enumId(enumId);
          auto* Cfn_ti = new TypeInst(Location().introduce(), Xt);
          Type argT;
          argT.ot(Type::OT_OPTIONAL);
          auto* Cfn_x_ti = new TypeInst(Location().introduce(), argT, otherEnumId);
          auto* vd_x = new VarDecl(Location().introduce(), Cfn_x_ti, "x");
          std::string Cfn_id(c->id().c_str());
          vd_x->toplevel(false);
          auto* occurs = new Call(Location().introduce(), "occurs", {vd_x->id()});
          auto* deopt = new Call(Location().introduce(), "deopt", {vd_x->id()});
          auto* inv = new Call(Location().introduce(), Cfn_id, {deopt});
          auto* toEnumAbsent =
              new Call(Location().introduce(), "to_enum", {vd->id(), constants().absent});
          auto* ite = new ITE(Location().introduce(), {occurs, inv}, toEnumAbsent);
          auto* Cfn = new FunctionI(Location().introduce(), Cfn_id, Cfn_ti, {vd_x}, ite);
          enumItems->addItem(Cfn);
        }
        {
          Type Xt = Type::varint();
          Xt.ot(Type::OT_OPTIONAL);
          Xt.enumId(enumId);
          auto* Cfn_ti = new TypeInst(Location().introduce(), Xt);
          Type argT;
          argT.ti(Type::TI_VAR);
          argT.ot(Type::OT_OPTIONAL);
          auto* Cfn_x_ti = new TypeInst(Location().introduce(), argT, otherEnumId);
          auto* vd_x = new VarDecl(Location().introduce(), Cfn_x_ti, "x");
          std::string Cfn_id(c->id().c_str());
          vd_x->toplevel(false);
          auto* occurs = new Call(Location().introduce(), "occurs", {vd_x->id()});
          auto* deopt = new Call(Location().introduce(), "deopt", {vd_x->id()});
          auto* toEnumAbsent =
              new Call(Location().introduce(), "to_enum", {vd->id(), constants().absent});
          auto* inv = new Call(Location().introduce(), Cfn_id, {deopt});
          auto* ite = new ITE(Location().introduce(), {occurs, inv}, toEnumAbsent);
          auto* Cfn = new FunctionI(Location().introduce(), Cfn_id, Cfn_ti, {vd_x}, ite);
          enumItems->addItem(Cfn);
        }
        {
          Type Xt = Type::parint();
          Xt.st(Type::ST_SET);
          Xt.enumId(enumId);
          auto* Cfn_ti = new TypeInst(Location().introduce(), Xt);
          Type argT;
          argT.st(Type::ST_SET);
          auto* Cfn_x_ti = new TypeInst(Location().introduce(), argT, otherEnumId);
          auto* vd_x = new VarDecl(Location().introduce(), Cfn_x_ti, "x");
          std::string Cfn_id(c->id().c_str());
          vd_x->toplevel(false);
          auto* s_ti = new TypeInst(Location().introduce(), Type::parint());
          auto* s = new VarDecl(Location().introduce(), s_ti, "s", nullptr);
          s->toplevel(false);
          auto* inv = new Call(Location().introduce(), Cfn_id, {s->id()});
          Generator gen({s}, vd_x->id(), nullptr);
          Generators gens;
          gens.g = {gen};
          auto* comprehension = new Comprehension(Location().introduce(), inv, gens, true);
          auto* Cfn = new FunctionI(Location().introduce(), Cfn_id, Cfn_ti, {vd_x}, comprehension);
          enumItems->addItem(Cfn);
        }
        {
          Type Xt = Type::varint();
          Xt.st(Type::ST_SET);
          Xt.enumId(enumId);
          auto* Cfn_ti = new TypeInst(Location().introduce(), Xt);
          Type argT;
          argT.ti(Type::TI_VAR);
          argT.st(Type::ST_SET);
          auto* Cfn_x_ti = new TypeInst(Location().introduce(), argT, otherEnumId);
          auto* vd_x = new VarDecl(Location().introduce(), Cfn_x_ti, "x");
          std::string Cfn_id(c->id().c_str());
          vd_x->toplevel(false);
          auto* s_ti = new TypeInst(Location().introduce(), Type::parint());
          auto* s = new VarDecl(Location().introduce(), s_ti, "s", nullptr);
          s->toplevel(false);
          auto* inv = new Call(Location().introduce(), Cfn_id, {s->id()});
          Generator gen({s}, vd_x->id(), nullptr);
          Generators gens;
          gens.g = {gen};
          auto* comprehension = new Comprehension(Location().introduce(), inv, gens, true);
          auto* Cfn = new FunctionI(Location().introduce(), Cfn_id, Cfn_ti, {vd_x}, comprehension);
          enumItems->addItem(Cfn);
        }
        /*
         function E: C⁻¹(X: x) = to_enum(E,x-partCardinality.back())
         function var E: C⁻¹(var X: x) = to_enum(E,x-partCardinality.back())
         function opt E: C⁻¹(opt X: x) = if occurs(x) then C⁻¹(deopt(x)) else to_enum(x,<>) endif
         function var opt E: C⁻¹(var opt X: x) = if occurs(x) then C⁻¹(deopt(x)) else to_enum(x,<>)
         endif
         function set of E: C⁻¹(set of X: x) = { C⁻¹(i) | i in x }
         function var set of E: C⁻¹(var set of X: x) = { C⁻¹(i) | i in x }
         */
        {
          auto* toEfn_ti = new TypeInst(Location().introduce(), Type(), otherEnumId);
          Type Xt = Type::parint();
          Xt.enumId(enumId);
          auto* toEfn_x_ti = new TypeInst(Location().introduce(), Xt, vd->id());
          auto* vd_x = new VarDecl(Location().introduce(), toEfn_x_ti, "x");
          vd_x->toplevel(false);
          Expression* realX;
          if (partCardinality.empty()) {
            realX = vd_x->id();
          } else {
            realX =
                new BinOp(Location().introduce(), vd_x->id(), BOT_MINUS, partCardinality.back());
          }
          auto* toEfn_body = new Call(Location().introduce(), "to_enum", {otherEnumId, realX});

          std::string Cinv_id(std::string(c->id().c_str()) + "⁻¹");
          auto* toEfn =
              new FunctionI(Location().introduce(), Cinv_id, toEfn_ti, {vd_x}, toEfn_body);
          enumItems->addItem(toEfn);
        }
        {
          Type rT;
          rT.ti(Type::TI_VAR);
          auto* toEfn_ti = new TypeInst(Location().introduce(), rT, otherEnumId);
          Type Xt = Type::varint();
          Xt.enumId(enumId);
          auto* toEfn_x_ti = new TypeInst(Location().introduce(), Xt, vd->id());
          auto* vd_x = new VarDecl(Location().introduce(), toEfn_x_ti, "x");
          vd_x->toplevel(false);
          Expression* realX;
          if (partCardinality.empty()) {
            realX = vd_x->id();
          } else {
            realX =
                new BinOp(Location().introduce(), vd_x->id(), BOT_MINUS, partCardinality.back());
          }
          auto* toEfn_body = new Call(Location().introduce(), "to_enum", {otherEnumId, realX});

          std::string Cinv_id(std::string(c->id().c_str()) + "⁻¹");
          auto* toEfn =
              new FunctionI(Location().introduce(), Cinv_id, toEfn_ti, {vd_x}, toEfn_body);
          enumItems->addItem(toEfn);
        }
        {
          Type rt;
          rt.ot(Type::OT_OPTIONAL);
          auto* Cfn_ti = new TypeInst(Location().introduce(), rt, otherEnumId);
          Type argT;
          argT.ot(Type::OT_OPTIONAL);
          argT.enumId(enumId);
          auto* Cfn_x_ti = new TypeInst(Location().introduce(), argT, vd->id());
          auto* vd_x = new VarDecl(Location().introduce(), Cfn_x_ti, "x");
          std::string Cinv_id(std::string(c->id().c_str()) + "⁻¹");
          vd_x->toplevel(false);
          auto* occurs = new Call(Location().introduce(), "occurs", {vd_x->id()});
          auto* deopt = new Call(Location().introduce(), "deopt", {vd_x->id()});
          auto* inv = new Call(Location().introduce(), Cinv_id, {deopt});
          auto* toEnumAbsent =
              new Call(Location().introduce(), "to_enum", {otherEnumId, constants().absent});
          auto* ite = new ITE(Location().introduce(), {occurs, inv}, toEnumAbsent);
          auto* Cfn = new FunctionI(Location().introduce(), Cinv_id, Cfn_ti, {vd_x}, ite);
          enumItems->addItem(Cfn);
        }
        {
          Type rt;
          rt.ti(Type::TI_VAR);
          rt.ot(Type::OT_OPTIONAL);
          auto* Cfn_ti = new TypeInst(Location().introduce(), rt, otherEnumId);
          Type argT = Type::varint();
          argT.ot(Type::OT_OPTIONAL);
          argT.enumId(enumId);
          auto* Cfn_x_ti = new TypeInst(Location().introduce(), argT, vd->id());
          auto* vd_x = new VarDecl(Location().introduce(), Cfn_x_ti, "x");
          std::string Cinv_id(std::string(c->id().c_str()) + "⁻¹");
          vd_x->toplevel(false);
          auto* occurs = new Call(Location().introduce(), "occurs", {vd_x->id()});
          auto* deopt = new Call(Location().introduce(), "deopt", {vd_x->id()});
          auto* inv = new Call(Location().introduce(), Cinv_id, {deopt});
          auto* toEnumAbsent =
              new Call(Location().introduce(), "to_enum", {otherEnumId, constants().absent});
          auto* ite = new ITE(Location().introduce(), {occurs, inv}, toEnumAbsent);
          auto* Cfn = new FunctionI(Location().introduce(), Cinv_id, Cfn_ti, {vd_x}, ite);
          enumItems->addItem(Cfn);
        }
        {
          Type Xt;
          Xt.st(Type::ST_SET);
          auto* Cfn_ti = new TypeInst(Location().introduce(), Xt, otherEnumId);
          Type argT = Type::parint();
          argT.st(Type::ST_SET);
          argT.enumId(enumId);
          auto* Cfn_x_ti = new TypeInst(Location().introduce(), argT, vd->id());
          auto* vd_x = new VarDecl(Location().introduce(), Cfn_x_ti, "x");
          vd_x->toplevel(false);
          std::string Cinv_id(std::string(c->id().c_str()) + "⁻¹");
          auto* s_ti = new TypeInst(Location().introduce(), Type::parint());
          auto* s = new VarDecl(Location().introduce(), s_ti, "s", nullptr);
          s->toplevel(false);
          auto* inv = new Call(Location().introduce(), Cinv_id, {s->id()});
          Generator gen({s}, vd_x->id(), nullptr);
          Generators gens;
          gens.g = {gen};
          auto* comprehension = new Comprehension(Location().introduce(), inv, gens, true);
          auto* Cfn = new FunctionI(Location().introduce(), Cinv_id, Cfn_ti, {vd_x}, comprehension);
          enumItems->addItem(Cfn);
        }
        {
          Type Xt;
          Xt.ti(Type::TI_VAR);
          Xt.st(Type::ST_SET);
          auto* Cfn_ti = new TypeInst(Location().introduce(), Xt, otherEnumId);
          Type argT = Type::varint();
          argT.st(Type::ST_SET);
          argT.enumId(enumId);
          auto* Cfn_x_ti = new TypeInst(Location().introduce(), argT, vd->id());
          auto* vd_x = new VarDecl(Location().introduce(), Cfn_x_ti, "x");
          vd_x->toplevel(false);
          std::string Cinv_id(std::string(c->id().c_str()) + "⁻¹");
          auto* s_ti = new TypeInst(Location().introduce(), Type::varint());
          auto* s = new VarDecl(Location().introduce(), s_ti, "s", nullptr);
          s->toplevel(false);
          auto* inv = new Call(Location().introduce(), Cinv_id, {s->id()});
          Generator gen({s}, vd_x->id(), nullptr);
          Generators gens;
          gens.g = {gen};
          auto* comprehension = new Comprehension(Location().introduce(), inv, gens, true);
          auto* Cfn = new FunctionI(Location().introduce(), Cinv_id, Cfn_ti, {vd_x}, comprehension);
          enumItems->addItem(Cfn);
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
          tx.ot(Type::OT_OPTIONAL);
          auto* ti_aa = new TypeInst(Location().introduce(), tx);
          auto* vd_aa = new VarDecl(Location().introduce(), ti_aa, "x");
          vd_aa->toplevel(false);

          auto* ti_ab = new TypeInst(Location().introduce(), Type::parbool());
          auto* vd_ab = new VarDecl(Location().introduce(), ti_ab, "b");
          vd_ab->toplevel(false);

          auto* ti_aj = new TypeInst(Location().introduce(), Type::parbool());
          auto* vd_aj = new VarDecl(Location().introduce(), ti_aj, "json");
          vd_aj->toplevel(false);

          std::vector<Expression*> deopt_args(1);
          deopt_args[0] = vd_aa->id();
          Call* deopt = new Call(Location().introduce(), "deopt", deopt_args);
          Call* if_absent = new Call(Location().introduce(), "absent", deopt_args);
          auto* sl_absent_dzn = new StringLit(Location().introduce(), "<>");
          ITE* sl_absent =
              new ITE(Location().introduce(),
                      {vd_aj->id(), new StringLit(Location().introduce(), ASTString("null"))},
                      sl_absent_dzn);

          Call* toEnumE = new Call(Location().introduce(), "to_enum", {otherEnumId, deopt});
          Call* toString = new Call(Location().introduce(),
                                    create_enum_to_string_name(otherEnumId, "_toString_"),
                                    {toEnumE, vd_ab->id(), vd_aj->id()});

          auto* openOther =
              new StringLit(Location().introduce(), std::string(c->id().c_str()) + "(");
          auto* openJson =
              new StringLit(Location().introduce(),
                            "{ \"c\" : \"" + std::string(c->id().c_str()) + "\", \"e\" : ");
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

          auto* fi =
              new FunctionI(Location().introduce(), create_enum_to_string_name(ident, XtoString),
                            ti_fi, fi_params, ite);
          enumItems->addItem(fi);
        }

        Call* cardE = new Call(Location().introduce(), "card", {otherEnumId});
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
  auto* rhs = new BinOp(vd->loc(), IntLit::a(1), BOT_DOTDOT, upperBound);
  vd->e(rhs);

  if (parts.size() > 1) {
    Type tx = Type::parint();
    tx.ot(Type::OT_OPTIONAL);
    auto* ti_aa = new TypeInst(Location().introduce(), tx);
    auto* vd_aa = new VarDecl(Location().introduce(), ti_aa, "x");
    vd_aa->toplevel(false);

    auto* ti_ab = new TypeInst(Location().introduce(), Type::parbool());
    auto* vd_ab = new VarDecl(Location().introduce(), ti_ab, "b");
    vd_ab->toplevel(false);

    auto* ti_aj = new TypeInst(Location().introduce(), Type::parbool());
    auto* vd_aj = new VarDecl(Location().introduce(), ti_aj, "json");
    vd_aj->toplevel(false);

    std::vector<Expression*> deopt_args(1);
    deopt_args[0] = vd_aa->id();
    Call* deopt = new Call(Location().introduce(), "deopt", deopt_args);
    Call* if_absent = new Call(Location().introduce(), "absent", deopt_args);
    auto* sl_absent_dzn = new StringLit(Location().introduce(), "<>");
    ITE* sl_absent = new ITE(
        Location().introduce(),
        {vd_aj->id(), new StringLit(Location().introduce(), ASTString("null"))}, sl_absent_dzn);

    std::vector<Expression*> ite_cases_a;
    Expression* ite_cases_else;
    for (unsigned int i = 0; i < parts.size(); i++) {
      std::string toString = "_toString_" + std::to_string(i) + "_";
      Expression* aa;
      if (i == 0) {
        aa = vd_aa->id();
      } else {
        auto* bo = new BinOp(Location().introduce(), deopt, BOT_MINUS, partCardinality[i - 1]);
        Call* c = new Call(Location().introduce(), "to_enum", {vd->id(), bo});
        aa = c;
      }
      Call* c = new Call(Location().introduce(), create_enum_to_string_name(ident, toString),
                         {aa, vd_ab->id(), vd_aj->id()});
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
    auto* fi =
        new FunctionI(Location().introduce(), create_enum_to_string_name(ident, "_toString_"),
                      ti_fi, fi_params, ite);
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
    auto* vd_x = new VarDecl(Location().introduce(), x_ti, "x");
    vd_x->toplevel(false);

    auto* b_ti = new TypeInst(Location().introduce(), Type::parbool());
    auto* vd_b = new VarDecl(Location().introduce(), b_ti, "b");
    vd_b->toplevel(false);

    auto* j_ti = new TypeInst(Location().introduce(), Type::parbool());
    auto* vd_j = new VarDecl(Location().introduce(), j_ti, "json");
    vd_j->toplevel(false);

    auto* xx_range = new TypeInst(Location().introduce(), Type::parint(), nullptr);
    std::vector<TypeInst*> xx_ranges(1);
    xx_ranges[0] = xx_range;
    auto* xx_ti = new TypeInst(Location().introduce(), tx, xx_ranges, ident);

    std::vector<Expression*> array1dArgs(1);
    array1dArgs[0] = vd_x->id();
    Call* array1dCall = new Call(Location().introduce(), "array1d", array1dArgs);

    auto* vd_xx = new VarDecl(Location().introduce(), xx_ti, "xx", array1dCall);
    vd_xx->toplevel(false);

    auto* idx_i_ti = new TypeInst(Location().introduce(), Type::parint());
    auto* idx_i = new VarDecl(Location().introduce(), idx_i_ti, "i");
    idx_i->toplevel(false);

    std::vector<Expression*> aa_xxi_idx(1);
    aa_xxi_idx[0] = idx_i->id();
    auto* aa_xxi = new ArrayAccess(Location().introduce(), vd_xx->id(), aa_xxi_idx);

    std::vector<Expression*> _toString_ENUMArgs(3);
    _toString_ENUMArgs[0] = aa_xxi;
    _toString_ENUMArgs[1] = vd_b->id();
    _toString_ENUMArgs[2] = vd_j->id();
    Call* _toString_ENUM =
        new Call(Location().introduce(), create_enum_to_string_name(ident, "_toString_"),
                 _toString_ENUMArgs);

    std::vector<Expression*> index_set_xx_args(1);
    index_set_xx_args[0] = vd_xx->id();
    Call* index_set_xx = new Call(Location().introduce(), "index_set", index_set_xx_args);
    std::vector<VarDecl*> gen_exps(1);
    gen_exps[0] = idx_i;
    Generator gen(gen_exps, index_set_xx, nullptr);

    Generators generators;
    generators.g.push_back(gen);
    auto* comp = new Comprehension(Location().introduce(), _toString_ENUM, generators, false);

    std::vector<Expression*> join_args(2);
    join_args[0] = new StringLit(Location().introduce(), ", ");
    join_args[1] = comp;
    Call* join = new Call(Location().introduce(), "join", join_args);

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
    auto* fi =
        new FunctionI(Location().introduce(), create_enum_to_string_name(ident, "_toString_"),
                      ti_fi, fi_params, let);
    enumItems->addItem(fi);
  }

  {
    /*

     function _toString_ENUM(opt set of ENUM: x, bool: b, bool: json) =
       if absent(x) then "<>" else "{" ++ join(", ", [ _toString_ENUM(i,b,json) | i in x ]) ++ "}"
     endif;

     */

    Type argType = Type::parsetenum(ident->type().enumId());
    argType.ot(Type::OT_OPTIONAL);
    auto* x_ti = new TypeInst(Location().introduce(), argType, ident);
    auto* vd_x = new VarDecl(Location().introduce(), x_ti, "x");
    vd_x->toplevel(false);

    auto* b_ti = new TypeInst(Location().introduce(), Type::parbool());
    auto* vd_b = new VarDecl(Location().introduce(), b_ti, "b");
    vd_b->toplevel(false);

    auto* j_ti = new TypeInst(Location().introduce(), Type::parbool());
    auto* vd_j = new VarDecl(Location().introduce(), j_ti, "json");
    vd_j->toplevel(false);

    auto* idx_i_ti = new TypeInst(Location().introduce(), Type::parint());
    auto* idx_i = new VarDecl(Location().introduce(), idx_i_ti, "i");
    idx_i->toplevel(false);

    std::vector<Expression*> _toString_ENUMArgs(3);
    _toString_ENUMArgs[0] = idx_i->id();
    _toString_ENUMArgs[1] = vd_b->id();
    _toString_ENUMArgs[2] = vd_j->id();
    Call* _toString_ENUM =
        new Call(Location().introduce(), create_enum_to_string_name(ident, "_toString_"),
                 _toString_ENUMArgs);

    std::vector<Expression*> deopt_args(1);
    deopt_args[0] = vd_x->id();
    Call* deopt = new Call(Location().introduce(), "deopt", deopt_args);
    Call* if_absent = new Call(Location().introduce(), "absent", deopt_args);
    auto* sl_absent_dzn = new StringLit(Location().introduce(), "<>");
    ITE* sl_absent = new ITE(Location().introduce(),
                             {vd_j->id(), new StringLit(Location().introduce(), ASTString("null"))},
                             sl_absent_dzn);

    std::vector<VarDecl*> gen_exps(1);
    gen_exps[0] = idx_i;
    Generator gen(gen_exps, deopt, nullptr);

    Generators generators;
    generators.g.push_back(gen);
    auto* comp = new Comprehension(Location().introduce(), _toString_ENUM, generators, false);

    std::vector<Expression*> join_args(2);
    join_args[0] = new StringLit(Location().introduce(), ", ");
    join_args[1] = comp;
    Call* join = new Call(Location().introduce(), "join", join_args);

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

    std::vector<Expression*> if_then(2);
    if_then[0] = if_absent;
    if_then[1] = sl_absent;
    ITE* ite = new ITE(Location().introduce(), if_then, bopp3);

    auto* ti_fi = new TypeInst(Location().introduce(), Type::parstring());
    std::vector<VarDecl*> fi_params(3);
    fi_params[0] = vd_x;
    fi_params[1] = vd_b;
    fi_params[2] = vd_j;
    auto* fi =
        new FunctionI(Location().introduce(), create_enum_to_string_name(ident, "_toString_"),
                      ti_fi, fi_params, ite);
    enumItems->addItem(fi);
  }

  {
    /*

     function _toString_ENUM(array[$U] of opt set of ENUM: x, bool: b, bool: json) =
     let {
     array[int] of opt set of ENUM: xx = array1d(x)
     } in "[" ++ join(", ", [ _toString_ENUM(xx[i],b,json) | i in index_set(xx) ]) ++ "]";

     */

    TIId* tiid = new TIId(Location().introduce(), "U");
    auto* ti_range = new TypeInst(Location().introduce(), Type::parint(), tiid);
    std::vector<TypeInst*> ranges(1);
    ranges[0] = ti_range;

    Type tx = Type::parsetint(-1);
    tx.ot(Type::OT_OPTIONAL);
    auto* x_ti = new TypeInst(Location().introduce(), tx, ranges, ident);
    auto* vd_x = new VarDecl(Location().introduce(), x_ti, "x");
    vd_x->toplevel(false);

    auto* b_ti = new TypeInst(Location().introduce(), Type::parbool());
    auto* vd_b = new VarDecl(Location().introduce(), b_ti, "b");
    vd_b->toplevel(false);

    auto* j_ti = new TypeInst(Location().introduce(), Type::parbool());
    auto* vd_j = new VarDecl(Location().introduce(), j_ti, "json");
    vd_j->toplevel(false);

    auto* xx_range = new TypeInst(Location().introduce(), Type::parint(), nullptr);
    std::vector<TypeInst*> xx_ranges(1);
    xx_ranges[0] = xx_range;
    auto* xx_ti = new TypeInst(Location().introduce(), tx, xx_ranges, ident);

    std::vector<Expression*> array1dArgs(1);
    array1dArgs[0] = vd_x->id();
    Call* array1dCall = new Call(Location().introduce(), "array1d", array1dArgs);

    auto* vd_xx = new VarDecl(Location().introduce(), xx_ti, "xx", array1dCall);
    vd_xx->toplevel(false);

    auto* idx_i_ti = new TypeInst(Location().introduce(), Type::parint());
    auto* idx_i = new VarDecl(Location().introduce(), idx_i_ti, "i");
    idx_i->toplevel(false);

    std::vector<Expression*> aa_xxi_idx(1);
    aa_xxi_idx[0] = idx_i->id();
    auto* aa_xxi = new ArrayAccess(Location().introduce(), vd_xx->id(), aa_xxi_idx);

    std::vector<Expression*> _toString_ENUMArgs(3);
    _toString_ENUMArgs[0] = aa_xxi;
    _toString_ENUMArgs[1] = vd_b->id();
    _toString_ENUMArgs[2] = vd_j->id();
    Call* _toString_ENUM =
        new Call(Location().introduce(), create_enum_to_string_name(ident, "_toString_"),
                 _toString_ENUMArgs);

    std::vector<Expression*> index_set_xx_args(1);
    index_set_xx_args[0] = vd_xx->id();
    Call* index_set_xx = new Call(Location().introduce(), "index_set", index_set_xx_args);
    std::vector<VarDecl*> gen_exps(1);
    gen_exps[0] = idx_i;
    Generator gen(gen_exps, index_set_xx, nullptr);

    Generators generators;
    generators.g.push_back(gen);
    auto* comp = new Comprehension(Location().introduce(), _toString_ENUM, generators, false);

    std::vector<Expression*> join_args(2);
    join_args[0] = new StringLit(Location().introduce(), ", ");
    join_args[1] = comp;
    Call* join = new Call(Location().introduce(), "join", join_args);

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
    auto* fi =
        new FunctionI(Location().introduce(), create_enum_to_string_name(ident, "_toString_"),
                      ti_fi, fi_params, let);
    enumItems->addItem(fi);
  }
}

void TopoSorter::add(EnvI& env, VarDeclI* vdi, bool handleEnums, Model* enumItems) {
  VarDecl* vd = vdi->e();
  if (handleEnums && vd->ti()->isEnum()) {
    unsigned int enumId = env.registerEnum(vdi);
    Type vdt = vd->type();
    vdt.enumId(enumId);
    vd->ti()->type(vdt);
    vd->type(vdt);

    create_enum_mapper(env, model, enumId, vd, enumItems);
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

VarDecl* TopoSorter::checkId(EnvI& env, const ASTString& id_v, const Location& loc) {
  GCLock lock;
  Id* id = new Id(loc, id_v, nullptr);
  return checkId(env, id, loc);
}

void TopoSorter::run(EnvI& env, Expression* e) {
  if (e == nullptr) {
    return;
  }
  switch (e->eid()) {
    case Expression::E_INTLIT:
    case Expression::E_FLOATLIT:
    case Expression::E_BOOLLIT:
    case Expression::E_STRINGLIT:
    case Expression::E_ANON:
      break;
    case Expression::E_SETLIT: {
      auto* sl = e->cast<SetLit>();
      if (sl->isv() == nullptr && sl->fsv() == nullptr) {
        for (unsigned int i = 0; i < sl->v().size(); i++) {
          run(env, sl->v()[i]);
        }
      }
    } break;
    case Expression::E_ID: {
      if (e != constants().absent) {
        VarDecl* vd = checkId(env, e->cast<Id>(), e->loc());
        e->cast<Id>()->decl(vd);
      }
    } break;
    case Expression::E_ARRAYLIT: {
      auto* al = e->cast<ArrayLit>();
      for (unsigned int i = 0; i < al->size(); i++) {
        run(env, (*al)[i]);
      }
    } break;
    case Expression::E_ARRAYACCESS: {
      auto* ae = e->cast<ArrayAccess>();
      run(env, ae->v());
      for (unsigned int i = 0; i < ae->idx().size(); i++) {
        run(env, ae->idx()[i]);
      }
    } break;
    case Expression::E_COMP: {
      auto* ce = e->cast<Comprehension>();
      scopes.push();
      for (int i = 0; i < ce->numberOfGenerators(); i++) {
        run(env, ce->in(i));
        for (int j = 0; j < ce->numberOfDecls(i); j++) {
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
      ITE* ite = e->cast<ITE>();
      for (int i = 0; i < ite->size(); i++) {
        run(env, ite->ifExpr(i));
        run(env, ite->thenExpr(i));
      }
      run(env, ite->elseExpr());
    } break;
    case Expression::E_BINOP: {
      auto* be = e->cast<BinOp>();
      std::vector<Expression*> todo;
      todo.push_back(be->lhs());
      todo.push_back(be->rhs());
      while (!todo.empty()) {
        Expression* be = todo.back();
        todo.pop_back();
        if (auto* e_bo = be->dynamicCast<BinOp>()) {
          todo.push_back(e_bo->lhs());
          todo.push_back(e_bo->rhs());
          for (ExpressionSetIter it = e_bo->ann().begin(); it != e_bo->ann().end(); ++it) {
            run(env, *it);
          }
        } else {
          run(env, be);
        }
      }
    } break;
    case Expression::E_UNOP: {
      UnOp* ue = e->cast<UnOp>();
      run(env, ue->e());
    } break;
    case Expression::E_CALL: {
      Call* ce = e->cast<Call>();
      for (unsigned int i = 0; i < ce->argCount(); i++) {
        run(env, ce->arg(i));
      }
    } break;
    case Expression::E_VARDECL: {
      auto* ve = e->cast<VarDecl>();
      auto pi = pos.find(ve);
      if (pi == pos.end()) {
        pos.insert(std::pair<VarDecl*, int>(ve, -1));
        run(env, ve->ti());
        run(env, ve->e());
        ve->payload(static_cast<int>(decls.size()));
        decls.push_back(ve);
        pi = pos.find(ve);
        pi->second = static_cast<int>(decls.size()) - 1;
      } else {
        assert(pi->second != -1);
      }
    } break;
    case Expression::E_TI: {
      auto* ti = e->cast<TypeInst>();
      for (unsigned int i = 0; i < ti->ranges().size(); i++) {
        run(env, ti->ranges()[i]);
      }
      run(env, ti->domain());
    } break;
    case Expression::E_TIID:
      break;
    case Expression::E_LET: {
      Let* let = e->cast<Let>();
      scopes.push();
      for (unsigned int i = 0; i < let->let().size(); i++) {
        run(env, let->let()[i]);
        if (auto* vd = let->let()[i]->dynamicCast<VarDecl>()) {
          scopes.add(env, vd);
        }
      }
      run(env, let->in());
      VarDeclCmp poscmp(pos);
      std::stable_sort(let->let().begin(), let->let().end(), poscmp);
      for (unsigned int i = 0, j = 0; i < let->let().size(); i++) {
        if (auto* vd = let->let()[i]->dynamicCast<VarDecl>()) {
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
    for (ExpressionSetIter it = e->ann().begin(); it != e->ann().end(); ++it) {
      try {
        run(env, *it);
      } catch (TypeError&) {
        toDelete.push_back(*it);
      }
      for (Expression* de : toDelete) {
        e->ann().remove(de);
      }
    }
  } else {
    for (ExpressionSetIter it = e->ann().begin(); it != e->ann().end(); ++it) {
      run(env, *it);
    }
  }
}

KeepAlive add_coercion(EnvI& env, Model* m, Expression* e, const Type& funarg_t) {
  if (e->isa<ArrayAccess>() && e->type().dim() > 0) {
    auto* aa = e->cast<ArrayAccess>();
    // Turn ArrayAccess into a slicing operation
    std::vector<Expression*> args;
    args.push_back(aa->v());
    args.push_back(nullptr);
    std::vector<Expression*> slice;
    GCLock lock;
    for (unsigned int i = 0; i < aa->idx().size(); i++) {
      if (aa->idx()[i]->type().isSet()) {
        bool needIdxSet = true;
        bool needInter = true;
        if (auto* sl = aa->idx()[i]->dynamicCast<SetLit>()) {
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
          Call* origIdxset = new Call(aa->v()->loc(), ASTString(oss.str()), origIdxsetArgs);
          FunctionI* fi = m->matchFn(env, origIdxset, false);
          if (fi == nullptr) {
            throw TypeError(env, e->loc(), "missing builtin " + oss.str());
          }
          origIdxset->type(fi->rtype(env, origIdxsetArgs, false));
          origIdxset->decl(fi);
          if (needInter) {
            auto* inter = new BinOp(aa->idx()[i]->loc(), aa->idx()[i], BOT_INTERSECT, origIdxset);
            inter->type(Type::parsetint());
            args.push_back(inter);
          } else {
            args.push_back(origIdxset);
          }
        }
        slice.push_back(aa->idx()[i]);
      } else {
        auto* bo = new BinOp(aa->idx()[i]->loc(), aa->idx()[i], BOT_DOTDOT, aa->idx()[i]);
        bo->type(Type::parsetint());
        slice.push_back(bo);
      }
    }
    auto* a_slice = new ArrayLit(e->loc(), slice);
    a_slice->type(Type::parsetint(1));
    args[1] = a_slice;
    std::ostringstream oss;
    oss << "slice_" << (args.size() - 2) << "d";
    Call* c = new Call(e->loc(), ASTString(oss.str()), args);
    FunctionI* fi = m->matchFn(env, c, false);
    if (fi == nullptr) {
      throw TypeError(env, e->loc(), "missing builtin " + oss.str());
    }
    c->type(fi->rtype(env, args, false));
    c->decl(fi);
    e = c;
  }
  if (e->type().dim() == funarg_t.dim() &&
      (funarg_t.bt() == Type::BT_BOT || funarg_t.bt() == Type::BT_TOP ||
       e->type().bt() == funarg_t.bt() || e->type().bt() == Type::BT_BOT)) {
    return e;
  }
  GCLock lock;
  Call* c = nullptr;
  if (e->type().dim() == 0 && funarg_t.dim() != 0) {
    if (e->type().isvar()) {
      throw TypeError(env, e->loc(), "cannot coerce var set into array");
    }
    if (e->type().isOpt()) {
      throw TypeError(env, e->loc(), "cannot coerce opt set into array");
    }
    std::vector<Expression*> set2a_args(1);
    set2a_args[0] = e;
    Call* set2a = new Call(e->loc(), ASTString("set2array"), set2a_args);
    FunctionI* fi = m->matchFn(env, set2a, false);
    if (fi != nullptr) {
      set2a->type(fi->rtype(env, set2a_args, false));
      set2a->decl(fi);
      e = set2a;
    }
  }
  if (funarg_t.bt() == Type::BT_TOP || e->type().bt() == funarg_t.bt() ||
      e->type().bt() == Type::BT_BOT) {
    KeepAlive ka(e);
    return ka;
  }
  std::vector<Expression*> args(1);
  args[0] = e;
  if (e->type().bt() == Type::BT_BOOL) {
    if (funarg_t.bt() == Type::BT_INT) {
      c = new Call(e->loc(), constants().ids.bool2int, args);
    } else if (funarg_t.bt() == Type::BT_FLOAT) {
      c = new Call(e->loc(), constants().ids.bool2float, args);
    }
  } else if (e->type().bt() == Type::BT_INT) {
    if (funarg_t.bt() == Type::BT_FLOAT) {
      c = new Call(e->loc(), constants().ids.int2float, args);
    }
  }
  if (c != nullptr) {
    FunctionI* fi = m->matchFn(env, c, false);
    assert(fi);
    Type ct = fi->rtype(env, args, false);
    ct.cv(e->type().cv() || ct.cv());
    c->type(ct);
    c->decl(fi);
    KeepAlive ka(c);
    return ka;
  }
  throw TypeError(env, e->loc(),
                  "cannot determine coercion from type " + e->type().toString(env) + " to type " +
                      funarg_t.toString(env));
}
KeepAlive add_coercion(EnvI& env, Model* m, Expression* e, Expression* funarg) {
  return add_coercion(env, m, e, funarg->type());
}

template <bool ignoreVarDecl>
class Typer {
private:
  EnvI& _env;
  Model* _model;
  std::vector<TypeError>& _typeErrors;
  bool _ignoreUndefined;

public:
  Typer(EnvI& env, Model* model, std::vector<TypeError>& typeErrors, bool ignoreUndefined)
      : _env(env), _model(model), _typeErrors(typeErrors), _ignoreUndefined(ignoreUndefined) {}
  /// Check annotations when expression is finished
  void exit(Expression* e) {
    for (ExpressionSetIter it = e->ann().begin(); it != e->ann().end(); ++it) {
      if (!(*it)->type().isAnn()) {
        throw TypeError(_env, (*it)->loc(),
                        "expected annotation, got `" + (*it)->type().toString(_env) + "'");
      }
    }
  }
  bool enter(Expression* /*e*/) { return true; }
  /// Visit integer literal
  void vIntLit(const IntLit& /*i*/) {}
  /// Visit floating point literal
  void vFloatLit(const FloatLit& /*f*/) {}
  /// Visit Boolean literal
  void vBoolLit(const BoolLit& /*b*/) {}
  /// Visit set literal
  void vSetLit(SetLit& sl) {
    Type ty;
    ty.st(Type::ST_SET);
    if (sl.isv() != nullptr) {
      ty.bt(Type::BT_INT);
      ty.enumId(sl.type().enumId());
      sl.type(ty);
      return;
    }
    if (sl.fsv() != nullptr) {
      ty.bt(Type::BT_FLOAT);
      sl.type(ty);
      return;
    }
    unsigned int enumId = sl.v().size() > 0 ? sl.v()[0]->type().enumId() : 0;
    for (unsigned int i = 0; i < sl.v().size(); i++) {
      Type vi_t = sl.v()[i]->type();
      vi_t.ot(Type::OT_PRESENT);
      if (sl.v()[i] == constants().absent) {
        continue;
      }
      if (vi_t.dim() > 0) {
        throw TypeError(_env, sl.v()[i]->loc(), "set literals cannot contain arrays");
      }
      if (vi_t.st() == Type::ST_SET) {
        throw TypeError(_env, sl.v()[i]->loc(), "set literals cannot contain sets");
      }
      if (vi_t.isvar()) {
        ty.ti(Type::TI_VAR);
      }
      if (vi_t.cv()) {
        ty.cv(true);
      }
      if (enumId != vi_t.enumId()) {
        enumId = 0;
      }
      if (!Type::btSubtype(vi_t, ty, true)) {
        if (ty.bt() == Type::BT_UNKNOWN || Type::btSubtype(ty, vi_t, true)) {
          ty.bt(vi_t.bt());
        } else {
          throw TypeError(_env, sl.loc(), "non-uniform set literal");
        }
      }
    }
    ty.enumId(enumId);
    if (ty.bt() == Type::BT_UNKNOWN) {
      ty.bt(Type::BT_BOT);
    } else {
      if (ty.isvar() && ty.bt() != Type::BT_INT) {
        if (ty.bt() == Type::BT_BOOL) {
          ty.bt(Type::BT_INT);
        } else {
          throw TypeError(_env, sl.loc(), "cannot coerce set literal element to var int");
        }
      }
      for (unsigned int i = 0; i < sl.v().size(); i++) {
        sl.v()[i] = add_coercion(_env, _model, sl.v()[i], ty)();
      }
    }
    sl.type(ty);
  }
  /// Visit string literal
  void vStringLit(const StringLit& /*sl*/) {}
  /// Visit identifier
  void vId(Id& id) {
    if (&id != constants().absent) {
      assert(!id.decl()->type().isunknown());
      id.type(id.decl()->type());
    }
  }
  /// Visit anonymous variable
  void vAnonVar(const AnonVar& /*v*/) {}
  /// Visit array literal
  void vArrayLit(ArrayLit& al) {
    Type ty;
    ty.dim(static_cast<int>(al.dims()));
    std::vector<AnonVar*> anons;
    bool haveAbsents = false;
    bool haveInferredType = false;
    for (unsigned int i = 0; i < al.size(); i++) {
      Expression* vi = al[i];
      if (vi->type().dim() > 0) {
        throw TypeError(_env, vi->loc(), "arrays cannot be elements of arrays");
      }
      if (vi == constants().absent) {
        haveAbsents = true;
      }
      auto* av = vi->dynamicCast<AnonVar>();
      if (av != nullptr) {
        ty.ti(Type::TI_VAR);
        anons.push_back(av);
      } else if (vi->type().isvar()) {
        ty.ti(Type::TI_VAR);
      }
      if (vi->type().cv()) {
        ty.cv(true);
      }
      if (vi->type().isOpt()) {
        ty.ot(Type::OT_OPTIONAL);
      }

      if (ty.bt() == Type::BT_UNKNOWN) {
        if (av == nullptr) {
          if (haveInferredType) {
            if (ty.st() != vi->type().st() && vi->type().ot() != Type::OT_OPTIONAL) {
              throw TypeError(_env, al.loc(), "non-uniform array literal");
            }
          } else {
            haveInferredType = true;
            ty.st(vi->type().st());
          }
          if (vi->type().bt() != Type::BT_BOT) {
            ty.bt(vi->type().bt());
            ty.enumId(vi->type().enumId());
          }
        }
      } else {
        if (av == nullptr) {
          if (vi->type().bt() == Type::BT_BOT) {
            if (vi->type().st() != ty.st() && vi->type().ot() != Type::OT_OPTIONAL) {
              throw TypeError(_env, al.loc(), "non-uniform array literal");
            }
            if (vi->type().enumId() != 0 && ty.enumId() != vi->type().enumId()) {
              ty.enumId(0);
            }
          } else {
            unsigned int tyEnumId = ty.enumId();
            ty.enumId(vi->type().enumId());
            if (Type::btSubtype(ty, vi->type(), true)) {
              ty.bt(vi->type().bt());
            }
            if (tyEnumId != vi->type().enumId()) {
              ty.enumId(0);
            }
            if (!Type::btSubtype(vi->type(), ty, true) || ty.st() != vi->type().st()) {
              throw TypeError(_env, al.loc(), "non-uniform array literal");
            }
          }
        }
      }
    }
    if (ty.bt() == Type::BT_UNKNOWN) {
      ty.bt(Type::BT_BOT);
      if (!anons.empty()) {
        throw TypeError(_env, al.loc(),
                        "array literal must contain at least one non-anonymous variable");
      }
      if (haveAbsents) {
        throw TypeError(_env, al.loc(), "array literal must contain at least one non-absent value");
      }
    } else {
      Type at = ty;
      at.dim(0);
      if (at.ti() == Type::TI_VAR && at.st() == Type::ST_SET && at.bt() != Type::BT_INT) {
        if (at.bt() == Type::BT_BOOL) {
          ty.bt(Type::BT_INT);
          at.bt(Type::BT_INT);
        } else {
          throw TypeError(_env, al.loc(), "cannot coerce array element to var set of int");
        }
      }
      for (auto& anon : anons) {
        anon->type(at);
      }
      for (unsigned int i = 0; i < al.size(); i++) {
        al.set(i, add_coercion(_env, _model, al[i], at)());
      }
    }
    if (ty.enumId() != 0) {
      std::vector<unsigned int> enumIds(ty.dim() + 1);
      for (int i = 0; i < ty.dim(); i++) {
        enumIds[i] = 0;
      }
      enumIds[ty.dim()] = ty.enumId();
      ty.enumId(_env.registerArrayEnum(enumIds));
    }
    al.type(ty);
  }
  /// Visit array access
  void vArrayAccess(ArrayAccess& aa) {
    if (aa.v()->type().dim() == 0) {
      if (aa.v()->type().st() == Type::ST_SET) {
        Type tv = aa.v()->type();
        tv.st(Type::ST_PLAIN);
        tv.dim(1);
        aa.v(add_coercion(_env, _model, aa.v(), tv)());
      } else {
        std::ostringstream oss;
        oss << "array access attempted on expression of type `" << aa.v()->type().toString(_env)
            << "'";
        throw TypeError(_env, aa.v()->loc(), oss.str());
      }
    } else if (aa.v()->isa<ArrayAccess>()) {
      aa.v(add_coercion(_env, _model, aa.v(), aa.v()->type())());
    }
    if (aa.v()->type().dim() != aa.idx().size()) {
      std::ostringstream oss;
      oss << aa.v()->type().dim() << "-dimensional array accessed with " << aa.idx().size()
          << (aa.idx().size() == 1 ? " expression" : " expressions");
      throw TypeError(_env, aa.v()->loc(), oss.str());
    }
    Type tt = aa.v()->type();
    if (tt.enumId() != 0) {
      const std::vector<unsigned int>& arrayEnumIds = _env.getArrayEnum(tt.enumId());
      std::vector<unsigned int> newArrayEnumids;

      for (unsigned int i = 0; i < arrayEnumIds.size() - 1; i++) {
        Expression* aai = aa.idx()[i];
        // Check if index is slice operator, and convert to correct enum type
        if (auto* aai_sl = aai->dynamicCast<SetLit>()) {
          if (IntSetVal* aai_isv = aai_sl->isv()) {
            if (aai_isv->min() == -IntVal::infinity() && aai_isv->max() == IntVal::infinity()) {
              Type aai_sl_t = aai_sl->type();
              aai_sl_t.enumId(arrayEnumIds[i]);
              aai_sl->type(aai_sl_t);
            }
          }
        } else if (auto* aai_bo = aai->dynamicCast<BinOp>()) {
          if (aai_bo->op() == BOT_DOTDOT) {
            Type aai_bo_t = aai_bo->type();
            if (auto* il = aai_bo->lhs()->dynamicCast<IntLit>()) {
              if (il->v() == -IntVal::infinity()) {
                // Expression is ..X, so result gets enum type of X
                aai_bo_t.enumId(aai_bo->rhs()->type().enumId());
              }
            } else if (auto* il = aai_bo->rhs()->dynamicCast<IntLit>()) {
              if (il->v() == IntVal::infinity()) {
                // Expression is X.., so result gets enum type of X
                aai_bo_t.enumId(aai_bo->lhs()->type().enumId());
              }
            }
            aai_bo->type(aai_bo_t);
          }
        }
        if (aai->type().isSet()) {
          newArrayEnumids.push_back(arrayEnumIds[i]);
        }

        if (arrayEnumIds[i] != 0) {
          if (aa.idx()[i]->type().enumId() != arrayEnumIds[i]) {
            std::ostringstream oss;
            oss << "array index ";
            if (aa.idx().size() > 1) {
              oss << (i + 1) << " ";
            }
            oss << "must be `" << _env.getEnum(arrayEnumIds[i])->e()->id()->str() << "', but is `"
                << aa.idx()[i]->type().toString(_env) << "'";
            throw TypeError(_env, aa.loc(), oss.str());
          }
        }
      }
      if (newArrayEnumids.empty()) {
        tt.enumId(arrayEnumIds[arrayEnumIds.size() - 1]);
      } else {
        newArrayEnumids.push_back(arrayEnumIds[arrayEnumIds.size() - 1]);
        int newEnumId = _env.registerArrayEnum(newArrayEnumids);
        tt.enumId(newEnumId);
      }
    }
    int n_dimensions = 0;
    bool isVarAccess = false;
    bool isSlice = false;
    for (unsigned int i = 0; i < aa.idx().size(); i++) {
      Expression* aai = aa.idx()[i];
      if (aai->isa<AnonVar>()) {
        aai->type(Type::varint());
      }
      if ((aai->type().bt() != Type::BT_INT && aai->type().bt() != Type::BT_BOOL) ||
          aai->type().dim() != 0) {
        throw TypeError(_env, aa.loc(),
                        "array index must be `int' or `set of int', but is `" +
                            aai->type().toString(_env) + "'");
      }
      if (aai->type().isSet()) {
        if (isVarAccess || aai->type().isvar()) {
          throw TypeError(_env, aa.loc(),
                          "array slicing with variable range or index not supported");
        }
        isSlice = true;
        aa.idx()[i] = add_coercion(_env, _model, aai, Type::varsetint())();
        n_dimensions++;
      } else {
        aa.idx()[i] = add_coercion(_env, _model, aai, Type::varint())();
      }

      if (aai->type().isOpt()) {
        tt.ot(Type::OT_OPTIONAL);
      }
      if (aai->type().isvar()) {
        isVarAccess = true;
        if (isSlice) {
          throw TypeError(_env, aa.loc(),
                          "array slicing with variable range or index not supported");
        }
        tt.ti(Type::TI_VAR);
        if (tt.bt() == Type::BT_ANN || tt.bt() == Type::BT_STRING) {
          throw TypeError(_env, aai->loc(),
                          std::string("array access using a variable not supported for array of ") +
                              (tt.bt() == Type::BT_ANN ? "ann" : "string"));
        }
      }
      tt.dim(n_dimensions);
      if (aai->type().cv()) {
        tt.cv(true);
      }
    }
    aa.type(tt);
  }
  /// Visit array comprehension
  void vComprehension(Comprehension& c) {
    Type tt = c.e()->type();
    typedef std::unordered_map<VarDecl*, std::pair<int, int>> genMap_t;
    typedef std::unordered_map<VarDecl*, std::vector<Expression*>> whereMap_t;
    genMap_t generatorMap;
    whereMap_t whereMap;
    int declCount = 0;

    for (int i = 0; i < c.numberOfGenerators(); i++) {
      for (int j = 0; j < c.numberOfDecls(i); j++) {
        generatorMap[c.decl(i, j)] = std::pair<int, int>(i, declCount++);
        whereMap[c.decl(i, j)] = std::vector<Expression*>();
      }
      Expression* g_in = c.in(i);
      if (g_in != nullptr) {
        const Type& ty_in = g_in->type();
        if (ty_in == Type::varsetint()) {
          if (!c.set()) {
            tt.ot(Type::OT_OPTIONAL);
          }
          tt.ti(Type::TI_VAR);
          tt.cv(true);
        }
        if (ty_in.cv()) {
          tt.cv(true);
        }
        if (c.where(i) != nullptr) {
          if (c.where(i)->type() == Type::varbool()) {
            if (!c.set()) {
              tt.ot(Type::OT_OPTIONAL);
            }
            tt.ti(Type::TI_VAR);
            tt.cv(true);
          } else if (c.where(i)->type() != Type::parbool()) {
            throw TypeError(
                _env, c.where(i)->loc(),
                "where clause must be bool, but is `" + c.where(i)->type().toString(_env) + "'");
          }
          if (c.where(i)->type().cv()) {
            tt.cv(true);
          }

          // Try to move parts of the where clause to earlier generators
          std::vector<Expression*> wherePartsStack;
          std::vector<Expression*> whereParts;
          wherePartsStack.push_back(c.where(i));
          while (!wherePartsStack.empty()) {
            Expression* e = wherePartsStack.back();
            wherePartsStack.pop_back();
            if (auto* bo = e->dynamicCast<BinOp>()) {
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
              int declIndex;
              VarDecl* decl;
              const genMap_t& generatorMap;
              Comprehension* comp;
              FindLatestGen(const genMap_t& generatorMap0, Comprehension* comp0)
                  : declIndex(-1),
                    decl(comp0->decl(0, 0)),
                    generatorMap(generatorMap0),
                    comp(comp0) {}
              void vId(const Id& ident) {
                auto it = generatorMap.find(ident.decl());
                if (it != generatorMap.end() && it->second.second > declIndex) {
                  declIndex = it->second.second;
                  decl = ident.decl();
                  int gen = it->second.first;
                  while (comp->in(gen) == nullptr && gen < comp->numberOfGenerators() - 1) {
                    declIndex++;
                    gen++;
                    decl = comp->decl(gen, 0);
                  }
                }
              }
            } flg(generatorMap, &c);
            top_down(flg, wp);
            whereMap[flg.decl].push_back(wp);
          }
        }
      } else {
        assert(c.where(i) != nullptr);
        whereMap[c.decl(i, 0)].push_back(c.where(i));
      }
    }

    {
      GCLock lock;
      Generators generators;
      for (int i = 0; i < c.numberOfGenerators(); i++) {
        std::vector<VarDecl*> decls;
        for (int j = 0; j < c.numberOfDecls(i); j++) {
          decls.push_back(c.decl(i, j));
          KeepAlive c_in =
              c.in(i) != nullptr ? add_coercion(_env, _model, c.in(i), c.in(i)->type()) : nullptr;
          if (!whereMap[c.decl(i, j)].empty()) {
            // need a generator for all the decls up to this point
            Expression* whereExpr = whereMap[c.decl(i, j)][0];
            for (unsigned int k = 1; k < whereMap[c.decl(i, j)].size(); k++) {
              GCLock lock;
              auto* bo =
                  new BinOp(Location().introduce(), whereExpr, BOT_AND, whereMap[c.decl(i, j)][k]);
              Type bo_t = whereMap[c.decl(i, j)][k]->type().isPar() && whereExpr->type().isPar()
                              ? Type::parbool()
                              : Type::varbool();
              if (whereMap[c.decl(i, j)][k]->type().cv() || whereExpr->type().cv()) {
                bo_t.cv(true);
              }
              bo->type(bo_t);
              whereExpr = bo;
            }
            generators.g.emplace_back(decls, c_in(), whereExpr);
            decls.clear();
          } else if (j == c.numberOfDecls(i) - 1) {
            generators.g.emplace_back(decls, c_in(), nullptr);
            decls.clear();
          }
        }
      }
      c.init(c.e(), generators);
    }

    if (c.set()) {
      if (c.e()->type().dim() != 0 || c.e()->type().st() == Type::ST_SET) {
        throw TypeError(_env, c.e()->loc(),
                        "set comprehension expression must be scalar, but is `" +
                            c.e()->type().toString(_env) + "'");
      }
      tt.st(Type::ST_SET);
      if (tt.isvar()) {
        c.e(add_coercion(_env, _model, c.e(), Type::varint())());
        tt.bt(Type::BT_INT);
      }
    } else {
      if (c.e()->type().dim() != 0) {
        throw TypeError(_env, c.e()->loc(), "array comprehension expression cannot be an array");
      }
      tt.dim(1);
      if (tt.enumId() != 0) {
        std::vector<unsigned int> enumIds(2);
        enumIds[0] = 0;
        enumIds[1] = tt.enumId();
        tt.enumId(_env.registerArrayEnum(enumIds));
      }
    }
    c.type(tt);
  }
  /// Visit array comprehension generator
  void vComprehensionGenerator(Comprehension& c, int gen_i) {
    Expression* g_in = c.in(gen_i);
    if (g_in == nullptr) {
      // This is an "assignment generator" (i = expr)
      assert(c.where(gen_i) != nullptr);
      assert(c.numberOfDecls(gen_i) == 1);
      const Type& ty_where = c.where(gen_i)->type();
      c.decl(gen_i, 0)->type(ty_where);
      c.decl(gen_i, 0)->ti()->type(ty_where);
    } else {
      const Type& ty_in = g_in->type();
      if (ty_in != Type::varsetint() && ty_in != Type::parsetint() && ty_in.dim() != 1) {
        throw TypeError(_env, g_in->loc(),
                        "generator expression must be (par or var) set of int or one-dimensional "
                        "array, but is `" +
                            ty_in.toString(_env) + "'");
      }
      Type ty_id;
      if (ty_in.dim() == 0) {
        ty_id = Type::parint();
        ty_id.enumId(ty_in.enumId());
      } else {
        ty_id = ty_in;
        if (ty_in.enumId() != 0) {
          const std::vector<unsigned int>& enumIds = _env.getArrayEnum(ty_in.enumId());
          ty_id.enumId(enumIds.back());
        }
        ty_id.dim(0);
      }
      for (int j = 0; j < c.numberOfDecls(gen_i); j++) {
        c.decl(gen_i, j)->type(ty_id);
        c.decl(gen_i, j)->ti()->type(ty_id);
      }
    }
  }
  /// Visit if-then-else
  void vITE(ITE& ite) {
    bool mustBeBool = false;
    if (ite.elseExpr() == nullptr) {
      // this is an "if <cond> then <expr> endif" so the <expr> must be bool
      ite.elseExpr(constants().boollit(true));
      mustBeBool = true;
    }
    Type tret = ite.elseExpr()->type();
    std::vector<AnonVar*> anons;
    bool allpar = !(tret.isvar());
    if (tret.isunknown()) {
      if (auto* av = ite.elseExpr()->dynamicCast<AnonVar>()) {
        allpar = false;
        anons.push_back(av);
      } else {
        throw TypeError(_env, ite.elseExpr()->loc(),
                        "cannot infer type of expression in `else' branch of conditional");
      }
    }
    bool allpresent = !(tret.isOpt());
    bool varcond = false;
    for (int i = 0; i < ite.size(); i++) {
      Expression* eif = ite.ifExpr(i);
      Expression* ethen = ite.thenExpr(i);
      varcond = varcond || (eif->type() == Type::varbool());
      if (eif->type() != Type::parbool() && eif->type() != Type::varbool()) {
        throw TypeError(
            _env, eif->loc(),
            "expected bool conditional expression, got `" + eif->type().toString(_env) + "'");
      }
      if (eif->type().cv()) {
        tret.cv(true);
      }
      if (ethen->type().isunknown()) {
        if (auto* av = ethen->dynamicCast<AnonVar>()) {
          allpar = false;
          anons.push_back(av);
        } else {
          throw TypeError(_env, ethen->loc(),
                          "cannot infer type of expression in `then' branch of conditional");
        }
      } else {
        if (tret.isbot() || tret.isunknown()) {
          tret.bt(ethen->type().bt());
        }
        if (mustBeBool &&
            (ethen->type().bt() != Type::BT_BOOL || ethen->type().dim() > 0 ||
             ethen->type().st() != Type::ST_PLAIN || ethen->type().ot() != Type::OT_PRESENT)) {
          throw TypeError(_env, ite.loc(),
                          std::string("conditional without `else' branch must have bool type, ") +
                              "but `then' branch has type `" + ethen->type().toString(_env) + "'");
        }
        if ((!ethen->type().isbot() && !Type::btSubtype(ethen->type(), tret, true) &&
             !Type::btSubtype(tret, ethen->type(), true)) ||
            ethen->type().st() != tret.st() || ethen->type().dim() != tret.dim()) {
          throw TypeError(_env, ethen->loc(),
                          "type mismatch in branches of conditional. `then' branch has type `" +
                              ethen->type().toString(_env) + "', but `else' branch has type `" +
                              tret.toString(_env) + "'");
        }
        if (Type::btSubtype(tret, ethen->type(), true)) {
          tret.bt(ethen->type().bt());
        }
        if (tret.enumId() != 0 && ethen->type().enumId() == 0) {
          tret.enumId(0);
        }
        if (ethen->type().isvar()) {
          allpar = false;
        }
        if (ethen->type().isOpt()) {
          allpresent = false;
        }
        if (ethen->type().cv()) {
          tret.cv(true);
        }
      }
    }
    Type tret_var(tret);
    tret_var.ti(Type::TI_VAR);
    for (auto& anon : anons) {
      anon->type(tret_var);
    }
    for (int i = 0; i < ite.size(); i++) {
      ite.thenExpr(i, add_coercion(_env, _model, ite.thenExpr(i), tret)());
    }
    ite.elseExpr(add_coercion(_env, _model, ite.elseExpr(), tret)());
    /// TODO: perhaps extend flattener to array types, but for now throw an error
    if (varcond && tret.dim() > 0) {
      throw TypeError(_env, ite.loc(), "conditional with var condition cannot have array type");
    }
    if (varcond || !allpar) {
      tret.ti(Type::TI_VAR);
    }
    if (!allpresent) {
      tret.ot(Type::OT_OPTIONAL);
    }
    ite.type(tret);
  }
  /// Visit binary operator
  void vBinOp(BinOp& bop) {
    std::vector<Expression*> args(2);
    args[0] = bop.lhs();
    args[1] = bop.rhs();
    if (FunctionI* fi = _model->matchFn(_env, bop.opToString(), args, true)) {
      bop.lhs(add_coercion(_env, _model, bop.lhs(), fi->argtype(_env, args, 0))());
      bop.rhs(add_coercion(_env, _model, bop.rhs(), fi->argtype(_env, args, 1))());
      args[0] = bop.lhs();
      args[1] = bop.rhs();
      Type ty = fi->rtype(_env, args, true);
      ty.cv(bop.lhs()->type().cv() || bop.rhs()->type().cv() || ty.cv());
      bop.type(ty);

      if (fi->e() != nullptr) {
        bop.decl(fi);
      } else {
        bop.decl(nullptr);
      }

      if (bop.lhs()->type().isint() && bop.rhs()->type().isint() &&
          (bop.op() == BOT_EQ || bop.op() == BOT_GQ || bop.op() == BOT_GR || bop.op() == BOT_NQ ||
           bop.op() == BOT_LE || bop.op() == BOT_LQ)) {
        Call* call = bop.lhs()->dynamicCast<Call>();
        Expression* rhs = bop.rhs();
        BinOpType bot = bop.op();
        if (call == nullptr) {
          call = bop.rhs()->dynamicCast<Call>();
          rhs = bop.lhs();
          switch (bop.op()) {
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
        if ((call != nullptr) && (call->id() == "count" || call->id() == "sum") &&
            call->type().isvar()) {
          if (call->argCount() == 1 && call->arg(0)->isa<Comprehension>()) {
            auto* comp = call->arg(0)->cast<Comprehension>();
            auto* inner_bo = comp->e()->dynamicCast<BinOp>();
            if (inner_bo != nullptr) {
              if (inner_bo->op() == BOT_EQ && inner_bo->lhs()->type().isint()) {
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
                  ct.bt(generated->type().bt());
                  comp->type(ct);

                  std::vector<Expression*> args({comp, comparedTo, rhs});
                  FunctionI* newCall_decl = _model->matchFn(_env, cid, args, true);
                  if (newCall_decl == nullptr) {
                    std::ostringstream ss;
                    ss << "could not replace binary operator by call to " << cid;
                    throw InternalError(ss.str());
                  }
                  Call* newCall = bop.morph(cid, args);
                  newCall->decl(newCall_decl);
                }
              }
            }
          } else if (call->argCount() == 2 && call->arg(0)->type().isIntArray() &&
                     call->arg(1)->type().isint()) {
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
            Call* newCall = bop.morph(cid, args);
            newCall->decl(newCall_decl);
          }
        }
      }
    } else {
      std::ostringstream ss;
      ss << "type error in operator application for `" << bop.opToString()
         << "'. No matching operator found with left-hand side type `"
         << bop.lhs()->type().toString(_env) << "' and right-hand side type `"
         << bop.rhs()->type().toString(_env) << "'";
      throw TypeError(_env, bop.loc(), ss.str());
    }
  }
  /// Visit unary operator
  void vUnOp(UnOp& uop) {
    std::vector<Expression*> args(1);
    args[0] = uop.e();
    if (FunctionI* fi = _model->matchFn(_env, uop.opToString(), args, true)) {
      uop.e(add_coercion(_env, _model, uop.e(), fi->argtype(_env, args, 0))());
      args[0] = uop.e();
      Type ty = fi->rtype(_env, args, true);
      ty.cv(uop.e()->type().cv() || ty.cv());
      uop.type(ty);
      if (fi->e() != nullptr) {
        uop.decl(fi);
      }
    } else {
      std::ostringstream ss;
      ss << "type error in operator application for `" << uop.opToString()
         << "'. No matching operator found with type `" << uop.e()->type().toString(_env) << "'";
      throw TypeError(_env, uop.loc(), ss.str());
    }
  }

  /// Visit call
  void vCall(Call& call) {
    std::vector<Expression*> args(call.argCount());
    for (auto i = static_cast<unsigned int>(args.size()); (i--) != 0U;) {
      args[i] = call.arg(i);
    }
    FunctionI* fi = _model->matchFn(_env, &call, true, true);

    if (fi != nullptr && fi->id() == "symmetry_breaking_constraint" && fi->params().size() == 1 &&
        fi->params()[0]->type().isbool()) {
      GCLock lock;
      call.id(ASTString("mzn_symmetry_breaking_constraint"));
      fi = _model->matchFn(_env, &call, true, true);
    } else if (fi != nullptr && fi->id() == "redundant_constraint" && fi->params().size() == 1 &&
               fi->params()[0]->type().isbool()) {
      GCLock lock;
      call.id(ASTString("mzn_redundant_constraint"));
      fi = _model->matchFn(_env, &call, true, true);
    }

    if ((fi->e() != nullptr) && fi->e()->isa<Call>()) {
      Call* next_call = fi->e()->cast<Call>();
      if ((next_call->decl() != nullptr) && next_call->argCount() == fi->params().size() &&
          _model->sameOverloading(_env, args, fi, next_call->decl())) {
        bool macro = true;
        for (unsigned int i = 0; i < fi->params().size(); i++) {
          if (!Expression::equal(next_call->arg(i), fi->params()[i]->id())) {
            macro = false;
            break;
          }
        }
        if (macro) {
          // Call is not a macro if it has a reification implementation
          GCLock lock;
          ASTString reif_id = _env.reifyId(fi->id());
          std::vector<Type> tt(fi->params().size() + 1);
          for (unsigned int i = 0; i < fi->params().size(); i++) {
            tt[i] = fi->params()[i]->type();
          }
          tt[fi->params().size()] = Type::varbool();

          macro = _model->matchFn(_env, reif_id, tt, true) == nullptr;
        }
        if (macro) {
          call.decl(next_call->decl());
          for (ExpressionSetIter esi = next_call->ann().begin(); esi != next_call->ann().end();
               ++esi) {
            call.addAnnotation(*esi);
          }
          call.rehash();
          fi = next_call->decl();
        }
      }
    }

    bool cv = false;
    for (unsigned int i = 0; i < args.size(); i++) {
      if (auto* c = call.arg(i)->dynamicCast<Comprehension>()) {
        Type t_before = c->e()->type();
        Type t = fi->argtype(_env, args, i);
        t.dim(0);
        c->e(add_coercion(_env, _model, c->e(), t)());
        Type t_after = c->e()->type();
        if (t_before != t_after) {
          Type ct = c->type();
          ct.bt(t_after.bt());
          c->type(ct);
        }
      } else {
        args[i] = add_coercion(_env, _model, call.arg(i), fi->argtype(_env, args, i))();
        call.arg(i, args[i]);
      }
      cv = cv || args[i]->type().cv();
    }
    // Replace par enums with their string versions
    if (call.id() == "format" || call.id() == "show" || call.id() == "showDzn" ||
        call.id() == "showJSON") {
      if (call.arg(call.argCount() - 1)->type().isPar()) {
        unsigned int enumId = call.arg(call.argCount() - 1)->type().enumId();
        if (enumId != 0U && call.arg(call.argCount() - 1)->type().dim() != 0) {
          const std::vector<unsigned int>& enumIds = _env.getArrayEnum(enumId);
          enumId = enumIds[enumIds.size() - 1];
        }
        if (enumId > 0) {
          VarDecl* enumDecl = _env.getEnum(enumId)->e();
          if (enumDecl->e() != nullptr) {
            Id* ti_id = _env.getEnum(enumId)->e()->id();
            GCLock lock;
            std::vector<Expression*> args(3);
            args[0] = call.arg(call.argCount() - 1);
            if (args[0]->type().dim() > 1) {
              std::vector<Expression*> a1dargs(1);
              a1dargs[0] = args[0];
              Call* array1d = new Call(Location().introduce(), ASTString("array1d"), a1dargs);
              Type array1dt = args[0]->type();
              array1dt.dim(1);
              array1d->type(array1dt);
              args[0] = array1d;
            }
            args[1] = constants().boollit(call.id() == "showDzn");
            args[2] = constants().boollit(call.id() == "showJSON");
            ASTString enumName(create_enum_to_string_name(ti_id, "_toString_"));
            call.id(enumName);
            call.args(args);
            if (call.id() == "showDzn") {
              call.id(constants().ids.show);
            }
            fi = _model->matchFn(_env, &call, false, true);
          }
        }
      }
    }

    // Set type and decl
    Type ty = fi->rtype(_env, args, true);
    ty.cv(cv || ty.cv());
    call.type(ty);

    if (Call* deprecated = fi->ann().getCall(constants().ann.mzn_deprecated)) {
      // rewrite this call into a call to mzn_deprecate(..., e)
      GCLock lock;
      std::vector<Expression*> params(call.argCount());
      for (unsigned int i = 0; i < params.size(); i++) {
        params[i] = call.arg(i);
      }
      Call* origCall = new Call(call.loc(), call.id(), params);
      origCall->type(ty);
      origCall->decl(fi);
      call.id(constants().ids.mzn_deprecate);
      std::vector<Expression*> args(
          {new StringLit(Location(), fi->id()), deprecated->arg(0), deprecated->arg(1), origCall});
      call.args(args);
      FunctionI* deprecated_fi = _model->matchFn(_env, &call, false, true);
      call.decl(deprecated_fi);
    } else {
      call.decl(fi);
    }
  }
  /// Visit let
  void vLet(Let& let) {
    bool cv = false;
    bool isVar = false;
    for (unsigned int i = 0, j = 0; i < let.let().size(); i++) {
      Expression* li = let.let()[i];
      cv = cv || li->type().cv();
      if (auto* vdi = li->dynamicCast<VarDecl>()) {
        if (vdi->e() == nullptr && vdi->type().isSet() && vdi->type().isvar() &&
            vdi->ti()->domain() == nullptr) {
          std::ostringstream ss;
          ss << "set element type for `" << vdi->id()->str() << "' is not finite";
          _typeErrors.emplace_back(_env, vdi->loc(), ss.str());
        }
        if (vdi->type().isPar() && (vdi->e() == nullptr)) {
          std::ostringstream ss;
          ss << "let variable `" << vdi->id()->v() << "' must be initialised";
          throw TypeError(_env, vdi->loc(), ss.str());
        }
        if (vdi->ti()->hasTiVariable()) {
          std::ostringstream ss;
          ss << "type-inst variables not allowed in type-inst for let variable `"
             << vdi->id()->str() << "'";
          _typeErrors.emplace_back(_env, vdi->loc(), ss.str());
        }
        let.letOrig()[j++] = vdi->e();
        for (unsigned int k = 0; k < vdi->ti()->ranges().size(); k++) {
          let.letOrig()[j++] = vdi->ti()->ranges()[k]->domain();
        }
      }
      isVar |= li->type().isvar();
    }
    Type ty = let.in()->type();
    ty.cv(cv || ty.cv());
    if (isVar && ty.bt() == Type::BT_BOOL && ty.dim() == 0) {
      ty.ti(Type::TI_VAR);
    }
    let.type(ty);
  }
  /// Visit variable declaration
  void vVarDecl(VarDecl& vd) {
    if (ignoreVarDecl) {
      if (vd.e() != nullptr) {
        Type vdt = vd.ti()->type();
        Type vet = vd.e()->type();
        if (vdt.enumId() != 0 && vdt.dim() > 0 &&
            (vd.e()->isa<ArrayLit>() || vd.e()->isa<Comprehension>() ||
             (vd.e()->isa<BinOp>() && vd.e()->cast<BinOp>()->op() == BOT_PLUSPLUS))) {
          // Special case: index sets of array literals and comprehensions automatically
          // coerce to any enum index set
          const std::vector<unsigned int>& enumIds = _env.getArrayEnum(vdt.enumId());
          if (enumIds[enumIds.size() - 1] == 0) {
            vdt.enumId(0);
          } else {
            std::vector<unsigned int> nEnumIds(enumIds.size());
            for (unsigned int i = 0; i < nEnumIds.size() - 1; i++) {
              nEnumIds[i] = 0;
            }
            nEnumIds[nEnumIds.size() - 1] = enumIds[enumIds.size() - 1];
            vdt.enumId(_env.registerArrayEnum(nEnumIds));
          }
        } else if (vd.ti()->isEnum() && vd.e()->isa<Call>()) {
          if (vd.e()->cast<Call>()->id() == "anon_enum") {
            vet.enumId(vdt.enumId());
          }
        }

        if (vd.type().isunknown()) {
          vd.ti()->type(vet);
          vd.type(vet);
        } else if (!_env.isSubtype(vet, vdt, true)) {
          if (vet == Type::bot(1) && vd.e()->isa<ArrayLit>() &&
              vd.e()->cast<ArrayLit>()->size() == 0 &&
              vdt.dim() != 0) {  // NOLINT(bugprone-branch-clone): see TODO in other branch
            // this is okay: assigning an empty array (one-dimensional) to an array variable
          } else if (vd.ti()->isEnum() && vet == Type::parsetint()) {
            // let's ignore this for now (TODO: add an annotation to make sure only
            // compiler-generated ones are accepted)
          } else {
            const Location& loc = vd.e()->loc().isNonAlloc() ? vd.loc() : vd.e()->loc();
            std::ostringstream ss;
            ss << "initialisation value for `" << vd.id()->str()
               << "' has invalid type-inst: expected `" << vd.ti()->type().toString(_env)
               << "', actual `" << vd.e()->type().toString(_env) << "'";
            _typeErrors.emplace_back(_env, loc, ss.str());
          }
        } else {
          vd.e(add_coercion(_env, _model, vd.e(), vd.ti()->type())());
        }
      } else {
        assert(!vd.type().isunknown());
      }
    } else {
      vd.type(vd.ti()->type());
      vd.id()->type(vd.type());
    }
  }
  /// Visit type inst
  void vTypeInst(TypeInst& ti) {
    Type tt = ti.type();
    bool foundEnum =
        ti.ranges().size() > 0 && (ti.domain() != nullptr) && ti.domain()->type().enumId() != 0;
    if (ti.ranges().size() > 0) {
      bool foundTIId = false;
      for (unsigned int i = 0; i < ti.ranges().size(); i++) {
        TypeInst* ri = ti.ranges()[i];
        assert(ri != nullptr);
        if (ri->type().cv()) {
          tt.cv(true);
        }
        if (ri->type().enumId() != 0) {
          foundEnum = true;
        }
        if (ri->type() == Type::top()) {
          //            if (foundTIId) {
          //              throw TypeError(_env,ri->loc(),
          //                "only one type-inst variable allowed in array index");
          //            } else {
          foundTIId = true;
          //            }
        } else if (ri->type() != Type::parint()) {
          assert(ri->isa<TypeInst>());
          auto* riti = ri->cast<TypeInst>();
          if (riti->domain() != nullptr) {
            throw TypeError(_env, ri->loc(),
                            "array index set expression has invalid type, expected `set of int', "
                            "actual `set of " +
                                ri->type().toString(_env) + "'");
          }
          throw TypeError(_env, ri->loc(),
                          "cannot use `" + ri->type().toString(_env) +
                              "' as array index set (did you mean `int'?)");
        }
      }
      tt.dim(foundTIId ? -1 : static_cast<int>(ti.ranges().size()));
    }
    if ((ti.domain() != nullptr) && ti.domain()->type().cv()) {
      tt.cv(true);
    }
    if (ti.domain() != nullptr) {
      if (TIId* tiid = ti.domain()->dynamicCast<TIId>()) {
        if (tiid->isEnum()) {
          tt.bt(Type::BT_INT);
        }
      } else {
        if (ti.domain()->type().ti() != Type::TI_PAR || ti.domain()->type().st() != Type::ST_SET) {
          throw TypeError(
              _env, ti.domain()->loc().isNonAlloc() ? ti.loc() : ti.domain()->loc(),
              "type-inst must be par set but is `" + ti.domain()->type().toString(_env) + "'");
        }
        if (ti.domain()->type().dim() != 0) {
          throw TypeError(_env, ti.domain()->loc(), "type-inst cannot be an array");
        }
      }
    }
    if (tt.isunknown() && (ti.domain() != nullptr)) {
      assert(ti.domain());
      switch (ti.domain()->type().bt()) {
        case Type::BT_INT:
        case Type::BT_FLOAT:
          break;
        case Type::BT_BOT: {
          Type tidt = ti.domain()->type();
          tidt.bt(Type::BT_INT);
          ti.domain()->type(tidt);
        } break;
        default:
          throw TypeError(_env, ti.domain()->loc(), "type-inst must be int or float");
      }
      tt.bt(ti.domain()->type().bt());
      tt.enumId(ti.domain()->type().enumId());
    } else {
      //        assert(ti.domain()==NULL || ti.domain()->isa<TIId>());
    }
    if (foundEnum) {
      std::vector<unsigned int> enumIds(ti.ranges().size() + 1);
      for (unsigned int i = 0; i < ti.ranges().size(); i++) {
        enumIds[i] = ti.ranges()[i]->type().enumId();
      }
      enumIds[ti.ranges().size()] = ti.domain() != nullptr ? ti.domain()->type().enumId() : 0;
      int arrayEnumId = _env.registerArrayEnum(enumIds);
      tt.enumId(arrayEnumId);
    }

    if (tt.st() == Type::ST_SET && tt.ti() == Type::TI_VAR && tt.bt() != Type::BT_INT &&
        tt.bt() != Type::BT_TOP) {
      throw TypeError(_env, ti.loc(), "var set element types other than `int' not allowed");
    }
    ti.type(tt);
  }
  void vTIId(TIId& id) {}
};

void typecheck(Env& env, Model* origModel, std::vector<TypeError>& typeErrors,
               bool ignoreUndefinedParameters, bool allowMultiAssignment, bool isFlatZinc) {
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
  TopoSorter ts(m);

  std::vector<FunctionI*> functionItems;
  std::vector<AssignI*> assignItems;
  auto* enumItems = new Model;

  class TSVFuns : public ItemVisitor {
  public:
    EnvI& env;
    Model* model;
    std::vector<FunctionI*>& fis;
    TSVFuns(EnvI& env0, Model* model0, std::vector<FunctionI*>& fis0)
        : env(env0), model(model0), fis(fis0) {}
    void vFunctionI(FunctionI* i) {
      (void)model->registerFn(env, i);
      fis.push_back(i);
    }
  } _tsvf(env.envi(), m, functionItems);
  iter_items(_tsvf, m);

  class TSV0 : public ItemVisitor {
  public:
    EnvI& env;
    TopoSorter& ts;
    Model* model;
    bool hadSolveItem;
    std::vector<AssignI*>& ais;
    VarDeclI* objective;
    Model* enumis;
    bool isFlatZinc;
    TSV0(EnvI& env0, TopoSorter& ts0, Model* model0, std::vector<AssignI*>& ais0, Model* enumis0,
         bool isFlatZinc0)
        : env(env0),
          ts(ts0),
          model(model0),
          hadSolveItem(false),
          ais(ais0),
          objective(nullptr),
          enumis(enumis0),
          isFlatZinc(isFlatZinc0) {}
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
        throw TypeError(env, si->loc(), "Only one solve item allowed");
      }
      hadSolveItem = true;
      if (!isFlatZinc && (si->e() != nullptr)) {
        GCLock lock;
        auto* ti = new TypeInst(Location().introduce(), Type());
        auto* obj = new VarDecl(Location().introduce(), ti, "_objective", si->e());
        si->e(obj->id());
        objective = new VarDeclI(Location().introduce(), obj);
      }
    }
  } _tsv0(env.envi(), ts, m, assignItems, enumItems, isFlatZinc);
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
    } else {
      auto* fi = (*enumItems)[i]->dynamicCast<FunctionI>();
      m->addItem(fi);
      (void)m->registerFn(env.envi(), fi);
      functionItems.push_back(fi);
    }
  }

  auto* enumItems2 = new Model;

  for (auto* ai : assignItems) {
    VarDecl* vd = nullptr;
    if (env.envi().ignoreUnknownIds) {
      try {
        vd = ts.get(env.envi(), ai->id(), ai->loc());
      } catch (TypeError&) {
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
          throw TypeError(env.envi(), ai->loc(), "multiple assignment to the same variable");
        }
      } else {
        vd->e(ai->e());
        vd->ann().add(constants().ann.rhs_from_assignment);
        if (vd->ti()->isEnum()) {
          create_enum_mapper(env.envi(), m, vd->ti()->type().enumId(), vd, enumItems2);
        }
      }
    }
    ai->remove();
  }

  for (auto& i : *enumItems2) {
    if (auto* vdi = i->dynamicCast<VarDeclI>()) {
      m->addItem(vdi);
      ts.add(env.envi(), vdi, false, enumItems);
    } else {
      auto* fi = i->cast<FunctionI>();
      m->addItem(fi);
      (void)m->registerFn(env.envi(), fi);
      functionItems.push_back(fi);
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
    void vOutputI(OutputI* i) { ts.run(env, i->e()); }
    void vFunctionI(FunctionI* fi) {
      ts.run(env, fi->ti());
      for (unsigned int i = 0; i < fi->params().size(); i++) {
        ts.run(env, fi->params()[i]);
      }
      for (ExpressionSetIter it = fi->ann().begin(); it != fi->ann().end(); ++it) {
        ts.run(env, *it);
      }
      ts.scopes.pushFun();
      for (unsigned int i = 0; i < fi->params().size(); i++) {
        ts.scopes.add(env, fi->params()[i]);
      }
      ts.run(env, fi->e());
      ts.scopes.pop();
    }
  } _tsv1(env.envi(), ts);
  iter_items(_tsv1, m);

  m->sortFn();

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

  {
    Typer<false> ty(env.envi(), m, typeErrors, ignoreUndefinedParameters);
    BottomUpIterator<Typer<false>> bottomUpTyper(ty);
    for (auto& decl : ts.decls) {
      decl->payload(0);
      bottomUpTyper.run(decl->ti());
      ty.vVarDecl(*decl);
    }
    for (auto& functionItem : functionItems) {
      bottomUpTyper.run(functionItem->ti());
      for (unsigned int j = 0; j < functionItem->params().size(); j++) {
        bottomUpTyper.run(functionItem->params()[j]);
      }
    }
  }

  m->fixFnMap();

  {
    Typer<true> ty(env.envi(), m, typeErrors, ignoreUndefinedParameters);
    BottomUpIterator<Typer<true>> bottomUpTyper(ty);

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
        if (i->e()->ti()->hasTiVariable()) {
          std::ostringstream ss;
          ss << "type-inst variables not allowed in type-inst for `" << i->e()->id()->str() << "'";
          _typeErrors.emplace_back(_env, i->e()->loc(), ss.str());
        }
        VarDecl* vdi = i->e();
        if (vdi->e() == nullptr && vdi->type().isSet() && vdi->type().isvar() &&
            vdi->ti()->domain() == nullptr) {
          std::ostringstream ss;
          ss << "set element type for `" << vdi->id()->str() << "' is not finite";
          _typeErrors.emplace_back(_env, vdi->loc(), ss.str());
        }
        if (i->e()->ann().contains(constants().ann.output_only)) {
          if (vdi->e() == nullptr) {
            _typeErrors.emplace_back(
                _env, vdi->loc(),
                "variables annotated with ::output_only must have a right hand side");
          } else if (vdi->e()->type().isvar()) {
            _typeErrors.emplace_back(_env, vdi->loc(),
                                     "variables annotated with ::output_only must be par");
          }
        }
      }
      void vAssignI(AssignI* i) {
        _bottomUpTyper.run(i->e());
        if (!_env.isSubtype(i->e()->type(), i->decl()->ti()->type(), true)) {
          std::ostringstream ss;
          ss << "assignment value for `" << i->decl()->id()->str()
             << "' has invalid type-inst: expected `" << i->decl()->ti()->type().toString(_env)
             << "', actual `" << i->e()->type().toString(_env) << "'";
          _typeErrors.emplace_back(_env, i->loc(), ss.str());
          // Assign to "true" constant to avoid generating further errors that the parameter
          // is undefined
          i->decl()->e(constants().literalTrue);
        }
      }
      void vConstraintI(ConstraintI* i) {
        _bottomUpTyper.run(i->e());
        if (!_env.isSubtype(i->e()->type(), Type::varbool(), true)) {
          throw TypeError(_env, i->loc(),
                          "invalid type of constraint, expected `" +
                              Type::varbool().toString(_env) + "', actual `" +
                              i->e()->type().toString(_env) + "'");
        }
      }
      void vSolveI(SolveI* i) {
        for (ExpressionSetIter it = i->ann().begin(); it != i->ann().end(); ++it) {
          _bottomUpTyper.run(*it);
          if (!(*it)->type().isAnn()) {
            throw TypeError(_env, (*it)->loc(),
                            "expected annotation, got `" + (*it)->type().toString(_env) + "'");
          }
        }
        _bottomUpTyper.run(i->e());
        if (i->e() != nullptr) {
          Type et = i->e()->type();
          if (et.isbool()) {
            Type target_t = Type::varint();
            if (et.isOpt()) {
              target_t.ot(Type::OT_OPTIONAL);
            }
            i->e(add_coercion(_env, _env.model, i->e(), target_t)());
          }

          bool needOptCoercion = et.isOpt() && et.isint();
          if (needOptCoercion) {
            et.ot(Type::OT_PRESENT);
          }

          if (!(_env.isSubtype(et, Type::varint(), true) ||
                _env.isSubtype(et, Type::varfloat(), true))) {
            throw TypeError(_env, i->e()->loc(),
                            "objective has invalid type, expected int or float, actual `" +
                                et.toString(_env) + "'");
          }

          if (needOptCoercion) {
            GCLock lock;
            std::vector<Expression*> args(2);
            args[0] = i->e();
            args[1] = constants().boollit(i->st() == SolveI::ST_MAX);
            Call* c = new Call(Location().introduce(), ASTString("objective_deopt_"), args);
            c->decl(_env.model->matchFn(_env, c, false));
            assert(c->decl());
            c->type(et);
            i->e(c);
          }
        }
      }
      void vOutputI(OutputI* i) {
        _bottomUpTyper.run(i->e());
        if (i->e()->type() != Type::parstring(1) && i->e()->type() != Type::bot(1)) {
          throw TypeError(_env, i->e()->loc(),
                          "invalid type in output item, expected `" +
                              Type::parstring(1).toString(_env) + "', actual `" +
                              i->e()->type().toString(_env) + "'");
        }
      }
      void vFunctionI(FunctionI* i) {
        for (ExpressionSetIter it = i->ann().begin(); it != i->ann().end(); ++it) {
          _bottomUpTyper.run(*it);
          if (!(*it)->type().isAnn()) {
            throw TypeError(_env, (*it)->loc(),
                            "expected annotation, got `" + (*it)->type().toString(_env) + "'");
          }
        }
        _bottomUpTyper.run(i->ti());
        _bottomUpTyper.run(i->e());
        if ((i->e() != nullptr) && !_env.isSubtype(i->e()->type(), i->ti()->type(), true)) {
          throw TypeError(_env, i->e()->loc(),
                          "return type of function does not match body, declared type is `" +
                              i->ti()->type().toString(_env) + "', body type is `" +
                              i->e()->type().toString(_env) + "'");
        }
        if ((i->e() != nullptr) && i->e()->type().isPar() && i->ti()->type().isvar()) {
          // this is a par function declared as var, so change declared return type
          Type i_t = i->ti()->type();
          i_t.ti(Type::TI_PAR);
          i->ti()->type(i_t);
        }
        if (i->e() != nullptr) {
          i->e(add_coercion(_env, _m, i->e(), i->ti()->type())());
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
    void vAssignI(AssignI* i) { i->decl()->e(add_coercion(env, m, i->e(), i->decl()->type())()); }
    void vOutputI(OutputI* oi) {
      if (outputItem == nullptr) {
        outputItem = oi;
      } else {
        GCLock lock;
        auto* bo = new BinOp(Location().introduce(), outputItem->e(), BOT_PLUSPLUS, oi->e());
        bo->type(Type::parstring(1));
        outputItem->e(bo);
        oi->remove();
        m->setOutputItem(outputItem);
      }
    }
  } _tsv3(env.envi(), m);
  if (typeErrors.empty()) {
    iter_items(_tsv3, m);
  }

  // Create a par version of each function that returns par and
  // that has a body that can be made par
  std::unordered_map<FunctionI*, std::pair<bool, std::vector<FunctionI*>>> fnsToMakePar;
  for (auto& f : m->functions()) {
    if (f.id() == "mzn_reverse_map_var") {
      continue;
    }
    if (f.e() != nullptr && f.ti()->type().bt() != Type::BT_ANN) {
      bool foundVar = false;
      for (auto* p : f.params()) {
        if (p->type().isvar()) {
          foundVar = true;
          break;
        }
      }
      if (foundVar) {
        // create par version of parameter types
        std::vector<Type> tv;
        for (auto* p : f.params()) {
          Type t = p->type();
          t.cv(false);
          t.ti(Type::TI_PAR);
          tv.push_back(t);
        }
        // check if specialised par version of function already exists
        FunctionI* fi_par = m->matchFn(env.envi(), f.id(), tv, false);
        bool parIsUsable = false;
        if (fi_par != nullptr) {
          bool foundVar = false;
          for (auto* p : fi_par->params()) {
            if (p->type().isvar()) {
              foundVar = true;
              break;
            }
          }
          parIsUsable = !foundVar;
        }
        if (!parIsUsable) {
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
            void vId(const Id& ident) {
              if (ident.decl() != nullptr && ident.type().isvar() && ident.decl()->toplevel()) {
                isPar = false;
              }
            }
            void vLet(const Let& let) {
              // check if any of the declared variables does not have a RHS
              for (auto* e : let.let()) {
                if (auto* vd = e->dynamicCast<VarDecl>()) {
                  if (vd->e() == nullptr) {
                    isPar = false;
                    break;
                  }
                }
              }
            }
            void vCall(const Call& c) {
              if (!c.type().isAnn()) {
                FunctionI* decl = c.decl();
                // create par version of parameter types
                std::vector<Type> tv;
                for (auto* p : decl->params()) {
                  Type t = p->type();
                  t.cv(false);
                  t.ti(Type::TI_PAR);
                  tv.push_back(t);
                }
                // check if specialised par version of function already exists
                FunctionI* decl_par = m->matchFn(env, decl->id(), tv, false);
                bool parIsUsable = decl_par->ti()->type().isPar();
                if (parIsUsable && decl_par->e() == nullptr && decl_par->fromStdLib()) {
                  parIsUsable = true;
                } else if (parIsUsable) {
                  bool foundVar = false;
                  for (auto* p : decl_par->params()) {
                    if (p->type().isvar()) {
                      foundVar = true;
                      break;
                    }
                  }
                  parIsUsable = !foundVar;
                }
                if (!parIsUsable) {
                  deps.push_back(decl_par);
                }
              }
            }
          } cpb(env.envi(), m);
          top_down(cpb, f.e());
          if (cpb.isPar) {
            fnsToMakePar.insert({&f, {false, cpb.deps}});
          }
        } else {
          fnsToMakePar.insert({fi_par, {true, std::vector<FunctionI*>()}});
        }
      }
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
      void vId(Id& ident) {
        if (ident.decl() != nullptr && ident.decl()->toplevel()) {
          cm.insert(ident.decl(), ident.decl());
        }
      }
    } _egd(parCopyMap);
    for (auto& p : fnsToMakePar) {
      if (!p.second.first) {
        for (auto* param : p.first->params()) {
          top_down(_egd, param);
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
        for (auto* v : cp->params()) {
          Type vt = v->ti()->type();
          vt.ti(Type::TI_PAR);
          v->ti()->type(vt);
          v->type(vt);
        }
        Type cpt(cp->ti()->type());
        cpt.ti(Type::TI_PAR);
        cp->ti()->type(cpt);
        bool didRegister = m->registerFn(env.envi(), cp, true, false);
        if (didRegister) {
          m->addItem(cp);
          parFunctions.push_back(cp);
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
      static bool enter(Expression* e) {
        Type t(e->type());
        t.ti(Type::TI_PAR);
        t.cv(false);
        e->type(t);
        return true;
      }
      void vCall(Call& c) {
        FunctionI* decl = m->matchFn(env, &c, false);
        c.decl(decl);
      }
      void vBinOp(BinOp& bo) {
        if (bo.decl() != nullptr) {
          std::vector<Type> ta(2);
          ta[0] = bo.lhs()->type();
          ta[1] = bo.rhs()->type();
          FunctionI* decl = m->matchFn(env, bo.opToString(), ta, false);
          bo.decl(decl);
        }
      }
      void vUnOp(UnOp& uo) {
        if (uo.decl() != nullptr) {
          std::vector<Type> ta(1);
          ta[0] = uo.e()->type();
          FunctionI* decl = m->matchFn(env, uo.opToString(), ta, false);
          uo.decl(decl);
        }
      }
    } _mfp(env.envi(), m);

    for (auto* p : parFunctions) {
      bottom_up(_mfp, p->e());
    }
  }

  try {
    m->checkFnOverloading(env.envi());
  } catch (TypeError& e) {
    typeErrors.push_back(e);
  }

  for (auto& decl : ts.decls) {
    if (decl->toplevel() && decl->type().isPar() && !decl->type().isAnn() && decl->e() == nullptr) {
      if (decl->type().isOpt() && decl->type().dim() == 0) {
        decl->e(constants().absent);
        decl->addAnnotation(constants().ann.mzn_was_undefined);
      } else if (!ignoreUndefinedParameters) {
        std::ostringstream ss;
        ss << "  symbol error: variable `" << decl->id()->str()
           << "' must be defined (did you forget to specify a data file?)";
        typeErrors.emplace_back(env.envi(), decl->loc(), ss.str());
      }
    }
    if (decl->ti()->isEnum()) {
      decl->ti()->setIsEnum(false);
      Type vdt = decl->ti()->type();
      vdt.enumId(0);
      decl->ti()->type(vdt);
    }
  }

  for (auto vd_k : env.envi().checkVars) {
    try {
      VarDecl* vd;
      try {
        vd = ts.get(env.envi(), vd_k()->cast<VarDecl>()->id()->str(),
                    vd_k()->cast<VarDecl>()->loc());
      } catch (TypeError&) {
        if (vd_k()->cast<VarDecl>()->type().isvar()) {
          continue;  // var can be undefined
        }
        throw;
      }
      vd->ann().add(constants().ann.mzn_check_var);
      if (vd->type().enumId() != 0) {
        GCLock lock;
        std::vector<unsigned int> enumIds({vd->type().enumId()});
        if (vd->type().dim() > 0) {
          enumIds = env.envi().getArrayEnum(vd->type().enumId());
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
            new Call(Location().introduce(), constants().ann.mzn_check_enum_var, args);
        checkEnum->type(Type::ann());
        checkEnum->decl(env.envi().model->matchFn(env.envi(), checkEnum, false));
        vd->ann().add(checkEnum);
      }
      Type vdktype = vd_k()->type();
      vdktype.ti(Type::TI_VAR);
      if (!vd_k()->type().isSubtypeOf(vd->type(), false)) {
        std::ostringstream ss;
        ss << "Solution checker requires `" << vd->id()->str() << "' to be of type `"
           << vdktype.toString(env.envi()) << "'";
        typeErrors.emplace_back(env.envi(), vd->loc(), ss.str());
      }
    } catch (TypeError& e) {
      typeErrors.emplace_back(env.envi(), e.loc(),
                              e.msg() + " (required by solution checker model)");
    }
  }
}

void typecheck(Env& env, Model* m, AssignI* ai) {
  std::vector<TypeError> typeErrors;
  Typer<true> ty(env.envi(), m, typeErrors, false);
  BottomUpIterator<Typer<true>> bottomUpTyper(ty);
  bottomUpTyper.run(ai->e());
  if (!typeErrors.empty()) {
    throw typeErrors[0];
  }
  if (!env.envi().isSubtype(ai->e()->type(), ai->decl()->ti()->type(), true)) {
    std::ostringstream ss;
    ss << "assignment value for `" << ai->decl()->id()->str()
       << "' has invalid type-inst: expected `" << ai->decl()->ti()->type().toString(env.envi())
       << "', actual `" << ai->e()->type().toString(env.envi()) << "'";
    throw TypeError(env.envi(), ai->e()->loc(), ss.str());
  }
}

void output_var_desc_json(Env& env, VarDecl* vd, std::ostream& os, bool extra = false) {
  os << "    \"" << *vd->id() << "\" : {";
  os << "\"type\" : ";
  switch (vd->type().bt()) {
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
    default:
      os << "\"?\"";
      break;
  }
  if (vd->type().ot() == Type::OT_OPTIONAL) {
    os << ", \"optional\" : true";
  }
  if (vd->type().st() == Type::ST_SET) {
    os << ", \"set\" : true";
  }
  if (vd->type().dim() > 0) {
    os << ", \"dim\" : " << vd->type().dim();

    if (extra) {
      os << ", \"dims\" : [";
      bool had_dim = false;
      ASTExprVec<TypeInst> ranges = vd->ti()->ranges();
      for (auto& range : ranges) {
        if (range->type().enumId() > 0) {
          os << (had_dim ? "," : "") << "\""
             << *env.envi().getEnum(range->type().enumId())->e()->id() << "\"";
        } else {
          os << (had_dim ? "," : "") << "\"int\"";
        }
        had_dim = true;
      }
      os << "]";

      if (vd->type().enumId() > 0) {
        const std::vector<unsigned int>& enumIds = env.envi().getArrayEnum(vd->type().enumId());
        if (enumIds.back() > 0) {
          os << ", \"enum_type\" : \"" << *env.envi().getEnum(enumIds.back())->e()->id() << "\"";
        }
      }
    }

  } else {
    if (extra) {
      if (vd->type().enumId() > 0) {
        os << ", \"enum_type\" : \"" << *env.envi().getEnum(vd->type().enumId())->e()->id() << "\"";
      }
    }
  }
  os << "}";
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
      } else if (vdi->e()->type().st() == Type::ST_SET && vdi->e()->type().enumId() != 0 &&
                 !vdi->e()->type().isAnn()) {
        if (hadEnum) {
          ossEnums << ", ";
        }
        ossEnums << "\"" << *env.envi().getEnum(vdi->e()->type().enumId())->e()->id() << "\"";
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

void output_model_interface(Env& env, Model* m, std::ostream& os,
                            const std::vector<std::string>& skipDirs) {
  class IfcVisitor : public ItemVisitor {
  public:
    Env& env;
    const std::vector<std::string>& skipDirs;
    bool hadInput;
    bool hadOutput;
    bool hadIncludedFiles;
    bool hadAddToOutput = false;
    std::ostringstream ossInput;
    std::ostringstream ossOutput;
    std::ostringstream ossIncludedFiles;
    std::string method;
    bool outputItem;
    IfcVisitor(Env& env0, const std::vector<std::string>& skipDirs0)
        : env(env0),
          skipDirs(skipDirs0),
          hadInput(false),
          hadOutput(false),
          hadIncludedFiles(false),
          method("sat"),
          outputItem(false) {}
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
          ossIncludedFiles << ",\n";
        }
        ossIncludedFiles << "    \"" << Printer::escapeStringLit(ii->m()->filepath()) << "\"";
        hadIncludedFiles = true;
      }
      return true;
    }
    void vVarDeclI(VarDeclI* vdi) {
      VarDecl* vd = vdi->e();
      if (vd->type().isPar() && !vd->type().isAnn() &&
          (vd->e() == nullptr || (vd->e() == constants().absent &&
                                  vd->ann().contains(constants().ann.mzn_was_undefined)))) {
        if (hadInput) {
          ossInput << ",\n";
        }
        output_var_desc_json(env, vd, ossInput);
        hadInput = true;
      } else {
        bool process_var = false;
        if (vd->ann().contains(constants().ann.add_to_output)) {
          if (!hadAddToOutput) {
            ossOutput.str("");
            hadOutput = false;
          }
          hadAddToOutput = true;
          process_var = true;
        } else if (!hadAddToOutput) {
          process_var =
              vd->type().isvar() &&
              (vd->e() == nullptr || vd->ann().contains(constants().ann.rhs_from_assignment));
        }
        if (process_var) {
          if (hadOutput) {
            ossOutput << ",\n";
          }
          output_var_desc_json(env, vd, ossOutput);
          hadOutput = true;
        }
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
    void vOutputI(OutputI* oi) { outputItem = true; }
  } _ifc(env, skipDirs);
  iter_items(_ifc, m);
  os << "{\n  \"input\" : {\n"
     << _ifc.ossInput.str() << "\n  },\n  \"output\" : {\n"
     << _ifc.ossOutput.str() << "\n  }";
  os << ",\n  \"method\": \"";
  os << _ifc.method;
  os << "\"";
  os << ",\n  \"has_output_item\": " << (_ifc.outputItem ? "true" : "false");
  os << ",\n  \"included_files\": [\n" << _ifc.ossIncludedFiles.str() << "\n  ]";
  os << "\n}\n";
}

std::string create_enum_to_string_name(Id* ident, const std::string& prefix) {
  std::ostringstream ss;
  if (ident->str().c_str()[0] == '\'') {
    ss << "'" << prefix << ident->str().substr(1);
  } else {
    ss << prefix << *ident;
  }
  return ss.str();
}

}  // namespace MiniZinc
