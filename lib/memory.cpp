/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/ast.hh>
#include <minizinc/hash.hh>
#include <minizinc/memory.hh>
#include <minizinc/values.hh>

#include <algorithm>
#include <cstdlib>
#include <vector>

#ifdef MZN_RC_CHECK
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#endif

#ifdef MZN_USE_MIMALLOC
#include <mimalloc.h>
#endif

#if !defined(MZN_USE_MIMALLOC) && !defined(_WIN32)
#include <sys/resource.h>
#endif

namespace MiniZinc {

#ifdef MZN_RC_CHECK
namespace {
/// Every allocated node (never destroyed, so that it outlives all other static objects)
std::unordered_set<ASTNode*>& live_nodes() {
  static auto* nodes = new std::unordered_set<ASTNode*>();
  return *nodes;
}
/// Reports the leaks when the program ends
struct LeakReport {
  ~LeakReport() { RC::reportLeaks(std::cerr); }
} leak_report;
}  // namespace
#endif

void* node_alloc(size_t size) {
#ifdef MZN_USE_MIMALLOC
  void* ret = mi_malloc(size);
#else
  void* ret = ::malloc(size);
#endif
  if (ret == nullptr) {
    throw Error("out of memory");
  }
#ifdef MZN_RC_CHECK
  live_nodes().insert(static_cast<ASTNode*>(ret));
#endif
  return ret;
}

void node_free(void* p) {
#ifdef MZN_RC_CHECK
  live_nodes().erase(static_cast<ASTNode*>(p));
#endif
#ifdef MZN_USE_MIMALLOC
  mi_free(p);
#else
  ::free(p);
#endif
}

size_t peak_memory() {
#ifdef MZN_USE_MIMALLOC
  size_t peak = 0;
  mi_process_info(nullptr, nullptr, nullptr, nullptr, &peak, nullptr, nullptr, nullptr);
  return peak;
#elif defined(_WIN32)
  return 0;  // Not measured
#else
  struct rusage ru;
  getrusage(RUSAGE_SELF, &ru);
#ifdef __APPLE__
  return static_cast<size_t>(ru.ru_maxrss);  // bytes
#else
  return static_cast<size_t>(ru.ru_maxrss) * 1024;  // kilobytes
#endif
#endif
}

ASTVec::ASTVec(size_t size) : ASTNode(NID_VEC), _size(size) {}

ASTChunk::ASTChunk(size_t size, unsigned int id) : ASTNode(id), _size(size) {}

namespace {
/// A trail item
struct TItem {
  Expression** l;
  Expression* v;
  bool mark;
  TItem(Expression** l0, Expression* v0) : l(l0), v(v0), mark(false) {}
};
thread_local std::vector<TItem> trail_items;
}  // namespace

void Trail::mark() {
  trail_items.emplace_back(nullptr, nullptr);
  trail_items.back().mark = true;
}
void Trail::trail(Expression** l, Expression* v) {
  RC::inc(v);  // the trail holds the old value until it is restored
  trail_items.emplace_back(l, v);
}
void Trail::untrail() {
  while (!trail_items.back().mark) {
    // The trail's count on the old value moves back to the field
    Expression* old = *trail_items.back().l;
    *trail_items.back().l = trail_items.back().v;
    trail_items.pop_back();
    RC::dec(old);
  }
  assert(!trail_items.empty() && trail_items.back().mark);
  trail_items.pop_back();
}

static_assert(RC::NID_ID == Expression::E_ID, "RC::NID_ID");
static_assert(RC::NID_VARDECL == Expression::E_VARDECL, "RC::NID_VARDECL");

// The count word must not make nodes larger (Expression reuses the padding next to _type)
static_assert(sizeof(void*) != 8 || sizeof(Expression) == 32, "Expression grew");
static_assert(sizeof(void*) != 8 || sizeof(ASTChunk) == 24, "ASTChunk grew");
static_assert(sizeof(void*) != 8 || sizeof(ASTVec) == 32, "ASTVec grew");

template <class F>
void Expression::forEachChild(ASTNode* n, F f) {
  using E = Expression::Edge;
  auto vec = [&](ASTNode* v) {
    if (v != nullptr) {
      f(v, E::Strong);
    }
  };
  auto ann = [&](const Annotation& a) {
    for (ExpressionSetIter it = a.begin(); it != a.end(); ++it) {
      f(*it, E::Strong);
    }
  };
  switch (n->_id) {
    case ASTNode::NID_CHUNK:
    case ASTNode::NID_STR:
      return;
    case ASTNode::NID_VEC: {
      // Expression vectors and location vectors: every element is a node, a string or unboxed
      auto* v = static_cast<ASTVec*>(n);
      for (size_t i = 0; i < v->_size; i++) {
        auto* c = static_cast<ASTNode*>(v->_data[i]);
        if (c != nullptr && (RC::node(c) == nullptr || RC::node(c)->_id != ASTNode::NID_STR)) {
          f(c, E::Strong);
        }
      }
      return;
    }
    case Item::II_INC:
    case Item::II_ASN:
    case Item::II_CON:
    case Item::II_SOL:
    case Item::II_OUT:
    case Item::II_FUN: {
      auto* item = static_cast<Item*>(n);
      switch (item->iid()) {
        case Item::II_ASN:
          f(item->cast<AssignI>()->e(), E::Strong);
          f(item->cast<AssignI>()->_decl, E::Weak);
          break;
        case Item::II_CON:
          f(item->cast<ConstraintI>()->e(), E::Strong);
          break;
        case Item::II_SOL:
          ann(item->cast<SolveI>()->ann());
          f(item->cast<SolveI>()->e(), E::Strong);
          break;
        case Item::II_OUT:
          ann(item->cast<OutputI>()->ann());
          f(item->cast<OutputI>()->e(), E::Strong);
          break;
        case Item::II_FUN: {
          auto* fi = item->cast<FunctionI>();
          f(fi->ti(), E::Strong);
          ann(fi->ann());
          f(fi->e(), E::Strong);
          vec(fi->_params.vec());
        } break;
        default:
          break;
      }
      return;
    }
    default:
      break;
  }
  auto* e = static_cast<Expression*>(n);
  // ponytail: locations (Location::LocVec) are not counted and never freed; their number is bounded
  // by the parsed source. Count them if flattening starts to create many.
  ann(Expression::ann(e));
  switch (Expression::eid(e)) {
    case Expression::E_INTLIT:
    case Expression::E_FLOATLIT:
    case Expression::E_BOOLLIT:
    case Expression::E_ANON:
    case Expression::E_STRINGLIT:
    case Expression::E_TIID:
      break;
    case Expression::E_SETLIT: {
      // Raw fields, not the getters: which set value the getters return depends on the type, which
      // can change after the value is stored
      auto* sl = Expression::cast<SetLit>(e);
      vec(sl->_u.isv);
      vec(sl->_v.vec());
    } break;
    case Expression::E_ID:
      // Raw fields, not the getters: the getters hide a target that is no longer alive.
      // Strong: copies of declarations can be reachable only through Ids.
      f(Expression::cast<Id>(e)->_decl, E::Strong);
      break;
    case Expression::E_ARRAYLIT: {
      auto* al = Expression::cast<ArrayLit>(e);
      if (al->_flag2) {
        f(al->_u.al, E::Strong);
      } else {
        vec(al->_u.v);
      }
      vec(al->_dims.vec());
    } break;
    case Expression::E_ARRAYACCESS:
      f(Expression::cast<ArrayAccess>(e)->v(), E::Strong);
      vec(Expression::cast<ArrayAccess>(e)->idx().vec());
      break;
    case Expression::E_FIELDACCESS:
      f(Expression::cast<FieldAccess>(e)->v(), E::Strong);
      f(Expression::cast<FieldAccess>(e)->field(), E::Strong);
      break;
    case Expression::E_COMP: {
      auto* c = Expression::cast<Comprehension>(e);
      f(c->_e, E::Strong);
      vec(c->_g.vec());
      vec(c->_gIndex.vec());
    } break;
    case Expression::E_ITE:
      f(Expression::cast<ITE>(e)->_eElse, E::Strong);
      vec(Expression::cast<ITE>(e)->_eIfThen.vec());
      break;
    case Expression::E_BINOP:
      f(Expression::cast<BinOp>(e)->lhs(), E::Strong);
      f(Expression::cast<BinOp>(e)->rhs(), E::Strong);
      f(Expression::cast<BinOp>(e)->_decl, E::Weak);
      break;
    case Expression::E_UNOP:
      f(Expression::cast<UnOp>(e)->e(), E::Strong);
      f(Expression::cast<UnOp>(e)->_decl, E::Weak);
      break;
    case Expression::E_CALL: {
      auto* c = Expression::cast<Call>(e);
      if (static_cast<Call::CallKind>(e->_secondaryId) >= Call::CK_NARY) {
        vec(static_cast<CallNary*>(c)->_args);
      } else {
        for (unsigned int i = 0; i < c->argCount(); i++) {
          f(c->arg(i), E::Strong);
        }
      }
      f(c->rawDecl(), E::Weak);
    } break;
    case Expression::E_VARDECL: {
      auto* vd = Expression::cast<VarDecl>(e);
      f(vd->ti(), E::Strong);
      f(vd->e(), E::Strong);
      f(vd->_id, E::Weak);
      if (vd->_id != nullptr && vd->_id->_decl != vd) {
        f(vd->_id, E::Strong);  // (a redirected Id, see setIdDecl)
      }
      f(vd->_flat, E::Weak);
    } break;
    case Expression::E_LET:
      vec(Expression::cast<Let>(e)->let().vec());
      vec(Expression::cast<Let>(e)->_letOrig.vec());
      f(Expression::cast<Let>(e)->in(), E::Strong);
      break;
    case Expression::E_TI:
      f(Expression::cast<TypeInst>(e)->domain(), E::Strong);
      vec(Expression::cast<TypeInst>(e)->ranges().vec());
      break;
  }
}

thread_local std::vector<std::pair<const void*, bool>> RC::_oldChildren;

void RC::pushChildren(ASTNode* n) {
  Expression::forEachChild(n, [](const void* c, Expression::Edge edge) {
    _oldChildren.emplace_back(c, edge == Expression::Edge::Strong);
  });
}

void RC::adoptChildren(ASTNode* n) {
  if (n == nullptr) {
    return;
  }
  Expression::forEachChild(n, [](const void* c, Expression::Edge edge) {
    if (edge == Expression::Edge::Weak) {
      incWeak(c);
    } else {
      inc(c);
    }
  });
}

ASTNode* RC::ownDecl(ASTNode* n) {
  auto* id = static_cast<Id*>(n);
  Expression* d = id->_decl;
  if (d != nullptr && d->_id == NID_VARDECL && static_cast<VarDecl*>(d)->_id == id) {
    return d;
  }
  return nullptr;
}

ASTNode* RC::dying(ASTNode* n) {
  ASTNode* vd = n->_id == NID_ID ? ownDecl(n) : nullptr;
  if (vd == nullptr) {
    return n;
  }
  // (a count of 0 means the VarDecl is already dying)
  if (strong(vd) == STRONG_MAX || strong(vd) == 0 || (--vd->_rc & STRONG_MAX) != 0) {
    return nullptr;
  }
  return vd;
}

bool RC::ownIdAlive(ASTNode* n) {
  ASTNode* vd = ownDecl(n);
  return vd != nullptr && strong(vd) != 0;
}

void RC::setIdDecl(ASTNode* n, Expression* d) {
  auto* id = static_cast<Id*>(n);
  // The declaration is counted unless this is an own Id that nothing holds
  ASTNode* oldOwner = ownDecl(id);
  bool counted = strong(id) != 0 || oldOwner == nullptr;
  Expression* old = id->_decl;
  id->_decl = d;
  ASTNode* newOwner = ownDecl(id);
  // An Id only becomes an own Id in the VarDecl constructor, before anything holds the VarDecl
  assert(oldOwner != nullptr || newOwner == nullptr || strong(newOwner) == 0);
  if (oldOwner != nullptr && newOwner == nullptr) {
    // A VarDecl whose own Id is redirected (unification) holds that Id strongly from now on, as
    // the Id no longer lives with it (see forEachChild)
    inc(id);
  }
  if (strong(id) != 0 || newOwner == nullptr) {
    inc(d);
  }
  if (counted) {
    dec(old);
  }
}

thread_local int RC::_releasing = 0;
thread_local std::vector<ASTNode*> RC::_toFree;
thread_local std::vector<ASTNode*> RC::_dead;

void RC::release(ASTNode* n) {
  // Each dying node has strong count 0 while it waits here. Memory is freed only at the end of the
  // outermost cascade, so that no node in the cascade reads memory that is already freed. The work
  // list is reused between releases; a nested release only works on the part above base.
  std::vector<ASTNode*>& dead = _dead;
  size_t base = dead.size();
  dead.push_back(n);
  _releasing++;
  while (dead.size() > base) {
    ASTNode* cur = dead.back();
    dead.pop_back();
    Expression::forEachChild(cur, [&](const void* c, Expression::Edge edge) {
      ASTNode* cn = node(c);
      if (cn == nullptr) {
        return;
      }
      if (edge == Expression::Edge::Weak) {
        decWeak(cn);
      } else if (strong(cn) != STRONG_MAX && strong(cn) != 0) {
        // (a count of 0 means the child is already in this cascade)
        if ((--cn->_rc & STRONG_MAX) == 0) {
          if (ASTNode* d = dying(cn)) {
            dead.push_back(d);
          }
        }
      }
    });
    // Destructors
    if (cur->_id > ASTNode::NID_END && cur->_id <= Expression::EID_END) {
      Expression::ann(static_cast<Expression*>(cur)).forget();
    }
    switch (cur->_id) {
      case NID_VARDECL: {
        // The own Id dies with its VarDecl (nothing holds it, or it would hold the VarDecl)
        Id* id = static_cast<VarDecl*>(cur)->_id;
        if (id != nullptr && strong(id) == 0 && id->_decl == cur) {
          dead.push_back(id);
        }
      } break;
      case NID_ID:
        static_cast<Id*>(cur)->_decl = nullptr;  // (so that ownDecl never reads a freed VarDecl)
        break;
      case Item::II_FUN:
        static_cast<FunctionI*>(cur)->ann().forget();
        break;
      case Item::II_SOL:
        static_cast<SolveI*>(cur)->ann().forget();
        break;
      case Item::II_OUT:
        static_cast<OutputI*>(cur)->ann().forget();
        break;
      default:
        break;
    }
    _toFree.push_back(cur);
  }
  if (--_releasing == 0) {
    if (_toFree.size() > 1) {
      // A node can be queued twice: by the cascade and by its last weak reference
      std::sort(_toFree.begin(), _toFree.end());
      _toFree.erase(std::unique(_toFree.begin(), _toFree.end()), _toFree.end());
    }
    for (auto* f : _toFree) {
      if (f->_rc == 0) {
        node_free(f);  // no weak references left
      }
    }
    _toFree.clear();
  }
}

void RC::reportLeaks(std::ostream& os) {
#ifdef MZN_RC_CHECK
  // A counted node is alive if an immortal node (the Constants table) reaches it. The others leak:
  // either a count was never released, or the node is in a cycle of strong references.
  auto& live = live_nodes();
  auto strongChildren = [&](ASTNode* n, const std::function<void(ASTNode*)>& f) {
    Expression::forEachChild(n, [&](const void* c, Expression::Edge edge) {
      ASTNode* cn = node(c);
      if (edge == Expression::Edge::Strong && cn != nullptr && live.count(cn) != 0) {
        f(cn);
      }
    });
  };
  /// Add every node that the nodes in \a set reach to \a set
  auto reach = [&](std::unordered_set<ASTNode*>& set) {
    std::vector<ASTNode*> todo(set.begin(), set.end());
    while (!todo.empty()) {
      ASTNode* n = todo.back();
      todo.pop_back();
      strongChildren(n, [&](ASTNode* c) {
        if (set.insert(c).second) {
          todo.push_back(c);
        }
      });
    }
  };
  std::unordered_set<ASTNode*> reached;
  for (auto* n : live) {
    if (strong(n) == STRONG_MAX) {
      reached.insert(n);
    }
  }
  reach(reached);
  // Destroyed nodes that weak references keep (count 0) are not leaks, and their children are gone
  std::unordered_map<ASTNode*, uint32_t> leaked;
  for (auto* n : live) {
    if (strong(n) != 0 && reached.count(n) == 0) {
      leaked.emplace(n, 0);
    }
  }
  if (leaked.empty()) {
    return;
  }
  // Counts that come from other leaked nodes. A node with more counts than that is held from
  // outside the leaked set (a count that was never released); the rest are only held by cycles.
  for (auto& it : leaked) {
    strongChildren(it.first, [&](ASTNode* c) {
      auto lc = leaked.find(c);
      if (lc != leaked.end()) {
        lc->second++;
      }
    });
  }
  auto kind = [](ASTNode* n) -> std::string {
    static const char* names[] = {
        "IntLit",   "FloatLit",    "SetLit",      "BoolLit",       "StringLit", "Id",    "AnonVar",
        "ArrayLit", "ArrayAccess", "FieldAccess", "Comprehension", "ITE",       "BinOp", "UnOp",
        "Call",     "VarDecl",     "Let",         "TypeInst",      "TIId"};
    switch (n->_id) {
      case ASTNode::NID_CHUNK:
        return "ASTChunk";
      case ASTNode::NID_VEC:
        return "ASTVec";
      case ASTNode::NID_STR:
        return "ASTString";
      case Item::II_INC:
        return "IncludeI";
      case Item::II_ASN:
        return "AssignI";
      case Item::II_CON:
        return "ConstraintI";
      case Item::II_SOL:
        return "SolveI";
      case Item::II_OUT:
        return "OutputI";
      case Item::II_FUN:
        return "FunctionI";
      default:
        return names[n->_id - Expression::E_INTLIT];
    }
  };
  // Roots: nodes with a count that was never released. Nodes that they reach leak with them.
  // Leaked nodes that no root reaches are held only by cycles.
  std::unordered_set<ASTNode*> roots;
  for (auto& it : leaked) {
    if (strong(it.first) > it.second) {
      roots.insert(it.first);
    }
  }
  std::unordered_set<ASTNode*> fromRoots = roots;
  reach(fromRoots);
  struct Counts {
    size_t roots = 0;
    size_t reached = 0;
    size_t cycles = 0;
  };
  std::map<std::string, Counts> byKind;
  std::map<std::string, ASTNode*> example;
  for (auto& it : leaked) {
    auto& c = byKind[kind(it.first)];
    if (roots.count(it.first) != 0) {
      c.roots++;
      example.emplace(kind(it.first), it.first);
    } else if (fromRoots.count(it.first) != 0) {
      c.reached++;
    } else {
      c.cycles++;
      example.emplace(kind(it.first), it.first);
    }
  }
  os << "RC check: " << leaked.size()
     << " leaked nodes (count never released, reached from those, in cycles)\n";
  for (auto& it : byKind) {
    os << "  " << it.first << ": " << it.second.roots << ", " << it.second.reached << ", "
       << it.second.cycles;
    auto e = example.find(it.first);
    if (e != example.end() && e->second->_id > ASTNode::NID_END &&
        e->second->_id <= Expression::EID_END) {
      os << "  (e.g. " << Expression::loc(static_cast<Expression*>(e->second)).toString() << ")";
    }
    os << "\n";
  }
#else
  (void)os;
#endif
}

}  // namespace MiniZinc
