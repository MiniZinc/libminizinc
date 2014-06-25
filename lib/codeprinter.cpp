/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <sstream>
#include <limits>
#include <iomanip>
#include <minizinc/codeprinter.hh>
#include <minizinc/model.hh>
#include <minizinc/astiterator.hh>
#include <minizinc/prettyprinter.hh>

namespace MiniZinc {

  CodePrinter::CodePrinter(std::ostream& os) : _os(os), _icount(0), _ecount(0), _acount(0) {}
  
  void
  CodePrinter::print(ASTString& s) {
    _os << s;
  }
  
  int
  CodePrinter::print(Expression* e) {
    int ret;
    ExpressionMap<int>::iterator it = _emap.find(e);
    if (it != _emap.end())
      return it->second;

    switch (e->eid()) {
      case Expression::E_INTLIT:
      {
        ret = _ecount++;
        _os << "  Expression* ex"<<ret<<" = new IntLit(Location(), ";
        _os << e->cast<IntLit>()->v() << ");\n";
        break;
      }
      case Expression::E_FLOATLIT:
      {
        ret = _ecount++;
        _os << "  Expression* ex"<<ret<<" = new FloatLit(Location(), ";
        _os << e->cast<FloatLit>()->v() << ");\n";
        break;
      }
      case Expression::E_SETLIT:
      {
        const SetLit* sl = e->cast<SetLit>();
        if (sl->isv()) {
          IntSetVal* isv = sl->isv();
          ret = _ecount++;
          _os << "  Expression* ex"<<ret<<";\n";
          _os << "  {\n";
          _os << "    std::vector<Range> ranges;\n";
          for (unsigned int i=0; i<isv->size(); i++) {
            _os << "    ranges.push_back(Range("<<isv->min(i)<<","<<isv->max(i)<<"));\n";
          }
          _os << "    ex"<<ret<<" = new SetLit(Location(), new IntSetVal(ranges));\n";
          _os << "  }\n";
        } else {
          std::vector<int> elems(sl->v().size());
          for (unsigned int i=0; i<sl->v().size(); i++) {
            elems[i] = print(sl->v()[i]);
          }
          ret = _ecount++;
          _os << "  Expression* ex"<<ret<<";\n";
          _os << "  {\n";
          _os << "    std::vector<Expression*> elems;\n";
          for (unsigned int i=0; i<elems.size(); i++) {
            _os << "    elems.push_back(ex"<<elems[i]<<");\n";
          }
          _os << "    ex"<<ret<<" = new SetLit(Location(), elems);\n";
          _os << "  }\n";
        }
        break;
      }
      case Expression::E_BOOLLIT:
      {
        ret = _ecount++;
        _os << "  Expression* ex"<<ret<<" = constants().boollit("<<e->cast<BoolLit>()->v()<<");\n";
        break;
      }
      case Expression::E_STRINGLIT:
      {
        ret = _ecount++;
        _os << "  Expression* ex"<<ret<<" = new StringLit(Location(), \"";
        _os << escapeStringLit(e->cast<StringLit>()->v()) << "\");\n";
        break;
      }
      case Expression::E_ID:
      {
        ret = _ecount++;
        const Id* id = e->cast<Id>();
        if (id->idn() != -1) {
          _os << "  Expression* ex"<<ret<<" = new Id(Location(), "<<id->idn()<<", NULL);\n";
        } else {
          _os << "  Expression* ex"<<ret<<" = new Id(Location(), \""<<id->v()<<"\", NULL);\n";
        }
        break;
      }
      case Expression::E_ANON:
      {
        ret = _ecount++;
        _os << "  Expression* ex"<<ret<<" = new AnonVar(Location());\n";
        break;
      }
      case Expression::E_ARRAYLIT:
      {
        ArrayLit* al = e->cast<ArrayLit>();
        std::vector<int> elems(al->v().size());
        for (unsigned int i=0; i<al->v().size(); i++) {
          elems[i] = print(al->v()[i]);
        }
        ret = _ecount++;
        _os << "  Expression* ex"<<ret<<";\n";
        _os << "  {\n";
        _os << "    std::vector<Expression*> elems;\n";
        for (unsigned int i=0; i<elems.size(); i++) {
          _os << "    elems.push_back(ex"<<elems[i]<<");\n";
        }
        _os << "    std::vector<std::pair<int,int> > dims;\n";
        for (unsigned int i=0; i<al->dims(); i++) {
          _os << "    dims.push_back(std::pair<int,int>("<<al->min(i)<<","<<al->max(i)<<"));\n";
        }
        _os << "    ex"<<ret<<" = new ArrayLit(Location(), elems, dims);\n";
        _os << "  }\n";
        break;
      }
      case Expression::E_ARRAYACCESS:
      {
        ArrayAccess* aa = e->cast<ArrayAccess>();
        int ex = print(aa->v());
        std::vector<int> idx(aa->idx().size());
        for (unsigned int i=0; i<aa->idx().size(); i++) {
          idx[i] = print(aa->idx()[i]);
        }
        ret = _ecount++;
        _os << "  Expression* ex"<<ret<<";\n";
        _os << "  {\n";
        _os << "    std::vector<Expression*> idx;\n";
        for (unsigned int i=0; i<idx.size(); i++) {
          _os << "    idx.push_back(ex"<<idx[i]<<");\n";
        }
        _os << "    ex"<<ret<<" = new ArrayAccess(Location(), ex"<<ex<<", idx);\n";
        _os << "  }\n";
        break;
      }
      case Expression::E_COMP:
      {
        Comprehension* comp = e->cast<Comprehension>();
        int ex = print(comp->e());
        std::vector<std::vector<int> > vardecls;
        std::vector<int> in_exps;
        int where = comp->where()==NULL ? -1 : print(comp->where());
        for (unsigned int i=0; i<comp->n_generators(); i++) {
          vardecls.push_back(std::vector<int>());
          for (unsigned int j=0; j<comp->n_decls(i); j++) {
            vardecls[i].push_back(print(comp->decl(i,j)));
          }
          in_exps.push_back(print(comp->in(i)));
        }
        ret = _ecount++;
        _os << "  Expression* ex"<<ret<<";\n";
        _os << "  {\n";
        _os << "    Generators g;\n";
        for (unsigned int i=0; i<vardecls.size(); i++) {
          _os << "    std::vector<VarDecl*> v"<<i<<";\n";
          for (unsigned int j=0; j<vardecls[i].size(); j++) {
            _os << "    v"<<i<<".push_back(ex"<<vardecls[i][j]<<"->cast<VarDecl>());\n";
          }
          _os << "    g._g.push_back(Generator(v"<<i<<", ex"<<in_exps[i]<<"));\n";
        }
        if (where != -1) {
          _os << "    g._w = ex"<<where<<";\n";
        }
        _os << "    ex"<<ret<<" = new Comprehension(Location(), ex"<<ex<<", g, "<<comp->set()<<");\n";
        _os << "  }\n";
        break;
      }
      case Expression::E_ITE:
      {
        ITE* ite = e->cast<ITE>();
        std::vector<int> exps;
        for (unsigned int i=0; i<ite->size(); i++) {
          exps.push_back(print(ite->e_if(i)));
          exps.push_back(print(ite->e_then(i)));
        }
        int e_else = print(ite->e_else());
        ret = _ecount++;
        _os << "  Expression* ex"<<ret<<";\n";
        _os << "  {\n";
        _os << "    std::vector<Expression*> e_if_then;\n";
        for (unsigned int i=0; i<exps.size(); i++) {
          _os << "    e_if_then.push_back(ex"<<exps[i]<<");\n";
        }
        _os << "    ex"<<ret<<" = new ITE(Location(), e_if_then, ex"<<e_else<<");\n";
        _os << "  }\n";
        break;
      }
      case Expression::E_BINOP:
      {
        BinOp* bo = e->cast<BinOp>();
        int lhs = print(bo->lhs());
        int rhs = print(bo->rhs());
        ret = _ecount++;
        _os << "  Expression* ex"<<ret<<" = new BinOp(Location(),";
        _os << "ex"<<lhs<<", static_cast<BinOpType>("<<bo->op()<<"), ex"<<rhs<<");\n";
        break;
      }
      case Expression::E_UNOP:
      {
        UnOp* uo = e->cast<UnOp>();
        int ex = print(uo->e());
        ret = _ecount++;
        _os << "  Expression* ex"<<ret<<" = new UnOp(Location(),";
        _os << "static_cast<UnOpType>("<<uo->op()<<"), ex"<<ex<<");\n";
        break;
      }
      case Expression::E_CALL:
      {
        Call* c = e->cast<Call>();
        std::vector<int> args(c->args().size());
        for (unsigned int i=0; i<args.size(); i++) {
          args[i] = print(c->args()[i]);
        }
        ret = _ecount++;
        _os << "  Expression* ex"<<ret<<";\n";
        _os << "  {\n";
        _os << "    std::vector<Expression*> args;\n";
        for (unsigned int i=0; i<args.size(); i++) {
          _os << "    args.push_back(ex"<<args[i]<<");\n";
        }
        _os << "    ex"<<ret<<" = new Call(Location(), \"";
        _os << c->id() << "\", args);\n";
        _os << "  }\n";
        break;
      }
      case Expression::E_VARDECL:
      {
        VarDecl* vd = e->cast<VarDecl>();
        int ti = print(vd->ti());
        int id = print(vd->id());
        int ex = vd->e() == NULL ? -1 : print(vd->e());
        ret = _ecount++;
        _os << "  Expression* ex"<<ret<<" = new VarDecl(Location(),";
        _os << "ex"<<ti<<"->cast<TypeInst>(), ex"<<id<<"->cast<Id>()";
        if (ex!=-1) _os << ", ex"<<ex;
        _os << ");\n";
        break;
      }
      case Expression::E_LET:
      {
        Let* let = e->cast<Let>();
        std::vector<int> exps(let->let().size());
        for (unsigned int i=0; i<exps.size(); i++) {
          exps[i] = print(let->let()[i]);
        }
        int in = print(let->in());
        ret = _ecount++;
        _os << "  Expression* ex"<<ret<<";\n";
        _os << "  {\n";
        _os << "    std::vector<Expression*> exps;\n";
        for (unsigned int i=0; i<exps.size(); i++) {
          _os << "    exps.push_back(ex"<<exps[i]<<");\n";
        }
        _os << "    ex"<<ret<<" = new Let(Location(), exps, ex"<<in<<");\n";
        _os << "  }\n";
        break;
      }
      case Expression::E_TI:
      {
        TypeInst* ti = e->cast<TypeInst>();
        int dom = ti->domain()==NULL ? -1 : print(ti->domain());
        std::vector<int> ranges(ti->ranges().size());
        for (unsigned int i=0; i<ti->ranges().size(); i++) {
          ranges[i] = print(ti->ranges()[i]);
        }
        ret = _ecount++;
        if (ranges.size()==0) {
          _os << "  Expression* ex"<<ret<<" = new TypeInst(Location(), ";
          _os << "Type::fromInt("<<ti->type().toInt()<<"), ";
          if (dom==-1) {
            _os << "NULL";
          } else {
            _os << "ex"<<dom;
          }
          _os << ");\n";
        } else {
          _os << "  Expression* ex"<<ret<<";\n";
          _os << "  {\n";
          _os << "    std::vector<TypeInst*> ranges;\n";
          for (unsigned int i=0; i<ranges.size(); i++) {
            _os << "    ranges.push_back(ex"<<ranges[i]<<"->cast<TypeInst>());\n";
          }
          _os << "    ex"<<ret<<" = new TypeInst(Location(), ";
          _os << "Type::fromInt("<<ti->type().toInt()<<"), ranges, ";
          if (dom==-1) {
            _os << "NULL";
          } else {
            _os << "ex"<<dom;
          }
          _os << ");\n";
          _os << "  }\n";
        }
        break;
      }
      case Expression::E_TIID:
      {
        ret = _ecount++;
        _os << "  ex"<<ret<<" = new TIId(Location(),\""<<e->cast<TIId>()->v()<<"\");\n";
        break;
      }
    }
    if (!e->ann().isEmpty()) {
      int ann = print(e->ann());
      _os << "  ex"<<ret<<"->ann().merge(ann"<<ann<<");\n";
    }
    _emap.insert(e, ret);
    return ret;
  }
  
  int
  CodePrinter::print(Annotation& ann) {
    std::vector<int> exprs;
    for (ExpressionSetIter it = ann.begin(); it != ann.end(); ++it) {
      exprs.push_back(print(*it));
    }
    int ret = _acount++;
    _os << "  Annotation ann"<<ret<<";\n";
    _os << "  {\n";
    _os << "    std::vector<Expression*> exprs;\n";
    for (unsigned int i=0; i<exprs.size(); i++) {
      _os << "    exprs.push_back(ex"<<exprs[i]<<");\n";
    }
    _os << "    ann"<<ret<<".add(exprs);\n";
    _os << "  }\n";
    return ret;
  }
  
  int
  CodePrinter::print(Item* item) {
    int ret = _icount++;
    switch (item->iid()) {
      case Item::II_INC:
        _os << "  Item* item" << ret << " = new IncludeI(Location(), ASTString(\"";
        _os << item->cast<IncludeI>()->f();
        _os << "\"));\n";
        break;
      case Item::II_VD:
      {
        int vd = print(item->cast<VarDeclI>()->e());
        _os << "  Item* item" << ret << " = new VarDeclI(Location(), ";
        _os << "ex"<<vd<<"->cast<VarDecl>());\n";
        break;
      }
      case Item::II_ASN:
      {
        int ex = print(item->cast<AssignI>()->e());
        _os << "  Item* item" << ret << " = new AssignI(Location(), ";
        _os << "    \"" << item->cast<AssignI>()->id() << "\", ";
        _os << "    ex"<<ex<<");\n";
        break;
      }
      case Item::II_CON:
      {
        int ex = print(item->cast<ConstraintI>()->e());
        _os << "  Item* item" << ret << " = new ConstraintI(Location(), ";
        _os << "    ex"<<ex<<");\n";
        break;
      }
      case Item::II_SOL:
      {
        SolveI* si = item->cast<SolveI>();
        int ex = si->e()==NULL ? -1 : print(si->e());
        _os << "  Item* item" << ret << " = SolveI::";
        switch (si->st()) {
          case SolveI::ST_SAT:
            _os << " sat(Location());\n";
            break;
          case SolveI::ST_MIN:
            _os << " min(Location(), ex"<<ex<<");\n";
            break;
          case SolveI::ST_MAX:
            _os << " max(Location(), ex"<<ex<<");\n";
            break;
        }
        if (!si->ann().isEmpty()) {
          int ann = print(si->ann());
          _os << "  item"<<ret<<"->cast<SolveI>()->ann().merge(ann" << ann << ");\n";
        }
        break;
      }
      case Item::II_OUT:
      {
        int ex = print(item->cast<OutputI>()->e());
        _os << "  Item* item" << ret << " = new OutputI(Location(), ";
        _os << "    ex"<<ex<<");\n";
        break;
      }
      case Item::II_FUN:
      {
        const FunctionI* fi = item->cast<FunctionI>();
        int ti = print(fi->ti());
        std::vector<int> params(fi->params().size());
        for (unsigned int i=0; i<fi->params().size(); i++) {
          params[i] = print(fi->params()[i]);
        }
        int ex = fi->e() == NULL ? -1 : print(fi->e());
        _os << "  Item* item" << ret << ";\n";
        _os << "  {\n";
        _os << "    std::vector<VarDecl*> params;\n";
        for (unsigned int i=0; i<fi->params().size(); i++) {
          _os << "    params.push_back(ex"<<params[i]<<"->cast<VarDecl>());\n";
        }
        _os << "    item"<<ret<<" = new FunctionI(Location(), \"";
        _os << fi->id() << "\", ex"<<ti<<"->cast<TypeInst>(), params, ";
        if (ex==-1) _os << "NULL"; else _os << "ex"<<ex;
        _os << ");\n";
        _os << "  }\n";
        break;
      }
    }
    return ret;
  }
  
  void
  CodePrinter::print(Model* m, const std::string& functionName) {
    _os << "Model* " << functionName << "(void) {\n";
    _os << "  GCLock lock;\n";
    _os << "  Model* m = new Model();\n";
    for (unsigned int i=0; i<m->size(); i++) {
      int item = print((*m)[i]);
      _os << "  m->addItem(item"<<item<<");\n";
    }
    _os << "  return m;\n";
    _os << "}\n";
  }

}
