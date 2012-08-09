/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Pierre WILKE (wilke.pierre@gmail.com)
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/solver_interface/solver_interface.hh>
#include <minizinc/printer.hh>
namespace MiniZinc {
  SolverInterface::~SolverInterface(){
  }
  void SolverInterface::fromFlatZinc(MiniZinc::Model& m){
    for(unsigned int i = 0; i < m._items.size(); i++){
      Item* item = m._items[i];
      if(item->isa<VarDeclI>()){
	addVar(item->cast<VarDeclI>()->_e);
      } else if (item->isa<ConstraintI>()){
	postConstraint(*item->cast<ConstraintI>());
      } else if (item->isa<SolveI>()){	  
	solve(item->cast<SolveI>());
      } else {
	//  std::cerr << "This type of item should not appear in a FlatZinc file"<<item->_iid << std::endl;
	// Printer::getInstance()->print(item);
	//std::exit(-1);
      }
    }
  }
  void SolverInterface::addVar(VarDecl* vd){
    void* solverVar = addSolverVar(vd);
    addVar(vd,solverVar);
  }
  void SolverInterface::addVar(VarDecl* vd, void* ptr){
    variableMap.erase(vd);
    variableMap.insert(std::pair<VarDecl*, void*>(vd,ptr));
  }
  void SolverInterface::postConstraint(ConstraintI& constraint){
    Call* c = constraint._e->cast<Call>();
    std::string con_id = c->_id.str();
    std::map<std::string, poster>::iterator it = constraintMap.find(con_id);
    if(it == constraintMap.end()){
      std::cerr << "Error : couldn't find constraint " << con_id 
		<< " in constraints map." << std::endl;
      Printer::getInstance()->print(&constraint);
      std::exit(-1);
    }
    it->second(*this,c);
  }
  void* SolverInterface::lookupVar(VarDecl* vd){
    std::map<VarDecl*,void*>::iterator it;
    it = variableMap.find(vd);
    if(it != variableMap.end()){
      return it->second;
    } else {
      std::cerr << "Error : couldn't find variable " << vd->_id.str()
		<< " in constraints map." << std::endl;
      throw -1;	    
    }
  }
  void* SolverInterface::lookupVar(std::string s){
    std::map<VarDecl*,void*>::iterator it;
    for(it = variableMap.begin(); it != variableMap.end(); it++){
      if(it->first->_id.str() == s){
	return it->second;
      }
    }
    std::cerr << "Error : couldn't find variable " << s
	      << " in constraints map." << std::endl;
    throw -1;	    
  }

  SolverInterface::poster SolverInterface::lookupConstraint(std::string& s){
    std::map<std::string, SolverInterface::poster>::iterator it 
      = constraintMap.find(s);
    if(it == constraintMap.end()){
      std::cerr << "Error : couldn't find constraint " << s
		<< " in constraints map." << std::endl;
      throw -1;
    } else return it->second;
  }

  void SolverInterface::addConstraintMapping(std::string mzn_constraint,
					     SolverInterface::poster func){
    constraintMap[mzn_constraint] = func;
  }
};
