#include <minizinc/print.hh>

#include <sstream>

namespace MiniZinc {

int precedence(const Expression* e) {
	if (const BinOp* bo = e->dyn_cast<BinOp>()) {
		switch (bo->_op) {
		case BOT_EQUIV:
			return 1200;
		case BOT_IMPL:
			return 1100;
		case BOT_RIMPL:
			return 1100;
		case BOT_OR:
			return 1000;
		case BOT_XOR:
			return 1000;
		case BOT_AND:
			return 900;
		case BOT_LE:
			return 800;
		case BOT_LQ:
			return 800;
		case BOT_GR:
			return 800;
		case BOT_GQ:
			return 800;
		case BOT_EQ:
			return 800;
		case BOT_NQ:
			return 800;
		case BOT_IN:
			return 700;
		case BOT_SUBSET:
			return 700;
		case BOT_SUPERSET:
			return 700;
		case BOT_UNION:
			return 600;
		case BOT_DIFF:
			return 600;
		case BOT_SYMDIFF:
			return 600;
		case BOT_DOTDOT:
			return 500;
		case BOT_PLUS:
			return 400;
		case BOT_MINUS:
			return 400;
		case BOT_MULT:
			return 300;
		case BOT_IDIV:
			return 300;
		case BOT_MOD:
			return 300;
		case BOT_DIV:
			return 300;
		case BOT_INTERSECT:
			return 300;
		case BOT_PLUSPLUS:
			return 200;
		default:
			assert(false);
			return -1;
		}

	} else if (e->isa<Let>()) {
		return 1300;

	} else {
		return 0;
	}
}

enum Parentheses {
	PN_LEFT = 1, PN_RIGHT = 2
};

Parentheses needParens(const BinOp* bo, const Expression* left,
		const Expression* right) {
	int pbo = precedence(bo);
	int pl = precedence(left);
	int pr = precedence(right);
	int ret = (pbo < pl) || (pbo == pl && pbo == 200);
	ret += 2 * ((pbo < pr) || (pbo == pr && pbo != 200));
	return static_cast<Parentheses>(ret);
}

template<class T>
class ExpressionMapper {
protected:
	T& _t;
public:
	ExpressionMapper(T& t) :
			_t(t) {
	}
	typename T::ret map(const Expression* e) {
		switch (e->_eid) {
		case Expression::E_INTLIT:
			return _t.mapIntLit(*e->cast<IntLit>());
		case Expression::E_FLOATLIT:
			return _t.mapFloatLit(*e->cast<FloatLit>());
		case Expression::E_SETLIT:
			return _t.mapSetLit(*e->cast<SetLit>());
		case Expression::E_BOOLLIT:
			return _t.mapBoolLit(*e->cast<BoolLit>());
		case Expression::E_STRINGLIT:
			return _t.mapStringLit(*e->cast<StringLit>());
		case Expression::E_ID:
			return _t.mapId(*e->cast<Id>());
		case Expression::E_ANON:
			return _t.mapAnonVar(*e->cast<AnonVar>());
		case Expression::E_ARRAYLIT:
			return _t.mapArrayLit(*e->cast<ArrayLit>());
		case Expression::E_ARRAYACCESS:
			return _t.mapArrayAccess(*e->cast<ArrayAccess>());
		case Expression::E_COMP:
			return _t.mapComprehension(*e->cast<Comprehension>());
		case Expression::E_ITE:
			return _t.mapITE(*e->cast<ITE>());
		case Expression::E_BINOP:
			return _t.mapBinOp(*e->cast<BinOp>());
		case Expression::E_UNOP:
			return _t.mapUnOp(*e->cast<UnOp>());
		case Expression::E_CALL:
			return _t.mapCall(*e->cast<Call>());
		case Expression::E_VARDECL:
			return _t.mapVarDecl(*e->cast<VarDecl>());
		case Expression::E_LET:
			return _t.mapLet(*e->cast<Let>());
		case Expression::E_ANN:
			return _t.mapAnnotation(*e->cast<Annotation>());
		case Expression::E_TI:
			return _t.mapTypeInst(*e->cast<TypeInst>());
		default:
			assert(false);
			break;
		}
	}
};





template<class T>
class ItemMapper {
protected:
	T& _t;
public:
	ItemMapper(T& t) :
			_t(t) {
	}
	typename T::ret map(Item* i) {
		switch (i->_iid) {
		case Item::II_INC:
			return _t.mapIncludeI(*i->cast<IncludeI>());
		case Item::II_VD:
			return _t.mapVarDeclI(*i->cast<VarDeclI>());
		case Item::II_ASN:
			return _t.mapAssignI(*i->cast<AssignI>());
		case Item::II_CON:
			return _t.mapConstraintI(*i->cast<ConstraintI>());
		case Item::II_SOL:
			return _t.mapSolveI(*i->cast<SolveI>());
		case Item::II_OUT:
			return _t.mapOutputI(*i->cast<OutputI>());
		case Item::II_FUN:
			return _t.mapFunctionI(*i->cast<FunctionI>());
		default:
			assert(false);
			break;
		}
	}
};



Document* expressionToDocument(const Expression* e);
Document* tiexpressionToDocument(const Type& type, const Expression* e) {
  DocumentList* dl = new DocumentList("","","",false);
  switch (type._ti) {
    case Type::TI_PAR: break;
    case Type::TI_VAR: dl->addStringToList("var "); break;
    case Type::TI_SVAR: dl->addStringToList("svar "); break;
    case Type::TI_ANY: dl->addStringToList("any "); break;
  }
  if (type._st==Type::ST_SET)
    dl->addStringToList("set of ");
  if (e==NULL) {
    switch (type._bt) {
      case Type::BT_INT: dl->addStringToList("int"); break;
      case Type::BT_BOOL: dl->addStringToList("bool"); break;
      case Type::BT_FLOAT: dl->addStringToList("float"); break;
      case Type::BT_STRING: dl->addStringToList("string"); break;
      case Type::BT_ANN: dl->addStringToList("ann"); break;
      case Type::BT_BOT: dl->addStringToList("bot"); break;
      case Type::BT_UNKNOWN: dl->addStringToList("???"); break;
    }
  } else {
    dl->addDocumentToList(expressionToDocument(e));
  }
  return dl;
}

class ExpressionDocumentMapper {
public:
	typedef Document* ret;
	ret mapIntLit(const IntLit& il) {
		std::ostringstream oss;
		oss << il._v;
		return new StringDocument(oss.str());
	}
	ret mapFloatLit(const FloatLit& fl) {

		std::ostringstream oss;
		oss << fl._v;
		return new StringDocument(oss.str());
	}
	ret mapSetLit(const SetLit& sl) {
		DocumentList* dl = new DocumentList("{", ", ", "}", true);
		for (unsigned int i = 0; i < sl._v->size(); i++) {
			dl->addDocumentToList(expressionToDocument(((*sl._v)[i])));
		}
		return dl;
	}
	ret mapBoolLit(const BoolLit& bl) {
		return new StringDocument(std::string(bl._v ? "true" : "false"));
	}
	ret mapStringLit(const StringLit& sl) {
		std::ostringstream oss;
		oss << "\"" << sl._v.str() << "\"";
		return new StringDocument(oss.str());

	}
	ret mapId(const Id& id) {
		return new StringDocument(id._v.str());
	}
	ret mapAnonVar(const AnonVar& av) {
		return new StringDocument("_");
	}
	ret mapArrayLit(const ArrayLit& al) {
		/// TODO: test multi-dimensional arrays handling
		DocumentList* dl;
		int n = al._dims->size();
		if (n == 1 && (*al._dims)[0].first == 1) {
			dl = new DocumentList("[", ", ", "]");
			for (unsigned int i = 0; i < al._v->size(); i++)
				dl->addDocumentToList(expressionToDocument((*al._v)[i]));
		} else if (n == 2 && (*al._dims)[0].first == 1
				&& (*al._dims)[1].first == 1) {
			dl = new DocumentList("[| ", " | ", " |]");
			for (int i = 0; i < (*al._dims)[0].second; i++) {
				DocumentList* row = new DocumentList("", ", ", "");
				for (int j = 0; j < (*al._dims)[1].second; j++) {
					row->addDocumentToList(
							expressionToDocument(
									(*al._v)[i * (*al._dims)[0].second + j]));
				}
				dl->addDocumentToList(row);
				if (i != (*al._dims)[0].second - 1)
					dl->addBreakPoint(true); // dont simplify
			}
		} else {
			dl = new DocumentList("", "", "");
			std::stringstream oss;
			oss << "array" << n << "d";
			dl->addStringToList(oss.str());
			DocumentList* args = new DocumentList("(", ", ", ")");

			for (int i = 0; i < al._dims->size(); i++) {
				oss.str("");
				oss << (*al._dims)[i].first << ".." << (*al._dims)[i].second;
				args->addStringToList(oss.str());
			}
			DocumentList* array = new DocumentList("[", ", ", "]");
			for (unsigned int i = 0; i < al._v->size(); i++)
				array->addDocumentToList(expressionToDocument((*al._v)[i]));
			args->addDocumentToList(array);
			dl->addDocumentToList(args);
		}
		return dl;
	}
	ret mapArrayAccess(const ArrayAccess& aa) {
		DocumentList* dl = new DocumentList("", "", "");

		dl->addDocumentToList(expressionToDocument(aa._v));
		DocumentList* args = new DocumentList("[", ", ", "]");
		for (unsigned int i = 0; i < aa._idx->size(); i++) {
			args->addDocumentToList(expressionToDocument((*aa._idx)[i]));
		}
		dl->addDocumentToList(args);
		return dl;
	}
	ret mapComprehension(const Comprehension& c) {
		std::ostringstream oss;
		DocumentList* dl;
		if (c._set)
			dl = new DocumentList("{ ", " | ", " }");
		else
			dl = new DocumentList("[ ", " | ", " ]");
		dl->addDocumentToList(expressionToDocument(c._e));
		DocumentList* generators = new DocumentList("", ", ", "");
		for (unsigned int i = 0; i < c._g->size(); i++) {
			Generator* g = (*c._g)[i];
			DocumentList* gen = new DocumentList("", "", "");
			DocumentList* idents = new DocumentList("", ", ", "");
			for (unsigned int j = 0; j < g->_v->size(); j++) {
				idents->addStringToList((*g->_v)[j]->_id.str());
			}
			gen->addDocumentToList(idents);
			gen->addStringToList(" in ");
			gen->addDocumentToList(expressionToDocument(g->_in));
			generators->addDocumentToList(gen);
		}
		dl->addDocumentToList(generators);
		if (c._where != NULL) {

			dl->addStringToList(" where ");
			dl->addDocumentToList(expressionToDocument(c._where));
		}

		return dl;
	}
	ret mapITE(const ITE& ite) {

		DocumentList* dl = new DocumentList("", "", "");
		for (unsigned int i = 0; i < ite._e_if->size(); i++) {
			std::string beg = (i == 0 ? "if " : " elseif ");
			dl->addStringToList(beg);
			dl->addDocumentToList(expressionToDocument((*ite._e_if)[i].first));
			dl->addStringToList(" then ");

			DocumentList* ifdoc = new DocumentList("", "", "", false);
			ifdoc->addBreakPoint();
			ifdoc->addDocumentToList(
					expressionToDocument((*ite._e_if)[i].second));
			dl->addDocumentToList(ifdoc);
			dl->addStringToList(" ");
		}
		dl->addBreakPoint();
		dl->addStringToList("else ");

		DocumentList* elsedoc = new DocumentList("", "", "", false);
		elsedoc->addBreakPoint();
		elsedoc->addDocumentToList(expressionToDocument(ite._e_else));
		dl->addDocumentToList(elsedoc);
		dl->addStringToList(" ");
		dl->addBreakPoint();
		dl->addStringToList("endif");

		return dl;
	}
	ret mapBinOp(const BinOp& bo) {
		Parentheses ps = needParens(&bo, bo._e0, bo._e1);
		DocumentList* opLeft;
		DocumentList* dl;
		DocumentList* opRight;
		bool linebreak = false;
		if (ps & PN_LEFT)
			opLeft = new DocumentList("(", " ", ")");
		else
			opLeft = new DocumentList("", " ", "");
		opLeft->addDocumentToList(expressionToDocument(bo._e0));
		std::string op;
		switch (bo._op) {
		case BOT_PLUS:
			op = "+";
			break;
		case BOT_MINUS:
			op = "-";
			break;
		case BOT_MULT:
			op = "*";
			break;
		case BOT_DIV:
			op = "/";
			break;
		case BOT_IDIV:
			op = " div ";
			break;
		case BOT_MOD:
			op = " mod ";
			break;
		case BOT_LE:
			op = "<";
			break;
		case BOT_LQ:
			op = "<=";
			break;
		case BOT_GR:
			op = ">";
			break;
		case BOT_GQ:
			op = ">=";
			break;
		case BOT_EQ:
			op = "==";
			break;
		case BOT_NQ:
			op = "!=";
			break;
		case BOT_IN:
			op = " in ";
			break;
		case BOT_SUBSET:
			op = " subset ";
			break;
		case BOT_SUPERSET:
			op = " superset ";
			break;
		case BOT_UNION:
			op = " union ";
			break;
		case BOT_DIFF:
			op = " diff ";
			break;
		case BOT_SYMDIFF:
			op = " symdiff ";
			break;
		case BOT_INTERSECT:
			op = " intersect ";
			break;
		case BOT_PLUSPLUS:
			op = "++";
			linebreak = true;
			break;
		case BOT_EQUIV:
			op = " <-> ";
			break;
		case BOT_IMPL:
			op = " -> ";
			break;
		case BOT_RIMPL:
			op = " <- ";
			break;
		case BOT_OR:
			op = " \\/ ";
			linebreak = true;
			break;
		case BOT_AND:
			op = " /\\ ";
			linebreak = true;
			break;
		case BOT_XOR:
			op = " xor ";
			break;
		case BOT_DOTDOT:
			op = "..";
			break;
		default:
			assert(false);
			break;
		}
		dl = new DocumentList("", op, "");

		if (ps & PN_RIGHT)
			opRight = new DocumentList("(", " ", ")");
		else
			opRight = new DocumentList("", "", "");
		opRight->addDocumentToList(expressionToDocument(bo._e1));
		dl->addDocumentToList(opLeft);
		if (linebreak)
			dl->addBreakPoint();
		dl->addDocumentToList(opRight);

		return dl;
	}
	ret mapUnOp(const UnOp& uo) {
		DocumentList* dl = new DocumentList("", "", "");
		std::string op;
		switch (uo._op) {
		case UOT_NOT:
			op = "not ";
			break;
		case UOT_PLUS:
			op = "+";
			break;
		case UOT_MINUS:
			op = "-";
			break;
		default:
			assert(false);
			break;
		}
		dl->addStringToList(op);
		DocumentList* unop;
		bool needParen = (uo._e0->isa<BinOp>() || uo._e0->isa<UnOp>());
		if (needParen)
			unop = new DocumentList("(", " ", ")");
		else
			unop = new DocumentList("", " ", "");

		unop->addDocumentToList(expressionToDocument(uo._e0));
		dl->addDocumentToList(unop);
		return dl;
	}
	ret mapCall(const Call& c) {
		if (c._args->size() == 1) {
			/*
			 * if we have only one argument, and this is an array comprehension,
			 * we convert it into the following syntax
			 * forall (f(i,j) | i in 1..10)
			 * -->
			 * forall (i in 1..10) (f(i,j))
			 */

			Expression* e = (*c._args)[0];
			if (e->isa<Comprehension>()) {
				Comprehension* com = e->cast<Comprehension>();
				if (!com->_set) {
					DocumentList* dl = new DocumentList("", " ", "");
					dl->addStringToList(c._id.str());
					DocumentList* args = new DocumentList("", " ", "", false);
					DocumentList* generators = new DocumentList("(", ", ", ")");
					for (unsigned int i = 0; i < com->_g->size(); i++) {
						Generator* g = (*com->_g)[i];
						DocumentList* gen = new DocumentList("", "", "");
						for (unsigned int j = 0; j < g->_v->size(); j++) {
							gen->addStringToList((*g->_v)[j]->_id.str());
						}
						gen->addStringToList(" in ");
						gen->addDocumentToList(expressionToDocument(g->_in));
						generators->addDocumentToList(gen);
					}
					args->addDocumentToList(generators);
					args->addStringToList("(");
					args->addBreakPoint();
					args->addDocumentToList(expressionToDocument(com->_e));

					dl->addDocumentToList(args);
					dl->addBreakPoint();
					dl->addStringToList(")");

					return dl;
				}
			}

		}
		std::string beg = c._id.str() + "(";
		DocumentList* dl = new DocumentList(beg, ", ", ")");
		for (unsigned int i = 0; i < c._args->size(); i++) {
			dl->addDocumentToList(expressionToDocument((*c._args)[i]));
		}
		return dl;

	}
	ret mapVarDecl(const VarDecl& vd) {
		std::ostringstream oss;
		DocumentList* dl = new DocumentList("", "", "");
		dl->addDocumentToList(expressionToDocument(vd._ti));
		dl->addStringToList(": ");
		dl->addStringToList(vd._id.str());
		if (vd._e) {
			dl->addStringToList(" = ");
			dl->addDocumentToList(expressionToDocument(vd._e));
		}
		return dl;
	}
	ret mapLet(const Let& l) {
		DocumentList* letin = new DocumentList("", "", "", false);
		DocumentList* lets = new DocumentList("", " ", "", true);
		DocumentList* inexpr = new DocumentList("", "", "");
		bool ds = l._let->size() > 1;

		for (unsigned int i = 0; i < l._let->size(); i++) {
			if (i != 0)
				lets->addBreakPoint(ds);
			DocumentList* exp = new DocumentList("", " ", ";");
			Expression* li = (*l._let)[i];
			if (!li->isa<VarDecl>())
				exp->addStringToList("constraint");
			exp->addDocumentToList(expressionToDocument(li));
			lets->addDocumentToList(exp);
		}

		inexpr->addDocumentToList(expressionToDocument(l._in));
		letin->addBreakPoint(ds);
		letin->addDocumentToList(lets);

		DocumentList* letin2 = new DocumentList("", "", "", false);

		letin2->addBreakPoint();
		letin2->addDocumentToList(inexpr);

		DocumentList* dl = new DocumentList("", "", "");
		dl->addStringToList("let {");
		dl->addDocumentToList(letin);
		dl->addBreakPoint(ds);
		dl->addStringToList("} in ");
		dl->addDocumentToList(letin2);
		//dl->addBreakPoint();
		//dl->addStringToList(")");
		return dl;
	}
	ret mapAnnotation(const Annotation& an) {
		const Annotation* a = &an;
		DocumentList* dl = new DocumentList(" :: ", " :: ", "");
		while (a) {
			Document* ann = expressionToDocument(a->_e);
			dl->addDocumentToList(ann);
			a = a->_a;
		}
		return dl;
	}
	ret mapTypeInst(const TypeInst& ti) {
		DocumentList* dl = new DocumentList("", "", "");
		if (ti.isarray()) {
			dl->addStringToList("array[");
			DocumentList* ran = new DocumentList("", ", ", "");
			for (unsigned int i = 0; i < ti._ranges->size(); i++) {
				ran->addDocumentToList(
	    	tiexpressionToDocument(Type::parint(), (*ti._ranges)[i]));
			}
			dl->addDocumentToList(ran);
			dl->addStringToList("] of ");
		}
		dl->addDocumentToList(tiexpressionToDocument(ti._type,ti._domain));
		return dl;
	}
};

Document* expressionToDocument(const Expression* e) {
	ExpressionDocumentMapper esm;
	ExpressionMapper<ExpressionDocumentMapper> em(esm);
	DocumentList* dl = new DocumentList("", "", "");
	Document* s = em.map(e);
	dl->addDocumentToList(s);
	if (e->_ann) {
		dl->addDocumentToList(em.map(e->_ann));
	}
	return dl;
}

class ItemDocumentMapper {
public:
	typedef Document* ret;
	ret mapIncludeI(const IncludeI& ii) {
		std::ostringstream oss;
		oss << "include \"" << ii._f.str() << "\";";
		return new StringDocument(oss.str());
	}
	ret mapVarDeclI(const VarDeclI& vi) {
		DocumentList* dl = new DocumentList("", " ", ";");
		dl->addDocumentToList(expressionToDocument(vi._e));
		return dl;
	}
	ret mapAssignI(const AssignI& ai) {
		DocumentList* dl = new DocumentList("", " = ", ";");
		dl->addStringToList(ai._id.str());
		dl->addDocumentToList(expressionToDocument(ai._e));
		return dl;
	}
	ret mapConstraintI(const ConstraintI& ci) {
		DocumentList* dl = new DocumentList("constraint ", " ", ";");
		dl->addDocumentToList(expressionToDocument(ci._e));
		return dl;
	}
	ret mapSolveI(const SolveI& si) {
		DocumentList* dl = new DocumentList("", "", ";");
		dl->addStringToList("solve");
		if (si._ann)
			dl->addDocumentToList(expressionToDocument(si._ann));
		switch (si._st) {
		case SolveI::ST_SAT:
			dl->addStringToList(" satisfy");
			break;
		case SolveI::ST_MIN:
			dl->addStringToList(" minimize ");
			dl->addDocumentToList(expressionToDocument(si._e));
			break;
		case SolveI::ST_MAX:
			dl->addStringToList(" maximize ");
			dl->addDocumentToList(expressionToDocument(si._e));
			break;
		}
		return dl;

	}
	ret mapOutputI(const OutputI& oi) {
		DocumentList* dl = new DocumentList("output ", " ", ";");
		dl->addDocumentToList(expressionToDocument(oi._e));
		return dl;
	}
	ret mapFunctionI(const FunctionI& fi) {
		DocumentList* dl;
		if (fi._ti->_type.isann() && fi._e == NULL) {
			dl = new DocumentList("annotation ", " ", ";", false);
		} else if (fi._ti->_type == Type::parbool()) {
			dl = new DocumentList("test ", "", ";", false);
		} else if (fi._ti->_type == Type::varbool()) {
			dl = new DocumentList("predicate ", "", ";", false);
		} else {
			dl = new DocumentList("function ", "", ";", false);
			dl->addDocumentToList(expressionToDocument(fi._ti));
			dl->addStringToList(": ");
		}
		dl->addStringToList(fi._id.str());
		if (!fi._params->empty()) {
			DocumentList* params = new DocumentList("(", ", ", ")");
			for (unsigned int i = 0; i < fi._params->size(); i++) {
				DocumentList* par = new DocumentList("", "", "");
				par->setUnbreakable(true);
				par->addDocumentToList(expressionToDocument((*fi._params)[i]));
				params->addDocumentToList(par);
			}
			dl->addDocumentToList(params);
		}
		if (fi._ann) {
			dl->addDocumentToList(expressionToDocument(fi._ann));
		}
		if (fi._e) {
			dl->addStringToList(" = ");
			dl->addBreakPoint();
			dl->addDocumentToList(expressionToDocument(fi._e));
		}

		return dl;
	}
};

void printDoc(std::ostream& os, Model* m) {
	ItemDocumentMapper ism;
	ItemMapper<ItemDocumentMapper> im(ism);
	PrettyPrinter* printer = new PrettyPrinter(80, 4, true, true);
	for (unsigned int i = 0; i < m->_items.size(); i++) {
		Document* d = im.map(m->_items[i]);
		printer->print(d);
		delete d;
	}
	os << *printer;
	delete printer;
}

}
