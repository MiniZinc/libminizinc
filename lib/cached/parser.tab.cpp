/* A Bison parser, made by GNU Bison 3.7.6.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30706

/* Bison version string.  */
#define YYBISON_VERSION "3.7.6"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         mzn_yyparse
#define yylex           mzn_yylex
#define yyerror         mzn_yyerror
#define yydebug         mzn_yydebug
#define yynerrs         mzn_yynerrs

/* First part of user prologue.  */

#define SCANNER static_cast<ParserState*>(parm)->yyscanner
#include <iostream>
#include <fstream>
#include <map>
#include <cerrno>
#include <regex>

namespace MiniZinc{ class ParserLocation; }
#define YYLTYPE MiniZinc::ParserLocation
#define YYLTYPE_IS_DECLARED 1
#define YYLTYPE_IS_TRIVIAL 0

#define YYMAXDEPTH 10000
#define YYINITDEPTH 10000

#include <minizinc/parser.hh>
#include <minizinc/file_utils.hh>

using namespace std;
using namespace MiniZinc;

#define YYLLOC_DEFAULT(Current, Rhs, N) \
  do { \
    if (N > 0) { \
      (Current).filename(YYRHSLOC(Rhs, 1).filename()); \
      (Current).firstLine(YYRHSLOC(Rhs, 1).firstLine()); \
      (Current).firstColumn(YYRHSLOC(Rhs, 1).firstColumn()); \
      (Current).lastLine(YYRHSLOC(Rhs, N).lastLine()); \
      (Current).lastColumn(YYRHSLOC(Rhs, N).lastColumn()); \
    } else { \
      (Current).filename(YYRHSLOC(Rhs, 0).filename()); \
      (Current).firstLine(YYRHSLOC(Rhs, 0).lastLine()); \
      (Current).firstColumn(YYRHSLOC(Rhs, 0).lastColumn()); \
      (Current).lastLine(YYRHSLOC(Rhs, 0).lastLine()); \
      (Current).lastColumn(YYRHSLOC(Rhs, 0).lastColumn()); \
    } \
  } while (false)

int mzn_yyparse(void*);
int mzn_yylex(YYSTYPE*, YYLTYPE*, void* scanner);
int mzn_yylex_init (void** scanner);
int mzn_yylex_destroy (void* scanner);
int mzn_yyget_lineno (void* scanner);
void mzn_yyset_extra (void* user_defined ,void* yyscanner );

extern int yydebug;

namespace MiniZinc {

void yyerror(YYLTYPE* location, void* parm, const string& str) {
  ParserState* pp = static_cast<ParserState*>(parm);
  Model* m = pp->model;
  std::vector<ASTString> includeStack;
  while (m->parent() != nullptr) {
    m = m->parent();
    includeStack.push_back(m->filename());
  }
  auto currentLine = pp->getCurrentLine(location->firstColumn(), location->lastColumn());
  pp->hadError = true;
  pp->syntaxErrors.emplace_back(Location(*location), currentLine, includeStack, str);
}

bool notInDatafile(YYLTYPE* location, void* parm, const string& item) {
  ParserState* pp = static_cast<ParserState*>(parm);
  if (pp->isDatafile) {
    yyerror(location,parm,item+" item not allowed in data file");
    return false;
  }
  return true;
}

Expression* createDocComment(const ParserLocation& loc, const std::string& s) {
  std::vector<Expression*> args(1);
  args[0] = new StringLit(loc, s);
  Call* c = Call::a(Location(loc), Constants::constants().ann.doc_comment, args);
  Expression::type(c, Type::ann());
  return c;
}

Expression* createAccess(const ParserLocation& loc, Expression* e, const std::vector<Expression*>& idx) {
  Expression* ret = e;
  for (auto* expr : idx) {
    // Set location of access expression (TODO: can this be more accurate?)
    Expression::loc(expr, loc);

    // Insert member being accessed
    if (auto* fa = Expression::dynamicCast<FieldAccess>(expr)) {
      fa->v(ret);
    } else {
      auto* aa = Expression::cast<ArrayAccess>(expr);
      aa->v(ret);
    }

    // Set access expression as current incumbent
    ret = expr;
  }
  return ret;
}

// Variant of definition in lexer.lxx
IntVal fast_strtointval(const std::string& s) {
  MiniZinc::IntVal x = 0;
  try {
    for (size_t i = 0; i < s.size(); ++i) {
      x = (x*10) + (s[i] - '0');
    }
  } catch (MiniZinc::ArithmeticError&) {
    return false;
  }
  return x;
}

void parseFieldTail(const ParserLocation& loc, std::vector<Expression*>& parsed, const std::string& tail) {
  auto it = tail.begin();
  auto isWS = [&]() {
    return (*it == ' ' || *it == '\f' || *it == '\xd' || *it == '\t');
  };
  while (it != tail.end()) {
    // skip whitespace and dot
    while(isWS()) { ++it; }
    assert(*it == '.'); // otherwise error in the lexer regex
    ++it;
    while(isWS()) { ++it; }

    // parse field name
    auto field_start = it;
    bool is_num = isdigit(*it);
    std::string field;
    if (*it == '\'') {
      ++field_start;
      ++it;
      while (*it != '\'') {
        ++it;
        assert(it != tail.end()); // otherwise error in the lexer regex
      }
      field = std::string(field_start, it);
      ++it;
    } else {
      while (it != tail.end() && !isWS() && *it != '.') {
        is_num = is_num && isdigit(*it);
        ++it;
      }
      field = std::string(field_start, it);
    }

    // emit field
    Expression* field_expr = nullptr;
    if (is_num) {
      IntVal fieldVal = MiniZinc::fast_strtointval(field);
      field_expr = IntLit::a(fieldVal);
    } else {
      field_expr = new Id(loc, field, nullptr);
    }
    parsed.push_back({ new FieldAccess(loc, nullptr, field_expr) });
  }
}

}



# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include <minizinc/parser.tab.hh>
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_MZN_INTEGER_LITERAL = 3,        /* "integer literal"  */
  YYSYMBOL_MZN_BOOL_LITERAL = 4,           /* "bool literal"  */
  YYSYMBOL_MZN_FLOAT_LITERAL = 5,          /* "float literal"  */
  YYSYMBOL_MZN_IDENTIFIER = 6,             /* "identifier"  */
  YYSYMBOL_MZN_QUOTED_IDENTIFIER = 7,      /* "quoted identifier"  */
  YYSYMBOL_MZN_STRING_LITERAL = 8,         /* "string literal"  */
  YYSYMBOL_MZN_STRING_QUOTE_START = 9,     /* "interpolated string start"  */
  YYSYMBOL_MZN_STRING_QUOTE_MID = 10,      /* "interpolated string middle"  */
  YYSYMBOL_MZN_STRING_QUOTE_END = 11,      /* "interpolated string end"  */
  YYSYMBOL_MZN_TI_IDENTIFIER = 12,         /* "type-inst identifier"  */
  YYSYMBOL_MZN_TI_ENUM_IDENTIFIER = 13,    /* "type-inst enum identifier"  */
  YYSYMBOL_MZN_DOC_COMMENT = 14,           /* "documentation comment"  */
  YYSYMBOL_MZN_DOC_FILE_COMMENT = 15,      /* "file-level documentation comment"  */
  YYSYMBOL_MZN_FIELD_TAIL = 16,            /* "field access"  */
  YYSYMBOL_MZN_VAR = 17,                   /* "var"  */
  YYSYMBOL_MZN_PAR = 18,                   /* "par"  */
  YYSYMBOL_MZN_ABSENT = 19,                /* "<>"  */
  YYSYMBOL_MZN_ANN = 20,                   /* "ann"  */
  YYSYMBOL_MZN_ANNOTATION = 21,            /* "annotation"  */
  YYSYMBOL_MZN_ANY = 22,                   /* "any"  */
  YYSYMBOL_MZN_ARRAY = 23,                 /* "array"  */
  YYSYMBOL_MZN_BOOL = 24,                  /* "bool"  */
  YYSYMBOL_MZN_CASE = 25,                  /* "case"  */
  YYSYMBOL_MZN_CONSTRAINT = 26,            /* "constraint"  */
  YYSYMBOL_MZN_DEFAULT = 27,               /* "default"  */
  YYSYMBOL_MZN_ELSE = 28,                  /* "else"  */
  YYSYMBOL_MZN_ELSEIF = 29,                /* "elseif"  */
  YYSYMBOL_MZN_ENDIF = 30,                 /* "endif"  */
  YYSYMBOL_MZN_ENUM = 31,                  /* "enum"  */
  YYSYMBOL_MZN_FLOAT = 32,                 /* "float"  */
  YYSYMBOL_MZN_FUNCTION = 33,              /* "function"  */
  YYSYMBOL_MZN_IF = 34,                    /* "if"  */
  YYSYMBOL_MZN_INCLUDE = 35,               /* "include"  */
  YYSYMBOL_MZN_INFINITY = 36,              /* "infinity"  */
  YYSYMBOL_MZN_INT = 37,                   /* "int"  */
  YYSYMBOL_MZN_LET = 38,                   /* "let"  */
  YYSYMBOL_MZN_LIST = 39,                  /* "list"  */
  YYSYMBOL_MZN_MAXIMIZE = 40,              /* "maximize"  */
  YYSYMBOL_MZN_MINIMIZE = 41,              /* "minimize"  */
  YYSYMBOL_MZN_OF = 42,                    /* "of"  */
  YYSYMBOL_MZN_OPT = 43,                   /* "opt"  */
  YYSYMBOL_MZN_SATISFY = 44,               /* "satisfy"  */
  YYSYMBOL_MZN_OUTPUT = 45,                /* "output"  */
  YYSYMBOL_MZN_PREDICATE = 46,             /* "predicate"  */
  YYSYMBOL_MZN_RECORD = 47,                /* "record"  */
  YYSYMBOL_MZN_SET = 48,                   /* "set"  */
  YYSYMBOL_MZN_SOLVE = 49,                 /* "solve"  */
  YYSYMBOL_MZN_STRING = 50,                /* "string"  */
  YYSYMBOL_MZN_TEST = 51,                  /* "test"  */
  YYSYMBOL_MZN_THEN = 52,                  /* "then"  */
  YYSYMBOL_MZN_TUPLE = 53,                 /* "tuple"  */
  YYSYMBOL_MZN_TYPE = 54,                  /* "type"  */
  YYSYMBOL_MZN_UNDERSCORE = 55,            /* "_"  */
  YYSYMBOL_MZN_VARIANT_RECORD = 56,        /* "variant_record"  */
  YYSYMBOL_MZN_WHERE = 57,                 /* "where"  */
  YYSYMBOL_MZN_LEFT_BRACKET = 58,          /* "["  */
  YYSYMBOL_MZN_LEFT_2D_BRACKET = 59,       /* "[|"  */
  YYSYMBOL_MZN_RIGHT_BRACKET = 60,         /* "]"  */
  YYSYMBOL_MZN_RIGHT_2D_BRACKET = 61,      /* "|]"  */
  YYSYMBOL_QUOTED_IDENTIFIER = 62,         /* QUOTED_IDENTIFIER  */
  YYSYMBOL_MZN_INVALID_INTEGER_LITERAL = 63, /* "invalid integer literal"  */
  YYSYMBOL_MZN_INVALID_FLOAT_LITERAL = 64, /* "invalid float literal"  */
  YYSYMBOL_MZN_UNTERMINATED_STRING = 65,   /* "unterminated string"  */
  YYSYMBOL_MZN_END_OF_LINE_IN_STRING = 66, /* "end of line inside string literal"  */
  YYSYMBOL_MZN_INVALID_NULL = 67,          /* "null character"  */
  YYSYMBOL_MZN_INVALID_STRING_LITERAL = 68, /* "invalid string literal"  */
  YYSYMBOL_MZN_EQUIV = 69,                 /* "<->"  */
  YYSYMBOL_MZN_IMPL = 70,                  /* "->"  */
  YYSYMBOL_MZN_RIMPL = 71,                 /* "<-"  */
  YYSYMBOL_MZN_OR = 72,                    /* "\\/"  */
  YYSYMBOL_MZN_XOR = 73,                   /* "xor"  */
  YYSYMBOL_MZN_AND = 74,                   /* "/\\"  */
  YYSYMBOL_MZN_LE = 75,                    /* "<"  */
  YYSYMBOL_MZN_GR = 76,                    /* ">"  */
  YYSYMBOL_MZN_LQ = 77,                    /* "<="  */
  YYSYMBOL_MZN_GQ = 78,                    /* ">="  */
  YYSYMBOL_MZN_EQ = 79,                    /* "="  */
  YYSYMBOL_MZN_NQ = 80,                    /* "!="  */
  YYSYMBOL_MZN_WEAK_EQ = 81,               /* "~="  */
  YYSYMBOL_MZN_WEAK_NQ = 82,               /* "~!="  */
  YYSYMBOL_MZN_IN = 83,                    /* "in"  */
  YYSYMBOL_MZN_SUBSET = 84,                /* "subset"  */
  YYSYMBOL_MZN_SUPERSET = 85,              /* "superset"  */
  YYSYMBOL_MZN_UNION = 86,                 /* "union"  */
  YYSYMBOL_MZN_DIFF = 87,                  /* "diff"  */
  YYSYMBOL_MZN_SYMDIFF = 88,               /* "symdiff"  */
  YYSYMBOL_MZN_DOTDOT = 89,                /* ".."  */
  YYSYMBOL_MZN_DOTDOT_LE = 90,             /* "..<"  */
  YYSYMBOL_MZN_LE_DOTDOT = 91,             /* "<.."  */
  YYSYMBOL_MZN_LE_DOTDOT_LE = 92,          /* "<..<"  */
  YYSYMBOL_MZN_PLUS = 93,                  /* "+"  */
  YYSYMBOL_MZN_MINUS = 94,                 /* "-"  */
  YYSYMBOL_MZN_WEAK_PLUS = 95,             /* "~+"  */
  YYSYMBOL_MZN_WEAK_MINUS = 96,            /* "~-"  */
  YYSYMBOL_MZN_MULT = 97,                  /* "*"  */
  YYSYMBOL_MZN_DIV = 98,                   /* "/"  */
  YYSYMBOL_MZN_IDIV = 99,                  /* "div"  */
  YYSYMBOL_MZN_MOD = 100,                  /* "mod"  */
  YYSYMBOL_MZN_WEAK_DIV = 101,             /* "~/"  */
  YYSYMBOL_MZN_WEAK_IDIV = 102,            /* "~div"  */
  YYSYMBOL_MZN_INTERSECT = 103,            /* "intersect"  */
  YYSYMBOL_MZN_WEAK_MULT = 104,            /* "~*"  */
  YYSYMBOL_MZN_POW = 105,                  /* "^"  */
  YYSYMBOL_MZN_POW_MINUS1 = 106,           /* "^-1"  */
  YYSYMBOL_MZN_NOT = 107,                  /* "not"  */
  YYSYMBOL_MZN_PLUSPLUS = 108,             /* "++"  */
  YYSYMBOL_MZN_COLONCOLON = 109,           /* "::"  */
  YYSYMBOL_PREC_ANNO = 110,                /* PREC_ANNO  */
  YYSYMBOL_MZN_EQUIV_QUOTED = 111,         /* "'<->'"  */
  YYSYMBOL_MZN_IMPL_QUOTED = 112,          /* "'->'"  */
  YYSYMBOL_MZN_RIMPL_QUOTED = 113,         /* "'<-'"  */
  YYSYMBOL_MZN_OR_QUOTED = 114,            /* "'\\/'"  */
  YYSYMBOL_MZN_XOR_QUOTED = 115,           /* "'xor'"  */
  YYSYMBOL_MZN_AND_QUOTED = 116,           /* "'/\\'"  */
  YYSYMBOL_MZN_LE_QUOTED = 117,            /* "'<'"  */
  YYSYMBOL_MZN_GR_QUOTED = 118,            /* "'>'"  */
  YYSYMBOL_MZN_LQ_QUOTED = 119,            /* "'<='"  */
  YYSYMBOL_MZN_GQ_QUOTED = 120,            /* "'>='"  */
  YYSYMBOL_MZN_EQ_QUOTED = 121,            /* "'='"  */
  YYSYMBOL_MZN_NQ_QUOTED = 122,            /* "'!='"  */
  YYSYMBOL_MZN_IN_QUOTED = 123,            /* "'in'"  */
  YYSYMBOL_MZN_SUBSET_QUOTED = 124,        /* "'subset'"  */
  YYSYMBOL_MZN_SUPERSET_QUOTED = 125,      /* "'superset'"  */
  YYSYMBOL_MZN_UNION_QUOTED = 126,         /* "'union'"  */
  YYSYMBOL_MZN_DIFF_QUOTED = 127,          /* "'diff'"  */
  YYSYMBOL_MZN_SYMDIFF_QUOTED = 128,       /* "'symdiff'"  */
  YYSYMBOL_MZN_DOTDOT_QUOTED = 129,        /* "'..'"  */
  YYSYMBOL_MZN_LE_DOTDOT_QUOTED = 130,     /* "'<..'"  */
  YYSYMBOL_MZN_DOTDOT_LE_QUOTED = 131,     /* "'..<'"  */
  YYSYMBOL_MZN_LE_DOTDOT_LE_QUOTED = 132,  /* "'<..<'"  */
  YYSYMBOL_MZN_PLUS_QUOTED = 133,          /* "'+'"  */
  YYSYMBOL_MZN_MINUS_QUOTED = 134,         /* "'-'"  */
  YYSYMBOL_MZN_MULT_QUOTED = 135,          /* "'*'"  */
  YYSYMBOL_MZN_DIV_QUOTED = 136,           /* "'/'"  */
  YYSYMBOL_MZN_IDIV_QUOTED = 137,          /* "'div'"  */
  YYSYMBOL_MZN_MOD_QUOTED = 138,           /* "'mod'"  */
  YYSYMBOL_MZN_INTERSECT_QUOTED = 139,     /* "'intersect'"  */
  YYSYMBOL_MZN_POW_QUOTED = 140,           /* "'^'"  */
  YYSYMBOL_MZN_NOT_QUOTED = 141,           /* "'not'"  */
  YYSYMBOL_MZN_COLONCOLON_QUOTED = 142,    /* "'::'"  */
  YYSYMBOL_MZN_PLUSPLUS_QUOTED = 143,      /* "'++'"  */
  YYSYMBOL_144_ = 144,                     /* ';'  */
  YYSYMBOL_145_ = 145,                     /* '{'  */
  YYSYMBOL_146_ = 146,                     /* '}'  */
  YYSYMBOL_147_ = 147,                     /* '('  */
  YYSYMBOL_148_ = 148,                     /* ')'  */
  YYSYMBOL_149_ = 149,                     /* ','  */
  YYSYMBOL_150_ = 150,                     /* ':'  */
  YYSYMBOL_151_ = 151,                     /* '|'  */
  YYSYMBOL_YYACCEPT = 152,                 /* $accept  */
  YYSYMBOL_model = 153,                    /* model  */
  YYSYMBOL_item_list = 154,                /* item_list  */
  YYSYMBOL_item_list_head = 155,           /* item_list_head  */
  YYSYMBOL_doc_file_comments = 156,        /* doc_file_comments  */
  YYSYMBOL_semi_or_none = 157,             /* semi_or_none  */
  YYSYMBOL_item = 158,                     /* item  */
  YYSYMBOL_item_tail = 159,                /* item_tail  */
  YYSYMBOL_error_item_start = 160,         /* error_item_start  */
  YYSYMBOL_include_item = 161,             /* include_item  */
  YYSYMBOL_vardecl_item = 162,             /* vardecl_item  */
  YYSYMBOL_enum_init = 163,                /* enum_init  */
  YYSYMBOL_enum_construct = 164,           /* enum_construct  */
  YYSYMBOL_string_lit_list = 165,          /* string_lit_list  */
  YYSYMBOL_enum_id_list = 166,             /* enum_id_list  */
  YYSYMBOL_assign_item = 167,              /* assign_item  */
  YYSYMBOL_constraint_item = 168,          /* constraint_item  */
  YYSYMBOL_solve_item = 169,               /* solve_item  */
  YYSYMBOL_output_item = 170,              /* output_item  */
  YYSYMBOL_predicate_item = 171,           /* predicate_item  */
  YYSYMBOL_function_item = 172,            /* function_item  */
  YYSYMBOL_annotation_item = 173,          /* annotation_item  */
  YYSYMBOL_ann_param = 174,                /* ann_param  */
  YYSYMBOL_operation_item_tail = 175,      /* operation_item_tail  */
  YYSYMBOL_params = 176,                   /* params  */
  YYSYMBOL_params_list = 177,              /* params_list  */
  YYSYMBOL_params_list_head = 178,         /* params_list_head  */
  YYSYMBOL_comma_or_none = 179,            /* comma_or_none  */
  YYSYMBOL_pipe_or_none = 180,             /* pipe_or_none  */
  YYSYMBOL_ti_expr_and_id_or_anon = 181,   /* ti_expr_and_id_or_anon  */
  YYSYMBOL_ti_expr_and_id = 182,           /* ti_expr_and_id  */
  YYSYMBOL_ti_expr_and_id_list = 183,      /* ti_expr_and_id_list  */
  YYSYMBOL_ti_expr_and_id_list_head = 184, /* ti_expr_and_id_list_head  */
  YYSYMBOL_ti_expr_list = 185,             /* ti_expr_list  */
  YYSYMBOL_ti_expr_list_head = 186,        /* ti_expr_list_head  */
  YYSYMBOL_ti_expr = 187,                  /* ti_expr  */
  YYSYMBOL_base_ti_expr = 188,             /* base_ti_expr  */
  YYSYMBOL_opt_opt = 189,                  /* opt_opt  */
  YYSYMBOL_base_ti_expr_tail = 190,        /* base_ti_expr_tail  */
  YYSYMBOL_array_access_expr_list = 191,   /* array_access_expr_list  */
  YYSYMBOL_array_access_expr_list_head = 192, /* array_access_expr_list_head  */
  YYSYMBOL_array_access_expr = 193,        /* array_access_expr  */
  YYSYMBOL_expr_list = 194,                /* expr_list  */
  YYSYMBOL_expr_list_head = 195,           /* expr_list_head  */
  YYSYMBOL_set_expr = 196,                 /* set_expr  */
  YYSYMBOL_expr = 197,                     /* expr  */
  YYSYMBOL_expr_atom_head = 198,           /* expr_atom_head  */
  YYSYMBOL_expr_atom_head_nonstring = 199, /* expr_atom_head_nonstring  */
  YYSYMBOL_string_expr = 200,              /* string_expr  */
  YYSYMBOL_string_quote_rest = 201,        /* string_quote_rest  */
  YYSYMBOL_access_tail = 202,              /* access_tail  */
  YYSYMBOL_set_literal = 203,              /* set_literal  */
  YYSYMBOL_tuple_literal = 204,            /* tuple_literal  */
  YYSYMBOL_record_literal = 205,           /* record_literal  */
  YYSYMBOL_record_field_list_head = 206,   /* record_field_list_head  */
  YYSYMBOL_record_field = 207,             /* record_field  */
  YYSYMBOL_set_comp = 208,                 /* set_comp  */
  YYSYMBOL_comp_tail = 209,                /* comp_tail  */
  YYSYMBOL_generator_list = 210,           /* generator_list  */
  YYSYMBOL_generator_list_head = 211,      /* generator_list_head  */
  YYSYMBOL_generator = 212,                /* generator  */
  YYSYMBOL_generator_eq = 213,             /* generator_eq  */
  YYSYMBOL_id_list = 214,                  /* id_list  */
  YYSYMBOL_id_list_head = 215,             /* id_list_head  */
  YYSYMBOL_simple_array_literal = 216,     /* simple_array_literal  */
  YYSYMBOL_simple_array_literal_2d = 217,  /* simple_array_literal_2d  */
  YYSYMBOL_simple_array_literal_3d_list = 218, /* simple_array_literal_3d_list  */
  YYSYMBOL_simple_array_literal_2d_list = 219, /* simple_array_literal_2d_list  */
  YYSYMBOL_simple_array_literal_2d_indexed_list = 220, /* simple_array_literal_2d_indexed_list  */
  YYSYMBOL_simple_array_literal_2d_indexed_list_head = 221, /* simple_array_literal_2d_indexed_list_head  */
  YYSYMBOL_simple_array_literal_2d_indexed_list_row = 222, /* simple_array_literal_2d_indexed_list_row  */
  YYSYMBOL_simple_array_literal_2d_indexed_list_row_head = 223, /* simple_array_literal_2d_indexed_list_row_head  */
  YYSYMBOL_simple_array_comp = 224,        /* simple_array_comp  */
  YYSYMBOL_comp_expr_list = 225,           /* comp_expr_list  */
  YYSYMBOL_comp_expr_list_head = 226,      /* comp_expr_list_head  */
  YYSYMBOL_if_then_else_expr = 227,        /* if_then_else_expr  */
  YYSYMBOL_elseif_list = 228,              /* elseif_list  */
  YYSYMBOL_quoted_op = 229,                /* quoted_op  */
  YYSYMBOL_quoted_op_call = 230,           /* quoted_op_call  */
  YYSYMBOL_call_expr = 231,                /* call_expr  */
  YYSYMBOL_comp_or_expr = 232,             /* comp_or_expr  */
  YYSYMBOL_comp_or_expr_head = 233,        /* comp_or_expr_head  */
  YYSYMBOL_let_expr = 234,                 /* let_expr  */
  YYSYMBOL_let_vardecl_item_list = 235,    /* let_vardecl_item_list  */
  YYSYMBOL_comma_or_semi = 236,            /* comma_or_semi  */
  YYSYMBOL_let_vardecl_item = 237,         /* let_vardecl_item  */
  YYSYMBOL_annotations = 238,              /* annotations  */
  YYSYMBOL_annotation_expr = 239,          /* annotation_expr  */
  YYSYMBOL_ne_annotations = 240,           /* ne_annotations  */
  YYSYMBOL_id_or_quoted_op = 241           /* id_or_quoted_op  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  206
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   8345

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  152
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  90
/* YYNRULES -- Number of rules.  */
#define YYNRULES  451
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  769

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   398


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     147,   148,     2,     2,   149,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   150,   144,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   145,   151,   146,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   372,   372,   374,   376,   379,   388,   397,   406,   415,
     417,   420,   428,   437,   437,   439,   455,   459,   461,   463,
     464,   466,   468,   470,   472,   474,   477,   477,   477,   478,
     478,   478,   478,   478,   479,   482,   505,   514,   521,   531,
     549,   563,   575,   579,   588,   593,   599,   606,   607,   611,
     619,   620,   624,   628,   634,   636,   643,   648,   653,   660,
     662,   671,   682,   693,   704,   717,   731,   747,   760,   770,
     771,   781,   782,   787,   788,   790,   795,   796,   800,   811,
     823,   823,   824,   824,   827,   829,   833,   843,   847,   849,
     852,   856,   858,   862,   863,   869,   876,   888,   891,   900,
     912,   925,   933,   943,   956,   970,   974,   979,   980,   984,
     986,   988,   990,   992,   994,  1001,  1011,  1013,  1018,  1024,
    1027,  1029,  1033,  1035,  1037,  1039,  1041,  1044,  1047,  1049,
    1055,  1056,  1058,  1060,  1062,  1064,  1073,  1075,  1077,  1079,
    1081,  1083,  1085,  1087,  1089,  1091,  1093,  1095,  1097,  1099,
    1101,  1110,  1112,  1114,  1116,  1118,  1120,  1122,  1124,  1126,
    1128,  1130,  1132,  1134,  1139,  1144,  1149,  1154,  1159,  1164,
    1169,  1175,  1181,  1183,  1196,  1197,  1199,  1201,  1203,  1205,
    1207,  1209,  1211,  1213,  1215,  1217,  1219,  1221,  1223,  1225,
    1227,  1229,  1231,  1233,  1235,  1244,  1246,  1248,  1250,  1252,
    1254,  1256,  1258,  1260,  1262,  1264,  1266,  1268,  1270,  1272,
    1281,  1283,  1285,  1287,  1289,  1291,  1293,  1295,  1297,  1299,
    1301,  1303,  1305,  1307,  1312,  1317,  1322,  1327,  1332,  1337,
    1342,  1347,  1353,  1355,  1362,  1374,  1376,  1380,  1382,  1384,
    1386,  1388,  1390,  1393,  1395,  1398,  1400,  1403,  1405,  1408,
    1410,  1412,  1414,  1416,  1418,  1420,  1422,  1424,  1426,  1428,
    1429,  1432,  1434,  1437,  1438,  1441,  1443,  1446,  1447,  1450,
    1452,  1455,  1456,  1459,  1461,  1464,  1465,  1468,  1470,  1473,
    1474,  1477,  1479,  1482,  1483,  1484,  1487,  1488,  1491,  1492,
    1495,  1497,  1500,  1501,  1504,  1506,  1511,  1513,  1519,  1524,
    1532,  1541,  1547,  1556,  1565,  1567,  1572,  1577,  1591,  1599,
    1601,  1605,  1612,  1618,  1621,  1624,  1626,  1628,  1634,  1636,
    1638,  1646,  1648,  1651,  1654,  1657,  1659,  1661,  1663,  1667,
    1669,  1716,  1718,  1779,  1819,  1822,  1827,  1834,  1839,  1842,
    1845,  1855,  1867,  1878,  1881,  1885,  1896,  1907,  1926,  1933,
    1937,  1940,  1944,  1955,  1975,  1982,  1998,  1999,  2003,  2005,
    2007,  2009,  2011,  2013,  2015,  2017,  2019,  2021,  2023,  2025,
    2027,  2029,  2031,  2033,  2035,  2037,  2039,  2041,  2043,  2045,
    2047,  2049,  2051,  2053,  2055,  2057,  2061,  2069,  2101,  2103,
    2105,  2106,  2126,  2180,  2200,  2255,  2258,  2264,  2270,  2272,
    2276,  2278,  2285,  2294,  2296,  2304,  2306,  2315,  2315,  2318,
    2326,  2337,  2338,  2341,  2343,  2345,  2349,  2353,  2357,  2359,
    2361,  2363,  2365,  2367,  2369,  2371,  2373,  2375,  2377,  2379,
    2381,  2383,  2385,  2387,  2389,  2391,  2393,  2395,  2397,  2399,
    2401,  2403,  2405,  2407,  2409,  2411,  2413,  2415,  2417,  2419,
    2421,  2423
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "\"integer literal\"",
  "\"bool literal\"", "\"float literal\"", "\"identifier\"",
  "\"quoted identifier\"", "\"string literal\"",
  "\"interpolated string start\"", "\"interpolated string middle\"",
  "\"interpolated string end\"", "\"type-inst identifier\"",
  "\"type-inst enum identifier\"", "\"documentation comment\"",
  "\"file-level documentation comment\"", "\"field access\"", "\"var\"",
  "\"par\"", "\"<>\"", "\"ann\"", "\"annotation\"", "\"any\"", "\"array\"",
  "\"bool\"", "\"case\"", "\"constraint\"", "\"default\"", "\"else\"",
  "\"elseif\"", "\"endif\"", "\"enum\"", "\"float\"", "\"function\"",
  "\"if\"", "\"include\"", "\"infinity\"", "\"int\"", "\"let\"",
  "\"list\"", "\"maximize\"", "\"minimize\"", "\"of\"", "\"opt\"",
  "\"satisfy\"", "\"output\"", "\"predicate\"", "\"record\"", "\"set\"",
  "\"solve\"", "\"string\"", "\"test\"", "\"then\"", "\"tuple\"",
  "\"type\"", "\"_\"", "\"variant_record\"", "\"where\"", "\"[\"",
  "\"[|\"", "\"]\"", "\"|]\"", "QUOTED_IDENTIFIER",
  "\"invalid integer literal\"", "\"invalid float literal\"",
  "\"unterminated string\"", "\"end of line inside string literal\"",
  "\"null character\"", "\"invalid string literal\"", "\"<->\"", "\"->\"",
  "\"<-\"", "\"\\\\/\"", "\"xor\"", "\"/\\\\\"", "\"<\"", "\">\"",
  "\"<=\"", "\">=\"", "\"=\"", "\"!=\"", "\"~=\"", "\"~!=\"", "\"in\"",
  "\"subset\"", "\"superset\"", "\"union\"", "\"diff\"", "\"symdiff\"",
  "\"..\"", "\"..<\"", "\"<..\"", "\"<..<\"", "\"+\"", "\"-\"", "\"~+\"",
  "\"~-\"", "\"*\"", "\"/\"", "\"div\"", "\"mod\"", "\"~/\"", "\"~div\"",
  "\"intersect\"", "\"~*\"", "\"^\"", "\"^-1\"", "\"not\"", "\"++\"",
  "\"::\"", "PREC_ANNO", "\"'<->'\"", "\"'->'\"", "\"'<-'\"",
  "\"'\\\\/'\"", "\"'xor'\"", "\"'/\\\\'\"", "\"'<'\"", "\"'>'\"",
  "\"'<='\"", "\"'>='\"", "\"'='\"", "\"'!='\"", "\"'in'\"",
  "\"'subset'\"", "\"'superset'\"", "\"'union'\"", "\"'diff'\"",
  "\"'symdiff'\"", "\"'..'\"", "\"'<..'\"", "\"'..<'\"", "\"'<..<'\"",
  "\"'+'\"", "\"'-'\"", "\"'*'\"", "\"'/'\"", "\"'div'\"", "\"'mod'\"",
  "\"'intersect'\"", "\"'^'\"", "\"'not'\"", "\"'::'\"", "\"'++'\"", "';'",
  "'{'", "'}'", "'('", "')'", "','", "':'", "'|'", "$accept", "model",
  "item_list", "item_list_head", "doc_file_comments", "semi_or_none",
  "item", "item_tail", "error_item_start", "include_item", "vardecl_item",
  "enum_init", "enum_construct", "string_lit_list", "enum_id_list",
  "assign_item", "constraint_item", "solve_item", "output_item",
  "predicate_item", "function_item", "annotation_item", "ann_param",
  "operation_item_tail", "params", "params_list", "params_list_head",
  "comma_or_none", "pipe_or_none", "ti_expr_and_id_or_anon",
  "ti_expr_and_id", "ti_expr_and_id_list", "ti_expr_and_id_list_head",
  "ti_expr_list", "ti_expr_list_head", "ti_expr", "base_ti_expr",
  "opt_opt", "base_ti_expr_tail", "array_access_expr_list",
  "array_access_expr_list_head", "array_access_expr", "expr_list",
  "expr_list_head", "set_expr", "expr", "expr_atom_head",
  "expr_atom_head_nonstring", "string_expr", "string_quote_rest",
  "access_tail", "set_literal", "tuple_literal", "record_literal",
  "record_field_list_head", "record_field", "set_comp", "comp_tail",
  "generator_list", "generator_list_head", "generator", "generator_eq",
  "id_list", "id_list_head", "simple_array_literal",
  "simple_array_literal_2d", "simple_array_literal_3d_list",
  "simple_array_literal_2d_list", "simple_array_literal_2d_indexed_list",
  "simple_array_literal_2d_indexed_list_head",
  "simple_array_literal_2d_indexed_list_row",
  "simple_array_literal_2d_indexed_list_row_head", "simple_array_comp",
  "comp_expr_list", "comp_expr_list_head", "if_then_else_expr",
  "elseif_list", "quoted_op", "quoted_op_call", "call_expr",
  "comp_or_expr", "comp_or_expr_head", "let_expr", "let_vardecl_item_list",
  "comma_or_semi", "let_vardecl_item", "annotations", "annotation_expr",
  "ne_annotations", "id_or_quoted_op", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,    59,   123,   125,    40,    41,    44,
      58,   124
};
#endif

#define YYPACT_NINF (-616)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-77)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    1171,  -121,   -68,   -62,   -53,    16,  -616,  4220,  -616,  -616,
    2335,  -616,     4,     4,   -34,  -616,    67,    76,    34,  -616,
    3640,   101,  -616,  2770,  4220,   109,    55,  -616,   -26,   124,
    3060,  3785,   162,    24,   153,    87,  -616,   224,    85,   228,
      17,  3930,   799,  4800,  4800,  4800,  4800,  4800,  4800,  -616,
    -616,  -616,  -616,  -616,  -616,  -616,  -616,  -616,  -616,  -616,
    -616,  -616,  -616,  -616,  -616,  -616,  -616,    88,    90,    94,
      95,  -616,  -616,  -616,  -616,  -616,  -616,  -616,  -616,  -616,
    -616,  4075,  4365,   240,  -616,   100,  1900,   518,  -616,  -616,
    -616,  -616,  -616,  -616,  -616,  -616,  -616,  -616,   167,   -63,
    -616,  -616,  1146,  -616,  -616,  -616,    69,    73,   141,   220,
     306,   308,   321,   322,   106,  -616,   324,  -616,  2190,  -616,
    -616,  -616,  -616,  4510,  4220,   107,  1462,   327,    26,  4220,
    4220,  4220,  4220,  4220,  4220,  4220,   108,   112,   113,   114,
       6,  7839,  -616,  -616,  -616,  -616,  3205,  3350,  -616,   115,
    -616,  2770,    62,  7839,    87,   -59,  7204,  -616,  -616,  2480,
    2915,   214,  -616,    62,  7839,    22,  2770,  3495,  4986,   140,
     156,    38,  2770,    87,  -616,   329,  -616,  5088,   203,   117,
    -616,   951,  7839,   -47,   207,   118,  -616,    10,  5350,  5350,
    5350,  5350,    91,    91,  4220,  4800,  4800,  4800,  -616,   125,
     121,  5187,     2,  5426,   128,  -616,  -616,  2045,  -616,  -616,
    -616,  -616,  -616,  -616,  -616,  -616,  -616,  -616,  -616,  -616,
    -616,  4220,  2915,   317,  4800,  4800,  4800,  4800,  4800,  4800,
    4800,  4945,  4945,  4945,  4945,  4800,  4800,  4800,  4800,  4800,
    4800,  4800,  4800,  4800,  4800,  4800,  4800,  4800,  4986,  -616,
     331,  -616,   333,  -616,   399,  -616,   400,  -616,   454,  -616,
     475,  -616,   487,  -616,   488,  4220,  -616,   525,  -616,  4220,
    4220,  4220,  4220,   267,   182,  -616,  7839,  7839,  1608,  -616,
    7303,   184,   186,  -616,  4510,  -616,   351,   351,   351,   351,
      39,    39,    39,  4220,  4220,  4220,  4220,  4220,  -616,  4220,
    4220,  4220,  4220,  4220,  4220,  4220,  4220,  4220,  4220,  4220,
    4220,  4220,  4220,  4220,  4220,  4220,  4220,  4220,  4220,  4220,
    4220,  4220,  4655,  4655,  4655,  4655,  4220,  4220,  4220,  4220,
    4220,  4220,  4220,  4220,  4220,  4220,  4220,  4220,  4220,  4220,
    4986,   294,  -616,   299,  -616,  1316,   265,   286,   199,   242,
    4220,   272,  7069,  4220,   269,  -616,   276,   -45,    21,  -616,
    -616,  3495,  4220,   115,   336,  -616,   209,   210,  -616,  -616,
    -616,  -616,  -616,  4220,  4220,  -616,  4986,   115,   336,   212,
     284,  -616,  4220,    44,  -616,  4220,  -616,  -616,  -616,   216,
    -616,   217,  -616,  4220,  -616,  4220,  4220,  -616,  5525,   427,
    6045,  6144,  -616,  4220,  -616,    44,  4220,   534,  1754,   363,
     222,  1900,  -616,  7839,  -616,   -89,   262,    32,  5467,  5467,
    5566,  5566,  5566,  5350,  5350,  5350,  5350,   367,   367,   367,
     367,    29,    29,    29,    29,    29,    29,  5566,    29,    91,
    -616,  -616,  -616,  -616,  -616,  -616,  -616,  -616,  -616,  5624,
    -616,  -616,  4510,  -616,  -616,   233,  4220,   225,  4220,  -616,
     328,  5723,  5822,  5921,  6020,  -616,  7839,   275,    33,  7938,
    7976,  7976,  8075,  8075,  8110,  8209,  8209,  8209,  8209,  8209,
    8209,  8209,  8209,  8236,  8236,  8236,  5111,  5111,  5111,   351,
     351,   351,   351,    89,    89,    89,    89,    28,    28,    28,
      28,    28,    28,  5111,    28,    39,    93,  -616,  3495,  3495,
     244,   245,   241,  -616,  -616,   -45,  4220,   353,  2770,  -616,
    7839,     9,   290,  -616,  -616,  -616,  -616,  -616,  -616,  -616,
    -616,  -616,  -616,  -616,  -616,  -616,  -616,  -616,  -616,  -616,
    -616,  -616,  -616,  -616,  -616,  -616,  -616,  -616,  -616,  -616,
    -616,  -616,  -616,  -616,  -616,   115,  7402,  4220,  4220,   391,
    -616,   315,  -616,  2625,  -616,  7839,   336,   249,    87,  -616,
    2770,  -616,  7839,  7839,  -616,   336,    87,  -616,  2770,  5228,
     325,  -616,   340,  -616,   254,  -616,   348,   323,   258,  5327,
    4220,  4220,  -616,    54,  7839,  7839,  -616,  4220,  -616,  4800,
    -616,  4800,  -616,  4800,   273,  7839,  -616,   541,  -616,   260,
     261,  -616,  -616,  -616,  2770,  -616,  -616,  4220,  -616,   266,
    7839,  4220,  7501,  -616,  -616,  4220,  -616,  4220,  -616,  4220,
    -616,  4220,  -616,  -616,  -616,  -616,  2770,  -616,  7839,  2915,
     242,   271,   277,   412,   416,   318,  -616,  -616,   336,  -616,
     154,  7839,  7839,    87,  4220,   342,  -616,  -616,    87,   417,
     350,  -616,    87,   350,   242,    44,  4220,  -616,    44,  -616,
    4220,  4220,    46,  -616,  4220,  -616,   280,  4220,  6243,  6943,
    6968,  7067,  -616,  -616,  -616,   314,  6284,  4220,  6383,  4220,
    6424,  6523,  6564,  6663,  -616,  -616,  4220,  4220,  -616,    -3,
    -616,   326,    13,    87,  4220,  4220,  7839,  4220,   350,  -616,
    4220,  -616,   350,  -616,   403,  7839,  -616,   420,  7839,  7600,
    -616,  -616,  7839,  4220,  -616,  -616,  -616,  -616,   336,  -616,
    6704,  -616,  7839,  -616,  -616,  -616,  -616,  6803,  6844,  -616,
     465,   473,   334,  -616,   350,  7699,  7798,  7839,  -616,  7839,
    -616,  -616,  4220,  4220,    87,  -616,  -616,  -616,  -616,  -616,
    -616,  -616,  -616,  4220,  7839,  7839,   350,  7839,  -616
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       0,     0,   251,   249,   255,   241,   296,     0,   117,   118,
       0,    11,   107,   107,   257,   113,     0,   106,     0,   110,
       0,     0,   111,     0,     0,     0,   253,   109,     0,     0,
       0,     0,     0,     0,     0,   411,   112,     0,     0,     0,
     245,     0,     0,     0,     0,     0,     0,     0,     0,   358,
     359,   360,   361,   362,   363,   364,   365,   366,   367,   368,
     369,   370,   371,   372,   373,   374,   375,     0,     0,     0,
       0,   376,   377,   378,   380,   381,   382,   383,   379,   385,
     384,     0,     0,     0,     2,    13,     0,     5,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    36,     0,
      93,    97,   116,   130,   235,   236,   259,   288,   292,   263,
     267,   271,   275,   279,     0,   390,   284,   283,     0,   252,
     250,   256,   301,     0,     0,   243,     0,   242,   241,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   128,   174,   297,    15,   108,     0,     0,   258,    73,
     105,     0,     0,    54,   411,     0,     0,    35,   254,     0,
       0,     0,    98,     0,    59,    73,     0,     0,     0,     0,
     412,    73,     0,   411,   247,   246,   329,   350,     0,    80,
     331,     0,   344,     0,     0,    82,   340,    80,   143,   144,
     145,   146,   172,   173,     0,     0,     0,     0,   304,     0,
      80,   128,   241,     0,    80,   309,     1,    14,     4,    12,
       6,    34,    29,    27,    32,    26,    28,    31,    30,    33,
       9,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   139,   140,   141,   142,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   261,
     260,   290,   289,   294,   293,   265,   264,   269,   268,   273,
     272,   277,   276,   281,   280,     0,   286,   285,    10,   123,
     124,   125,   126,     0,    80,   120,   122,    53,     0,   388,
     396,     0,    80,   303,     0,   244,   202,   203,   204,   205,
     233,   234,   232,     0,     0,     0,     0,     0,   298,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   198,   199,   200,   201,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   100,     0,    99,     0,    67,     0,    80,    91,
       0,    38,     0,     0,     0,   404,   409,     0,     0,   403,
      95,     0,     0,    73,    69,    88,     0,    80,   101,   414,
     413,   415,   416,     0,     0,    56,     0,    73,    69,     0,
       0,   248,     0,     0,   330,    81,   349,   334,   337,     0,
     333,     0,   332,    83,   339,    81,   341,   343,     0,     0,
       0,     0,   305,    81,   127,     0,     0,   237,     0,    81,
       0,     0,     7,    37,    96,   411,   171,   170,   168,   169,
     132,   133,   134,   135,   136,   137,   138,   156,   157,   163,
     164,   158,   159,   160,   161,   166,   167,   155,   165,   162,
     131,   262,   291,   295,   266,   270,   274,   278,   282,     0,
     287,   300,    81,   119,   389,     0,     0,   391,    81,   395,
       0,     0,     0,     0,     0,   299,   129,   231,   230,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   228,   229,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   216,   217,   223,   224,   218,   219,   220,
     221,   226,   227,   214,   225,   222,   215,   175,     0,     0,
       0,     0,    80,    78,    84,    85,     0,     0,    81,    90,
      55,     0,   418,   420,   421,   422,   423,   424,   425,   426,
     427,   428,   429,   430,   431,   432,   433,   434,   435,   436,
     437,   438,   439,   440,   441,   442,   443,   444,   446,   447,
     448,   449,   445,   450,   451,    73,   356,     0,     0,     0,
     408,     0,   407,     0,   102,    60,    69,     0,   411,   115,
      81,    87,    58,    57,   417,    69,   411,   114,     0,   351,
     325,   326,     0,   313,    80,   315,   316,     0,    80,   352,
     335,     0,   342,    80,   346,   345,   154,     0,   152,     0,
     151,     0,   153,     0,     0,   311,   239,   238,   306,     0,
       0,   310,   308,     8,    76,    86,   387,     0,   121,   393,
     397,     0,   398,   302,   213,     0,   211,     0,   210,     0,
     212,     0,   104,   103,    75,    74,    81,    77,    68,     0,
      92,     0,     0,    47,    50,    39,    42,   419,    69,   354,
       0,   400,   410,   411,     0,     0,   406,   405,   411,     0,
      71,    89,   411,    71,    41,     0,     0,   348,    81,   314,
       0,     0,    81,   324,     0,   338,     0,     0,     0,     0,
       0,     0,   312,   240,   307,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    79,    94,     0,     0,    48,     0,
      51,    80,     0,   411,     0,     0,   401,     0,    71,    70,
       0,    61,    71,    62,     0,   323,   318,   319,   317,   321,
     327,   328,   353,   336,   150,   148,   147,   149,    69,   386,
       0,   392,   399,   209,   207,   206,   208,     0,     0,    40,
       0,    81,     0,    43,    71,     0,     0,   402,    63,    72,
      64,   347,     0,     0,   411,   394,    45,    46,    49,    52,
      44,    65,   355,     0,   320,   322,    71,   357,    66
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -616,  -616,  -616,  -616,   274,  -616,   -77,   474,  -616,  -616,
    -616,  -616,  -217,  -616,  -616,  -616,  -154,  -616,  -616,  -616,
    -616,  -616,  -327,  -615,  -134,  -126,  -616,  -110,  -616,  -147,
    -153,  -616,  -616,   320,  -616,   -16,  -157,   477,   -22,   211,
    -616,    41,   -80,    23,   -19,   -20,   542,   -15,  -109,   197,
     -30,  -616,  -616,  -616,  -616,    92,  -616,  -393,  -616,  -616,
    -171,  -170,  -616,  -616,  -616,  -616,  -616,   -92,  -616,  -616,
     147,   155,  -616,  -616,  -616,  -616,  -616,  -616,  -616,  -616,
     229,  -616,  -616,  -616,  -616,   -52,   -33,  -214,  -616,  -616
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    83,    84,    85,    86,   208,    87,    88,   220,    89,
      90,   645,   646,   699,   701,    91,    92,    93,    94,    95,
      96,    97,   568,   711,   346,   511,   512,   397,   394,   513,
      98,   366,   367,   347,   348,    99,   100,   146,   101,   273,
     274,   275,   388,   200,   102,   141,   142,   104,   105,   143,
     127,   106,   107,   108,   204,   205,   109,   582,   583,   584,
     585,   586,   587,   588,   110,   111,   183,   389,   184,   185,
     186,   187,   112,   178,   179,   113,   650,   114,   115,   116,
     281,   282,   117,   358,   563,   359,   615,   372,   170,   555
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     153,   199,   169,   360,   156,   355,   356,   155,   162,   210,
     175,   164,   604,   365,   390,   641,   297,   298,   122,   641,
     168,   177,   182,   118,   188,   189,   190,   191,   192,   193,
     140,   364,   122,   122,   440,   300,   224,   378,   119,   224,
     300,   268,   122,   350,   120,   222,   300,   145,   713,   222,
     580,   576,   720,   121,   362,   301,   225,   739,   614,   371,
     123,   201,   203,   222,   642,   414,   301,   643,   642,   386,
       6,     7,   148,   149,   123,   123,   250,   252,   254,   256,
     258,   260,   262,   264,   123,   122,   267,   223,   150,   122,
     404,   352,   151,   748,   410,   124,   300,   750,   224,   581,
     300,   721,   391,   276,   277,   559,   280,   154,   125,   286,
     287,   288,   289,   290,   291,   292,   301,   157,   225,   159,
     301,   351,   125,   174,   342,   344,   507,   123,   363,   761,
     412,   123,   125,   338,   247,   349,   339,   340,   248,   371,
     380,   248,   340,   357,   377,   368,   740,   339,   340,   126,
     357,   768,   406,   370,   644,   299,   349,   122,   644,   395,
     396,   158,   574,   126,   453,   560,   160,   561,   165,   345,
     562,   166,   459,   126,   398,   249,   399,   400,   401,   251,
     373,   374,   704,   705,   375,   345,   330,   331,   332,   333,
     334,   335,   514,   337,   338,   167,   168,   339,   340,   123,
     248,   413,   340,   395,   677,   416,   417,   418,   419,   420,
     421,   422,   423,   424,   425,   426,   427,   428,   429,   430,
     431,   432,   433,   434,   435,   436,   437,   438,   439,   566,
     171,   371,   172,   370,   173,   194,   122,   195,   519,   658,
     206,   196,   197,   575,   207,   449,   221,   253,   662,   286,
     287,   288,   289,   265,   278,   293,   361,   571,   280,   294,
     295,   296,   345,   384,   276,   376,   385,   371,   392,   393,
     403,   402,   714,   461,   462,   463,   464,   409,   123,   466,
     467,   468,   469,   470,   471,   472,   473,   474,   475,   476,
     477,   478,   479,   480,   481,   482,   483,   484,   485,   486,
     487,   488,   489,   490,   491,   492,   493,   494,   495,   496,
     497,   498,   499,   500,   501,   502,   503,   504,   505,   506,
     140,   703,   122,   415,   122,   370,   255,   451,   609,   515,
     520,   452,   457,   556,   613,   458,   508,   122,   122,   564,
     122,   509,   565,   283,   516,   283,   517,   283,   518,   283,
     222,   521,   557,   572,   573,   558,   567,   569,   300,   570,
     577,   370,   579,   578,   123,   589,   123,   590,   591,   610,
     612,   248,   621,   182,   224,   594,   595,   607,   301,   123,
     123,   619,   123,   466,   340,   284,   605,   284,   623,   284,
     636,   284,   634,   635,   225,   639,   647,   653,   654,   659,
     667,   754,   637,   668,   666,   670,   671,   672,   684,   656,
     356,   406,   257,   687,   259,   283,   283,   661,   696,   682,
     698,   648,   700,   709,   697,   707,   702,   261,   263,   710,
     266,   723,   276,   285,   224,   381,   620,   441,   622,   442,
     -77,   -77,   -77,   -77,   326,   327,   328,   329,   330,   331,
     332,   333,   334,   335,   225,   337,   338,   284,   284,   339,
     340,   514,   728,   751,   239,   240,   241,   242,   243,   244,
     283,   246,   247,   758,   669,   741,   248,   752,   673,   759,
     760,   411,   695,   514,   144,   743,   632,   633,   685,   694,
     147,   283,   379,   618,   465,   460,   638,   716,   717,   676,
       0,   611,   640,   283,   283,   443,   444,   455,   226,   227,
     675,   657,   284,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   284,     0,   660,   248,   651,   652,   211,
     592,   283,   103,   663,   212,   284,   284,   357,   593,   213,
     122,   214,   103,   215,   357,     0,     0,   283,     0,     0,
     445,     0,   664,   216,   217,   103,     0,   218,     0,   219,
       0,     0,   103,     0,     0,   598,   599,   678,     0,     0,
     679,   446,   680,   284,   681,   103,   103,   103,   103,   103,
     103,   742,   123,   447,   448,     0,     0,   686,   515,   284,
       0,   688,     0,     0,     0,   690,     0,   691,     0,   692,
       0,   693,     0,     0,     0,     0,     0,     0,     0,     0,
     515,     0,     0,     0,     0,   708,     0,     0,   103,   712,
       0,   450,     0,     0,   706,     0,     0,     0,     0,     0,
     606,     0,     0,   675,     0,     0,   715,   683,     0,     0,
     718,   719,     0,     0,   722,     0,     0,   595,     0,     0,
     103,     0,     0,     0,     0,     0,     0,   730,     0,   732,
     744,     0,     0,     0,     0,     0,   737,   738,     0,     0,
       0,     0,     0,     0,   745,   746,     0,   747,   103,   103,
     749,     0,     0,   103,     0,     0,     0,     0,     0,     0,
       0,   103,   103,     0,     0,     0,     0,     0,   103,   103,
       0,     0,     0,     0,   103,     0,     0,     0,     0,     0,
       0,   766,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   764,   765,     0,     0,     0,   103,   103,   103,
       0,     0,     0,   767,     0,     0,     0,     0,     0,   103,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   103,     0,   103,   103,   103,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     2,     3,     4,   128,     0,     6,     7,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    14,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    24,     0,    26,     0,    28,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    40,     0,     0,    41,    42,     0,
     180,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   103,   129,   130,
     131,   132,   133,   134,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   103,     0,     0,   135,     0,     0,     0,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,   136,   137,
     138,   139,    71,    72,    73,    74,    75,    76,    77,    78,
      79,     0,    80,     0,    81,     0,    82,     0,     0,     0,
     181,     0,     0,   103,     2,     3,     4,   128,     0,     6,
       7,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      14,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    24,     0,    26,     0,    28,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    40,     0,     0,    41,
      42,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     129,   130,   131,   132,   133,   134,     0,     0,     0,     0,
     103,   103,     0,     0,     0,     0,     0,     0,   135,     0,
     103,     0,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
     136,   137,   138,   139,    71,    72,    73,    74,    75,    76,
      77,    78,    79,     0,    80,     0,    81,     0,    82,     0,
       0,     0,   387,     0,     0,   103,     0,     0,     0,     0,
       0,     0,   103,     0,     0,     0,     0,     0,     0,     0,
     103,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   103,     0,   103,     0,   103,     0,     0,     0,     0,
       0,     0,     0,   224,     0,     0,   103,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    -3,     1,   225,     2,     3,     4,     5,   103,     6,
       7,   103,     0,     8,     9,    10,    11,     0,    12,    13,
      14,    15,    16,    17,    18,    19,     0,    20,     0,     0,
       0,     0,    21,    22,    23,    24,    25,    26,    27,    28,
      29,     0,     0,     0,    30,     0,    31,    32,    33,    34,
      35,    36,    37,     0,    38,    39,    40,   226,   227,    41,
      42,     0,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,   245,
     246,   247,     0,     0,     0,   248,     0,     0,     0,     0,
      43,    44,    45,    46,    47,    48,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,     0,    80,     0,    81,   510,    82,     2,
       3,     4,   128,     0,     6,     7,     0,     0,     8,     9,
       0,     0,     0,    12,    13,    14,    15,     0,    17,    18,
      19,     0,     0,     0,     0,     0,     0,     0,    22,     0,
      24,     0,    26,    27,    28,    29,     0,     0,     0,    30,
       0,     0,     0,    33,    34,     0,    36,     0,     0,    38,
       0,    40,     0,     0,    41,    42,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    43,    44,    45,    46,    47,
      48,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,     0,    80,
       0,    81,     0,    82,   -76,     2,     3,     4,   128,     0,
       6,     7,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    14,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    24,     0,    26,     0,
      28,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    40,     0,     0,
      41,    42,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   129,   130,   131,   132,   133,   134,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   135,
       0,     0,     0,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,   136,   137,   138,   139,    71,    72,    73,    74,    75,
      76,    77,    78,    79,     0,    80,     0,    81,     0,    82,
     279,     2,     3,     4,   128,     0,     6,     7,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    14,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    24,     0,    26,     0,    28,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    40,     0,     0,    41,    42,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   129,   130,   131,
     132,   133,   134,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   135,     0,     0,     0,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,   136,   137,   138,
     139,    71,    72,    73,    74,    75,    76,    77,    78,    79,
       0,    80,     0,    81,     0,    82,   454,     2,     3,     4,
     128,     0,     6,     7,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    14,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    24,     0,
      26,     0,    28,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    40,
       0,     0,    41,    42,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   129,   130,   131,   132,   133,   134,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   135,     0,     0,     0,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,   136,   137,   138,   139,    71,    72,    73,
      74,    75,    76,    77,    78,    79,     0,    80,     0,    81,
       0,    82,   608,     2,     3,     4,     5,     0,     6,     7,
       0,     0,     8,     9,    10,   209,     0,    12,    13,    14,
      15,    16,    17,    18,    19,     0,    20,     0,     0,     0,
       0,    21,    22,    23,    24,    25,    26,    27,    28,    29,
       0,     0,     0,    30,     0,    31,    32,    33,    34,    35,
      36,    37,     0,    38,    39,    40,     0,     0,    41,    42,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    43,
      44,    45,    46,    47,    48,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,     0,    80,     0,    81,     0,    82,     2,     3,
       4,     5,     0,     6,     7,     0,     0,     8,     9,    10,
      11,     0,    12,    13,    14,    15,    16,    17,    18,    19,
       0,    20,     0,     0,     0,     0,    21,    22,    23,    24,
      25,    26,    27,    28,    29,     0,     0,     0,    30,     0,
      31,    32,    33,    34,    35,    36,    37,     0,    38,    39,
      40,     0,     0,    41,    42,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    43,    44,    45,    46,    47,    48,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,     0,    80,     0,
      81,     0,    82,     2,     3,     4,     5,     0,     6,     7,
       0,     0,     8,     9,    10,     0,     0,    12,    13,    14,
      15,    16,    17,    18,    19,     0,    20,     0,     0,     0,
       0,    21,    22,    23,    24,    25,    26,    27,    28,    29,
       0,     0,     0,    30,     0,    31,    32,    33,    34,    35,
      36,    37,     0,    38,    39,    40,     0,     0,    41,    42,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    43,
      44,    45,    46,    47,    48,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,     0,    80,     0,    81,     0,    82,     2,     3,
       4,     5,     0,     6,     7,     0,     0,     8,     9,     0,
       0,     0,    12,    13,    14,    15,    16,    17,    18,    19,
       0,    20,     0,     0,     0,     0,    21,    22,    23,    24,
      25,    26,    27,    28,    29,     0,     0,     0,    30,     0,
      31,    32,    33,    34,    35,    36,    37,     0,    38,    39,
      40,     0,     0,    41,    42,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    43,    44,    45,    46,    47,    48,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,     0,    80,     0,
      81,     0,    82,     2,     3,     4,   128,     0,     6,     7,
       0,     0,     8,     9,     0,     0,     0,    12,    13,    14,
      15,     0,    17,    18,    19,     0,    20,     0,     0,     0,
       0,     0,    22,     0,    24,     0,    26,    27,    28,    29,
       0,     0,     0,    30,     0,     0,     0,    33,    34,     0,
      36,     0,     0,    38,     0,    40,     0,     0,    41,    42,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    43,
      44,    45,    46,    47,    48,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,     0,    80,     0,    81,   354,    82,     2,     3,
       4,   128,     0,     6,     7,     0,     0,     8,     9,     0,
       0,     0,    12,    13,    14,    15,     0,    17,    18,    19,
       0,    20,     0,     0,     0,     0,     0,    22,     0,    24,
       0,    26,    27,    28,    29,     0,     0,     0,    30,     0,
       0,     0,    33,    34,     0,    36,     0,     0,    38,     0,
      40,     0,     0,    41,    42,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    43,    44,    45,    46,    47,    48,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,     0,    80,     0,
      81,   655,    82,     2,     3,     4,   128,     0,     6,     7,
       0,     0,     8,     9,     0,     0,     0,    12,    13,    14,
      15,     0,    17,    18,    19,     0,     0,     0,     0,     0,
       0,     0,    22,     0,    24,     0,    26,    27,    28,    29,
       0,     0,     0,    30,     0,     0,     0,    33,    34,     0,
      36,     0,     0,    38,     0,    40,     0,     0,    41,    42,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    43,
      44,    45,    46,    47,    48,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,     0,    80,     0,    81,     0,    82,     2,     3,
       4,   128,     0,     6,     7,     0,     0,     8,     9,     0,
       0,     0,    12,    13,    14,    15,     0,    17,     0,    19,
       0,     0,     0,     0,     0,     0,     0,    22,     0,    24,
       0,    26,    27,    28,     0,     0,     0,     0,    30,     0,
       0,     0,    33,    34,     0,    36,     0,     0,    38,     0,
      40,     0,     0,    41,    42,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    43,    44,    45,    46,    47,    48,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,     0,    80,     0,
      81,     0,    82,     2,     3,     4,   128,     0,     6,     7,
       0,     0,     8,     9,     0,     0,     0,     0,     0,    14,
      15,     0,     0,     0,    19,     0,     0,     0,     0,     0,
       0,     0,    22,     0,    24,     0,    26,    27,    28,     0,
       0,     0,     0,     0,     0,     0,     0,    33,   161,     0,
      36,     0,     0,    38,     0,    40,     0,     0,    41,    42,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    43,
      44,    45,    46,    47,    48,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,     0,    80,     0,    81,     0,    82,     2,     3,
       4,   128,     0,     6,     7,     0,     0,     8,     9,     0,
       0,     0,     0,     0,    14,    15,     0,     0,     0,    19,
       0,     0,     0,     0,     0,     0,     0,    22,     0,    24,
       0,    26,    27,    28,     0,     0,     0,     0,     0,     0,
       0,     0,    33,   341,     0,    36,     0,     0,    38,     0,
      40,     0,     0,    41,    42,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    43,    44,    45,    46,    47,    48,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,     0,    80,     0,
      81,     0,    82,     2,     3,     4,   128,     0,     6,     7,
       0,     0,     8,     9,     0,     0,     0,     0,     0,    14,
      15,     0,     0,     0,    19,     0,     0,     0,     0,     0,
       0,     0,    22,     0,    24,     0,    26,    27,    28,     0,
       0,     0,     0,     0,     0,     0,     0,    33,   343,     0,
      36,     0,     0,    38,     0,    40,     0,     0,    41,    42,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    43,
      44,    45,    46,    47,    48,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,     0,    80,     0,    81,     0,    82,     2,     3,
       4,   128,     0,     6,     7,     0,     0,     8,     9,     0,
       0,     0,     0,     0,    14,    15,     0,     0,     0,    19,
       0,     0,     0,     0,     0,     0,     0,    22,     0,    24,
       0,    26,    27,    28,     0,     0,     0,     0,     0,     0,
       0,     0,    33,     0,     0,    36,     0,     0,    38,     0,
      40,     0,     0,    41,    42,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    43,    44,    45,    46,    47,    48,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,     0,    80,     0,
      81,     0,    82,     2,     3,     4,   128,     0,     6,     7,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    14,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    24,     0,    26,     0,    28,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    40,     0,     0,    41,    42,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   129,
     130,   131,   132,   133,   134,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   135,     0,   152,
       0,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,   136,
     137,   138,   139,    71,    72,    73,    74,    75,    76,    77,
      78,    79,     0,    80,     0,    81,     0,    82,     2,     3,
       4,   128,     0,     6,     7,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    14,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    24,
       0,    26,     0,    28,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      40,     0,     0,    41,    42,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   129,   130,   131,   132,   133,   134,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   135,     0,   163,     0,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,   136,   137,   138,   139,    71,    72,
      73,    74,    75,    76,    77,    78,    79,     0,    80,     0,
      81,     0,    82,     2,     3,     4,   128,     0,     6,     7,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    14,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    24,     0,    26,     0,    28,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    40,     0,     0,    41,    42,
     176,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   129,
     130,   131,   132,   133,   134,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   135,     0,     0,
       0,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,   136,
     137,   138,   139,    71,    72,    73,    74,    75,    76,    77,
      78,    79,     0,    80,     0,    81,     0,    82,     2,     3,
       4,   128,     0,     6,     7,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    14,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    24,
       0,    26,     0,    28,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      40,     0,     0,    41,    42,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   129,   130,   131,   132,   133,   134,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   135,     0,     0,     0,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,   136,   137,   138,   139,    71,    72,
      73,    74,    75,    76,    77,    78,    79,     0,    80,     0,
      81,   198,    82,     2,     3,     4,   128,     0,     6,     7,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    14,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    24,     0,    26,     0,    28,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    40,     0,     0,    41,    42,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   129,
     130,   131,   132,   133,   134,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   135,     0,     0,
       0,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,   136,
     137,   138,   139,    71,    72,    73,    74,    75,    76,    77,
      78,    79,     0,    80,     0,    81,     0,    82,     2,     3,
       4,   202,     0,     6,     7,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    14,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    24,
       0,    26,     0,    28,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      40,     0,     0,    41,    42,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   129,   130,   131,   132,   133,   134,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   135,     0,     0,     0,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,   136,   137,   138,   139,    71,    72,
      73,    74,    75,    76,    77,    78,    79,     0,    80,     0,
      81,     0,    82,     2,     3,     4,   128,     0,     6,     7,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    14,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    24,     0,    26,     0,    28,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    40,     0,     0,    41,    42,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   269,
     270,   271,   272,   133,   134,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   135,     0,     0,
       0,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,   136,
     137,   138,   139,    71,    72,    73,    74,    75,    76,    77,
      78,    79,     0,    80,     0,    81,     0,    82,     2,     3,
       4,   128,     0,     6,     7,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    14,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    24,
       0,    26,     0,    28,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      40,     0,     0,    41,    42,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   -77,   -77,   -77,   -77,   133,   134,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   135,     0,     0,     0,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,   136,   137,   138,   139,    71,    72,
      73,    74,    75,    76,    77,    78,    79,     0,    80,     0,
      81,     0,    82,     2,     3,     4,   128,     0,     6,     7,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    14,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    24,     0,    26,     0,    28,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    40,     0,     0,    41,    42,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    43,
      44,    45,    46,    47,    48,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,     0,    80,     0,    81,     0,    82,     2,     3,
       4,   128,     0,     6,     7,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    14,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    24,
       0,    26,     0,    28,     0,     0,     0,     0,     0,     2,
       3,     4,   128,     0,     6,     7,     0,     0,     0,     0,
      40,     0,     0,    41,    42,    14,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      24,     0,    26,     0,    28,     0,     0,     0,     0,     0,
       0,   369,     0,     0,   -77,   -77,   -77,   -77,    47,    48,
       0,    40,     0,     0,    41,    42,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,     0,    80,     0,
      81,     0,    82,     0,     0,   300,     0,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,   301,     0,     0,   300,    71,
      72,    73,    74,    75,    76,    77,    78,    79,     0,    80,
       0,    81,     0,    82,     0,     0,     0,     0,   301,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   300,     0,   339,   340,     0,     0,
     322,   323,   324,   325,   326,   327,   328,   329,   330,   331,
     332,   333,   334,   335,   301,   337,   338,     0,     0,   339,
     340,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   300,     0,     0,   382,   383,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   301,   302,   303,   304,   305,
     306,   307,   308,   309,   310,   311,   312,   313,   314,   315,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,     0,     0,   339,   340,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   300,     0,   339,   340,   405,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   301,     0,     0,   224,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   225,     0,   665,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   302,   303,   304,   305,
     306,   307,   308,   309,   310,   311,   312,   313,   314,   315,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,   300,     0,   339,   340,     0,     0,   -77,
     -77,   -77,   -77,   235,   236,   237,   238,   239,   240,   241,
     242,   243,   244,   301,   246,   247,     0,     0,     0,   248,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   224,     0,     0,   674,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   225,   302,   303,   304,   305,   306,
     307,   308,   309,   310,   311,   312,   313,   314,   315,   316,
     317,   318,   319,   320,   321,   322,   323,   324,   325,   326,
     327,   328,   329,   330,   331,   332,   333,   334,   335,   336,
     337,   338,   300,     0,   339,   340,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   -77,   -77,
       0,     0,   301,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   224,   407,   408,   248,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   225,   302,   303,   304,   305,   306,   307,
     308,   309,   310,   311,   312,   313,   314,   315,   316,   317,
     318,   319,   320,   321,   322,   323,   324,   325,   326,   327,
     328,   329,   330,   331,   332,   333,   334,   335,   336,   337,
     338,   300,     0,   339,   340,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   301,     0,     0,     0,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,     0,
     246,   247,     0,   596,   597,   248,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     300,     0,   339,   340,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     301,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   616,   617,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   300,
       0,   339,   340,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   301,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   624,   625,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   302,   303,   304,   305,   306,   307,   308,   309,   310,
     311,   312,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   322,   323,   324,   325,   326,   327,   328,   329,   330,
     331,   332,   333,   334,   335,   336,   337,   338,   300,     0,
     339,   340,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   301,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     626,   627,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     302,   303,   304,   305,   306,   307,   308,   309,   310,   311,
     312,   313,   314,   315,   316,   317,   318,   319,   320,   321,
     322,   323,   324,   325,   326,   327,   328,   329,   330,   331,
     332,   333,   334,   335,   336,   337,   338,   300,     0,   339,
     340,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   301,     0,     0,
       0,     0,   224,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   628,
     629,     0,   225,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   302,
     303,   304,   305,   306,   307,   308,   309,   310,   311,   312,
     313,   314,   315,   316,   317,   318,   319,   320,   321,   322,
     323,   324,   325,   326,   327,   328,   329,   330,   331,   332,
     333,   334,   335,   336,   337,   338,   226,   227,   339,   340,
       0,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,   238,   239,   240,   241,   242,   243,   244,   245,   246,
     247,   224,     0,     0,   248,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   630,   631,
       0,   225,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   600,   601,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   226,   227,     0,     0,     0,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,   244,   245,   246,   247,
     300,     0,     0,   248,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     301,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   300,   602,   603,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,     0,
       0,   339,   340,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     300,   724,   339,   340,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     301,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   300,   729,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,     0,
       0,   339,   340,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     300,   731,   339,   340,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     301,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   300,   733,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,     0,
       0,   339,   340,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     300,   734,   339,   340,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     301,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   300,   735,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,     0,
       0,   339,   340,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     300,   736,   339,   340,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     301,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   300,   755,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,     0,
       0,   339,   340,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     224,   756,   339,   340,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     225,     0,     0,     0,     0,   224,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   757,     0,     0,   225,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   226,   227,     0,     0,     0,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   243,   244,   245,   246,   247,   226,
     227,     0,   248,     0,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     244,   245,   246,   247,   224,   522,     0,   248,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   725,     0,     0,   225,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   726,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   226,   227,
       0,     0,     0,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,     0,     0,     0,   248,     0,     0,     0,
     523,   524,   525,   526,   527,   528,   529,   530,   531,   532,
     533,   534,   535,   536,   537,   538,   539,   540,   541,   542,
     543,   544,   545,   546,   547,   548,   549,   550,   551,   552,
     553,   300,   554,     0,     0,   727,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   301,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   353,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     300,     0,   339,   340,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     301,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     456,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   300,
       0,   339,   340,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   301,
       0,     0,   649,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   302,   303,   304,   305,   306,   307,   308,   309,   310,
     311,   312,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   322,   323,   324,   325,   326,   327,   328,   329,   330,
     331,   332,   333,   334,   335,   336,   337,   338,   300,     0,
     339,   340,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   301,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   689,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     302,   303,   304,   305,   306,   307,   308,   309,   310,   311,
     312,   313,   314,   315,   316,   317,   318,   319,   320,   321,
     322,   323,   324,   325,   326,   327,   328,   329,   330,   331,
     332,   333,   334,   335,   336,   337,   338,   300,     0,   339,
     340,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   301,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   753,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   302,
     303,   304,   305,   306,   307,   308,   309,   310,   311,   312,
     313,   314,   315,   316,   317,   318,   319,   320,   321,   322,
     323,   324,   325,   326,   327,   328,   329,   330,   331,   332,
     333,   334,   335,   336,   337,   338,   300,     0,   339,   340,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   301,     0,     0,   762,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   302,   303,
     304,   305,   306,   307,   308,   309,   310,   311,   312,   313,
     314,   315,   316,   317,   318,   319,   320,   321,   322,   323,
     324,   325,   326,   327,   328,   329,   330,   331,   332,   333,
     334,   335,   336,   337,   338,   300,     0,   339,   340,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   301,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   300,     0,     0,     0,
     763,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,     0,     0,   339,   340,   302,   303,
     304,   305,   306,   307,   308,   309,   310,   311,   312,   313,
     314,   315,   316,   317,   318,   319,   320,   321,   322,   323,
     324,   325,   326,   327,   328,   329,   330,   331,   332,   333,
     334,   335,   336,   337,   338,   300,     0,   339,   340,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   301,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   300,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   301,     0,     0,     0,     0,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,     0,     0,   339,   340,   305,   306,
     307,   308,   309,   310,   311,   312,   313,   314,   315,   316,
     317,   318,   319,   320,   321,   322,   323,   324,   325,   326,
     327,   328,   329,   330,   331,   332,   333,   334,   335,   336,
     337,   338,   300,     0,   339,   340,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   301,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   300,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   301,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   307,
     308,   309,   310,   311,   312,   313,   314,   315,   316,   317,
     318,   319,   320,   321,   322,   323,   324,   325,   326,   327,
     328,   329,   330,   331,   332,   333,   334,   335,   336,   337,
     338,     0,     0,   339,   340,   308,   309,   310,   311,   312,
     313,   314,   315,   316,   317,   318,   319,   320,   321,   322,
     323,   324,   325,   326,   327,   328,   329,   330,   331,   332,
     333,   334,   335,   336,   337,   338,   300,     0,   339,   340,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   301,     0,     0,     0,
       0,     0,     0,   300,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   301,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   -77,   -77,   -77,   -77,   -77,   -77,
     -77,   -77,   316,   317,   318,   319,   320,   321,   322,   323,
     324,   325,   326,   327,   328,   329,   330,   331,   332,   333,
     334,   335,   336,   337,   338,     0,     0,   339,   340,   -77,
     -77,   -77,   319,   320,   321,   322,   323,   324,   325,   326,
     327,   328,   329,   330,   331,   332,   333,   334,   335,   336,
     337,   338,     0,     0,   339,   340
};

static const yytype_int16 yycheck[] =
{
      20,    81,    35,   160,    24,   159,   159,    23,    30,    86,
      40,    31,   405,   166,    61,     6,    10,    11,    16,     6,
     109,    41,    42,   144,    43,    44,    45,    46,    47,    48,
       7,   165,    16,    16,   248,     7,     7,   171,   106,     7,
       7,   118,    16,   152,   106,   108,     7,    43,   663,   108,
       6,   378,     6,   106,   163,    27,    27,    60,   147,   168,
      58,    81,    82,   108,    55,   222,    27,    58,    55,   179,
       8,     9,   106,     6,    58,    58,   106,   107,   108,   109,
     110,   111,   112,   113,    58,    16,   116,   150,    12,    16,
     200,   150,    58,   708,   204,    79,     7,   712,     7,    55,
       7,    55,   149,   123,   124,   150,   126,     6,   106,   129,
     130,   131,   132,   133,   134,   135,    27,     8,    27,   145,
      27,   154,   106,   106,   146,   147,   340,    58,   106,   744,
     207,    58,   106,   105,   105,   151,   108,   109,   109,   248,
     173,   109,   109,   159,   106,   167,   149,   108,   109,   147,
     166,   766,   150,   168,   145,   149,   172,    16,   145,   149,
     150,   106,   376,   147,   274,   144,    42,   146,     6,   147,
     149,   147,   282,   147,   194,   106,   195,   196,   197,   106,
      40,    41,    28,    29,    44,   147,    97,    98,    99,   100,
     101,   102,   345,   104,   105,    42,   109,   108,   109,    58,
     109,   221,   109,   149,   150,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   243,   244,   245,   246,   247,   363,
       6,   340,   147,   248,     6,   147,    16,   147,   348,   566,
       0,   147,   147,   377,   144,   265,    79,   106,   575,   269,
     270,   271,   272,   147,   147,   147,    42,   367,   278,   147,
     147,   147,   147,    60,   284,   109,   149,   376,    61,   151,
     149,   146,   665,   293,   294,   295,   296,   149,    58,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     297,   648,    16,     6,    16,   340,   106,    60,   408,   345,
     350,   149,   148,   353,   411,   149,    42,    16,    16,   361,
      16,    42,   362,    16,    79,    16,    60,    16,   149,    16,
     108,    79,    83,   373,   374,    79,    20,   148,     7,   149,
     148,   376,   382,    79,    58,   385,    58,   151,   151,     6,
     148,   109,   147,   393,     7,   395,   396,   407,    27,    58,
      58,   148,    58,   403,   109,    58,   406,    58,    60,    58,
     149,    58,   148,   148,    27,    42,   106,     6,    83,   150,
      60,   728,   512,   149,    79,    57,    83,   149,   148,   563,
     563,   150,   106,   147,   106,    16,    16,   570,   147,   146,
       8,   555,     6,     6,   147,    83,   108,   106,   106,    79,
     106,   151,   452,   106,     7,   106,   456,   106,   458,   106,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,    27,   104,   105,    58,    58,   108,
     109,   614,   148,    60,    97,    98,    99,   100,   101,   102,
      16,   104,   105,     8,   584,   149,   109,    57,   588,     6,
     146,   207,   639,   636,    10,   702,   508,   509,   614,   636,
      13,    16,   172,   452,   297,   284,   516,   668,   668,   591,
      -1,   409,   518,    16,    16,   106,   106,   278,    81,    82,
     590,   563,    58,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,    58,    -1,   568,   109,   557,   558,    21,
     393,    16,     0,   576,    26,    58,    58,   563,   393,    31,
      16,    33,    10,    35,   570,    -1,    -1,    16,    -1,    -1,
     106,    -1,   578,    45,    46,    23,    -1,    49,    -1,    51,
      -1,    -1,    30,    -1,    -1,   148,   149,   597,    -1,    -1,
     599,   106,   601,    58,   603,    43,    44,    45,    46,    47,
      48,   701,    58,   106,   106,    -1,    -1,   617,   614,    58,
      -1,   621,    -1,    -1,    -1,   625,    -1,   627,    -1,   629,
      -1,   631,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     636,    -1,    -1,    -1,    -1,   658,    -1,    -1,    86,   662,
      -1,   106,    -1,    -1,   654,    -1,    -1,    -1,    -1,    -1,
     106,    -1,    -1,   723,    -1,    -1,   666,   106,    -1,    -1,
     670,   671,    -1,    -1,   674,    -1,    -1,   677,    -1,    -1,
     118,    -1,    -1,    -1,    -1,    -1,    -1,   687,    -1,   689,
     703,    -1,    -1,    -1,    -1,    -1,   696,   697,    -1,    -1,
      -1,    -1,    -1,    -1,   704,   705,    -1,   707,   146,   147,
     710,    -1,    -1,   151,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   159,   160,    -1,    -1,    -1,    -1,    -1,   166,   167,
      -1,    -1,    -1,    -1,   172,    -1,    -1,    -1,    -1,    -1,
      -1,   754,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   752,   753,    -1,    -1,    -1,   195,   196,   197,
      -1,    -1,    -1,   763,    -1,    -1,    -1,    -1,    -1,   207,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   222,    -1,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,   244,   245,   246,   247,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    34,    -1,    36,    -1,    38,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    55,    -1,    -1,    58,    59,    -1,
      61,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   345,    89,    90,
      91,    92,    93,    94,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   361,    -1,    -1,   107,    -1,    -1,    -1,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,    -1,   143,    -1,   145,    -1,   147,    -1,    -1,    -1,
     151,    -1,    -1,   411,     3,     4,     5,     6,    -1,     8,
       9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    34,    -1,    36,    -1,    38,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,    -1,    58,
      59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      89,    90,    91,    92,    93,    94,    -1,    -1,    -1,    -1,
     508,   509,    -1,    -1,    -1,    -1,    -1,    -1,   107,    -1,
     518,    -1,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,    -1,   143,    -1,   145,    -1,   147,    -1,
      -1,    -1,   151,    -1,    -1,   563,    -1,    -1,    -1,    -1,
      -1,    -1,   570,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     578,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   599,    -1,   601,    -1,   603,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     7,    -1,    -1,   614,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     0,     1,    27,     3,     4,     5,     6,   636,     8,
       9,   639,    -1,    12,    13,    14,    15,    -1,    17,    18,
      19,    20,    21,    22,    23,    24,    -1,    26,    -1,    -1,
      -1,    -1,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    -1,    -1,    -1,    43,    -1,    45,    46,    47,    48,
      49,    50,    51,    -1,    53,    54,    55,    81,    82,    58,
      59,    -1,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,    -1,    -1,    -1,   109,    -1,    -1,    -1,    -1,
      89,    90,    91,    92,    93,    94,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,    -1,   143,    -1,   145,     1,   147,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    12,    13,
      -1,    -1,    -1,    17,    18,    19,    20,    -1,    22,    23,
      24,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,    -1,
      34,    -1,    36,    37,    38,    39,    -1,    -1,    -1,    43,
      -1,    -1,    -1,    47,    48,    -1,    50,    -1,    -1,    53,
      -1,    55,    -1,    -1,    58,    59,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    89,    90,    91,    92,    93,
      94,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,    -1,   143,
      -1,   145,    -1,   147,   148,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    34,    -1,    36,    -1,
      38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,    -1,
      58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    89,    90,    91,    92,    93,    94,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,
      -1,    -1,    -1,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,    -1,   143,    -1,   145,    -1,   147,
     148,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    34,    -1,    36,    -1,    38,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    55,    -1,    -1,    58,    59,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,    90,    91,
      92,    93,    94,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   107,    -1,    -1,    -1,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
      -1,   143,    -1,   145,    -1,   147,   148,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    34,    -1,
      36,    -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,
      -1,    -1,    58,    59,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    89,    90,    91,    92,    93,    94,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   107,    -1,    -1,    -1,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,    -1,   143,    -1,   145,
      -1,   147,   148,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    12,    13,    14,    15,    -1,    17,    18,    19,
      20,    21,    22,    23,    24,    -1,    26,    -1,    -1,    -1,
      -1,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      -1,    -1,    -1,    43,    -1,    45,    46,    47,    48,    49,
      50,    51,    -1,    53,    54,    55,    -1,    -1,    58,    59,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,
      90,    91,    92,    93,    94,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,    -1,   143,    -1,   145,    -1,   147,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    12,    13,    14,
      15,    -1,    17,    18,    19,    20,    21,    22,    23,    24,
      -1,    26,    -1,    -1,    -1,    -1,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    -1,    -1,    -1,    43,    -1,
      45,    46,    47,    48,    49,    50,    51,    -1,    53,    54,
      55,    -1,    -1,    58,    59,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    90,    91,    92,    93,    94,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,    -1,   143,    -1,
     145,    -1,   147,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    12,    13,    14,    -1,    -1,    17,    18,    19,
      20,    21,    22,    23,    24,    -1,    26,    -1,    -1,    -1,
      -1,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      -1,    -1,    -1,    43,    -1,    45,    46,    47,    48,    49,
      50,    51,    -1,    53,    54,    55,    -1,    -1,    58,    59,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,
      90,    91,    92,    93,    94,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,    -1,   143,    -1,   145,    -1,   147,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    12,    13,    -1,
      -1,    -1,    17,    18,    19,    20,    21,    22,    23,    24,
      -1,    26,    -1,    -1,    -1,    -1,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    -1,    -1,    -1,    43,    -1,
      45,    46,    47,    48,    49,    50,    51,    -1,    53,    54,
      55,    -1,    -1,    58,    59,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    90,    91,    92,    93,    94,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,    -1,   143,    -1,
     145,    -1,   147,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    12,    13,    -1,    -1,    -1,    17,    18,    19,
      20,    -1,    22,    23,    24,    -1,    26,    -1,    -1,    -1,
      -1,    -1,    32,    -1,    34,    -1,    36,    37,    38,    39,
      -1,    -1,    -1,    43,    -1,    -1,    -1,    47,    48,    -1,
      50,    -1,    -1,    53,    -1,    55,    -1,    -1,    58,    59,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,
      90,    91,    92,    93,    94,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,    -1,   143,    -1,   145,   146,   147,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    12,    13,    -1,
      -1,    -1,    17,    18,    19,    20,    -1,    22,    23,    24,
      -1,    26,    -1,    -1,    -1,    -1,    -1,    32,    -1,    34,
      -1,    36,    37,    38,    39,    -1,    -1,    -1,    43,    -1,
      -1,    -1,    47,    48,    -1,    50,    -1,    -1,    53,    -1,
      55,    -1,    -1,    58,    59,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    90,    91,    92,    93,    94,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,    -1,   143,    -1,
     145,   146,   147,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    12,    13,    -1,    -1,    -1,    17,    18,    19,
      20,    -1,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    32,    -1,    34,    -1,    36,    37,    38,    39,
      -1,    -1,    -1,    43,    -1,    -1,    -1,    47,    48,    -1,
      50,    -1,    -1,    53,    -1,    55,    -1,    -1,    58,    59,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,
      90,    91,    92,    93,    94,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,    -1,   143,    -1,   145,    -1,   147,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    12,    13,    -1,
      -1,    -1,    17,    18,    19,    20,    -1,    22,    -1,    24,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,    -1,    34,
      -1,    36,    37,    38,    -1,    -1,    -1,    -1,    43,    -1,
      -1,    -1,    47,    48,    -1,    50,    -1,    -1,    53,    -1,
      55,    -1,    -1,    58,    59,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    90,    91,    92,    93,    94,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,    -1,   143,    -1,
     145,    -1,   147,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    12,    13,    -1,    -1,    -1,    -1,    -1,    19,
      20,    -1,    -1,    -1,    24,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    32,    -1,    34,    -1,    36,    37,    38,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,
      50,    -1,    -1,    53,    -1,    55,    -1,    -1,    58,    59,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,
      90,    91,    92,    93,    94,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,    -1,   143,    -1,   145,    -1,   147,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    12,    13,    -1,
      -1,    -1,    -1,    -1,    19,    20,    -1,    -1,    -1,    24,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,    -1,    34,
      -1,    36,    37,    38,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    47,    48,    -1,    50,    -1,    -1,    53,    -1,
      55,    -1,    -1,    58,    59,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    90,    91,    92,    93,    94,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,    -1,   143,    -1,
     145,    -1,   147,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    12,    13,    -1,    -1,    -1,    -1,    -1,    19,
      20,    -1,    -1,    -1,    24,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    32,    -1,    34,    -1,    36,    37,    38,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,
      50,    -1,    -1,    53,    -1,    55,    -1,    -1,    58,    59,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,
      90,    91,    92,    93,    94,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,    -1,   143,    -1,   145,    -1,   147,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    12,    13,    -1,
      -1,    -1,    -1,    -1,    19,    20,    -1,    -1,    -1,    24,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,    -1,    34,
      -1,    36,    37,    38,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    47,    -1,    -1,    50,    -1,    -1,    53,    -1,
      55,    -1,    -1,    58,    59,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    90,    91,    92,    93,    94,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,    -1,   143,    -1,
     145,    -1,   147,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    34,    -1,    36,    -1,    38,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    55,    -1,    -1,    58,    59,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,
      90,    91,    92,    93,    94,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,    -1,   109,
      -1,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,    -1,   143,    -1,   145,    -1,   147,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    34,
      -1,    36,    -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      55,    -1,    -1,    58,    59,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    90,    91,    92,    93,    94,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   107,    -1,   109,    -1,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,    -1,   143,    -1,
     145,    -1,   147,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    34,    -1,    36,    -1,    38,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    55,    -1,    -1,    58,    59,
      60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,
      90,    91,    92,    93,    94,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,    -1,    -1,
      -1,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,    -1,   143,    -1,   145,    -1,   147,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    34,
      -1,    36,    -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      55,    -1,    -1,    58,    59,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    90,    91,    92,    93,    94,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   107,    -1,    -1,    -1,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,    -1,   143,    -1,
     145,   146,   147,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    34,    -1,    36,    -1,    38,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    55,    -1,    -1,    58,    59,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,
      90,    91,    92,    93,    94,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,    -1,    -1,
      -1,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,    -1,   143,    -1,   145,    -1,   147,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    34,
      -1,    36,    -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      55,    -1,    -1,    58,    59,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    90,    91,    92,    93,    94,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   107,    -1,    -1,    -1,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,    -1,   143,    -1,
     145,    -1,   147,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    34,    -1,    36,    -1,    38,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    55,    -1,    -1,    58,    59,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,
      90,    91,    92,    93,    94,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,    -1,    -1,
      -1,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,    -1,   143,    -1,   145,    -1,   147,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    34,
      -1,    36,    -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      55,    -1,    -1,    58,    59,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    90,    91,    92,    93,    94,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   107,    -1,    -1,    -1,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,    -1,   143,    -1,
     145,    -1,   147,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    34,    -1,    36,    -1,    38,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    55,    -1,    -1,    58,    59,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,
      90,    91,    92,    93,    94,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,    -1,   143,    -1,   145,    -1,   147,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    34,
      -1,    36,    -1,    38,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,
      55,    -1,    -1,    58,    59,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      34,    -1,    36,    -1,    38,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    89,    90,    91,    92,    93,    94,
      -1,    55,    -1,    -1,    58,    59,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,    -1,   143,    -1,
     145,    -1,   147,    -1,    -1,     7,    -1,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,    27,    -1,    -1,     7,   133,
     134,   135,   136,   137,   138,   139,   140,   141,    -1,   143,
      -1,   145,    -1,   147,    -1,    -1,    -1,    -1,    27,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,     7,    -1,   108,   109,    -1,    -1,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,    27,   104,   105,    -1,    -1,   108,
     109,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     7,    -1,    -1,   150,   151,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    27,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,    -1,    -1,   108,   109,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,     7,    -1,   108,   109,   151,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    27,    -1,    -1,     7,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,   151,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,     7,    -1,   108,   109,    -1,    -1,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,    27,   104,   105,    -1,    -1,    -1,   109,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     7,    -1,    -1,   150,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    27,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,     7,    -1,   108,   109,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    81,    82,
      -1,    -1,    27,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,     7,   148,   149,   109,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    27,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,     7,    -1,   108,   109,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    27,    -1,    -1,    -1,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,    -1,
     104,   105,    -1,   148,   149,   109,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
       7,    -1,   108,   109,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   148,   149,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,     7,
      -1,   108,   109,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   148,   149,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,     7,    -1,
     108,   109,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     148,   149,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,     7,    -1,   108,
     109,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,
      -1,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   148,
     149,    -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,    81,    82,   108,   109,
      -1,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,     7,    -1,    -1,   109,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   148,   149,
      -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   148,   149,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    81,    82,    -1,    -1,    -1,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
       7,    -1,    -1,   109,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     7,   148,   149,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    27,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,    -1,
      -1,   108,   109,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
       7,   148,   108,   109,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     7,   148,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    27,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,    -1,
      -1,   108,   109,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
       7,   148,   108,   109,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     7,   148,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    27,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,    -1,
      -1,   108,   109,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
       7,   148,   108,   109,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     7,   148,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    27,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,    -1,
      -1,   108,   109,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
       7,   148,   108,   109,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     7,   148,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    27,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,    -1,
      -1,   108,   109,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
       7,   148,   108,   109,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      27,    -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   148,    -1,    -1,    27,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    81,    82,    -1,    -1,    -1,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,    81,
      82,    -1,   109,    -1,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,     7,     6,    -1,   109,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   148,    -1,    -1,    27,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   148,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    81,    82,
      -1,    -1,    -1,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,    -1,    -1,    -1,   109,    -1,    -1,    -1,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,     7,   143,    -1,    -1,   148,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    52,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
       7,    -1,   108,   109,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,     7,
      -1,   108,   109,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,
      -1,    -1,    30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,     7,    -1,
     108,   109,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    57,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,     7,    -1,   108,
     109,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    57,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,     7,    -1,   108,   109,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,    30,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,     7,    -1,   108,   109,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,
      52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    27,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,    -1,    -1,   108,   109,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,     7,    -1,   108,   109,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,    -1,    -1,   108,   109,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,     7,    -1,   108,   109,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,    -1,    -1,   108,   109,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,     7,    -1,   108,   109,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,
      -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,    -1,    -1,   108,   109,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,    -1,    -1,   108,   109
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     4,     5,     6,     8,     9,    12,    13,
      14,    15,    17,    18,    19,    20,    21,    22,    23,    24,
      26,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      43,    45,    46,    47,    48,    49,    50,    51,    53,    54,
      55,    58,    59,    89,    90,    91,    92,    93,    94,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     143,   145,   147,   153,   154,   155,   156,   158,   159,   161,
     162,   167,   168,   169,   170,   171,   172,   173,   182,   187,
     188,   190,   196,   198,   199,   200,   203,   204,   205,   208,
     216,   217,   224,   227,   229,   230,   231,   234,   144,   106,
     106,   106,    16,    58,    79,   106,   147,   202,     6,    89,
      90,    91,    92,    93,    94,   107,   129,   130,   131,   132,
     195,   197,   198,   201,   159,    43,   189,   189,   106,     6,
      12,    58,   109,   197,     6,   187,   197,     8,   106,   145,
      42,    48,   190,   109,   197,     6,   147,    42,   109,   238,
     240,     6,   147,     6,   106,   202,    60,   197,   225,   226,
      61,   151,   197,   218,   220,   221,   222,   223,   196,   196,
     196,   196,   196,   196,   147,   147,   147,   147,   146,   194,
     195,   197,     6,   197,   206,   207,     0,   144,   157,    15,
     158,    21,    26,    31,    33,    35,    45,    46,    49,    51,
     160,    79,   108,   150,     7,    27,    81,    82,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   109,   106,
     202,   106,   202,   106,   202,   106,   202,   106,   202,   106,
     202,   106,   202,   106,   202,   147,   106,   202,   158,    89,
      90,    91,    92,   191,   192,   193,   197,   197,   147,   148,
     197,   232,   233,    16,    58,   106,   197,   197,   197,   197,
     197,   197,   197,   147,   147,   147,   147,    10,    11,   149,
       7,    27,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   108,
     109,    48,   190,    48,   190,   147,   176,   185,   186,   187,
     200,   238,   150,    52,   146,   168,   182,   187,   235,   237,
     188,    42,   200,   106,   176,   182,   183,   184,   190,    45,
     199,   200,   239,    40,    41,    44,   109,   106,   176,   185,
     238,   106,   150,   151,    60,   149,   179,   151,   194,   219,
      61,   149,    61,   151,   180,   149,   150,   179,   197,   196,
     196,   196,   146,   149,   179,   151,   150,   148,   149,   149,
     179,   156,   158,   197,   188,     6,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     239,   106,   106,   106,   106,   106,   106,   106,   106,   197,
     106,    60,   149,   179,   148,   232,    57,   148,   149,   179,
     191,   197,   197,   197,   197,   201,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   239,    42,    42,
       1,   177,   178,   181,   182,   187,    79,    60,   149,   179,
     197,    79,     6,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   143,   241,   197,    83,    79,   150,
     144,   146,   149,   236,   190,   197,   176,    20,   174,   148,
     149,   179,   197,   197,   239,   176,   174,   148,    79,   197,
       6,    55,   209,   210,   211,   212,   213,   214,   215,   197,
     151,   151,   222,   223,   197,   197,   148,   149,   148,   149,
     148,   149,   148,   149,   209,   197,   106,   202,   148,   194,
       6,   207,   148,   158,   147,   238,   148,   149,   193,   148,
     197,   147,   197,    60,   148,   149,   148,   149,   148,   149,
     148,   149,   190,   190,   148,   148,   149,   179,   197,    42,
     187,     6,    55,    58,   145,   163,   164,   106,   176,    30,
     228,   197,   197,     6,    83,   146,   168,   237,   174,   150,
     238,   182,   174,   238,   187,   151,    79,    60,   149,   179,
      57,    83,   149,   179,   150,   194,   219,   150,   197,   196,
     196,   196,   146,   106,   148,   177,   197,   147,   197,    57,
     197,   197,   197,   197,   181,   188,   147,   147,     8,   165,
       6,   166,   108,   174,    28,    29,   197,    83,   238,     6,
      79,   175,   238,   175,   209,   197,   212,   213,   197,   197,
       6,    55,   197,   151,   148,   148,   148,   148,   148,   148,
     197,   148,   197,   148,   148,   148,   148,   197,   197,    60,
     149,   149,   179,   164,   238,   197,   197,   197,   175,   197,
     175,    60,    57,    57,   174,   148,   148,   148,     8,     6,
     146,   175,    30,    52,   197,   197,   238,   197,   175
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   152,   153,   154,   154,   155,   155,   155,   155,   155,
     155,   156,   156,   157,   157,   158,   158,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   161,   162,   162,   162,   162,
     162,   162,   163,   163,   164,   164,   164,   165,   165,   165,
     166,   166,   166,   167,   168,   168,   169,   169,   169,   170,
     170,   171,   171,   171,   171,   172,   172,   173,   173,   174,
     174,   175,   175,   176,   176,   176,   177,   177,   178,   178,
     179,   179,   180,   180,   181,   181,   182,   183,   184,   184,
     185,   186,   186,   187,   187,   187,   187,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   189,   189,   190,
     190,   190,   190,   190,   190,   190,   190,   190,   190,   191,
     192,   192,   193,   193,   193,   193,   193,   194,   195,   195,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   198,   198,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   200,   200,   201,   201,
     202,   202,   202,   202,   203,   203,   204,   204,   205,   206,
     206,   207,   208,   209,   210,   211,   211,   211,   211,   211,
     211,   212,   212,   213,   214,   215,   215,   215,   215,   216,
     216,   217,   217,   217,   218,   218,   218,   219,   219,   220,
     221,   221,   221,   222,   223,   223,   223,   224,   224,   225,
     226,   226,   226,   226,   227,   227,   228,   228,   229,   229,
     229,   229,   229,   229,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   229,   229,   230,   230,   231,   231,
     231,   231,   231,   231,   231,   232,   233,   233,   233,   233,
     234,   234,   234,   235,   235,   235,   235,   236,   236,   237,
     237,   238,   238,   239,   239,   239,   240,   240,   241,   241,
     241,   241,   241,   241,   241,   241,   241,   241,   241,   241,
     241,   241,   241,   241,   241,   241,   241,   241,   241,   241,
     241,   241,   241,   241,   241,   241,   241,   241,   241,   241,
     241,   241
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     2,     3,     4,     2,
       3,     1,     2,     0,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     3,     3,     5,
       7,     5,     1,     3,     4,     4,     4,     0,     1,     3,
       0,     1,     3,     3,     2,     4,     3,     4,     4,     2,
       4,     6,     6,     7,     7,     8,     9,     3,     5,     0,
       3,     0,     2,     0,     3,     3,     0,     2,     1,     3,
       0,     1,     0,     1,     1,     1,     4,     2,     1,     3,
       2,     1,     3,     1,     6,     3,     3,     1,     2,     3,
       3,     3,     4,     5,     5,     2,     1,     0,     1,     1,
       1,     1,     1,     1,     4,     4,     1,     1,     1,     2,
       1,     3,     1,     1,     1,     1,     1,     2,     1,     3,
       1,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     2,     2,     2,     2,     2,     2,     6,     6,     6,
       6,     4,     4,     4,     4,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     2,
       2,     2,     2,     2,     2,     2,     6,     6,     6,     6,
       4,     4,     4,     4,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     2,     1,     1,     3,     4,     4,
       5,     1,     2,     2,     3,     1,     2,     2,     3,     1,
       2,     1,     2,     1,     2,     1,     2,     1,     2,     1,
       2,     2,     3,     1,     2,     2,     3,     1,     2,     2,
       3,     1,     2,     2,     3,     1,     2,     2,     3,     1,
       2,     2,     3,     1,     1,     2,     2,     3,     1,     2,
       2,     3,     1,     2,     2,     3,     1,     2,     2,     3,
       3,     1,     4,     2,     2,     3,     4,     5,     4,     1,
       3,     3,     5,     1,     2,     1,     1,     3,     3,     3,
       5,     3,     5,     3,     2,     1,     1,     3,     3,     2,
       3,     2,     3,     3,     2,     3,     5,     1,     3,     2,
       1,     2,     3,     2,     1,     3,     3,     7,     5,     2,
       1,     3,     3,     5,     5,     8,     0,     5,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     6,     4,     3,     4,
       1,     4,     7,     5,     8,     2,     1,     3,     3,     5,
       5,     6,     7,     1,     1,     3,     3,     1,     1,     1,
       3,     0,     1,     1,     1,     1,     2,     3,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (&yylloc, parm, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YY_LOCATION_PRINT
#  if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#   define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

#  else
#   define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#  endif
# endif /* !defined YY_LOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location, parm); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void *parm)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  YY_USE (parm);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void *parm)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YY_LOCATION_PRINT (yyo, *yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp, parm);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule, void *parm)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]), parm);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, parm); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, void *parm)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  YY_USE (parm);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (void *parm)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

/* User initialization code.  */
{
  GCLock lock;
  yylloc.filename(ASTString(static_cast<ParserState*>(parm)->filename));
}


  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, &yylloc, SCANNER);
    }

  if (yychar <= END)
    {
      yychar = END;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 5: /* item_list_head: item  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[0].item)) {
          pp->model->addItem((yyvsp[0].item));
          GC::unlock();
          GC::lock();
        }
      }
    break;

  case 6: /* item_list_head: doc_file_comments item  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[0].item)) {
          pp->model->addItem((yyvsp[0].item));
          GC::unlock();
          GC::lock();
        }
      }
    break;

  case 7: /* item_list_head: item_list_head ';' item  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[0].item)) {
          pp->model->addItem((yyvsp[0].item));
          GC::unlock();
          GC::lock();
        }
      }
    break;

  case 8: /* item_list_head: item_list_head ';' doc_file_comments item  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[0].item)) {
          pp->model->addItem((yyvsp[0].item));
          GC::unlock();
          GC::lock();
        }
      }
    break;

  case 9: /* item_list_head: item error_item_start  */
{ yyerror(&(yylsp[0]), parm, "unexpected item, expecting ';' or end of file"); YYERROR; }
    break;

  case 11: /* doc_file_comments: "file-level documentation comment"  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if (pp->parseDocComments && (yyvsp[0].sValue)) {
          pp->model->addDocComment((yyvsp[0].sValue));
        }
        free((yyvsp[0].sValue));
      }
    break;

  case 12: /* doc_file_comments: doc_file_comments "file-level documentation comment"  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if (pp->parseDocComments && (yyvsp[0].sValue)) {
          pp->model->addDocComment((yyvsp[0].sValue));
        }
        free((yyvsp[0].sValue));
      }
    break;

  case 15: /* item: "documentation comment" item_tail  */
      { (yyval.item) = (yyvsp[0].item);
        ParserState* pp = static_cast<ParserState*>(parm);
        if (FunctionI* fi = Item::dynamicCast<FunctionI>((yyval.item))) {
          if (pp->parseDocComments) {
            fi->ann().add(createDocComment((yylsp[-1]),(yyvsp[-1].sValue)));
          }
        } else if (VarDeclI* vdi = Item::dynamicCast<VarDeclI>((yyval.item))) {
          if (pp->parseDocComments) {
            Expression::addAnnotation(vdi->e(), createDocComment((yylsp[-1]),(yyvsp[-1].sValue)));
          }
        } else {
          yyerror(&(yylsp[0]), parm, "documentation comments are only supported for function, predicate and variable declarations");
        }
        free((yyvsp[-1].sValue));
      }
    break;

  case 16: /* item: item_tail  */
      { (yyval.item) = (yyvsp[0].item); }
    break;

  case 17: /* item_tail: include_item  */
      { (yyval.item)=notInDatafile(&(yyloc),parm,"include") ? (yyvsp[0].item) : nullptr; }
    break;

  case 18: /* item_tail: vardecl_item  */
      { (yyval.item)=notInDatafile(&(yyloc),parm, (yyvsp[0].item)->cast<VarDeclI>()->e()->isTypeAlias() ? "type alias" : "variable declaration") ? (yyvsp[0].item) : nullptr; }
    break;

  case 20: /* item_tail: constraint_item  */
      { (yyval.item)=notInDatafile(&(yyloc),parm,"constraint") ? (yyvsp[0].item) : nullptr; }
    break;

  case 21: /* item_tail: solve_item  */
      { (yyval.item)=notInDatafile(&(yyloc),parm,"solve") ? (yyvsp[0].item) : nullptr; }
    break;

  case 22: /* item_tail: output_item  */
      { (yyval.item)=notInDatafile(&(yyloc),parm,"output") ? (yyvsp[0].item) : nullptr; }
    break;

  case 23: /* item_tail: predicate_item  */
      { (yyval.item)=notInDatafile(&(yyloc),parm,"predicate") ? (yyvsp[0].item) : nullptr; }
    break;

  case 24: /* item_tail: function_item  */
      { (yyval.item)=notInDatafile(&(yyloc),parm,"predicate") ? (yyvsp[0].item) : nullptr; }
    break;

  case 25: /* item_tail: annotation_item  */
      { (yyval.item)=notInDatafile(&(yyloc),parm,"annotation") ? (yyvsp[0].item) : nullptr; }
    break;

  case 35: /* include_item: "include" "string literal"  */
      { ParserState* pp = static_cast<ParserState*>(parm);
        string canonicalName=pp->canonicalFilename((yyvsp[0].sValue));
        map<string,Model*>::iterator ret = pp->seenModels.find(canonicalName);
        IncludeI* ii = new IncludeI((yyloc),ASTString((yyvsp[0].sValue)));
        (yyval.item) = ii;
        if (ret == pp->seenModels.end()) {
          Model* im = new Model;
          im->setParent(pp->model);
          im->setFilename(canonicalName);
          string fpath = FileUtils::dir_name(pp->filename);
          if (fpath=="")
            fpath="./";
          pp->files.emplace_back(im, ii, fpath, canonicalName, pp->isSTDLib);
          ii->m(im);
          pp->seenModels.insert(pair<string,Model*>(canonicalName,im));
        } else {
          ii->m(ret->second, false);
        }
        free((yyvsp[0].sValue));
      }
    break;

  case 36: /* vardecl_item: ti_expr_and_id  */
      { if ((yyvsp[0].vardeclexpr)) {
          if (Expression::type((yyvsp[0].vardeclexpr)->ti()).any() && (yyvsp[0].vardeclexpr)->ti()->domain() == nullptr) {
            // This is an any type, not allowed without a right hand side
            yyerror(&(yylsp[0]), parm, "declarations with `any' type-inst require definition");
          }
          (yyval.item) = VarDeclI::a((yyloc),(yyvsp[0].vardeclexpr));
        }
      }
    break;

  case 37: /* vardecl_item: ti_expr_and_id "=" expr  */
      { 
        if ((yyvsp[-2].vardeclexpr)) {
          (yyvsp[-2].vardeclexpr)->e((yyvsp[0].expression));
          (yyval.item) = VarDeclI::a((yyloc),(yyvsp[-2].vardeclexpr));
        }
      }
    break;

  case 38: /* vardecl_item: "enum" "identifier" annotations  */
      {
        TypeInst* ti = new TypeInst((yyloc),Type::parsetint());
        ti->setIsEnum(true);
        VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[-1].sValue));
        if ((yyvsp[-1].sValue) && (yyvsp[0].expressions1d))
        Expression::addAnnotations(vd, *(yyvsp[0].expressions1d));
        free((yyvsp[-1].sValue));
        (yyval.item) = VarDeclI::a((yyloc),vd);
      }
    break;

  case 39: /* vardecl_item: "enum" "identifier" annotations "=" enum_init  */
      {
        if ((yyvsp[0].expressions1d)) {
          TypeInst* ti = new TypeInst((yyloc),Type::parsetint());
          ti->setIsEnum(true);
          Expression* e;
          if ((yyvsp[0].expressions1d)->size()==1) {
            e = (*(yyvsp[0].expressions1d))[0];
          } else {
            ArrayLit* al = new ArrayLit((yyloc),*(yyvsp[0].expressions1d));
            e = Call::a((yyloc), ASTString("enumFromConstructors"), {al});
          }
          VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[-3].sValue),e);
          (yyval.item) = VarDeclI::a((yyloc),vd);
        }
        free((yyvsp[-3].sValue));
        delete (yyvsp[0].expressions1d);
      }
    break;

  case 40: /* vardecl_item: "enum" "identifier" annotations "=" "[" string_lit_list "]"  */
      {
        TypeInst* ti = new TypeInst((yyloc),Type::parsetint());
        ti->setIsEnum(true);
        vector<Expression*> args;
        args.push_back(new ArrayLit((yyloc),*(yyvsp[-1].expressions1d)));
        Call* sl = Call::a((yyloc), Constants::constants().ids.anonEnumFromStrings, args);
        VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[-5].sValue),sl);
        if ((yyvsp[-5].sValue) && (yyvsp[-4].expressions1d))
        Expression::addAnnotations(vd, *(yyvsp[-4].expressions1d));
        free((yyvsp[-5].sValue));
        delete (yyvsp[-1].expressions1d);
        (yyval.item) = VarDeclI::a((yyloc),vd);
      }
    break;

  case 41: /* vardecl_item: "type" "identifier" annotations "=" ti_expr  */
      {
        TypeInst* ti = (yyvsp[0].tiexpr);
        VarDecl* vd = new VarDecl((yyloc), nullptr, (yyvsp[-3].sValue), ti);
        if ((yyvsp[-2].expressions1d)) {
          Expression::addAnnotations(vd, *(yyvsp[-2].expressions1d));
        }
        free((yyvsp[-3].sValue));
        (yyval.item) = VarDeclI::a((yyloc), vd);
      }
    break;

  case 42: /* enum_init: enum_construct  */
      {
        (yyval.expressions1d) = new std::vector<Expression*>({(yyvsp[0].expression)});
      }
    break;

  case 43: /* enum_init: enum_init "++" enum_construct  */
      {
        (yyval.expressions1d) = (yyvsp[-2].expressions1d);
        if ((yyval.expressions1d)) {
          (yyval.expressions1d)->push_back((yyvsp[0].expression));
        }
      }
    break;

  case 44: /* enum_construct: '{' enum_id_list comma_or_none '}'  */
      {
        (yyval.expression) = new SetLit((yyloc), *(yyvsp[-2].expressions1d));
        delete (yyvsp[-2].expressions1d);
      }
    break;

  case 45: /* enum_construct: "identifier" '(' expr ')'  */
      {
        vector<Expression*> args({(yyvsp[-1].expression)});
        (yyval.expression) = Call::a((yyloc), ASTString((yyvsp[-3].sValue)), args);
        free((yyvsp[-3].sValue));
      }
    break;

  case 46: /* enum_construct: "_" '(' expr ')'  */
      {
        (yyval.expression) = Call::a((yyloc), Constants::constants().ids.anon_enum_set, {(yyvsp[-1].expression)});
      }
    break;

  case 47: /* string_lit_list: %empty  */
      { (yyval.expressions1d) = new std::vector<Expression*>(); }
    break;

  case 48: /* string_lit_list: "string literal"  */
      { (yyval.expressions1d) = new std::vector<Expression*>();
        (yyval.expressions1d)->push_back(new StringLit((yyloc), (yyvsp[0].sValue))); free((yyvsp[0].sValue));
      }
    break;

  case 49: /* string_lit_list: string_lit_list ',' "string literal"  */
      { (yyval.expressions1d) = (yyvsp[-2].expressions1d);
        if ((yyval.expressions1d)) (yyval.expressions1d)->push_back(new StringLit((yyloc), (yyvsp[0].sValue)));
        free((yyvsp[0].sValue));
      }
    break;

  case 50: /* enum_id_list: %empty  */
      { (yyval.expressions1d) = new std::vector<Expression*>(); }
    break;

  case 51: /* enum_id_list: "identifier"  */
      { (yyval.expressions1d) = new std::vector<Expression*>();
        (yyval.expressions1d)->push_back(new Id((yyloc),(yyvsp[0].sValue),nullptr)); free((yyvsp[0].sValue));
      }
    break;

  case 52: /* enum_id_list: enum_id_list ',' "identifier"  */
      { (yyval.expressions1d) = (yyvsp[-2].expressions1d); if ((yyval.expressions1d)) (yyval.expressions1d)->push_back(new Id((yyloc),(yyvsp[0].sValue),nullptr)); free((yyvsp[0].sValue)); }
    break;

  case 53: /* assign_item: "identifier" "=" expr  */
      { (yyval.item) = new AssignI((yyloc),(yyvsp[-2].sValue),(yyvsp[0].expression));
        free((yyvsp[-2].sValue));
      }
    break;

  case 54: /* constraint_item: "constraint" expr  */
      { (yyval.item) = new ConstraintI((yyloc),(yyvsp[0].expression));}
    break;

  case 55: /* constraint_item: "constraint" "::" string_expr expr  */
      { (yyval.item) = new ConstraintI((yyloc),(yyvsp[0].expression));
        if ((yyvsp[0].expression) && (yyvsp[-1].expression))
          Expression::ann((yyval.item)->cast<ConstraintI>()->e()).add(Call::a((yylsp[-2]), ASTString("mzn_constraint_name"), {(yyvsp[-1].expression)}));
      }
    break;

  case 56: /* solve_item: "solve" annotations "satisfy"  */
      { (yyval.item) = SolveI::sat((yyloc));
        if ((yyval.item) && (yyvsp[-1].expressions1d)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[-1].expressions1d));
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 57: /* solve_item: "solve" annotations "minimize" expr  */
      { (yyval.item) = SolveI::min((yyloc),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-2].expressions1d)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[-2].expressions1d));
        delete (yyvsp[-2].expressions1d);
      }
    break;

  case 58: /* solve_item: "solve" annotations "maximize" expr  */
      { (yyval.item) = SolveI::max((yyloc),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-2].expressions1d)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[-2].expressions1d));
        delete (yyvsp[-2].expressions1d);
      }
    break;

  case 59: /* output_item: "output" expr  */
      { (yyval.item) = new OutputI((yyloc),(yyvsp[0].expression)); }
    break;

  case 60: /* output_item: "output" "::" string_expr expr  */
      { (yyval.item) = new OutputI((yyloc),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-1].expression)) {
          (yyval.item)->cast<OutputI>()->ann().add(Call::a((yyloc), ASTString("mzn_output_section"), {(yyvsp[-1].expression)}));
        }
      }
    break;

  case 61: /* predicate_item: "predicate" "identifier" params ann_param annotations operation_item_tail  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[-3].vardeclexprs) && (yyvsp[-2].vardeclexpr)) (yyvsp[-3].vardeclexprs)->push_back((yyvsp[-2].vardeclexpr));
        if ((yyvsp[-3].vardeclexprs)) (yyval.item) = new FunctionI((yyloc),ASTString((yyvsp[-4].sValue)),new TypeInst((yyloc),
                                   Type::varbool()),*(yyvsp[-3].vardeclexprs),(yyvsp[0].expression),pp->isSTDLib,(yyvsp[-2].vardeclexpr) != nullptr);
        if ((yyval.item) && (yyvsp[-1].expressions1d)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expressions1d));
        free((yyvsp[-4].sValue));
        delete (yyvsp[-3].vardeclexprs);
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 62: /* predicate_item: "test" "identifier" params ann_param annotations operation_item_tail  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[-3].vardeclexprs) && (yyvsp[-2].vardeclexpr)) (yyvsp[-3].vardeclexprs)->push_back((yyvsp[-2].vardeclexpr));
        if ((yyvsp[-3].vardeclexprs)) (yyval.item) = new FunctionI((yyloc),ASTString((yyvsp[-4].sValue)),new TypeInst((yyloc),
                                   Type::parbool()),*(yyvsp[-3].vardeclexprs),(yyvsp[0].expression),pp->isSTDLib,(yyvsp[-2].vardeclexpr) != nullptr);
        if ((yyval.item) && (yyvsp[-1].expressions1d)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expressions1d));
        free((yyvsp[-4].sValue));
        delete (yyvsp[-3].vardeclexprs);
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 63: /* predicate_item: "predicate" "identifier" "^-1" params ann_param annotations operation_item_tail  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[-3].vardeclexprs) && (yyvsp[-2].vardeclexpr)) (yyvsp[-3].vardeclexprs)->push_back((yyvsp[-2].vardeclexpr));
        if ((yyvsp[-3].vardeclexprs)) (yyval.item) = new FunctionI((yyloc),ASTString(std::string((yyvsp[-5].sValue))+"⁻¹"),new TypeInst((yyloc),
                                   Type::varbool()),*(yyvsp[-3].vardeclexprs),(yyvsp[0].expression),pp->isSTDLib,(yyvsp[-2].vardeclexpr) != nullptr);
        if ((yyval.item) && (yyvsp[-1].expressions1d)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expressions1d));
        free((yyvsp[-5].sValue));
        delete (yyvsp[-3].vardeclexprs);
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 64: /* predicate_item: "test" "identifier" "^-1" params ann_param annotations operation_item_tail  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[-3].vardeclexprs) && (yyvsp[-2].vardeclexpr)) (yyvsp[-3].vardeclexprs)->push_back((yyvsp[-2].vardeclexpr));
        if ((yyvsp[-3].vardeclexprs)) (yyval.item) = new FunctionI((yyloc),ASTString(std::string((yyvsp[-5].sValue))+"⁻¹"),new TypeInst((yyloc),
                                   Type::parbool()),*(yyvsp[-3].vardeclexprs),(yyvsp[0].expression),pp->isSTDLib,(yyvsp[-2].vardeclexpr) != nullptr);
        if ((yyval.item) && (yyvsp[-1].expressions1d)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expressions1d));
        free((yyvsp[-5].sValue));
        delete (yyvsp[-3].vardeclexprs);
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 65: /* function_item: "function" ti_expr ':' id_or_quoted_op params ann_param annotations operation_item_tail  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[-3].vardeclexprs) && (yyvsp[-2].vardeclexpr)) (yyvsp[-3].vardeclexprs)->push_back((yyvsp[-2].vardeclexpr));
        if ((yyvsp[-6].tiexpr) && Expression::type((yyvsp[-6].tiexpr)).any() && (yyvsp[-6].tiexpr)->domain() == nullptr) {
          // This is an any type, not allowed without a right hand side
          yyerror(&(yylsp[-6]), parm, "return type cannot have `any' type-inst without type-inst variable");
        }
        if ((yyvsp[-3].vardeclexprs)) (yyval.item) = new FunctionI((yyloc),ASTString((yyvsp[-4].sValue)),(yyvsp[-6].tiexpr),*(yyvsp[-3].vardeclexprs),(yyvsp[0].expression),pp->isSTDLib,(yyvsp[-2].vardeclexpr) != nullptr);
        if ((yyval.item) && (yyvsp[-1].expressions1d)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expressions1d));
        free((yyvsp[-4].sValue));
        delete (yyvsp[-3].vardeclexprs);
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 66: /* function_item: ti_expr ':' "identifier" '(' params_list ')' ann_param annotations operation_item_tail  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[-8].tiexpr) && Expression::type((yyvsp[-8].tiexpr)).any() && (yyvsp[-8].tiexpr)->domain() == nullptr) {
          // This is an any type, not allowed without a right hand side
          yyerror(&(yylsp[-8]), parm, "return type cannot have `any' type-inst without type-inst variable");
        }
        if ((yyvsp[-4].vardeclexprs) && (yyvsp[-2].vardeclexpr)) (yyvsp[-4].vardeclexprs)->push_back((yyvsp[-2].vardeclexpr));
        if ((yyvsp[-4].vardeclexprs)) (yyval.item) = new FunctionI((yyloc),ASTString((yyvsp[-6].sValue)),(yyvsp[-8].tiexpr),*(yyvsp[-4].vardeclexprs),(yyvsp[0].expression),pp->isSTDLib,(yyvsp[-2].vardeclexpr) != nullptr);
        if ((yyval.item) && (yyvsp[-1].expressions1d)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expressions1d));
        free((yyvsp[-6].sValue));
        delete (yyvsp[-4].vardeclexprs);
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 67: /* annotation_item: "annotation" "identifier" params  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        TypeInst* ti=new TypeInst((yylsp[-2]),Type::ann());
        if ((yyvsp[0].vardeclexprs)==nullptr || (yyvsp[0].vardeclexprs)->empty()) {
          VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[-1].sValue));
          (yyval.item) = VarDeclI::a((yyloc),vd);
        } else {
          (yyval.item) = new FunctionI((yyloc),ASTString((yyvsp[-1].sValue)),ti,*(yyvsp[0].vardeclexprs),nullptr,pp->isSTDLib);
        }
        free((yyvsp[-1].sValue));
        delete (yyvsp[0].vardeclexprs);
      }
    break;

  case 68: /* annotation_item: "annotation" "identifier" params "=" expr  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        TypeInst* ti=new TypeInst((yylsp[-4]),Type::ann());
        if ((yyvsp[-2].vardeclexprs)) (yyval.item) = new FunctionI((yyloc),ASTString((yyvsp[-3].sValue)),ti,*(yyvsp[-2].vardeclexprs),(yyvsp[0].expression),pp->isSTDLib);
        delete (yyvsp[-2].vardeclexprs);
      }
    break;

  case 69: /* ann_param: %empty  */
      { (yyval.vardeclexpr)=nullptr; }
    break;

  case 70: /* ann_param: "ann" ':' "identifier"  */
      { if ((yyvsp[0].sValue)) {
          auto* ident = new Id((yylsp[0]), (yyvsp[0].sValue), nullptr);
          auto* ti = new TypeInst((yyloc),Type::ann(1));
          (yyval.vardeclexpr) = new VarDecl((yyloc), ti, ident);
          (yyval.vardeclexpr)->toplevel(false);
        } }
    break;

  case 71: /* operation_item_tail: %empty  */
      { (yyval.expression)=nullptr; }
    break;

  case 72: /* operation_item_tail: "=" expr  */
      { (yyval.expression)=(yyvsp[0].expression); }
    break;

  case 73: /* params: %empty  */
      { (yyval.vardeclexprs)=new vector<VarDecl*>(); }
    break;

  case 74: /* params: '(' params_list ')'  */
      { (yyval.vardeclexprs)=(yyvsp[-1].vardeclexprs); }
    break;

  case 75: /* params: '(' error ')'  */
      { (yyval.vardeclexprs)=new vector<VarDecl*>(); }
    break;

  case 76: /* params_list: %empty  */
      { (yyval.vardeclexprs)=new vector<VarDecl*>(); }
    break;

  case 77: /* params_list: params_list_head comma_or_none  */
      { (yyval.vardeclexprs)=(yyvsp[-1].vardeclexprs); }
    break;

  case 78: /* params_list_head: ti_expr_and_id_or_anon  */
      { (yyval.vardeclexprs)=new vector<VarDecl*>();
        if ((yyvsp[0].vardeclexpr)) {
          if (Expression::type((yyvsp[0].vardeclexpr)->ti()).any() && (yyvsp[0].vardeclexpr)->ti()->domain() == nullptr) {
            // This is an any type, not allowed in parameter list
            yyerror(&(yylsp[0]), parm, "parameter declaration cannot have `any' type-inst without type-inst variable");
          }
          (yyvsp[0].vardeclexpr)->toplevel(false);
          (yyval.vardeclexprs)->push_back((yyvsp[0].vardeclexpr));
        }
      }
    break;

  case 79: /* params_list_head: params_list_head ',' ti_expr_and_id_or_anon  */
      { (yyval.vardeclexprs)=(yyvsp[-2].vardeclexprs);
        if ((yyvsp[0].vardeclexpr)) (yyvsp[0].vardeclexpr)->toplevel(false);
        if ((yyvsp[-2].vardeclexprs) && (yyvsp[0].vardeclexpr)) {
          (yyvsp[-2].vardeclexprs)->push_back((yyvsp[0].vardeclexpr));
          if (Expression::type((yyvsp[0].vardeclexpr)->ti()).any() && (yyvsp[0].vardeclexpr)->ti()->domain() == nullptr) {
            // This is an any type, not allowed in parameter list
            yyerror(&(yylsp[0]), parm, "parameter declaration cannot have `any' type-inst without type-inst variable");
          }
        }
      }
    break;

  case 84: /* ti_expr_and_id_or_anon: ti_expr_and_id  */
      { (yyval.vardeclexpr)=(yyvsp[0].vardeclexpr); }
    break;

  case 85: /* ti_expr_and_id_or_anon: ti_expr  */
      { if ((yyvsp[0].tiexpr)) (yyval.vardeclexpr)=new VarDecl((yyloc), (yyvsp[0].tiexpr), ""); }
    break;

  case 86: /* ti_expr_and_id: ti_expr ':' "identifier" annotations  */
      { if ((yyvsp[-3].tiexpr) && (yyvsp[-1].sValue)) {
          Id* ident = new Id((yylsp[-1]), (yyvsp[-1].sValue), nullptr);
          (yyval.vardeclexpr) = new VarDecl((yyloc), (yyvsp[-3].tiexpr), ident);
          if ((yyvsp[0].expressions1d)) Expression::ann((yyval.vardeclexpr)).add(*(yyvsp[0].expressions1d));
        }
        free((yyvsp[-1].sValue));
        delete (yyvsp[0].expressions1d);
      }
    break;

  case 87: /* ti_expr_and_id_list: ti_expr_and_id_list_head comma_or_none  */
      { (yyval.vardeclexprs)=(yyvsp[-1].vardeclexprs); }
    break;

  case 88: /* ti_expr_and_id_list_head: ti_expr_and_id  */
      { (yyval.vardeclexprs)=new vector<VarDecl*>(); (yyval.vardeclexprs)->push_back((yyvsp[0].vardeclexpr)); }
    break;

  case 89: /* ti_expr_and_id_list_head: ti_expr_and_id_list_head ',' ti_expr_and_id  */
      { (yyval.vardeclexprs)=(yyvsp[-2].vardeclexprs); if ((yyvsp[-2].vardeclexprs) && (yyvsp[0].vardeclexpr)) (yyvsp[-2].vardeclexprs)->push_back((yyvsp[0].vardeclexpr)); }
    break;

  case 90: /* ti_expr_list: ti_expr_list_head comma_or_none  */
      { (yyval.tiexprs)=(yyvsp[-1].tiexprs); }
    break;

  case 91: /* ti_expr_list_head: ti_expr  */
      { (yyval.tiexprs)=new vector<TypeInst*>(); (yyval.tiexprs)->push_back((yyvsp[0].tiexpr)); }
    break;

  case 92: /* ti_expr_list_head: ti_expr_list_head ',' ti_expr  */
      { (yyval.tiexprs)=(yyvsp[-2].tiexprs); if ((yyvsp[-2].tiexprs) && (yyvsp[0].tiexpr)) (yyvsp[-2].tiexprs)->push_back((yyvsp[0].tiexpr)); }
    break;

  case 94: /* ti_expr: "array" "[" ti_expr_list "]" "of" base_ti_expr  */
      {
        (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr) && (yyvsp[-3].tiexprs)) (yyval.tiexpr)->setRanges(*(yyvsp[-3].tiexprs));
        delete (yyvsp[-3].tiexprs);
      }
    break;

  case 95: /* ti_expr: "list" "of" base_ti_expr  */
      {
        (yyval.tiexpr) = (yyvsp[0].tiexpr);
        std::vector<TypeInst*> ti(1);
        ti[0] = new TypeInst((yyloc),Type::parint());
        if ((yyval.tiexpr)) (yyval.tiexpr)->setRanges(ti);
      }
    break;

  case 96: /* ti_expr: ti_expr "++" base_ti_expr  */
      {
        (yyval.tiexpr) = (yyvsp[-2].tiexpr);
        Type tt = Expression::type((yyval.tiexpr));
        tt.dim(0);
        TypeInst* lhs = new TypeInst((yyloc), tt, (yyvsp[-2].tiexpr)->domain());
        BinOp* bop = new BinOp((yyloc), lhs, BOT_PLUSPLUS, (yyvsp[0].tiexpr));
        bop->type(tt);
        (yyval.tiexpr)->domain(bop);
      }
    break;

  case 97: /* base_ti_expr: base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
      }
    break;

  case 98: /* base_ti_expr: "opt" base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = Expression::type((yyval.tiexpr));
          tt.ot(Type::OT_OPTIONAL);
          tt.otExplicit(true);
          (yyval.tiexpr)->type(tt);
        }
      }
    break;

  case 99: /* base_ti_expr: "par" opt_opt base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = Expression::type((yyval.tiexpr));
          tt.tiExplicit(true);
          if ((yyvsp[-1].bValue)) {
            tt.ot(Type::OT_OPTIONAL);
            tt.otExplicit(true);
          }
          (yyval.tiexpr)->type(tt);
        }
      }
    break;

  case 100: /* base_ti_expr: "var" opt_opt base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        Type tt = Expression::type((yyval.tiexpr));
        if ((yyval.tiexpr)) {
          tt.ti(Type::TI_VAR);
          tt.tiExplicit(true);
          if ((yyvsp[-1].bValue)) {
            tt.ot(Type::OT_OPTIONAL);
            tt.otExplicit(true);
          }
          (yyval.tiexpr)->type(tt);
        }
      }
    break;

  case 101: /* base_ti_expr: "set" "of" base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = Expression::type((yyval.tiexpr));
          tt.st(Type::ST_SET);
          (yyval.tiexpr)->type(tt);
        }
      }
    break;

  case 102: /* base_ti_expr: "opt" "set" "of" base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = Expression::type((yyval.tiexpr));
          tt.st(Type::ST_SET);
          tt.ot(Type::OT_OPTIONAL);
          tt.otExplicit(true);
          (yyval.tiexpr)->type(tt);
        }
      }
    break;

  case 103: /* base_ti_expr: "par" opt_opt "set" "of" base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = Expression::type((yyval.tiexpr));
          tt.tiExplicit(true);
          tt.st(Type::ST_SET);
          if ((yyvsp[-3].bValue)) {
            tt.ot(Type::OT_OPTIONAL);
            tt.otExplicit(true);
          }
          (yyval.tiexpr)->type(tt);
        }
      }
    break;

  case 104: /* base_ti_expr: "var" opt_opt "set" "of" base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = Expression::type((yyval.tiexpr));
          tt.ti(Type::TI_VAR);
          tt.tiExplicit(true);
          tt.st(Type::ST_SET);
          if ((yyvsp[-3].bValue)) {
            tt.ot(Type::OT_OPTIONAL);
            tt.otExplicit(true);
          }
          (yyval.tiexpr)->type(tt);
        }
      }
    break;

  case 105: /* base_ti_expr: "any" "type-inst identifier"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::mkAny(),new TIId((yyloc), (yyvsp[0].sValue)));
        free((yyvsp[0].sValue));
      }
    break;

  case 106: /* base_ti_expr: "any"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::mkAny()); }
    break;

  case 107: /* opt_opt: %empty  */
      { (yyval.bValue) = false; }
    break;

  case 108: /* opt_opt: "opt"  */
      { (yyval.bValue) = true; }
    break;

  case 109: /* base_ti_expr_tail: "int"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::parint()); }
    break;

  case 110: /* base_ti_expr_tail: "bool"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::parbool()); }
    break;

  case 111: /* base_ti_expr_tail: "float"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::parfloat()); }
    break;

  case 112: /* base_ti_expr_tail: "string"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::parstring()); }
    break;

  case 113: /* base_ti_expr_tail: "ann"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::ann()); }
    break;

  case 114: /* base_ti_expr_tail: "tuple" '(' ti_expr_list ')'  */
      {
        std::vector<Expression*> tmp((yyvsp[-1].tiexprs)->begin(), (yyvsp[-1].tiexprs)->end());
        ArrayLit* al = ArrayLit::constructTuple((yyloc), tmp);
        (yyval.tiexpr) = new TypeInst((yyloc), Type::tuple(), al);
        delete (yyvsp[-1].tiexprs);
      }
    break;

  case 115: /* base_ti_expr_tail: "record" '(' ti_expr_and_id_list ')'  */
      {
        for (auto* vd : *(yyvsp[-1].vardeclexprs)) {
          vd->toplevel(false);
        }
        std::vector<Expression*> tmp((yyvsp[-1].vardeclexprs)->begin(), (yyvsp[-1].vardeclexprs)->end());
        ArrayLit* al = ArrayLit::constructTuple((yyloc), tmp);
        (yyval.tiexpr) = new TypeInst((yyloc), Type::record(), al);
        delete (yyvsp[-1].vardeclexprs);
      }
    break;

  case 116: /* base_ti_expr_tail: set_expr  */
        { if ((yyvsp[0].expression)) (yyval.tiexpr) = new TypeInst((yyloc),Type(),(yyvsp[0].expression)); }
    break;

  case 117: /* base_ti_expr_tail: "type-inst identifier"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::top(),
                         new TIId((yyloc), (yyvsp[0].sValue)));
        free((yyvsp[0].sValue));
      }
    break;

  case 118: /* base_ti_expr_tail: "type-inst enum identifier"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::parint(),
          new TIId((yyloc), (yyvsp[0].sValue)));
          free((yyvsp[0].sValue));
      }
    break;

  case 120: /* array_access_expr_list_head: array_access_expr  */
      { (yyval.expressions1d)=new std::vector<MiniZinc::Expression*>; (yyval.expressions1d)->push_back((yyvsp[0].expression)); }
    break;

  case 121: /* array_access_expr_list_head: array_access_expr_list_head ',' array_access_expr  */
      { (yyval.expressions1d)=(yyvsp[-2].expressions1d); if ((yyval.expressions1d) && (yyvsp[0].expression)) (yyval.expressions1d)->push_back((yyvsp[0].expression)); }
    break;

  case 122: /* array_access_expr: expr  */
      { (yyval.expression) = (yyvsp[0].expression); }
    break;

  case 123: /* array_access_expr: ".."  */
      { (yyval.expression)=new SetLit((yyloc), IntSetVal::a(-IntVal::infinity(),IntVal::infinity())); }
    break;

  case 124: /* array_access_expr: "..<"  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'..<'"), {}); }
    break;

  case 125: /* array_access_expr: "<.."  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..'"), {}); }
    break;

  case 126: /* array_access_expr: "<..<"  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..<'"), {}); }
    break;

  case 128: /* expr_list_head: expr  */
      { (yyval.expressions1d)=new std::vector<MiniZinc::Expression*>; (yyval.expressions1d)->push_back((yyvsp[0].expression)); }
    break;

  case 129: /* expr_list_head: expr_list_head ',' expr  */
      { (yyval.expressions1d)=(yyvsp[-2].expressions1d); if ((yyval.expressions1d) && (yyvsp[0].expression)) (yyval.expressions1d)->push_back((yyvsp[0].expression)); }
    break;

  case 131: /* set_expr: set_expr "::" annotation_expr  */
      { if ((yyvsp[-2].expression) && (yyvsp[0].expression)) Expression::addAnnotation((yyvsp[-2].expression), (yyvsp[0].expression)); (yyval.expression)=(yyvsp[-2].expression); }
    break;

  case 132: /* set_expr: set_expr "union" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_UNION, (yyvsp[0].expression)); }
    break;

  case 133: /* set_expr: set_expr "diff" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIFF, (yyvsp[0].expression)); }
    break;

  case 134: /* set_expr: set_expr "symdiff" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SYMDIFF, (yyvsp[0].expression)); }
    break;

  case 135: /* set_expr: set_expr ".." set_expr  */
      { if ((yyvsp[-2].expression)==nullptr || (yyvsp[0].expression)==nullptr) {
          (yyval.expression) = nullptr;
        } else if (Expression::isa<IntLit>((yyvsp[-2].expression)) && Expression::isa<IntLit>((yyvsp[0].expression))) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a(IntLit::v(Expression::cast<IntLit>((yyvsp[-2].expression))),IntLit::v(Expression::cast<IntLit>((yyvsp[0].expression)))));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DOTDOT, (yyvsp[0].expression));
        }
      }
    break;

  case 136: /* set_expr: set_expr "..<" set_expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'..<'"), {(yyvsp[-2].expression), (yyvsp[0].expression)}); }
    break;

  case 137: /* set_expr: set_expr "<.." set_expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..'"), {(yyvsp[-2].expression), (yyvsp[0].expression)}); }
    break;

  case 138: /* set_expr: set_expr "<..<" set_expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..<'"), {(yyvsp[-2].expression), (yyvsp[0].expression)}); }
    break;

  case 139: /* set_expr: set_expr ".."  */
      { (yyval.expression)=Call::a((yyloc), ASTString("..o"), {(yyvsp[-1].expression)}); }
    break;

  case 140: /* set_expr: set_expr "..<"  */
      { (yyval.expression)=Call::a((yyloc), ASTString("..<o"), {(yyvsp[-1].expression)}); }
    break;

  case 141: /* set_expr: set_expr "<.."  */
      { (yyval.expression)=Call::a((yyloc), ASTString("<..o"), {(yyvsp[-1].expression)}); }
    break;

  case 142: /* set_expr: set_expr "<..<"  */
      { (yyval.expression)=Call::a((yyloc), ASTString("<..<o"), {(yyvsp[-1].expression)}); }
    break;

  case 143: /* set_expr: ".." set_expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("o.."), {(yyvsp[0].expression)}); }
    break;

  case 144: /* set_expr: "..<" set_expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("o..<"), {(yyvsp[0].expression)}); }
    break;

  case 145: /* set_expr: "<.." set_expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("o<.."), {(yyvsp[0].expression)}); }
    break;

  case 146: /* set_expr: "<..<" set_expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("o<..<"), {(yyvsp[0].expression)}); }
    break;

  case 147: /* set_expr: "'..<'" '(' set_expr ',' set_expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'..<'"), {(yyvsp[-3].expression), (yyvsp[-1].expression)}); }
    break;

  case 148: /* set_expr: "'<..'" '(' set_expr ',' set_expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..'"), {(yyvsp[-3].expression), (yyvsp[-1].expression)}); }
    break;

  case 149: /* set_expr: "'<..<'" '(' set_expr ',' set_expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..<'"), {(yyvsp[-3].expression), (yyvsp[-1].expression)}); }
    break;

  case 150: /* set_expr: "'..'" '(' expr ',' expr ')'  */
      { if ((yyvsp[-3].expression)==nullptr || (yyvsp[-1].expression)==nullptr) {
          (yyval.expression) = nullptr;
        } else if (Expression::isa<IntLit>((yyvsp[-3].expression)) && Expression::isa<IntLit>((yyvsp[-1].expression))) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a(IntLit::v(Expression::cast<IntLit>((yyvsp[-3].expression))),IntLit::v(Expression::cast<IntLit>((yyvsp[-1].expression)))));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-3].expression), BOT_DOTDOT, (yyvsp[-1].expression));
        }
      }
    break;

  case 151: /* set_expr: "'..<'" '(' set_expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'..<'"), {(yyvsp[-1].expression)}); }
    break;

  case 152: /* set_expr: "'<..'" '(' set_expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..'"), {(yyvsp[-1].expression)}); }
    break;

  case 153: /* set_expr: "'<..<'" '(' set_expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..<'"), {(yyvsp[-1].expression)}); }
    break;

  case 154: /* set_expr: "'..'" '(' expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'..'"), {(yyvsp[-1].expression)}); }
    break;

  case 155: /* set_expr: set_expr "intersect" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_INTERSECT, (yyvsp[0].expression)); }
    break;

  case 156: /* set_expr: set_expr "+" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_PLUS, (yyvsp[0].expression)); }
    break;

  case 157: /* set_expr: set_expr "-" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MINUS, (yyvsp[0].expression)); }
    break;

  case 158: /* set_expr: set_expr "*" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MULT, (yyvsp[0].expression)); }
    break;

  case 159: /* set_expr: set_expr "/" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIV, (yyvsp[0].expression)); }
    break;

  case 160: /* set_expr: set_expr "div" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IDIV, (yyvsp[0].expression)); }
    break;

  case 161: /* set_expr: set_expr "mod" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MOD, (yyvsp[0].expression)); }
    break;

  case 162: /* set_expr: set_expr "^" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_POW, (yyvsp[0].expression)); }
    break;

  case 163: /* set_expr: set_expr "~+" set_expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~+"), args);
      }
    break;

  case 164: /* set_expr: set_expr "~-" set_expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~-"), args);
      }
    break;

  case 165: /* set_expr: set_expr "~*" set_expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~*"), args);
      }
    break;

  case 166: /* set_expr: set_expr "~/" set_expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~/"), args);
      }
    break;

  case 167: /* set_expr: set_expr "~div" set_expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~div"), args);
      }
    break;

  case 168: /* set_expr: set_expr "~=" set_expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~="), args);
      }
    break;

  case 169: /* set_expr: set_expr "~!=" set_expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~!="), args);
      }
    break;

  case 170: /* set_expr: set_expr "default" set_expr  */
      {
        vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("default"), args);
      }
    break;

  case 171: /* set_expr: set_expr "quoted identifier" set_expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), (yyvsp[-1].sValue), args);
        free((yyvsp[-1].sValue));
      }
    break;

  case 172: /* set_expr: "+" set_expr  */
      { (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[0].expression)); }
    break;

  case 173: /* set_expr: "-" set_expr  */
      { if ((yyvsp[0].expression) && Expression::isa<IntLit>((yyvsp[0].expression))) {
          (yyval.expression) = IntLit::a(-IntLit::v(Expression::cast<IntLit>((yyvsp[0].expression))));
        } else if ((yyvsp[0].expression) && Expression::isa<FloatLit>((yyvsp[0].expression))) {
          (yyval.expression) = FloatLit::a(-FloatLit::v(Expression::cast<FloatLit>((yyvsp[0].expression))));
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_MINUS, (yyvsp[0].expression));
        }
      }
    break;

  case 175: /* expr: expr "::" annotation_expr  */
      { if ((yyvsp[-2].expression) && (yyvsp[0].expression)) Expression::addAnnotation((yyvsp[-2].expression), (yyvsp[0].expression)); (yyval.expression)=(yyvsp[-2].expression); }
    break;

  case 176: /* expr: expr "<->" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_EQUIV, (yyvsp[0].expression)); }
    break;

  case 177: /* expr: expr "->" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IMPL, (yyvsp[0].expression)); }
    break;

  case 178: /* expr: expr "<-" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_RIMPL, (yyvsp[0].expression)); }
    break;

  case 179: /* expr: expr "\\/" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_OR, (yyvsp[0].expression)); }
    break;

  case 180: /* expr: expr "xor" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_XOR, (yyvsp[0].expression)); }
    break;

  case 181: /* expr: expr "/\\" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_AND, (yyvsp[0].expression)); }
    break;

  case 182: /* expr: expr "<" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_LE, (yyvsp[0].expression)); }
    break;

  case 183: /* expr: expr ">" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_GR, (yyvsp[0].expression)); }
    break;

  case 184: /* expr: expr "<=" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_LQ, (yyvsp[0].expression)); }
    break;

  case 185: /* expr: expr ">=" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_GQ, (yyvsp[0].expression)); }
    break;

  case 186: /* expr: expr "=" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_EQ, (yyvsp[0].expression)); }
    break;

  case 187: /* expr: expr "!=" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_NQ, (yyvsp[0].expression)); }
    break;

  case 188: /* expr: expr "in" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IN, (yyvsp[0].expression)); }
    break;

  case 189: /* expr: expr "subset" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SUBSET, (yyvsp[0].expression)); }
    break;

  case 190: /* expr: expr "superset" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SUPERSET, (yyvsp[0].expression)); }
    break;

  case 191: /* expr: expr "union" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_UNION, (yyvsp[0].expression)); }
    break;

  case 192: /* expr: expr "diff" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIFF, (yyvsp[0].expression)); }
    break;

  case 193: /* expr: expr "symdiff" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SYMDIFF, (yyvsp[0].expression)); }
    break;

  case 194: /* expr: expr ".." expr  */
      { if ((yyvsp[-2].expression)==nullptr || (yyvsp[0].expression)==nullptr) {
          (yyval.expression) = nullptr;
        } else if (Expression::isa<IntLit>((yyvsp[-2].expression)) && Expression::isa<IntLit>((yyvsp[0].expression))) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a(IntLit::v(Expression::cast<IntLit>((yyvsp[-2].expression))),IntLit::v(Expression::cast<IntLit>((yyvsp[0].expression)))));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DOTDOT, (yyvsp[0].expression));
        }
      }
    break;

  case 195: /* expr: expr "..<" expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'..<'"), {(yyvsp[-2].expression), (yyvsp[0].expression)}); }
    break;

  case 196: /* expr: expr "<.." expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..'"), {(yyvsp[-2].expression), (yyvsp[0].expression)}); }
    break;

  case 197: /* expr: expr "<..<" expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..<'"), {(yyvsp[-2].expression), (yyvsp[0].expression)}); }
    break;

  case 198: /* expr: expr ".."  */
      { (yyval.expression)=Call::a((yyloc), ASTString("..o"), {(yyvsp[-1].expression)}); }
    break;

  case 199: /* expr: expr "..<"  */
      { (yyval.expression)=Call::a((yyloc), ASTString("..<o"), {(yyvsp[-1].expression)}); }
    break;

  case 200: /* expr: expr "<.."  */
      { (yyval.expression)=Call::a((yyloc), ASTString("<..o"), {(yyvsp[-1].expression)}); }
    break;

  case 201: /* expr: expr "<..<"  */
      { (yyval.expression)=Call::a((yyloc), ASTString("<..<o"), {(yyvsp[-1].expression)}); }
    break;

  case 202: /* expr: ".." expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("o.."), {(yyvsp[0].expression)}); }
    break;

  case 203: /* expr: "..<" expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("o..<"), {(yyvsp[0].expression)}); }
    break;

  case 204: /* expr: "<.." expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("o<.."), {(yyvsp[0].expression)}); }
    break;

  case 205: /* expr: "<..<" expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("o<..<"), {(yyvsp[0].expression)}); }
    break;

  case 206: /* expr: "'..<'" '(' expr ',' expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'..<'"), {(yyvsp[-3].expression), (yyvsp[-1].expression)}); }
    break;

  case 207: /* expr: "'<..'" '(' expr ',' expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..'"), {(yyvsp[-3].expression), (yyvsp[-1].expression)}); }
    break;

  case 208: /* expr: "'<..<'" '(' expr ',' expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..<'"), {(yyvsp[-3].expression), (yyvsp[-1].expression)}); }
    break;

  case 209: /* expr: "'..'" '(' expr ',' expr ')'  */
      { if ((yyvsp[-3].expression)==nullptr || (yyvsp[-1].expression)==nullptr) {
          (yyval.expression) = nullptr;
        } else if (Expression::isa<IntLit>((yyvsp[-3].expression)) && Expression::isa<IntLit>((yyvsp[-1].expression))) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a(IntLit::v(Expression::cast<IntLit>((yyvsp[-3].expression))),IntLit::v(Expression::cast<IntLit>((yyvsp[-1].expression)))));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-3].expression), BOT_DOTDOT, (yyvsp[-1].expression));
        }
      }
    break;

  case 210: /* expr: "'..<'" '(' expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'..<'"), {(yyvsp[-1].expression)}); }
    break;

  case 211: /* expr: "'<..'" '(' expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..'"), {(yyvsp[-1].expression)}); }
    break;

  case 212: /* expr: "'<..<'" '(' expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..<'"), {(yyvsp[-1].expression)}); }
    break;

  case 213: /* expr: "'..'" '(' expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'..'"), {(yyvsp[-1].expression)}); }
    break;

  case 214: /* expr: expr "intersect" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_INTERSECT, (yyvsp[0].expression)); }
    break;

  case 215: /* expr: expr "++" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_PLUSPLUS, (yyvsp[0].expression)); }
    break;

  case 216: /* expr: expr "+" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_PLUS, (yyvsp[0].expression)); }
    break;

  case 217: /* expr: expr "-" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MINUS, (yyvsp[0].expression)); }
    break;

  case 218: /* expr: expr "*" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MULT, (yyvsp[0].expression)); }
    break;

  case 219: /* expr: expr "/" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIV, (yyvsp[0].expression)); }
    break;

  case 220: /* expr: expr "div" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IDIV, (yyvsp[0].expression)); }
    break;

  case 221: /* expr: expr "mod" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MOD, (yyvsp[0].expression)); }
    break;

  case 222: /* expr: expr "^" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_POW, (yyvsp[0].expression)); }
    break;

  case 223: /* expr: expr "~+" expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~+"), args);
      }
    break;

  case 224: /* expr: expr "~-" expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~-"), args);
      }
    break;

  case 225: /* expr: expr "~*" expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~*"), args);
      }
    break;

  case 226: /* expr: expr "~/" expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~/"), args);
      }
    break;

  case 227: /* expr: expr "~div" expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~div"), args);
      }
    break;

  case 228: /* expr: expr "~=" expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~="), args);
      }
    break;

  case 229: /* expr: expr "~!=" expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~!="), args);
      }
    break;

  case 230: /* expr: expr "default" expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("default"), args);
      }
    break;

  case 231: /* expr: expr "quoted identifier" expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), (yyvsp[-1].sValue), args);
        free((yyvsp[-1].sValue));
      }
    break;

  case 232: /* expr: "not" expr  */
      { (yyval.expression)=new UnOp((yyloc), UOT_NOT, (yyvsp[0].expression)); }
    break;

  case 233: /* expr: "+" expr  */
      { if (((yyvsp[0].expression) && Expression::isa<IntLit>((yyvsp[0].expression))) || ((yyvsp[0].expression) && Expression::isa<FloatLit>((yyvsp[0].expression)))) {
          (yyval.expression) = (yyvsp[0].expression);
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[0].expression));
        }
      }
    break;

  case 234: /* expr: "-" expr  */
      { if ((yyvsp[0].expression) && Expression::isa<IntLit>((yyvsp[0].expression))) {
          (yyval.expression) = IntLit::a(-IntLit::v(Expression::cast<IntLit>((yyvsp[0].expression))));
        } else if ((yyvsp[0].expression) && Expression::isa<FloatLit>((yyvsp[0].expression))) {
          (yyval.expression) = FloatLit::a(-FloatLit::v(Expression::cast<FloatLit>((yyvsp[0].expression))));
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_MINUS, (yyvsp[0].expression));
        }
      }
    break;

  case 235: /* expr_atom_head: expr_atom_head_nonstring  */
      { (yyval.expression)=(yyvsp[0].expression); }
    break;

  case 236: /* expr_atom_head: string_expr  */
      { (yyval.expression)=(yyvsp[0].expression); }
    break;

  case 237: /* expr_atom_head_nonstring: '(' expr ')'  */
      { (yyval.expression)=(yyvsp[-1].expression); }
    break;

  case 238: /* expr_atom_head_nonstring: '(' expr ')' access_tail  */
      { if ((yyvsp[0].expressions1d)) (yyval.expression)=createAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[0].expressions1d)); delete (yyvsp[0].expressions1d); }
    break;

  case 239: /* expr_atom_head_nonstring: '(' expr ')' "^-1"  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 240: /* expr_atom_head_nonstring: '(' expr ')' access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) (yyval.expression)=new BinOp((yyloc),createAccess((yyloc), (yyvsp[-3].expression), *(yyvsp[-1].expressions1d)), BOT_POW, IntLit::a(-1)); delete (yyvsp[-1].expressions1d); }
    break;

  case 241: /* expr_atom_head_nonstring: "identifier"  */
      { (yyval.expression)=new Id((yyloc), (yyvsp[0].sValue), nullptr); free((yyvsp[0].sValue)); }
    break;

  case 242: /* expr_atom_head_nonstring: "identifier" access_tail  */
      { if ((yyvsp[0].expressions1d)) (yyval.expression)=createAccess((yyloc), new Id((yylsp[-1]),(yyvsp[-1].sValue),nullptr), *(yyvsp[0].expressions1d));
        free((yyvsp[-1].sValue)); delete (yyvsp[0].expressions1d); }
    break;

  case 243: /* expr_atom_head_nonstring: "identifier" "^-1"  */
      { (yyval.expression)=new BinOp((yyloc),new Id((yyloc), (yyvsp[-1].sValue), nullptr), BOT_POW, IntLit::a(-1)); free((yyvsp[-1].sValue)); }
    break;

  case 244: /* expr_atom_head_nonstring: "identifier" access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) (yyval.expression)=new BinOp((yyloc),createAccess((yyloc), new Id((yylsp[-2]),(yyvsp[-2].sValue),nullptr), *(yyvsp[-1].expressions1d)), BOT_POW, IntLit::a(-1));
        free((yyvsp[-2].sValue)); delete (yyvsp[-1].expressions1d); }
    break;

  case 245: /* expr_atom_head_nonstring: "_"  */
      { (yyval.expression)=new AnonVar((yyloc)); }
    break;

  case 246: /* expr_atom_head_nonstring: "_" access_tail  */
      { if ((yyvsp[0].expressions1d)) (yyval.expression)=createAccess((yyloc), new AnonVar((yyloc)), *(yyvsp[0].expressions1d));
        delete (yyvsp[0].expressions1d); }
    break;

  case 247: /* expr_atom_head_nonstring: "_" "^-1"  */
      { (yyval.expression)=new BinOp((yyloc),new AnonVar((yyloc)), BOT_POW, IntLit::a(-1)); }
    break;

  case 248: /* expr_atom_head_nonstring: "_" access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) (yyval.expression)=new BinOp((yyloc),createAccess((yyloc), new AnonVar((yyloc)), *(yyvsp[-1].expressions1d)), BOT_POW, IntLit::a(-1));
        delete (yyvsp[-1].expressions1d); }
    break;

  case 249: /* expr_atom_head_nonstring: "bool literal"  */
      { (yyval.expression)=Constants::constants().boollit(((yyvsp[0].iValue)!=0)); }
    break;

  case 250: /* expr_atom_head_nonstring: "bool literal" "^-1"  */
      { (yyval.expression)=new BinOp((yyloc),Constants::constants().boollit(((yyvsp[-1].iValue)!=0)), BOT_POW, IntLit::a(-1)); }
    break;

  case 251: /* expr_atom_head_nonstring: "integer literal"  */
      { (yyval.expression)=IntLit::a((yyvsp[0].iValue)); }
    break;

  case 252: /* expr_atom_head_nonstring: "integer literal" "^-1"  */
      { (yyval.expression)=new BinOp((yyloc),IntLit::a((yyvsp[-1].iValue)), BOT_POW, IntLit::a(-1)); }
    break;

  case 253: /* expr_atom_head_nonstring: "infinity"  */
      { (yyval.expression)=IntLit::a(IntVal::infinity()); }
    break;

  case 254: /* expr_atom_head_nonstring: "infinity" "^-1"  */
      { (yyval.expression)=new BinOp((yyloc),IntLit::a(IntVal::infinity()), BOT_POW, IntLit::a(-1)); }
    break;

  case 255: /* expr_atom_head_nonstring: "float literal"  */
      { (yyval.expression)=FloatLit::a((yyvsp[0].dValue)); }
    break;

  case 256: /* expr_atom_head_nonstring: "float literal" "^-1"  */
      { (yyval.expression)=new BinOp((yyloc),FloatLit::a((yyvsp[-1].dValue)), BOT_POW, IntLit::a(-1)); }
    break;

  case 257: /* expr_atom_head_nonstring: "<>"  */
      { (yyval.expression)=Constants::constants().absent; }
    break;

  case 258: /* expr_atom_head_nonstring: "<>" "^-1"  */
      { (yyval.expression)=Constants::constants().absent; }
    break;

  case 260: /* expr_atom_head_nonstring: set_literal access_tail  */
      { if ((yyvsp[0].expressions1d)) (yyval.expression)=createAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions1d));
        delete (yyvsp[0].expressions1d); }
    break;

  case 261: /* expr_atom_head_nonstring: set_literal "^-1"  */
      { (yyval.expression) = new BinOp((yyloc),(yyvsp[-1].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 262: /* expr_atom_head_nonstring: set_literal access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) (yyval.expression)=new BinOp((yyloc),createAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions1d)), BOT_POW, IntLit::a(-1));
        delete (yyvsp[-1].expressions1d); }
    break;

  case 264: /* expr_atom_head_nonstring: set_comp access_tail  */
      { if ((yyvsp[0].expressions1d)) (yyval.expression)=createAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions1d));
        delete (yyvsp[0].expressions1d); }
    break;

  case 265: /* expr_atom_head_nonstring: set_comp "^-1"  */
      { (yyval.expression) = new BinOp((yyloc),(yyvsp[-1].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 266: /* expr_atom_head_nonstring: set_comp access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) (yyval.expression)=new BinOp((yyloc),createAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions1d)), BOT_POW, IntLit::a(-1));
        delete (yyvsp[-1].expressions1d); }
    break;

  case 268: /* expr_atom_head_nonstring: simple_array_literal access_tail  */
      { if ((yyvsp[0].expressions1d)) (yyval.expression)=createAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions1d));
        delete (yyvsp[0].expressions1d); }
    break;

  case 269: /* expr_atom_head_nonstring: simple_array_literal "^-1"  */
      { (yyval.expression) = new BinOp((yyloc),(yyvsp[-1].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 270: /* expr_atom_head_nonstring: simple_array_literal access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) (yyval.expression)=new BinOp((yyloc),createAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions1d)), BOT_POW, IntLit::a(-1));
        delete (yyvsp[-1].expressions1d); }
    break;

  case 272: /* expr_atom_head_nonstring: simple_array_literal_2d access_tail  */
      { if ((yyvsp[0].expressions1d)) (yyval.expression)=createAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions1d));
        delete (yyvsp[0].expressions1d); }
    break;

  case 273: /* expr_atom_head_nonstring: simple_array_literal_2d "^-1"  */
      { (yyval.expression) = new BinOp((yyloc),(yyvsp[-1].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 274: /* expr_atom_head_nonstring: simple_array_literal_2d access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) (yyval.expression)=new BinOp((yyloc),createAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions1d)), BOT_POW, IntLit::a(-1));
        delete (yyvsp[-1].expressions1d); }
    break;

  case 276: /* expr_atom_head_nonstring: simple_array_comp access_tail  */
      { if ((yyvsp[0].expressions1d)) (yyval.expression)=createAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions1d));
        delete (yyvsp[0].expressions1d); }
    break;

  case 277: /* expr_atom_head_nonstring: simple_array_comp "^-1"  */
      { (yyval.expression) = new BinOp((yyloc),(yyvsp[-1].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 278: /* expr_atom_head_nonstring: simple_array_comp access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) (yyval.expression)=new BinOp((yyloc),createAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions1d)), BOT_POW, IntLit::a(-1));
        delete (yyvsp[-1].expressions1d); }
    break;

  case 280: /* expr_atom_head_nonstring: if_then_else_expr access_tail  */
      { if ((yyvsp[0].expressions1d)) (yyval.expression)=createAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions1d));
        delete (yyvsp[0].expressions1d); }
    break;

  case 281: /* expr_atom_head_nonstring: if_then_else_expr "^-1"  */
      { (yyval.expression) = new BinOp((yyloc),(yyvsp[-1].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 282: /* expr_atom_head_nonstring: if_then_else_expr access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) (yyval.expression)=new BinOp((yyloc),createAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions1d)), BOT_POW, IntLit::a(-1));
        delete (yyvsp[-1].expressions1d); }
    break;

  case 285: /* expr_atom_head_nonstring: call_expr access_tail  */
      { if ((yyvsp[0].expressions1d)) (yyval.expression)=createAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions1d));
        delete (yyvsp[0].expressions1d); }
    break;

  case 287: /* expr_atom_head_nonstring: call_expr access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) (yyval.expression)=createAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions1d));
        delete (yyvsp[-1].expressions1d); }
    break;

  case 289: /* expr_atom_head_nonstring: tuple_literal access_tail  */
      { if ((yyvsp[0].expressions1d)) (yyval.expression)=createAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions1d));
        delete (yyvsp[0].expressions1d); }
    break;

  case 290: /* expr_atom_head_nonstring: tuple_literal "^-1"  */
      { (yyval.expression) = new BinOp((yyloc),(yyvsp[-1].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 291: /* expr_atom_head_nonstring: tuple_literal access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) (yyval.expression)=new BinOp((yyloc),createAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions1d)), BOT_POW, IntLit::a(-1));
        delete (yyvsp[-1].expressions1d); }
    break;

  case 293: /* expr_atom_head_nonstring: record_literal access_tail  */
      { if ((yyvsp[0].expressions1d)) (yyval.expression)=createAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions1d));
        delete (yyvsp[0].expressions1d); }
    break;

  case 294: /* expr_atom_head_nonstring: record_literal "^-1"  */
      { (yyval.expression) = new BinOp((yyloc),(yyvsp[-1].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 295: /* expr_atom_head_nonstring: record_literal access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) (yyval.expression)=new BinOp((yyloc),createAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions1d)), BOT_POW, IntLit::a(-1));
        delete (yyvsp[-1].expressions1d); }
    break;

  case 296: /* string_expr: "string literal"  */
      { (yyval.expression)=new StringLit((yyloc), (yyvsp[0].sValue)); free((yyvsp[0].sValue)); }
    break;

  case 297: /* string_expr: "interpolated string start" string_quote_rest  */
      { (yyval.expression)=new BinOp((yyloc), new StringLit((yyloc), (yyvsp[-1].sValue)), BOT_PLUSPLUS, (yyvsp[0].expression));
        free((yyvsp[-1].sValue));
      }
    break;

  case 298: /* string_quote_rest: expr_list_head "interpolated string end"  */
      { if ((yyvsp[-1].expressions1d)) (yyval.expression)=new BinOp((yyloc), Call::a((yyloc), ASTString("format"), *(yyvsp[-1].expressions1d)), BOT_PLUSPLUS, new StringLit((yyloc),(yyvsp[0].sValue)));
        free((yyvsp[0].sValue));
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 299: /* string_quote_rest: expr_list_head "interpolated string middle" string_quote_rest  */
      { if ((yyvsp[-2].expressions1d)) (yyval.expression)=new BinOp((yyloc), Call::a((yyloc), ASTString("format"), *(yyvsp[-2].expressions1d)), BOT_PLUSPLUS,
                             new BinOp((yyloc), new StringLit((yyloc),(yyvsp[-1].sValue)), BOT_PLUSPLUS, (yyvsp[0].expression)));
        free((yyvsp[-1].sValue));
        delete (yyvsp[-2].expressions1d);
      }
    break;

  case 300: /* access_tail: "[" array_access_expr_list "]"  */
      {
        (yyval.expressions1d)=new std::vector<Expression*>();
        if ((yyvsp[-1].expressions1d)) {
          auto* al = new ArrayAccess((yyloc), nullptr, *(yyvsp[-1].expressions1d));
          (yyval.expressions1d)->push_back(al);
          delete (yyvsp[-1].expressions1d);
        }
      }
    break;

  case 301: /* access_tail: "field access"  */
      {
        (yyval.expressions1d)=new std::vector<Expression*>();
        std::string tail((yyvsp[0].sValue)); free((yyvsp[0].sValue));
        parseFieldTail((yyloc), *(yyval.expressions1d), tail);
      }
    break;

  case 302: /* access_tail: access_tail "[" array_access_expr_list "]"  */
      {
        (yyval.expressions1d)=(yyvsp[-3].expressions1d);
        if ((yyval.expressions1d) && (yyvsp[-1].expressions1d)) {
          auto* al = new ArrayAccess((yyloc), nullptr, *(yyvsp[-1].expressions1d));
          (yyval.expressions1d)->push_back(al);
          delete (yyvsp[-1].expressions1d);
        }
      }
    break;

  case 303: /* access_tail: access_tail "field access"  */
      {
        (yyval.expressions1d)=(yyvsp[-1].expressions1d);
        std::string tail((yyvsp[0].sValue)); free((yyvsp[0].sValue));
        parseFieldTail((yyloc), *(yyval.expressions1d), tail);
      }
    break;

  case 304: /* set_literal: '{' '}'  */
      { (yyval.expression) = new SetLit((yyloc), std::vector<Expression*>()); }
    break;

  case 305: /* set_literal: '{' expr_list '}'  */
      { if ((yyvsp[-1].expressions1d)) (yyval.expression) = new SetLit((yyloc), *(yyvsp[-1].expressions1d));
        delete (yyvsp[-1].expressions1d); }
    break;

  case 306: /* tuple_literal: '(' expr ',' ')'  */
      {
        std::vector<Expression*> list({ (yyvsp[-2].expression) });
        (yyval.expression)=ArrayLit::constructTuple((yyloc), list);
      }
    break;

  case 307: /* tuple_literal: '(' expr ',' expr_list ')'  */
      {
        auto* list = (yyvsp[-1].expressions1d);
        if (list == nullptr) {
          list = new std::vector<Expression*>();
        }
        if ((yyvsp[-3].expression)) {
          list->insert(list->begin(), (yyvsp[-3].expression));
        }
        (yyval.expression)=ArrayLit::constructTuple((yyloc), *list);
        delete list;
      }
    break;

  case 308: /* record_literal: '(' record_field_list_head comma_or_none ')'  */
      {
        (yyval.expression) = ArrayLit::constructTuple((yyloc), *(yyvsp[-2].expressions1d));
        Expression::type((yyval.expression), Type::record());
        delete((yyvsp[-2].expressions1d));
      }
    break;

  case 309: /* record_field_list_head: record_field  */
      { (yyval.expressions1d) = new vector<Expression*>(1); (*(yyval.expressions1d))[0] = (yyvsp[0].vardeclexpr); }
    break;

  case 310: /* record_field_list_head: record_field_list_head ',' record_field  */
      { (yyval.expressions1d)=(yyvsp[-2].expressions1d); (yyval.expressions1d)->push_back((yyvsp[0].vardeclexpr)); }
    break;

  case 311: /* record_field: "identifier" ':' expr  */
      {
        (yyval.vardeclexpr) = new VarDecl((yyloc), new TypeInst((yyloc), Type()), (yyvsp[-2].sValue), (yyvsp[0].expression));
        free((yyvsp[-2].sValue));
      }
    break;

  case 312: /* set_comp: '{' expr '|' comp_tail '}'  */
      { if ((yyvsp[-1].generatorsPointer)) (yyval.expression) = new Comprehension((yyloc), (yyvsp[-3].expression), *(yyvsp[-1].generatorsPointer), true);
        delete (yyvsp[-1].generatorsPointer);
      }
    break;

  case 313: /* comp_tail: generator_list  */
      { if ((yyvsp[0].generators)) (yyval.generatorsPointer)=new Generators; (yyval.generatorsPointer)->g = *(yyvsp[0].generators); delete (yyvsp[0].generators); }
    break;

  case 315: /* generator_list_head: generator  */
      { (yyval.generators)=new std::vector<Generator>; if ((yyvsp[0].generator)) (yyval.generators)->push_back(*(yyvsp[0].generator)); delete (yyvsp[0].generator); }
    break;

  case 316: /* generator_list_head: generator_eq  */
      { (yyval.generators)=new std::vector<Generator>; if ((yyvsp[0].generator)) (yyval.generators)->push_back(*(yyvsp[0].generator)); delete (yyvsp[0].generator); }
    break;

  case 317: /* generator_list_head: generator_eq "where" expr  */
      { (yyval.generators)=new std::vector<Generator>;
        if ((yyvsp[-2].generator)) (yyval.generators)->push_back(*(yyvsp[-2].generator));
        if ((yyvsp[-2].generator) && (yyvsp[0].expression)) (yyval.generators)->push_back(Generator((yyval.generators)->size(),(yyvsp[0].expression)));
        delete (yyvsp[-2].generator);
      }
    break;

  case 318: /* generator_list_head: generator_list_head ',' generator  */
      { (yyval.generators)=(yyvsp[-2].generators); if ((yyval.generators) && (yyvsp[0].generator)) (yyval.generators)->push_back(*(yyvsp[0].generator)); delete (yyvsp[0].generator); }
    break;

  case 319: /* generator_list_head: generator_list_head ',' generator_eq  */
      { (yyval.generators)=(yyvsp[-2].generators); if ((yyval.generators) && (yyvsp[0].generator)) (yyval.generators)->push_back(*(yyvsp[0].generator)); delete (yyvsp[0].generator); }
    break;

  case 320: /* generator_list_head: generator_list_head ',' generator_eq "where" expr  */
      { (yyval.generators)=(yyvsp[-4].generators);
        if ((yyval.generators) && (yyvsp[-2].generator)) (yyval.generators)->push_back(*(yyvsp[-2].generator));
        if ((yyval.generators) && (yyvsp[-2].generator) && (yyvsp[0].expression)) (yyval.generators)->push_back(Generator((yyval.generators)->size(),(yyvsp[0].expression)));
        delete (yyvsp[-2].generator);
      }
    break;

  case 321: /* generator: id_list "in" expr  */
      { if ((yyvsp[-2].strings) && (yyvsp[0].expression)) (yyval.generator)=new Generator(*(yyvsp[-2].strings),(yyvsp[0].expression),nullptr); else (yyval.generator)=nullptr; delete (yyvsp[-2].strings); }
    break;

  case 322: /* generator: id_list "in" expr "where" expr  */
      { if ((yyvsp[-4].strings) && (yyvsp[-2].expression)) (yyval.generator)=new Generator(*(yyvsp[-4].strings),(yyvsp[-2].expression),(yyvsp[0].expression)); else (yyval.generator)=nullptr; delete (yyvsp[-4].strings); }
    break;

  case 323: /* generator_eq: "identifier" "=" expr  */
      { if ((yyvsp[0].expression)) (yyval.generator)=new Generator({(yyvsp[-2].sValue)},nullptr,(yyvsp[0].expression)); else (yyval.generator)=nullptr; free((yyvsp[-2].sValue)); }
    break;

  case 325: /* id_list_head: "identifier"  */
      { (yyval.strings)=new std::vector<std::string>; (yyval.strings)->push_back((yyvsp[0].sValue)); free((yyvsp[0].sValue)); }
    break;

  case 326: /* id_list_head: "_"  */
      { (yyval.strings)=new std::vector<std::string>; (yyval.strings)->push_back(""); }
    break;

  case 327: /* id_list_head: id_list_head ',' "identifier"  */
      { (yyval.strings)=(yyvsp[-2].strings); if ((yyval.strings) && (yyvsp[0].sValue)) (yyval.strings)->push_back((yyvsp[0].sValue)); free((yyvsp[0].sValue)); }
    break;

  case 328: /* id_list_head: id_list_head ',' "_"  */
      { (yyval.strings)=(yyvsp[-2].strings); if ((yyval.strings)) (yyval.strings)->push_back(""); }
    break;

  case 329: /* simple_array_literal: "[" "]"  */
      { (yyval.expression)=new ArrayLit((yyloc), std::vector<MiniZinc::Expression*>()); }
    break;

  case 330: /* simple_array_literal: "[" comp_expr_list "]"  */
      { if ((yyvsp[-1].indexedexpression2d)) {
          if ((yyvsp[-1].indexedexpression2d)->first.empty()) {
            (yyval.expression)=new ArrayLit((yyloc), (yyvsp[-1].indexedexpression2d)->second);
          } else {
            const auto* tuple = Expression::dynamicCast<ArrayLit>((yyvsp[-1].indexedexpression2d)->first[0]);
            if (tuple) {
              std::vector<std::vector<Expression*>> dims(tuple->size());
              for (const auto* t : (yyvsp[-1].indexedexpression2d)->first) {
                if ( (tuple = Expression::dynamicCast<ArrayLit>(t)) != nullptr ) {
                  if (tuple->size() == dims.size()) {
                    for (unsigned int i = 0; i < dims.size(); i++) {
                      dims[i].push_back((*tuple)[i]);
                    }
                  } else {
                    yyerror(&(yylsp[-1]), parm, "syntax error, non-uniform indexed array literal");
                  }
                } else {
                  yyerror(&(yylsp[-1]), parm, "syntax error, non-uniform indexed array literal");
                }
              }
              std::vector<Expression*> arrayNdArgs(dims.size());
              for (unsigned int i = 0; i < dims.size(); i++) {
                arrayNdArgs[i] = new ArrayLit((yyloc), dims[i]);
              }
              arrayNdArgs.push_back(new ArrayLit((yyloc), (yyvsp[-1].indexedexpression2d)->second));

              if ((yyvsp[-1].indexedexpression2d)->first.size() != (yyvsp[-1].indexedexpression2d)->second.size()) {
                yyerror(&(yylsp[-1]), parm, "syntax error, non-uniform indexed array literal");
                (yyval.expression)=nullptr;
              } else {
                (yyval.expression)=Call::a((yyloc), "arrayNd", arrayNdArgs);
              }
            } else {
              for (const auto* t : (yyvsp[-1].indexedexpression2d)->first) {
                if (Expression::isa<ArrayLit>(t)) {
                  yyerror(&(yylsp[-1]), parm, "syntax error, non-uniform indexed array literal");
                }
              }
              (yyval.expression)=Call::a((yyloc), "arrayNd", {new ArrayLit((yyloc), (yyvsp[-1].indexedexpression2d)->first), new ArrayLit((yyloc), (yyvsp[-1].indexedexpression2d)->second)});
            }
          }
          delete (yyvsp[-1].indexedexpression2d);
        }
      }
    break;

  case 331: /* simple_array_literal_2d: "[|" "|]"  */
      { (yyval.expression)=new ArrayLit((yyloc), std::vector<std::vector<Expression*> >()); }
    break;

  case 332: /* simple_array_literal_2d: "[|" simple_array_literal_2d_indexed_list "|]"  */
      { if ((yyvsp[-1].indexedexpressions2d)) {
          std::vector<std::vector<Expression*>> v((yyvsp[-1].indexedexpressions2d)->size());
          std::vector<Expression*> columnHeader;
          std::vector<Expression*> rowHeader;
          bool hadHeaderRow = false;
          for (unsigned int i = 0; i < (yyvsp[-1].indexedexpressions2d)->size(); i++) {
            if (i == 0 && (*(yyvsp[-1].indexedexpressions2d))[i].second.empty() && !(*(yyvsp[-1].indexedexpressions2d))[i].first.empty()) {
              hadHeaderRow = true;
              columnHeader = (*(yyvsp[-1].indexedexpressions2d))[i].first;
              v.resize(v.size()-1);
              continue;
            }
            if (i > 0 && (*(yyvsp[-1].indexedexpressions2d))[i].second.size() != (*(yyvsp[-1].indexedexpressions2d))[i-1].second.size()) {
              if (i == 1 && hadHeaderRow) {
                if ((*(yyvsp[-1].indexedexpressions2d))[i].second.size() != (*(yyvsp[-1].indexedexpressions2d))[i-1].first.size()) {
                  yyerror(&(yylsp[-1]), parm, "syntax error, sub-array of 2d array literal has different length from index row");
                }
              } else {
                yyerror(&(yylsp[-1]), parm, "syntax error, all sub-arrays of 2d array literal must have the same length");
              }
            }
            if (i > hadHeaderRow && (*(yyvsp[-1].indexedexpressions2d))[i].first.size() != (*(yyvsp[-1].indexedexpressions2d))[i-1].first.size()) {
              yyerror(&(yylsp[-1]), parm, "syntax error, mixing indexed and non-indexed sub-arrays in 2d array literal");
            }
            if (i >= hadHeaderRow && !(*(yyvsp[-1].indexedexpressions2d))[i].first.empty()) {
              rowHeader.push_back((*(yyvsp[-1].indexedexpressions2d))[i].first[0]);
            }
            for (unsigned int j = 0; j < (*(yyvsp[-1].indexedexpressions2d))[i].second.size(); j++) {
              v[i - hadHeaderRow].push_back((*(yyvsp[-1].indexedexpressions2d))[i].second[j]);
            }
          }
          if (columnHeader.empty() && rowHeader.empty()) {
            (yyval.expression)=new ArrayLit((yyloc), v);
          } else {
            std::vector<Expression*> vv;
            for (auto& row : v) {
              for (auto* e : row) {
                vv.push_back(e);
              }
            }
            if (rowHeader.empty()) {
              auto nRows = vv.size() / columnHeader.size();
              rowHeader.resize(nRows);
              for (unsigned int i = 0; i < nRows; i++) {
                rowHeader[i] = IntLit::a(i+1);
              }
            } else if (columnHeader.empty()) {
              auto nCols = vv.size() / rowHeader.size();
              columnHeader.resize(nCols);
              for (unsigned int i = 0; i < nCols; i++) {
                columnHeader[i] = IntLit::a(i+1);
              }
            }
            (yyval.expression)=Call::a((yyloc), "array2d", {new ArrayLit((yyloc), rowHeader), new ArrayLit((yyloc), columnHeader), new ArrayLit((yyloc), vv)});
          }
          delete (yyvsp[-1].indexedexpressions2d);
        } else {
          (yyval.expression) = nullptr;
        }
      }
    break;

  case 333: /* simple_array_literal_2d: "[|" simple_array_literal_3d_list "|]"  */
    {
      if ((yyvsp[-1].expressions3d)) {
        std::vector<std::pair<int,int> > dims(3);
        dims[0] = std::pair<int,int>(1,static_cast<int>((yyvsp[-1].expressions3d)->size()));
        if ((yyvsp[-1].expressions3d)->size()==0) {
          dims[1] = std::pair<int,int>(1,0);
          dims[2] = std::pair<int,int>(1,0);
        } else {
          dims[1] = std::pair<int,int>(1,static_cast<int>((*(yyvsp[-1].expressions3d))[0].size()));
          if ((*(yyvsp[-1].expressions3d))[0].size()==0) {
            dims[2] = std::pair<int,int>(1,0);
          } else {
            dims[2] = std::pair<int,int>(1,static_cast<int>((*(yyvsp[-1].expressions3d))[0][0].size()));
          }
        }
        std::vector<Expression*> a;
        for (int i=0; i<dims[0].second; i++) {
          if ((*(yyvsp[-1].expressions3d))[i].size() != dims[1].second) {
            yyerror(&(yylsp[-1]), parm, "syntax error, all sub-arrays of 3d array literal must have the same length");
          } else {
            for (int j=0; j<dims[1].second; j++) {
              if ((*(yyvsp[-1].expressions3d))[i][j].size() != dims[2].second) {
                yyerror(&(yylsp[-1]), parm, "syntax error, all sub-arrays of 3d array literal must have the same length");
              } else {
                for (int k=0; k<dims[2].second; k++) {
                  a.push_back((*(yyvsp[-1].expressions3d))[i][j][k]);
                }
              }
            }
          }
        }
        (yyval.expression) = new ArrayLit((yyloc),a,dims);
        delete (yyvsp[-1].expressions3d);
      } else {
        (yyval.expression) = nullptr;
      }
    }
    break;

  case 334: /* simple_array_literal_3d_list: '|' '|'  */
      { (yyval.expressions3d)=new std::vector<std::vector<std::vector<MiniZinc::Expression*> > >;
      }
    break;

  case 335: /* simple_array_literal_3d_list: '|' simple_array_literal_2d_list '|'  */
      { (yyval.expressions3d)=new std::vector<std::vector<std::vector<MiniZinc::Expression*> > >;
        if ((yyvsp[-1].expressions2d)) (yyval.expressions3d)->push_back(*(yyvsp[-1].expressions2d));
        delete (yyvsp[-1].expressions2d);
      }
    break;

  case 336: /* simple_array_literal_3d_list: simple_array_literal_3d_list ',' '|' simple_array_literal_2d_list '|'  */
      { (yyval.expressions3d)=(yyvsp[-4].expressions3d);
        if ((yyval.expressions3d) && (yyvsp[-1].expressions2d)) (yyval.expressions3d)->push_back(*(yyvsp[-1].expressions2d));
        delete (yyvsp[-1].expressions2d);
      }
    break;

  case 337: /* simple_array_literal_2d_list: expr_list  */
      { (yyval.expressions2d)=new std::vector<std::vector<MiniZinc::Expression*> >;
        if ((yyvsp[0].expressions1d)) (yyval.expressions2d)->push_back(*(yyvsp[0].expressions1d));
        delete (yyvsp[0].expressions1d);
      }
    break;

  case 338: /* simple_array_literal_2d_list: simple_array_literal_2d_list '|' expr_list  */
      { (yyval.expressions2d)=(yyvsp[-2].expressions2d); if ((yyval.expressions2d) && (yyvsp[0].expressions1d)) (yyval.expressions2d)->push_back(*(yyvsp[0].expressions1d)); delete (yyvsp[0].expressions1d); }
    break;

  case 340: /* simple_array_literal_2d_indexed_list_head: simple_array_literal_2d_indexed_list_row  */
      { (yyval.indexedexpressions2d)=new std::vector<std::pair<std::vector<MiniZinc::Expression*>,std::vector<MiniZinc::Expression*>>>();
        if ((yyvsp[0].indexedexpression2d)) {
          if ((yyvsp[0].indexedexpression2d)->first.size() > 1 || ((yyvsp[0].indexedexpression2d)->first.size() == 1 && (yyvsp[0].indexedexpression2d)->second.empty())) {
            yyerror(&(yyloc),parm,"invalid array literal, mixing indexes and values");
          }
          (yyval.indexedexpressions2d)->push_back(*(yyvsp[0].indexedexpression2d));
          delete (yyvsp[0].indexedexpression2d);
        }
      }
    break;

  case 341: /* simple_array_literal_2d_indexed_list_head: simple_array_literal_2d_indexed_list_row_head ':'  */
      { (yyval.indexedexpressions2d)=new std::vector<std::pair<std::vector<MiniZinc::Expression*>,std::vector<MiniZinc::Expression*>>>();
        if ((yyvsp[-1].indexedexpression2d)) {
          if ((yyvsp[-1].indexedexpression2d)->second.size() != 1) {
            yyerror(&(yyloc),parm,"invalid array literal, mixing indexes and values");
          }
          (yyvsp[-1].indexedexpression2d)->first.push_back((yyvsp[-1].indexedexpression2d)->second.back());
          (yyvsp[-1].indexedexpression2d)->second.pop_back();
          (yyval.indexedexpressions2d)->push_back(*(yyvsp[-1].indexedexpression2d));
          delete (yyvsp[-1].indexedexpression2d);
        }
      }
    break;

  case 342: /* simple_array_literal_2d_indexed_list_head: simple_array_literal_2d_indexed_list_head '|' simple_array_literal_2d_indexed_list_row  */
      { (yyval.indexedexpressions2d)=(yyvsp[-2].indexedexpressions2d);
        if ((yyval.indexedexpressions2d) && (yyvsp[0].indexedexpression2d)) {
          if ((yyvsp[0].indexedexpression2d)->first.size() > 1 || ((yyvsp[0].indexedexpression2d)->first.size() == 1 && (yyvsp[0].indexedexpression2d)->second.empty())) {
            yyerror(&(yylsp[0]),parm,"invalid array literal, mixing indexes and values");
          }
          (yyval.indexedexpressions2d)->push_back(*(yyvsp[0].indexedexpression2d));
          delete (yyvsp[0].indexedexpression2d);
        }
      }
    break;

  case 344: /* simple_array_literal_2d_indexed_list_row_head: expr  */
      { (yyval.indexedexpression2d)=new std::pair<std::vector<MiniZinc::Expression*>,std::vector<MiniZinc::Expression*>>();
        (yyval.indexedexpression2d)->second.push_back((yyvsp[0].expression));
      }
    break;

  case 345: /* simple_array_literal_2d_indexed_list_row_head: simple_array_literal_2d_indexed_list_row_head ':' expr  */
      { (yyval.indexedexpression2d)=(yyvsp[-2].indexedexpression2d);
        if ((yyval.indexedexpression2d)) {
          if ((yyval.indexedexpression2d)->second.size() != 1) {
            yyerror(&(yyloc),parm,"invalid array literal, mixing indexes and values");
          }
          (yyval.indexedexpression2d)->first.push_back((yyval.indexedexpression2d)->second.back());
          (yyval.indexedexpression2d)->second.pop_back();
          (yyval.indexedexpression2d)->second.push_back((yyvsp[0].expression));
        }
      }
    break;

  case 346: /* simple_array_literal_2d_indexed_list_row_head: simple_array_literal_2d_indexed_list_row_head ',' expr  */
      { (yyval.indexedexpression2d)=(yyvsp[-2].indexedexpression2d);
        if ((yyval.indexedexpression2d)) {
          if ((yyval.indexedexpression2d)->second.empty()) {
            yyerror(&(yyloc),parm,"invalid array literal, mixing indexes and values");
          }
          (yyval.indexedexpression2d)->second.push_back((yyvsp[0].expression));
        }
      }
    break;

  case 347: /* simple_array_comp: "[" expr ':' expr '|' comp_tail "]"  */
      { if ((yyvsp[-5].expression) && (yyvsp[-1].generatorsPointer)) {
          std::vector<Expression*> tv;
          if (auto* al = Expression::dynamicCast<ArrayLit>((yyvsp[-5].expression))) {
            for (unsigned int i=0; i<al->size(); i++) {
              tv.push_back((*al)[i]);
            }
          } else {
            tv.push_back((yyvsp[-5].expression));
          }
          tv.push_back((yyvsp[-3].expression));
          auto* t = ArrayLit::constructTuple((yyloc),tv);
          Type ty = Type::tuple();
          ty.typeId(Type::COMP_INDEX);
          t->type(ty);
          (yyval.expression)=new Comprehension((yyloc), t, *(yyvsp[-1].generatorsPointer), false);
          delete (yyvsp[-1].generatorsPointer);
        }
      }
    break;

  case 348: /* simple_array_comp: "[" expr '|' comp_tail "]"  */
      { if ((yyvsp[-1].generatorsPointer)) (yyval.expression)=new Comprehension((yyloc), (yyvsp[-3].expression), *(yyvsp[-1].generatorsPointer), false);
        delete (yyvsp[-1].generatorsPointer);
      }
    break;

  case 350: /* comp_expr_list_head: expr  */
      { (yyval.indexedexpression2d)=new std::pair<std::vector<MiniZinc::Expression*>,std::vector<MiniZinc::Expression*>>;
        (yyval.indexedexpression2d)->second.push_back((yyvsp[0].expression)); }
    break;

  case 351: /* comp_expr_list_head: expr ':' expr  */
      { (yyval.indexedexpression2d)=new std::pair<std::vector<MiniZinc::Expression*>,std::vector<MiniZinc::Expression*>>;
        (yyval.indexedexpression2d)->first.push_back((yyvsp[-2].expression));
        (yyval.indexedexpression2d)->second.push_back((yyvsp[0].expression)); }
    break;

  case 352: /* comp_expr_list_head: comp_expr_list_head ',' expr  */
      { (yyval.indexedexpression2d)=(yyvsp[-2].indexedexpression2d);
        if ((yyval.indexedexpression2d) && (yyvsp[0].expression)) {
          if ((yyval.indexedexpression2d)->first.size() > 1) {
            yyerror(&(yyloc),parm,"invalid array literal, mixing indexed and non-indexed values");
            (yyval.indexedexpression2d) = nullptr;
          } else {
            (yyval.indexedexpression2d)->second.push_back((yyvsp[0].expression));
          }
        }
      }
    break;

  case 353: /* comp_expr_list_head: comp_expr_list_head ',' expr ':' expr  */
      { (yyval.indexedexpression2d)=(yyvsp[-4].indexedexpression2d);
        if ((yyval.indexedexpression2d) && (yyvsp[-2].expression)) {
          if ((yyval.indexedexpression2d)->first.size() != (yyval.indexedexpression2d)->second.size()) {
            yyerror(&(yyloc),parm,"invalid array literal, mixing indexed and non-indexed values");
            (yyval.indexedexpression2d) = nullptr;
          } else if (Expression::isa<ArrayLit>((yyvsp[-2].expression)) && Expression::cast<ArrayLit>((yyvsp[-2].expression))->isTuple() && Expression::cast<ArrayLit>((yyvsp[-2].expression))->size() == 1) {
            (yyval.indexedexpression2d)->first.push_back((*Expression::cast<ArrayLit>((yyvsp[-2].expression)))[0]);
            (yyval.indexedexpression2d)->second.push_back((yyvsp[0].expression));
            delete (yyvsp[-2].expression);
          } else {
            (yyval.indexedexpression2d)->first.push_back((yyvsp[-2].expression));
            (yyval.indexedexpression2d)->second.push_back((yyvsp[0].expression));
          }
        }
      }
    break;

  case 354: /* if_then_else_expr: "if" expr "then" expr "endif"  */
      {
        std::vector<Expression*> iexps;
        iexps.push_back((yyvsp[-3].expression));
        iexps.push_back((yyvsp[-1].expression));
        (yyval.expression)=new ITE((yyloc), iexps, nullptr);
      }
    break;

  case 355: /* if_then_else_expr: "if" expr "then" expr elseif_list "else" expr "endif"  */
      {
        std::vector<Expression*> iexps;
        iexps.push_back((yyvsp[-6].expression));
        iexps.push_back((yyvsp[-4].expression));
        if ((yyvsp[-3].expressions1d)) {
          for (unsigned int i=0; i<(yyvsp[-3].expressions1d)->size(); i+=2) {
            iexps.push_back((*(yyvsp[-3].expressions1d))[i]);
            iexps.push_back((*(yyvsp[-3].expressions1d))[i+1]);
          }
        }
        (yyval.expression)=new ITE((yyloc), iexps,(yyvsp[-1].expression));
        delete (yyvsp[-3].expressions1d);
      }
    break;

  case 356: /* elseif_list: %empty  */
      { (yyval.expressions1d)=new std::vector<MiniZinc::Expression*>; }
    break;

  case 357: /* elseif_list: elseif_list "elseif" expr "then" expr  */
      { (yyval.expressions1d)=(yyvsp[-4].expressions1d); if ((yyval.expressions1d) && (yyvsp[-2].expression) && (yyvsp[0].expression)) { (yyval.expressions1d)->push_back((yyvsp[-2].expression)); (yyval.expressions1d)->push_back((yyvsp[0].expression)); } }
    break;

  case 358: /* quoted_op: "'<->'"  */
      { (yyval.iValue)=BOT_EQUIV; }
    break;

  case 359: /* quoted_op: "'->'"  */
      { (yyval.iValue)=BOT_IMPL; }
    break;

  case 360: /* quoted_op: "'<-'"  */
      { (yyval.iValue)=BOT_RIMPL; }
    break;

  case 361: /* quoted_op: "'\\/'"  */
      { (yyval.iValue)=BOT_OR; }
    break;

  case 362: /* quoted_op: "'xor'"  */
      { (yyval.iValue)=BOT_XOR; }
    break;

  case 363: /* quoted_op: "'/\\'"  */
      { (yyval.iValue)=BOT_AND; }
    break;

  case 364: /* quoted_op: "'<'"  */
      { (yyval.iValue)=BOT_LE; }
    break;

  case 365: /* quoted_op: "'>'"  */
      { (yyval.iValue)=BOT_GR; }
    break;

  case 366: /* quoted_op: "'<='"  */
      { (yyval.iValue)=BOT_LQ; }
    break;

  case 367: /* quoted_op: "'>='"  */
      { (yyval.iValue)=BOT_GQ; }
    break;

  case 368: /* quoted_op: "'='"  */
      { (yyval.iValue)=BOT_EQ; }
    break;

  case 369: /* quoted_op: "'!='"  */
      { (yyval.iValue)=BOT_NQ; }
    break;

  case 370: /* quoted_op: "'in'"  */
      { (yyval.iValue)=BOT_IN; }
    break;

  case 371: /* quoted_op: "'subset'"  */
      { (yyval.iValue)=BOT_SUBSET; }
    break;

  case 372: /* quoted_op: "'superset'"  */
      { (yyval.iValue)=BOT_SUPERSET; }
    break;

  case 373: /* quoted_op: "'union'"  */
      { (yyval.iValue)=BOT_UNION; }
    break;

  case 374: /* quoted_op: "'diff'"  */
      { (yyval.iValue)=BOT_DIFF; }
    break;

  case 375: /* quoted_op: "'symdiff'"  */
      { (yyval.iValue)=BOT_SYMDIFF; }
    break;

  case 376: /* quoted_op: "'+'"  */
      { (yyval.iValue)=BOT_PLUS; }
    break;

  case 377: /* quoted_op: "'-'"  */
      { (yyval.iValue)=BOT_MINUS; }
    break;

  case 378: /* quoted_op: "'*'"  */
      { (yyval.iValue)=BOT_MULT; }
    break;

  case 379: /* quoted_op: "'^'"  */
      { (yyval.iValue)=BOT_POW; }
    break;

  case 380: /* quoted_op: "'/'"  */
      { (yyval.iValue)=BOT_DIV; }
    break;

  case 381: /* quoted_op: "'div'"  */
      { (yyval.iValue)=BOT_IDIV; }
    break;

  case 382: /* quoted_op: "'mod'"  */
      { (yyval.iValue)=BOT_MOD; }
    break;

  case 383: /* quoted_op: "'intersect'"  */
      { (yyval.iValue)=BOT_INTERSECT; }
    break;

  case 384: /* quoted_op: "'++'"  */
      { (yyval.iValue)=BOT_PLUSPLUS; }
    break;

  case 385: /* quoted_op: "'not'"  */
      { (yyval.iValue)=-1; }
    break;

  case 386: /* quoted_op_call: quoted_op '(' expr ',' expr ')'  */
      { if ((yyvsp[-5].iValue)==-1) {
          (yyval.expression)=nullptr;
          yyerror(&(yylsp[-3]), parm, "syntax error, unary operator with two arguments");
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-3].expression),static_cast<BinOpType>((yyvsp[-5].iValue)),(yyvsp[-1].expression));
        }
      }
    break;

  case 387: /* quoted_op_call: quoted_op '(' expr ')'  */
      { int uot=-1;
        switch ((yyvsp[-3].iValue)) {
        case -1:
          uot = UOT_NOT;
          break;
        case BOT_MINUS:
          uot = UOT_MINUS;
          break;
        case BOT_PLUS:
          uot = UOT_PLUS;
          break;
        default:
          yyerror(&(yylsp[-1]), parm, "syntax error, binary operator with unary argument list");
          break;
        }
        if (uot==-1)
          (yyval.expression)=nullptr;
        else {
          if (uot==UOT_PLUS && (yyvsp[-1].expression) && (Expression::isa<IntLit>((yyvsp[-1].expression)) || Expression::isa<FloatLit>((yyvsp[-1].expression)))) {
            (yyval.expression) = (yyvsp[-1].expression);
          } else if (uot==UOT_MINUS && (yyvsp[-1].expression) && Expression::isa<IntLit>((yyvsp[-1].expression))) {
            (yyval.expression) = IntLit::a(-IntLit::v(Expression::cast<IntLit>((yyvsp[-1].expression))));
          } else if (uot==UOT_MINUS && (yyvsp[-1].expression) && Expression::isa<FloatLit>((yyvsp[-1].expression))) {
            (yyval.expression) = FloatLit::a(-FloatLit::v(Expression::cast<FloatLit>((yyvsp[-1].expression))));
          } else {
            (yyval.expression)=new UnOp((yyloc), static_cast<UnOpType>(uot),(yyvsp[-1].expression));
          }
        }
      }
    break;

  case 388: /* call_expr: "identifier" '(' ')'  */
      { (yyval.expression)=Call::a((yyloc), (yyvsp[-2].sValue), std::vector<Expression*>()); free((yyvsp[-2].sValue)); }
    break;

  case 389: /* call_expr: "identifier" "^-1" '(' ')'  */
      { (yyval.expression)=Call::a((yyloc), std::string((yyvsp[-3].sValue))+"⁻¹", std::vector<Expression*>()); free((yyvsp[-3].sValue)); }
    break;

  case 391: /* call_expr: "identifier" '(' comp_or_expr ')'  */
      {
        if ((yyvsp[-1].expressionPairs)!=nullptr) {
          bool hadWhere = false;
          std::vector<Expression*> args;
          for (unsigned int i=0; i<(yyvsp[-1].expressionPairs)->size(); i++) {
            if ((*(yyvsp[-1].expressionPairs))[i].second) {
              yyerror(&(yylsp[-1]), parm, "syntax error, 'where' expression outside generator call");
              hadWhere = true;
              (yyval.expression)=nullptr;
            }
            args.push_back((*(yyvsp[-1].expressionPairs))[i].first);
          }
          if (!hadWhere) {
            (yyval.expression)=Call::a((yyloc), (yyvsp[-3].sValue), args);
          }
        }
        free((yyvsp[-3].sValue));
        delete (yyvsp[-1].expressionPairs);
      }
    break;

  case 392: /* call_expr: "identifier" '(' comp_or_expr ')' '(' expr ')'  */
      {
        vector<Generator> gens;
        vector<Id*> ids;
        if ((yyvsp[-4].expressionPairs)) {
          for (unsigned int i=0; i<(yyvsp[-4].expressionPairs)->size(); i++) {
            if (Id* id = Expression::dynamicCast<Id>((*(yyvsp[-4].expressionPairs))[i].first)) {
              if ((*(yyvsp[-4].expressionPairs))[i].second) {
                ParserLocation loc = Expression::loc((*(yyvsp[-4].expressionPairs))[i].second).parserLocation();
                yyerror(&loc, parm, "illegal where expression in generator call");
              }
              ids.push_back(id);
            } else {
              if (BinOp* boe = Expression::dynamicCast<BinOp>((*(yyvsp[-4].expressionPairs))[i].first)) {
                if (boe->lhs() && boe->rhs()) {
                  Id* id = Expression::dynamicCast<Id>(boe->lhs());
                  if (id && boe->op() == BOT_IN) {
                    ids.push_back(id);
                    gens.push_back(Generator(ids,boe->rhs(),(*(yyvsp[-4].expressionPairs))[i].second));
                    ids = vector<Id*>();
                  } else if (id && boe->op() == BOT_EQ && ids.empty()) {
                    ids.push_back(id);
                    gens.push_back(Generator(ids,nullptr,boe->rhs()));
                    if ((*(yyvsp[-4].expressionPairs))[i].second) {
                      gens.push_back(Generator(gens.size(),(*(yyvsp[-4].expressionPairs))[i].second));
                    }
                    ids = vector<Id*>();
                  } else {
                    ParserLocation loc = Expression::loc((*(yyvsp[-4].expressionPairs))[i].first).parserLocation();
                    yyerror(&loc, parm, "illegal expression in generator call");
                  }
                }
              } else {
                ParserLocation loc = Expression::loc((*(yyvsp[-4].expressionPairs))[i].first).parserLocation();
                yyerror(&loc, parm, "illegal expression in generator call");
              }
            }
          }
        }
        if (ids.size() != 0) {
          yyerror(&(yylsp[-4]), parm, "illegal expression in generator call");
        }
        ParserState* pp = static_cast<ParserState*>(parm);
        if (pp->hadError) {
          (yyval.expression)=nullptr;
        } else {
          Generators g; g.g = gens;
          Comprehension* ac = new Comprehension((yyloc), (yyvsp[-1].expression),g,false);
          vector<Expression*> args; args.push_back(ac);
          (yyval.expression)=Call::a((yyloc), (yyvsp[-6].sValue), args);
        }
        free((yyvsp[-6].sValue));
        delete (yyvsp[-4].expressionPairs);
      }
    break;

  case 393: /* call_expr: "identifier" "^-1" '(' comp_or_expr ')'  */
      {
        if ((yyvsp[-1].expressionPairs)!=nullptr) {
          bool hadWhere = false;
          std::vector<Expression*> args;
          for (unsigned int i=0; i<(yyvsp[-1].expressionPairs)->size(); i++) {
            if ((*(yyvsp[-1].expressionPairs))[i].second) {
              yyerror(&(yylsp[-1]), parm, "syntax error, 'where' expression outside generator call");
              hadWhere = true;
              (yyval.expression)=nullptr;
            }
            args.push_back((*(yyvsp[-1].expressionPairs))[i].first);
          }
          if (!hadWhere) {
            (yyval.expression)=Call::a((yyloc), std::string((yyvsp[-4].sValue))+"⁻¹", args);
          }
        }
        free((yyvsp[-4].sValue));
        delete (yyvsp[-1].expressionPairs);
      }
    break;

  case 394: /* call_expr: "identifier" "^-1" '(' comp_or_expr ')' '(' expr ')'  */
      {
        vector<Generator> gens;
        vector<Id*> ids;
        if ((yyvsp[-4].expressionPairs)) {
          for (unsigned int i=0; i<(yyvsp[-4].expressionPairs)->size(); i++) {
            if (Id* id = Expression::dynamicCast<Id>((*(yyvsp[-4].expressionPairs))[i].first)) {
              if ((*(yyvsp[-4].expressionPairs))[i].second) {
                ParserLocation loc = Expression::loc((*(yyvsp[-4].expressionPairs))[i].second).parserLocation();
                yyerror(&loc, parm, "illegal where expression in generator call");
              }
              ids.push_back(id);
            } else {
              if (BinOp* boe = Expression::dynamicCast<BinOp>((*(yyvsp[-4].expressionPairs))[i].first)) {
                if (boe->lhs() && boe->rhs()) {
                  Id* id = Expression::dynamicCast<Id>(boe->lhs());
                  if (id && boe->op() == BOT_IN) {
                    ids.push_back(id);
                    gens.push_back(Generator(ids,boe->rhs(),(*(yyvsp[-4].expressionPairs))[i].second));
                    ids = vector<Id*>();
                  } else if (id && boe->op() == BOT_EQ && ids.empty()) {
                    ids.push_back(id);
                    gens.push_back(Generator(ids,nullptr,boe->rhs()));
                    if ((*(yyvsp[-4].expressionPairs))[i].second) {
                      gens.push_back(Generator(gens.size(),(*(yyvsp[-4].expressionPairs))[i].second));
                    }
                    ids = vector<Id*>();
                  } else {
                    ParserLocation loc = Expression::loc((*(yyvsp[-4].expressionPairs))[i].first).parserLocation();
                    yyerror(&loc, parm, "illegal expression in generator call");
                  }
                }
              } else {
                ParserLocation loc = Expression::loc((*(yyvsp[-4].expressionPairs))[i].first).parserLocation();
                yyerror(&loc, parm, "illegal expression in generator call");
              }
            }
          }
        }
        if (ids.size() != 0) {
          yyerror(&(yylsp[-4]), parm, "illegal expression in generator call");
        }
        ParserState* pp = static_cast<ParserState*>(parm);
        if (pp->hadError) {
          (yyval.expression)=nullptr;
        } else {
          Generators g; g.g = gens;
          Comprehension* ac = new Comprehension((yyloc), (yyvsp[-1].expression),g,false);
          vector<Expression*> args; args.push_back(ac);
          (yyval.expression)=Call::a((yyloc), std::string((yyvsp[-7].sValue))+"⁻¹", args);
        }
        free((yyvsp[-7].sValue));
        delete (yyvsp[-4].expressionPairs);
      }
    break;

  case 396: /* comp_or_expr_head: expr  */
      { (yyval.expressionPairs)=new vector<pair<Expression*,Expression*> >;
        if ((yyvsp[0].expression)) {
          (yyval.expressionPairs)->push_back(pair<Expression*,Expression*>((yyvsp[0].expression),nullptr));
        }
      }
    break;

  case 397: /* comp_or_expr_head: expr "where" expr  */
      { (yyval.expressionPairs)=new vector<pair<Expression*,Expression*> >;
        if ((yyvsp[-2].expression) && (yyvsp[0].expression)) {
          (yyval.expressionPairs)->push_back(pair<Expression*,Expression*>((yyvsp[-2].expression),(yyvsp[0].expression)));
        }
      }
    break;

  case 398: /* comp_or_expr_head: comp_or_expr_head ',' expr  */
      { (yyval.expressionPairs)=(yyvsp[-2].expressionPairs); if ((yyval.expressionPairs) && (yyvsp[0].expression)) (yyval.expressionPairs)->push_back(pair<Expression*,Expression*>((yyvsp[0].expression),nullptr)); }
    break;

  case 399: /* comp_or_expr_head: comp_or_expr_head ',' expr "where" expr  */
      { (yyval.expressionPairs)=(yyvsp[-4].expressionPairs); if ((yyval.expressionPairs) && (yyvsp[-2].expression) && (yyvsp[0].expression)) (yyval.expressionPairs)->push_back(pair<Expression*,Expression*>((yyvsp[-2].expression),(yyvsp[0].expression))); }
    break;

  case 400: /* let_expr: "let" '{' '}' "in" expr  */
      { (yyval.expression)=(yyvsp[0].expression); }
    break;

  case 401: /* let_expr: "let" '{' let_vardecl_item_list '}' "in" expr  */
      { if ((yyvsp[-3].expressions1d) && (yyvsp[0].expression)) {
          (yyval.expression)=new Let((yyloc), *(yyvsp[-3].expressions1d), (yyvsp[0].expression)); delete (yyvsp[-3].expressions1d);
        } else {
          (yyval.expression)=nullptr;
        }
      }
    break;

  case 402: /* let_expr: "let" '{' let_vardecl_item_list comma_or_semi '}' "in" expr  */
      { if ((yyvsp[-4].expressions1d) && (yyvsp[0].expression)) {
          (yyval.expression)=new Let((yyloc), *(yyvsp[-4].expressions1d), (yyvsp[0].expression)); delete (yyvsp[-4].expressions1d);
        } else {
          (yyval.expression)=nullptr;
        }
      }
    break;

  case 403: /* let_vardecl_item_list: let_vardecl_item  */
      { (yyval.expressions1d)=new vector<Expression*>; (yyval.expressions1d)->push_back((yyvsp[0].vardeclexpr)); }
    break;

  case 404: /* let_vardecl_item_list: constraint_item  */
      { (yyval.expressions1d)=new vector<Expression*>;
        if ((yyvsp[0].item)) {
          ConstraintI* ce = (yyvsp[0].item)->cast<ConstraintI>();
          (yyval.expressions1d)->push_back(ce->e());
          ce->e(nullptr);
        }
      }
    break;

  case 405: /* let_vardecl_item_list: let_vardecl_item_list comma_or_semi let_vardecl_item  */
      { (yyval.expressions1d)=(yyvsp[-2].expressions1d); if ((yyval.expressions1d) && (yyvsp[0].vardeclexpr)) (yyval.expressions1d)->push_back((yyvsp[0].vardeclexpr)); }
    break;

  case 406: /* let_vardecl_item_list: let_vardecl_item_list comma_or_semi constraint_item  */
      { (yyval.expressions1d)=(yyvsp[-2].expressions1d);
        if ((yyval.expressions1d) && (yyvsp[0].item)) {
          ConstraintI* ce = (yyvsp[0].item)->cast<ConstraintI>();
          (yyval.expressions1d)->push_back(ce->e());
          ce->e(nullptr);
        }
      }
    break;

  case 409: /* let_vardecl_item: ti_expr_and_id  */
      { (yyval.vardeclexpr) = (yyvsp[0].vardeclexpr);
        if ((yyvsp[0].vardeclexpr) && Expression::type((yyvsp[0].vardeclexpr)->ti()).any() && (yyvsp[0].vardeclexpr)->ti()->domain() == nullptr) {
          // This is an any type, not allowed without a right hand side
          yyerror(&(yylsp[0]), parm, "declarations with `any' type-inst require definition");
        }
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->toplevel(false);
      }
    break;

  case 410: /* let_vardecl_item: ti_expr_and_id "=" expr  */
      { if ((yyvsp[-2].vardeclexpr)) {
          (yyvsp[-2].vardeclexpr)->e((yyvsp[0].expression));
        }
        (yyval.vardeclexpr) = (yyvsp[-2].vardeclexpr);
        if ((yyval.vardeclexpr)) Expression::loc((yyval.vardeclexpr), (yyloc));
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->toplevel(false);
      }
    break;

  case 411: /* annotations: %empty  */
      { (yyval.expressions1d)=nullptr; }
    break;

  case 413: /* annotation_expr: expr_atom_head_nonstring  */
      { (yyval.expression) = (yyvsp[0].expression); }
    break;

  case 414: /* annotation_expr: "output"  */
      { (yyval.expression) = new Id((yylsp[0]), Constants::constants().ids.output, nullptr); }
    break;

  case 415: /* annotation_expr: string_expr  */
      { (yyval.expression) = Call::a((yylsp[0]), ASTString("mzn_expression_name"), {(yyvsp[0].expression)}); }
    break;

  case 416: /* ne_annotations: "::" annotation_expr  */
      { (yyval.expressions1d)=new std::vector<Expression*>(1);
        (*(yyval.expressions1d))[0] = (yyvsp[0].expression);
      }
    break;

  case 417: /* ne_annotations: ne_annotations "::" annotation_expr  */
      { (yyval.expressions1d)=(yyvsp[-2].expressions1d); if ((yyval.expressions1d)) (yyval.expressions1d)->push_back((yyvsp[0].expression)); }
    break;

  case 418: /* id_or_quoted_op: "identifier"  */
      { (yyval.sValue)=(yyvsp[0].sValue); }
    break;

  case 419: /* id_or_quoted_op: "identifier" "^-1"  */
      { (yyval.sValue)=strdup((std::string((yyvsp[-1].sValue))+"⁻¹").c_str()); }
    break;

  case 420: /* id_or_quoted_op: "'<->'"  */
      { (yyval.sValue)=strdup("'<->'"); }
    break;

  case 421: /* id_or_quoted_op: "'->'"  */
      { (yyval.sValue)=strdup("'->'"); }
    break;

  case 422: /* id_or_quoted_op: "'<-'"  */
      { (yyval.sValue)=strdup("'<-'"); }
    break;

  case 423: /* id_or_quoted_op: "'\\/'"  */
      { (yyval.sValue)=strdup("'\\/'"); }
    break;

  case 424: /* id_or_quoted_op: "'xor'"  */
      { (yyval.sValue)=strdup("'xor'"); }
    break;

  case 425: /* id_or_quoted_op: "'/\\'"  */
      { (yyval.sValue)=strdup("'/\\'"); }
    break;

  case 426: /* id_or_quoted_op: "'<'"  */
      { (yyval.sValue)=strdup("'<'"); }
    break;

  case 427: /* id_or_quoted_op: "'>'"  */
      { (yyval.sValue)=strdup("'>'"); }
    break;

  case 428: /* id_or_quoted_op: "'<='"  */
      { (yyval.sValue)=strdup("'<='"); }
    break;

  case 429: /* id_or_quoted_op: "'>='"  */
      { (yyval.sValue)=strdup("'>='"); }
    break;

  case 430: /* id_or_quoted_op: "'='"  */
      { (yyval.sValue)=strdup("'='"); }
    break;

  case 431: /* id_or_quoted_op: "'!='"  */
      { (yyval.sValue)=strdup("'!='"); }
    break;

  case 432: /* id_or_quoted_op: "'in'"  */
      { (yyval.sValue)=strdup("'in'"); }
    break;

  case 433: /* id_or_quoted_op: "'subset'"  */
      { (yyval.sValue)=strdup("'subset'"); }
    break;

  case 434: /* id_or_quoted_op: "'superset'"  */
      { (yyval.sValue)=strdup("'superset'"); }
    break;

  case 435: /* id_or_quoted_op: "'union'"  */
      { (yyval.sValue)=strdup("'union'"); }
    break;

  case 436: /* id_or_quoted_op: "'diff'"  */
      { (yyval.sValue)=strdup("'diff'"); }
    break;

  case 437: /* id_or_quoted_op: "'symdiff'"  */
      { (yyval.sValue)=strdup("'symdiff'"); }
    break;

  case 438: /* id_or_quoted_op: "'..'"  */
      { (yyval.sValue)=strdup("'..'"); }
    break;

  case 439: /* id_or_quoted_op: "'<..'"  */
      { (yyval.sValue)=strdup("'<..'"); }
    break;

  case 440: /* id_or_quoted_op: "'..<'"  */
      { (yyval.sValue)=strdup("'..<'"); }
    break;

  case 441: /* id_or_quoted_op: "'<..<'"  */
      { (yyval.sValue)=strdup("'<..<'"); }
    break;

  case 442: /* id_or_quoted_op: "'+'"  */
      { (yyval.sValue)=strdup("'+'"); }
    break;

  case 443: /* id_or_quoted_op: "'-'"  */
      { (yyval.sValue)=strdup("'-'"); }
    break;

  case 444: /* id_or_quoted_op: "'*'"  */
      { (yyval.sValue)=strdup("'*'"); }
    break;

  case 445: /* id_or_quoted_op: "'^'"  */
      { (yyval.sValue)=strdup("'^'"); }
    break;

  case 446: /* id_or_quoted_op: "'/'"  */
      { (yyval.sValue)=strdup("'/'"); }
    break;

  case 447: /* id_or_quoted_op: "'div'"  */
      { (yyval.sValue)=strdup("'div'"); }
    break;

  case 448: /* id_or_quoted_op: "'mod'"  */
      { (yyval.sValue)=strdup("'mod'"); }
    break;

  case 449: /* id_or_quoted_op: "'intersect'"  */
      { (yyval.sValue)=strdup("'intersect'"); }
    break;

  case 450: /* id_or_quoted_op: "'not'"  */
      { (yyval.sValue)=strdup("'not'"); }
    break;

  case 451: /* id_or_quoted_op: "'++'"  */
      { (yyval.sValue)=strdup("'++'"); }
    break;



      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (&yylloc, parm, yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          goto yyexhaustedlab;
      }
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= END)
        {
          /* Return failure if at end of input.  */
          if (yychar == END)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, parm);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp, parm);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if 1
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, parm, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, parm);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp, parm);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

