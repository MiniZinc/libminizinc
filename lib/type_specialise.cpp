/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/astiterator.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/hash.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/type.hh>
#include <minizinc/typecheck.hh>

namespace MiniZinc {

namespace {

// Type-inst occurrence (indexed into the types vector)
struct TIOcc {
  // index in the types vector
  unsigned int idx;
  // index_set, or -1
  int idxSet;
  // Constructor
  TIOcc(unsigned int idx0, int idxSet0 = -1) : idx(idx0), idxSet(idxSet0) {}
};

typedef std::unordered_map<ASTString, std::vector<TIOcc>> TIOccMap;

Type type_meet(const Type& t0, const Type& t1) {
  Type m = t0;
  if (t0.bt() == Type::BT_FLOAT || t1.bt() == Type::BT_FLOAT) {
    m.bt(Type::BT_FLOAT);
  } else if (t0.bt() == Type::BT_INT || t1.bt() == Type::BT_INT) {
    m.bt(Type::BT_INT);
    if (t0.typeId() != t1.typeId()) {
      m.typeId(0);
    }
  }
  return m;
}

/// Return base type of TIId occurrence \a occ
/// If \a occ refers to the domain, return the base type of the variable
/// If \a occ refers to an index, return the enum type or int for that index
Type base_type(EnvI& env, std::vector<Type>& types, const TIOcc& occ) {
  Type cur_t;
  if (occ.idxSet == -1) {
    cur_t = types[occ.idx];
    if (cur_t.dim() > 0 && cur_t.typeId() != 0) {
      const auto& aes = env.getArrayEnum(cur_t.typeId());
      cur_t.typeId(0);
      cur_t.dim(0);
      cur_t.typeId(aes[aes.size() - 1]);
    }
  } else {
    if (types[occ.idx].typeId() != 0) {
      const auto& aes = env.getArrayEnum(types[occ.idx].typeId());
      cur_t = Type::parenum(aes[occ.idxSet]);
    } else {
      cur_t = Type::parint();
    }
    cur_t.st(types[occ.idx].st());
  }
  return cur_t;
}

void adapt_to_base_type(EnvI& env, std::vector<Type>& types, const TIOcc& occ, Type bt) {
  if (occ.idxSet == -1) {
    Type& t = types[occ.idx];
    t.bt(bt.bt());
    if (t.typeId() != 0 && bt.typeId() == 0) {
      if (t.dim() > 0) {
        const auto& aes = env.getArrayEnum(types[occ.idx].typeId());
        if (aes[aes.size() - 1] != 0) {
          std::vector<unsigned int> et = aes;
          et[aes.size() - 1] = 0;
          t.typeId(env.registerArrayEnum(et));
        }
      } else {
        t.typeId(0);
      }
    }
  } else {
    Type& t = types[occ.idx];
    if (t.typeId() != 0 && bt.typeId() == 0) {
      if (t.dim() > 0) {
        const auto& aes = env.getArrayEnum(types[occ.idx].typeId());
        if (aes[occ.idxSet] != 0) {
          std::vector<unsigned int> et = aes;
          et[occ.idxSet] = 0;
          t.typeId(env.registerArrayEnum(et));
        }
      } else {
        t.typeId(0);
      }
    }
  }
}

void least_common_supertype(EnvI& env, Call* call, std::vector<Type>& types, TIOccMap& map,
                            ASTString v, const TIOcc& cur) {
  auto& occs = map.emplace(v, std::vector<TIOcc>()).first->second;

  Type cur_t = base_type(env, types, cur);

  if (!occs.empty()) {
    // Invariant: all recorded occurrences are already at the least common supertype
    Type prevLcs = base_type(env, types, occs[0]);
    Type lcs = type_meet(prevLcs, cur_t);
    if (lcs != prevLcs) {
      // need to change all other occurrences
      for (const auto& o : occs) {
        adapt_to_base_type(env, types, o, lcs);
      }
    }
    // change current occurrence
    adapt_to_base_type(env, types, cur, lcs);
  }
  occs.emplace_back(cur);
}

/// Return the types of the concrete instantiation of \a call
std::vector<Type> instantiated_types(EnvI& env, Call* call) {
  // For each type-inst variable in the decl of the call,
  // compute the common supertype of all arguments of that type-inst variable
  FunctionI* decl = call->decl();
  std::vector<Type> types(call->argCount());
  TIOccMap ti_var_types;

  for (unsigned int i = 0; i < call->argCount(); i++) {
    types[i] = decl->param(i)->ti()->hasTiVariable() ? Expression::type(call->arg(i))
                                                     : decl->param(i)->ti()->type();
    if (types[i].isbot()) {
      if (decl->param(i)->ti()->type().st() == Type::ST_SET) {
        // array of bot can be used for array of set of ... param
        types[i].st(Type::ST_SET);
      }
      auto bt = decl->param(i)->ti()->type().bt();
      if (bt != Type::BT_TUPLE && bt != Type::BT_RECORD && bt != Type::BT_TOP) {
        types[i].bt(bt);
      }
    }
    TypeInst* ti = decl->param(i)->ti();
    for (int j = 0; j < static_cast<int>(ti->ranges().size()); j++) {
      if (TIId* tiid = Expression::dynamicCast<TIId>(ti->ranges()[j]->domain())) {
        least_common_supertype(env, call, types, ti_var_types, tiid->v(), TIOcc(i, j));
      }
    }
    if (TIId* tiid = Expression::dynamicCast<TIId>(ti->domain())) {
      least_common_supertype(env, call, types, ti_var_types, tiid->v(), TIOcc(i));
    }
  }

  return types;
}

struct InstantiatedItem {
  ASTString ident;
  std::vector<Type> argTypes;

  InstantiatedItem() {}
  InstantiatedItem(EnvI& env, ASTString ident0, std::vector<Type> argTypes0)
      : ident(ident0), argTypes(std::move(argTypes0)) {}

  bool operator==(const InstantiatedItem& ia) const {
    if (ident != ia.ident || argTypes.size() != ia.argTypes.size()) {
      return false;
    }
    for (unsigned int i = 0; i < argTypes.size(); i++) {
      if (argTypes[i] != ia.argTypes[i]) {
        return false;
      }
      if (argTypes[i].typeId() != ia.argTypes[i].typeId()) {
        return false;
      }
    }
    return true;
  }
};

struct IAHash {
  size_t operator()(const InstantiatedItem& ia) const {
    /// TODO: better hash function
    return ia.ident.hash();
  }
};

struct InstanceMapItem {
  ASTString baseName;
  bool exists;
  int instanceId;
  std::vector<Type> argTypes;
  bool parExists;
  std::vector<Type> parTypes;

  InstanceMapItem(const ASTString baseName0, bool exists0, int instanceId0,
                  std::vector<Type> argTypes0, bool parExists0, std::vector<Type> parTypes0)
      : baseName(std::move(baseName0)),
        exists(exists0),
        instanceId(instanceId0),
        argTypes(std::move(argTypes0)),
        parExists(parExists0),
        parTypes(std::move(parTypes0)) {}
};

class InstanceMap {
private:
  std::unordered_map<InstantiatedItem, int, IAHash> _map;
  std::unordered_set<InstantiatedItem, IAHash> _instances;
  int _instanceCount = 0;

public:
  InstanceMapItem getOrInsert(EnvI& env, Call* call) {
    auto argTypes = instantiated_types(env, call);
    for (const auto& t : argTypes) {
      assert(t.dim() >= 0);
    }
    auto baseName = call->id();
    if (baseName.endsWith("_reif")) {
      std::string reifName(baseName.c_str());
      baseName = ASTString(reifName.substr(0, reifName.length() - 5));
      argTypes.pop_back();
    } else if (baseName.endsWith("_imp")) {
      std::string impName(baseName.c_str());
      baseName = ASTString(impName.substr(0, impName.length() - 4));
      argTypes.pop_back();
    }
    auto parTypes = argTypes;
    for (auto& t : parTypes) {
      t.mkPar(env);
    }
    auto baseArgTypes = parTypes;
    for (auto& t : baseArgTypes) {
      t.mkPresent(env);
    }
    auto baseInstance =
        _map.emplace(InstantiatedItem(env, call->id(), baseArgTypes), _instanceCount);
    if (baseInstance.second) {
      // Base instance not created yet, so use new instance ID
      _instanceCount++;
    }
    auto instanceId = baseInstance.first->second;
    auto concreteInstance = _instances.emplace(env, call->id(), argTypes);
    auto parInstance = _instances.emplace(env, call->id(), parTypes);

    return {baseName, !concreteInstance.second, instanceId,
            argTypes, !parInstance.second,      parTypes};
  }
};

class ConcreteCallAgenda {
private:
  std::vector<Call*> _agenda;
  std::unordered_set<Call*> _seen;

public:
  void push(Call* c) {
    if (_seen.emplace(c).second) {
      _agenda.push_back(c);
    }
  }
  Call* back() const { return _agenda.back(); }
  void pop() { _agenda.pop_back(); }
  bool empty() const { return _agenda.empty(); }
};

class CollectConcreteCalls : public EVisitor {
public:
  ConcreteCallAgenda& agenda;
  CollectConcreteCalls(ConcreteCallAgenda& agenda0) : agenda(agenda0) {}
  void vCall(Call* c) {
    if (c->decl() != nullptr) {
      if (c->decl()->e() != nullptr || (c->argCount() == 1 && c->id() == "enum_of")) {
        if (c->decl()->isPolymorphic()) {
          agenda.push(c);
        }
      }
    }
  }
};

class CollectConcreteCallsFromItems : public ItemVisitor {
public:
  CollectConcreteCalls ccc;
  CollectConcreteCallsFromItems(ConcreteCallAgenda& agenda0) : ccc(agenda0) {}
  void vVarDeclI(VarDeclI* vdi) { top_down(ccc, vdi->e()); }
  void vAssignI(AssignI* ai) { top_down(ccc, ai->e()); }
  void vConstraintI(ConstraintI* ci) { top_down(ccc, ci->e()); }
  void vSolveI(SolveI* si) {
    if (si->e() != nullptr) {
      top_down(ccc, si->e());
    }
  }
  void vOutputI(OutputI* oi) { top_down(ccc, oi->e()); }
  void vFunctionI(FunctionI* fi) {
    // Check if function is polymorphic. If not, collect calls from body.
    if (!fi->isPolymorphic()) {
      top_down(ccc, fi->e());
      top_down(ccc, fi->ti());
      for (unsigned int i = 0; i < fi->paramCount(); i++) {
        top_down(ccc, fi->param(i));
      }
    }
  }
};

class Instantiator {
private:
  EnvI& _env;
  ConcreteCallAgenda& _agenda;
  InstanceMap& _instanceMap;
  TyperFn& _typer;
  std::unique_ptr<Model> _specialised;

public:
  Instantiator(EnvI& env, ConcreteCallAgenda& agenda, InstanceMap& instanceMap, TyperFn& typer)
      : _env(env),
        _agenda(agenda),
        _instanceMap(instanceMap),
        _typer(typer),
        _specialised(new Model) {}

  static bool walkTIMap(EnvI& env, ASTStringMap<Type>& ti_map, TypeInst* struct_ti,
                        StructType* tt) {
    auto* al = Expression::cast<ArrayLit>(struct_ti->domain());
    assert(al->size() == tt->size() ||
           al->size() + 1 == tt->size());  // May have added concrete type for reification
    for (unsigned int i = 0; i < al->size(); i++) {
      auto* ti = Expression::cast<TypeInst>((*al)[i]);
      Type curType = ti->type();
      Type concrete_type = (*tt)[i];
      curType.bt(concrete_type.bt());
      curType.typeId(0);
      curType.st(concrete_type.st());
      if (curType.dim() == -1) {
        curType.dim(concrete_type.dim());
      }
      if (curType.any()) {
        curType.any(false);
        if (curType.structBT()) {
          curType.ot(Type::OT_PRESENT);
          curType.ti(Type::TI_PAR);
          curType.cv(false);
        } else {
          curType.ot(concrete_type.ot());
          curType.ti(concrete_type.ti());
          curType.cv(concrete_type.cv());
        }
      }
      curType.typeId(concrete_type.typeId());
      ti->type(curType);
      if (TIId* tiid = Expression::dynamicCast<TIId>(ti->domain())) {
        ti_map.emplace(tiid->v(), ti->ranges().empty() ? curType : curType.elemType(env));
        if (curType.typeId() == 0) {
          // replace tiid with empty domain
          ti->domain(nullptr);
        } else if (curType.structBT()) {
          StructType* ctt = env.getStructType(curType);
          // Create new TypeInst domain for struct argument
          ti->setStructDomain(env, curType, true);
          if (!walkTIMap(env, ti_map, ti, ctt)) {
            return false;
          }
        } else {
          auto enumId = curType.typeId();
          if (curType.dim() != 0) {
            const auto& aet = env.getArrayEnum(curType.typeId());
            enumId = aet[aet.size() - 1];
          }
          if (enumId == 0) {
            // replace tiid with empty domain
            ti->domain(nullptr);
          } else {
            VarDeclI* enumVdi = env.getEnum(enumId);
            ti->domain(enumVdi->e()->id());
          }
        }
      } else if (curType.structBT()) {
        assert(concrete_type.bt() == curType.bt());
        assert(concrete_type.typeId() != 0);
        if (!walkTIMap(env, ti_map, ti, env.getStructType(ti->type()))) {
          return false;
        }
      }
      for (unsigned int j = 0; j < ti->ranges().size(); j++) {
        if (TIId* tiid = Expression::dynamicCast<TIId>(ti->ranges()[j]->domain())) {
          if (tiid->isEnum()) {
            // find concrete enum type
            if (curType.typeId() == 0) {
              // lct is not an enum type -> turn this one into a simple int
              ti->ranges()[j]->domain(nullptr);
              ti_map.emplace(tiid->v(), Type::parint());
            } else {
              const auto& aet = env.getArrayEnum(curType.typeId());
              if (aet[j] == 0) {
                // lct is not an enum type -> turn this one into a simple int
                ti->ranges()[j]->domain(nullptr);
                ti_map.emplace(tiid->v(), Type::parint());
              } else {
                ti->ranges()[j]->domain(nullptr);
                ti->ranges()[j]->type(Type::parenum(aet[j]));
                ti_map.emplace(tiid->v(), Type::parenum(aet[j]));
              }
            }
          } else {
            ti_map.emplace(tiid->v(), Type::parint(curType.dim()));
            // add concrete number of ranges
            std::vector<TypeInst*> newRanges(curType.dim());
            for (int k = 0; k < curType.dim(); k++) {
              newRanges[k] = new TypeInst(Location().introduce(), Type::parint());
            }
            ti->setRanges(newRanges);
            break;  // only one general tiid allowed in index set
          }
        }
      }
      if (ti->type().bt() == Type::BT_TUPLE) {
        env.registerTupleType(ti);
      } else if (ti->type().bt() == Type::BT_RECORD) {
        env.registerRecordType(ti);
      }
    }
    return true;
  }

  static void updateReturnTypeInst(EnvI& env, ASTStringMap<Type>& ti_map, TypeInst* ti) {
    if (ti->type().bt() == Type::BT_TUPLE) {
      auto* al = Expression::cast<ArrayLit>(ti->domain());
      auto* st = env.getStructType(ti->type());
      for (unsigned int i = 0; i < st->size(); i++) {
        updateReturnTypeInst(env, ti_map, Expression::cast<TypeInst>((*al)[i]));
      }
    } else if (ti->type().bt() == Type::BT_RECORD) {
      auto* al = Expression::cast<ArrayLit>(ti->domain());
      auto* st = env.getStructType(ti->type());
      for (unsigned int i = 0; i < st->size(); i++) {
        updateReturnTypeInst(env, ti_map, Expression::cast<TypeInst>((*al)[i]));
      }
    } else if (TIId* tiid = Expression::dynamicCast<TIId>(ti->domain())) {
      Type ret_type = ti_map.find(tiid->v())->second;
      if (ret_type.dim() != 0 && ti->type().dim() == 0) {
        ret_type = ret_type.elemType(env);
      }
      if (ret_type.structBT()) {
        ti->setStructDomain(env, ret_type, false, false);
      } else {
        Type t = ti->type();
        t.bt(ret_type.bt());
        if (!tiid->isEnum()) {
          t.st(ret_type.st());
        }
        if (t.any()) {
          t.any(false);
          t.ot(ret_type.ot());
          t.ti(ret_type.ti());
        }
        ti->domain(nullptr);
        auto typeId = ret_type.typeId();
        if (typeId != 0 && ret_type.bt() == Type::BT_INT) {
          if (ret_type.dim() != 0 && typeId != 0) {
            const auto& aet = env.getArrayEnum(typeId);
            typeId = aet[aet.size() - 1];
          }
          if (typeId != 0) {
            VarDeclI* enumVdi = env.getEnum(typeId);
            ti->domain(enumVdi->e()->id());
          }
        }
        t.typeId(typeId);
        ti->type(t);
      }
    }
    // update index sets in return type
    for (unsigned int i = 0; i < ti->ranges().size(); i++) {
      if (TIId* tiid = Expression::dynamicCast<TIId>(ti->ranges()[i]->domain())) {
        Type ret_type = ti_map.find(tiid->v())->second;
        if (tiid->isEnum()) {
          // find concrete enum type
          if (ret_type.typeId() == 0) {
            // not an enum type -> turn this one into a simple int
            ti->ranges()[i]->domain(nullptr);
          } else if (ret_type.dim() != 0) {
            const auto& aet = env.getArrayEnum(ret_type.typeId());
            if (aet[i] == 0) {
              // not an enum type -> turn this one into a simple int
              ti->ranges()[i]->domain(nullptr);
            } else {
              ti->ranges()[i]->domain(nullptr);
              ti->ranges()[i]->type(Type::parenum(aet[i]));
            }
          } else {
            ti->ranges()[i]->domain(nullptr);
            ti->ranges()[i]->type(Type::parenum(ret_type.typeId()));
          }
        } else {
          // add concrete number of ranges
          std::vector<TypeInst*> newRanges(ret_type.dim());
          for (int k = 0; k < ret_type.dim(); k++) {
            newRanges[k] = new TypeInst(Location().introduce(), Type::parint());
          }
          auto t = ti->type();
          t.typeId(0);
          t.dim(ret_type.dim());
          // Type ID is actually element type, array will be registered later
          t.typeId(ti->type().typeId());
          ti->type(t);
          ti->setRanges(newRanges);
          break;  // only one general tiid allowed in index set
        }
      }
    }
    if (ti->type().dim() > 0 && !ti->type().structBT()) {
      auto t = ti->type();
      std::vector<unsigned int> enumIds(ti->type().dim() + 1, 0);
      for (unsigned int i = 0; i < ti->ranges().size(); i++) {
        enumIds[i] = ti->ranges()[i]->type().typeId();
      }
      enumIds[ti->type().dim()] = ti->type().typeId();
      t.typeId(env.registerArrayEnum(enumIds));
      ti->type(t);
    } else if (ti->type().bt() == Type::BT_TUPLE) {
      env.registerTupleType(ti);
    } else if (ti->type().bt() == Type::BT_RECORD) {
      env.registerRecordType(ti);
    }
  }

  void operator()(Call* call) {
    if (call->id() == _env.constants.ids.enumOf && call->argCount() == 1) {
      // Rewrite to enum_of_internal with enum argument
      auto enumId = Expression::type(call->arg(0)).typeId();
      if (enumId != 0 && Expression::type(call->arg(0)).dim() != 0) {
        const auto& enumIds = _env.getArrayEnum(enumId);
        enumId = enumIds[enumIds.size() - 1];
      }
      call->id(ASTString(_env.constants.ids.enumOfInternal));
      if (enumId != 0) {
        VarDecl* enumDecl = _env.getEnum(enumId)->e();
        call->arg(0, enumDecl->id());
      } else {
        GCLock lock;
        IntSetVal* inf = IntSetVal::a(-IntVal::infinity(), IntVal::infinity());
        call->arg(0, new SetLit(Location().introduce(), inf));
      }
      FunctionI* newDecl = _env.model->matchFn(_env, call, false);
      call->decl(newDecl);
    }
    // Check if instance for this call already exists
    auto lookup = _instanceMap.getOrInsert(_env, call);
    auto instanceId = lookup.instanceId;
    if (!lookup.exists) {
      // new instance: create copies of non-reif, _reif and _imp function
      std::vector<Type> concrete_types = lookup.argTypes;
      auto* nonReif = _env.model->matchFn(_env, lookup.baseName, concrete_types, false);
      // Push additional var bool for reified versions
      concrete_types.push_back(Type::varbool());
      auto* reified =
          _env.model->matchFn(_env, _env.reifyId(lookup.baseName), concrete_types, false);
      auto* halfReif =
          _env.model->matchFn(_env, EnvI::halfReifyId(lookup.baseName), concrete_types, false);
      assert(call->decl() == nonReif || call->decl() == reified || call->decl() == halfReif);
      std::vector<FunctionI*> matches({nonReif, reified, halfReif});
      if (!lookup.parExists) {
        // Also create par version in case required by output.
        // This is needed since if this instance can't be made par by the type checker,
        // but we actually have a par version of the polymorphic function, we should use it.
        std::vector<Type> concrete_types = lookup.parTypes;
        auto* parNonReif = _env.model->matchFn(_env, lookup.baseName, concrete_types, false);
        if (nonReif != nullptr && nonReif != parNonReif) {
          matches.push_back(parNonReif);
        }
        concrete_types.push_back(Type::parbool());
        auto* parReified =
            _env.model->matchFn(_env, _env.reifyId(lookup.baseName), concrete_types, false);
        if (parReified != nullptr && reified != parReified) {
          matches.push_back(parReified);
        }
        auto* parHalfReif =
            _env.model->matchFn(_env, EnvI::halfReifyId(lookup.baseName), concrete_types, false);
        if (parHalfReif != nullptr && halfReif != parHalfReif) {
          matches.push_back(parHalfReif);
        }
      }

      for (auto* fi : matches) {
        if (fi == nullptr) {
          continue;
        }

        // Copy function (without following Ids or copying other function decls)
        _typer.reset(_env, fi);
        auto* fi_copy = copy(_env, fi, false, false, false)->cast<FunctionI>();
        fi_copy->isMonomorphised(true);
        // Rename copy
        std::ostringstream oss;
        oss << "\\" << instanceId << "@" << fi->id();
        fi_copy->id(ASTString(oss.str()));

        // Replace type-inst vars by concrete types

        std::unordered_map<ASTString, Type> ti_map;
        // Update parameter types
        TypeList tt(concrete_types);
        walkTIMap(_env, ti_map, fi_copy->paramTypes(), &tt);
        // Update VarDecl types based on updated TypeInst objects
        for (unsigned int i = 0; i < fi_copy->paramCount(); ++i) {
          fi_copy->param(i)->type(fi_copy->param(i)->ti()->type());
        }

        // update return type
        updateReturnTypeInst(_env, ti_map, fi_copy->ti());

        if (fi_copy->e() == nullptr) {
          // built-in function, have to redirect to original polymorphic built-in
          std::vector<Expression*> args(fi_copy->paramCount());
          for (unsigned int i = 0; i < fi_copy->paramCount(); i++) {
            args[i] = fi_copy->param(i)->id();
          }
          Call* body = Call::a(Location().introduce(), fi->id(), args);
          body->decl(fi);
          body->type(fi_copy->ti()->type());
          fi_copy->e(body);
        } else {
          // update all types in the body
          _typer.retype(_env, fi_copy);
          // put calls in the body on the agenda
          CollectConcreteCalls ccc(_agenda);
          top_down(ccc, fi_copy->e());
        }
        // TODO: Currently it's possible for us to actually be generating the same
        // concrete instance again, even though instantiated_types() gave a different
        // result. We should probably fix instantiated_types() to be more accurate.
        if (_specialised->registerFn(_env, fi_copy, true, false)) {
          _specialised->addItem(fi_copy);
          if (call->decl() == fi) {
            call->decl(fi_copy);
            call->rehash();
          }
        } else if (call->decl() == fi) {
          call->id(fi_copy->id());
          FunctionI* newDecl = _specialised->matchFn(_env, call, false);
          assert(newDecl != nullptr);
          call->decl(newDecl);
          call->rehash();
        }

        _typer.retype(_env, fi);
      }
    } else {
      // match call to previously copied function
      std::ostringstream oss;
      oss << "\\" << instanceId << "@" << call->decl()->id();
      call->id(ASTString(oss.str()));
      FunctionI* newDecl = _specialised->matchFn(_env, call, false);
      assert(newDecl != nullptr);
      call->decl(newDecl);
      call->rehash();
    }
  }

  void finish() {
    for (auto* it : *_specialised) {
      auto* fi = it->cast<FunctionI>();
      _env.model->registerFn(_env, fi, true);
      _env.model->addItem(fi);
    }
  }
};

}  // namespace

/*
 Specialisation of parametric functions

 After type checking:
 - recursively collect all calls to parametric functions from
   - toplevel variable declarations
   - constraints
   - solve item
   - function decls that are not parametric
 - all these calls have concrete types
 - put these on the todo list, and for each item of the todo list:
     - make copies of the parametric functions for the concrete types
     - we cannot overload on enum type, so we need to mangle the names
     - mangling scheme: prefix identifier with \XXX@, where X is an integer.
     - change all occurrences of type-inst variables to the concrete type
     - type-check the body of the function again (or at least propagate the concrete type)
     - if the body contains calls to parametric functions, put these calls
         (with the now concrete types) on the todo list
     - change call ids and decls to point to copies
     - keep a registry of (name,concrete type) -> instantiated function
 */
void type_specialise(Env& env, Model* model, TyperFn& typer) {
  ConcreteCallAgenda agenda;

  CollectConcreteCallsFromItems cci(agenda);
  iter_items(cci, model);

  InstanceMap instanceMap;

  Instantiator instantiate(env.envi(), agenda, instanceMap, typer);

  while (!agenda.empty()) {
    GCLock lock;
    Call* call = agenda.back();
    agenda.pop();
    instantiate(call);
  }

  instantiate.finish();
}

std::string demonomorphise_identifier(const ASTString& ident) {
  if (ident.empty() || ident.c_str()[0] != '\\') {
    return std::string(ident.c_str());
  }
  std::string s(ident.c_str() + 1);
  auto s_end = s.find_first_of('@');
  if (s_end != std::string::npos) {
    return s.substr(s_end + 1);
  }
  return std::string(ident.c_str());
}

namespace {

class Demonomorphiser : public EVisitor {
public:
  // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
  void vCall(Call* c) {
    if (c->decl() != nullptr && c->decl()->isMonomorphised() && c->decl()->fromStdLib()) {
      c->id(ASTString(demonomorphise_identifier(c->id())));
    }
  }
};

class ItemDemonomorphiser : public ItemVisitor {
public:
  Demonomorphiser dm;
  void vVarDeclI(VarDeclI* vdi) { top_down(dm, vdi->e()); }
  void vAssignI(AssignI* ai) { top_down(dm, ai->e()); }
  void vConstraintI(ConstraintI* ci) { top_down(dm, ci->e()); }
  void vSolveI(SolveI* si) {
    if (si->e() != nullptr) {
      top_down(dm, si->e());
    }
  }
  void vOutputI(OutputI* oi) { top_down(dm, oi->e()); }
  void vFunctionI(FunctionI* fi) {
    top_down(dm, fi->ti());
    for (unsigned int i = 0; i < fi->paramCount(); i++) {
      top_down(dm, fi->param(i));
    }
    if (fi->e() != nullptr) {
      top_down(dm, fi->e());
    }
  }
};

}  // namespace

void type_demonomorphise_library(Env& e, Model* model) {
  std::vector<FunctionI*> toRename;
  ASTStringSet functionIds;
  for (auto& fi : model->functions()) {
    if (!fi.fromStdLib()) {
      if (fi.id().beginsWith("\\")) {
        toRename.push_back(&fi);
      }
      if (fi.id().find('@') != std::string::npos) {
        functionIds.insert(fi.id());
      }
    }
  }
  for (auto* fi : toRename) {
    GCLock lock;
    std::string ident(fi->id().c_str());
    ident[0] = '_';
    while (functionIds.find(ASTString(ident)) != functionIds.end()) {
      ident = "_" + ident;
    }
    ASTString new_ident(ident);
    fi->id(new_ident);
    functionIds.insert(new_ident);
  }
  ItemDemonomorphiser idm;
  iter_items(idm, model);
}

}  // namespace MiniZinc
