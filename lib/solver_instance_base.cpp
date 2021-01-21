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
  std::ostringstream oss;

  if (_options->printStatistics) {
    printStatistics();  // Insert stats before sol separator
  }
  if (nullptr == _pS2Out) {
    getEnv()->evalOutput(std::cout, std::cerr);  // deprecated
    std::cout << oss.str();
    if ((!oss.str().empty()) && '\n' != oss.str().back()) {
      std::cout << '\n';
    }
    std::cout << "----------" << std::endl;
  } else {
    getSolns2Out()->evalOutput(oss.str());
  }
}

void SolverInstanceBase2::printSolution() {
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

void SolverInstanceBase2::assignSolutionToOutput() {
  GCLock lock;

  MZN_ASSERT_HARD_MSG(
      nullptr != _pS2Out,
      "Setup a Solns2Out object to use default solution extraction/reporting procs");

  if (_varsWithOutput.empty()) {
    for (VarDeclIterator it = getEnv()->flat()->vardecls().begin();
         it != getEnv()->flat()->vardecls().end(); ++it) {
      if (!it->removed()) {
        VarDecl* vd = it->e();
        if (!vd->ann().isEmpty()) {
          if (vd->ann().containsCall(constants().ann.output_array.aststr()) ||
              vd->ann().contains(constants().ann.output_var)) {
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
    if (Call* output_array_ann = Expression::dynamicCast<Call>(
            get_annotation(vd->ann(), constants().ann.output_array.aststr()))) {
      assert(vd->e());

      if (auto* al = vd->e()->dynamicCast<ArrayLit>()) {
        std::vector<Expression*> array_elems;
        ArrayLit& array = *al;
        for (unsigned int j = 0; j < array.size(); j++) {
          if (Id* id = array[j]->dynamicCast<Id>()) {
            // std::cout << "DEBUG: getting solution value from " << *id  << " : " << id->v() <<
            // std::endl;
            array_elems.push_back(getSolutionValue(id));
          } else if (auto* floatLit = array[j]->dynamicCast<FloatLit>()) {
            array_elems.push_back(floatLit);
          } else if (auto* intLit = array[j]->dynamicCast<IntLit>()) {
            array_elems.push_back(intLit);
          } else if (auto* boolLit = array[j]->dynamicCast<BoolLit>()) {
            array_elems.push_back(boolLit);
          } else if (auto* setLit = array[j]->dynamicCast<SetLit>()) {
            array_elems.push_back(setLit);
          } else if (auto* strLit = array[j]->dynamicCast<StringLit>()) {
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
        if (auto* al = e->dynamicCast<ArrayLit>()) {
          dims = al;
        } else if (Id* id = e->dynamicCast<Id>()) {
          dims = id->decl()->e()->cast<ArrayLit>();
        } else {
          throw -1;
        }
        std::vector<std::pair<int, int> > dims_v;
        for (int i = 0; i < dims->length(); i++) {
          IntSetVal* isv = eval_intset(getEnv()->envi(), (*dims)[i]);
          if (isv->size() == 0) {
            dims_v.emplace_back(1, 0);
          } else {
            dims_v.emplace_back(static_cast<int>(isv->min().toInt()),
                                static_cast<int>(isv->max().toInt()));
          }
        }
        auto* array_solution = new ArrayLit(Location(), array_elems, dims_v);
        KeepAlive ka(array_solution);
        auto& de = getSolns2Out()->findOutputVar(vd->id()->str());
        de.first->e(array_solution);
      }
    } else if (vd->ann().contains(constants().ann.output_var)) {
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
    if (e->isa<Call>() &&
        (e->cast<Call>()->id() == "seq_search" || e->cast<Call>()->id() == "warm_start_array")) {
      Call* c = e->cast<Call>();
      auto* anns = c->arg(0)->cast<ArrayLit>();
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
    MZN_ASSERT_HARD_MSG(0 == nGoalH++, "Several goal hierarchies provided");
    Expression* e = *i;
    if (e->isa<Call>() && (e->cast<Call>()->id() == "goal_hierarchy")) {
      MZN_ASSERT_HARD_MSG(getEnv()->flat()->solveItem()->st() == SolveI::SolveType::ST_SAT,
                          "goal_hierarchy provided but solve item is not SAT");
      Call* c = e->cast<Call>();
      auto* anns = c->arg(0)->cast<ArrayLit>();
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
  MZN_ASSERT_HARD(e->isa<Call>());
  Call* c = e->cast<Call>();
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
