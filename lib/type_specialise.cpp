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
    types[i] = call->arg(i)->type();
    if (types[i].isbot() && call->arg(i) == env.constants.absent) {
      types[i] = decl->param(i)->type();
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
      : ident(ident0), argTypes(std::move(argTypes0)) {
    for (auto& t : argTypes) {
      t.ot(Type::OT_PRESENT);
      t.mkPar(env);
    }
  }

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

class InstanceMap {
private:
  std::unordered_map<InstantiatedItem, int, IAHash> _map;

public:
  void insert(EnvI& env, ASTString ident, const std::vector<Type>& argTypes, int instanceCount) {
    _map.emplace(InstantiatedItem(env, ident, argTypes), instanceCount);
    for (const auto& t : argTypes) {
      assert(t.dim() >= 0);
    }
  }
  std::pair<int, std::vector<Type>> lookup(EnvI& env, Call* call) const {
    std::vector<Type> argTypes = instantiated_types(env, call);
    const auto& v = _map.find(InstantiatedItem(env, call->id(), argTypes));
    if (v == _map.end()) {
      return {-1, argTypes};
    }
    return {v->second, argTypes};
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

bool is_polymorphic(FunctionI* fi) {
  bool isPoly = fi->ti()->hasTiVariable();
  if (!isPoly) {
    for (unsigned int i = 0; i < fi->paramCount(); i++) {
      if (fi->param(i)->ti()->hasTiVariable()) {
        return true;
      }
    }
    return false;
  }
  return true;
}

class CollectConcreteCalls : public EVisitor {
public:
  ConcreteCallAgenda& agenda;
  CollectConcreteCalls(ConcreteCallAgenda& agenda0) : agenda(agenda0) {}
  void vCall(Call* c) {
    if (c->decl() != nullptr) {
      if (c->decl()->e() != nullptr || (c->argCount() == 1 && c->id() == "enum_of")) {
        if (is_polymorphic(c->decl())) {
          assert(c->argCount() != 1 || c->arg(0)->type().st() == Type::ST_PLAIN ||
                 c->arg(0)->type().ot() == Type::OT_PRESENT);
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
    if (!is_polymorphic(fi)) {
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
  int _instanceCount = 0;
  EnvI& _env;
  ConcreteCallAgenda& _agenda;
  InstanceMap& _instanceMap;
  TyperFn& _typer;

  struct ToGenerate {
    // If matches contains TIIds with "any" type, we have to generate the
    // corresponding par, par opt, var and var opt versions, but only if those
    // are not present in matches yet
    enum GenVersion { TG_STRUCT, TG_PAR, TG_PAROPT, TG_VAR, TG_VAROPT };
    GenVersion ver;
    std::unique_ptr<std::vector<ToGenerate>> inner;

    ToGenerate(const GenVersion& ver0) : ver(ver0), inner(nullptr) { assert(ver0 != TG_STRUCT); }
    ToGenerate(std::vector<ToGenerate>&& tup0)
        : ver(TG_STRUCT), inner(new std::vector<ToGenerate>(std::move(tup0))) {}
    ToGenerate(const std::vector<ToGenerate>& tup0)
        : ver(TG_STRUCT), inner(new std::vector<ToGenerate>(tup0)) {}
    ToGenerate(const ToGenerate& gen)
        : ver(gen.ver),
          inner(gen.inner == nullptr ? nullptr : new std::vector<ToGenerate>(*gen.inner)) {}
    ToGenerate(ToGenerate&& gen) : ver(gen.ver), inner(std::move(gen.inner)) {}

    ToGenerate& operator=(ToGenerate&&) = default;
    bool operator==(const ToGenerate& other) const {
      if (ver != other.ver) {
        return false;
      }
      if (ver == TG_STRUCT) {
        if (inner->size() != other.inner->size()) {
          return false;
        }
        for (size_t i = 0; i < inner->size(); ++i) {
          if (!((*inner)[i] == (*other.inner)[i])) {
            return false;
          }
        }
      }
      return true;
    }

    std::string toString() const {
      switch (ver) {
        case TG_STRUCT: {
          std::ostringstream ss;
          ss << "(";
          for (size_t i = 0; i < inner->size(); i++) {
            ss << (*inner)[i].toString();
            if (i < inner->size() - 1) {
              ss << ", ";
            }
          }
          ss << ")";
          return ss.str();
        }
        case TG_PAR:
          return "PAR";
        case TG_PAROPT:
          return "PAROPT";
        case TG_VAR:
          return "VAR";
        case TG_VAROPT:
          return "VAROPT";
      }
      assert(false);
      return "";
    }

    void reset(EnvI& env, StructType* match_types, StructType* concrete_types) const {
      assert(ver == TG_STRUCT);
      assert(match_types->size() == inner->size());
      assert(concrete_types->size() == inner->size());
      for (size_t i = 0; i < inner->size(); ++i) {
        Type ty = (*match_types)[i];
        if ((*inner)[i].ver == TG_STRUCT) {
          StructType* concrete_tt = env.getStructType((*concrete_types)[i]);
          StructType* match_tt = concrete_tt;
          std::vector<Type> inst_types;
          TypeList inst_types_ref(inst_types);
          if (ty.structBT()) {
            StructType* match_tt = env.getStructType(ty);
          } else if (ty.any()) {
            inst_types.resize(concrete_tt->size());
            for (size_t j = 0; j < inst_types.size(); ++j) {
              inst_types[j] = (*concrete_tt)[j];
              inst_types[j].any(true);
            }
            match_tt = &inst_types_ref;
          }
          (*inner)[i].reset(env, match_tt, concrete_tt);
        } else if (ty.any()) {
          assert(ver != TG_STRUCT);
          (*inner)[i].ver = TG_PAR;
        }
      }
    }

    bool increment(EnvI& env, StructType* match_types, StructType* concrete_types) const {
      assert(ver == TG_STRUCT);
      assert(match_types->size() == inner->size());
      assert(concrete_types->size() == inner->size());
      for (int i = static_cast<int>(inner->size() - 1); i >= 0; --i) {
        Type ty = (*match_types)[i];
        bool incremented = false;
        if ((*inner)[i].ver == TG_STRUCT) {
          StructType* concrete_tt = env.getStructType((*concrete_types)[i]);
          StructType* match_tt = concrete_tt;
          std::vector<Type> inst_types;
          TypeList inst_types_ref(inst_types);
          if (ty.structBT()) {
            StructType* match_tt = env.getStructType(ty);
          } else if (ty.any()) {
            inst_types.resize(concrete_tt->size());
            for (size_t j = 0; j < inst_types.size(); ++j) {
              inst_types[j] = (*concrete_tt)[j];
              inst_types[j].any(true);
            }
            match_tt = &inst_types_ref;
          }
          incremented = (*inner)[i].increment(env, match_tt, concrete_tt);
        } else if ((*inner)[i].ver < TG_VAROPT && ty.any()) {
          // found a match type we can increment
          (*inner)[i].ver = static_cast<GenVersion>((*inner)[i].ver + 1);
          incremented = true;
        }
        if (incremented) {
          for (size_t j = i + 1; j < inner->size(); ++j) {
            Type ty_back = (*match_types)[j];
            if ((*inner)[j].ver == TG_STRUCT) {
              StructType* concrete_tt = env.getStructType((*concrete_types)[j]);
              StructType* match_tt = concrete_tt;
              std::vector<Type> inst_types;
              TypeList inst_types_ref(inst_types);
              if (ty_back.structBT()) {
                StructType* match_tt = env.getStructType(ty_back);
              } else if (ty.any()) {
                inst_types.resize(concrete_tt->size());
                for (size_t j = 0; j < inst_types.size(); ++j) {
                  inst_types[j] = (*concrete_tt)[j];
                  inst_types[j].any(true);
                }
                match_tt = &inst_types_ref;
              }
              (*inner)[j].reset(env, match_tt, concrete_tt);
            } else if (ty_back.any()) {
              (*inner)[j].ver = TG_PAR;
            }
          }
          return true;
        }
      }
      return false;
    }

    // returns generation template from function item. Template is added to toGenerate if
    // isDuplicate return false. Returns whether the function parameters contained an "any".
    static std::pair<bool, ToGenerate> fromTIs(const EnvI& env, StructType* match_types,
                                               StructType* concrete_types) {
      bool hadAny = false;
      std::vector<ToGenerate> matchTypes;
      matchTypes.reserve(match_types->size());
      for (size_t i = 0; i < match_types->size(); ++i) {
        Type parType = (*match_types)[i];
        if (parType.structBT()) {
          StructType* match_tt = env.getStructType(parType);
          StructType* concrete_tt = env.getStructType((*concrete_types)[i]);
          auto ret = fromTIs(env, match_tt, concrete_tt);
          hadAny = hadAny || ret.first;
          matchTypes.emplace_back(ret.second);
        } else if (i < concrete_types->size() && (*concrete_types)[i].structBT()) {
          // Found a $-type that is being instantiated by a struct
          assert(parType.bt() == Type::BT_TOP);
          StructType* concrete_tt = env.getStructType((*concrete_types)[i]);
          StructType* match_tt = concrete_tt;
          std::vector<Type> inst_types;
          TypeList inst_types_ref(inst_types);
          if (parType.any()) {
            inst_types.resize(concrete_tt->size());
            for (size_t j = 0; j < inst_types.size(); ++j) {
              inst_types[j] = (*concrete_tt)[j];
              inst_types[j].any(true);
            }
            match_tt = &inst_types_ref;
          }
          auto ret = fromTIs(env, match_tt, concrete_tt);
          hadAny = hadAny || ret.first;
          matchTypes.emplace_back(ret.second);
        } else if (parType.any()) {
          hadAny = true;
          // start any types at TG_PAR (go through variants using increment)
          matchTypes.emplace_back(TG_PAR);
        } else if (parType.isPar()) {
          matchTypes.emplace_back(parType.isOpt() ? TG_PAROPT : TG_PAR);
        } else {
          matchTypes.emplace_back(parType.isOpt() ? TG_VAROPT : TG_VAR);
        }
      }
      assert(match_types->size() == matchTypes.size());
      return {hadAny, ToGenerate(std::move(matchTypes))};
    }
    // returns generation template from function item. Template is added to toGenerate if
    // condition returns true
    static bool anyVariants(EnvI& env, ToGenerate& gen, StructType* match_types,
                            StructType* concrete_types, std::vector<ToGenerate>& toGenerate,
                            const std::function<bool(const ToGenerate&)>& condition) {
      // Generate all versions for each any type that match "condition" and add them to toGenerate
      do {
        // check if current matchTypes already exists, if not, add it
        if (condition(gen)) {
          toGenerate.push_back(gen);
        }
      } while (gen.increment(env, match_types, concrete_types));
      return true;
    }
  };

public:
  Instantiator(EnvI& env, ConcreteCallAgenda& agenda, InstanceMap& instanceMap, TyperFn& typer)
      : _env(env), _agenda(agenda), _instanceMap(instanceMap), _typer(typer) {}

  static bool walkTIMap(EnvI& env, ASTStringMap<Type>& ti_map, TypeInst* struct_ti, StructType* tt,
                        const ToGenerate& tg) {
    auto* al = struct_ti->domain()->cast<ArrayLit>();
    assert(al->size() == tt->size() ||
           al->size() + 1 == tt->size());  // May have added concrete type for reification
    assert(tg.ver == ToGenerate::TG_STRUCT);
    assert(al->size() == tg.inner->size());
    for (size_t i = 0; i < al->size(); i++) {
      auto* ti = (*al)[i]->cast<TypeInst>();
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
        switch ((*tg.inner)[i].ver) {
          case ToGenerate::TG_PAR:
            curType.ot(Type::OT_PRESENT);
            curType.ti(Type::TI_PAR);
            break;
          case ToGenerate::TG_PAROPT:
            if (curType.st() == Type::ST_SET) {
              return false;
            }
            curType.ot(Type::OT_OPTIONAL);
            curType.ti(Type::TI_PAR);
            break;
          case ToGenerate::TG_VAR:
            if (curType.bt() != Type::BT_BOOL && curType.bt() != Type::BT_INT &&
                curType.bt() != Type::BT_FLOAT) {
              return false;
            }
            curType.ot(Type::OT_PRESENT);
            curType.ti(Type::TI_VAR);
            break;
          case ToGenerate::TG_VAROPT:
            if ((curType.bt() != Type::BT_BOOL && curType.bt() != Type::BT_INT &&
                 curType.bt() != Type::BT_FLOAT) ||
                curType.st() == Type::ST_SET) {
              return false;
            }
            curType.ot(Type::OT_OPTIONAL);
            curType.ti(Type::TI_VAR);
            break;
          case ToGenerate::TG_STRUCT:
            // Real "any" types are set in recursive call

            // The following statements violate the cononical representation of structs. (Hence the
            // workaround of setting the typeId to 0 above, and then back to its concrete type
            // below). The representation will be fixed again when the struct is registered.
            curType.ot(Type::OT_PRESENT);
            curType.ti(Type::TI_PAR);
            curType.cv(false);
            break;
          default:
            assert(false);
        }
      }
      curType.typeId(concrete_type.typeId());
      ti->type(curType);
      if (TIId* tiid = Expression::dynamicCast<TIId>(ti->domain())) {
        ti_map.emplace(tiid->v(), curType);
        if (curType.typeId() == 0) {
          // replace tiid with empty domain
          ti->domain(nullptr);
        } else if (curType.structBT()) {
          StructType* ctt = env.getStructType(curType);
          // Create new TypeInst domain for struct argument
          ti->setStructDomain(env, curType, true);
          if (!walkTIMap(env, ti_map, ti, ctt, (*tg.inner)[i])) {
            return false;
          }
        } else {
          auto enumId = curType.typeId();
          if (curType.dim() != 0) {
            const auto& aet = env.getArrayEnum(curType.typeId());
            enumId = aet[aet.size() - 1];
          }
          if (enumId != 0) {
            VarDeclI* enumVdi = env.getEnum(enumId);
            ti->domain(enumVdi->e()->id());
          }
        }
      } else if (curType.structBT()) {
        assert(concrete_type.bt() == curType.bt());
        assert(concrete_type.typeId() != 0);
        if (!walkTIMap(env, ti_map, ti, env.getStructType(ti->type()), (*tg.inner)[i])) {
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
            for (unsigned int k = 0; k < curType.dim(); k++) {
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
      auto* al = ti->domain()->cast<ArrayLit>();
      auto* st = env.getStructType(ti->type());
      for (unsigned int i = 0; i < st->size(); i++) {
        updateReturnTypeInst(env, ti_map, (*al)[i]->cast<TypeInst>());
      }
    } else if (ti->type().bt() == Type::BT_RECORD) {
      auto* al = ti->domain()->cast<ArrayLit>();
      auto* st = env.getStructType(ti->type());
      for (unsigned int i = 0; i < st->size(); i++) {
        updateReturnTypeInst(env, ti_map, (*al)[i]->cast<TypeInst>());
      }
    } else if (TIId* tiid = Expression::dynamicCast<TIId>(ti->domain())) {
      Type ret_type = ti_map.find(tiid->v())->second;
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
          for (unsigned int k = 0; k < ret_type.dim(); k++) {
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
      auto enumId = call->arg(0)->type().typeId();
      if (enumId != 0 && call->arg(0)->type().dim() != 0) {
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
    auto lookup = _instanceMap.lookup(_env, call);
    auto instanceId = lookup.first;
    ASTString mangledName;
    if (instanceId == -1) {
      // new instance: create copies of all possible functions

      auto matches = _env.model->possibleMatches(_env, call->id(), lookup.second);

      instanceId = _instanceCount++;

      TypeList concrete_tt = TypeList(lookup.second);
      std::vector<std::vector<Type>> match_types;
      match_types.reserve(matches.size());
      std::vector<TypeList> match_tts;
      match_tts.reserve(matches.size());

      std::vector<std::vector<ToGenerate>> toGenerate(
          matches.size(), std::vector<ToGenerate>(1, ToGenerate::TG_PAR));
      std::vector<bool> generateVariants(matches.size(), false);
      for (size_t i = 0; i < matches.size(); ++i) {
        match_types.emplace_back(matches[i]->paramCount());
        for (int j = 0; j < match_types[i].size(); ++j) {
          match_types[i][j] = matches[i]->param(j)->ti()->type();
        }
        match_tts.emplace_back(match_types[i]);
        auto pair = ToGenerate::fromTIs(_env, &match_tts[i], &concrete_tt);
        generateVariants[i] = pair.first;
        toGenerate[i][0] = std::move(pair.second);
      }

      // Reprocess functions that contain "any" parameters (non-"any" versions are preferred, and
      // duplicates that would be generated by "any" versions are not not generated)
      for (size_t i = 0; i < matches.size(); ++i) {
        if (generateVariants[i]) {
          ToGenerate tg(std::move(toGenerate[i][0]));
          toGenerate[i].pop_back();
          ToGenerate::anyVariants(_env, tg, &match_tts[i], &concrete_tt, toGenerate[i],
                                  [&](const ToGenerate& gen) {
                                    for (const auto& tg : toGenerate) {
                                      for (const auto& m : tg) {
                                        if (m == gen) {
                                          return false;
                                        }
                                      }
                                    }
                                    return true;
                                  });
        }
      }

      std::ostringstream oss;
      oss << "\\" << instanceId << "@" << call->decl()->id();
      mangledName = ASTString(oss.str());
      ASTString mangledBaseName;
      ASTString mangledReifName;
      ASTString mangledImpName;
      if (mangledName.endsWith("_reif")) {
        std::string ident_s(mangledName.c_str());
        mangledBaseName = ASTString(ident_s.substr(0, ident_s.length() - 5));
        mangledReifName = mangledName;
        mangledImpName = EnvI::halfReifyId(mangledBaseName);
      } else if (mangledName.endsWith("_imp")) {
        std::string ident_s(mangledName.c_str());
        mangledBaseName = ASTString(ident_s.substr(0, ident_s.length() - 4));
        mangledReifName = _env.reifyId(mangledBaseName);
        mangledImpName = mangledName;
      } else {
        mangledBaseName = mangledName;
        mangledReifName = _env.reifyId(mangledBaseName);
        mangledImpName = EnvI::halfReifyId(mangledBaseName);
      }

      _instanceMap.insert(_env, call->id(), lookup.second, instanceId);

      for (unsigned int matchIdx = 0; matchIdx < matches.size(); matchIdx++) {
        auto* fi = matches[matchIdx];
        for (auto& tg : toGenerate[matchIdx]) {
          // Copy function (without following Ids or copying other function decls)
          _typer.reset(_env, fi);
          auto* fi_copy = copy(_env, fi, false, false, false)->cast<FunctionI>();
          fi_copy->isMonomorphised(true);
          // Rename copy
          if (fi->id().endsWith("_reif")) {
            fi_copy->id(mangledReifName);
          } else if (fi->id().endsWith("_imp")) {
            fi_copy->id(mangledImpName);
          } else {
            fi_copy->id(mangledBaseName);
          }
          // Replace type-inst vars by concrete types

          std::vector<Type> concrete_types = lookup.second;
          // Push additional var bool for reified versions
          concrete_types.push_back(Type::varbool());

          std::unordered_map<ASTString, Type> ti_map;

          // Update parameter types
          TypeList tt(concrete_types);
          if (!walkTIMap(_env, ti_map, fi_copy->paramTypes(), &tt, tg)) {
            continue;
          }
          // Update VarDecl types based on updated TypeInst objects
          for (size_t i = 0; i < fi_copy->paramCount(); ++i) {
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
          _env.model->registerFn(_env, fi_copy, true);
          _env.model->addItem(fi_copy);

          if (call->decl() == fi) {
            call->decl(fi_copy);
            call->rehash();
          }
          _typer.retype(_env, fi);
        }
      }
    } else {
      std::ostringstream oss;
      oss << "\\" << instanceId << "@" << call->decl()->id();
      mangledName = ASTString(oss.str());
    }
    // match call to previously copied function
    call->id(mangledName);
    FunctionI* newDecl = _env.model->matchFn(_env, call, false);
    assert(newDecl != nullptr);
    call->decl(newDecl);
    call->rehash();
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
