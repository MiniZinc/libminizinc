/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/eval_par.hh>
#include <minizinc/solver_instance_base.hh>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

namespace MiniZinc {

SolverInstanceBase::Status SolverInstanceBase::solve() { return SolverInstanceError; }

void SolverInstanceBase::reset() { assert(false); }

void SolverInstanceBase::resetWithConstraints(Model::iterator begin, Model::iterator end) {
  assert(false);
}

void SolverInstanceBase::processPermanentConstraints(Model::iterator begin, Model::iterator end) {
  assert(false);
}

void Registry::add(const ASTString name, poster p) { _registry.insert(std::make_pair(name, p)); }
void Registry::add(const std::string& name, poster p) {
  GCLock lock;
  ASTString str(name);
  return add(str, p);
}
void Registry::post(Call* c) {
  auto it = _registry.find(c->id());
  if (it == _registry.end()) {
    std::ostringstream ss;
    ss << "Error: solver backend cannot handle constraint: " << c->id();
    throw InternalError(ss.str());
  }
  it->second(_base, c);
}

void SolverInstanceBase::printSolution() {
  if (_options->printStatistics) {
    printStatistics();  // Insert stats before sol separator
  }
  if (nullptr == _pS2Out) {
    getEnv()->evalOutput(std::cout, std::cerr);  // deprecated
    std::cout << "----------" << std::endl;
  } else {
    getSolns2Out()->evalOutput();
  }
}

template class SolverInstanceBase2<false>;
template class SolverInstanceBase2<true>;

template <bool AsgArray>
void SolverInstanceBase2<AsgArray>::printSolution() {
  GCLock lock;
  assignSolutionToOutput();
  SolverInstanceBase::printSolution();
}

//   void
//   SolverInstanceBase::assignSolutionToOutput() {
//     for (VarDeclIterator it = getEnv()->output()->vardecls().begin(); it !=
//     getEnv()->output()->vardecls().end(); ++it) {
//       if (it->e()->e() == NULL) {
//         it->e()->e(getSolutionValue(it->e()->id()));
//       }
//     }
//   }

template <bool AsgArray>
void SolverInstanceBase2<AsgArray>::assignSolutionToOutput() {
  GCLock lock;

  MZN_ASSERT_HARD_MSG(
      nullptr != _pS2Out,
      "Setup a Solns2Out object to use default solution extraction/reporting procs");

  if (_varsWithOutput.empty()) {
    for (VarDeclIterator it = getEnv()->flat()->vardecls().begin();
         it != getEnv()->flat()->vardecls().end(); ++it) {
      if (!it->removed()) {
        VarDecl* vd = it->e();
        if (!Expression::ann(vd).isEmpty()) {
          if (Expression::ann(vd).containsCall(Constants::constants().ann.output_array.aststr()) ||
              Expression::ann(vd).contains(Constants::constants().ann.output_var)) {
            _varsWithOutput.push_back(vd);
          }
        }
      }
    }
  }

  _pS2Out->declNewOutput();  // Even for empty output decl

  // iterate over set of ids that have an output annotation && obtain their right hand side from the
  // flat model
  for (auto* vd : _varsWithOutput) {
    // std::cout << "DEBUG: Looking at var-decl with output-annotation: " << *vd << std::endl;
    Call* output_array_ann =
        AsgArray ? nullptr
                 : Expression::dynamicCast<Call>(get_annotation(
                       Expression::ann(vd), Constants::constants().ann.output_array.aststr()));
    if (output_array_ann != nullptr) {
      assert(vd->e());

      if (auto* al = Expression::dynamicCast<ArrayLit>(vd->e())) {
        std::vector<Expression*> array_elems;
        ArrayLit& array = *al;
        for (unsigned int j = 0; j < array.size(); j++) {
          if (Id* id = Expression::dynamicCast<Id>(array[j])) {
            // std::cout << "DEBUG: getting solution value from " << *id  << " : " << id->v() <<
            // std::endl;
            array_elems.push_back(getSolutionValue(id));
          } else if (auto* floatLit = Expression::dynamicCast<FloatLit>(array[j])) {
            array_elems.push_back(floatLit);
          } else if (auto* intLit = Expression::dynamicCast<IntLit>(array[j])) {
            array_elems.push_back(intLit);
          } else if (auto* boolLit = Expression::dynamicCast<BoolLit>(array[j])) {
            array_elems.push_back(boolLit);
          } else if (auto* setLit = Expression::dynamicCast<SetLit>(array[j])) {
            array_elems.push_back(setLit);
          } else if (auto* strLit = Expression::dynamicCast<StringLit>(array[j])) {
            array_elems.push_back(strLit);
          } else {
            std::ostringstream oss;
            oss << "Error: array element " << *array[j] << " is not an id nor a literal";
            throw InternalError(oss.str());
          }
        }
        GCLock lock;
        ArrayLit* dims;
        Expression* e = output_array_ann->arg(0);
        if (auto* al = Expression::dynamicCast<ArrayLit>(e)) {
          dims = al;
        } else if (Id* id = Expression::dynamicCast<Id>(e)) {
          dims = Expression::cast<ArrayLit>(id->decl()->e());
        } else {
          throw -1;
        }
        std::vector<std::pair<int, int> > dims_v;
        for (unsigned int i = 0; i < dims->length(); i++) {
          IntSetVal* isv = eval_intset(getEnv()->envi(), (*dims)[i]);
          if (isv->empty()) {
            dims_v.emplace_back(1, 0);
          } else {
            dims_v.emplace_back(static_cast<int>(isv->min().toInt()),
                                static_cast<int>(isv->max().toInt()));
          }
        }
        auto* array_solution = new ArrayLit(Location(), array_elems, dims_v);
        if (array_elems.empty()) {
          Expression::type(array_solution, Type::bot(static_cast<int>(dims_v.size())));
        } else {
          auto t = Expression::type(array_elems.back());
          t.dim(static_cast<int>(dims_v.size()));
          Expression::type(array_solution, t);
        }
        KeepAlive ka(array_solution);
        auto& de = getSolns2Out()->findOutputVar(vd->id()->str());
        de.first->e(array_solution);
      }
    } else {
      Expression* sol = getSolutionValue(vd->id());
      vd->e(sol);
      auto& de = getSolns2Out()->findOutputVar(vd->id()->str());
      de.first->e(sol);
    }
  }
}

void SolverInstanceBase::flattenSearchAnnotations(const Annotation& ann,
                                                  std::vector<Expression*>& out) {
  for (ExpressionSetIter i = ann.begin(); i != ann.end(); ++i) {
    Expression* e = *i;
    if (Expression::isa<Call>(e) && (Expression::cast<Call>(e)->id() == "seq_search" ||
                                     Expression::cast<Call>(e)->id() == "warm_start_array")) {
      Call* c = Expression::cast<Call>(e);
      auto* anns = Expression::cast<ArrayLit>(c->arg(0));
      for (unsigned int i = 0; i < anns->size(); i++) {
        Annotation subann;
        subann.add((*anns)[i]);
        flattenSearchAnnotations(subann, out);
      }
    } else {
      out.push_back(*i);
    }
  }
}

void SolverInstanceBase::flattenMultipleObjectives(const Annotation& ann,
                                                   MultipleObjectives& mo) const {
  int nGoalH = 0;
  for (ExpressionSetIter i = ann.begin(); i != ann.end(); ++i) {
    Expression* e = *i;
    if (Expression::isa<Call>(e) && (Expression::cast<Call>(e)->id() == "goal_hierarchy")) {
      MZN_ASSERT_HARD_MSG(0 == nGoalH++, "Several goal hierarchies provided");
      MZN_ASSERT_HARD_MSG(getEnv()->flat()->solveItem()->st() == SolveI::SolveType::ST_SAT,
                          "goal_hierarchy provided but solve item is not SAT");
      Call* c = Expression::cast<Call>(e);
      auto* anns = Expression::cast<ArrayLit>(c->arg(0));
      for (unsigned int i = 0; i < anns->size(); i++) {
        Annotation subann;
        subann.add((*anns)[i]);
        MultipleObjectives::Objective obj;
        flattenMultObjComponent(subann, obj);
        mo.add(obj);
      }
    }
  }
}

void SolverInstanceBase::flattenMultObjComponent(const Annotation& ann,
                                                 MultipleObjectives::Objective& obj) {
  MZN_ASSERT_HARD(!ann.isEmpty());
  Expression* e = *ann.begin();
  MZN_ASSERT_HARD(Expression::isa<Call>(e));
  Call* c = Expression::cast<Call>(e);
  obj.setVariable(c->arg(0));
  const auto id = c->id();
  if (id == "min_goal" || id == "int_min_goal" || id == "float_min_goal") {
    obj.setWeight(-1.0);
  } else if (id == "sat_goal" || id == "max_goal" || id == "int_max_goal" ||
             id == "float_max_goal") {
    obj.setWeight(1.0);
  } else {
    MZN_ASSERT_HARD_MSG(false, "unknown goal: " << id);
  }
}

}  // namespace MiniZinc
