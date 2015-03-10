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
  SolverInstanceBase::solve(void) { return SolverInstance::ERROR; } 
  
  void
  SolverInstanceBase::Registry::add(const ASTString& name, poster p) {
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
  
  bool 
  NISolverInstanceBase::postConstraints(std::vector<Call*> cts) {
    // the constraints are already added to the flat model
    return true;
  }
  
  bool 
  NISolverInstanceBase::addVariables(std::vector<VarDecl*> vars) {
    // the variables are already added to the flat model
    return true;
  }
  
  std::vector<Call*> 
  NISolverInstanceBase::deriveNoGoodsFromSolution(void) {
    Model* output = env().output();    
    std::vector<BinOp*> disequalities;
    // collect the solution assignments from the output model
    for(VarDeclIterator it = output->begin_vardecls(); it!=output->end_vardecls(); ++it) {
      Id* id = it->e()->id();
      if(!it->e()->e()) {
        std::stringstream ssm;
        ssm << "NISolverInstanceBase::deriveNoGoodsFromSolution: no solution assigned to \"" << *id << "\" in the output model.";
        throw InternalError(ssm.str());
      }      
      Expression* e = eval_par(env().envi(), it->e()->e());
      // create the constraints: x_i != sol_i
      BinOp* bo = new BinOp(Location(), id, BOT_NQ, e);
      disequalities.push_back(bo);
    }
    // TODO: create the constraints: \/_i { x_i != sol_i }   
    // TODO: wrap into KeepAlive!!
    std::vector<Call*> nogoods;
    return nogoods;
  }
  
  SolverInstance::Status
  NISolverInstanceBase::next(void) {
    if(_new_solution)
      postSolutionNoGoods();
    // the variables and constraints to be posted are already added to the flat model during flattening
    Status status = nextSolution();  
    if(status == Status::SAT)
      _new_solution = true;
    else _new_solution = false;
  }
  
  void
  NISolverInstanceBase::postSolutionNoGoods(void) {
    // TODO: obtain solution from output model
    // TODO: derive nogoods from solution
    // TODO: add nogoods to the flat model
  }
  
}
