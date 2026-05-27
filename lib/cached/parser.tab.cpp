/* A Bison parser, made by GNU Bison 3.8.2.  */

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
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

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

#include <vector>

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
  YYSYMBOL_MZN_MAX_NEGATIVE_INTEGER_LITERAL = 58, /* "9223372036854775808"  */
  YYSYMBOL_MZN_LEFT_BRACKET = 59,          /* "["  */
  YYSYMBOL_MZN_LEFT_2D_BRACKET = 60,       /* "[|"  */
  YYSYMBOL_MZN_RIGHT_BRACKET = 61,         /* "]"  */
  YYSYMBOL_MZN_RIGHT_2D_BRACKET = 62,      /* "|]"  */
  YYSYMBOL_QUOTED_IDENTIFIER = 63,         /* QUOTED_IDENTIFIER  */
  YYSYMBOL_MZN_INVALID_INTEGER_LITERAL = 64, /* "invalid integer literal"  */
  YYSYMBOL_MZN_INVALID_FLOAT_LITERAL = 65, /* "invalid float literal"  */
  YYSYMBOL_MZN_UNTERMINATED_STRING = 66,   /* "unterminated string"  */
  YYSYMBOL_MZN_END_OF_LINE_IN_STRING = 67, /* "end of line inside string literal"  */
  YYSYMBOL_MZN_INVALID_NULL = 68,          /* "null character"  */
  YYSYMBOL_MZN_INVALID_STRING_LITERAL = 69, /* "invalid string literal"  */
  YYSYMBOL_MZN_EQUIV = 70,                 /* "<->"  */
  YYSYMBOL_MZN_IMPL = 71,                  /* "->"  */
  YYSYMBOL_MZN_RIMPL = 72,                 /* "<-"  */
  YYSYMBOL_MZN_OR = 73,                    /* "\\/"  */
  YYSYMBOL_MZN_XOR = 74,                   /* "xor"  */
  YYSYMBOL_MZN_AND = 75,                   /* "/\\"  */
  YYSYMBOL_MZN_LE = 76,                    /* "<"  */
  YYSYMBOL_MZN_GR = 77,                    /* ">"  */
  YYSYMBOL_MZN_LQ = 78,                    /* "<="  */
  YYSYMBOL_MZN_GQ = 79,                    /* ">="  */
  YYSYMBOL_MZN_EQ = 80,                    /* "="  */
  YYSYMBOL_MZN_NQ = 81,                    /* "!="  */
  YYSYMBOL_MZN_WEAK_EQ = 82,               /* "~="  */
  YYSYMBOL_MZN_WEAK_NQ = 83,               /* "~!="  */
  YYSYMBOL_MZN_IN = 84,                    /* "in"  */
  YYSYMBOL_MZN_SUBSET = 85,                /* "subset"  */
  YYSYMBOL_MZN_SUPERSET = 86,              /* "superset"  */
  YYSYMBOL_MZN_UNION = 87,                 /* "union"  */
  YYSYMBOL_MZN_DIFF = 88,                  /* "diff"  */
  YYSYMBOL_MZN_SYMDIFF = 89,               /* "symdiff"  */
  YYSYMBOL_MZN_DOTDOT = 90,                /* ".."  */
  YYSYMBOL_MZN_DOTDOT_LE = 91,             /* "..<"  */
  YYSYMBOL_MZN_LE_DOTDOT = 92,             /* "<.."  */
  YYSYMBOL_MZN_LE_DOTDOT_LE = 93,          /* "<..<"  */
  YYSYMBOL_MZN_PLUS = 94,                  /* "+"  */
  YYSYMBOL_MZN_MINUS = 95,                 /* "-"  */
  YYSYMBOL_MZN_WEAK_PLUS = 96,             /* "~+"  */
  YYSYMBOL_MZN_WEAK_MINUS = 97,            /* "~-"  */
  YYSYMBOL_MZN_MULT = 98,                  /* "*"  */
  YYSYMBOL_MZN_DIV = 99,                   /* "/"  */
  YYSYMBOL_MZN_IDIV = 100,                 /* "div"  */
  YYSYMBOL_MZN_MOD = 101,                  /* "mod"  */
  YYSYMBOL_MZN_WEAK_DIV = 102,             /* "~/"  */
  YYSYMBOL_MZN_WEAK_IDIV = 103,            /* "~div"  */
  YYSYMBOL_MZN_INTERSECT = 104,            /* "intersect"  */
  YYSYMBOL_MZN_WEAK_MULT = 105,            /* "~*"  */
  YYSYMBOL_MZN_POW = 106,                  /* "^"  */
  YYSYMBOL_MZN_POW_MINUS1 = 107,           /* "^-1"  */
  YYSYMBOL_MZN_NOT = 108,                  /* "not"  */
  YYSYMBOL_MZN_PLUSPLUS = 109,             /* "++"  */
  YYSYMBOL_MZN_COLONCOLON = 110,           /* "::"  */
  YYSYMBOL_PREC_ANNO = 111,                /* PREC_ANNO  */
  YYSYMBOL_MZN_EQUIV_QUOTED = 112,         /* "'<->'"  */
  YYSYMBOL_MZN_IMPL_QUOTED = 113,          /* "'->'"  */
  YYSYMBOL_MZN_RIMPL_QUOTED = 114,         /* "'<-'"  */
  YYSYMBOL_MZN_OR_QUOTED = 115,            /* "'\\/'"  */
  YYSYMBOL_MZN_XOR_QUOTED = 116,           /* "'xor'"  */
  YYSYMBOL_MZN_AND_QUOTED = 117,           /* "'/\\'"  */
  YYSYMBOL_MZN_LE_QUOTED = 118,            /* "'<'"  */
  YYSYMBOL_MZN_GR_QUOTED = 119,            /* "'>'"  */
  YYSYMBOL_MZN_LQ_QUOTED = 120,            /* "'<='"  */
  YYSYMBOL_MZN_GQ_QUOTED = 121,            /* "'>='"  */
  YYSYMBOL_MZN_EQ_QUOTED = 122,            /* "'='"  */
  YYSYMBOL_MZN_NQ_QUOTED = 123,            /* "'!='"  */
  YYSYMBOL_MZN_IN_QUOTED = 124,            /* "'in'"  */
  YYSYMBOL_MZN_SUBSET_QUOTED = 125,        /* "'subset'"  */
  YYSYMBOL_MZN_SUPERSET_QUOTED = 126,      /* "'superset'"  */
  YYSYMBOL_MZN_UNION_QUOTED = 127,         /* "'union'"  */
  YYSYMBOL_MZN_DIFF_QUOTED = 128,          /* "'diff'"  */
  YYSYMBOL_MZN_SYMDIFF_QUOTED = 129,       /* "'symdiff'"  */
  YYSYMBOL_MZN_DOTDOT_QUOTED = 130,        /* "'..'"  */
  YYSYMBOL_MZN_LE_DOTDOT_QUOTED = 131,     /* "'<..'"  */
  YYSYMBOL_MZN_DOTDOT_LE_QUOTED = 132,     /* "'..<'"  */
  YYSYMBOL_MZN_LE_DOTDOT_LE_QUOTED = 133,  /* "'<..<'"  */
  YYSYMBOL_MZN_PLUS_QUOTED = 134,          /* "'+'"  */
  YYSYMBOL_MZN_MINUS_QUOTED = 135,         /* "'-'"  */
  YYSYMBOL_MZN_MULT_QUOTED = 136,          /* "'*'"  */
  YYSYMBOL_MZN_DIV_QUOTED = 137,           /* "'/'"  */
  YYSYMBOL_MZN_IDIV_QUOTED = 138,          /* "'div'"  */
  YYSYMBOL_MZN_MOD_QUOTED = 139,           /* "'mod'"  */
  YYSYMBOL_MZN_INTERSECT_QUOTED = 140,     /* "'intersect'"  */
  YYSYMBOL_MZN_POW_QUOTED = 141,           /* "'^'"  */
  YYSYMBOL_MZN_NOT_QUOTED = 142,           /* "'not'"  */
  YYSYMBOL_MZN_COLONCOLON_QUOTED = 143,    /* "'::'"  */
  YYSYMBOL_MZN_PLUSPLUS_QUOTED = 144,      /* "'++'"  */
  YYSYMBOL_145_ = 145,                     /* ';'  */
  YYSYMBOL_146_ = 146,                     /* '{'  */
  YYSYMBOL_147_ = 147,                     /* '}'  */
  YYSYMBOL_148_ = 148,                     /* '('  */
  YYSYMBOL_149_ = 149,                     /* ')'  */
  YYSYMBOL_150_ = 150,                     /* ','  */
  YYSYMBOL_151_ = 151,                     /* ':'  */
  YYSYMBOL_152_ = 152,                     /* '|'  */
  YYSYMBOL_YYACCEPT = 153,                 /* $accept  */
  YYSYMBOL_model = 154,                    /* model  */
  YYSYMBOL_item_list = 155,                /* item_list  */
  YYSYMBOL_item_list_head = 156,           /* item_list_head  */
  YYSYMBOL_doc_file_comments = 157,        /* doc_file_comments  */
  YYSYMBOL_semi_or_none = 158,             /* semi_or_none  */
  YYSYMBOL_item = 159,                     /* item  */
  YYSYMBOL_item_tail = 160,                /* item_tail  */
  YYSYMBOL_error_item_start = 161,         /* error_item_start  */
  YYSYMBOL_include_item = 162,             /* include_item  */
  YYSYMBOL_vardecl_item = 163,             /* vardecl_item  */
  YYSYMBOL_enum_init = 164,                /* enum_init  */
  YYSYMBOL_enum_construct = 165,           /* enum_construct  */
  YYSYMBOL_string_lit_list = 166,          /* string_lit_list  */
  YYSYMBOL_enum_id_list = 167,             /* enum_id_list  */
  YYSYMBOL_assign_item = 168,              /* assign_item  */
  YYSYMBOL_constraint_item = 169,          /* constraint_item  */
  YYSYMBOL_solve_item = 170,               /* solve_item  */
  YYSYMBOL_output_item = 171,              /* output_item  */
  YYSYMBOL_output_annotation = 172,        /* output_annotation  */
  YYSYMBOL_predicate_item = 173,           /* predicate_item  */
  YYSYMBOL_function_item = 174,            /* function_item  */
  YYSYMBOL_annotation_item = 175,          /* annotation_item  */
  YYSYMBOL_ann_param = 176,                /* ann_param  */
  YYSYMBOL_operation_item_tail = 177,      /* operation_item_tail  */
  YYSYMBOL_params = 178,                   /* params  */
  YYSYMBOL_params_list = 179,              /* params_list  */
  YYSYMBOL_params_list_head = 180,         /* params_list_head  */
  YYSYMBOL_comma_or_none = 181,            /* comma_or_none  */
  YYSYMBOL_pipe_or_none = 182,             /* pipe_or_none  */
  YYSYMBOL_ti_expr_and_id_or_anon = 183,   /* ti_expr_and_id_or_anon  */
  YYSYMBOL_ti_expr_and_id = 184,           /* ti_expr_and_id  */
  YYSYMBOL_ti_expr_and_id_list = 185,      /* ti_expr_and_id_list  */
  YYSYMBOL_ti_expr_and_id_list_head = 186, /* ti_expr_and_id_list_head  */
  YYSYMBOL_ti_expr_list = 187,             /* ti_expr_list  */
  YYSYMBOL_ti_expr_list_head = 188,        /* ti_expr_list_head  */
  YYSYMBOL_array_index_list = 189,         /* array_index_list  */
  YYSYMBOL_array_index_list_head = 190,    /* array_index_list_head  */
  YYSYMBOL_array_index_entry = 191,        /* array_index_entry  */
  YYSYMBOL_ti_expr = 192,                  /* ti_expr  */
  YYSYMBOL_base_ti_expr = 193,             /* base_ti_expr  */
  YYSYMBOL_opt_opt = 194,                  /* opt_opt  */
  YYSYMBOL_base_ti_expr_tail = 195,        /* base_ti_expr_tail  */
  YYSYMBOL_array_access_expr_list = 196,   /* array_access_expr_list  */
  YYSYMBOL_array_access_expr_list_head = 197, /* array_access_expr_list_head  */
  YYSYMBOL_array_access_expr = 198,        /* array_access_expr  */
  YYSYMBOL_expr_list = 199,                /* expr_list  */
  YYSYMBOL_expr_list_head = 200,           /* expr_list_head  */
  YYSYMBOL_set_expr = 201,                 /* set_expr  */
  YYSYMBOL_expr = 202,                     /* expr  */
  YYSYMBOL_expr_atom_head = 203,           /* expr_atom_head  */
  YYSYMBOL_expr_atom_head_nonstring = 204, /* expr_atom_head_nonstring  */
  YYSYMBOL_string_expr = 205,              /* string_expr  */
  YYSYMBOL_string_quote_rest = 206,        /* string_quote_rest  */
  YYSYMBOL_access_tail = 207,              /* access_tail  */
  YYSYMBOL_set_literal = 208,              /* set_literal  */
  YYSYMBOL_tuple_literal = 209,            /* tuple_literal  */
  YYSYMBOL_record_literal = 210,           /* record_literal  */
  YYSYMBOL_record_field_list_head = 211,   /* record_field_list_head  */
  YYSYMBOL_record_field = 212,             /* record_field  */
  YYSYMBOL_set_comp = 213,                 /* set_comp  */
  YYSYMBOL_comp_tail = 214,                /* comp_tail  */
  YYSYMBOL_generator_list = 215,           /* generator_list  */
  YYSYMBOL_generator_list_head = 216,      /* generator_list_head  */
  YYSYMBOL_generator = 217,                /* generator  */
  YYSYMBOL_generator_eq = 218,             /* generator_eq  */
  YYSYMBOL_id_list = 219,                  /* id_list  */
  YYSYMBOL_id_list_head = 220,             /* id_list_head  */
  YYSYMBOL_simple_array_literal = 221,     /* simple_array_literal  */
  YYSYMBOL_simple_array_literal_2d = 222,  /* simple_array_literal_2d  */
  YYSYMBOL_simple_array_literal_3d_list = 223, /* simple_array_literal_3d_list  */
  YYSYMBOL_simple_array_literal_2d_list = 224, /* simple_array_literal_2d_list  */
  YYSYMBOL_simple_array_literal_2d_indexed_list = 225, /* simple_array_literal_2d_indexed_list  */
  YYSYMBOL_simple_array_literal_2d_indexed_list_head = 226, /* simple_array_literal_2d_indexed_list_head  */
  YYSYMBOL_simple_array_literal_2d_indexed_list_row = 227, /* simple_array_literal_2d_indexed_list_row  */
  YYSYMBOL_simple_array_literal_2d_indexed_list_row_head = 228, /* simple_array_literal_2d_indexed_list_row_head  */
  YYSYMBOL_simple_array_comp = 229,        /* simple_array_comp  */
  YYSYMBOL_comp_expr_list = 230,           /* comp_expr_list  */
  YYSYMBOL_comp_expr_list_head = 231,      /* comp_expr_list_head  */
  YYSYMBOL_if_then_else_expr = 232,        /* if_then_else_expr  */
  YYSYMBOL_elseif_list = 233,              /* elseif_list  */
  YYSYMBOL_quoted_op = 234,                /* quoted_op  */
  YYSYMBOL_quoted_op_call = 235,           /* quoted_op_call  */
  YYSYMBOL_call_expr = 236,                /* call_expr  */
  YYSYMBOL_comp_or_expr = 237,             /* comp_or_expr  */
  YYSYMBOL_comp_or_expr_head = 238,        /* comp_or_expr_head  */
  YYSYMBOL_let_expr = 239,                 /* let_expr  */
  YYSYMBOL_let_vardecl_item_list = 240,    /* let_vardecl_item_list  */
  YYSYMBOL_comma_or_semi = 241,            /* comma_or_semi  */
  YYSYMBOL_let_vardecl_item = 242,         /* let_vardecl_item  */
  YYSYMBOL_annotations = 243,              /* annotations  */
  YYSYMBOL_annotation_expr = 244,          /* annotation_expr  */
  YYSYMBOL_ne_annotations = 245,           /* ne_annotations  */
  YYSYMBOL_id_or_quoted_op = 246           /* id_or_quoted_op  */
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

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
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
#define YYFINAL  208
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   9356

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  153
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  94
/* YYNRULES -- Number of rules.  */
#define YYNRULES  468
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  809

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   399


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
     148,   149,     2,     2,   150,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   151,   145,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   146,   152,   147,     2,     2,     2,     2,
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
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   380,   380,   382,   384,   387,   396,   405,   414,   423,
     425,   428,   436,   445,   445,   447,   463,   467,   469,   471,
     472,   474,   476,   478,   480,   482,   485,   485,   485,   486,
     486,   486,   486,   486,   487,   490,   513,   524,   533,   544,
     564,   579,   591,   595,   604,   609,   615,   622,   623,   627,
     635,   636,   640,   644,   654,   656,   663,   668,   673,   680,
     682,   690,   692,   694,   696,   700,   715,   730,   744,   761,
     779,   799,   812,   826,   827,   840,   841,   846,   847,   849,
     854,   855,   859,   870,   882,   882,   883,   883,   886,   888,
     897,   909,   913,   915,   918,   922,   924,   927,   931,   933,
     937,   939,   959,   960,   972,   992,  1006,  1009,  1018,  1030,
    1043,  1051,  1063,  1073,  1086,  1100,  1118,  1122,  1127,  1128,
    1132,  1134,  1136,  1138,  1140,  1142,  1149,  1159,  1166,  1171,
    1177,  1180,  1182,  1186,  1188,  1190,  1192,  1194,  1197,  1200,
    1202,  1208,  1209,  1211,  1213,  1215,  1217,  1226,  1228,  1230,
    1232,  1234,  1236,  1238,  1240,  1242,  1244,  1246,  1248,  1250,
    1252,  1254,  1263,  1265,  1267,  1269,  1271,  1273,  1275,  1277,
    1279,  1281,  1283,  1285,  1287,  1292,  1297,  1302,  1307,  1312,
    1317,  1322,  1328,  1334,  1336,  1345,  1351,  1352,  1354,  1356,
    1358,  1360,  1362,  1364,  1366,  1368,  1370,  1372,  1374,  1376,
    1378,  1380,  1382,  1384,  1386,  1388,  1390,  1399,  1401,  1403,
    1405,  1407,  1409,  1411,  1413,  1415,  1417,  1419,  1421,  1423,
    1425,  1427,  1436,  1438,  1440,  1442,  1444,  1446,  1448,  1450,
    1452,  1454,  1456,  1458,  1460,  1462,  1467,  1472,  1477,  1482,
    1487,  1492,  1497,  1502,  1508,  1510,  1517,  1526,  1531,  1533,
    1537,  1539,  1547,  1549,  1557,  1559,  1566,  1568,  1575,  1577,
    1584,  1586,  1593,  1595,  1597,  1599,  1601,  1603,  1605,  1607,
    1609,  1611,  1613,  1615,  1616,  1623,  1625,  1632,  1633,  1640,
    1642,  1649,  1650,  1657,  1659,  1666,  1667,  1674,  1676,  1683,
    1684,  1691,  1693,  1700,  1701,  1708,  1710,  1717,  1718,  1719,
    1726,  1727,  1734,  1735,  1742,  1744,  1751,  1752,  1759,  1761,
    1770,  1772,  1778,  1786,  1797,  1806,  1812,  1821,  1830,  1832,
    1841,  1846,  1860,  1868,  1870,  1874,  1881,  1891,  1900,  1903,
    1905,  1907,  1913,  1915,  1917,  1925,  1927,  1930,  1932,  1935,
    1938,  1940,  1942,  1944,  1948,  1950,  1999,  2001,  2062,  2102,
    2105,  2110,  2117,  2122,  2125,  2128,  2138,  2150,  2161,  2164,
    2168,  2179,  2190,  2211,  2222,  2226,  2229,  2236,  2247,  2267,
    2281,  2297,  2298,  2302,  2304,  2306,  2308,  2310,  2312,  2314,
    2316,  2318,  2320,  2322,  2324,  2326,  2328,  2330,  2332,  2334,
    2336,  2338,  2340,  2342,  2344,  2346,  2348,  2350,  2352,  2354,
    2356,  2360,  2368,  2400,  2402,  2404,  2405,  2427,  2483,  2505,
    2562,  2565,  2571,  2577,  2591,  2593,  2595,  2611,  2613,  2620,
    2629,  2631,  2639,  2641,  2650,  2650,  2653,  2661,  2672,  2673,
    2676,  2678,  2680,  2684,  2688,  2692,  2694,  2696,  2698,  2700,
    2702,  2704,  2706,  2708,  2710,  2712,  2714,  2716,  2718,  2720,
    2722,  2724,  2726,  2728,  2730,  2732,  2734,  2736,  2738,  2740,
    2742,  2744,  2746,  2748,  2750,  2752,  2754,  2756,  2758
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
  "\"type\"", "\"_\"", "\"variant_record\"", "\"where\"",
  "\"9223372036854775808\"", "\"[\"", "\"[|\"", "\"]\"", "\"|]\"",
  "QUOTED_IDENTIFIER", "\"invalid integer literal\"",
  "\"invalid float literal\"", "\"unterminated string\"",
  "\"end of line inside string literal\"", "\"null character\"",
  "\"invalid string literal\"", "\"<->\"", "\"->\"", "\"<-\"", "\"\\\\/\"",
  "\"xor\"", "\"/\\\\\"", "\"<\"", "\">\"", "\"<=\"", "\">=\"", "\"=\"",
  "\"!=\"", "\"~=\"", "\"~!=\"", "\"in\"", "\"subset\"", "\"superset\"",
  "\"union\"", "\"diff\"", "\"symdiff\"", "\"..\"", "\"..<\"", "\"<..\"",
  "\"<..<\"", "\"+\"", "\"-\"", "\"~+\"", "\"~-\"", "\"*\"", "\"/\"",
  "\"div\"", "\"mod\"", "\"~/\"", "\"~div\"", "\"intersect\"", "\"~*\"",
  "\"^\"", "\"^-1\"", "\"not\"", "\"++\"", "\"::\"", "PREC_ANNO",
  "\"'<->'\"", "\"'->'\"", "\"'<-'\"", "\"'\\\\/'\"", "\"'xor'\"",
  "\"'/\\\\'\"", "\"'<'\"", "\"'>'\"", "\"'<='\"", "\"'>='\"", "\"'='\"",
  "\"'!='\"", "\"'in'\"", "\"'subset'\"", "\"'superset'\"", "\"'union'\"",
  "\"'diff'\"", "\"'symdiff'\"", "\"'..'\"", "\"'<..'\"", "\"'..<'\"",
  "\"'<..<'\"", "\"'+'\"", "\"'-'\"", "\"'*'\"", "\"'/'\"", "\"'div'\"",
  "\"'mod'\"", "\"'intersect'\"", "\"'^'\"", "\"'not'\"", "\"'::'\"",
  "\"'++'\"", "';'", "'{'", "'}'", "'('", "')'", "','", "':'", "'|'",
  "$accept", "model", "item_list", "item_list_head", "doc_file_comments",
  "semi_or_none", "item", "item_tail", "error_item_start", "include_item",
  "vardecl_item", "enum_init", "enum_construct", "string_lit_list",
  "enum_id_list", "assign_item", "constraint_item", "solve_item",
  "output_item", "output_annotation", "predicate_item", "function_item",
  "annotation_item", "ann_param", "operation_item_tail", "params",
  "params_list", "params_list_head", "comma_or_none", "pipe_or_none",
  "ti_expr_and_id_or_anon", "ti_expr_and_id", "ti_expr_and_id_list",
  "ti_expr_and_id_list_head", "ti_expr_list", "ti_expr_list_head",
  "array_index_list", "array_index_list_head", "array_index_entry",
  "ti_expr", "base_ti_expr", "opt_opt", "base_ti_expr_tail",
  "array_access_expr_list", "array_access_expr_list_head",
  "array_access_expr", "expr_list", "expr_list_head", "set_expr", "expr",
  "expr_atom_head", "expr_atom_head_nonstring", "string_expr",
  "string_quote_rest", "access_tail", "set_literal", "tuple_literal",
  "record_literal", "record_field_list_head", "record_field", "set_comp",
  "comp_tail", "generator_list", "generator_list_head", "generator",
  "generator_eq", "id_list", "id_list_head", "simple_array_literal",
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

#define YYPACT_NINF (-673)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-81)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    1248,  -119,   -75,   -71,   -62,    30,  -673,  4757,  -673,  -673,
    2567,  -673,    14,    14,   -42,  -673,    63,    84,    43,  -673,
    4027,    98,  -673,  3005,  4757,   149,    73,  -673,    27,   140,
    3443,  4173,   178,    40,   -35,    83,  -673,   190,    57,   202,
      39,  4319,   833,  5633,  5633,  5633,  5633,  5633,  4903,  -673,
    -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,
    -673,  -673,  -673,  -673,  -673,  -673,  -673,    65,    67,    71,
      81,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,
    -673,  4465,  5049,   221,  -673,   109,  2129,   513,  -673,  -673,
    -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,   177,   -65,
    -673,  -673,  1223,  -673,  -673,  -673,    46,    90,    93,   148,
     151,   163,   167,   320,   111,  -673,   323,  -673,  2421,  -673,
    -673,  -673,  -673,  5195,  4757,   115,  1541,   326,    44,  4757,
    4757,  4757,  4757,  4757,  4611,  4757,   121,   123,   127,   131,
    8205,  -673,  -673,  -673,  -673,  3589,  3735,  -673,   141,  -673,
    3151,    82,  8846,    83,   -15,  8305,  -673,  -673,  2713,  3005,
     246,  -673,    22,  8846,   -51,  3005,  3881,  4757,  5821,   237,
     186,   -48,  3005,    83,  -673,  4757,   327,  -673,  5924,   277,
     194,  -673,   996,  8846,   -43,   278,   195,  -673,    74,  8328,
    8328,  8328,  8328,    31,  -673,    31,  4757,  5633,  5633,  5633,
    -673,   208,   206,  6024,    -6,  6265,   207,  -673,  -673,  2275,
    -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,
    -673,  -673,  -673,  4757,  3297,   353,  5633,  5633,  5633,  5633,
    5633,  5633,  5633,  5779,  5779,  5779,  5779,  5633,  5633,  5633,
    5633,  5633,  5633,  5633,  5633,  5633,  5633,  5633,  5633,  5633,
    5821,  -673,   329,  -673,   336,  -673,   348,  -673,   352,  -673,
     354,  -673,   357,  -673,   381,  -673,   392,  4757,  -673,   425,
    -673,  4757,  4757,  4757,  4757,   297,   210,  -673,  8846,  8846,
    1688,    11,  -673,  8405,   214,   215,  -673,  5195,  -673,   969,
     969,   969,   969,    85,  -673,    85,   108,  4757,  4757,  4757,
    4757,  4757,  4757,  -673,  4757,  4757,  4757,  4757,  4757,  4757,
    4757,  4757,  4757,  4757,  4757,  4757,  4757,  4757,  4757,  4757,
    4757,  4757,  4757,  4757,  4757,  5341,  5341,  5341,  5341,  4757,
    4757,  4757,  4757,  4757,  4757,  4757,  4757,  4757,  4757,  4757,
    4757,  4757,  4757,  5821,    -5,  -673,   324,  -673,  1394,   287,
      79,   308,   222,  -673,   262,  4757,   298,  8067,  4757,   293,
    -673,   300,    60,    59,  -673,   262,  3881,   233,  4757,  4757,
    -673,   141,   364,  -673,   240,   242,  -673,  6953,  -673,  -673,
    -673,  -673,  4757,  4757,  -673,  5821,   141,   364,   244,   250,
     262,   314,  6994,  -673,  4757,    17,  -673,  4757,  -673,  -673,
    -673,  8846,   252,  -673,   253,  -673,  4757,  -673,  4757,  4757,
    -673,  6307,  6728,  6828,  6853,  -673,  4757,  -673,    17,  4757,
     479,  1835,   390,   257,  2129,  -673,  8846,  -673,    80,   304,
      34,  6207,  6207,  5963,  5963,  5963,  8328,  8328,  8328,  8328,
     391,   391,   391,   391,    66,    66,    66,    66,    66,    66,
    8428,    66,    31,  -673,  -673,  -673,  -673,  -673,  -673,  -673,
    -673,  -673,  6411,  -673,  -673,  5195,  -673,  -673,   261,  4757,
    4757,   267,  5487,  -673,   356,  6453,  6557,  6599,  6703,   309,
    -673,    56,  8946,  8984,  8984,  9084,  9084,  9119,  9219,  9219,
    9219,  9219,  9219,  9219,  9219,  9219,  9246,  9246,  9246,   376,
     376,   376,   969,   969,   969,   969,   347,   347,   347,   347,
      85,    85,    85,    85,    85,    85,  8228,    85,   108,   158,
    -673,  3881,  4757,  3881,   271,   272,   273,  -673,  -673,    60,
    4757,  5633,   380,  3151,  -673,  8846,     9,   318,  -673,  -673,
    -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,
    -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,
    -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,
     141,  8846,  4757,  4757,   420,  -673,   351,  -673,  2859,  -673,
    1982,  7094,  8846,   364,   280,    83,  -673,  3005,  -673,   395,
    8846,  8846,  -673,   364,    83,  -673,  3005,  -673,  3005,  -673,
    6065,   359,   362,   393,  -673,   333,  -673,   445,   419,   358,
    6165,  4757,  4757,  -673,   136,  8846,  8846,  -673,  4757,  -673,
    5633,  -673,  5633,  -673,  5633,  8846,   363,  8846,  -673,   482,
    -673,   355,   360,  -673,  -673,  -673,  3005,  -673,  -673,  4757,
    -673,   361,  8846,  8846,  4757,    26,  8505,  -673,  -673,  4757,
    -673,  4757,  -673,  4757,  -673,  4757,  -673,  7135,  -673,  -673,
    -673,  3005,  -673,  8846,  1223,  3005,  -673,   365,   366,   498,
     510,   408,  -673,  -673,   364,   255,  8846,  8846,    83,  4757,
     434,  -673,  -673,  -673,   370,  -673,    83,   514,   441,  -673,
    3881,    83,   441,   262,   262,    17,  4757,  4757,  -673,    17,
    -673,  4757,  4757,   165,  -673,  4757,  -673,   372,  4757,  7235,
    7940,  7965,  8065,  -673,  -673,  -673,   377,  7276,  4757,  7376,
    4757,  4757,  7417,  7517,  7558,  7658,   483,  -673,   262,  4757,
    4757,  -673,   -18,  -673,   378,    12,    83,  4757,  4757,  -673,
    8846,  4757,  -673,   441,  -673,  4757,  -673,  -673,   441,  -673,
     466,  8846,  8846,  -673,   472,  8846,  8605,  -673,  -673,  8846,
    4757,  -673,  -673,  -673,  -673,   364,  -673,  7699,  -673,  8846,
    8846,  -673,  -673,  -673,  -673,  3881,  7799,  7840,  -673,   522,
     527,   389,  -673,   441,  8705,  8805,  8846,  -673,  8846,  -673,
    -673,  4757,  4757,    83,  -673,  -673,  -673,  -673,  -673,  -673,
    -673,  -673,  -673,  4757,  8846,  8846,   441,  8846,  -673
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       0,     0,   265,   263,   269,   254,   310,     0,   128,   129,
       0,    11,   118,   118,   271,   124,     0,   117,     0,   121,
       0,     0,   122,     0,     0,     0,   267,   120,     0,     0,
       0,     0,     0,     0,     0,   428,   123,     0,     0,     0,
     258,     0,     0,     0,     0,     0,     0,     0,     0,   373,
     374,   375,   376,   377,   378,   379,   380,   381,   382,   383,
     384,   385,   386,   387,   388,   389,   390,     0,     0,     0,
       0,   391,   392,   393,   395,   396,   397,   398,   394,   400,
     399,     0,     0,     0,     2,    13,     0,     5,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    36,     0,
     102,   106,   127,   141,   248,   249,   273,   302,   306,   277,
     281,   285,   289,   293,     0,   405,   298,   297,     0,   266,
     264,   270,   315,     0,     0,   256,     0,   255,   254,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   186,   311,    15,   119,     0,     0,   272,    77,   116,
       0,     0,    54,   428,     0,     0,    35,   268,     0,     0,
       0,   107,     0,    59,    77,     0,     0,     0,     0,     0,
     429,    77,     0,   428,   260,     0,   259,   344,   365,     0,
      84,   346,     0,   359,     0,     0,    86,   355,    84,   154,
     155,   156,   157,   183,   185,   184,     0,     0,     0,     0,
     318,     0,    84,   139,   254,     0,    84,   323,     1,    14,
       4,    12,     6,    34,    29,    27,    32,    26,    28,    31,
      30,    33,     9,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   150,   151,   152,   153,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   275,   274,   304,   303,   308,   307,   279,   278,   283,
     282,   287,   286,   291,   290,   295,   294,     0,   300,   299,
      10,   134,   135,   136,   137,     0,    84,   131,   133,    53,
       0,   254,   403,   411,     0,    84,   317,     0,   257,   214,
     215,   216,   217,   245,   247,   246,   244,     0,     0,     0,
       0,     0,     0,   312,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   210,   211,   212,   213,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   109,     0,   108,     0,    71,
     254,     0,    84,    98,   100,     0,    38,     0,     0,     0,
     421,   426,     0,     0,   420,   104,     0,     0,     0,     0,
      61,    77,    73,    92,     0,    84,   110,     0,   431,   430,
     432,   433,     0,     0,    56,     0,    77,    73,     0,    84,
      95,     0,     0,   261,     0,     0,   345,    85,   364,   349,
     352,   139,     0,   348,     0,   347,    87,   354,    85,   356,
     358,     0,     0,     0,     0,   319,    85,   138,     0,     0,
     250,     0,    85,     0,     0,     7,    37,   105,   428,   182,
     181,   179,   180,   143,   144,   145,   146,   147,   148,   149,
     167,   168,   174,   175,   169,   170,   171,   172,   177,   178,
     166,   176,   173,   142,   276,   305,   309,   280,   284,   288,
     292,   296,     0,   301,   314,    85,   130,   404,     0,     0,
       0,   406,    85,   410,     0,     0,     0,     0,     0,   243,
     313,   242,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   240,   241,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   228,   229,   235,   236,
     230,   231,   232,   233,   238,   239,   226,   237,   234,   227,
     187,     0,     0,     0,     0,     0,    84,    82,    88,    89,
       0,     0,     0,    85,    97,    55,     0,   435,   437,   438,
     439,   440,   441,   442,   443,   444,   445,   446,   447,   448,
     449,   450,   451,   452,   453,   454,   455,   456,   457,   458,
     459,   460,   461,   463,   464,   465,   466,   462,   467,   468,
      77,   371,     0,     0,     0,   425,     0,   424,     0,   112,
       0,     0,    60,    73,     0,   428,   126,    85,    91,     0,
      58,    57,   434,    73,   428,   125,    85,    94,     0,   262,
     366,   340,   341,     0,   327,    84,   329,   330,     0,    84,
     367,   350,     0,   357,    84,   361,   360,   165,     0,   163,
       0,   162,     0,   164,     0,   140,     0,   325,   252,   251,
     320,     0,     0,   324,   322,     8,    80,    90,   402,     0,
     132,   408,   413,   412,     0,   254,   414,   316,   225,     0,
     223,     0,   222,     0,   224,     0,   114,     0,   113,    79,
      78,    85,    81,    72,   101,     0,    99,     0,     0,    47,
      50,    39,    42,   436,    73,     0,   417,   427,   428,     0,
       0,   423,   422,    62,     0,    64,   428,     0,    75,    93,
       0,   428,    75,    96,    41,     0,     0,     0,   363,    85,
     328,     0,     0,    85,   339,     0,   353,     0,     0,     0,
       0,     0,     0,   326,   253,   321,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    83,   103,     0,
       0,    48,     0,    51,    84,     0,   428,     0,     0,   370,
     418,     0,    63,    75,    74,     0,    65,   111,    75,    66,
       0,   337,   338,   332,   333,   331,   335,   342,   343,   368,
     351,   161,   159,   158,   160,    73,   401,     0,   407,   416,
     415,   221,   219,   218,   220,     0,     0,     0,    40,     0,
      85,     0,    43,    75,     0,     0,   419,    67,    76,    68,
     362,     0,     0,   428,   409,   115,    45,    46,    49,    52,
      44,    69,   369,     0,   334,   336,    75,   372,    70
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -673,  -673,  -673,  -673,   328,  -673,   -78,   530,  -673,  -673,
    -673,  -673,  -193,  -673,  -673,  -673,  -154,  -673,  -673,  -673,
    -673,  -673,  -673,  -321,  -672,  -110,   -93,  -673,  -177,  -673,
    -116,  -149,  -673,  -673,  -673,  -673,  -673,  -673,    16,   -11,
     330,   534,   -16,   263,  -673,    86,   -80,  -673,     4,    -7,
     574,  -162,  -129,   251,   -29,  -673,  -673,  -673,  -673,   133,
    -673,  -413,  -673,  -673,  -143,  -139,  -673,  -673,  -673,  -673,
    -673,   -49,  -673,  -673,   162,   164,  -673,  -673,  -673,  -673,
    -673,  -673,  -673,  -673,   289,  -673,  -673,  -673,  -673,    -3,
     -33,  -229,  -673,  -673
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    83,    84,    85,    86,   210,    87,    88,   222,    89,
      90,   671,   672,   732,   734,    91,    92,    93,    94,   369,
      95,    96,    97,   585,   746,   349,   525,   526,   410,   407,
     527,    98,   374,   375,   388,   389,   351,   352,   353,    99,
     100,   145,   101,   275,   276,   277,   400,   202,   102,   401,
     141,   104,   105,   142,   127,   106,   107,   108,   206,   207,
     109,   603,   604,   605,   606,   607,   608,   609,   110,   111,
     184,   402,   185,   186,   187,   188,   112,   179,   180,   113,
     675,   114,   115,   116,   284,   285,   117,   363,   578,   364,
     637,   381,   170,   570
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     140,   201,   169,   398,   360,   626,   379,   166,   212,   361,
     122,   176,   154,   152,   161,   667,   373,   155,   667,   403,
     749,   453,   355,   601,   163,   417,   118,   122,   367,   423,
       6,     7,   119,   370,   178,   183,   120,   521,   226,   380,
     270,   226,   122,   778,   224,   121,   122,   189,   190,   191,
     192,   193,   195,   123,   372,   122,   371,   144,   227,   386,
     122,   387,   122,   301,   668,   147,   594,   668,   669,   148,
     123,   787,   602,   226,   203,   205,   789,   252,   254,   256,
     258,   260,   262,   264,   266,   123,   225,   269,   379,   123,
       6,     7,   301,   227,   224,   122,   149,   348,   123,   466,
     348,   125,   150,   123,   153,   123,   122,   404,   473,   122,
     124,   801,   304,   167,   520,   301,   278,   279,   125,   283,
     356,   380,   289,   290,   291,   292,   293,   295,   296,   345,
     347,   425,   779,   125,   808,   304,   357,   125,   123,   354,
     391,   250,   126,   522,   250,   419,   174,   362,   365,   123,
     376,   125,   123,   251,   362,   670,   592,   156,   670,   126,
     377,   390,   469,   531,   122,   301,   343,   122,   392,   224,
     368,   757,   249,   158,   126,   534,   250,   720,   126,   122,
     157,   379,   159,   122,   164,   304,   125,   175,   165,   411,
     168,   341,   126,   168,   342,   343,   171,   253,   588,   528,
     255,   412,   413,   414,   575,   172,   576,   123,   173,   577,
     123,   574,   597,   196,   380,   197,   426,   342,   343,   198,
     758,   208,   123,   379,   408,   409,   123,   126,   636,   199,
     429,   430,   431,   432,   433,   434,   435,   436,   437,   438,
     439,   440,   441,   442,   443,   444,   445,   446,   447,   448,
     449,   450,   451,   452,   209,   257,   380,   223,   259,   267,
     462,   583,   686,   280,   289,   290,   291,   292,   343,   297,
     261,   298,   691,   283,   263,   299,   593,   382,   383,   300,
     278,   384,   750,   737,   738,   739,   408,   708,   366,   348,
     475,   476,   477,   478,   479,   140,   385,   481,   482,   483,
     484,   485,   486,   487,   488,   489,   490,   491,   492,   493,
     494,   495,   496,   497,   498,   499,   500,   501,   502,   503,
     504,   505,   506,   507,   508,   509,   510,   511,   512,   513,
     514,   515,   516,   517,   518,   519,   122,   529,   396,   122,
     405,   631,   286,   286,   397,   286,   635,   406,   535,   662,
     579,   571,   286,   736,   301,   415,   416,   422,   464,   428,
     465,   581,   582,   471,   286,   472,   523,   530,   286,   532,
     286,   224,   533,   286,   304,   590,   591,   572,   536,   123,
     573,   580,   123,   301,   584,   287,   287,   600,   287,   586,
     610,   629,   587,   595,   598,   287,   632,   286,   226,   183,
     596,   615,   616,   304,   611,   612,   634,   287,   286,   625,
     641,   287,   627,   287,   250,   644,   287,   647,   227,   343,
     659,   660,   665,   661,   681,   673,   678,   265,   700,   361,
     268,   687,   704,   288,   393,   679,   454,   690,   689,   696,
     287,   286,   697,   455,   793,   333,   334,   335,   336,   337,
     338,   287,   340,   341,   698,   456,   342,   343,   278,   457,
     674,   458,   642,   643,   459,   646,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     339,   340,   341,   699,   287,   342,   343,   528,   460,   241,
     242,   243,   244,   245,   246,   122,   248,   249,   286,   461,
     684,   250,   701,   702,   715,   656,   731,   658,   703,   718,
     713,   419,   528,   729,   730,   657,   733,   735,   741,   742,
     744,   745,   354,   663,   760,   775,   765,   790,   780,   791,
     798,   706,   463,   799,   213,   664,   800,   424,   123,   214,
     143,   287,   782,   716,   215,   727,   216,   146,   217,   666,
     474,   640,   688,   480,   427,   633,   753,   781,   218,   219,
     754,   692,   220,   707,   221,   676,   677,   362,   613,   468,
     614,     0,     0,     0,   103,   682,   362,     0,     0,     0,
       0,     0,     0,     0,   103,   693,   628,   694,     0,   714,
       0,     0,     0,     0,     0,     0,     0,   103,     0,     0,
       0,     0,     0,     0,   103,     0,     0,     0,     0,     0,
       0,   709,     0,     0,     0,     0,     0,   103,   103,   103,
     103,   103,   103,     0,   710,   529,   711,     0,   712,     0,
       0,     0,   717,     0,     0,     0,     0,   719,     0,     0,
       0,     0,   722,     0,   723,     0,   724,     0,   725,     0,
     529,     0,     0,   743,   728,     0,     0,     0,   748,     0,
     103,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   740,     0,   747,     0,     0,     0,     0,     0,
     706,     0,     0,     0,     0,     0,     0,     0,     0,   751,
     752,     0,   103,     0,   755,   756,     0,     0,   759,     0,
       0,   616,     0,   783,     0,     0,     0,     0,     0,     0,
       0,   767,     0,   769,   770,     0,     0,     0,     0,   103,
     103,     0,   776,   777,   103,     0,     0,     0,     0,     0,
     784,   785,   103,   103,   786,     0,     0,     0,   788,   103,
     103,     0,     0,     0,     0,     0,   103,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   795,
     806,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   103,   103,   103,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   103,   804,   805,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   807,     0,   103,     0,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
     103,   103,   103,   103,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     2,     3,     4,   128,
       0,     6,     7,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    14,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    24,     0,    26,
       0,    28,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    40,     0,
       0,     0,    41,    42,     0,   181,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   103,   129,   130,   131,   132,   133,   134,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     103,   135,     0,     0,     0,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,   136,   137,   138,   139,    71,    72,    73,
      74,    75,    76,    77,    78,    79,   301,    80,     0,    81,
       0,    82,     0,     0,     0,   182,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   304,     0,   103,     2,
       3,     4,   128,     0,     6,     7,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    14,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      24,     0,    26,     0,    28,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    40,     0,     0,     0,    41,    42,     0,     0,   -81,
     -81,   -81,   -81,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,     0,   340,   341,     0,     0,   342,   343,
       0,     0,     0,     0,     0,     0,   129,   130,   131,   132,
     133,   134,     0,     0,     0,   103,     0,   103,     0,     0,
       0,     0,     0,     0,   135,   103,     0,   103,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,   136,   137,   138,   139,
      71,    72,    73,    74,    75,    76,    77,    78,    79,     0,
      80,     0,    81,     0,    82,     0,     0,     0,   399,     0,
       0,     0,   103,     0,     0,     0,     0,     0,     0,     0,
       0,   103,     0,     0,     0,     0,     0,     0,     0,     0,
     103,     0,   103,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   103,     0,   103,     0,   103,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     103,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     226,     0,     0,     0,     0,   103,     0,     0,     0,   103,
       0,     0,     0,     0,     0,     0,     0,     0,    -3,     1,
     227,     2,     3,     4,     5,     0,     6,     7,     0,     0,
       8,     9,    10,    11,   103,    12,    13,    14,    15,    16,
      17,    18,    19,     0,    20,     0,     0,     0,     0,    21,
      22,    23,    24,    25,    26,    27,    28,    29,     0,     0,
       0,    30,     0,    31,    32,    33,    34,    35,    36,    37,
       0,    38,    39,    40,     0,   228,   229,    41,    42,     0,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,   242,   243,   244,   245,   246,   247,   248,   249,
       0,     0,     0,   250,     0,     0,     0,     0,    43,    44,
      45,    46,    47,    48,     0,     0,     0,     0,     0,   103,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,     0,    80,     0,    81,   524,    82,     2,     3,     4,
     128,     0,     6,     7,     0,     0,     8,     9,     0,     0,
       0,    12,    13,    14,    15,     0,    17,    18,    19,     0,
       0,     0,     0,     0,     0,     0,    22,     0,    24,     0,
      26,    27,    28,    29,     0,     0,     0,    30,     0,     0,
       0,    33,    34,     0,    36,     0,     0,    38,     0,    40,
       0,     0,     0,    41,    42,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    43,    44,    45,    46,    47,    48,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,     0,    80,     0,
      81,     0,    82,   -80,     2,     3,     4,   281,     0,     6,
       7,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      14,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    24,     0,    26,     0,    28,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    40,     0,     0,     0,
      41,    42,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   129,   130,   131,   132,   133,   134,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   135,
       0,     0,     0,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,   136,   137,   138,   139,    71,    72,    73,    74,    75,
      76,    77,    78,    79,     0,    80,     0,    81,     0,    82,
     282,     2,     3,     4,   281,     0,     6,     7,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    14,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    24,     0,    26,     0,    28,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    40,     0,     0,     0,    41,    42,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   129,   130,
     131,   132,   133,   134,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   135,     0,     0,     0,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,   136,   137,
     138,   139,    71,    72,    73,    74,    75,    76,    77,    78,
      79,     0,    80,     0,    81,     0,    82,   467,     2,     3,
       4,   128,     0,     6,     7,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    14,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    24,
       0,    26,     0,    28,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      40,     0,     0,     0,    41,    42,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   129,   130,   131,   132,   133,
     134,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   135,     0,     0,     0,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,   136,   137,   138,   139,    71,
      72,    73,    74,    75,    76,    77,    78,    79,     0,    80,
       0,    81,     0,    82,   630,     2,     3,     4,   128,     0,
       6,     7,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    14,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    24,     0,    26,     0,
      28,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    40,     0,     0,
       0,    41,    42,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   129,   130,   131,   132,   133,   134,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     135,     0,     0,     0,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,   136,   137,   138,   139,    71,    72,    73,    74,
      75,    76,    77,    78,    79,     0,    80,     0,    81,     0,
      82,   683,     2,     3,     4,     5,     0,     6,     7,     0,
       0,     8,     9,    10,   211,     0,    12,    13,    14,    15,
      16,    17,    18,    19,     0,    20,     0,     0,     0,     0,
      21,    22,    23,    24,    25,    26,    27,    28,    29,     0,
       0,     0,    30,     0,    31,    32,    33,    34,    35,    36,
      37,     0,    38,    39,    40,     0,     0,     0,    41,    42,
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
      40,     0,     0,     0,    41,    42,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    43,    44,    45,    46,    47,
      48,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,     0,    80,
       0,    81,     0,    82,     2,     3,     4,     5,     0,     6,
       7,     0,     0,     8,     9,    10,     0,     0,    12,    13,
      14,    15,    16,    17,    18,    19,     0,    20,     0,     0,
       0,     0,    21,    22,    23,    24,    25,    26,    27,    28,
      29,     0,     0,     0,    30,     0,    31,    32,    33,    34,
      35,    36,    37,     0,    38,    39,    40,     0,     0,     0,
      41,    42,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    43,    44,    45,    46,    47,    48,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,     0,    80,     0,    81,     0,    82,
       2,     3,     4,     5,     0,     6,     7,     0,     0,     8,
       9,     0,     0,     0,    12,    13,    14,    15,    16,    17,
      18,    19,     0,    20,     0,     0,     0,     0,    21,    22,
      23,    24,    25,    26,    27,    28,    29,     0,     0,     0,
      30,     0,    31,    32,    33,    34,    35,    36,    37,     0,
      38,    39,    40,     0,     0,     0,    41,    42,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    43,    44,    45,
      46,    47,    48,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
       0,    80,     0,    81,     0,    82,     2,     3,     4,   128,
       0,     6,     7,     0,     0,     8,     9,     0,     0,     0,
      12,    13,    14,    15,     0,    17,    18,    19,     0,    20,
       0,     0,     0,     0,     0,    22,     0,    24,     0,    26,
      27,    28,    29,     0,     0,     0,    30,     0,     0,     0,
      33,    34,     0,    36,     0,     0,    38,     0,    40,     0,
       0,     0,    41,    42,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    43,    44,    45,    46,    47,    48,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,     0,    80,     0,    81,
     359,    82,     2,     3,     4,   128,     0,     6,     7,     0,
       0,     8,     9,     0,     0,     0,    12,    13,    14,    15,
       0,    17,    18,    19,     0,    20,     0,     0,     0,     0,
       0,    22,     0,    24,     0,    26,    27,    28,    29,     0,
       0,     0,    30,     0,     0,     0,    33,    34,     0,    36,
       0,     0,    38,     0,    40,     0,     0,     0,    41,    42,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    43,
      44,    45,    46,    47,    48,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,     0,    80,     0,    81,   680,    82,     2,     3,
       4,   128,     0,     6,     7,     0,     0,     8,     9,     0,
       0,     0,    12,    13,    14,    15,     0,    17,    18,    19,
       0,     0,     0,     0,     0,     0,     0,    22,     0,    24,
       0,    26,    27,    28,    29,     0,     0,     0,    30,     0,
       0,     0,    33,    34,     0,    36,     0,     0,    38,     0,
      40,     0,     0,     0,    41,    42,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    43,    44,    45,    46,    47,
      48,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,     0,    80,
       0,    81,     0,    82,     2,     3,     4,   350,     0,     6,
       7,     0,     0,     8,     9,     0,     0,     0,    12,    13,
      14,    15,     0,    17,    18,    19,     0,     0,     0,     0,
       0,     0,     0,    22,     0,    24,     0,    26,    27,    28,
      29,     0,     0,     0,    30,     0,     0,     0,    33,    34,
       0,    36,     0,     0,    38,     0,    40,     0,     0,     0,
      41,    42,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    43,    44,    45,    46,    47,    48,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,     0,    80,     0,    81,     0,    82,
       2,     3,     4,   128,     0,     6,     7,     0,     0,     8,
       9,     0,     0,     0,    12,    13,    14,    15,     0,    17,
       0,    19,     0,     0,     0,     0,     0,     0,     0,    22,
       0,    24,     0,    26,    27,    28,     0,     0,     0,     0,
      30,     0,     0,     0,    33,    34,     0,    36,     0,     0,
      38,     0,    40,     0,     0,     0,    41,    42,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    43,    44,    45,
      46,    47,    48,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
       0,    80,     0,    81,     0,    82,     2,     3,     4,   128,
       0,     6,     7,     0,     0,     8,     9,     0,     0,     0,
       0,     0,    14,    15,     0,     0,     0,    19,     0,     0,
       0,     0,     0,     0,     0,    22,     0,    24,     0,    26,
      27,    28,     0,     0,     0,     0,     0,     0,     0,     0,
      33,   160,     0,    36,     0,     0,    38,     0,    40,     0,
       0,     0,    41,    42,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    43,    44,    45,    46,    47,    48,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,     0,    80,     0,    81,
       0,    82,     2,     3,     4,   128,     0,     6,     7,     0,
       0,     8,     9,     0,     0,     0,     0,     0,    14,    15,
       0,     0,     0,    19,     0,     0,     0,     0,     0,     0,
       0,    22,     0,    24,     0,    26,    27,    28,     0,     0,
       0,     0,     0,     0,     0,     0,    33,   344,     0,    36,
       0,     0,    38,     0,    40,     0,     0,     0,    41,    42,
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
       0,     0,    33,   346,     0,    36,     0,     0,    38,     0,
      40,     0,     0,     0,    41,    42,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    43,    44,    45,    46,    47,
      48,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,     0,    80,
       0,    81,     0,    82,     2,     3,     4,   128,     0,     6,
       7,     0,     0,     8,     9,     0,     0,     0,     0,     0,
      14,    15,     0,     0,     0,    19,     0,     0,     0,     0,
       0,     0,     0,    22,     0,    24,     0,    26,    27,    28,
       0,     0,     0,     0,     0,     0,     0,     0,    33,     0,
       0,    36,     0,     0,    38,     0,    40,     0,     0,     0,
      41,    42,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    43,    44,    45,    46,    47,    48,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,     0,    80,     0,    81,     0,    82,
       2,     3,     4,   128,     0,     6,     7,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    14,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    24,     0,    26,     0,    28,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    40,     0,     0,     0,    41,    42,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   129,   130,   131,
     132,   133,   134,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   135,     0,   151,     0,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,   136,   137,   138,
     139,    71,    72,    73,    74,    75,    76,    77,    78,    79,
       0,    80,     0,    81,     0,    82,     2,     3,     4,   128,
       0,     6,     7,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    14,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    24,     0,    26,
       0,    28,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    40,     0,
       0,     0,    41,    42,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   129,   130,   131,   132,   133,   134,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   135,     0,   162,     0,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,   136,   137,   138,   139,    71,    72,    73,
      74,    75,    76,    77,    78,    79,     0,    80,     0,    81,
       0,    82,     2,     3,     4,   128,     0,     6,     7,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    14,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    24,     0,    26,     0,    28,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    40,     0,     0,     0,    41,    42,
     177,     0,     0,     0,     0,     0,     0,     0,     0,     0,
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
      40,     0,     0,     0,    41,    42,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   129,   130,   131,   132,   133,
     134,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   135,     0,     0,     0,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,   136,   137,   138,   139,    71,
      72,    73,    74,    75,    76,    77,    78,    79,     0,    80,
       0,    81,   200,    82,     2,     3,     4,   128,     0,     6,
       7,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      14,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    24,     0,    26,     0,    28,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    40,     0,     0,   294,
      41,    42,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   129,   130,   131,   132,   133,   134,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   135,
       0,     0,     0,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,   136,   137,   138,   139,    71,    72,    73,    74,    75,
      76,    77,    78,    79,     0,    80,     0,    81,     0,    82,
       2,     3,     4,   128,     0,     6,     7,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    14,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    24,     0,    26,     0,    28,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    40,     0,     0,     0,    41,    42,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   129,   130,   131,
     132,   133,   134,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   135,     0,     0,     0,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,   136,   137,   138,
     139,    71,    72,    73,    74,    75,    76,    77,    78,    79,
       0,    80,     0,    81,     0,    82,     2,     3,     4,   128,
       0,     6,     7,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    14,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    24,     0,    26,
       0,    28,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    40,     0,
       0,   194,    41,    42,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    43,    44,    45,    46,    47,    48,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,     0,    80,     0,    81,
       0,    82,     2,     3,     4,   204,     0,     6,     7,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    14,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    24,     0,    26,     0,    28,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    40,     0,     0,     0,    41,    42,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
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
      40,     0,     0,     0,    41,    42,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   271,   272,   273,   274,   133,
     134,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   135,     0,     0,     0,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,   136,   137,   138,   139,    71,
      72,    73,    74,    75,    76,    77,    78,    79,     0,    80,
       0,    81,     0,    82,     2,     3,     4,   128,     0,     6,
       7,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      14,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    24,     0,    26,     0,    28,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    40,     0,     0,     0,
      41,    42,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   -81,   -81,   -81,   -81,   133,   134,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   135,
       0,     0,     0,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,   136,   137,   138,   139,    71,    72,    73,    74,    75,
      76,    77,    78,    79,     0,    80,     0,    81,     0,    82,
       2,     3,     4,   645,     0,     6,     7,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    14,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    24,     0,    26,     0,    28,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    40,     0,     0,     0,    41,    42,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   129,   130,   131,
     132,   133,   134,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   135,     0,     0,     0,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,   136,   137,   138,
     139,    71,    72,    73,    74,    75,    76,    77,    78,    79,
       0,    80,     0,    81,     0,    82,     2,     3,     4,   128,
       0,     6,     7,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    14,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    24,     0,    26,
       0,    28,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    40,     0,
       0,     0,    41,    42,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    43,    44,    45,    46,    47,    48,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,     0,    80,     0,    81,
       0,    82,     2,     3,     4,   128,     0,     6,     7,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    14,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    24,     0,    26,     0,    28,     0,     0,
       0,     0,     0,     0,     2,     3,     4,   128,     0,     6,
       7,     0,     0,     0,    40,     0,     0,     0,    41,    42,
      14,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    24,     0,    26,     0,    28,
       0,     0,     0,     0,     0,     0,   378,     0,     0,   -81,
     -81,   -81,   -81,    47,    48,     0,    40,     0,     0,     0,
      41,    42,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,     0,    80,     0,    81,     0,    82,     0,     0,
       0,   301,     0,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,   304,     0,     0,     0,    71,    72,    73,    74,    75,
      76,    77,    78,    79,     0,    80,     0,    81,     0,    82,
     226,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     227,     0,     0,     0,   305,   306,   307,   308,   309,   310,
     311,   312,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   322,   323,   324,   325,   326,   327,   328,   329,   330,
     331,   332,   333,   334,   335,   336,   337,   338,   339,   340,
     341,   301,     0,   342,   343,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   304,     0,   233,   234,   235,   236,   237,   238,   239,
     240,   241,   242,   243,   244,   245,   246,   247,   248,   249,
       0,     0,   301,   250,     0,   394,   395,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   304,     0,   305,   306,   307,   308,   309,   310,
     311,   312,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   322,   323,   324,   325,   326,   327,   328,   329,   330,
     331,   332,   333,   334,   335,   336,   337,   338,   339,   340,
     341,     0,     0,   342,   343,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,   301,     0,   342,   343,   418,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   304,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   226,     0,     0,   695,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   227,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,   301,     0,   342,   343,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   -81,
     -81,     0,   304,     0,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,   245,
     246,   247,   248,   249,   301,     0,   705,   250,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,     0,     0,   342,   343,     0,   305,   306,   307,
     308,   309,   310,   311,   312,   313,   314,   315,   316,   317,
     318,   319,   320,   321,   322,   323,   324,   325,   326,   327,
     328,   329,   330,   331,   332,   333,   334,   335,   336,   337,
     338,   339,   340,   341,   420,   421,   342,   343,   301,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   304,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   617,   618,     0,     0,
     301,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     304,   305,   306,   307,   308,   309,   310,   311,   312,   313,
     314,   315,   316,   317,   318,   319,   320,   321,   322,   323,
     324,   325,   326,   327,   328,   329,   330,   331,   332,   333,
     334,   335,   336,   337,   338,   339,   340,   341,     0,     0,
     342,   343,     0,   305,   306,   307,   308,   309,   310,   311,
     312,   313,   314,   315,   316,   317,   318,   319,   320,   321,
     322,   323,   324,   325,   326,   327,   328,   329,   330,   331,
     332,   333,   334,   335,   336,   337,   338,   339,   340,   341,
     638,   639,   342,   343,   301,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   304,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   648,   649,     0,     0,   301,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   304,   305,   306,   307,
     308,   309,   310,   311,   312,   313,   314,   315,   316,   317,
     318,   319,   320,   321,   322,   323,   324,   325,   326,   327,
     328,   329,   330,   331,   332,   333,   334,   335,   336,   337,
     338,   339,   340,   341,     0,     0,   342,   343,     0,   305,
     306,   307,   308,   309,   310,   311,   312,   313,   314,   315,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,   339,   340,   341,   650,   651,   342,   343,
     301,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     304,     0,     0,     0,     0,   226,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   652,   653,
       0,     0,     0,     0,     0,   227,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   305,   306,   307,   308,   309,   310,   311,
     312,   313,   314,   315,   316,   317,   318,   319,   320,   321,
     322,   323,   324,   325,   326,   327,   328,   329,   330,   331,
     332,   333,   334,   335,   336,   337,   338,   339,   340,   341,
     228,   229,   342,   343,     0,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   226,     0,     0,   250,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   654,   655,     0,   227,     0,     0,     0,     0,
     226,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   619,   620,     0,
     227,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     228,   229,     0,     0,     0,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   228,   229,     0,   250,     0,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,   242,   243,   244,   245,   246,   247,   248,   249,
     301,     0,     0,   250,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   621,   622,     0,
     304,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   301,   623,   624,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   304,     0,   305,   306,   307,   308,   309,   310,   311,
     312,   313,   314,   315,   316,   317,   318,   319,   320,   321,
     322,   323,   324,   325,   326,   327,   328,   329,   330,   331,
     332,   333,   334,   335,   336,   337,   338,   339,   340,   341,
       0,     0,   342,   343,   305,   306,   307,   308,   309,   310,
     311,   312,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   322,   323,   324,   325,   326,   327,   328,   329,   330,
     331,   332,   333,   334,   335,   336,   337,   338,   339,   340,
     341,   301,   589,   342,   343,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   304,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   301,   599,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   304,     0,   305,   306,   307,   308,   309,   310,
     311,   312,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   322,   323,   324,   325,   326,   327,   328,   329,   330,
     331,   332,   333,   334,   335,   336,   337,   338,   339,   340,
     341,     0,     0,   342,   343,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,   301,   685,   342,   343,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   304,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   301,   726,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   304,     0,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,     0,     0,   342,   343,   305,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     339,   340,   341,   301,   761,   342,   343,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   304,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   301,   766,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   304,     0,   305,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     339,   340,   341,     0,     0,   342,   343,   305,   306,   307,
     308,   309,   310,   311,   312,   313,   314,   315,   316,   317,
     318,   319,   320,   321,   322,   323,   324,   325,   326,   327,
     328,   329,   330,   331,   332,   333,   334,   335,   336,   337,
     338,   339,   340,   341,   301,   768,   342,   343,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   304,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   301,   771,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   304,     0,   305,   306,   307,
     308,   309,   310,   311,   312,   313,   314,   315,   316,   317,
     318,   319,   320,   321,   322,   323,   324,   325,   326,   327,
     328,   329,   330,   331,   332,   333,   334,   335,   336,   337,
     338,   339,   340,   341,     0,     0,   342,   343,   305,   306,
     307,   308,   309,   310,   311,   312,   313,   314,   315,   316,
     317,   318,   319,   320,   321,   322,   323,   324,   325,   326,
     327,   328,   329,   330,   331,   332,   333,   334,   335,   336,
     337,   338,   339,   340,   341,   301,   772,   342,   343,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   304,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   301,   773,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   304,     0,   305,   306,
     307,   308,   309,   310,   311,   312,   313,   314,   315,   316,
     317,   318,   319,   320,   321,   322,   323,   324,   325,   326,
     327,   328,   329,   330,   331,   332,   333,   334,   335,   336,
     337,   338,   339,   340,   341,     0,     0,   342,   343,   305,
     306,   307,   308,   309,   310,   311,   312,   313,   314,   315,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,   339,   340,   341,   301,   774,   342,   343,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   304,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   301,   794,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   304,     0,   305,
     306,   307,   308,   309,   310,   311,   312,   313,   314,   315,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,   339,   340,   341,     0,     0,   342,   343,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   226,   796,   342,
     343,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   227,     0,     0,
       0,     0,   226,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   797,
       0,     0,   227,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   228,   229,     0,     0,     0,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   228,   229,     0,
     250,     0,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,   244,   245,   246,   247,
     248,   249,   226,   537,     0,   250,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   762,
       0,     0,   227,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   763,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   228,   229,     0,
       0,     0,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,   244,   245,   246,   247,
     248,   249,     0,     0,     0,   250,     0,     0,     0,   538,
     539,   540,   541,   542,   543,   544,   545,   546,   547,   548,
     549,   550,   551,   552,   553,   554,   555,   556,   557,   558,
     559,   560,   561,   562,   563,   564,   565,   566,   567,   568,
       0,   569,   301,     0,   764,   302,   303,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   304,     0,     0,   301,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   304,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,   301,     0,   342,   343,     0,     0,   325,   326,
     327,   328,   329,   330,   331,   332,   333,   334,   335,   336,
     337,   338,   304,   340,   341,   226,     0,   342,   343,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   227,     0,   358,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,   301,     0,   342,   343,     0,     0,   -81,   -81,
     -81,   -81,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   304,   248,   249,   226,     0,     0,   250,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   227,     0,     0,     0,     0,
       0,     0,   470,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,   301,     0,   342,   343,     0,     0,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   304,   248,   249,     0,     0,     0,   250,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   721,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,   301,     0,   342,   343,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   304,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   792,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,   301,     0,   342,   343,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   304,     0,     0,   802,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,   301,     0,   342,   343,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   304,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   301,     0,     0,     0,   803,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   304,     0,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,     0,     0,   342,   343,   305,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     339,   340,   341,   301,     0,   342,   343,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   304,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   301,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   304,     0,     0,     0,     0,     0,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     339,   340,   341,     0,     0,   342,   343,   308,   309,   310,
     311,   312,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   322,   323,   324,   325,   326,   327,   328,   329,   330,
     331,   332,   333,   334,   335,   336,   337,   338,   339,   340,
     341,   301,     0,   342,   343,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   304,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   301,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   304,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   310,
     311,   312,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   322,   323,   324,   325,   326,   327,   328,   329,   330,
     331,   332,   333,   334,   335,   336,   337,   338,   339,   340,
     341,     0,     0,   342,   343,   311,   312,   313,   314,   315,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,   339,   340,   341,   301,     0,   342,   343,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   304,     0,     0,     0,
       0,     0,     0,   301,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   304,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   -81,   -81,   -81,   -81,   -81,
     -81,   -81,   -81,   319,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,   339,   340,   341,     0,     0,   342,   343,
     -81,   -81,   -81,   322,   323,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     339,   340,   341,     0,     0,   342,   343
};

static const yytype_int16 yycheck[] =
{
       7,    81,    35,   180,   158,   418,   168,    42,    86,   158,
      16,    40,    23,    20,    30,     6,   165,    24,     6,    62,
     692,   250,   151,     6,    31,   202,   145,    16,     6,   206,
       8,     9,   107,   162,    41,    42,   107,    42,     7,   168,
     118,     7,    16,    61,   109,   107,    16,    43,    44,    45,
      46,    47,    48,    59,   164,    16,   107,    43,    27,   107,
      16,   171,    16,     7,    55,   107,   387,    55,    59,     6,
      59,   743,    55,     7,    81,    82,   748,   106,   107,   108,
     109,   110,   111,   112,   113,    59,   151,   116,   250,    59,
       8,     9,     7,    27,   109,    16,    12,   148,    59,   276,
     148,   107,    59,    59,     6,    59,    16,   150,   285,    16,
      80,   783,    27,   148,   343,     7,   123,   124,   107,   126,
     153,   250,   129,   130,   131,   132,   133,   134,   135,   145,
     146,   209,   150,   107,   806,    27,   151,   107,    59,   150,
     173,   110,   148,   148,   110,   151,   107,   158,   159,    59,
     166,   107,    59,   107,   165,   146,   385,     8,   146,   148,
     167,   172,   151,    84,    16,     7,   110,    16,   175,   109,
     148,     6,   106,   146,   148,   352,   110,   151,   148,    16,
     107,   343,    42,    16,     6,    27,   107,   148,   148,   196,
     110,   106,   148,   110,   109,   110,     6,   107,   375,   348,
     107,   197,   198,   199,   145,   148,   147,    59,     6,   150,
      59,   151,   389,   148,   343,   148,   223,   109,   110,   148,
      55,     0,    59,   385,   150,   151,    59,   148,   148,   148,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,   245,
     246,   247,   248,   249,   145,   107,   385,    80,   107,   148,
     267,   371,   583,   148,   271,   272,   273,   274,   110,   148,
     107,   148,   593,   280,   107,   148,   386,    40,    41,   148,
     287,    44,   695,    28,    29,    30,   150,   151,    42,   148,
     297,   298,   299,   300,   301,   302,   110,   304,   305,   306,
     307,   308,   309,   310,   311,   312,   313,   314,   315,   316,
     317,   318,   319,   320,   321,   322,   323,   324,   325,   326,
     327,   328,   329,   330,   331,   332,   333,   334,   335,   336,
     337,   338,   339,   340,   341,   342,    16,   348,    61,    16,
      62,   421,    16,    16,   150,    16,   424,   152,   355,   526,
     366,   358,    16,   674,     7,   147,   150,   150,    61,     6,
     150,   368,   369,   149,    16,   150,    42,    80,    16,    61,
      16,   109,   150,    16,    27,   382,   383,    84,    80,    59,
      80,   148,    59,     7,    20,    59,    59,   394,    59,   149,
     397,   420,   150,   149,    80,    59,     6,    16,     7,   406,
     150,   408,   409,    27,   152,   152,   149,    59,    16,   416,
     149,    59,   419,    59,   110,   148,    59,    61,    27,   110,
     149,   149,    42,   150,   578,   107,     6,   107,   605,   578,
     107,   151,   609,   107,   107,    84,   107,    42,   587,    80,
      59,    16,    80,   107,   765,    98,    99,   100,   101,   102,
     103,    59,   105,   106,    61,   107,   109,   110,   465,   107,
     570,   107,   469,   470,   107,   472,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   150,    59,   109,   110,   636,   107,    98,
      99,   100,   101,   102,   103,    16,   105,   106,    16,   107,
     580,   110,    57,    84,   149,   521,     8,   523,   150,   148,
     147,   151,   661,   148,   148,   522,     6,   109,    84,   149,
       6,    80,   533,   530,   152,    42,   149,    61,   150,    57,
       8,   611,   107,     6,    21,   531,   147,   209,    59,    26,
      10,    59,   735,   636,    31,   661,    33,    13,    35,   533,
     287,   465,   585,   302,   224,   422,   699,   734,    45,    46,
     699,   594,    49,   612,    51,   572,   573,   578,   406,   280,
     406,    -1,    -1,    -1,     0,   578,   587,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    10,   596,   107,   598,    -1,   107,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    23,    -1,    -1,
      -1,    -1,    -1,    -1,    30,    -1,    -1,    -1,    -1,    -1,
      -1,   618,    -1,    -1,    -1,    -1,    -1,    43,    44,    45,
      46,    47,    48,    -1,   620,   636,   622,    -1,   624,    -1,
      -1,    -1,   639,    -1,    -1,    -1,    -1,   644,    -1,    -1,
      -1,    -1,   649,    -1,   651,    -1,   653,    -1,   655,    -1,
     661,    -1,    -1,   686,   665,    -1,    -1,    -1,   691,    -1,
      86,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   679,    -1,   690,    -1,    -1,    -1,    -1,    -1,
     760,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   696,
     697,    -1,   118,    -1,   701,   702,    -1,    -1,   705,    -1,
      -1,   708,    -1,   736,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   718,    -1,   720,   721,    -1,    -1,    -1,    -1,   145,
     146,    -1,   729,   730,   150,    -1,    -1,    -1,    -1,    -1,
     737,   738,   158,   159,   741,    -1,    -1,    -1,   745,   165,
     166,    -1,    -1,    -1,    -1,    -1,   172,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   775,
     793,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   197,   198,   199,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   209,   791,   792,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   803,    -1,   224,    -1,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,   245,
     246,   247,   248,   249,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    34,    -1,    36,
      -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,
      -1,    -1,    59,    60,    -1,    62,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   348,    90,    91,    92,    93,    94,    95,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     366,   108,    -1,    -1,    -1,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,     7,   144,    -1,   146,
      -1,   148,    -1,    -1,    -1,   152,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,   424,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      34,    -1,    36,    -1,    38,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    55,    -1,    -1,    -1,    59,    60,    -1,    -1,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,    -1,   105,   106,    -1,    -1,   109,   110,
      -1,    -1,    -1,    -1,    -1,    -1,    90,    91,    92,    93,
      94,    95,    -1,    -1,    -1,   521,    -1,   523,    -1,    -1,
      -1,    -1,    -1,    -1,   108,   531,    -1,   533,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,    -1,
     144,    -1,   146,    -1,   148,    -1,    -1,    -1,   152,    -1,
      -1,    -1,   578,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   587,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     596,    -1,   598,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   620,    -1,   622,    -1,   624,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     636,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       7,    -1,    -1,    -1,    -1,   661,    -1,    -1,    -1,   665,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     0,     1,
      27,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      12,    13,    14,    15,   690,    17,    18,    19,    20,    21,
      22,    23,    24,    -1,    26,    -1,    -1,    -1,    -1,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    -1,    -1,
      -1,    43,    -1,    45,    46,    47,    48,    49,    50,    51,
      -1,    53,    54,    55,    -1,    82,    83,    59,    60,    -1,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
      -1,    -1,    -1,   110,    -1,    -1,    -1,    -1,    90,    91,
      92,    93,    94,    95,    -1,    -1,    -1,    -1,    -1,   775,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,    -1,   144,    -1,   146,     1,   148,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    12,    13,    -1,    -1,
      -1,    17,    18,    19,    20,    -1,    22,    23,    24,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    32,    -1,    34,    -1,
      36,    37,    38,    39,    -1,    -1,    -1,    43,    -1,    -1,
      -1,    47,    48,    -1,    50,    -1,    -1,    53,    -1,    55,
      -1,    -1,    -1,    59,    60,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    90,    91,    92,    93,    94,    95,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,    -1,   144,    -1,
     146,    -1,   148,   149,     3,     4,     5,     6,    -1,     8,
       9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    34,    -1,    36,    -1,    38,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,    -1,    -1,
      59,    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    90,    91,    92,    93,    94,    95,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,
      -1,    -1,    -1,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,    -1,   144,    -1,   146,    -1,   148,
     149,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    34,    -1,    36,    -1,    38,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    55,    -1,    -1,    -1,    59,    60,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    90,    91,
      92,    93,    94,    95,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   108,    -1,    -1,    -1,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,    -1,   144,    -1,   146,    -1,   148,   149,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    34,
      -1,    36,    -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      55,    -1,    -1,    -1,    59,    60,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    90,    91,    92,    93,    94,
      95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   108,    -1,    -1,    -1,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,    -1,   144,
      -1,   146,    -1,   148,   149,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    34,    -1,    36,    -1,
      38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,    -1,
      -1,    59,    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    90,    91,    92,    93,    94,    95,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     108,    -1,    -1,    -1,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,    -1,   144,    -1,   146,    -1,
     148,   149,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    12,    13,    14,    15,    -1,    17,    18,    19,    20,
      21,    22,    23,    24,    -1,    26,    -1,    -1,    -1,    -1,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    -1,
      -1,    -1,    43,    -1,    45,    46,    47,    48,    49,    50,
      51,    -1,    53,    54,    55,    -1,    -1,    -1,    59,    60,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    90,
      91,    92,    93,    94,    95,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,    -1,   144,    -1,   146,    -1,   148,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    12,    13,    14,
      15,    -1,    17,    18,    19,    20,    21,    22,    23,    24,
      -1,    26,    -1,    -1,    -1,    -1,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    -1,    -1,    -1,    43,    -1,
      45,    46,    47,    48,    49,    50,    51,    -1,    53,    54,
      55,    -1,    -1,    -1,    59,    60,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    90,    91,    92,    93,    94,
      95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,    -1,   144,
      -1,   146,    -1,   148,     3,     4,     5,     6,    -1,     8,
       9,    -1,    -1,    12,    13,    14,    -1,    -1,    17,    18,
      19,    20,    21,    22,    23,    24,    -1,    26,    -1,    -1,
      -1,    -1,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    -1,    -1,    -1,    43,    -1,    45,    46,    47,    48,
      49,    50,    51,    -1,    53,    54,    55,    -1,    -1,    -1,
      59,    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    90,    91,    92,    93,    94,    95,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,    -1,   144,    -1,   146,    -1,   148,
       3,     4,     5,     6,    -1,     8,     9,    -1,    -1,    12,
      13,    -1,    -1,    -1,    17,    18,    19,    20,    21,    22,
      23,    24,    -1,    26,    -1,    -1,    -1,    -1,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    -1,    -1,    -1,
      43,    -1,    45,    46,    47,    48,    49,    50,    51,    -1,
      53,    54,    55,    -1,    -1,    -1,    59,    60,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    90,    91,    92,
      93,    94,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
      -1,   144,    -1,   146,    -1,   148,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    12,    13,    -1,    -1,    -1,
      17,    18,    19,    20,    -1,    22,    23,    24,    -1,    26,
      -1,    -1,    -1,    -1,    -1,    32,    -1,    34,    -1,    36,
      37,    38,    39,    -1,    -1,    -1,    43,    -1,    -1,    -1,
      47,    48,    -1,    50,    -1,    -1,    53,    -1,    55,    -1,
      -1,    -1,    59,    60,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    90,    91,    92,    93,    94,    95,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,    -1,   144,    -1,   146,
     147,   148,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    12,    13,    -1,    -1,    -1,    17,    18,    19,    20,
      -1,    22,    23,    24,    -1,    26,    -1,    -1,    -1,    -1,
      -1,    32,    -1,    34,    -1,    36,    37,    38,    39,    -1,
      -1,    -1,    43,    -1,    -1,    -1,    47,    48,    -1,    50,
      -1,    -1,    53,    -1,    55,    -1,    -1,    -1,    59,    60,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    90,
      91,    92,    93,    94,    95,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,    -1,   144,    -1,   146,   147,   148,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    12,    13,    -1,
      -1,    -1,    17,    18,    19,    20,    -1,    22,    23,    24,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,    -1,    34,
      -1,    36,    37,    38,    39,    -1,    -1,    -1,    43,    -1,
      -1,    -1,    47,    48,    -1,    50,    -1,    -1,    53,    -1,
      55,    -1,    -1,    -1,    59,    60,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    90,    91,    92,    93,    94,
      95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,    -1,   144,
      -1,   146,    -1,   148,     3,     4,     5,     6,    -1,     8,
       9,    -1,    -1,    12,    13,    -1,    -1,    -1,    17,    18,
      19,    20,    -1,    22,    23,    24,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    32,    -1,    34,    -1,    36,    37,    38,
      39,    -1,    -1,    -1,    43,    -1,    -1,    -1,    47,    48,
      -1,    50,    -1,    -1,    53,    -1,    55,    -1,    -1,    -1,
      59,    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    90,    91,    92,    93,    94,    95,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,    -1,   144,    -1,   146,    -1,   148,
       3,     4,     5,     6,    -1,     8,     9,    -1,    -1,    12,
      13,    -1,    -1,    -1,    17,    18,    19,    20,    -1,    22,
      -1,    24,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,
      -1,    34,    -1,    36,    37,    38,    -1,    -1,    -1,    -1,
      43,    -1,    -1,    -1,    47,    48,    -1,    50,    -1,    -1,
      53,    -1,    55,    -1,    -1,    -1,    59,    60,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    90,    91,    92,
      93,    94,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
      -1,   144,    -1,   146,    -1,   148,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    12,    13,    -1,    -1,    -1,
      -1,    -1,    19,    20,    -1,    -1,    -1,    24,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    32,    -1,    34,    -1,    36,
      37,    38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      47,    48,    -1,    50,    -1,    -1,    53,    -1,    55,    -1,
      -1,    -1,    59,    60,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    90,    91,    92,    93,    94,    95,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,    -1,   144,    -1,   146,
      -1,   148,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    12,    13,    -1,    -1,    -1,    -1,    -1,    19,    20,
      -1,    -1,    -1,    24,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    32,    -1,    34,    -1,    36,    37,    38,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,    50,
      -1,    -1,    53,    -1,    55,    -1,    -1,    -1,    59,    60,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    90,
      91,    92,    93,    94,    95,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,    -1,   144,    -1,   146,    -1,   148,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    12,    13,    -1,
      -1,    -1,    -1,    -1,    19,    20,    -1,    -1,    -1,    24,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,    -1,    34,
      -1,    36,    37,    38,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    47,    48,    -1,    50,    -1,    -1,    53,    -1,
      55,    -1,    -1,    -1,    59,    60,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    90,    91,    92,    93,    94,
      95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,    -1,   144,
      -1,   146,    -1,   148,     3,     4,     5,     6,    -1,     8,
       9,    -1,    -1,    12,    13,    -1,    -1,    -1,    -1,    -1,
      19,    20,    -1,    -1,    -1,    24,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    32,    -1,    34,    -1,    36,    37,    38,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    -1,
      -1,    50,    -1,    -1,    53,    -1,    55,    -1,    -1,    -1,
      59,    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    90,    91,    92,    93,    94,    95,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,    -1,   144,    -1,   146,    -1,   148,
       3,     4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    34,    -1,    36,    -1,    38,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    55,    -1,    -1,    -1,    59,    60,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    90,    91,    92,
      93,    94,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   108,    -1,   110,    -1,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
      -1,   144,    -1,   146,    -1,   148,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    34,    -1,    36,
      -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,
      -1,    -1,    59,    60,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    90,    91,    92,    93,    94,    95,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   108,    -1,   110,    -1,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,    -1,   144,    -1,   146,
      -1,   148,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    34,    -1,    36,    -1,    38,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    55,    -1,    -1,    -1,    59,    60,
      61,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    90,
      91,    92,    93,    94,    95,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,    -1,    -1,
      -1,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,    -1,   144,    -1,   146,    -1,   148,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    34,
      -1,    36,    -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      55,    -1,    -1,    -1,    59,    60,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    90,    91,    92,    93,    94,
      95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   108,    -1,    -1,    -1,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,    -1,   144,
      -1,   146,   147,   148,     3,     4,     5,     6,    -1,     8,
       9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    34,    -1,    36,    -1,    38,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,    -1,    58,
      59,    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    90,    91,    92,    93,    94,    95,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,
      -1,    -1,    -1,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,    -1,   144,    -1,   146,    -1,   148,
       3,     4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    34,    -1,    36,    -1,    38,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    55,    -1,    -1,    -1,    59,    60,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    90,    91,    92,
      93,    94,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   108,    -1,    -1,    -1,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
      -1,   144,    -1,   146,    -1,   148,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    34,    -1,    36,
      -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,
      -1,    58,    59,    60,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    90,    91,    92,    93,    94,    95,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,    -1,   144,    -1,   146,
      -1,   148,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    34,    -1,    36,    -1,    38,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    55,    -1,    -1,    -1,    59,    60,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    90,
      91,    92,    93,    94,    95,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,    -1,    -1,
      -1,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,    -1,   144,    -1,   146,    -1,   148,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    34,
      -1,    36,    -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      55,    -1,    -1,    -1,    59,    60,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    90,    91,    92,    93,    94,
      95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   108,    -1,    -1,    -1,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,    -1,   144,
      -1,   146,    -1,   148,     3,     4,     5,     6,    -1,     8,
       9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    34,    -1,    36,    -1,    38,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,    -1,    -1,
      59,    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    90,    91,    92,    93,    94,    95,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,
      -1,    -1,    -1,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,    -1,   144,    -1,   146,    -1,   148,
       3,     4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    34,    -1,    36,    -1,    38,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    55,    -1,    -1,    -1,    59,    60,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    90,    91,    92,
      93,    94,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   108,    -1,    -1,    -1,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
      -1,   144,    -1,   146,    -1,   148,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    34,    -1,    36,
      -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,
      -1,    -1,    59,    60,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    90,    91,    92,    93,    94,    95,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,    -1,   144,    -1,   146,
      -1,   148,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    34,    -1,    36,    -1,    38,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,     6,    -1,     8,
       9,    -1,    -1,    -1,    55,    -1,    -1,    -1,    59,    60,
      19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    34,    -1,    36,    -1,    38,
      -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    90,
      91,    92,    93,    94,    95,    -1,    55,    -1,    -1,    -1,
      59,    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,    -1,   144,    -1,   146,    -1,   148,    -1,    -1,
      -1,     7,    -1,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,    27,    -1,    -1,    -1,   134,   135,   136,   137,   138,
     139,   140,   141,   142,    -1,   144,    -1,   146,    -1,   148,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      27,    -1,    -1,    -1,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,     7,    -1,   109,   110,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    27,    -1,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
      -1,    -1,     7,   110,    -1,   151,   152,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    27,    -1,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,    -1,    -1,   109,   110,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,     7,    -1,   109,   110,   152,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     7,    -1,    -1,   152,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    27,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,     7,    -1,   109,   110,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,
      83,    -1,    27,    -1,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,     7,    -1,   151,   110,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    27,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,    -1,    -1,   109,   110,    -1,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   149,   150,   109,   110,     7,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   149,   150,    -1,    -1,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      27,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,    -1,    -1,
     109,   110,    -1,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     149,   150,   109,   110,     7,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   149,   150,    -1,    -1,     7,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    27,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,    -1,    -1,   109,   110,    -1,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   149,   150,   109,   110,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      27,    -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   149,   150,
      -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
      82,    83,   109,   110,    -1,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,     7,    -1,    -1,   110,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   149,   150,    -1,    27,    -1,    -1,    -1,    -1,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   149,   150,    -1,
      27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      82,    83,    -1,    -1,    -1,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,    82,    83,    -1,   110,    -1,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
       7,    -1,    -1,   110,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   149,   150,    -1,
      27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     7,   149,   150,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    27,    -1,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
      -1,    -1,   109,   110,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,     7,   149,   109,   110,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     7,   149,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    27,    -1,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,    -1,    -1,   109,   110,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,     7,   149,   109,   110,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     7,   149,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    27,    -1,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,    -1,    -1,   109,   110,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,     7,   149,   109,   110,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     7,   149,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    27,    -1,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,    -1,    -1,   109,   110,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,     7,   149,   109,   110,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     7,   149,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    27,    -1,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,    -1,    -1,   109,   110,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,     7,   149,   109,   110,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     7,   149,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,    -1,    -1,   109,   110,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,     7,   149,   109,   110,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,   149,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,    -1,    -1,   109,   110,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,     7,   149,   109,
     110,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,
      -1,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   149,
      -1,    -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    82,    83,    -1,    -1,    -1,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,    82,    83,    -1,
     110,    -1,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,     7,     6,    -1,   110,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   149,
      -1,    -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   149,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    83,    -1,
      -1,    -1,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,    -1,    -1,    -1,   110,    -1,    -1,    -1,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
      -1,   144,     7,    -1,   149,    10,    11,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    27,    -1,    -1,     7,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,     7,    -1,   109,   110,    -1,    -1,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,    27,   105,   106,     7,    -1,   109,   110,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    27,    -1,    52,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,     7,    -1,   109,   110,    -1,    -1,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,    27,   105,   106,     7,    -1,    -1,   110,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,
      -1,    -1,    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,     7,    -1,   109,   110,    -1,    -1,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,    27,   105,   106,    -1,    -1,    -1,   110,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,     7,    -1,   109,   110,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,     7,    -1,   109,   110,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    27,    -1,    -1,    30,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,     7,    -1,   109,   110,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     7,    -1,    -1,    -1,    52,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    27,    -1,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,    -1,    -1,   109,   110,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,     7,    -1,   109,   110,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    27,    -1,    -1,    -1,    -1,    -1,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,    -1,    -1,   109,   110,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,     7,    -1,   109,   110,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,    -1,    -1,   109,   110,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,     7,    -1,   109,   110,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,
      -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    27,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,    -1,    -1,   109,   110,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,    -1,    -1,   109,   110
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     4,     5,     6,     8,     9,    12,    13,
      14,    15,    17,    18,    19,    20,    21,    22,    23,    24,
      26,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      43,    45,    46,    47,    48,    49,    50,    51,    53,    54,
      55,    59,    60,    90,    91,    92,    93,    94,    95,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
     144,   146,   148,   154,   155,   156,   157,   159,   160,   162,
     163,   168,   169,   170,   171,   173,   174,   175,   184,   192,
     193,   195,   201,   203,   204,   205,   208,   209,   210,   213,
     221,   222,   229,   232,   234,   235,   236,   239,   145,   107,
     107,   107,    16,    59,    80,   107,   148,   207,     6,    90,
      91,    92,    93,    94,    95,   108,   130,   131,   132,   133,
     202,   203,   206,   160,    43,   194,   194,   107,     6,    12,
      59,   110,   202,     6,   192,   202,     8,   107,   146,    42,
      48,   195,   110,   202,     6,   148,    42,   148,   110,   243,
     245,     6,   148,     6,   107,   148,   207,    61,   202,   230,
     231,    62,   152,   202,   223,   225,   226,   227,   228,   201,
     201,   201,   201,   201,    58,   201,   148,   148,   148,   148,
     147,   199,   200,   202,     6,   202,   211,   212,     0,   145,
     158,    15,   159,    21,    26,    31,    33,    35,    45,    46,
      49,    51,   161,    80,   109,   151,     7,    27,    82,    83,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     110,   107,   207,   107,   207,   107,   207,   107,   207,   107,
     207,   107,   207,   107,   207,   107,   207,   148,   107,   207,
     159,    90,    91,    92,    93,   196,   197,   198,   202,   202,
     148,     6,   149,   202,   237,   238,    16,    59,   107,   202,
     202,   202,   202,   202,    58,   202,   202,   148,   148,   148,
     148,     7,    10,    11,    27,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   109,   110,    48,   195,    48,   195,   148,   178,
       6,   189,   190,   191,   192,   205,   243,   151,    52,   147,
     169,   184,   192,   240,   242,   192,    42,     6,   148,   172,
     205,   107,   178,   184,   185,   186,   195,   202,    45,   204,
     205,   244,    40,    41,    44,   110,   107,   178,   187,   188,
     192,   243,   202,   107,   151,   152,    61,   150,   181,   152,
     199,   202,   224,    62,   150,    62,   152,   182,   150,   151,
     181,   202,   201,   201,   201,   147,   150,   181,   152,   151,
     149,   150,   150,   181,   157,   159,   202,   193,     6,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     201,   201,   201,   244,   107,   107,   107,   107,   107,   107,
     107,   107,   202,   107,    61,   150,   181,   149,   237,   151,
      57,   149,   150,   181,   196,   202,   202,   202,   202,   202,
     206,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     244,    42,   148,    42,     1,   179,   180,   183,   184,   192,
      80,    84,    61,   150,   181,   202,    80,     6,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   144,
     246,   202,    84,    80,   151,   145,   147,   150,   241,   195,
     148,   202,   202,   178,    20,   176,   149,   150,   181,   149,
     202,   202,   244,   178,   176,   149,   150,   181,    80,   149,
     202,     6,    55,   214,   215,   216,   217,   218,   219,   220,
     202,   152,   152,   227,   228,   202,   202,   149,   150,   149,
     150,   149,   150,   149,   150,   202,   214,   202,   107,   207,
     149,   199,     6,   212,   149,   159,   148,   243,   149,   150,
     198,   149,   202,   202,   148,     6,   202,    61,   149,   150,
     149,   150,   149,   150,   149,   150,   195,   202,   195,   149,
     149,   150,   181,   202,   201,    42,   191,     6,    55,    59,
     146,   164,   165,   107,   178,   233,   202,   202,     6,    84,
     147,   169,   242,   149,   199,   149,   176,   151,   243,   184,
      42,   176,   243,   192,   192,   152,    80,    80,    61,   150,
     181,    57,    84,   150,   181,   151,   199,   224,   151,   202,
     201,   201,   201,   147,   107,   149,   179,   202,   148,   202,
     151,    57,   202,   202,   202,   202,   149,   183,   192,   148,
     148,     8,   166,     6,   167,   109,   176,    28,    29,    30,
     202,    84,   149,   243,     6,    80,   177,   195,   243,   177,
     214,   202,   202,   217,   218,   202,   202,     6,    55,   202,
     152,   149,   149,   149,   149,   149,   149,   202,   149,   202,
     202,   149,   149,   149,   149,    42,   202,   202,    61,   150,
     150,   181,   165,   243,   202,   202,   202,   177,   202,   177,
      61,    57,    57,   176,   149,   195,   149,   149,     8,     6,
     147,   177,    30,    52,   202,   202,   243,   202,   177
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   153,   154,   155,   155,   156,   156,   156,   156,   156,
     156,   157,   157,   158,   158,   159,   159,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   162,   163,   163,   163,   163,
     163,   163,   164,   164,   165,   165,   165,   166,   166,   166,
     167,   167,   167,   168,   169,   169,   170,   170,   170,   171,
     171,   172,   172,   172,   172,   173,   173,   173,   173,   174,
     174,   175,   175,   176,   176,   177,   177,   178,   178,   178,
     179,   179,   180,   180,   181,   181,   182,   182,   183,   183,
     184,   185,   186,   186,   187,   188,   188,   189,   190,   190,
     191,   191,   192,   192,   192,   192,   193,   193,   193,   193,
     193,   193,   193,   193,   193,   193,   193,   193,   194,   194,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     196,   197,   197,   198,   198,   198,   198,   198,   199,   200,
     200,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     201,   201,   201,   201,   201,   201,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   203,   203,
     204,   204,   204,   204,   204,   204,   204,   204,   204,   204,
     204,   204,   204,   204,   204,   204,   204,   204,   204,   204,
     204,   204,   204,   204,   204,   204,   204,   204,   204,   204,
     204,   204,   204,   204,   204,   204,   204,   204,   204,   204,
     204,   204,   204,   204,   204,   204,   204,   204,   204,   204,
     204,   204,   204,   204,   204,   204,   204,   204,   204,   204,
     205,   205,   206,   206,   207,   207,   207,   207,   208,   208,
     209,   209,   210,   211,   211,   212,   213,   214,   215,   216,
     216,   216,   216,   216,   216,   217,   217,   218,   218,   219,
     220,   220,   220,   220,   221,   221,   222,   222,   222,   223,
     223,   223,   224,   224,   225,   226,   226,   226,   227,   228,
     228,   228,   229,   229,   230,   231,   231,   231,   231,   232,
     232,   233,   233,   234,   234,   234,   234,   234,   234,   234,
     234,   234,   234,   234,   234,   234,   234,   234,   234,   234,
     234,   234,   234,   234,   234,   234,   234,   234,   234,   234,
     234,   235,   235,   236,   236,   236,   236,   236,   236,   236,
     237,   238,   238,   238,   238,   238,   238,   239,   239,   239,
     240,   240,   240,   240,   241,   241,   242,   242,   243,   243,
     244,   244,   244,   245,   245,   246,   246,   246,   246,   246,
     246,   246,   246,   246,   246,   246,   246,   246,   246,   246,
     246,   246,   246,   246,   246,   246,   246,   246,   246,   246,
     246,   246,   246,   246,   246,   246,   246,   246,   246
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     2,     3,     4,     2,
       3,     1,     2,     0,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     3,     3,     5,
       7,     5,     1,     3,     4,     4,     4,     0,     1,     3,
       0,     1,     3,     3,     2,     4,     3,     4,     4,     2,
       4,     1,     3,     4,     3,     6,     6,     7,     7,     8,
       9,     3,     5,     0,     3,     0,     2,     0,     3,     3,
       0,     2,     1,     3,     0,     1,     0,     1,     1,     1,
       4,     2,     1,     3,     2,     1,     3,     2,     1,     3,
       1,     3,     1,     6,     3,     3,     1,     2,     3,     3,
       3,     6,     4,     5,     5,     8,     2,     1,     0,     1,
       1,     1,     1,     1,     1,     4,     4,     1,     1,     1,
       2,     1,     3,     1,     1,     1,     1,     1,     2,     1,
       3,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     2,     2,     2,     2,     2,     2,     2,     6,     6,
       6,     6,     4,     4,     4,     4,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     2,     2,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     2,     2,     2,     2,     2,     2,     2,     6,     6,
       6,     6,     4,     4,     4,     4,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     2,     2,     2,     1,     1,
       3,     4,     4,     5,     1,     2,     2,     3,     1,     2,
       2,     3,     4,     1,     2,     1,     2,     1,     2,     1,
       2,     1,     2,     1,     2,     2,     3,     1,     2,     2,
       3,     1,     2,     2,     3,     1,     2,     2,     3,     1,
       2,     2,     3,     1,     2,     2,     3,     1,     1,     2,
       2,     3,     1,     2,     2,     3,     1,     2,     2,     3,
       1,     2,     2,     3,     3,     1,     4,     2,     2,     3,
       4,     5,     4,     1,     3,     3,     5,     1,     2,     1,
       1,     3,     3,     3,     5,     3,     5,     3,     3,     2,
       1,     1,     3,     3,     2,     3,     2,     3,     3,     2,
       3,     5,     1,     3,     2,     1,     2,     3,     2,     1,
       3,     3,     7,     5,     2,     1,     3,     3,     5,     8,
       6,     0,     5,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     6,     4,     3,     4,     1,     4,     7,     5,     8,
       2,     1,     3,     3,     3,     5,     5,     5,     6,     7,
       1,     1,     3,     3,     1,     1,     1,     3,     0,     1,
       1,     1,     1,     2,     3,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


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


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

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

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


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

  YYLOCATION_PRINT (yyo, yylocationp);
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
    YYNOMEM;
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
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
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
        } else {
          (yyval.item) = nullptr;
        }
      }
    break;

  case 37: /* vardecl_item: ti_expr_and_id "=" expr  */
      { 
        if ((yyvsp[-2].vardeclexpr)) {
          (yyvsp[-2].vardeclexpr)->e((yyvsp[0].expression));
          (yyval.item) = VarDeclI::a((yyloc),(yyvsp[-2].vardeclexpr));
        } else {
          (yyval.item) = nullptr;
        }
      }
    break;

  case 38: /* vardecl_item: "enum" "identifier" annotations  */
      {
        TypeInst* ti = new TypeInst((yyloc),Type::parsetint());
        ti->setIsEnum(true);
        VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[-1].sValue));
        if ((yyvsp[-1].sValue) && (yyvsp[0].expressions1d)) {
          Expression::addAnnotations(vd, *(yyvsp[0].expressions1d));
        }
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
        } else {
          (yyval.item) = nullptr;
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
        if ((yyvsp[-5].sValue) && (yyvsp[-4].expressions1d)) {
          Expression::addAnnotations(vd, *(yyvsp[-4].expressions1d));
        }
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
      { if ((yyvsp[0].expression)) {
          (yyval.item) = new AssignI((yyloc),(yyvsp[-2].sValue),(yyvsp[0].expression));
        } else {
          (yyval.item) = nullptr;
        }
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

  case 60: /* output_item: "output" "::" output_annotation expr  */
      { (yyval.item) = new OutputI((yyloc),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-1].expression)) {
          (yyval.item)->cast<OutputI>()->ann().add(Call::a((yyloc), ASTString("mzn_output_section"), {(yyvsp[-1].expression)}));
        }
      }
    break;

  case 61: /* output_annotation: string_expr  */
      { (yyval.expression) = (yyvsp[0].expression); }
    break;

  case 62: /* output_annotation: "identifier" '(' ')'  */
      { (yyval.expression) = Call::a((yyloc), (yyvsp[-2].sValue), std::vector<Expression*>()); free((yyvsp[-2].sValue)); }
    break;

  case 63: /* output_annotation: "identifier" '(' expr_list ')'  */
      { (yyval.expression) = Call::a((yyloc), (yyvsp[-3].sValue), *(yyvsp[-1].expressions1d)); free((yyvsp[-3].sValue)); }
    break;

  case 64: /* output_annotation: '(' expr ')'  */
      { (yyval.expression) = (yyvsp[-1].expression); }
    break;

  case 65: /* predicate_item: "predicate" "identifier" params ann_param annotations operation_item_tail  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[-3].vardeclexprs) && (yyvsp[-2].vardeclexpr)) (yyvsp[-3].vardeclexprs)->push_back((yyvsp[-2].vardeclexpr));
        if ((yyvsp[-3].vardeclexprs)) {
          (yyval.item) = new FunctionI((yyloc),ASTString((yyvsp[-4].sValue)),new TypeInst((yyloc),
                             Type::varbool()),*(yyvsp[-3].vardeclexprs),(yyvsp[0].expression),pp->isSTDLib,(yyvsp[-2].vardeclexpr) != nullptr);
        } else {
          (yyval.item) = nullptr;
        }
        if ((yyval.item) && (yyvsp[-1].expressions1d)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expressions1d));
        free((yyvsp[-4].sValue));
        delete (yyvsp[-3].vardeclexprs);
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 66: /* predicate_item: "test" "identifier" params ann_param annotations operation_item_tail  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[-3].vardeclexprs) && (yyvsp[-2].vardeclexpr)) (yyvsp[-3].vardeclexprs)->push_back((yyvsp[-2].vardeclexpr));
        if ((yyvsp[-3].vardeclexprs)) {
          (yyval.item) = new FunctionI((yyloc),ASTString((yyvsp[-4].sValue)),new TypeInst((yyloc),
                             Type::parbool()),*(yyvsp[-3].vardeclexprs),(yyvsp[0].expression),pp->isSTDLib,(yyvsp[-2].vardeclexpr) != nullptr);
        } else {
         (yyval.item) = nullptr;
       }
        if ((yyval.item) && (yyvsp[-1].expressions1d)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expressions1d));
        free((yyvsp[-4].sValue));
        delete (yyvsp[-3].vardeclexprs);
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 67: /* predicate_item: "predicate" "identifier" "^-1" params ann_param annotations operation_item_tail  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[-3].vardeclexprs) && (yyvsp[-2].vardeclexpr)) (yyvsp[-3].vardeclexprs)->push_back((yyvsp[-2].vardeclexpr));
        if ((yyvsp[-3].vardeclexprs)) { (yyval.item) = new FunctionI((yyloc),ASTString(std::string((yyvsp[-5].sValue))+"⁻¹"),new TypeInst((yyloc),
                                     Type::varbool()),*(yyvsp[-3].vardeclexprs),(yyvsp[0].expression),pp->isSTDLib,(yyvsp[-2].vardeclexpr) != nullptr);
        } else {
          (yyval.item) = nullptr;
        }
        if ((yyval.item) && (yyvsp[-1].expressions1d)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expressions1d));
        free((yyvsp[-5].sValue));
        delete (yyvsp[-3].vardeclexprs);
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 68: /* predicate_item: "test" "identifier" "^-1" params ann_param annotations operation_item_tail  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[-3].vardeclexprs) && (yyvsp[-2].vardeclexpr)) (yyvsp[-3].vardeclexprs)->push_back((yyvsp[-2].vardeclexpr));
        if ((yyvsp[-3].vardeclexprs)) {
          (yyval.item) = new FunctionI((yyloc),ASTString(std::string((yyvsp[-5].sValue))+"⁻¹"),new TypeInst((yyloc),
                             Type::parbool()),*(yyvsp[-3].vardeclexprs),(yyvsp[0].expression),pp->isSTDLib,(yyvsp[-2].vardeclexpr) != nullptr);
        } else {
          (yyval.item) = nullptr;
        }
        if ((yyval.item) && (yyvsp[-1].expressions1d)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expressions1d));
        free((yyvsp[-5].sValue));
        delete (yyvsp[-3].vardeclexprs);
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 69: /* function_item: "function" ti_expr ':' id_or_quoted_op params ann_param annotations operation_item_tail  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[-3].vardeclexprs) && (yyvsp[-2].vardeclexpr)) (yyvsp[-3].vardeclexprs)->push_back((yyvsp[-2].vardeclexpr));
        if ((yyvsp[-6].tiexpr) && Expression::type((yyvsp[-6].tiexpr)).any() && (yyvsp[-6].tiexpr)->domain() == nullptr) {
          // This is an any type, not allowed without a right hand side
          yyerror(&(yylsp[-6]), parm, "return type cannot have `any' type-inst without type-inst variable");
        }
        if ((yyvsp[-3].vardeclexprs))  {
          (yyval.item) = new FunctionI((yyloc),ASTString((yyvsp[-4].sValue)),(yyvsp[-6].tiexpr),*(yyvsp[-3].vardeclexprs),(yyvsp[0].expression),pp->isSTDLib,(yyvsp[-2].vardeclexpr) != nullptr);
        } else {
          (yyval.item) = nullptr;
        }
        if ((yyval.item) && (yyvsp[-1].expressions1d)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expressions1d));
        free((yyvsp[-4].sValue));
        delete (yyvsp[-3].vardeclexprs);
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 70: /* function_item: ti_expr ':' "identifier" '(' params_list ')' ann_param annotations operation_item_tail  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[-8].tiexpr) && Expression::type((yyvsp[-8].tiexpr)).any() && (yyvsp[-8].tiexpr)->domain() == nullptr) {
          // This is an any type, not allowed without a right hand side
          yyerror(&(yylsp[-8]), parm, "return type cannot have `any' type-inst without type-inst variable");
        }
        if ((yyvsp[-4].vardeclexprs) && (yyvsp[-2].vardeclexpr)) (yyvsp[-4].vardeclexprs)->push_back((yyvsp[-2].vardeclexpr));
        if ((yyvsp[-4].vardeclexprs)) {
          (yyval.item) = new FunctionI((yyloc),ASTString((yyvsp[-6].sValue)),(yyvsp[-8].tiexpr),*(yyvsp[-4].vardeclexprs),(yyvsp[0].expression),pp->isSTDLib,(yyvsp[-2].vardeclexpr) != nullptr);
        } else {
          (yyval.item) = nullptr;
        }
        if ((yyval.item) && (yyvsp[-1].expressions1d)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expressions1d));
        free((yyvsp[-6].sValue));
        delete (yyvsp[-4].vardeclexprs);
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 71: /* annotation_item: "annotation" "identifier" params  */
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

  case 72: /* annotation_item: "annotation" "identifier" params "=" expr  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        TypeInst* ti=new TypeInst((yylsp[-4]),Type::ann());
        if ((yyvsp[-2].vardeclexprs)) {
          (yyval.item) = new FunctionI((yyloc),ASTString((yyvsp[-3].sValue)),ti,*(yyvsp[-2].vardeclexprs),(yyvsp[0].expression),pp->isSTDLib);
        } else {
          (yyval.item) = nullptr;
        }
        delete (yyvsp[-2].vardeclexprs);
      }
    break;

  case 73: /* ann_param: %empty  */
      { (yyval.vardeclexpr)=nullptr; }
    break;

  case 74: /* ann_param: "ann" ':' "identifier"  */
      { if ((yyvsp[0].sValue)) {
          auto* ident = new Id((yylsp[0]), (yyvsp[0].sValue), nullptr);
          auto* ti = new TypeInst((yyloc),Type::ann(1));
          (yyval.vardeclexpr) = new VarDecl((yyloc), ti, ident);
          (yyval.vardeclexpr)->toplevel(false);
        } else {
          (yyval.vardeclexpr) = nullptr;
        }
      }
    break;

  case 75: /* operation_item_tail: %empty  */
      { (yyval.expression)=nullptr; }
    break;

  case 76: /* operation_item_tail: "=" expr  */
      { (yyval.expression)=(yyvsp[0].expression); }
    break;

  case 77: /* params: %empty  */
      { (yyval.vardeclexprs)=new vector<VarDecl*>(); }
    break;

  case 78: /* params: '(' params_list ')'  */
      { (yyval.vardeclexprs)=(yyvsp[-1].vardeclexprs); }
    break;

  case 79: /* params: '(' error ')'  */
      { (yyval.vardeclexprs)=new vector<VarDecl*>(); }
    break;

  case 80: /* params_list: %empty  */
      { (yyval.vardeclexprs)=new vector<VarDecl*>(); }
    break;

  case 81: /* params_list: params_list_head comma_or_none  */
      { (yyval.vardeclexprs)=(yyvsp[-1].vardeclexprs); }
    break;

  case 82: /* params_list_head: ti_expr_and_id_or_anon  */
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

  case 83: /* params_list_head: params_list_head ',' ti_expr_and_id_or_anon  */
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

  case 88: /* ti_expr_and_id_or_anon: ti_expr_and_id  */
      { (yyval.vardeclexpr)=(yyvsp[0].vardeclexpr); }
    break;

  case 89: /* ti_expr_and_id_or_anon: ti_expr  */
      { if ((yyvsp[0].tiexpr)) {
          (yyval.vardeclexpr)=new VarDecl((yyloc), (yyvsp[0].tiexpr), "");
        } else {
          (yyval.vardeclexpr) = nullptr;
        }
      }
    break;

  case 90: /* ti_expr_and_id: ti_expr ':' "identifier" annotations  */
      { if ((yyvsp[-3].tiexpr) && (yyvsp[-1].sValue)) {
          Id* ident = new Id((yylsp[-1]), (yyvsp[-1].sValue), nullptr);
          (yyval.vardeclexpr) = new VarDecl((yyloc), (yyvsp[-3].tiexpr), ident);
          if ((yyvsp[0].expressions1d)) Expression::ann((yyval.vardeclexpr)).add(*(yyvsp[0].expressions1d));
        } else {
          (yyval.vardeclexpr) = nullptr;
        }
        free((yyvsp[-1].sValue));
        delete (yyvsp[0].expressions1d);
      }
    break;

  case 91: /* ti_expr_and_id_list: ti_expr_and_id_list_head comma_or_none  */
      { (yyval.vardeclexprs)=(yyvsp[-1].vardeclexprs); }
    break;

  case 92: /* ti_expr_and_id_list_head: ti_expr_and_id  */
      { (yyval.vardeclexprs)=new vector<VarDecl*>(); (yyval.vardeclexprs)->push_back((yyvsp[0].vardeclexpr)); }
    break;

  case 93: /* ti_expr_and_id_list_head: ti_expr_and_id_list_head ',' ti_expr_and_id  */
      { (yyval.vardeclexprs)=(yyvsp[-2].vardeclexprs); if ((yyvsp[-2].vardeclexprs) && (yyvsp[0].vardeclexpr)) (yyvsp[-2].vardeclexprs)->push_back((yyvsp[0].vardeclexpr)); }
    break;

  case 94: /* ti_expr_list: ti_expr_list_head comma_or_none  */
      { (yyval.tiexprs)=(yyvsp[-1].tiexprs); }
    break;

  case 95: /* ti_expr_list_head: ti_expr  */
      { (yyval.tiexprs)=new vector<TypeInst*>(); (yyval.tiexprs)->push_back((yyvsp[0].tiexpr)); }
    break;

  case 96: /* ti_expr_list_head: ti_expr_list_head ',' ti_expr  */
      { (yyval.tiexprs)=(yyvsp[-2].tiexprs); if ((yyvsp[-2].tiexprs) && (yyvsp[0].tiexpr)) (yyvsp[-2].tiexprs)->push_back((yyvsp[0].tiexpr)); }
    break;

  case 97: /* array_index_list: array_index_list_head comma_or_none  */
      { (yyval.tiexprs)=(yyvsp[-1].tiexprs); }
    break;

  case 98: /* array_index_list_head: array_index_entry  */
      { (yyval.tiexprs)=new vector<TypeInst*>(); (yyval.tiexprs)->push_back((yyvsp[0].tiexpr)); }
    break;

  case 99: /* array_index_list_head: array_index_list_head ',' array_index_entry  */
      { (yyval.tiexprs)=(yyvsp[-2].tiexprs); if ((yyvsp[-2].tiexprs) && (yyvsp[0].tiexpr)) (yyvsp[-2].tiexprs)->push_back((yyvsp[0].tiexpr)); }
    break;

  case 100: /* array_index_entry: ti_expr  */
      { (yyval.tiexpr)=(yyvsp[0].tiexpr); }
    break;

  case 101: /* array_index_entry: "identifier" "in" set_expr  */
      {
        if ((yyvsp[0].expression) != nullptr) {
          Id* binder = new Id((yylsp[-2]), (yyvsp[-2].sValue), nullptr);
          // Wrap the set expression in a TypeInst so the marker carries the
          // same shape as it did when this production accepted a ti_expr.
          TypeInst* range_ti = new TypeInst((yylsp[0]), Type(), (yyvsp[0].expression));
          // Index-binder marker: 2-tuple (range_ti, binder).  Distinguishable
          // from a set-cardinality marker (which has a TypeInst at index 1)
          // by carrying the TypeInst at index 0 and the binder Id at index 1.
          ArrayLit* marker =
            ArrayLit::constructTuple((yyloc), std::vector<Expression*>({range_ti, binder}));
          (yyval.tiexpr) = new TypeInst((yyloc), Type(), marker);
        } else {
          (yyval.tiexpr) = nullptr;
        }
        free((yyvsp[-2].sValue));
      }
    break;

  case 103: /* ti_expr: "array" "[" array_index_list "]" "of" ti_expr  */
      {
        if ((yyvsp[0].tiexpr) != nullptr && (yyvsp[0].tiexpr)->isarray()) {
          (yyval.tiexpr) = new TypeInst((yyloc), Type::tuple(), (yyvsp[0].tiexpr));
        } else {
          (yyval.tiexpr) = (yyvsp[0].tiexpr);
        }
        if ((yyval.tiexpr) != nullptr && (yyvsp[-3].tiexprs) != nullptr) {
          (yyval.tiexpr)->setRanges(*(yyvsp[-3].tiexprs));
        }
        delete (yyvsp[-3].tiexprs);
      }
    break;

  case 104: /* ti_expr: "list" "of" ti_expr  */
      {
        // `list of T` is sugar for `array[1..infinity] of T`: a 1-based array of
        // arbitrary length. The 1..infinity index set is what marks a `list`; it is
        // accepted only in declarations and is replaced by the actual (finite) index
        // set when a value is bound to it (see check_index_sets), while call arguments
        // are coerced 1-based with array1d during type-checking.
        if ((yyvsp[0].tiexpr) != nullptr && (yyvsp[0].tiexpr)->isarray()) {
          (yyval.tiexpr) = new TypeInst((yyloc), Type::tuple(), (yyvsp[0].tiexpr));
        } else {
          (yyval.tiexpr) = (yyvsp[0].tiexpr);
        }
        std::vector<TypeInst*> ti(1);
        ti[0] = new TypeInst((yyloc), Type(),
                             new BinOp((yyloc), IntLit::a(1), BOT_DOTDOT,
                                       IntLit::a(IntVal::infinity())));
        if ((yyval.tiexpr) != nullptr){
          (yyval.tiexpr)->setRanges(ti);
        }
      }
    break;

  case 105: /* ti_expr: ti_expr "++" base_ti_expr  */
      {
        (yyval.tiexpr) = (yyvsp[-2].tiexpr);
        if ((yyval.tiexpr) != nullptr) {
          Type tt = Expression::type((yyval.tiexpr));
          tt.dim(0);
          TypeInst* lhs = new TypeInst((yyloc), tt, (yyvsp[-2].tiexpr)->domain());
          BinOp* bop = new BinOp((yyloc), lhs, BOT_PLUSPLUS, (yyvsp[0].tiexpr));
          bop->type(tt);
          (yyval.tiexpr)->domain(bop);
        }
      }
    break;

  case 106: /* base_ti_expr: base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
      }
    break;

  case 107: /* base_ti_expr: "opt" base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr) != nullptr) {
          Type tt = Expression::type((yyval.tiexpr));
          tt.ot(Type::OT_OPTIONAL);
          tt.otExplicit(true);
          (yyval.tiexpr)->type(tt);
        }
      }
    break;

  case 108: /* base_ti_expr: "par" opt_opt base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr) != nullptr) {
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

  case 109: /* base_ti_expr: "var" opt_opt base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr) != nullptr) {
          Type tt = Expression::type((yyval.tiexpr));
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

  case 110: /* base_ti_expr: "set" "of" base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr) != nullptr) {
          Type tt = Expression::type((yyval.tiexpr));
          tt.st(Type::ST_SET);
          (yyval.tiexpr)->type(tt);
        }
      }
    break;

  case 111: /* base_ti_expr: "set" '(' expr ')' "of" base_ti_expr_tail  */
      { if ((yyvsp[0].tiexpr) != nullptr && (yyvsp[-3].expression) != nullptr) {
          Type tt = Expression::type((yyvsp[0].tiexpr));
          tt.st(Type::ST_SET);
          ArrayLit* card_marker =
            ArrayLit::constructTuple((yyloc), std::vector<Expression*>({(yyvsp[-3].expression), (yyvsp[0].tiexpr)}));
          (yyval.tiexpr) = new TypeInst((yyloc), tt, card_marker);
          (yyval.tiexpr)->setIsEnum((yyvsp[0].tiexpr)->isEnum());
        } else {
          (yyval.tiexpr) = nullptr;
        }
      }
    break;

  case 112: /* base_ti_expr: "opt" "set" "of" base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr) != nullptr) {
          Type tt = Expression::type((yyval.tiexpr));
          tt.st(Type::ST_SET);
          tt.ot(Type::OT_OPTIONAL);
          tt.otExplicit(true);
          (yyval.tiexpr)->type(tt);
        }
      }
    break;

  case 113: /* base_ti_expr: "par" opt_opt "set" "of" base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr) != nullptr) {
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

  case 114: /* base_ti_expr: "var" opt_opt "set" "of" base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr) != nullptr) {
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

  case 115: /* base_ti_expr: "var" opt_opt "set" '(' expr ')' "of" base_ti_expr_tail  */
      { if ((yyvsp[0].tiexpr) != nullptr && (yyvsp[-3].expression) != nullptr) {
          Type tt = Expression::type((yyvsp[0].tiexpr));
          tt.ti(Type::TI_VAR);
          tt.tiExplicit(true);
          tt.st(Type::ST_SET);
          if ((yyvsp[-6].bValue)) {
            tt.ot(Type::OT_OPTIONAL);
            tt.otExplicit(true);
          }
          ArrayLit* card_marker =
            ArrayLit::constructTuple((yyloc), std::vector<Expression*>({(yyvsp[-3].expression), (yyvsp[0].tiexpr)}));
          (yyval.tiexpr) = new TypeInst((yyloc), tt, card_marker);
          (yyval.tiexpr)->setIsEnum((yyvsp[0].tiexpr)->isEnum());
        } else {
          (yyval.tiexpr) = nullptr;
        }
      }
    break;

  case 116: /* base_ti_expr: "any" "type-inst identifier"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::mkAny(),new TIId((yyloc), (yyvsp[0].sValue)));
        free((yyvsp[0].sValue));
      }
    break;

  case 117: /* base_ti_expr: "any"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::mkAny()); }
    break;

  case 118: /* opt_opt: %empty  */
      { (yyval.bValue) = false; }
    break;

  case 119: /* opt_opt: "opt"  */
      { (yyval.bValue) = true; }
    break;

  case 120: /* base_ti_expr_tail: "int"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::parint()); }
    break;

  case 121: /* base_ti_expr_tail: "bool"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::parbool()); }
    break;

  case 122: /* base_ti_expr_tail: "float"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::parfloat()); }
    break;

  case 123: /* base_ti_expr_tail: "string"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::parstring()); }
    break;

  case 124: /* base_ti_expr_tail: "ann"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::ann()); }
    break;

  case 125: /* base_ti_expr_tail: "tuple" '(' ti_expr_list ')'  */
      {
        std::vector<Expression*> tmp((yyvsp[-1].tiexprs)->begin(), (yyvsp[-1].tiexprs)->end());
        ArrayLit* al = ArrayLit::constructTuple((yyloc), tmp);
        (yyval.tiexpr) = new TypeInst((yyloc), Type::tuple(), al);
        delete (yyvsp[-1].tiexprs);
      }
    break;

  case 126: /* base_ti_expr_tail: "record" '(' ti_expr_and_id_list ')'  */
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

  case 127: /* base_ti_expr_tail: set_expr  */
      { if ((yyvsp[0].expression)) {
          (yyval.tiexpr) = new TypeInst((yyloc),Type(),(yyvsp[0].expression));
        } else {
          (yyval.tiexpr) = nullptr;
        }
      }
    break;

  case 128: /* base_ti_expr_tail: "type-inst identifier"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::top(),
                         new TIId((yyloc), (yyvsp[0].sValue)));
        free((yyvsp[0].sValue));
      }
    break;

  case 129: /* base_ti_expr_tail: "type-inst enum identifier"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::parint(),
                          new TIId((yyloc), (yyvsp[0].sValue)));
        free((yyvsp[0].sValue));
      }
    break;

  case 131: /* array_access_expr_list_head: array_access_expr  */
      { (yyval.expressions1d)=new std::vector<MiniZinc::Expression*>; (yyval.expressions1d)->push_back((yyvsp[0].expression)); }
    break;

  case 132: /* array_access_expr_list_head: array_access_expr_list_head ',' array_access_expr  */
      { (yyval.expressions1d)=(yyvsp[-2].expressions1d); if ((yyval.expressions1d) && (yyvsp[0].expression)) (yyval.expressions1d)->push_back((yyvsp[0].expression)); }
    break;

  case 133: /* array_access_expr: expr  */
      { (yyval.expression) = (yyvsp[0].expression); }
    break;

  case 134: /* array_access_expr: ".."  */
      { (yyval.expression)=new SetLit((yyloc), IntSetVal::a(-IntVal::infinity(),IntVal::infinity())); }
    break;

  case 135: /* array_access_expr: "..<"  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'..<'"), {}); }
    break;

  case 136: /* array_access_expr: "<.."  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..'"), {}); }
    break;

  case 137: /* array_access_expr: "<..<"  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..<'"), {}); }
    break;

  case 139: /* expr_list_head: expr  */
      { (yyval.expressions1d)=new std::vector<MiniZinc::Expression*>; (yyval.expressions1d)->push_back((yyvsp[0].expression)); }
    break;

  case 140: /* expr_list_head: expr_list_head ',' expr  */
      { (yyval.expressions1d)=(yyvsp[-2].expressions1d); if ((yyval.expressions1d) && (yyvsp[0].expression)) (yyval.expressions1d)->push_back((yyvsp[0].expression)); }
    break;

  case 142: /* set_expr: set_expr "::" annotation_expr  */
      { if ((yyvsp[-2].expression) && (yyvsp[0].expression)) Expression::addAnnotation((yyvsp[-2].expression), (yyvsp[0].expression)); (yyval.expression)=(yyvsp[-2].expression); }
    break;

  case 143: /* set_expr: set_expr "union" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_UNION, (yyvsp[0].expression)); }
    break;

  case 144: /* set_expr: set_expr "diff" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIFF, (yyvsp[0].expression)); }
    break;

  case 145: /* set_expr: set_expr "symdiff" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SYMDIFF, (yyvsp[0].expression)); }
    break;

  case 146: /* set_expr: set_expr ".." set_expr  */
      { if ((yyvsp[-2].expression)==nullptr || (yyvsp[0].expression)==nullptr) {
          (yyval.expression) = nullptr;
        } else if (Expression::isa<IntLit>((yyvsp[-2].expression)) && Expression::isa<IntLit>((yyvsp[0].expression))) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a(IntLit::v(Expression::cast<IntLit>((yyvsp[-2].expression))),IntLit::v(Expression::cast<IntLit>((yyvsp[0].expression)))));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DOTDOT, (yyvsp[0].expression));
        }
      }
    break;

  case 147: /* set_expr: set_expr "..<" set_expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'..<'"), {(yyvsp[-2].expression), (yyvsp[0].expression)}); }
    break;

  case 148: /* set_expr: set_expr "<.." set_expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..'"), {(yyvsp[-2].expression), (yyvsp[0].expression)}); }
    break;

  case 149: /* set_expr: set_expr "<..<" set_expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..<'"), {(yyvsp[-2].expression), (yyvsp[0].expression)}); }
    break;

  case 150: /* set_expr: set_expr ".."  */
      { (yyval.expression)=Call::a((yyloc), ASTString("..o"), {(yyvsp[-1].expression)}); }
    break;

  case 151: /* set_expr: set_expr "..<"  */
      { (yyval.expression)=Call::a((yyloc), ASTString("..<o"), {(yyvsp[-1].expression)}); }
    break;

  case 152: /* set_expr: set_expr "<.."  */
      { (yyval.expression)=Call::a((yyloc), ASTString("<..o"), {(yyvsp[-1].expression)}); }
    break;

  case 153: /* set_expr: set_expr "<..<"  */
      { (yyval.expression)=Call::a((yyloc), ASTString("<..<o"), {(yyvsp[-1].expression)}); }
    break;

  case 154: /* set_expr: ".." set_expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("o.."), {(yyvsp[0].expression)}); }
    break;

  case 155: /* set_expr: "..<" set_expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("o..<"), {(yyvsp[0].expression)}); }
    break;

  case 156: /* set_expr: "<.." set_expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("o<.."), {(yyvsp[0].expression)}); }
    break;

  case 157: /* set_expr: "<..<" set_expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("o<..<"), {(yyvsp[0].expression)}); }
    break;

  case 158: /* set_expr: "'..<'" '(' set_expr ',' set_expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'..<'"), {(yyvsp[-3].expression), (yyvsp[-1].expression)}); }
    break;

  case 159: /* set_expr: "'<..'" '(' set_expr ',' set_expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..'"), {(yyvsp[-3].expression), (yyvsp[-1].expression)}); }
    break;

  case 160: /* set_expr: "'<..<'" '(' set_expr ',' set_expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..<'"), {(yyvsp[-3].expression), (yyvsp[-1].expression)}); }
    break;

  case 161: /* set_expr: "'..'" '(' expr ',' expr ')'  */
      { if ((yyvsp[-3].expression)==nullptr || (yyvsp[-1].expression)==nullptr) {
          (yyval.expression) = nullptr;
        } else if (Expression::isa<IntLit>((yyvsp[-3].expression)) && Expression::isa<IntLit>((yyvsp[-1].expression))) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a(IntLit::v(Expression::cast<IntLit>((yyvsp[-3].expression))),IntLit::v(Expression::cast<IntLit>((yyvsp[-1].expression)))));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-3].expression), BOT_DOTDOT, (yyvsp[-1].expression));
        }
      }
    break;

  case 162: /* set_expr: "'..<'" '(' set_expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'..<'"), {(yyvsp[-1].expression)}); }
    break;

  case 163: /* set_expr: "'<..'" '(' set_expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..'"), {(yyvsp[-1].expression)}); }
    break;

  case 164: /* set_expr: "'<..<'" '(' set_expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..<'"), {(yyvsp[-1].expression)}); }
    break;

  case 165: /* set_expr: "'..'" '(' expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'..'"), {(yyvsp[-1].expression)}); }
    break;

  case 166: /* set_expr: set_expr "intersect" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_INTERSECT, (yyvsp[0].expression)); }
    break;

  case 167: /* set_expr: set_expr "+" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_PLUS, (yyvsp[0].expression)); }
    break;

  case 168: /* set_expr: set_expr "-" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MINUS, (yyvsp[0].expression)); }
    break;

  case 169: /* set_expr: set_expr "*" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MULT, (yyvsp[0].expression)); }
    break;

  case 170: /* set_expr: set_expr "/" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIV, (yyvsp[0].expression)); }
    break;

  case 171: /* set_expr: set_expr "div" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IDIV, (yyvsp[0].expression)); }
    break;

  case 172: /* set_expr: set_expr "mod" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MOD, (yyvsp[0].expression)); }
    break;

  case 173: /* set_expr: set_expr "^" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_POW, (yyvsp[0].expression)); }
    break;

  case 174: /* set_expr: set_expr "~+" set_expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~+"), args);
      }
    break;

  case 175: /* set_expr: set_expr "~-" set_expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~-"), args);
      }
    break;

  case 176: /* set_expr: set_expr "~*" set_expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~*"), args);
      }
    break;

  case 177: /* set_expr: set_expr "~/" set_expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~/"), args);
      }
    break;

  case 178: /* set_expr: set_expr "~div" set_expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~div"), args);
      }
    break;

  case 179: /* set_expr: set_expr "~=" set_expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~="), args);
      }
    break;

  case 180: /* set_expr: set_expr "~!=" set_expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~!="), args);
      }
    break;

  case 181: /* set_expr: set_expr "default" set_expr  */
      {
        vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("default"), args);
      }
    break;

  case 182: /* set_expr: set_expr "quoted identifier" set_expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), (yyvsp[-1].sValue), args);
        free((yyvsp[-1].sValue));
      }
    break;

  case 183: /* set_expr: "+" set_expr  */
      { (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[0].expression)); }
    break;

  case 184: /* set_expr: "-" set_expr  */
      { if ((yyvsp[0].expression) && Expression::isa<IntLit>((yyvsp[0].expression))) {
          (yyval.expression) = IntLit::a(-IntLit::v(Expression::cast<IntLit>((yyvsp[0].expression))));
        } else if ((yyvsp[0].expression) && Expression::isa<FloatLit>((yyvsp[0].expression))) {
          (yyval.expression) = FloatLit::a(-FloatLit::v(Expression::cast<FloatLit>((yyvsp[0].expression))));
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_MINUS, (yyvsp[0].expression));
        }
      }
    break;

  case 185: /* set_expr: "-" "9223372036854775808"  */
      { (yyval.expression) = IntLit::a((-9223372036854775807ll)-1); }
    break;

  case 187: /* expr: expr "::" annotation_expr  */
      { if ((yyvsp[-2].expression) && (yyvsp[0].expression)) Expression::addAnnotation((yyvsp[-2].expression), (yyvsp[0].expression)); (yyval.expression)=(yyvsp[-2].expression); }
    break;

  case 188: /* expr: expr "<->" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_EQUIV, (yyvsp[0].expression)); }
    break;

  case 189: /* expr: expr "->" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IMPL, (yyvsp[0].expression)); }
    break;

  case 190: /* expr: expr "<-" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_RIMPL, (yyvsp[0].expression)); }
    break;

  case 191: /* expr: expr "\\/" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_OR, (yyvsp[0].expression)); }
    break;

  case 192: /* expr: expr "xor" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_XOR, (yyvsp[0].expression)); }
    break;

  case 193: /* expr: expr "/\\" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_AND, (yyvsp[0].expression)); }
    break;

  case 194: /* expr: expr "<" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_LE, (yyvsp[0].expression)); }
    break;

  case 195: /* expr: expr ">" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_GR, (yyvsp[0].expression)); }
    break;

  case 196: /* expr: expr "<=" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_LQ, (yyvsp[0].expression)); }
    break;

  case 197: /* expr: expr ">=" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_GQ, (yyvsp[0].expression)); }
    break;

  case 198: /* expr: expr "=" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_EQ, (yyvsp[0].expression)); }
    break;

  case 199: /* expr: expr "!=" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_NQ, (yyvsp[0].expression)); }
    break;

  case 200: /* expr: expr "in" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IN, (yyvsp[0].expression)); }
    break;

  case 201: /* expr: expr "subset" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SUBSET, (yyvsp[0].expression)); }
    break;

  case 202: /* expr: expr "superset" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SUPERSET, (yyvsp[0].expression)); }
    break;

  case 203: /* expr: expr "union" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_UNION, (yyvsp[0].expression)); }
    break;

  case 204: /* expr: expr "diff" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIFF, (yyvsp[0].expression)); }
    break;

  case 205: /* expr: expr "symdiff" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SYMDIFF, (yyvsp[0].expression)); }
    break;

  case 206: /* expr: expr ".." expr  */
      { if ((yyvsp[-2].expression)==nullptr || (yyvsp[0].expression)==nullptr) {
          (yyval.expression) = nullptr;
        } else if (Expression::isa<IntLit>((yyvsp[-2].expression)) && Expression::isa<IntLit>((yyvsp[0].expression))) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a(IntLit::v(Expression::cast<IntLit>((yyvsp[-2].expression))),IntLit::v(Expression::cast<IntLit>((yyvsp[0].expression)))));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DOTDOT, (yyvsp[0].expression));
        }
      }
    break;

  case 207: /* expr: expr "..<" expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'..<'"), {(yyvsp[-2].expression), (yyvsp[0].expression)}); }
    break;

  case 208: /* expr: expr "<.." expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..'"), {(yyvsp[-2].expression), (yyvsp[0].expression)}); }
    break;

  case 209: /* expr: expr "<..<" expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..<'"), {(yyvsp[-2].expression), (yyvsp[0].expression)}); }
    break;

  case 210: /* expr: expr ".."  */
      { (yyval.expression)=Call::a((yyloc), ASTString("..o"), {(yyvsp[-1].expression)}); }
    break;

  case 211: /* expr: expr "..<"  */
      { (yyval.expression)=Call::a((yyloc), ASTString("..<o"), {(yyvsp[-1].expression)}); }
    break;

  case 212: /* expr: expr "<.."  */
      { (yyval.expression)=Call::a((yyloc), ASTString("<..o"), {(yyvsp[-1].expression)}); }
    break;

  case 213: /* expr: expr "<..<"  */
      { (yyval.expression)=Call::a((yyloc), ASTString("<..<o"), {(yyvsp[-1].expression)}); }
    break;

  case 214: /* expr: ".." expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("o.."), {(yyvsp[0].expression)}); }
    break;

  case 215: /* expr: "..<" expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("o..<"), {(yyvsp[0].expression)}); }
    break;

  case 216: /* expr: "<.." expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("o<.."), {(yyvsp[0].expression)}); }
    break;

  case 217: /* expr: "<..<" expr  */
      { (yyval.expression)=Call::a((yyloc), ASTString("o<..<"), {(yyvsp[0].expression)}); }
    break;

  case 218: /* expr: "'..<'" '(' expr ',' expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'..<'"), {(yyvsp[-3].expression), (yyvsp[-1].expression)}); }
    break;

  case 219: /* expr: "'<..'" '(' expr ',' expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..'"), {(yyvsp[-3].expression), (yyvsp[-1].expression)}); }
    break;

  case 220: /* expr: "'<..<'" '(' expr ',' expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..<'"), {(yyvsp[-3].expression), (yyvsp[-1].expression)}); }
    break;

  case 221: /* expr: "'..'" '(' expr ',' expr ')'  */
      { if ((yyvsp[-3].expression)==nullptr || (yyvsp[-1].expression)==nullptr) {
          (yyval.expression) = nullptr;
        } else if (Expression::isa<IntLit>((yyvsp[-3].expression)) && Expression::isa<IntLit>((yyvsp[-1].expression))) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a(IntLit::v(Expression::cast<IntLit>((yyvsp[-3].expression))),IntLit::v(Expression::cast<IntLit>((yyvsp[-1].expression)))));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-3].expression), BOT_DOTDOT, (yyvsp[-1].expression));
        }
      }
    break;

  case 222: /* expr: "'..<'" '(' expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'..<'"), {(yyvsp[-1].expression)}); }
    break;

  case 223: /* expr: "'<..'" '(' expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..'"), {(yyvsp[-1].expression)}); }
    break;

  case 224: /* expr: "'<..<'" '(' expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'<..<'"), {(yyvsp[-1].expression)}); }
    break;

  case 225: /* expr: "'..'" '(' expr ')'  */
      { (yyval.expression)=Call::a((yyloc), ASTString("'..'"), {(yyvsp[-1].expression)}); }
    break;

  case 226: /* expr: expr "intersect" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_INTERSECT, (yyvsp[0].expression)); }
    break;

  case 227: /* expr: expr "++" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_PLUSPLUS, (yyvsp[0].expression)); }
    break;

  case 228: /* expr: expr "+" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_PLUS, (yyvsp[0].expression)); }
    break;

  case 229: /* expr: expr "-" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MINUS, (yyvsp[0].expression)); }
    break;

  case 230: /* expr: expr "*" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MULT, (yyvsp[0].expression)); }
    break;

  case 231: /* expr: expr "/" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIV, (yyvsp[0].expression)); }
    break;

  case 232: /* expr: expr "div" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IDIV, (yyvsp[0].expression)); }
    break;

  case 233: /* expr: expr "mod" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MOD, (yyvsp[0].expression)); }
    break;

  case 234: /* expr: expr "^" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_POW, (yyvsp[0].expression)); }
    break;

  case 235: /* expr: expr "~+" expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~+"), args);
      }
    break;

  case 236: /* expr: expr "~-" expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~-"), args);
      }
    break;

  case 237: /* expr: expr "~*" expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~*"), args);
      }
    break;

  case 238: /* expr: expr "~/" expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~/"), args);
      }
    break;

  case 239: /* expr: expr "~div" expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~div"), args);
      }
    break;

  case 240: /* expr: expr "~=" expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~="), args);
      }
    break;

  case 241: /* expr: expr "~!=" expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("~!="), args);
      }
    break;

  case 242: /* expr: expr "default" expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), ASTString("default"), args);
      }
    break;

  case 243: /* expr: expr "quoted identifier" expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=Call::a((yyloc), (yyvsp[-1].sValue), args);
        free((yyvsp[-1].sValue));
      }
    break;

  case 244: /* expr: "not" expr  */
      { (yyval.expression)=new UnOp((yyloc), UOT_NOT, (yyvsp[0].expression)); }
    break;

  case 245: /* expr: "+" expr  */
      { if (((yyvsp[0].expression) && Expression::isa<IntLit>((yyvsp[0].expression))) || ((yyvsp[0].expression) && Expression::isa<FloatLit>((yyvsp[0].expression)))) {
          (yyval.expression) = (yyvsp[0].expression);
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[0].expression));
        }
      }
    break;

  case 246: /* expr: "-" expr  */
      { if ((yyvsp[0].expression) && Expression::isa<IntLit>((yyvsp[0].expression))) {
          (yyval.expression) = IntLit::a(-IntLit::v(Expression::cast<IntLit>((yyvsp[0].expression))));
        } else if ((yyvsp[0].expression) && Expression::isa<FloatLit>((yyvsp[0].expression))) {
          (yyval.expression) = FloatLit::a(-FloatLit::v(Expression::cast<FloatLit>((yyvsp[0].expression))));
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_MINUS, (yyvsp[0].expression));
        }
      }
    break;

  case 247: /* expr: "-" "9223372036854775808"  */
      { (yyval.expression) = IntLit::a((-9223372036854775807ll)-1); }
    break;

  case 248: /* expr_atom_head: expr_atom_head_nonstring  */
      { (yyval.expression)=(yyvsp[0].expression); }
    break;

  case 249: /* expr_atom_head: string_expr  */
      { (yyval.expression)=(yyvsp[0].expression); }
    break;

  case 250: /* expr_atom_head_nonstring: '(' expr ')'  */
      { (yyval.expression)=(yyvsp[-1].expression); }
    break;

  case 251: /* expr_atom_head_nonstring: '(' expr ')' access_tail  */
      { if ((yyvsp[0].expressions1d)) {
          (yyval.expression)=createAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[0].expressions1d));
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[0].expressions1d);
      }
    break;

  case 252: /* expr_atom_head_nonstring: '(' expr ')' "^-1"  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 253: /* expr_atom_head_nonstring: '(' expr ')' access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) {
          (yyval.expression)=new BinOp((yyloc),createAccess((yyloc), (yyvsp[-3].expression), *(yyvsp[-1].expressions1d)), BOT_POW, IntLit::a(-1));
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 254: /* expr_atom_head_nonstring: "identifier"  */
      { (yyval.expression)=new Id((yyloc), (yyvsp[0].sValue), nullptr); free((yyvsp[0].sValue)); }
    break;

  case 255: /* expr_atom_head_nonstring: "identifier" access_tail  */
      { if ((yyvsp[0].expressions1d)) {
          (yyval.expression)=createAccess((yyloc), new Id((yylsp[-1]),(yyvsp[-1].sValue),nullptr), *(yyvsp[0].expressions1d));
        } else {
          (yyval.expression) = nullptr;
        }
        free((yyvsp[-1].sValue)); delete (yyvsp[0].expressions1d); }
    break;

  case 256: /* expr_atom_head_nonstring: "identifier" "^-1"  */
      { (yyval.expression)=new BinOp((yyloc),new Id((yyloc), (yyvsp[-1].sValue), nullptr), BOT_POW, IntLit::a(-1)); free((yyvsp[-1].sValue)); }
    break;

  case 257: /* expr_atom_head_nonstring: "identifier" access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) {
          (yyval.expression)=new BinOp((yyloc),createAccess((yyloc), new Id((yylsp[-2]),(yyvsp[-2].sValue),nullptr), *(yyvsp[-1].expressions1d)), BOT_POW, IntLit::a(-1));
        } else {
          (yyval.expression) = nullptr;
        }
        free((yyvsp[-2].sValue)); delete (yyvsp[-1].expressions1d); }
    break;

  case 258: /* expr_atom_head_nonstring: "_"  */
      { (yyval.expression)=new AnonVar((yyloc)); }
    break;

  case 259: /* expr_atom_head_nonstring: "_" access_tail  */
      { if ((yyvsp[0].expressions1d)) {
          (yyval.expression)=createAccess((yyloc), new AnonVar((yyloc)), *(yyvsp[0].expressions1d));
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[0].expressions1d); }
    break;

  case 260: /* expr_atom_head_nonstring: "_" "^-1"  */
      { (yyval.expression)=new BinOp((yyloc),new AnonVar((yyloc)), BOT_POW, IntLit::a(-1)); }
    break;

  case 261: /* expr_atom_head_nonstring: "_" access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) {
          (yyval.expression)=new BinOp((yyloc),createAccess((yyloc), new AnonVar((yyloc)), *(yyvsp[-1].expressions1d)), BOT_POW, IntLit::a(-1));
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[-1].expressions1d); }
    break;

  case 262: /* expr_atom_head_nonstring: "_" '(' expr ')'  */
      { (yyval.expression) = Call::a((yyloc), Constants::constants().ids.anon_enum_set, {(yyvsp[-1].expression)}); }
    break;

  case 263: /* expr_atom_head_nonstring: "bool literal"  */
      { (yyval.expression)=Constants::constants().boollit(((yyvsp[0].iValue)!=0)); }
    break;

  case 264: /* expr_atom_head_nonstring: "bool literal" "^-1"  */
      { (yyval.expression)=new BinOp((yyloc),Constants::constants().boollit(((yyvsp[-1].iValue)!=0)), BOT_POW, IntLit::a(-1)); }
    break;

  case 265: /* expr_atom_head_nonstring: "integer literal"  */
      { (yyval.expression)=IntLit::a((yyvsp[0].iValue)); }
    break;

  case 266: /* expr_atom_head_nonstring: "integer literal" "^-1"  */
      { (yyval.expression)=new BinOp((yyloc),IntLit::a((yyvsp[-1].iValue)), BOT_POW, IntLit::a(-1)); }
    break;

  case 267: /* expr_atom_head_nonstring: "infinity"  */
      { (yyval.expression)=IntLit::a(IntVal::infinity()); }
    break;

  case 268: /* expr_atom_head_nonstring: "infinity" "^-1"  */
      { (yyval.expression)=new BinOp((yyloc),IntLit::a(IntVal::infinity()), BOT_POW, IntLit::a(-1)); }
    break;

  case 269: /* expr_atom_head_nonstring: "float literal"  */
      { (yyval.expression)=FloatLit::a((yyvsp[0].dValue)); }
    break;

  case 270: /* expr_atom_head_nonstring: "float literal" "^-1"  */
      { (yyval.expression)=new BinOp((yyloc),FloatLit::a((yyvsp[-1].dValue)), BOT_POW, IntLit::a(-1)); }
    break;

  case 271: /* expr_atom_head_nonstring: "<>"  */
      { (yyval.expression)=Constants::constants().absent; }
    break;

  case 272: /* expr_atom_head_nonstring: "<>" "^-1"  */
      { (yyval.expression)=Constants::constants().absent; }
    break;

  case 274: /* expr_atom_head_nonstring: set_literal access_tail  */
      { if ((yyvsp[0].expressions1d)) {
          (yyval.expression)=createAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions1d));
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[0].expressions1d); }
    break;

  case 275: /* expr_atom_head_nonstring: set_literal "^-1"  */
      { (yyval.expression) = new BinOp((yyloc),(yyvsp[-1].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 276: /* expr_atom_head_nonstring: set_literal access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) {
          (yyval.expression)=new BinOp((yyloc),createAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions1d)), BOT_POW, IntLit::a(-1));
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[-1].expressions1d); }
    break;

  case 278: /* expr_atom_head_nonstring: set_comp access_tail  */
      { if ((yyvsp[0].expressions1d)) {
          (yyval.expression)=createAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions1d));
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[0].expressions1d); }
    break;

  case 279: /* expr_atom_head_nonstring: set_comp "^-1"  */
      { (yyval.expression) = new BinOp((yyloc),(yyvsp[-1].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 280: /* expr_atom_head_nonstring: set_comp access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) {
          (yyval.expression)=new BinOp((yyloc),createAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions1d)), BOT_POW, IntLit::a(-1));
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[-1].expressions1d); }
    break;

  case 282: /* expr_atom_head_nonstring: simple_array_literal access_tail  */
      { if ((yyvsp[0].expressions1d)) {
          (yyval.expression)=createAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions1d));
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[0].expressions1d); }
    break;

  case 283: /* expr_atom_head_nonstring: simple_array_literal "^-1"  */
      { (yyval.expression) = new BinOp((yyloc),(yyvsp[-1].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 284: /* expr_atom_head_nonstring: simple_array_literal access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) {
          (yyval.expression)=new BinOp((yyloc),createAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions1d)), BOT_POW, IntLit::a(-1));
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[-1].expressions1d); }
    break;

  case 286: /* expr_atom_head_nonstring: simple_array_literal_2d access_tail  */
      { if ((yyvsp[0].expressions1d)) {
          (yyval.expression)=createAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions1d));
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[0].expressions1d); }
    break;

  case 287: /* expr_atom_head_nonstring: simple_array_literal_2d "^-1"  */
      { (yyval.expression) = new BinOp((yyloc),(yyvsp[-1].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 288: /* expr_atom_head_nonstring: simple_array_literal_2d access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) {
          (yyval.expression)=new BinOp((yyloc),createAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions1d)), BOT_POW, IntLit::a(-1));
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[-1].expressions1d); }
    break;

  case 290: /* expr_atom_head_nonstring: simple_array_comp access_tail  */
      { if ((yyvsp[0].expressions1d)) {
          (yyval.expression)=createAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions1d));
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[0].expressions1d); }
    break;

  case 291: /* expr_atom_head_nonstring: simple_array_comp "^-1"  */
      { (yyval.expression) = new BinOp((yyloc),(yyvsp[-1].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 292: /* expr_atom_head_nonstring: simple_array_comp access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) {
          (yyval.expression)=new BinOp((yyloc),createAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions1d)), BOT_POW, IntLit::a(-1));
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[-1].expressions1d); }
    break;

  case 294: /* expr_atom_head_nonstring: if_then_else_expr access_tail  */
      { if ((yyvsp[0].expressions1d)) {
          (yyval.expression)=createAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions1d));
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[0].expressions1d); }
    break;

  case 295: /* expr_atom_head_nonstring: if_then_else_expr "^-1"  */
      { (yyval.expression) = new BinOp((yyloc),(yyvsp[-1].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 296: /* expr_atom_head_nonstring: if_then_else_expr access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) {
          (yyval.expression)=new BinOp((yyloc),createAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions1d)), BOT_POW, IntLit::a(-1));
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[-1].expressions1d); }
    break;

  case 299: /* expr_atom_head_nonstring: call_expr access_tail  */
      { if ((yyvsp[0].expressions1d)) {
          (yyval.expression)=createAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions1d));
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[0].expressions1d); }
    break;

  case 301: /* expr_atom_head_nonstring: call_expr access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) {
          (yyval.expression)=createAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions1d));
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[-1].expressions1d); }
    break;

  case 303: /* expr_atom_head_nonstring: tuple_literal access_tail  */
      { if ((yyvsp[0].expressions1d)) {
          (yyval.expression)=createAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions1d));
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[0].expressions1d); }
    break;

  case 304: /* expr_atom_head_nonstring: tuple_literal "^-1"  */
      { (yyval.expression) = new BinOp((yyloc),(yyvsp[-1].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 305: /* expr_atom_head_nonstring: tuple_literal access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) {
          (yyval.expression)=new BinOp((yyloc),createAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions1d)), BOT_POW, IntLit::a(-1));
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[-1].expressions1d); }
    break;

  case 307: /* expr_atom_head_nonstring: record_literal access_tail  */
      { if ((yyvsp[0].expressions1d)) {
          (yyval.expression)=createAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions1d));
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[0].expressions1d); }
    break;

  case 308: /* expr_atom_head_nonstring: record_literal "^-1"  */
      { (yyval.expression) = new BinOp((yyloc),(yyvsp[-1].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 309: /* expr_atom_head_nonstring: record_literal access_tail "^-1"  */
      { if ((yyvsp[-1].expressions1d)) {
          (yyval.expression)=new BinOp((yyloc),createAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions1d)), BOT_POW, IntLit::a(-1));
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[-1].expressions1d); }
    break;

  case 310: /* string_expr: "string literal"  */
      { (yyval.expression)=new StringLit((yyloc), (yyvsp[0].sValue)); free((yyvsp[0].sValue)); }
    break;

  case 311: /* string_expr: "interpolated string start" string_quote_rest  */
      { (yyval.expression)=new BinOp((yyloc), new StringLit((yyloc), (yyvsp[-1].sValue)), BOT_PLUSPLUS, (yyvsp[0].expression));
        free((yyvsp[-1].sValue));
      }
    break;

  case 312: /* string_quote_rest: expr "interpolated string end"  */
      { if ((yyvsp[-1].expression)) {
          (yyval.expression)=new BinOp((yyloc), Call::a((yyloc), ASTString("format"), {(yyvsp[-1].expression)}), BOT_PLUSPLUS, new StringLit((yyloc),(yyvsp[0].sValue)));
        } else {
          (yyval.expression) = nullptr;
        }
        free((yyvsp[0].sValue));
      }
    break;

  case 313: /* string_quote_rest: expr "interpolated string middle" string_quote_rest  */
      { if ((yyvsp[-2].expression)) {
          (yyval.expression)=new BinOp((yyloc), Call::a((yyloc), ASTString("format"), {(yyvsp[-2].expression)}), BOT_PLUSPLUS,
                       new BinOp((yyloc), new StringLit((yyloc),(yyvsp[-1].sValue)), BOT_PLUSPLUS, (yyvsp[0].expression)));
        } else {
          (yyval.expression) = nullptr;
        }
        free((yyvsp[-1].sValue));
      }
    break;

  case 314: /* access_tail: "[" array_access_expr_list "]"  */
      {
        (yyval.expressions1d)=new std::vector<Expression*>();
        if ((yyvsp[-1].expressions1d)) {
          auto* al = new ArrayAccess((yyloc), nullptr, *(yyvsp[-1].expressions1d));
          (yyval.expressions1d)->push_back(al);
          delete (yyvsp[-1].expressions1d);
        }
      }
    break;

  case 315: /* access_tail: "field access"  */
      {
        (yyval.expressions1d)=new std::vector<Expression*>();
        std::string tail((yyvsp[0].sValue)); free((yyvsp[0].sValue));
        parseFieldTail((yyloc), *(yyval.expressions1d), tail);
      }
    break;

  case 316: /* access_tail: access_tail "[" array_access_expr_list "]"  */
      {
        (yyval.expressions1d)=(yyvsp[-3].expressions1d);
        if ((yyval.expressions1d) && (yyvsp[-1].expressions1d)) {
          auto* al = new ArrayAccess((yyloc), nullptr, *(yyvsp[-1].expressions1d));
          (yyval.expressions1d)->push_back(al);
          delete (yyvsp[-1].expressions1d);
        }
      }
    break;

  case 317: /* access_tail: access_tail "field access"  */
      {
        (yyval.expressions1d)=(yyvsp[-1].expressions1d);
        std::string tail((yyvsp[0].sValue)); free((yyvsp[0].sValue));
        parseFieldTail((yyloc), *(yyval.expressions1d), tail);
      }
    break;

  case 318: /* set_literal: '{' '}'  */
      { (yyval.expression) = new SetLit((yyloc), std::vector<Expression*>()); }
    break;

  case 319: /* set_literal: '{' expr_list '}'  */
      { if ((yyvsp[-1].expressions1d)) {
          (yyval.expression) = new SetLit((yyloc), *(yyvsp[-1].expressions1d));
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[-1].expressions1d); }
    break;

  case 320: /* tuple_literal: '(' expr ',' ')'  */
      {
        std::vector<Expression*> list({ (yyvsp[-2].expression) });
        (yyval.expression)=ArrayLit::constructTuple((yyloc), list);
      }
    break;

  case 321: /* tuple_literal: '(' expr ',' expr_list ')'  */
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

  case 322: /* record_literal: '(' record_field_list_head comma_or_none ')'  */
      {
        (yyval.expression) = ArrayLit::constructTuple((yyloc), *(yyvsp[-2].expressions1d));
        Expression::type((yyval.expression), Type::record());
        delete((yyvsp[-2].expressions1d));
      }
    break;

  case 323: /* record_field_list_head: record_field  */
      { (yyval.expressions1d) = new vector<Expression*>(1); (*(yyval.expressions1d))[0] = (yyvsp[0].vardeclexpr); }
    break;

  case 324: /* record_field_list_head: record_field_list_head ',' record_field  */
      { (yyval.expressions1d)=(yyvsp[-2].expressions1d); (yyval.expressions1d)->push_back((yyvsp[0].vardeclexpr)); }
    break;

  case 325: /* record_field: "identifier" ':' expr  */
      {
        (yyval.vardeclexpr) = new VarDecl((yyloc), new TypeInst((yyloc), Type()), (yyvsp[-2].sValue), (yyvsp[0].expression));
        free((yyvsp[-2].sValue));
      }
    break;

  case 326: /* set_comp: '{' expr '|' comp_tail '}'  */
      { if ((yyvsp[-1].generatorsPointer)) {
          (yyval.expression) = new Comprehension((yyloc), (yyvsp[-3].expression), *(yyvsp[-1].generatorsPointer), true);
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[-1].generatorsPointer);
      }
    break;

  case 327: /* comp_tail: generator_list  */
      { if ((yyvsp[0].generators)) {
          (yyval.generatorsPointer)=new Generators; (yyval.generatorsPointer)->g = *(yyvsp[0].generators);
        } else {
          (yyval.generatorsPointer) = nullptr;
        }
        delete (yyvsp[0].generators);
      }
    break;

  case 329: /* generator_list_head: generator  */
      { (yyval.generators)=new std::vector<Generator>; if ((yyvsp[0].generator)) (yyval.generators)->push_back(*(yyvsp[0].generator)); delete (yyvsp[0].generator); }
    break;

  case 330: /* generator_list_head: generator_eq  */
      { (yyval.generators)=new std::vector<Generator>; if ((yyvsp[0].generator)) (yyval.generators)->push_back(*(yyvsp[0].generator)); delete (yyvsp[0].generator); }
    break;

  case 331: /* generator_list_head: generator_eq "where" expr  */
      { (yyval.generators)=new std::vector<Generator>;
        if ((yyvsp[-2].generator)) (yyval.generators)->push_back(*(yyvsp[-2].generator));
        if ((yyvsp[-2].generator) && (yyvsp[0].expression)) (yyval.generators)->push_back(Generator(static_cast<int>((yyval.generators)->size()),(yyvsp[0].expression)));
        delete (yyvsp[-2].generator);
      }
    break;

  case 332: /* generator_list_head: generator_list_head ',' generator  */
      { (yyval.generators)=(yyvsp[-2].generators); if ((yyval.generators) && (yyvsp[0].generator)) (yyval.generators)->push_back(*(yyvsp[0].generator)); delete (yyvsp[0].generator); }
    break;

  case 333: /* generator_list_head: generator_list_head ',' generator_eq  */
      { (yyval.generators)=(yyvsp[-2].generators); if ((yyval.generators) && (yyvsp[0].generator)) (yyval.generators)->push_back(*(yyvsp[0].generator)); delete (yyvsp[0].generator); }
    break;

  case 334: /* generator_list_head: generator_list_head ',' generator_eq "where" expr  */
      { (yyval.generators)=(yyvsp[-4].generators);
        if ((yyval.generators) && (yyvsp[-2].generator)) (yyval.generators)->push_back(*(yyvsp[-2].generator));
        if ((yyval.generators) && (yyvsp[-2].generator) && (yyvsp[0].expression)) (yyval.generators)->push_back(Generator(static_cast<int>((yyval.generators)->size()),(yyvsp[0].expression)));
        delete (yyvsp[-2].generator);
      }
    break;

  case 335: /* generator: id_list "in" expr  */
      { if ((yyvsp[-2].strings) && (yyvsp[0].expression)) (yyval.generator)=new Generator(*(yyvsp[-2].strings),(yyvsp[0].expression),nullptr); else (yyval.generator)=nullptr; delete (yyvsp[-2].strings); }
    break;

  case 336: /* generator: id_list "in" expr "where" expr  */
      { if ((yyvsp[-4].strings) && (yyvsp[-2].expression)) (yyval.generator)=new Generator(*(yyvsp[-4].strings),(yyvsp[-2].expression),(yyvsp[0].expression)); else (yyval.generator)=nullptr; delete (yyvsp[-4].strings); }
    break;

  case 337: /* generator_eq: "identifier" "=" expr  */
      { if ((yyvsp[0].expression)) (yyval.generator)=new Generator({(yyvsp[-2].sValue)},nullptr,(yyvsp[0].expression)); else (yyval.generator)=nullptr; free((yyvsp[-2].sValue)); }
    break;

  case 338: /* generator_eq: "_" "=" expr  */
      { if ((yyvsp[0].expression)) (yyval.generator)=new Generator({std::string()},nullptr,(yyvsp[0].expression)); else (yyval.generator)=nullptr; }
    break;

  case 340: /* id_list_head: "identifier"  */
      { (yyval.strings)=new std::vector<std::string>; (yyval.strings)->push_back((yyvsp[0].sValue)); free((yyvsp[0].sValue)); }
    break;

  case 341: /* id_list_head: "_"  */
      { (yyval.strings)=new std::vector<std::string>; (yyval.strings)->push_back(""); }
    break;

  case 342: /* id_list_head: id_list_head ',' "identifier"  */
      { (yyval.strings)=(yyvsp[-2].strings); if ((yyval.strings) && (yyvsp[0].sValue)) (yyval.strings)->push_back((yyvsp[0].sValue)); free((yyvsp[0].sValue)); }
    break;

  case 343: /* id_list_head: id_list_head ',' "_"  */
      { (yyval.strings)=(yyvsp[-2].strings); if ((yyval.strings)) (yyval.strings)->push_back(""); }
    break;

  case 344: /* simple_array_literal: "[" "]"  */
      { (yyval.expression)=new ArrayLit((yyloc), std::vector<MiniZinc::Expression*>()); }
    break;

  case 345: /* simple_array_literal: "[" comp_expr_list "]"  */
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
        } else {
          (yyval.expression) = nullptr;
        }
      }
    break;

  case 346: /* simple_array_literal_2d: "[|" "|]"  */
      { (yyval.expression)=new ArrayLit((yyloc), std::vector<std::vector<Expression*> >()); }
    break;

  case 347: /* simple_array_literal_2d: "[|" simple_array_literal_2d_indexed_list "|]"  */
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
            if (i > static_cast<unsigned int>(hadHeaderRow) && (*(yyvsp[-1].indexedexpressions2d))[i].first.size() != (*(yyvsp[-1].indexedexpressions2d))[i-1].first.size()) {
              yyerror(&(yylsp[-1]), parm, "syntax error, mixing indexed and non-indexed sub-arrays in 2d array literal");
            }
            if (i >= static_cast<unsigned int>(hadHeaderRow) && !(*(yyvsp[-1].indexedexpressions2d))[i].first.empty()) {
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

  case 348: /* simple_array_literal_2d: "[|" simple_array_literal_3d_list "|]"  */
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

  case 349: /* simple_array_literal_3d_list: '|' '|'  */
      { (yyval.expressions3d)=new std::vector<std::vector<std::vector<MiniZinc::Expression*> > >;
      }
    break;

  case 350: /* simple_array_literal_3d_list: '|' simple_array_literal_2d_list '|'  */
      { (yyval.expressions3d)=new std::vector<std::vector<std::vector<MiniZinc::Expression*> > >;
        if ((yyvsp[-1].expressions2d)) (yyval.expressions3d)->push_back(*(yyvsp[-1].expressions2d));
        delete (yyvsp[-1].expressions2d);
      }
    break;

  case 351: /* simple_array_literal_3d_list: simple_array_literal_3d_list ',' '|' simple_array_literal_2d_list '|'  */
      { (yyval.expressions3d)=(yyvsp[-4].expressions3d);
        if ((yyval.expressions3d) && (yyvsp[-1].expressions2d)) (yyval.expressions3d)->push_back(*(yyvsp[-1].expressions2d));
        delete (yyvsp[-1].expressions2d);
      }
    break;

  case 352: /* simple_array_literal_2d_list: expr_list  */
      { (yyval.expressions2d)=new std::vector<std::vector<MiniZinc::Expression*> >;
        if ((yyvsp[0].expressions1d)) (yyval.expressions2d)->push_back(*(yyvsp[0].expressions1d));
        delete (yyvsp[0].expressions1d);
      }
    break;

  case 353: /* simple_array_literal_2d_list: simple_array_literal_2d_list '|' expr_list  */
      { (yyval.expressions2d)=(yyvsp[-2].expressions2d); if ((yyval.expressions2d) && (yyvsp[0].expressions1d)) (yyval.expressions2d)->push_back(*(yyvsp[0].expressions1d)); delete (yyvsp[0].expressions1d); }
    break;

  case 355: /* simple_array_literal_2d_indexed_list_head: simple_array_literal_2d_indexed_list_row  */
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

  case 356: /* simple_array_literal_2d_indexed_list_head: simple_array_literal_2d_indexed_list_row_head ':'  */
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

  case 357: /* simple_array_literal_2d_indexed_list_head: simple_array_literal_2d_indexed_list_head '|' simple_array_literal_2d_indexed_list_row  */
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

  case 359: /* simple_array_literal_2d_indexed_list_row_head: expr  */
      { (yyval.indexedexpression2d)=new std::pair<std::vector<MiniZinc::Expression*>,std::vector<MiniZinc::Expression*>>();
        (yyval.indexedexpression2d)->second.push_back((yyvsp[0].expression));
      }
    break;

  case 360: /* simple_array_literal_2d_indexed_list_row_head: simple_array_literal_2d_indexed_list_row_head ':' expr  */
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

  case 361: /* simple_array_literal_2d_indexed_list_row_head: simple_array_literal_2d_indexed_list_row_head ',' expr  */
      { (yyval.indexedexpression2d)=(yyvsp[-2].indexedexpression2d);
        if ((yyval.indexedexpression2d)) {
          if ((yyval.indexedexpression2d)->second.empty()) {
            yyerror(&(yyloc),parm,"invalid array literal, mixing indexes and values");
          }
          (yyval.indexedexpression2d)->second.push_back((yyvsp[0].expression));
        }
      }
    break;

  case 362: /* simple_array_comp: "[" expr ':' expr '|' comp_tail "]"  */
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
        } else {
          (yyval.expression) = nullptr;
        }
      }
    break;

  case 363: /* simple_array_comp: "[" expr '|' comp_tail "]"  */
      { if ((yyvsp[-1].generatorsPointer)) {
          (yyval.expression)=new Comprehension((yyloc), (yyvsp[-3].expression), *(yyvsp[-1].generatorsPointer), false);
        } else {
          (yyval.expression) = nullptr;
        }
        delete (yyvsp[-1].generatorsPointer);
      }
    break;

  case 365: /* comp_expr_list_head: expr  */
      { (yyval.indexedexpression2d)=new std::pair<std::vector<MiniZinc::Expression*>,std::vector<MiniZinc::Expression*>>;
        if ((yyvsp[0].expression)) (yyval.indexedexpression2d)->second.push_back((yyvsp[0].expression)); }
    break;

  case 366: /* comp_expr_list_head: expr ':' expr  */
      { (yyval.indexedexpression2d)=new std::pair<std::vector<MiniZinc::Expression*>,std::vector<MiniZinc::Expression*>>;
        if ((yyvsp[0].expression)) {
          (yyval.indexedexpression2d)->first.push_back((yyvsp[-2].expression));
          (yyval.indexedexpression2d)->second.push_back((yyvsp[0].expression));
        }
      }
    break;

  case 367: /* comp_expr_list_head: comp_expr_list_head ',' expr  */
      { (yyval.indexedexpression2d)=(yyvsp[-2].indexedexpression2d);
        if ((yyval.indexedexpression2d) && (yyvsp[0].expression)) {
          if ((yyval.indexedexpression2d)->first.size() > 1) {
            (yyval.indexedexpression2d) = nullptr;
            yyerror(&(yyloc),parm,"invalid array literal, mixing indexed and non-indexed values");
          } else {
            (yyval.indexedexpression2d)->second.push_back((yyvsp[0].expression));
          }
        }
      }
    break;

  case 368: /* comp_expr_list_head: comp_expr_list_head ',' expr ':' expr  */
      { (yyval.indexedexpression2d)=(yyvsp[-4].indexedexpression2d);
        if ((yyval.indexedexpression2d) && (yyvsp[-2].expression) && (yyvsp[0].expression)) {
          if ((yyval.indexedexpression2d)->first.size() != (yyval.indexedexpression2d)->second.size()) {
            (yyval.indexedexpression2d) = nullptr;
            yyerror(&(yyloc),parm,"invalid array literal, mixing indexed and non-indexed values");
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

  case 369: /* if_then_else_expr: "if" expr "then" expr elseif_list "else" expr "endif"  */
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

  case 370: /* if_then_else_expr: "if" expr "then" expr elseif_list "endif"  */
    {
      std::vector<Expression*> iexps;
      iexps.push_back((yyvsp[-4].expression));
      iexps.push_back((yyvsp[-2].expression));
      if ((yyvsp[-1].expressions1d)) {
        for (unsigned int i=0; i<(yyvsp[-1].expressions1d)->size(); i+=2) {
          iexps.push_back((*(yyvsp[-1].expressions1d))[i]);
          iexps.push_back((*(yyvsp[-1].expressions1d))[i+1]);
        }
      }
      (yyval.expression)=new ITE((yyloc), iexps, nullptr);
      delete (yyvsp[-1].expressions1d);
    }
    break;

  case 371: /* elseif_list: %empty  */
      { (yyval.expressions1d)=new std::vector<MiniZinc::Expression*>; }
    break;

  case 372: /* elseif_list: elseif_list "elseif" expr "then" expr  */
      { (yyval.expressions1d)=(yyvsp[-4].expressions1d); if ((yyval.expressions1d) && (yyvsp[-2].expression) && (yyvsp[0].expression)) { (yyval.expressions1d)->push_back((yyvsp[-2].expression)); (yyval.expressions1d)->push_back((yyvsp[0].expression)); } }
    break;

  case 373: /* quoted_op: "'<->'"  */
      { (yyval.iValue)=BOT_EQUIV; }
    break;

  case 374: /* quoted_op: "'->'"  */
      { (yyval.iValue)=BOT_IMPL; }
    break;

  case 375: /* quoted_op: "'<-'"  */
      { (yyval.iValue)=BOT_RIMPL; }
    break;

  case 376: /* quoted_op: "'\\/'"  */
      { (yyval.iValue)=BOT_OR; }
    break;

  case 377: /* quoted_op: "'xor'"  */
      { (yyval.iValue)=BOT_XOR; }
    break;

  case 378: /* quoted_op: "'/\\'"  */
      { (yyval.iValue)=BOT_AND; }
    break;

  case 379: /* quoted_op: "'<'"  */
      { (yyval.iValue)=BOT_LE; }
    break;

  case 380: /* quoted_op: "'>'"  */
      { (yyval.iValue)=BOT_GR; }
    break;

  case 381: /* quoted_op: "'<='"  */
      { (yyval.iValue)=BOT_LQ; }
    break;

  case 382: /* quoted_op: "'>='"  */
      { (yyval.iValue)=BOT_GQ; }
    break;

  case 383: /* quoted_op: "'='"  */
      { (yyval.iValue)=BOT_EQ; }
    break;

  case 384: /* quoted_op: "'!='"  */
      { (yyval.iValue)=BOT_NQ; }
    break;

  case 385: /* quoted_op: "'in'"  */
      { (yyval.iValue)=BOT_IN; }
    break;

  case 386: /* quoted_op: "'subset'"  */
      { (yyval.iValue)=BOT_SUBSET; }
    break;

  case 387: /* quoted_op: "'superset'"  */
      { (yyval.iValue)=BOT_SUPERSET; }
    break;

  case 388: /* quoted_op: "'union'"  */
      { (yyval.iValue)=BOT_UNION; }
    break;

  case 389: /* quoted_op: "'diff'"  */
      { (yyval.iValue)=BOT_DIFF; }
    break;

  case 390: /* quoted_op: "'symdiff'"  */
      { (yyval.iValue)=BOT_SYMDIFF; }
    break;

  case 391: /* quoted_op: "'+'"  */
      { (yyval.iValue)=BOT_PLUS; }
    break;

  case 392: /* quoted_op: "'-'"  */
      { (yyval.iValue)=BOT_MINUS; }
    break;

  case 393: /* quoted_op: "'*'"  */
      { (yyval.iValue)=BOT_MULT; }
    break;

  case 394: /* quoted_op: "'^'"  */
      { (yyval.iValue)=BOT_POW; }
    break;

  case 395: /* quoted_op: "'/'"  */
      { (yyval.iValue)=BOT_DIV; }
    break;

  case 396: /* quoted_op: "'div'"  */
      { (yyval.iValue)=BOT_IDIV; }
    break;

  case 397: /* quoted_op: "'mod'"  */
      { (yyval.iValue)=BOT_MOD; }
    break;

  case 398: /* quoted_op: "'intersect'"  */
      { (yyval.iValue)=BOT_INTERSECT; }
    break;

  case 399: /* quoted_op: "'++'"  */
      { (yyval.iValue)=BOT_PLUSPLUS; }
    break;

  case 400: /* quoted_op: "'not'"  */
      { (yyval.iValue)=-1; }
    break;

  case 401: /* quoted_op_call: quoted_op '(' expr ',' expr ')'  */
      { if ((yyvsp[-5].iValue)==-1) {
          (yyval.expression)=nullptr;
          yyerror(&(yylsp[-3]), parm, "syntax error, unary operator with two arguments");
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-3].expression),static_cast<BinOpType>((yyvsp[-5].iValue)),(yyvsp[-1].expression));
        }
      }
    break;

  case 402: /* quoted_op_call: quoted_op '(' expr ')'  */
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

  case 403: /* call_expr: "identifier" '(' ')'  */
      { (yyval.expression)=Call::a((yyloc), (yyvsp[-2].sValue), std::vector<Expression*>()); free((yyvsp[-2].sValue)); }
    break;

  case 404: /* call_expr: "identifier" "^-1" '(' ')'  */
      { (yyval.expression)=Call::a((yyloc), std::string((yyvsp[-3].sValue))+"⁻¹", std::vector<Expression*>()); free((yyvsp[-3].sValue)); }
    break;

  case 406: /* call_expr: "identifier" '(' comp_or_expr ')'  */
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
        } else {
          (yyval.expression) = nullptr;
        }
        free((yyvsp[-3].sValue));
        delete (yyvsp[-1].expressionPairs);
      }
    break;

  case 407: /* call_expr: "identifier" '(' comp_or_expr ')' '(' expr ')'  */
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
                      gens.push_back(Generator(static_cast<int>(gens.size()),(*(yyvsp[-4].expressionPairs))[i].second));
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
        } else {
          (yyval.expression) = nullptr;
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

  case 408: /* call_expr: "identifier" "^-1" '(' comp_or_expr ')'  */
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
        } else {
          (yyval.expression) = nullptr;
        }
        free((yyvsp[-4].sValue));
        delete (yyvsp[-1].expressionPairs);
      }
    break;

  case 409: /* call_expr: "identifier" "^-1" '(' comp_or_expr ')' '(' expr ')'  */
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
                      gens.push_back(Generator(static_cast<int>(gens.size()),(*(yyvsp[-4].expressionPairs))[i].second));
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
        } else {
          (yyval.expression) = nullptr;
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

  case 411: /* comp_or_expr_head: expr  */
      { (yyval.expressionPairs)=new vector<pair<Expression*,Expression*> >;
        if ((yyvsp[0].expression)) {
          (yyval.expressionPairs)->push_back(pair<Expression*,Expression*>((yyvsp[0].expression),nullptr));
        }
      }
    break;

  case 412: /* comp_or_expr_head: expr "where" expr  */
      { (yyval.expressionPairs)=new vector<pair<Expression*,Expression*> >;
        if ((yyvsp[-2].expression) && (yyvsp[0].expression)) {
          (yyval.expressionPairs)->push_back(pair<Expression*,Expression*>((yyvsp[-2].expression),(yyvsp[0].expression)));
        }
      }
    break;

  case 413: /* comp_or_expr_head: "identifier" ':' expr  */
      { if ((yyvsp[-2].sValue) && (yyvsp[-2].sValue)[0]=='_') {
          std::ostringstream oss;
          oss << "parameter `" << std::string((yyvsp[-2].sValue)) << "' starts with '_' ";
          oss << "and cannot be used as a named argument (pass it positionally)";
          yyerror(&(yylsp[-2]), parm, oss.str());
        }
        (yyval.expressionPairs)=new vector<pair<Expression*,Expression*> >;
        if ((yyvsp[0].expression)) {
          VarDecl* vd = new VarDecl((yyloc), new TypeInst((yyloc), Type()), (yyvsp[-2].sValue), (yyvsp[0].expression));
          (yyval.expressionPairs)->push_back(pair<Expression*,Expression*>(vd,nullptr));
        }
        free((yyvsp[-2].sValue));
      }
    break;

  case 414: /* comp_or_expr_head: comp_or_expr_head ',' expr  */
      { (yyval.expressionPairs)=(yyvsp[-2].expressionPairs); if ((yyval.expressionPairs) && (yyvsp[0].expression)) (yyval.expressionPairs)->push_back(pair<Expression*,Expression*>((yyvsp[0].expression),nullptr)); }
    break;

  case 415: /* comp_or_expr_head: comp_or_expr_head ',' expr "where" expr  */
      { (yyval.expressionPairs)=(yyvsp[-4].expressionPairs); if ((yyval.expressionPairs) && (yyvsp[-2].expression) && (yyvsp[0].expression)) (yyval.expressionPairs)->push_back(pair<Expression*,Expression*>((yyvsp[-2].expression),(yyvsp[0].expression))); }
    break;

  case 416: /* comp_or_expr_head: comp_or_expr_head ',' "identifier" ':' expr  */
      { if ((yyvsp[-2].sValue) && (yyvsp[-2].sValue)[0]=='_') {
          std::ostringstream oss;
          oss << "parameter `" << std::string((yyvsp[-2].sValue)) << "' starts with '_' ";
          oss << "and cannot be used as a named argument (pass it positionally)";
          yyerror(&(yylsp[-2]), parm, oss.str());
        }
        (yyval.expressionPairs)=(yyvsp[-4].expressionPairs);
        if ((yyval.expressionPairs) && (yyvsp[0].expression)) {
          VarDecl* vd = new VarDecl((yyloc), new TypeInst((yyloc), Type()), (yyvsp[-2].sValue), (yyvsp[0].expression));
          (yyval.expressionPairs)->push_back(pair<Expression*,Expression*>(vd,nullptr));
        }
        free((yyvsp[-2].sValue));
      }
    break;

  case 417: /* let_expr: "let" '{' '}' "in" expr  */
      { (yyval.expression)=(yyvsp[0].expression); }
    break;

  case 418: /* let_expr: "let" '{' let_vardecl_item_list '}' "in" expr  */
      { if ((yyvsp[-3].expressions1d) && (yyvsp[0].expression)) {
          (yyval.expression)=new Let((yyloc), *(yyvsp[-3].expressions1d), (yyvsp[0].expression)); delete (yyvsp[-3].expressions1d);
        } else {
          (yyval.expression)=nullptr;
        }
      }
    break;

  case 419: /* let_expr: "let" '{' let_vardecl_item_list comma_or_semi '}' "in" expr  */
      { if ((yyvsp[-4].expressions1d) && (yyvsp[0].expression)) {
          (yyval.expression)=new Let((yyloc), *(yyvsp[-4].expressions1d), (yyvsp[0].expression)); delete (yyvsp[-4].expressions1d);
        } else {
          (yyval.expression)=nullptr;
        }
      }
    break;

  case 420: /* let_vardecl_item_list: let_vardecl_item  */
      { (yyval.expressions1d)=new vector<Expression*>; (yyval.expressions1d)->push_back((yyvsp[0].vardeclexpr)); }
    break;

  case 421: /* let_vardecl_item_list: constraint_item  */
      { (yyval.expressions1d)=new vector<Expression*>;
        if ((yyvsp[0].item)) {
          ConstraintI* ce = (yyvsp[0].item)->cast<ConstraintI>();
          (yyval.expressions1d)->push_back(ce->e());
          ce->e(nullptr);
        }
      }
    break;

  case 422: /* let_vardecl_item_list: let_vardecl_item_list comma_or_semi let_vardecl_item  */
      { (yyval.expressions1d)=(yyvsp[-2].expressions1d); if ((yyval.expressions1d) && (yyvsp[0].vardeclexpr)) (yyval.expressions1d)->push_back((yyvsp[0].vardeclexpr)); }
    break;

  case 423: /* let_vardecl_item_list: let_vardecl_item_list comma_or_semi constraint_item  */
      { (yyval.expressions1d)=(yyvsp[-2].expressions1d);
        if ((yyval.expressions1d) && (yyvsp[0].item)) {
          ConstraintI* ce = (yyvsp[0].item)->cast<ConstraintI>();
          (yyval.expressions1d)->push_back(ce->e());
          ce->e(nullptr);
        }
      }
    break;

  case 426: /* let_vardecl_item: ti_expr_and_id  */
      { (yyval.vardeclexpr) = (yyvsp[0].vardeclexpr);
        if ((yyvsp[0].vardeclexpr) && Expression::type((yyvsp[0].vardeclexpr)->ti()).any() && (yyvsp[0].vardeclexpr)->ti()->domain() == nullptr) {
          // This is an any type, not allowed without a right hand side
          yyerror(&(yylsp[0]), parm, "declarations with `any' type-inst require definition");
        }
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->toplevel(false);
      }
    break;

  case 427: /* let_vardecl_item: ti_expr_and_id "=" expr  */
      { if ((yyvsp[-2].vardeclexpr)) {
          (yyvsp[-2].vardeclexpr)->e((yyvsp[0].expression));
        }
        (yyval.vardeclexpr) = (yyvsp[-2].vardeclexpr);
        if ((yyval.vardeclexpr)) Expression::loc((yyval.vardeclexpr), (yyloc));
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->toplevel(false);
      }
    break;

  case 428: /* annotations: %empty  */
      { (yyval.expressions1d)=nullptr; }
    break;

  case 430: /* annotation_expr: expr_atom_head_nonstring  */
      { (yyval.expression) = (yyvsp[0].expression); }
    break;

  case 431: /* annotation_expr: "output"  */
      { (yyval.expression) = new Id((yylsp[0]), Constants::constants().ids.output, nullptr); }
    break;

  case 432: /* annotation_expr: string_expr  */
      { (yyval.expression) = Call::a((yylsp[0]), ASTString("mzn_expression_name"), {(yyvsp[0].expression)}); }
    break;

  case 433: /* ne_annotations: "::" annotation_expr  */
      { (yyval.expressions1d)=new std::vector<Expression*>(1);
        (*(yyval.expressions1d))[0] = (yyvsp[0].expression);
      }
    break;

  case 434: /* ne_annotations: ne_annotations "::" annotation_expr  */
      { (yyval.expressions1d)=(yyvsp[-2].expressions1d); if ((yyval.expressions1d)) (yyval.expressions1d)->push_back((yyvsp[0].expression)); }
    break;

  case 435: /* id_or_quoted_op: "identifier"  */
      { (yyval.sValue)=(yyvsp[0].sValue); }
    break;

  case 436: /* id_or_quoted_op: "identifier" "^-1"  */
      { (yyval.sValue)=strdup((std::string((yyvsp[-1].sValue))+"⁻¹").c_str()); }
    break;

  case 437: /* id_or_quoted_op: "'<->'"  */
      { (yyval.sValue)=strdup("'<->'"); }
    break;

  case 438: /* id_or_quoted_op: "'->'"  */
      { (yyval.sValue)=strdup("'->'"); }
    break;

  case 439: /* id_or_quoted_op: "'<-'"  */
      { (yyval.sValue)=strdup("'<-'"); }
    break;

  case 440: /* id_or_quoted_op: "'\\/'"  */
      { (yyval.sValue)=strdup("'\\/'"); }
    break;

  case 441: /* id_or_quoted_op: "'xor'"  */
      { (yyval.sValue)=strdup("'xor'"); }
    break;

  case 442: /* id_or_quoted_op: "'/\\'"  */
      { (yyval.sValue)=strdup("'/\\'"); }
    break;

  case 443: /* id_or_quoted_op: "'<'"  */
      { (yyval.sValue)=strdup("'<'"); }
    break;

  case 444: /* id_or_quoted_op: "'>'"  */
      { (yyval.sValue)=strdup("'>'"); }
    break;

  case 445: /* id_or_quoted_op: "'<='"  */
      { (yyval.sValue)=strdup("'<='"); }
    break;

  case 446: /* id_or_quoted_op: "'>='"  */
      { (yyval.sValue)=strdup("'>='"); }
    break;

  case 447: /* id_or_quoted_op: "'='"  */
      { (yyval.sValue)=strdup("'='"); }
    break;

  case 448: /* id_or_quoted_op: "'!='"  */
      { (yyval.sValue)=strdup("'!='"); }
    break;

  case 449: /* id_or_quoted_op: "'in'"  */
      { (yyval.sValue)=strdup("'in'"); }
    break;

  case 450: /* id_or_quoted_op: "'subset'"  */
      { (yyval.sValue)=strdup("'subset'"); }
    break;

  case 451: /* id_or_quoted_op: "'superset'"  */
      { (yyval.sValue)=strdup("'superset'"); }
    break;

  case 452: /* id_or_quoted_op: "'union'"  */
      { (yyval.sValue)=strdup("'union'"); }
    break;

  case 453: /* id_or_quoted_op: "'diff'"  */
      { (yyval.sValue)=strdup("'diff'"); }
    break;

  case 454: /* id_or_quoted_op: "'symdiff'"  */
      { (yyval.sValue)=strdup("'symdiff'"); }
    break;

  case 455: /* id_or_quoted_op: "'..'"  */
      { (yyval.sValue)=strdup("'..'"); }
    break;

  case 456: /* id_or_quoted_op: "'<..'"  */
      { (yyval.sValue)=strdup("'<..'"); }
    break;

  case 457: /* id_or_quoted_op: "'..<'"  */
      { (yyval.sValue)=strdup("'..<'"); }
    break;

  case 458: /* id_or_quoted_op: "'<..<'"  */
      { (yyval.sValue)=strdup("'<..<'"); }
    break;

  case 459: /* id_or_quoted_op: "'+'"  */
      { (yyval.sValue)=strdup("'+'"); }
    break;

  case 460: /* id_or_quoted_op: "'-'"  */
      { (yyval.sValue)=strdup("'-'"); }
    break;

  case 461: /* id_or_quoted_op: "'*'"  */
      { (yyval.sValue)=strdup("'*'"); }
    break;

  case 462: /* id_or_quoted_op: "'^'"  */
      { (yyval.sValue)=strdup("'^'"); }
    break;

  case 463: /* id_or_quoted_op: "'/'"  */
      { (yyval.sValue)=strdup("'/'"); }
    break;

  case 464: /* id_or_quoted_op: "'div'"  */
      { (yyval.sValue)=strdup("'div'"); }
    break;

  case 465: /* id_or_quoted_op: "'mod'"  */
      { (yyval.sValue)=strdup("'mod'"); }
    break;

  case 466: /* id_or_quoted_op: "'intersect'"  */
      { (yyval.sValue)=strdup("'intersect'"); }
    break;

  case 467: /* id_or_quoted_op: "'not'"  */
      { (yyval.sValue)=strdup("'not'"); }
    break;

  case 468: /* id_or_quoted_op: "'++'"  */
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
          YYNOMEM;
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
  ++yynerrs;

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
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, parm, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
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

