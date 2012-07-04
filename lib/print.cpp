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
	ret += (pbo < pr) || (pbo == pr && pbo != 200);
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
	typename T::ret map(Expression* e) {
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
			return _t.mapTiExpr(*e->cast<TiExpr>());
		default:
			assert(false);
			break;
		}
	}
};

std::string expressionToString(Expression* e);
std::string tiexpressionToString(BaseTiExpr* e) {
	switch (e->_tiid) {
	case BaseTiExpr::TI_INT: {
		IntTiExpr* ie = static_cast<IntTiExpr*>(e);
		if (ie->_domain)
			return expressionToString(ie->_domain);
		else
			return "int";
	}
	case BaseTiExpr::TI_FLOAT: {
		FloatTiExpr* fe = static_cast<FloatTiExpr*>(e);
		if (fe->_domain)
			return expressionToString(fe->_domain);
		else
			return "float";
	}
	case BaseTiExpr::TI_BOOL:
		return "bool";
	case BaseTiExpr::TI_STRING:
		return "string";
	case BaseTiExpr::TI_ANN:
		return "ann";
	default:
		assert(false);
		break;
	}
}

class ExpressionStringMapper {
public:
	typedef std::string ret;
	ret mapIntLit(const IntLit& il) {
		std::ostringstream oss;
		oss << il._v;
		return oss.str();
	}
	ret mapFloatLit(const FloatLit& fl) {
		std::ostringstream oss;
		oss << fl._v;
		return oss.str();
	}
	ret mapSetLit(const SetLit& sl) {
		std::ostringstream oss;
		oss << "{";
		for (unsigned int i = 0; i < sl._v->size(); i++) {
			oss << expressionToString((*sl._v)[i]);
			if (i < sl._v->size() - 1)
				oss << ", ";
		}
		oss << "}";
		return oss.str();
	}
	ret mapBoolLit(const BoolLit& bl) {
		return bl._v ? "true" : "false";
	}
	ret mapStringLit(const StringLit& sl) {
		std::ostringstream oss;
		oss << "\"" << sl._v << "\"";
		return oss.str();
	}
	ret mapId(const Id& id) {
		return id._v->str();
	}
	ret mapAnonVar(const AnonVar& av) {
		return "_";
	}
	ret mapArrayLit(const ArrayLit& al) {
		/// TODO: handle multi-dimensional arrays
		std::ostringstream oss;
		oss << "[";
		for (unsigned int i = 0; i < al._v->size(); i++) {
			oss << expressionToString((*al._v)[i]);
			if (i < al._v->size() - 1)
				oss << ", ";
		}
		oss << "]";
		return oss.str();
	}
	ret mapArrayAccess(const ArrayAccess& aa) {
		std::ostringstream oss;
		oss << expressionToString(aa._v);
		oss << "[";
		for (unsigned int i = 0; i < aa._idx->size(); i++) {
			oss << expressionToString((*aa._idx)[i]);
			if (i < aa._idx->size() - 1)
				oss << ", ";
		}
		oss << "]";
		return oss.str();
	}
	ret mapComprehension(const Comprehension& c) {
		std::ostringstream oss;
		oss << (c._set ? "{" : "[");
		oss << expressionToString(c._e);
		oss << " | ";
		for (unsigned int i = 0; i < c._g->size(); i++) {
			Generator* g = (*c._g)[i];
			for (unsigned int j = 0; j < g->_v->size(); j++) {
				oss << (*g->_v)[j]->_id;
				if (j < g->_v->size() - 1)
					oss << ", ";
			}
			oss << " in " << expressionToString(g->_in);
			if (i < c._g->size() - 1)
				oss << ", ";
		}
		if (c._where != NULL)
			oss << " where " << expressionToString(c._where);
		oss << (c._set ? "}" : "]");
		return oss.str();
	}
	ret mapITE(const ITE& ite) {
		std::ostringstream oss;
		for (unsigned int i = 0; i < ite._e_if->size(); i++) {
			oss << (i == 0 ? "if " : " elseif ");
			oss << expressionToString((*ite._e_if)[i].first);
			oss << " then ";
			oss << expressionToString((*ite._e_if)[i].second);
		}
		oss << " else ";
		oss << expressionToString(ite._e_else);
		oss << " endif";
		return oss.str();
	}
	ret mapBinOp(const BinOp& bo) {
		std::ostringstream oss;
		Parentheses ps = needParens(&bo, bo._e0, bo._e1);
		if (ps & PN_LEFT)
			oss << "(";
		oss << expressionToString(bo._e0);
		if (ps & PN_LEFT)
			oss << ")";
		switch (bo._op) {
		case BOT_PLUS:
			oss << "+";
			break;
		case BOT_MINUS:
			oss << "-";
			break;
		case BOT_MULT:
			oss << "*";
			break;
		case BOT_DIV:
			oss << "/";
			break;
		case BOT_IDIV:
			oss << " div ";
			break;
		case BOT_MOD:
			oss << " mod ";
			break;
		case BOT_LE:
			oss << "<";
			break;
		case BOT_LQ:
			oss << "<=";
			break;
		case BOT_GR:
			oss << ">";
			break;
		case BOT_GQ:
			oss << ">=";
			break;
		case BOT_EQ:
			oss << "==";
			break;
		case BOT_NQ:
			oss << "!=";
			break;
		case BOT_IN:
			oss << " in ";
			break;
		case BOT_SUBSET:
			oss << " subset ";
			break;
		case BOT_SUPERSET:
			oss << " superset ";
			break;
		case BOT_UNION:
			oss << " union ";
			break;
		case BOT_DIFF:
			oss << " diff ";
			break;
		case BOT_SYMDIFF:
			oss << " symdiff ";
			break;
		case BOT_INTERSECT:
			oss << " intersect ";
			break;
		case BOT_PLUSPLUS:
			oss << "++";
			break;
		case BOT_EQUIV:
			oss << " <-> ";
			break;
		case BOT_IMPL:
			oss << " -> ";
			break;
		case BOT_RIMPL:
			oss << " <- ";
			break;
		case BOT_OR:
			oss << " \\/ ";
			break;
		case BOT_AND:
			oss << " /\\ ";
			break;
		case BOT_XOR:
			oss << " xor ";
			break;
		case BOT_DOTDOT:
			oss << "..";
			break;
		default:
			assert(false);
			break;
		}
		if (ps & PN_RIGHT)
			oss << "(";
		oss << expressionToString(bo._e1);
		if (ps & PN_RIGHT)
			oss << ")";
		return oss.str();
	}
	ret mapUnOp(const UnOp& uo) {
		std::ostringstream oss;
		switch (uo._op) {
		case UOT_NOT:
			oss << "not ";
			break;
		case UOT_PLUS:
			oss << "+";
			break;
		case UOT_MINUS:
			oss << "-";
			break;
		default:
			assert(false);
			break;
		}
		bool needParen = (uo._e0->isa<BinOp>() || uo._e0->isa<UnOp>());
		if (needParen)
			oss << "(";
		oss << expressionToString(uo._e0);
		if (needParen)
			oss << ")";
		return oss.str();
	}
	ret mapCall(const Call& c) {
		std::ostringstream oss;
		oss << c._id << "(";
		for (unsigned int i = 0; i < c._args->size(); i++) {
			oss << expressionToString((*c._args)[i]);
			if (i < c._args->size() - 1)
				oss << ", ";
		}
		oss << ")";
		return oss.str();
	}
	ret mapVarDecl(const VarDecl& vd) {
		std::ostringstream oss;
		oss << expressionToString(vd._ti);
		oss << ": " << vd._id;
		if (vd._e)
			oss << " = " << expressionToString(vd._e);
		return oss.str();
	}
	ret mapLet(const Let& l) {
		std::ostringstream oss;
		oss << "let {";
		for (unsigned int i = 0; i < l._let->size(); i++) {
			Expression* li = (*l._let)[i];
			if (!li->isa<VarDecl>())
				oss << "constraint ";
			oss << expressionToString(li);
			if (i < l._let->size() - 1)
				oss << "; ";
		}
		oss << "} in ";
		oss << "(" << expressionToString(l._in) << ")";
		return oss.str();
	}
	ret mapAnnotation(const Annotation& an) {
		std::ostringstream oss;
		const Annotation* a = &an;
		while (a) {
			oss << " :: " << expressionToString(a->_e);
			a = a->_a;
		}
		return oss.str();
	}
	ret mapTiExpr(const TiExpr& ti) {
		std::ostringstream oss;
		if (ti.isarray()) {
			oss << "array[";
			for (unsigned int i = 0; i < ti._ranges->size(); i++) {
				oss << tiexpressionToString((*ti._ranges)[i]);
				if (i < ti._ranges->size() - 1)
					oss << ", ";
			}
			oss << "] of ";
		}
		if (ti.isvar())
			oss << "var ";
		if (ti.isset())
			oss << "set of ";
		oss << tiexpressionToString(ti._ti);
		return oss.str();
	}
};

std::string expressionToString(Expression* e) {
	ExpressionStringMapper esm;
	ExpressionMapper<ExpressionStringMapper> em(esm);
	std::string s = em.map(e);
	if (e->_ann)
		s += em.map(e->_ann);
	return s;
}

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
		case Item::II_PRED:
			return _t.mapPredicateI(*i->cast<PredicateI>());
		case Item::II_FUN:
			return _t.mapFunctionI(*i->cast<FunctionI>());
		default:
			assert(false);
			break;
		}
	}
};

class ItemStringMapper {
public:
	typedef std::string ret;
	ret mapIncludeI(const IncludeI& ii) {
		std::ostringstream oss;
		oss << "include \"" << ii._f << "\";";
		return oss.str();
	}
	ret mapVarDeclI(const VarDeclI& vi) {
		std::ostringstream oss;
		oss << expressionToString(vi._e) << ";";
		return oss.str();
	}
	ret mapAssignI(const AssignI& ai) {
		std::ostringstream oss;
		oss << ai._id << " = " << expressionToString(ai._e) << ";";
		return oss.str();
	}
	ret mapConstraintI(const ConstraintI& ci) {
		std::ostringstream oss;
		oss << "constraint " << expressionToString(ci._e) << ";";
		return oss.str();
	}
	ret mapSolveI(const SolveI& si) {
		std::ostringstream oss;
		oss << "solve";
		if (si._ann)
			oss << expressionToString(si._ann);
		switch (si._st) {
		case SolveI::ST_SAT:
			oss << " satisfy";
			break;
		case SolveI::ST_MIN:
			oss << " minimize " << expressionToString(si._e);
			break;
		case SolveI::ST_MAX:
			oss << " maximize " << expressionToString(si._e);
			break;
		}
		oss << ";";
		return oss.str();

	}
	ret mapOutputI(const OutputI& oi) {
		std::ostringstream oss;
		oss << "output " << expressionToString(oi._e) << ";";
		return oss.str();
	}
	ret mapPredicateI(const PredicateI& pi) {
		std::ostringstream oss;
		oss << (pi._test ? "test " : "predicate ") << pi._id;
		if (!pi._params->empty()) {
			oss << "(";
			for (unsigned int i = 0; i < pi._params->size(); i++) {
				oss << expressionToString((*pi._params)[i]);
				if (i < pi._params->size() - 1)
					oss << "; ";
			}
			oss << ")";
		}
		if (pi._ann)
			oss << expressionToString(pi._ann);
		if (pi._e) {
			oss << " = " << expressionToString(pi._e);
		}
		oss << ";";
		return oss.str();
	}
	ret mapFunctionI(const FunctionI& fi) {
		std::ostringstream oss;
		if (fi._ti->isann() && fi._e == NULL) {
			oss << "annotation " << fi._id;
		} else {
			oss << "function " << expressionToString(fi._ti) << " : " << fi._id;
		}
		if (!fi._params->empty()) {
			oss << "(";
			for (unsigned int i = 0; i < fi._params->size(); i++) {
				oss << expressionToString((*fi._params)[i]);
				if (i < fi._params->size() - 1)
					oss << "; ";
			}
			oss << ")";
		}
		if (fi._ann)
			oss << expressionToString(fi._ann);
		if (fi._e) {
			oss << " = " << expressionToString(fi._e);
		}
		oss << ";";
		return oss.str();
	}
};

void print(std::ostream& os, Model* m) {
	ItemStringMapper ism;
	ItemMapper<ItemStringMapper> im(ism);
	for (unsigned int i = 0; i < m->_items.size(); i++) {
		os << im.map(m->_items[i]) << std::endl;
	}
}

Document* expressionToDocument(Expression* e);
Document* tiexpressionToDocument(BaseTiExpr* e) {
	switch (e->_tiid) {
	case BaseTiExpr::TI_INT: {
		IntTiExpr* ie = static_cast<IntTiExpr*>(e);
		if (ie->_domain)
			return expressionToDocument(ie->_domain);
		else
			return new StringDocument("int");
	}
	case BaseTiExpr::TI_FLOAT: {
		FloatTiExpr* fe = static_cast<FloatTiExpr*>(e);
		if (fe->_domain)
			return expressionToDocument(fe->_domain);
		else
			return new StringDocument("float");
	}
	case BaseTiExpr::TI_BOOL:
		return new StringDocument("bool");
	case BaseTiExpr::TI_STRING:
		return new StringDocument("string");
	case BaseTiExpr::TI_ANN:
		return new StringDocument("ann");
	default:
		assert(false);
		break;
	}
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
		oss << "\"" << sl._v << "\"";
		return new StringDocument(oss.str());

	}
	ret mapId(const Id& id) {
		return new StringDocument(id._v->str());
	}
	ret mapAnonVar(const AnonVar& av) {
		return new StringDocument("_");
	}
	ret mapArrayLit(const ArrayLit& al) {
		/// TODO: handle multi-dimensional arrays
		DocumentList* dl = new DocumentList("[", ", ", "]");
		for (unsigned int i = 0; i < al._v->size(); i++)
			dl->addDocumentToList(expressionToDocument((*al._v)[i]));
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
			for (unsigned int j = 0; j < g->_v->size(); j++) {
				gen->addStringToList((*g->_v)[j]->_id->str());

			}
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
		}
		dl->addBreakPoint();
		dl->addStringToList("else ");

		DocumentList* elsedoc = new DocumentList("", "", "", false);
		elsedoc->addBreakPoint();
		elsedoc->addDocumentToList(expressionToDocument(ite._e_else));
		dl->addDocumentToList(elsedoc);
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
					dl->addStringToList(c._id->str());
					DocumentList* args = new DocumentList("", " ", "", false);
					DocumentList* generators = new DocumentList("(", ", ", ")");
					for (unsigned int i = 0; i < com->_g->size(); i++) {
						Generator* g = (*com->_g)[i];
						DocumentList* gen = new DocumentList("", "", "");
						for (unsigned int j = 0; j < g->_v->size(); j++) {
							gen->addStringToList((*g->_v)[j]->_id->str());
						}
						gen->addStringToList(" in ");
						gen->addDocumentToList(expressionToDocument(g->_in));
						generators->addDocumentToList(gen);
					}
					args->addDocumentToList(generators);
					args->addStringToList("(");
					args->addBreakPoint();
					args->addDocumentToList(expressionToDocument(com->_e));
					args->addBreakPoint();
					args->addStringToList(")");
					dl->addDocumentToList(args);

					return dl;
				}
			}

		}
		std::string beg = c._id->str() + "(";
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
		dl->addStringToList(vd._id->str());
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

		for (unsigned int i = 0; i < l._let->size(); i++) {
			if (i != 0)
				lets->addBreakPoint();
			DocumentList* exp = new DocumentList("", " ", ";");
			Expression* li = (*l._let)[i];
			if (!li->isa<VarDecl>())
				exp->addStringToList("constraint ");
			exp->addDocumentToList(expressionToDocument(li));
			lets->addDocumentToList(exp);
			/*if(i != l._let->size()-1)
			 lets->addBreakPoint();*/
		}

		inexpr->addDocumentToList(expressionToDocument(l._in));
		letin->addBreakPoint();
		letin->addDocumentToList(lets);

		DocumentList* letin2 = new DocumentList("", "", "", false);

		letin2->addBreakPoint();
		letin2->addDocumentToList(inexpr);

		DocumentList* dl = new DocumentList("", "", "");
		dl->addStringToList("let {");
		dl->addDocumentToList(letin);
		dl->addBreakPoint();
		dl->addStringToList("} in (");
		dl->addDocumentToList(letin2);
		dl->addBreakPoint();
		dl->addStringToList(")");
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
	ret mapTiExpr(const TiExpr& ti) {
		DocumentList* dl = new DocumentList("", "", "");
		if (ti.isarray()) {
			dl->addStringToList("array[");
			DocumentList* ran = new DocumentList("", ", ", "");
			for (unsigned int i = 0; i < ti._ranges->size(); i++) {
				ran->addDocumentToList(
						tiexpressionToDocument((*ti._ranges)[i]));
			}
			dl->addDocumentToList(ran);
			dl->addStringToList("] of ");
		}
		if (ti.isvar())
			dl->addStringToList("var ");
		if (ti.isset())
			dl->addStringToList("set of ");
		dl->addDocumentToList(tiexpressionToDocument(ti._ti));
		return dl;
	}
};

Document* expressionToDocument(Expression* e) {
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
		oss << "include \"" << ii._f << "\";";
		return new StringDocument(oss.str());
	}
	ret mapVarDeclI(const VarDeclI& vi) {
		DocumentList* dl = new DocumentList("", " ", ";");
		dl->addDocumentToList(expressionToDocument(vi._e));
		return dl;
	}
	ret mapAssignI(const AssignI& ai) {
		DocumentList* dl = new DocumentList("", " = ", ";");
		dl->addStringToList(ai._id->str());
		dl->addDocumentToList(expressionToDocument(ai._e));
		return dl;
	}
	ret mapConstraintI(const ConstraintI& ci) {
		DocumentList* dl = new DocumentList("constraint ", " ", ";");
		dl->addDocumentToList(expressionToDocument(ci._e));
		return dl;
	}
	ret mapSolveI(const SolveI& si) {
		DocumentList* dl = new DocumentList("", " ", ";");
		dl->addStringToList("solve");
		if (si._ann)
			dl->addDocumentToList(expressionToDocument(si._ann));
		switch (si._st) {
		case SolveI::ST_SAT:
			dl->addStringToList("satisfy");
			break;
		case SolveI::ST_MIN:
			dl->addStringToList("minimize");
			dl->addDocumentToList(expressionToDocument(si._e));
			break;
		case SolveI::ST_MAX:
			dl->addStringToList("maximize");
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
	ret mapPredicateI(const PredicateI& pi) {
		DocumentList* dl;
		dl = new DocumentList((pi._test ? "test " : "predicate "), "", ";",
				false);
		dl->addStringToList(pi._id->str());
		if (!pi._params->empty()) {
			DocumentList* params = new DocumentList("(", ", ", ")");
			for (unsigned int i = 0; i < pi._params->size(); i++) {
				params->addDocumentToList(
						expressionToDocument((*pi._params)[i]));
			}
			dl->addDocumentToList(params);
		}
		if (pi._ann)
			dl->addDocumentToList(expressionToDocument(pi._ann));
		if (pi._e) {
			dl->addStringToList(" =");
			dl->addBreakPoint();
			dl->addDocumentToList(expressionToDocument(pi._e));
		}

		return dl;
	}
	ret mapFunctionI(const FunctionI& fi) {
		DocumentList* dl;
		if (fi._ti->isann() && fi._e == NULL) {
			dl = new DocumentList("annotation ", " ", ";", false);
			dl->addStringToList(fi._id->str());
		} else {
			dl = new DocumentList("function ", "", ";", false);
			dl->addDocumentToList(expressionToDocument(fi._ti));
			dl->addStringToList(": ");
			dl->addStringToList(fi._id->str());
		}
		if (!fi._params->empty()) {
			DocumentList* params = new DocumentList("(", "; ", ")");
			for (unsigned int i = 0; i < fi._params->size(); i++) {
				params->addDocumentToList(
						expressionToDocument((*fi._params)[i]));
			}
			dl->addDocumentToList(params);
		}
		if (fi._ann){
			dl->addDocumentToList(expressionToDocument(fi._ann));
		}
		if (fi._e) {
			dl->addStringToList(" =");
			dl->addBreakPoint();
			dl->addDocumentToList(expressionToDocument(fi._e));
		}

		return dl;
	}
};

void printDoc(std::ostream& os, Model* m) {
	ItemDocumentMapper ism;
	ItemMapper<ItemDocumentMapper> im(ism);
	PrettyPrinter* printer = new PrettyPrinter(80);
	for (unsigned int i = 0; i < m->_items.size(); i++) {
		printer->print(im.map(m->_items[i]));
	}
	os << *printer;
}

}
