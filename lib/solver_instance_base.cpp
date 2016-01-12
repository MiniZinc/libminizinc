/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/solver_instance_base.hh>
#include <minizinc/flatten_internal.hh>

namespace MiniZinc {

  SolverInstanceBase::Status

  SolverInstanceBase::solve(void) { return SolverInstance::FAILURE; } 
  
  void
  SolverInstanceBase::Registry::add(const ASTString& name, poster p) {
    _keepAlive->addItem(new ConstraintI(Location(),new StringLit(Location(),name)));
    _registry.insert(std::make_pair(name, p));
  }
  void
  SolverInstanceBase::Registry::post(Call* c) {
    ASTStringMap<poster>::t::iterator it = _registry.find(c->id());
    if (it == _registry.end()) {
      std::cerr << "Error: constraint not found: " << c->id() << "\n";
      exit(EXIT_FAILURE);
    }
    it->second(_base, c);
  }
  
  void
  SolverInstanceBase::assignSolutionToOutput(void) {   
    for (VarDeclIterator it = _env.output()->begin_vardecls(); it != _env.output()->end_vardecls(); ++it) {       
      if (it->e()->e() == NULL) {
        it->e()->e(getSolutionValue(it->e()->id()));          
      }       
    }
  }
  
  void 
  SolverInstanceBase::flattenSearchAnnotations(const Annotation& ann, std::vector<Expression*>& out) {
    for(ExpressionSetIter i = ann.begin(); i != ann.end(); ++i) {
        Expression* e = *i;
        if(e->isa<Call>() && e->cast<Call>()->id().str() == "seq_search") {
            Call* c = e->cast<Call>();
            ArrayLit* anns = c->args()[0]->cast<ArrayLit>();
            for(unsigned int i=0; i<anns->v().size(); i++) {
                Annotation subann;
                subann.add(anns->v()[i]);
                flattenSearchAnnotations(subann, out);
            }
        }
        else if(e->isa<Call>() && e->cast<Call>()->id() == constants().ann.combinator) {
          continue; // don't collect the search combinator annotation          
        } else {
            out.push_back(*i);
        }
    }
  }
  
  bool
  NISolverInstanceBase::updateIntBounds(VarDecl* vd, int lb, int ub) {
    // the bounds have already been updated in the flat model
    return true;
  }
  
  bool 
  NISolverInstanceBase::postConstraints(std::vector<Call*> cts) {
    // the constraints are already added to the flat model
    return true;
  }
  
  bool 
  NISolverInstanceBase::addVariables(const std::vector<VarDecl*>& vars) {
    // the variables are already added to the flat model
    return true;
  }
  
  KeepAlive 
  NISolverInstanceBase::deriveNoGoodsFromSolution(void) {
    Model* flat = env().flat();
    GCLock lock;
    std::vector<Expression*> disequalities;
    for (VarDeclIterator it = flat->begin_vardecls(); it!=flat->end_vardecls(); ++it) {
      if (it->e()->ann().contains(constants().ann.output_var)) {
        Id* id = it->e()->id();
        Expression* sv = getSolutionValue(it->e()->id());
        Expression* e = eval_par(env().envi(),sv);
        // create the constraints: x_i != sol_i
        BinOp* bo = new BinOp(Location(), id, BOT_NQ, e);
        bo->type(Type::varbool());
        disequalities.push_back(bo);
      }
    }
    ArrayLit* array = new ArrayLit(Location(), disequalities);
    array->type(Type::varbool(1));
    std::vector<Expression*> args; 
    args.push_back(array);
    Call* disjunction = new Call(Location(), constants().ids.exists, args);
    disjunction->type(Type::varbool());
    disjunction->decl(env().model()->matchFn(env().envi(), disjunction));
    KeepAlive ka(disjunction);
    return ka;
  }
  
  SolverInstance::Status
  NISolverInstanceBase::next(void) {   
    if(_new_solution) {    
      // the variables and constraints to be posted are already added to the flat model during flattening
      postSolutionNoGoods();
    }    
    Status status = nextSolution();  
    if(status == Status::SUCCESS) {
      _new_solution = true;      
      assignSolutionToOutput();       
    }
    else _new_solution = false;
    return status;
  }
  
  void
  NISolverInstanceBase::postSolutionNoGoods(void) {
    KeepAlive nogoods = deriveNoGoodsFromSolution();    
    // flatten the nogoods, which adds it to the model
    FlatteningOptions fopt; 
    fopt.keepOutputInFzn = true;
    (void) flatten(env().envi(), nogoods(), constants().var_true, constants().var_true, fopt); 
    // convert to old flatzinc
    oldflatzinc(env());
  }
  
}
