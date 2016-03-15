/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip@dekker.li>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
//

#include <minizinc/presolve_utils.hh>
#include <fstream>
#include <minizinc/astiterator.hh>

namespace MiniZinc{

  void recursiveRegisterFns(Model* model, EnvI& env, FunctionI* fn) {
//    TODO: Can't use Evisitor because of const in parameter.
    class RegisterCalls {
    public:
      Model* model1;
      EnvI& env1;

      RegisterCalls(Model* _model, EnvI& _env) : model1(_model), env1(_env) { }

      /// Visit integer literal
      void vIntLit(const IntLit&) {}
      /// Visit floating point literal
      void vFloatLit(const FloatLit&) {}
      /// Visit Boolean literal
      void vBoolLit(const BoolLit&) {}
      /// Visit set literal
      void vSetLit(const SetLit&) {}
      /// Visit string literal
      void vStringLit(const StringLit&) {}
      /// Visit identifier
      void vId(const Id&) {}
      /// Visit anonymous variable
      void vAnonVar(const AnonVar&) {}
      /// Visit array literal
      void vArrayLit(const ArrayLit&) {}
      /// Visit array access
      void vArrayAccess(const ArrayAccess&) {}
      /// Visit array comprehension
      void vComprehension(const Comprehension&) {}
      /// Visit array comprehension (only generator \a gen_i)
      void vComprehensionGenerator(const Comprehension&, int gen_i) { (void) gen_i; }
      /// Visit if-then-else
      void vITE(const ITE&) {}
      /// Visit binary operator
      void vBinOp(const BinOp&) {}
      /// Visit unary operator
      void vUnOp(const UnOp&) {}
      /// Visit call
      void vCall(Call& call) {
        if (call.decl() && (model1->matchFn(env1, &call) == nullptr) ) {
          model1->registerFn(env1, call.decl());
          RegisterCalls rc(model1, env1);
          TopDownIterator<RegisterCalls> tdi(rc);
          tdi.run(call.decl()->e());
        }
      }
      /// Visit let
      void vLet(const Let&) {}
      /// Visit variable declaration
      void vVarDecl(const VarDecl&) {}
      /// Visit type inst
      void vTypeInst(const TypeInst&) {}
      /// Visit TIId
      void vTIId(const TIId&) {}
      /// Determine whether to enter node
      bool enter(Expression* e) { return true; }
      /// Exit node after processing has finished
      void exit(Expression* e) {}
    } rc(model, env);

    model->registerFn(env, fn);
    TopDownIterator<RegisterCalls> tdi(rc);
    tdi.run(fn->e());
  }

  Expression* computeDomainExpr(EnvI& env, Expression* exp) {
//    TODO: Should we compute the unions manually?
    if (exp->eid() == Expression::E_ID )
      return exp->cast<Id>()->decl()->ti()->domain();
    Expression* type = nullptr;
    if(exp->type().dim() > 0) {
      switch (exp->eid()) {
        case Expression::E_ARRAYLIT: {
          ArrayLit* arr = exp->cast<ArrayLit>();
          if (arr->length() > 0) {
            type = computeDomainExpr(env, arr->v()[0]);
            if (!type) return nullptr;
            for (int i = 1; i < arr->length(); ++i) {
              Expression* type2 = computeDomainExpr(env, arr->v()[i]);
              if(!type2) return nullptr;
              type = new BinOp(Location(), type, BOT_UNION, type2);
              type->type( type2->type() );
            }
          }
          return type;
        }
        case Expression::E_CALL: {
          Call* c = exp->cast<Call>();
          if ( c->id().str().find("array")  ) {
            return c->args()[c->args().size() == 1 ? 0 : 1]->cast<ArrayLit>();
          } else {
//            TODO: Are there more cases in which we can compute this?
            return nullptr;
          }
        }
        case Expression::E_BINOP: {
          BinOp* op = exp->cast<BinOp>();
          if (op->op() == BinOpType::BOT_PLUSPLUS) {
            Expression* left = computeDomainExpr(env, op->lhs());
            Expression* right = computeDomainExpr(env, op->rhs());
            if (!left || !right) return nullptr;
            type = new BinOp(Location(), left, BOT_UNION, right);
            type->type( left->type() );
          } else {
            return nullptr;
          }
        }
        case Expression::E_COMP: {
          Comprehension* comp = exp->cast<Comprehension>();
          return computeDomainExpr(env, comp->e());
        }
        default: {
//          TODO: Print expression for error
          throw InternalError("Presolver couldn't determine counds of expression");
        }
      }
    }
//    TODO: How about booleans?
    switch (exp->type().bt()) {
      case Type::BT_INT: {
        IntBounds ib = compute_int_bounds(env, exp);
        if (ib.valid) {
          type = new SetLit( Location(), IntSetVal::a(ib.l, ib.u) );
          type->type(Type::parsetint());
          return type;
        }
        break;
      }
      case Type::BT_FLOAT: {
        FloatBounds ib = compute_float_bounds(env, exp);
        if (ib.valid) {
          type = new SetLit(Location(), IntSetVal::a(ib.l, ib.u));
          type->type(Type::parsetfloat());
          return type;
        }
        break;
      }
      default:
        return nullptr;
    }
    return nullptr;
  }

  void generateFlatZinc(Env& env, bool rangeDomains, bool optimizeFZN, bool newFZN) {
//    TODO: Should this be integrated in Flattener?
    FlatteningOptions fopts;
    fopts.onlyRangeDomains = rangeDomains;
    flatten(env, fopts);

    if(optimizeFZN)
      optimize(env);

    if (!newFZN) {
      oldflatzinc(env);
    } else {
      env.flat()->compact();
      env.output()->compact();
    }
  }

  bool Presolver::Solns2Vector::evalOutput() {
    GCLock lock;

    std::unordered_map<std::string, Expression*>* solution = new std::unordered_map<std::string, Expression*>();
    for (unsigned int i = 0; i < getModel()->size(); i++) {
      if (VarDeclI* vdi = (*getModel())[i]->dyn_cast<VarDeclI>()) {
        Expression* cpi = copy(copyEnv, vdi->e()->e());
        solution->insert(std::pair<std::string, Expression*>(vdi->e()->id()->str().str(), cpi));
        GCProhibitors.emplace_back(cpi);
      }
    }
    solutions.push_back(solution);
    return true;
  }

  void Presolver::TableBuilder::buildFromSolver(FunctionI* f, Solns2Vector* solns,
                                                ASTExprVec<Expression> variables) {
    rows = static_cast<long long int>( solns->getSolutions().size());

    if (variables.size() == 0) {
      for (auto it = f->params().begin(); it != f->params().end(); ++it) {
        Expression* id = new Id(Location(), (*it)->id()->str(), (*it));
        id->type((*it)->type());
        addVariable(id);
      }
    } else {
      for (auto it = variables.begin(); it != variables.end(); ++it) {
        addVariable(*it);
      }
    }

    for (int i = 0; i < solns->getSolutions().size(); ++i) {
      auto sol = solns->getSolutions()[i];
      for (auto it = f->params().begin(); it != f->params().end(); ++it) {
        Expression* exp = sol->at((*it)->id()->str().str());
        exp->type(exp->type().bt() == Type::BT_BOOL ? Type::parbool((*it)->type().dim()) : Type::parint(
                (*it)->type().dim()));
        addData(exp);
      }
    }
  }

  Call* Presolver::TableBuilder::getExpression() {
    storeVars();
    ArrayLit* dataExpr = new ArrayLit(Location(), data);
    dataExpr->type(boolTable ? Type::parbool(1) : Type::parint(1));

    std::vector<Expression*> conversionArgs;
    SetLit* index1 = new SetLit(Location(),
                                IntSetVal::a(
                                        std::vector<IntSetVal::Range>(1, IntSetVal::Range(IntVal(1), IntVal(rows)))
                                ));
    index1->type(Type::parsetint());
    conversionArgs.push_back(index1);
    Call* index2 = new Call(Location(), "index_set", std::vector<Expression*>(1, variables));
    index2->type(Type::parsetint());
    index2->decl(m->matchFn(env, index2));
    conversionArgs.push_back(index2);
    conversionArgs.push_back(dataExpr);
    Call* tableData = new Call(Location(), "array2d", conversionArgs);
    tableData->type(boolTable ? Type::parbool(2) : Type::parint(2));
    tableData->decl(m->matchFn(env, tableData));

    std::vector<Expression*> tableArgs;
    tableArgs.push_back(variables);
    tableArgs.push_back(tableData);

    Call* tableCall = new Call(Location(), boolTable ? "table_bool" : "table_int", tableArgs);
    FunctionI* tableDecl = m->matchFn(env, tableCall);
    if (tableDecl == nullptr) {
      registerTableConstraint();
      tableDecl = m->matchFn(env, tableCall);
      assert(tableDecl != nullptr);
    }
    tableCall->decl(tableDecl);
    tableCall->type(Type::varbool());

    return tableCall;
  }

  void Presolver::TableBuilder::addVariable(Expression* var) {
    if (var->type().dim() > 1) {
      Call* c = new Call(Location(), "array1d", std::vector<Expression*>(1, var));
      c->type(var->type().bt() == Type::BT_BOOL ? Type::varbool(1) : Type::varint(1));
      c->decl(m->matchFn(env, c));
      var = c;
    }

    if (var->type().bt() == Type::BT_BOOL && !boolTable) {
      Call* c = new Call(Location(), "bool2int", std::vector<Expression*>(1, var));
      c->type(Type::varint(var->type().dim()));
      c->decl(m->matchFn(env, c));
      var = c;
    }

    if (var->type().dim() > 0) {
      storeVars();
      if (variables == nullptr) {
        variables = var;
      } else {
        variables = new BinOp(Location(), variables, BOT_PLUSPLUS, var);
        variables->type(boolTable ? Type::varbool(1) : Type::varint(1));
      }
    } else {
      vVariables.push_back(var);
    }

  }

  void Presolver::TableBuilder::addData(Expression* dat) {
    if (dat->type().dim() > 0) {
      ArrayLit* arr = nullptr;
      if (dat->eid() == Expression::E_CALL) {
        Call* c = dat->cast<Call>();
        arr = c->args()[c->args().size() - 1]->cast<ArrayLit>();
      } else if (dat->eid() == Expression::E_ARRAYLIT) {
        ArrayLit* arr = dat->cast<ArrayLit>();
      }
      assert(arr != nullptr);

      for (auto it = arr->v().begin(); it != arr->v().end(); ++it) {
        data.push_back(*it);
      }
    } else {
      data.push_back(dat);
    }
  }

  void Presolver::TableBuilder::storeVars() {
    if (vVariables.empty())
      return;

    if (variables == nullptr) {
      variables = new ArrayLit(Location(), vVariables);
    } else {
      Expression* arr = new ArrayLit(Location(), vVariables);
      arr->type(boolTable ? Type::varbool(1) : Type::varint(1));
      variables = new BinOp(Location(), variables, BOT_PLUSPLUS, arr);
    }
    variables->type(boolTable ? Type::varbool(1) : Type::varint(1));
    vVariables.clear();
  }

  void Presolver::TableBuilder::registerTableConstraint() {
    GCLock lock;

    std::string loc = boolTable ? "/table_bool.mzn" : "/table_int.mzn";
    std::ifstream file(options.stdLibDir + "/" + options.globalsDir + loc);
    bool exists = file.is_open();
    file.close();

    Model* table_model = nullptr;
    if (exists) {
      table_model = parse(std::vector<std::string>(1, options.stdLibDir + "/" + options.globalsDir + loc),
                          std::vector<std::string>(), options.includePaths, false, false,
                          false, std::cerr);
    } else {
      table_model = parse(std::vector<std::string>(1, options.stdLibDir + "/std" + loc),
                          std::vector<std::string>(), options.includePaths, false, false,
                          false, std::cerr);
    }

    Env table_env(table_model);

    std::vector<TypeError> typeErrors;
    typecheck(table_env, table_model, typeErrors, false);
    assert(typeErrors.size() == 0);

    registerBuiltins(table_env, table_model);

    class RegisterTable : public ItemVisitor {
    public:
      EnvI& env;
      Model* model;

      RegisterTable(EnvI& env, Model* model) : env(env), model(model) { }

      void vFunctionI(FunctionI* i) {
        if (i->id().str() == "table_int" || i->id().str() == "table_bool") {
          FunctionI* ci = copy(env, i, false, true, false)->cast<FunctionI>();
          model->addItem(ci);
          model->registerFn(env, ci);
        }
      }
    } rt(env, m);
    iterItems(rt, table_model);
  }
}