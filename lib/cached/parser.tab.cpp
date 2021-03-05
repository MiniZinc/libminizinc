/* A Bison parser, made by GNU Bison 3.7.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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
#define YYBISON 30704

/* Bison version string.  */
#define YYBISON_VERSION "3.7.4"

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
  while (m->parent() != nullptr) {
    m = m->parent();
    pp->err << "(included from file '" << m->filename() << "')" << endl;
  }
  pp->err << location->toString() << ":" << endl;
  pp->printCurrentLine(location->firstColumn(),location->lastColumn());
  pp->err << "Error: " << str << std::endl;
  pp->hadError = true;
  pp->syntaxErrors.push_back(SyntaxError(Location(*location), str));
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
  Call* c = new Call(Location(loc), constants().ann.doc_comment, args);
  c->type(Type::ann());
  return c;
}

Expression* createArrayAccess(const ParserLocation& loc, Expression* e, std::vector<std::vector<Expression*> >& idx) {
  Expression* ret = e;
  for (unsigned int i=0; i<idx.size(); i++) {
    ret = new ArrayAccess(Location(loc), ret, idx[i]);
  }
  return ret;
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
  YYSYMBOL_MZN_VAR = 16,                   /* "var"  */
  YYSYMBOL_MZN_PAR = 17,                   /* "par"  */
  YYSYMBOL_MZN_ABSENT = 18,                /* "<>"  */
  YYSYMBOL_MZN_ANN = 19,                   /* "ann"  */
  YYSYMBOL_MZN_ANNOTATION = 20,            /* "annotation"  */
  YYSYMBOL_MZN_ANY = 21,                   /* "any"  */
  YYSYMBOL_MZN_ARRAY = 22,                 /* "array"  */
  YYSYMBOL_MZN_BOOL = 23,                  /* "bool"  */
  YYSYMBOL_MZN_CASE = 24,                  /* "case"  */
  YYSYMBOL_MZN_CONSTRAINT = 25,            /* "constraint"  */
  YYSYMBOL_MZN_DEFAULT = 26,               /* "default"  */
  YYSYMBOL_MZN_ELSE = 27,                  /* "else"  */
  YYSYMBOL_MZN_ELSEIF = 28,                /* "elseif"  */
  YYSYMBOL_MZN_ENDIF = 29,                 /* "endif"  */
  YYSYMBOL_MZN_ENUM = 30,                  /* "enum"  */
  YYSYMBOL_MZN_FLOAT = 31,                 /* "float"  */
  YYSYMBOL_MZN_FUNCTION = 32,              /* "function"  */
  YYSYMBOL_MZN_IF = 33,                    /* "if"  */
  YYSYMBOL_MZN_INCLUDE = 34,               /* "include"  */
  YYSYMBOL_MZN_INFINITY = 35,              /* "infinity"  */
  YYSYMBOL_MZN_INT = 36,                   /* "int"  */
  YYSYMBOL_MZN_LET = 37,                   /* "let"  */
  YYSYMBOL_MZN_LIST = 38,                  /* "list"  */
  YYSYMBOL_MZN_MAXIMIZE = 39,              /* "maximize"  */
  YYSYMBOL_MZN_MINIMIZE = 40,              /* "minimize"  */
  YYSYMBOL_MZN_OF = 41,                    /* "of"  */
  YYSYMBOL_MZN_OPT = 42,                   /* "opt"  */
  YYSYMBOL_MZN_SATISFY = 43,               /* "satisfy"  */
  YYSYMBOL_MZN_OUTPUT = 44,                /* "output"  */
  YYSYMBOL_MZN_PREDICATE = 45,             /* "predicate"  */
  YYSYMBOL_MZN_RECORD = 46,                /* "record"  */
  YYSYMBOL_MZN_SET = 47,                   /* "set"  */
  YYSYMBOL_MZN_SOLVE = 48,                 /* "solve"  */
  YYSYMBOL_MZN_STRING = 49,                /* "string"  */
  YYSYMBOL_MZN_TEST = 50,                  /* "test"  */
  YYSYMBOL_MZN_THEN = 51,                  /* "then"  */
  YYSYMBOL_MZN_TUPLE = 52,                 /* "tuple"  */
  YYSYMBOL_MZN_TYPE = 53,                  /* "type"  */
  YYSYMBOL_MZN_UNDERSCORE = 54,            /* "_"  */
  YYSYMBOL_MZN_VARIANT_RECORD = 55,        /* "variant_record"  */
  YYSYMBOL_MZN_WHERE = 56,                 /* "where"  */
  YYSYMBOL_MZN_LEFT_BRACKET = 57,          /* "["  */
  YYSYMBOL_MZN_LEFT_2D_BRACKET = 58,       /* "[|"  */
  YYSYMBOL_MZN_RIGHT_BRACKET = 59,         /* "]"  */
  YYSYMBOL_MZN_RIGHT_2D_BRACKET = 60,      /* "|]"  */
  YYSYMBOL_FLATZINC_IDENTIFIER = 61,       /* FLATZINC_IDENTIFIER  */
  YYSYMBOL_MZN_INVALID_INTEGER_LITERAL = 62, /* "invalid integer literal"  */
  YYSYMBOL_MZN_INVALID_FLOAT_LITERAL = 63, /* "invalid float literal"  */
  YYSYMBOL_MZN_UNTERMINATED_STRING = 64,   /* "unterminated string"  */
  YYSYMBOL_MZN_END_OF_LINE_IN_STRING = 65, /* "end of line inside string literal"  */
  YYSYMBOL_MZN_INVALID_NULL = 66,          /* "null character"  */
  YYSYMBOL_MZN_EQUIV = 67,                 /* "<->"  */
  YYSYMBOL_MZN_IMPL = 68,                  /* "->"  */
  YYSYMBOL_MZN_RIMPL = 69,                 /* "<-"  */
  YYSYMBOL_MZN_OR = 70,                    /* "\\/"  */
  YYSYMBOL_MZN_XOR = 71,                   /* "xor"  */
  YYSYMBOL_MZN_AND = 72,                   /* "/\\"  */
  YYSYMBOL_MZN_LE = 73,                    /* "<"  */
  YYSYMBOL_MZN_GR = 74,                    /* ">"  */
  YYSYMBOL_MZN_LQ = 75,                    /* "<="  */
  YYSYMBOL_MZN_GQ = 76,                    /* ">="  */
  YYSYMBOL_MZN_EQ = 77,                    /* "="  */
  YYSYMBOL_MZN_NQ = 78,                    /* "!="  */
  YYSYMBOL_MZN_WEAK_EQ = 79,               /* "~="  */
  YYSYMBOL_MZN_IN = 80,                    /* "in"  */
  YYSYMBOL_MZN_SUBSET = 81,                /* "subset"  */
  YYSYMBOL_MZN_SUPERSET = 82,              /* "superset"  */
  YYSYMBOL_MZN_UNION = 83,                 /* "union"  */
  YYSYMBOL_MZN_DIFF = 84,                  /* "diff"  */
  YYSYMBOL_MZN_SYMDIFF = 85,               /* "symdiff"  */
  YYSYMBOL_MZN_DOTDOT = 86,                /* ".."  */
  YYSYMBOL_MZN_PLUS = 87,                  /* "+"  */
  YYSYMBOL_MZN_MINUS = 88,                 /* "-"  */
  YYSYMBOL_MZN_WEAK_PLUS = 89,             /* "~+"  */
  YYSYMBOL_MZN_WEAK_MINUS = 90,            /* "~-"  */
  YYSYMBOL_MZN_MULT = 91,                  /* "*"  */
  YYSYMBOL_MZN_DIV = 92,                   /* "/"  */
  YYSYMBOL_MZN_IDIV = 93,                  /* "div"  */
  YYSYMBOL_MZN_MOD = 94,                   /* "mod"  */
  YYSYMBOL_MZN_INTERSECT = 95,             /* "intersect"  */
  YYSYMBOL_MZN_WEAK_MULT = 96,             /* "~*"  */
  YYSYMBOL_MZN_POW = 97,                   /* "^"  */
  YYSYMBOL_MZN_POW_MINUS1 = 98,            /* "^-1"  */
  YYSYMBOL_MZN_NOT = 99,                   /* "not"  */
  YYSYMBOL_MZN_PLUSPLUS = 100,             /* "++"  */
  YYSYMBOL_MZN_COLONCOLON = 101,           /* "::"  */
  YYSYMBOL_PREC_ANNO = 102,                /* PREC_ANNO  */
  YYSYMBOL_MZN_EQUIV_QUOTED = 103,         /* "'<->'"  */
  YYSYMBOL_MZN_IMPL_QUOTED = 104,          /* "'->'"  */
  YYSYMBOL_MZN_RIMPL_QUOTED = 105,         /* "'<-'"  */
  YYSYMBOL_MZN_OR_QUOTED = 106,            /* "'\\/'"  */
  YYSYMBOL_MZN_XOR_QUOTED = 107,           /* "'xor'"  */
  YYSYMBOL_MZN_AND_QUOTED = 108,           /* "'/\\'"  */
  YYSYMBOL_MZN_LE_QUOTED = 109,            /* "'<'"  */
  YYSYMBOL_MZN_GR_QUOTED = 110,            /* "'>'"  */
  YYSYMBOL_MZN_LQ_QUOTED = 111,            /* "'<='"  */
  YYSYMBOL_MZN_GQ_QUOTED = 112,            /* "'>='"  */
  YYSYMBOL_MZN_EQ_QUOTED = 113,            /* "'='"  */
  YYSYMBOL_MZN_NQ_QUOTED = 114,            /* "'!='"  */
  YYSYMBOL_MZN_IN_QUOTED = 115,            /* "'in'"  */
  YYSYMBOL_MZN_SUBSET_QUOTED = 116,        /* "'subset'"  */
  YYSYMBOL_MZN_SUPERSET_QUOTED = 117,      /* "'superset'"  */
  YYSYMBOL_MZN_UNION_QUOTED = 118,         /* "'union'"  */
  YYSYMBOL_MZN_DIFF_QUOTED = 119,          /* "'diff'"  */
  YYSYMBOL_MZN_SYMDIFF_QUOTED = 120,       /* "'symdiff'"  */
  YYSYMBOL_MZN_DOTDOT_QUOTED = 121,        /* "'..'"  */
  YYSYMBOL_MZN_PLUS_QUOTED = 122,          /* "'+'"  */
  YYSYMBOL_MZN_MINUS_QUOTED = 123,         /* "'-'"  */
  YYSYMBOL_MZN_MULT_QUOTED = 124,          /* "'*'"  */
  YYSYMBOL_MZN_DIV_QUOTED = 125,           /* "'/'"  */
  YYSYMBOL_MZN_IDIV_QUOTED = 126,          /* "'div'"  */
  YYSYMBOL_MZN_MOD_QUOTED = 127,           /* "'mod'"  */
  YYSYMBOL_MZN_INTERSECT_QUOTED = 128,     /* "'intersect'"  */
  YYSYMBOL_MZN_POW_QUOTED = 129,           /* "'^'"  */
  YYSYMBOL_MZN_NOT_QUOTED = 130,           /* "'not'"  */
  YYSYMBOL_MZN_COLONCOLON_QUOTED = 131,    /* "'::'"  */
  YYSYMBOL_MZN_PLUSPLUS_QUOTED = 132,      /* "'++'"  */
  YYSYMBOL_133_ = 133,                     /* ';'  */
  YYSYMBOL_134_ = 134,                     /* '{'  */
  YYSYMBOL_135_ = 135,                     /* '}'  */
  YYSYMBOL_136_ = 136,                     /* '('  */
  YYSYMBOL_137_ = 137,                     /* ')'  */
  YYSYMBOL_138_ = 138,                     /* ','  */
  YYSYMBOL_139_ = 139,                     /* ':'  */
  YYSYMBOL_140_ = 140,                     /* '|'  */
  YYSYMBOL_YYACCEPT = 141,                 /* $accept  */
  YYSYMBOL_model = 142,                    /* model  */
  YYSYMBOL_item_list = 143,                /* item_list  */
  YYSYMBOL_item_list_head = 144,           /* item_list_head  */
  YYSYMBOL_doc_file_comments = 145,        /* doc_file_comments  */
  YYSYMBOL_semi_or_none = 146,             /* semi_or_none  */
  YYSYMBOL_item = 147,                     /* item  */
  YYSYMBOL_item_tail = 148,                /* item_tail  */
  YYSYMBOL_error_item_start = 149,         /* error_item_start  */
  YYSYMBOL_include_item = 150,             /* include_item  */
  YYSYMBOL_vardecl_item = 151,             /* vardecl_item  */
  YYSYMBOL_enum_init = 152,                /* enum_init  */
  YYSYMBOL_enum_construct = 153,           /* enum_construct  */
  YYSYMBOL_string_lit_list = 154,          /* string_lit_list  */
  YYSYMBOL_enum_id_list = 155,             /* enum_id_list  */
  YYSYMBOL_assign_item = 156,              /* assign_item  */
  YYSYMBOL_constraint_item = 157,          /* constraint_item  */
  YYSYMBOL_solve_item = 158,               /* solve_item  */
  YYSYMBOL_output_item = 159,              /* output_item  */
  YYSYMBOL_predicate_item = 160,           /* predicate_item  */
  YYSYMBOL_function_item = 161,            /* function_item  */
  YYSYMBOL_annotation_item = 162,          /* annotation_item  */
  YYSYMBOL_operation_item_tail = 163,      /* operation_item_tail  */
  YYSYMBOL_params = 164,                   /* params  */
  YYSYMBOL_params_list = 165,              /* params_list  */
  YYSYMBOL_params_list_head = 166,         /* params_list_head  */
  YYSYMBOL_comma_or_none = 167,            /* comma_or_none  */
  YYSYMBOL_ti_expr_and_id_or_anon = 168,   /* ti_expr_and_id_or_anon  */
  YYSYMBOL_ti_expr_and_id = 169,           /* ti_expr_and_id  */
  YYSYMBOL_ti_expr_list = 170,             /* ti_expr_list  */
  YYSYMBOL_ti_expr_list_head = 171,        /* ti_expr_list_head  */
  YYSYMBOL_ti_expr = 172,                  /* ti_expr  */
  YYSYMBOL_base_ti_expr = 173,             /* base_ti_expr  */
  YYSYMBOL_opt_opt = 174,                  /* opt_opt  */
  YYSYMBOL_base_ti_expr_tail = 175,        /* base_ti_expr_tail  */
  YYSYMBOL_array_access_expr_list = 176,   /* array_access_expr_list  */
  YYSYMBOL_array_access_expr_list_head = 177, /* array_access_expr_list_head  */
  YYSYMBOL_array_access_expr = 178,        /* array_access_expr  */
  YYSYMBOL_expr_list = 179,                /* expr_list  */
  YYSYMBOL_expr_list_head = 180,           /* expr_list_head  */
  YYSYMBOL_set_expr = 181,                 /* set_expr  */
  YYSYMBOL_expr = 182,                     /* expr  */
  YYSYMBOL_expr_atom_head = 183,           /* expr_atom_head  */
  YYSYMBOL_expr_atom_head_nonstring = 184, /* expr_atom_head_nonstring  */
  YYSYMBOL_string_expr = 185,              /* string_expr  */
  YYSYMBOL_string_quote_rest = 186,        /* string_quote_rest  */
  YYSYMBOL_array_access_tail = 187,        /* array_access_tail  */
  YYSYMBOL_set_literal = 188,              /* set_literal  */
  YYSYMBOL_set_comp = 189,                 /* set_comp  */
  YYSYMBOL_comp_tail = 190,                /* comp_tail  */
  YYSYMBOL_generator_list = 191,           /* generator_list  */
  YYSYMBOL_generator_list_head = 192,      /* generator_list_head  */
  YYSYMBOL_generator = 193,                /* generator  */
  YYSYMBOL_generator_eq = 194,             /* generator_eq  */
  YYSYMBOL_id_list = 195,                  /* id_list  */
  YYSYMBOL_id_list_head = 196,             /* id_list_head  */
  YYSYMBOL_simple_array_literal = 197,     /* simple_array_literal  */
  YYSYMBOL_simple_array_literal_2d = 198,  /* simple_array_literal_2d  */
  YYSYMBOL_simple_array_literal_3d_list = 199, /* simple_array_literal_3d_list  */
  YYSYMBOL_simple_array_literal_2d_list = 200, /* simple_array_literal_2d_list  */
  YYSYMBOL_simple_array_comp = 201,        /* simple_array_comp  */
  YYSYMBOL_if_then_else_expr = 202,        /* if_then_else_expr  */
  YYSYMBOL_elseif_list = 203,              /* elseif_list  */
  YYSYMBOL_quoted_op = 204,                /* quoted_op  */
  YYSYMBOL_quoted_op_call = 205,           /* quoted_op_call  */
  YYSYMBOL_call_expr = 206,                /* call_expr  */
  YYSYMBOL_comp_or_expr = 207,             /* comp_or_expr  */
  YYSYMBOL_comp_or_expr_head = 208,        /* comp_or_expr_head  */
  YYSYMBOL_let_expr = 209,                 /* let_expr  */
  YYSYMBOL_let_vardecl_item_list = 210,    /* let_vardecl_item_list  */
  YYSYMBOL_comma_or_semi = 211,            /* comma_or_semi  */
  YYSYMBOL_let_vardecl_item = 212,         /* let_vardecl_item  */
  YYSYMBOL_annotations = 213,              /* annotations  */
  YYSYMBOL_annotation_expr = 214,          /* annotation_expr  */
  YYSYMBOL_ne_annotations = 215,           /* ne_annotations  */
  YYSYMBOL_id_or_quoted_op = 216           /* id_or_quoted_op  */
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
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
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
#define YYFINAL  166
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   5229

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  141
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  76
/* YYNRULES -- Number of rules.  */
#define YYNRULES  356
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  588

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   387


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
     136,   137,     2,     2,   138,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   139,   133,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   134,   140,   135,     2,     2,     2,     2,
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
     125,   126,   127,   128,   129,   130,   131,   132
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   286,   286,   288,   290,   293,   302,   311,   320,   329,
     331,   334,   342,   351,   351,   353,   369,   373,   375,   377,
     378,   380,   382,   384,   386,   388,   391,   391,   391,   392,
     392,   392,   392,   392,   393,   396,   419,   425,   432,   442,
     460,   476,   480,   489,   494,   503,   504,   508,   516,   517,
     521,   525,   531,   533,   540,   545,   550,   557,   561,   571,
     581,   589,   599,   608,   619,   632,   642,   643,   648,   649,
     651,   656,   657,   661,   665,   670,   670,   673,   675,   679,
     687,   691,   693,   697,   698,   704,   713,   716,   724,   732,
     741,   749,   758,   767,   780,   781,   785,   787,   789,   791,
     793,   795,   797,   802,   808,   811,   813,   817,   819,   821,
     830,   841,   844,   846,   852,   853,   855,   857,   859,   861,
     870,   879,   881,   883,   885,   887,   889,   891,   893,   895,
     897,   902,   907,   912,   917,   923,   925,   938,   939,   941,
     943,   945,   947,   949,   951,   953,   955,   957,   959,   961,
     963,   965,   967,   969,   971,   973,   975,   977,   986,   995,
     997,   999,  1001,  1003,  1005,  1007,  1009,  1011,  1013,  1018,
    1023,  1028,  1033,  1039,  1041,  1048,  1060,  1062,  1066,  1068,
    1070,  1072,  1074,  1076,  1079,  1081,  1084,  1086,  1089,  1091,
    1094,  1096,  1098,  1100,  1102,  1104,  1106,  1108,  1110,  1112,
    1114,  1115,  1118,  1120,  1123,  1124,  1127,  1129,  1132,  1133,
    1136,  1138,  1141,  1142,  1145,  1147,  1150,  1151,  1154,  1156,
    1159,  1160,  1163,  1165,  1168,  1169,  1170,  1173,  1174,  1179,
    1181,  1187,  1192,  1200,  1207,  1216,  1218,  1223,  1229,  1232,
    1235,  1237,  1239,  1245,  1247,  1249,  1257,  1259,  1262,  1265,
    1268,  1270,  1274,  1276,  1280,  1282,  1293,  1304,  1344,  1347,
    1352,  1359,  1364,  1368,  1374,  1381,  1397,  1398,  1402,  1404,
    1406,  1408,  1410,  1412,  1414,  1416,  1418,  1420,  1422,  1424,
    1426,  1428,  1430,  1432,  1434,  1436,  1438,  1440,  1442,  1444,
    1446,  1448,  1450,  1452,  1454,  1456,  1460,  1468,  1500,  1502,
    1504,  1505,  1525,  1579,  1599,  1654,  1657,  1663,  1669,  1671,
    1675,  1682,  1691,  1693,  1701,  1703,  1712,  1712,  1715,  1721,
    1732,  1733,  1736,  1738,  1742,  1746,  1750,  1752,  1754,  1756,
    1758,  1760,  1762,  1764,  1766,  1768,  1770,  1772,  1774,  1776,
    1778,  1780,  1782,  1784,  1786,  1788,  1790,  1792,  1794,  1796,
    1798,  1800,  1802,  1804,  1806,  1808,  1810
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
  "\"file-level documentation comment\"", "\"var\"", "\"par\"", "\"<>\"",
  "\"ann\"", "\"annotation\"", "\"any\"", "\"array\"", "\"bool\"",
  "\"case\"", "\"constraint\"", "\"default\"", "\"else\"", "\"elseif\"",
  "\"endif\"", "\"enum\"", "\"float\"", "\"function\"", "\"if\"",
  "\"include\"", "\"infinity\"", "\"int\"", "\"let\"", "\"list\"",
  "\"maximize\"", "\"minimize\"", "\"of\"", "\"opt\"", "\"satisfy\"",
  "\"output\"", "\"predicate\"", "\"record\"", "\"set\"", "\"solve\"",
  "\"string\"", "\"test\"", "\"then\"", "\"tuple\"", "\"type\"", "\"_\"",
  "\"variant_record\"", "\"where\"", "\"[\"", "\"[|\"", "\"]\"", "\"|]\"",
  "FLATZINC_IDENTIFIER", "\"invalid integer literal\"",
  "\"invalid float literal\"", "\"unterminated string\"",
  "\"end of line inside string literal\"", "\"null character\"", "\"<->\"",
  "\"->\"", "\"<-\"", "\"\\\\/\"", "\"xor\"", "\"/\\\\\"", "\"<\"",
  "\">\"", "\"<=\"", "\">=\"", "\"=\"", "\"!=\"", "\"~=\"", "\"in\"",
  "\"subset\"", "\"superset\"", "\"union\"", "\"diff\"", "\"symdiff\"",
  "\"..\"", "\"+\"", "\"-\"", "\"~+\"", "\"~-\"", "\"*\"", "\"/\"",
  "\"div\"", "\"mod\"", "\"intersect\"", "\"~*\"", "\"^\"", "\"^-1\"",
  "\"not\"", "\"++\"", "\"::\"", "PREC_ANNO", "\"'<->'\"", "\"'->'\"",
  "\"'<-'\"", "\"'\\\\/'\"", "\"'xor'\"", "\"'/\\\\'\"", "\"'<'\"",
  "\"'>'\"", "\"'<='\"", "\"'>='\"", "\"'='\"", "\"'!='\"", "\"'in'\"",
  "\"'subset'\"", "\"'superset'\"", "\"'union'\"", "\"'diff'\"",
  "\"'symdiff'\"", "\"'..'\"", "\"'+'\"", "\"'-'\"", "\"'*'\"", "\"'/'\"",
  "\"'div'\"", "\"'mod'\"", "\"'intersect'\"", "\"'^'\"", "\"'not'\"",
  "\"'::'\"", "\"'++'\"", "';'", "'{'", "'}'", "'('", "')'", "','", "':'",
  "'|'", "$accept", "model", "item_list", "item_list_head",
  "doc_file_comments", "semi_or_none", "item", "item_tail",
  "error_item_start", "include_item", "vardecl_item", "enum_init",
  "enum_construct", "string_lit_list", "enum_id_list", "assign_item",
  "constraint_item", "solve_item", "output_item", "predicate_item",
  "function_item", "annotation_item", "operation_item_tail", "params",
  "params_list", "params_list_head", "comma_or_none",
  "ti_expr_and_id_or_anon", "ti_expr_and_id", "ti_expr_list",
  "ti_expr_list_head", "ti_expr", "base_ti_expr", "opt_opt",
  "base_ti_expr_tail", "array_access_expr_list",
  "array_access_expr_list_head", "array_access_expr", "expr_list",
  "expr_list_head", "set_expr", "expr", "expr_atom_head",
  "expr_atom_head_nonstring", "string_expr", "string_quote_rest",
  "array_access_tail", "set_literal", "set_comp", "comp_tail",
  "generator_list", "generator_list_head", "generator", "generator_eq",
  "id_list", "id_list_head", "simple_array_literal",
  "simple_array_literal_2d", "simple_array_literal_3d_list",
  "simple_array_literal_2d_list", "simple_array_comp", "if_then_else_expr",
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
     385,   386,   387,    59,   123,   125,    40,    41,    44,    58,
     124
};
#endif

#define YYPACT_NINF (-427)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-72)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     888,   -78,   -39,   -38,   -16,    -9,  -427,  3705,  -427,  -427,
    1829,  -427,    20,    20,    -6,  -427,    77,    38,  -427,  3035,
     107,  -427,  2231,  3705,   110,    48,  -427,    18,   114,  2499,
    3705,   109,   117,    72,  -427,   171,   -37,  3169,   569,  3839,
    3839,  -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,
    -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,    45,
    -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,
    3303,  3705,   185,  -427,    53,  1427,   300,  -427,  -427,  -427,
    -427,  -427,  -427,  -427,  -427,  -427,  -427,    72,    52,  -427,
    -427,   354,  -427,  -427,  -427,   -35,   -12,    -4,     7,     9,
      10,    58,  -427,    23,  -427,  1695,  -427,  -427,  -427,  3437,
    3705,    65,  1157,    55,     8,  3705,  3705,  3705,    66,     5,
    5000,  -427,  -427,  -427,  -427,  2633,  2767,  -427,    67,  2231,
      27,  5000,    72,    68,  4721,  -427,  -427,  2097,  2365,   163,
    -427,  5000,   -61,  2901,   607,   140,   105,   -49,  -427,    84,
    -427,   149,    71,  3909,  -427,   750,  -427,   -27,   -31,    37,
      37,  3705,  -427,    76,  4000,  4252,  -427,  1561,  -427,  -427,
    -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,
    -427,   135,   252,  3839,  3839,  3839,  3839,  3839,  3839,  3839,
    3839,  3839,  3839,  3839,  3839,  3839,  3839,  3839,  3839,  3839,
    3839,   607,  -427,   100,  -427,   115,  -427,   119,  -427,   159,
    -427,   211,  -427,   214,  3705,  -427,   216,  -427,  3705,   205,
     128,  -427,  5037,  5000,  1292,  -427,  4763,   132,   134,  3437,
    -427,    49,    49,    49,  3705,  3705,  -427,  3705,  3705,  3705,
    3705,  3705,  3705,  3705,  3705,  3705,  3705,  3705,  3705,  3705,
    3705,  3705,  3705,  3705,  3705,  3705,  3705,  3705,  3705,  3705,
    3705,  3705,  3705,  3705,  3705,  3705,  3705,  3705,  3705,  3705,
    3705,   607,   234,  -427,   236,  -427,  1022,   204,   223,   145,
    -427,  3705,   208,  3908,  3705,  -427,    72,   147,   -95,  -427,
    -427,  2901,    67,    72,  -427,  -427,  -427,  -427,  3705,  3705,
    -427,   607,    67,    72,  -427,  -427,  3705,  -427,   278,  -427,
     148,  -427,   150,  -427,  3571,  4126,  -427,   278,   217,  1427,
    -427,  3705,   153,   190,  1112,    74,    74,    74,   207,    32,
      32,    32,    32,    34,    34,    34,    34,    74,    34,    37,
      16,  -427,  -427,  -427,  -427,  -427,  -427,  -427,  4035,  -427,
    5000,  -427,  3437,  -427,  3705,  -427,   168,  3705,   170,  3705,
    -427,   254,  4161,  -427,  5000,   210,  5091,  1146,  1146,  1281,
    1281,  1416,  5128,  5128,  5128,  5128,  5128,  5128,  5128,   731,
     731,   731,  1244,  1244,  1244,  2987,    96,    96,    96,    96,
      39,    39,    39,    39,  1244,    39,    49,    19,  -427,  2901,
    2901,   181,   186,   184,  -427,  -427,   147,  3705,   283,  2231,
    -427,  5000,    22,   228,  -427,  -427,  -427,  -427,  -427,  -427,
    -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,
    -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,
    -427,  -427,  -427,    67,  4665,   250,   322,  -427,   249,  -427,
    1963,  -427,    72,   259,  5000,  5000,  -427,    72,   259,   260,
     272,  -427,   201,  -427,   285,   262,   209,  3705,  3705,  -427,
    -427,  3705,   218,  -427,   219,  -427,  5000,  2231,  -427,  3705,
    -427,   213,  5000,  3705,  4813,  -427,  3705,  -427,  -427,  -427,
    -427,  2231,  -427,  5000,  2365,  -427,   220,   335,   345,   255,
    -427,  -427,    72,  -427,    30,  3705,  -427,  3705,   274,  -427,
    -427,   259,  3705,  -427,   259,  -427,  3705,  -427,   278,  -427,
    3705,  3705,   351,  -427,   224,  4287,  -427,  -427,   222,  4378,
    3705,  4413,  3705,  4504,  -427,  -427,  3705,  -427,   -28,  -427,
     -88,    13,   259,  3705,  3705,  5000,  5000,  3705,  -427,  5000,
    -427,  5000,  -427,   307,  5000,  4855,  -427,  3705,  -427,    72,
    -427,  4539,  -427,  5000,  -427,  4630,  -427,   357,  -427,   360,
    -427,  -427,  4946,  4909,  5000,  3705,  3705,   259,  -427,  -427,
    -427,  -427,  -427,  3705,  5000,  5000,  -427,  5000
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       0,     0,   192,   190,   196,   182,   229,     0,   102,   103,
       0,    11,    94,    94,   198,   100,     0,     0,    97,     0,
       0,    98,     0,     0,     0,   194,    96,     0,     0,     0,
       0,     0,     0,   320,    99,     0,   186,     0,     0,     0,
       0,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     277,   278,   279,   280,   281,   282,   283,   284,   285,     0,
     286,   287,   288,   290,   291,   292,   293,   289,   295,   294,
       0,     0,     0,     2,    13,     0,     5,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,   320,     0,    83,
      86,   101,   114,   176,   177,   200,   204,   208,   212,   216,
     220,     0,   300,   225,   224,     0,   193,   191,   197,     0,
       0,   184,     0,   183,   182,     0,     0,     0,     0,     0,
     112,   137,   230,    15,    95,     0,     0,   199,    68,     0,
       0,    52,   320,     0,     0,    35,   195,     0,     0,     0,
      87,    57,    68,     0,     0,     0,   321,    68,   188,   187,
     252,     0,    75,   112,   254,     0,   261,     0,     0,   135,
     136,     0,   235,     0,   112,     0,     1,    14,     4,    12,
       6,    34,    29,    27,    32,    26,    28,    31,    30,    33,
       9,    36,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   202,   201,   206,   205,   210,   209,   214,   213,
     218,   217,   222,   221,     0,   227,   226,    10,   108,     0,
      75,   105,   107,    51,     0,   298,   306,     0,    75,     0,
     185,   174,   175,   173,     0,     0,   231,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    89,     0,    88,     0,    64,     0,    75,
      81,     0,    38,     0,     0,   313,   320,     0,     0,   312,
      85,     0,    68,   320,    90,   322,   323,   324,     0,     0,
      54,     0,    68,   320,   189,   253,    76,   111,     0,   258,
       0,   257,     0,   255,     0,     0,   236,     0,   178,     0,
       7,     0,    79,   134,   133,   116,   117,   118,   119,   123,
     124,   130,   131,   125,   126,   127,   128,   121,   132,   129,
     122,   115,   203,   207,   211,   215,   219,   223,     0,   228,
     109,   233,    76,   104,   110,   299,     0,     0,   301,    76,
     305,     0,     0,   232,   113,   172,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   171,   151,
     152,   153,   154,   155,   156,   157,   161,   162,   168,   169,
     163,   164,   165,   166,   159,   170,   167,   160,   138,     0,
       0,     0,     0,    75,    73,    77,    78,     0,     0,    76,
      80,    53,     0,   326,   328,   329,   330,   331,   332,   333,
     334,   335,   336,   337,   338,   339,   340,   341,   342,   343,
     344,   345,   346,   347,   348,   349,   351,   352,   353,   354,
     350,   355,   356,    68,   266,   318,     0,   317,     0,   316,
       0,    91,   320,    66,    56,    55,   325,   320,    66,   250,
       0,   238,    75,   240,   241,     0,    75,   259,     0,   256,
     262,     0,     0,   180,   179,     8,    37,    71,   297,     0,
     106,   303,   307,     0,   308,   234,     0,    93,    92,    70,
      69,    76,    72,    65,     0,    82,     0,    45,    48,    39,
      41,   327,   320,   264,     0,     0,    79,     0,     0,   315,
     314,    66,     0,    58,    66,    59,     0,   263,    76,   239,
       0,     0,    76,   249,     0,     0,   237,   181,     0,     0,
       0,     0,     0,     0,    74,    84,     0,    46,     0,    49,
       0,     0,    66,     0,     0,   319,   310,     0,    60,    67,
      61,   248,   243,   244,   242,   246,   251,   260,   120,   320,
     296,     0,   302,   309,   158,     0,    40,     0,    43,     0,
      42,    62,     0,     0,   311,     0,     0,    66,   304,    44,
      47,    50,   265,     0,   245,   247,    63,   267
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -427,  -427,  -427,  -427,   200,  -427,   -63,   358,  -427,  -427,
    -427,  -427,  -172,  -427,  -427,  -427,  -134,  -427,  -427,  -427,
    -427,  -427,  -426,  -133,  -107,  -427,  -203,  -120,  -131,  -427,
    -427,   -15,  -136,   359,   -24,   144,  -427,    25,   -36,    17,
     215,   -19,   352,  -123,  -117,   143,   -26,  -427,  -427,    62,
    -427,  -427,  -138,  -135,  -427,  -427,  -427,  -427,  -427,  -147,
    -427,  -427,  -427,  -427,  -427,  -427,   158,  -427,  -427,  -427,
    -427,   -66,   -33,  -171,  -427,  -427
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    72,    73,    74,    75,   168,    76,    77,   180,    78,
      79,   499,   500,   538,   540,    80,    81,    82,    83,    84,
      85,    86,   513,   277,   402,   403,   307,   404,    87,   278,
     279,    88,    89,   125,    90,   219,   220,   221,   156,   152,
      91,   120,   121,    93,    94,   122,   113,    95,    96,   460,
     461,   462,   463,   464,   465,   466,    97,    98,   157,   158,
      99,   100,   504,   101,   102,   103,   227,   228,   104,   288,
     450,   289,   145,   297,   146,   443
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     131,   151,   290,   285,   134,   140,   286,   133,   310,   293,
     149,   141,   170,   281,   303,   235,   236,   353,   153,   496,
     109,   295,   109,   183,   119,   360,   238,   296,   496,   313,
     341,   566,   515,   311,   163,     6,     7,   292,   447,   183,
     448,   183,   217,   449,   183,   109,   238,   568,   109,   302,
     569,   164,   165,   109,   181,   105,   238,   543,   544,   106,
     107,   148,   124,   202,   109,   109,   109,   109,   110,   203,
     205,   207,   209,   211,   213,   276,   410,   216,   295,   497,
     109,   183,   108,   128,   296,   548,   204,   276,   550,   111,
     222,   223,   127,   226,   206,   129,   231,   232,   233,   282,
     398,   273,   275,   238,   320,   208,   111,   210,   212,   314,
     567,   312,   229,   132,   280,   142,   571,   201,   135,   294,
     271,   215,   287,   193,   194,   195,   196,   112,   198,   199,
     456,   199,   200,   201,   200,   201,   269,   200,   201,   270,
     271,   229,   315,   237,   112,   405,   136,   498,   295,   270,
     271,   586,   137,   230,   296,   138,   498,   229,   143,   452,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   457,
     198,   199,   229,   144,   200,   201,   229,   147,   295,   298,
     299,   161,   304,   300,   296,   166,   167,   263,   264,   265,
     266,   182,   268,   269,   214,   348,   270,   271,   342,   350,
     492,   224,   234,   276,   291,   226,   301,   283,   305,   306,
     222,   316,   321,   343,   183,   362,   229,   344,   364,   365,
     366,   367,   368,   369,   370,   371,   372,   373,   374,   375,
     376,   377,   378,   379,   380,   381,   382,   383,   384,   385,
     386,   387,   388,   389,   390,   391,   392,   393,   394,   395,
     396,   397,   119,   445,   159,   160,   475,   345,   322,   519,
     453,   406,   411,   523,   351,   444,   352,   451,   229,   358,
     458,   229,   359,   229,   109,   399,   229,   400,   470,   454,
     455,   407,   408,   409,   459,   412,   446,   364,   467,   477,
     468,   201,   474,   -72,   189,   190,   191,   192,   193,   194,
     195,   196,   476,   198,   199,   481,   483,   200,   201,   346,
     502,   271,   347,   485,   349,   473,   509,   527,   489,   286,
     171,   524,   491,   490,   494,   172,   501,   505,   506,   507,
     173,   517,   174,   222,   175,   385,   512,   516,   482,   518,
     484,   520,   521,   537,   176,   177,   405,   522,   178,   530,
     179,   539,    92,   526,   547,   541,   536,   556,   535,   559,
     405,   183,    92,   575,   557,   580,   581,   319,   123,   570,
     528,   534,   126,   361,    92,   487,   488,   480,   363,   472,
     552,    92,   356,   553,   510,     0,     0,     0,   493,     0,
       0,    92,    92,     0,   495,     0,     0,     0,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,     0,     0,     0,   511,
       0,     0,     0,     0,   514,     0,     0,    92,     0,     0,
       0,   470,     0,   184,     0,   287,     0,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   525,     0,   200,   201,     0,    92,     0,     0,
     529,     0,   406,     0,   531,     0,     0,   533,     0,   542,
       0,     0,     0,     0,     0,     0,   406,    92,    92,     0,
       0,    92,     0,     0,     0,     0,   545,     0,   546,    92,
      92,     0,     0,   549,     0,    92,     0,   551,     0,     0,
       0,   554,   555,     0,     0,     0,     0,     0,     0,     0,
       0,   561,     0,   563,     0,     0,     0,   565,     0,    92,
       0,   470,     0,     0,   572,   573,   577,     0,   574,     0,
       0,     0,     0,     0,     0,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,     0,     0,     0,   584,   585,     0,     0,
       0,     0,     0,     0,   587,     0,     0,     0,     0,     0,
       0,     0,     2,     3,     4,   114,     0,     6,     7,     0,
       0,     0,     0,     0,     0,     0,     0,    14,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    23,     0,    25,     0,    27,     0,     0,     0,
       2,     3,     4,   114,     0,     6,     7,     0,     0,     0,
       0,     0,     0,    36,     0,    14,    37,    38,    92,   154,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      23,     0,    25,    92,    27,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   115,   116,     0,     0,
       0,    36,     0,     0,    37,    38,     0,     0,   117,     0,
       0,    92,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
     118,    60,    61,    62,    63,    64,    65,    66,    67,    68,
       0,    69,     0,    70,     0,    71,     0,     0,     0,   155,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,     0,    60,
      61,    62,    63,    64,    65,    66,    67,    68,   238,    69,
       0,    70,     0,    71,     0,     0,     0,     0,     0,     0,
       0,    92,    92,     2,     3,     4,   114,     0,     6,     7,
       0,    92,     0,     0,     0,     0,     0,     0,    14,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    23,     0,    25,     0,    27,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    92,     0,    36,     0,     0,    37,    38,     0,
       0,   -72,   -72,   -72,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,    92,
       0,   270,   271,     0,     0,     0,     0,   115,   116,     0,
       0,     0,     0,    92,     0,     0,    92,     0,     0,   117,
       0,     0,     0,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,   118,    60,    61,    62,    63,    64,    65,    66,    67,
      68,     0,    69,     0,    70,     0,    71,     0,    -3,     1,
     309,     2,     3,     4,     5,     0,     6,     7,     0,     0,
       8,     9,    10,    11,    12,    13,    14,    15,    16,     0,
      17,    18,     0,    19,     0,     0,     0,     0,    20,    21,
      22,    23,    24,    25,    26,    27,    28,     0,     0,     0,
      29,     0,    30,    31,     0,    32,    33,    34,    35,     0,
       0,     0,    36,     0,     0,    37,    38,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    39,    40,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,     0,
      69,     0,    70,   401,    71,     2,     3,     4,   114,     0,
       6,     7,     0,     0,     8,     9,     0,     0,    12,    13,
      14,    15,     0,     0,    17,    18,     0,     0,     0,     0,
       0,     0,     0,    21,     0,    23,     0,    25,    26,    27,
      28,     0,     0,     0,    29,     0,     0,     0,     0,    32,
       0,    34,     0,     0,     0,     0,    36,     0,     0,    37,
      38,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    39,
      40,     0,     0,     0,     0,     0,     0,     0,     0,   183,
       0,     0,     0,     0,     0,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,   238,    69,     0,    70,     0,    71,   -71,
       2,     3,     4,   114,     0,     6,     7,     0,     0,     0,
       0,     0,     0,     0,     0,    14,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      23,   -72,    25,     0,    27,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   194,   195,   196,   197,   198,   199,
       0,    36,   200,   201,    37,    38,   242,   243,   244,   245,
     246,   247,   248,   249,   250,   251,   252,   253,   254,   255,
     256,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   115,   116,   270,   271,     0,     0,
       0,   238,     0,     0,     0,     0,   117,     0,     0,     0,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,   118,    60,
      61,    62,    63,    64,    65,    66,    67,    68,   238,    69,
       0,    70,     0,    71,   225,     2,     3,     4,   114,     0,
       6,     7,     0,     0,     0,     0,     0,     0,     0,     0,
      14,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    23,     0,    25,     0,    27,
     258,   259,   260,   261,   262,   263,   264,   265,   266,     0,
     268,   269,     0,     0,   270,   271,    36,     0,     0,    37,
      38,     0,     0,   244,   245,   246,   247,   248,   249,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   115,
     116,   270,   271,     0,     0,     0,     0,     0,     0,     0,
       0,   117,     0,     0,     0,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,   118,    60,    61,    62,    63,    64,    65,
      66,    67,    68,   238,    69,     0,    70,     0,    71,   355,
       2,     3,     4,     5,     0,     6,     7,     0,     0,     8,
       9,    10,   169,    12,    13,    14,    15,    16,     0,    17,
      18,     0,    19,     0,     0,     0,     0,    20,    21,    22,
      23,    24,    25,    26,    27,    28,     0,     0,     0,    29,
       0,    30,    31,     0,    32,    33,    34,    35,     0,     0,
       0,    36,     0,     0,    37,    38,     0,     0,     0,   245,
     246,   247,   248,   249,   250,   251,   252,   253,   254,   255,
     256,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,    39,    40,   270,   271,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,     0,    69,
       0,    70,     0,    71,     2,     3,     4,     5,     0,     6,
       7,     0,     0,     8,     9,    10,    11,    12,    13,    14,
      15,    16,     0,    17,    18,     0,    19,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,    27,    28,
       0,     0,     0,    29,     0,    30,    31,     0,    32,    33,
      34,    35,     0,     0,     0,    36,     0,     0,    37,    38,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    39,    40,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,     0,    69,     0,    70,     0,    71,     2,     3,
       4,     5,     0,     6,     7,     0,     0,     8,     9,    10,
       0,    12,    13,    14,    15,    16,     0,    17,    18,     0,
      19,     0,     0,     0,     0,    20,    21,    22,    23,    24,
      25,    26,    27,    28,     0,     0,     0,    29,     0,    30,
      31,     0,    32,    33,    34,    35,     0,     0,     0,    36,
       0,     0,    37,    38,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    39,    40,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,     0,    69,     0,    70,
       0,    71,     2,     3,     4,     5,     0,     6,     7,     0,
       0,     8,     9,     0,     0,    12,    13,    14,    15,    16,
       0,    17,    18,     0,    19,     0,     0,     0,     0,    20,
      21,    22,    23,    24,    25,    26,    27,    28,     0,     0,
       0,    29,     0,    30,    31,     0,    32,    33,    34,    35,
       0,     0,     0,    36,     0,     0,    37,    38,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    39,    40,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
       0,    69,     0,    70,     0,    71,     2,     3,     4,   114,
       0,     6,     7,     0,     0,     8,     9,     0,     0,    12,
      13,    14,    15,     0,     0,    17,    18,     0,    19,     0,
       0,     0,     0,     0,    21,     0,    23,     0,    25,    26,
      27,    28,     0,     0,     0,    29,     0,     0,     0,     0,
      32,     0,    34,     0,     0,     0,     0,    36,     0,     0,
      37,    38,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      39,    40,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,     0,    69,     0,    70,   508,    71,
       2,     3,     4,   114,     0,     6,     7,     0,     0,     8,
       9,     0,     0,    12,    13,    14,    15,     0,     0,    17,
      18,     0,    19,     0,     0,     0,     0,     0,    21,     0,
      23,     0,    25,    26,    27,    28,     0,     0,     0,    29,
       0,     0,     0,     0,    32,     0,    34,     0,     0,     0,
       0,    36,     0,     0,    37,    38,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    39,    40,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,     0,    69,
       0,    70,     0,    71,     2,     3,     4,   114,     0,     6,
       7,     0,     0,     8,     9,     0,     0,    12,    13,    14,
      15,     0,     0,    17,    18,     0,     0,     0,     0,     0,
       0,     0,    21,     0,    23,     0,    25,    26,    27,    28,
       0,     0,     0,    29,     0,     0,     0,     0,    32,     0,
      34,     0,     0,     0,     0,    36,     0,     0,    37,    38,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    39,    40,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,     0,    69,     0,    70,     0,    71,     2,     3,
       4,   114,     0,     6,     7,     0,     0,     8,     9,     0,
       0,    12,    13,    14,    15,     0,     0,     0,    18,     0,
       0,     0,     0,     0,     0,     0,    21,     0,    23,     0,
      25,    26,    27,     0,     0,     0,     0,    29,     0,     0,
       0,     0,    32,     0,    34,     0,     0,     0,     0,    36,
       0,     0,    37,    38,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    39,    40,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,     0,    69,     0,    70,
       0,    71,     2,     3,     4,   114,     0,     6,     7,     0,
       0,     8,     9,     0,     0,     0,     0,    14,    15,     0,
       0,     0,    18,     0,     0,     0,     0,     0,     0,     0,
      21,     0,    23,     0,    25,    26,    27,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   139,     0,    34,     0,
       0,     0,     0,    36,     0,     0,    37,    38,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    39,    40,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
       0,    69,     0,    70,     0,    71,     2,     3,     4,   114,
       0,     6,     7,     0,     0,     8,     9,     0,     0,     0,
       0,    14,    15,     0,     0,     0,    18,     0,     0,     0,
       0,     0,     0,     0,    21,     0,    23,     0,    25,    26,
      27,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     272,     0,    34,     0,     0,     0,     0,    36,     0,     0,
      37,    38,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      39,    40,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,     0,    69,     0,    70,     0,    71,
       2,     3,     4,   114,     0,     6,     7,     0,     0,     8,
       9,     0,     0,     0,     0,    14,    15,     0,     0,     0,
      18,     0,     0,     0,     0,     0,     0,     0,    21,     0,
      23,     0,    25,    26,    27,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   274,     0,    34,     0,     0,     0,
       0,    36,     0,     0,    37,    38,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    39,    40,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,     0,    69,
       0,    70,     0,    71,     2,     3,     4,   114,     0,     6,
       7,     0,     0,     8,     9,     0,     0,     0,     0,    14,
      15,     0,     0,     0,    18,     0,     0,     0,     0,     0,
       0,     0,    21,     0,    23,     0,    25,    26,    27,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      34,     0,     0,     0,     0,    36,     0,     0,    37,    38,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    39,    40,
       0,     0,     0,     0,   238,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,     0,    69,     0,    70,     0,    71,     2,     3,
       4,   114,     0,     6,     7,     0,     0,     0,     0,     0,
       0,     0,     0,    14,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    23,     0,
      25,     0,    27,   -72,   259,   260,   261,   262,   263,   264,
     265,   266,     0,   268,   269,     0,     0,   270,   271,    36,
       0,     0,    37,    38,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   115,   116,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   117,     0,   130,     0,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,   118,    60,    61,    62,
      63,    64,    65,    66,    67,    68,     0,    69,     0,    70,
       0,    71,     2,     3,     4,   114,     0,     6,     7,     0,
       0,     0,     0,     0,     0,     0,     0,    14,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    23,     0,    25,     0,    27,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    36,     0,     0,    37,    38,   150,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   115,   116,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   117,     0,
       0,     0,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
     118,    60,    61,    62,    63,    64,    65,    66,    67,    68,
       0,    69,     0,    70,     0,    71,     2,     3,     4,   114,
       0,     6,     7,     0,     0,     0,     0,     0,     0,     0,
       0,    14,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    23,     0,    25,     0,
      27,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    36,     0,     0,
      37,    38,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     115,   116,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   117,     0,     0,     0,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,   118,    60,    61,    62,    63,    64,
      65,    66,    67,    68,     0,    69,     0,    70,   162,    71,
       2,     3,     4,   114,     0,     6,     7,     0,     0,     0,
       0,     0,     0,     0,     0,    14,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      23,     0,    25,     0,    27,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    36,     0,     0,    37,    38,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   218,   115,   116,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   117,     0,     0,     0,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,   118,    60,
      61,    62,    63,    64,    65,    66,    67,    68,     0,    69,
       0,    70,     0,    71,     2,     3,     4,   114,     0,     6,
       7,     0,     0,     0,     0,     0,     0,     0,     0,    14,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    23,     0,    25,     0,    27,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    36,     0,     0,    37,    38,
       0,   469,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   115,   116,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     117,     0,     0,     0,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,   118,    60,    61,    62,    63,    64,    65,    66,
      67,    68,     0,    69,     0,    70,     0,    71,     2,     3,
       4,   114,     0,     6,     7,     0,     0,     0,     0,     0,
       0,     0,     0,    14,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    23,     0,
      25,     0,    27,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    36,
       0,     0,    37,    38,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   115,   116,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   117,     0,     0,     0,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,   118,    60,    61,    62,
      63,    64,    65,    66,    67,    68,     0,    69,     0,    70,
       0,    71,     2,     3,     4,   114,     0,     6,     7,     0,
       0,     0,     0,     0,     0,     0,     0,    14,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    23,     0,    25,     0,    27,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    36,     0,     0,    37,    38,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   413,     0,   238,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    39,    40,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
       0,    69,     0,    70,     0,    71,   239,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   238,     0,   270,
     271,   414,   415,   416,   417,   418,   419,   420,   421,   422,
     423,   424,   425,   426,   427,   428,   429,   430,   431,   432,
     433,   434,   435,   436,   437,   438,   439,   440,   441,     0,
     442,     0,   238,     0,     0,     0,     0,     0,     0,   308,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   239,   240,   241,
     242,   243,   244,   245,   246,   247,   248,   249,   250,   251,
     252,   253,   254,   255,   256,   257,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   267,   268,   269,     0,     0,
     270,   271,   239,   240,   241,   242,   243,   244,   245,   246,
     247,   248,   249,   250,   251,   252,   253,   254,   255,   256,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   238,     0,   270,   271,     0,     0,     0,
     317,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   238,     0,
       0,     0,   478,   479,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   239,   240,   241,   242,   243,   244,   245,
     246,   247,   248,   249,   250,   251,   252,   253,   254,   255,
     256,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,     0,     0,   270,   271,   239,   240,
     241,   242,   243,   244,   245,   246,   247,   248,   249,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   238,
       0,   270,   271,     0,   471,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   238,     0,     0,     0,     0,   486,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   239,
     240,   241,   242,   243,   244,   245,   246,   247,   248,   249,
     250,   251,   252,   253,   254,   255,   256,   257,   258,   259,
     260,   261,   262,   263,   264,   265,   266,   267,   268,   269,
       0,     0,   270,   271,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   238,     0,   270,   271,   318,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     238,     0,     0,     0,   558,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   239,   240,   241,   242,   243,
     244,   245,   246,   247,   248,   249,   250,   251,   252,   253,
     254,   255,   256,   257,   258,   259,   260,   261,   262,   263,
     264,   265,   266,   267,   268,   269,     0,     0,   270,   271,
     239,   240,   241,   242,   243,   244,   245,   246,   247,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   238,     0,   270,   271,   560,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   238,     0,     0,     0,
     562,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   239,   240,   241,   242,   243,   244,   245,   246,   247,
     248,   249,   250,   251,   252,   253,   254,   255,   256,   257,
     258,   259,   260,   261,   262,   263,   264,   265,   266,   267,
     268,   269,     0,     0,   270,   271,   239,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   238,     0,   270,
     271,   564,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   238,     0,     0,     0,   578,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   503,     0,     0,   239,   240,   241,
     242,   243,   244,   245,   246,   247,   248,   249,   250,   251,
     252,   253,   254,   255,   256,   257,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   267,   268,   269,   238,     0,
     270,   271,   239,   240,   241,   242,   243,   244,   245,   246,
     247,   248,   249,   250,   251,   252,   253,   254,   255,   256,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,     0,     0,   270,   271,   579,     0,     0,
     238,     0,   284,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   239,   240,
     241,   242,   243,   244,   245,   246,   247,   248,   249,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   357,
     238,   270,   271,     0,     0,     0,     0,     0,     0,     0,
     239,   240,   241,   242,   243,   244,   245,   246,   247,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,     0,   238,   270,   271,     0,     0,     0,     0,   532,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     239,   240,   241,   242,   243,   244,   245,   246,   247,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   576,     0,   270,   271,     0,   238,     0,     0,     0,
       0,     0,   239,   240,   241,   242,   243,   244,   245,   246,
     247,   248,   249,   250,   251,   252,   253,   254,   255,   256,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   238,     0,   270,   271,     0,     0,     0,
     583,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   582,   239,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   238,     0,   270,
     271,     0,     0,   239,   240,   241,   242,   243,   244,   245,
     246,   247,   248,   249,   250,   251,   252,   253,   254,   255,
     256,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   238,     0,   270,   271,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   239,   240,   241,
     242,   243,   244,   245,   246,   247,   248,   249,   250,   251,
     252,   253,   254,   255,   256,   257,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   267,   268,   269,   238,     0,
     270,   271,     0,     0,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   354,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   238,     0,   270,   271,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   240,
     241,   242,   243,   244,   245,   246,   247,   248,   249,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,     0,
       0,   270,   271,     0,     0,     0,     0,     0,     0,     0,
       0,   -72,   -72,   -72,   -72,   -72,   -72,   -72,   252,   253,
     254,   255,   256,   257,   258,   259,   260,   261,   262,   263,
     264,   265,   266,   267,   268,   269,     0,     0,   270,   271
};

static const yytype_int16 yycheck[] =
{
      19,    37,   138,   137,    23,    29,   137,    22,   155,   142,
      36,    30,    75,   130,   147,    10,    11,   220,    37,     6,
      57,   144,    57,     7,     7,   228,     7,   144,     6,    60,
     201,    59,   458,    60,    70,     8,     9,    98,   133,     7,
     135,     7,   105,   138,     7,    57,     7,   135,    57,    98,
     138,    70,    71,    57,    87,   133,     7,    27,    28,    98,
      98,    98,    42,    98,    57,    57,    57,    57,    77,    95,
      96,    97,    98,    99,   100,   136,   279,   103,   201,    57,
      57,     7,    98,     6,   201,   511,    98,   136,   514,    98,
     109,   110,    98,   112,    98,    57,   115,   116,   117,   132,
     271,   125,   126,     7,   167,    98,    98,    98,    98,   140,
     138,   138,    57,     6,   129,     6,   542,   101,     8,   143,
     101,    98,   137,    91,    92,    93,    94,   136,    96,    97,
     301,    97,   100,   101,   100,   101,    97,   100,   101,   100,
     101,    57,   161,   138,   136,   276,    98,   134,   271,   100,
     101,   577,   134,    98,   271,    41,   134,    57,    41,   292,
      86,    87,    88,    89,    90,    91,    92,    93,    94,   302,
      96,    97,    57,   101,   100,   101,    57,     6,   301,    39,
      40,   136,    98,    43,   301,     0,   133,    91,    92,    93,
      94,   139,    96,    97,   136,   214,   100,   101,    98,   218,
     403,   136,   136,   136,    41,   224,   101,   139,    59,   138,
     229,   135,    77,    98,     7,   234,    57,    98,   237,   238,
     239,   240,   241,   242,   243,   244,   245,   246,   247,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   235,   286,    39,    40,   319,    98,     6,   462,
     293,   276,   281,   466,    59,   284,   138,   291,    57,   137,
     303,    57,   138,    57,    57,    41,    57,    41,   314,   298,
     299,    77,    59,   138,     6,    77,   139,   306,   140,   136,
     140,   101,   318,    86,    87,    88,    89,    90,    91,    92,
      93,    94,   321,    96,    97,   137,   136,   100,   101,    98,
     443,   101,    98,    59,    98,    98,   450,    98,   137,   450,
      20,   468,   138,   137,    41,    25,    98,    77,     6,    80,
      30,    59,    32,   352,    34,   354,    77,    77,   357,   138,
     359,    56,    80,     8,    44,    45,   477,   138,    48,   136,
      50,     6,     0,   135,    80,   100,   136,     6,   494,   137,
     491,     7,    10,    56,   140,     8,     6,   167,    10,   541,
     477,   491,    13,   229,    22,   399,   400,   352,   235,   317,
     518,    29,   224,   518,   450,    -1,    -1,    -1,   407,    -1,
      -1,    39,    40,    -1,   409,    -1,    -1,    -1,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,    -1,    -1,    -1,   452,
      -1,    -1,    -1,    -1,   457,    -1,    -1,    75,    -1,    -1,
      -1,   467,    -1,    79,    -1,   450,    -1,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,   471,    -1,   100,   101,    -1,   105,    -1,    -1,
     479,    -1,   477,    -1,   483,    -1,    -1,   486,    -1,   502,
      -1,    -1,    -1,    -1,    -1,    -1,   491,   125,   126,    -1,
      -1,   129,    -1,    -1,    -1,    -1,   505,    -1,   507,   137,
     138,    -1,    -1,   512,    -1,   143,    -1,   516,    -1,    -1,
      -1,   520,   521,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   530,    -1,   532,    -1,    -1,    -1,   536,    -1,   167,
      -1,   557,    -1,    -1,   543,   544,   559,    -1,   547,    -1,
      -1,    -1,    -1,    -1,    -1,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,    -1,    -1,    -1,   575,   576,    -1,    -1,
      -1,    -1,    -1,    -1,   583,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    33,    -1,    35,    -1,    37,    -1,    -1,    -1,
       3,     4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,
      -1,    -1,    -1,    54,    -1,    18,    57,    58,   276,    60,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      33,    -1,    35,   291,    37,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    87,    88,    -1,    -1,
      -1,    54,    -1,    -1,    57,    58,    -1,    -1,    99,    -1,
      -1,   319,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
      -1,   132,    -1,   134,    -1,   136,    -1,    -1,    -1,   140,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,    -1,   122,
     123,   124,   125,   126,   127,   128,   129,   130,     7,   132,
      -1,   134,    -1,   136,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   399,   400,     3,     4,     5,     6,    -1,     8,     9,
      -1,   409,    -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    33,    -1,    35,    -1,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   450,    -1,    54,    -1,    -1,    57,    58,    -1,
      -1,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,   477,
      -1,   100,   101,    -1,    -1,    -1,    -1,    87,    88,    -1,
      -1,    -1,    -1,   491,    -1,    -1,   494,    -1,    -1,    99,
      -1,    -1,    -1,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,    -1,   132,    -1,   134,    -1,   136,    -1,     0,     1,
     140,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    -1,
      22,    23,    -1,    25,    -1,    -1,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    -1,    -1,
      42,    -1,    44,    45,    -1,    47,    48,    49,    50,    -1,
      -1,    -1,    54,    -1,    -1,    57,    58,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    87,    88,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,    -1,
     132,    -1,   134,     1,   136,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    12,    13,    -1,    -1,    16,    17,
      18,    19,    -1,    -1,    22,    23,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    31,    -1,    33,    -1,    35,    36,    37,
      38,    -1,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,
      -1,    49,    -1,    -1,    -1,    -1,    54,    -1,    -1,    57,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,
      88,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,
      -1,    -1,    -1,    -1,    -1,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,     7,   132,    -1,   134,    -1,   136,   137,
       3,     4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    18,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      33,    79,    35,    -1,    37,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      -1,    54,   100,   101,    57,    58,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    87,    88,   100,   101,    -1,    -1,
      -1,     7,    -1,    -1,    -1,    -1,    99,    -1,    -1,    -1,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,     7,   132,
      -1,   134,    -1,   136,   137,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    33,    -1,    35,    -1,    37,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    -1,
      96,    97,    -1,    -1,   100,   101,    54,    -1,    -1,    57,
      58,    -1,    -1,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    87,
      88,   100,   101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    99,    -1,    -1,    -1,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,     7,   132,    -1,   134,    -1,   136,   137,
       3,     4,     5,     6,    -1,     8,     9,    -1,    -1,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    -1,    22,
      23,    -1,    25,    -1,    -1,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    -1,    -1,    -1,    42,
      -1,    44,    45,    -1,    47,    48,    49,    50,    -1,    -1,
      -1,    54,    -1,    -1,    57,    58,    -1,    -1,    -1,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    87,    88,   100,   101,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,    -1,   132,
      -1,   134,    -1,   136,     3,     4,     5,     6,    -1,     8,
       9,    -1,    -1,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    -1,    22,    23,    -1,    25,    -1,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      -1,    -1,    -1,    42,    -1,    44,    45,    -1,    47,    48,
      49,    50,    -1,    -1,    -1,    54,    -1,    -1,    57,    58,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,    88,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,    -1,   132,    -1,   134,    -1,   136,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    12,    13,    14,
      -1,    16,    17,    18,    19,    20,    -1,    22,    23,    -1,
      25,    -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    -1,    -1,    -1,    42,    -1,    44,
      45,    -1,    47,    48,    49,    50,    -1,    -1,    -1,    54,
      -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    87,    88,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,    -1,   132,    -1,   134,
      -1,   136,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    12,    13,    -1,    -1,    16,    17,    18,    19,    20,
      -1,    22,    23,    -1,    25,    -1,    -1,    -1,    -1,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    -1,    -1,
      -1,    42,    -1,    44,    45,    -1,    47,    48,    49,    50,
      -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    87,    88,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
      -1,   132,    -1,   134,    -1,   136,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    12,    13,    -1,    -1,    16,
      17,    18,    19,    -1,    -1,    22,    23,    -1,    25,    -1,
      -1,    -1,    -1,    -1,    31,    -1,    33,    -1,    35,    36,
      37,    38,    -1,    -1,    -1,    42,    -1,    -1,    -1,    -1,
      47,    -1,    49,    -1,    -1,    -1,    -1,    54,    -1,    -1,
      57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      87,    88,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,    -1,   132,    -1,   134,   135,   136,
       3,     4,     5,     6,    -1,     8,     9,    -1,    -1,    12,
      13,    -1,    -1,    16,    17,    18,    19,    -1,    -1,    22,
      23,    -1,    25,    -1,    -1,    -1,    -1,    -1,    31,    -1,
      33,    -1,    35,    36,    37,    38,    -1,    -1,    -1,    42,
      -1,    -1,    -1,    -1,    47,    -1,    49,    -1,    -1,    -1,
      -1,    54,    -1,    -1,    57,    58,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    87,    88,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,    -1,   132,
      -1,   134,    -1,   136,     3,     4,     5,     6,    -1,     8,
       9,    -1,    -1,    12,    13,    -1,    -1,    16,    17,    18,
      19,    -1,    -1,    22,    23,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    31,    -1,    33,    -1,    35,    36,    37,    38,
      -1,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,
      49,    -1,    -1,    -1,    -1,    54,    -1,    -1,    57,    58,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,    88,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,    -1,   132,    -1,   134,    -1,   136,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    12,    13,    -1,
      -1,    16,    17,    18,    19,    -1,    -1,    -1,    23,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,    33,    -1,
      35,    36,    37,    -1,    -1,    -1,    -1,    42,    -1,    -1,
      -1,    -1,    47,    -1,    49,    -1,    -1,    -1,    -1,    54,
      -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    87,    88,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,    -1,   132,    -1,   134,
      -1,   136,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    12,    13,    -1,    -1,    -1,    -1,    18,    19,    -1,
      -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      31,    -1,    33,    -1,    35,    36,    37,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    47,    -1,    49,    -1,
      -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    87,    88,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
      -1,   132,    -1,   134,    -1,   136,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    12,    13,    -1,    -1,    -1,
      -1,    18,    19,    -1,    -1,    -1,    23,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    31,    -1,    33,    -1,    35,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      47,    -1,    49,    -1,    -1,    -1,    -1,    54,    -1,    -1,
      57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      87,    88,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,    -1,   132,    -1,   134,    -1,   136,
       3,     4,     5,     6,    -1,     8,     9,    -1,    -1,    12,
      13,    -1,    -1,    -1,    -1,    18,    19,    -1,    -1,    -1,
      23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,
      33,    -1,    35,    36,    37,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    47,    -1,    49,    -1,    -1,    -1,
      -1,    54,    -1,    -1,    57,    58,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    87,    88,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,    -1,   132,
      -1,   134,    -1,   136,     3,     4,     5,     6,    -1,     8,
       9,    -1,    -1,    12,    13,    -1,    -1,    -1,    -1,    18,
      19,    -1,    -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    31,    -1,    33,    -1,    35,    36,    37,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      49,    -1,    -1,    -1,    -1,    54,    -1,    -1,    57,    58,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,    88,
      -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,    -1,   132,    -1,   134,    -1,   136,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    18,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,
      35,    -1,    37,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    -1,    96,    97,    -1,    -1,   100,   101,    54,
      -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    87,    88,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    99,    -1,   101,    -1,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,    -1,   132,    -1,   134,
      -1,   136,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    33,    -1,    35,    -1,    37,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    54,    -1,    -1,    57,    58,    59,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    87,    88,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    99,    -1,
      -1,    -1,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
      -1,   132,    -1,   134,    -1,   136,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,    35,    -1,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,    -1,
      57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      87,    88,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    99,    -1,    -1,    -1,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,    -1,   132,    -1,   134,   135,   136,
       3,     4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    18,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      33,    -1,    35,    -1,    37,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    54,    -1,    -1,    57,    58,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    87,    88,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    99,    -1,    -1,    -1,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,    -1,   132,
      -1,   134,    -1,   136,     3,     4,     5,     6,    -1,     8,
       9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    33,    -1,    35,    -1,    37,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    54,    -1,    -1,    57,    58,
      -1,    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,    88,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      99,    -1,    -1,    -1,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,    -1,   132,    -1,   134,    -1,   136,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    18,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,
      35,    -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,
      -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    87,    88,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    99,    -1,    -1,    -1,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,    -1,   132,    -1,   134,
      -1,   136,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    33,    -1,    35,    -1,    37,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     6,    -1,     7,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    87,    88,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
      -1,   132,    -1,   134,    -1,   136,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,     7,    -1,   100,
     101,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,    -1,
     132,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,   140,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    -1,    -1,
     100,   101,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,     7,    -1,   100,   101,    -1,    -1,    -1,
     140,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,
      -1,    -1,   137,   138,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    -1,    -1,   100,   101,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,     7,
      -1,   100,   101,    -1,   138,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,   138,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      -1,    -1,   100,   101,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,     7,    -1,   100,   101,   137,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       7,    -1,    -1,    -1,   137,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    -1,    -1,   100,   101,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,     7,    -1,   100,   101,   137,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,
     137,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    -1,    -1,   100,   101,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,     7,    -1,   100,
     101,   137,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     7,    -1,    -1,    -1,   137,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    29,    -1,    -1,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,     7,    -1,
     100,   101,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    -1,    -1,   100,   101,   137,    -1,    -1,
       7,    -1,    51,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    56,
       7,   100,   101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    -1,     7,   100,   101,    -1,    -1,    -1,    -1,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    56,    -1,   100,   101,    -1,     7,    -1,    -1,    -1,
      -1,    -1,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,     7,    -1,   100,   101,    -1,    -1,    -1,
      51,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    29,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,     7,    -1,   100,
     101,    -1,    -1,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,     7,    -1,   100,   101,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,     7,    -1,
     100,   101,    -1,    -1,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,     7,    -1,   100,   101,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    -1,
      -1,   100,   101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    -1,    -1,   100,   101
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     4,     5,     6,     8,     9,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    22,    23,    25,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    42,
      44,    45,    47,    48,    49,    50,    54,    57,    58,    87,
      88,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   132,
     134,   136,   142,   143,   144,   145,   147,   148,   150,   151,
     156,   157,   158,   159,   160,   161,   162,   169,   172,   173,
     175,   181,   183,   184,   185,   188,   189,   197,   198,   201,
     202,   204,   205,   206,   209,   133,    98,    98,    98,    57,
      77,    98,   136,   187,     6,    87,    88,    99,   121,   180,
     182,   183,   186,   148,    42,   174,   174,    98,     6,    57,
     101,   182,     6,   172,   182,     8,    98,   134,    41,    47,
     175,   182,     6,    41,   101,   213,   215,     6,    98,   187,
      59,   179,   180,   182,    60,   140,   179,   199,   200,   181,
     181,   136,   135,   179,   182,   182,     0,   133,   146,    15,
     147,    20,    25,    30,    32,    34,    44,    45,    48,    50,
     149,   213,   139,     7,    79,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
     100,   101,    98,   187,    98,   187,    98,   187,    98,   187,
      98,   187,    98,   187,   136,    98,   187,   147,    86,   176,
     177,   178,   182,   182,   136,   137,   182,   207,   208,    57,
      98,   182,   182,   182,   136,    10,    11,   138,     7,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
     100,   101,    47,   175,    47,   175,   136,   164,   170,   171,
     172,   185,   213,   139,    51,   157,   169,   172,   210,   212,
     173,    41,    98,   164,   175,   184,   185,   214,    39,    40,
      43,   101,    98,   164,    98,    59,   138,   167,   140,   140,
     200,    60,   138,    60,   140,   182,   135,   140,   137,   145,
     147,    77,     6,   181,   181,   181,   181,   181,   181,   181,
     181,   181,   181,   181,   181,   181,   181,   181,   181,   181,
     181,   214,    98,    98,    98,    98,    98,    98,   182,    98,
     182,    59,   138,   167,    86,   137,   207,    56,   137,   138,
     167,   176,   182,   186,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   214,    41,
      41,     1,   165,   166,   168,   169,   172,    77,    59,   138,
     167,   182,    77,     6,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   132,   216,   182,   213,   139,   133,   135,   138,
     211,   175,   164,   213,   182,   182,   214,   164,   213,     6,
     190,   191,   192,   193,   194,   195,   196,   140,   140,    60,
     179,   138,   190,    98,   187,   147,   182,   136,   137,   138,
     178,   137,   182,   136,   182,    59,   138,   175,   175,   137,
     137,   138,   167,   182,    41,   172,     6,    57,   134,   152,
     153,    98,   164,    29,   203,    77,     6,    80,   135,   157,
     212,   213,    77,   163,   213,   163,    77,    59,   138,   167,
      56,    80,   138,   167,   200,   182,   135,    98,   165,   182,
     136,   182,    56,   182,   168,   173,   136,     8,   154,     6,
     155,   100,   213,    27,    28,   182,   182,    80,   163,   182,
     163,   182,   193,   194,   182,   182,     6,   140,   137,   137,
     137,   182,   137,   182,   137,   182,    59,   138,   135,   138,
     153,   163,   182,   182,   182,    56,    56,   213,   137,   137,
       8,     6,    29,    51,   182,   182,   163,   182
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   141,   142,   143,   143,   144,   144,   144,   144,   144,
     144,   145,   145,   146,   146,   147,   147,   148,   148,   148,
     148,   148,   148,   148,   148,   148,   149,   149,   149,   149,
     149,   149,   149,   149,   149,   150,   151,   151,   151,   151,
     151,   152,   152,   153,   153,   154,   154,   154,   155,   155,
     155,   156,   157,   157,   158,   158,   158,   159,   160,   160,
     160,   160,   161,   161,   162,   162,   163,   163,   164,   164,
     164,   165,   165,   166,   166,   167,   167,   168,   168,   169,
     170,   171,   171,   172,   172,   172,   173,   173,   173,   173,
     173,   173,   173,   173,   174,   174,   175,   175,   175,   175,
     175,   175,   175,   175,   176,   177,   177,   178,   178,   178,
     178,   179,   180,   180,   181,   181,   181,   181,   181,   181,
     181,   181,   181,   181,   181,   181,   181,   181,   181,   181,
     181,   181,   181,   181,   181,   181,   181,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   183,   183,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   185,
     185,   186,   186,   187,   187,   188,   188,   189,   190,   191,
     192,   192,   192,   192,   192,   192,   193,   193,   194,   195,
     196,   196,   197,   197,   198,   198,   198,   198,   199,   199,
     199,   200,   200,   201,   202,   202,   203,   203,   204,   204,
     204,   204,   204,   204,   204,   204,   204,   204,   204,   204,
     204,   204,   204,   204,   204,   204,   204,   204,   204,   204,
     204,   204,   204,   204,   204,   204,   205,   205,   206,   206,
     206,   206,   206,   206,   206,   207,   208,   208,   208,   208,
     209,   209,   210,   210,   210,   210,   211,   211,   212,   212,
     213,   213,   214,   214,   215,   215,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     2,     3,     4,     2,
       3,     1,     2,     0,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     4,     3,     5,
       7,     1,     3,     3,     4,     0,     1,     3,     0,     1,
       3,     3,     2,     4,     3,     4,     4,     2,     5,     5,
       6,     6,     7,     8,     3,     5,     0,     2,     0,     3,
       3,     0,     2,     1,     3,     0,     1,     1,     1,     3,
       2,     1,     3,     1,     6,     3,     1,     2,     3,     3,
       3,     4,     5,     5,     0,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     3,     1,     1,     2,
       2,     2,     1,     3,     1,     3,     3,     3,     3,     3,
       6,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     2,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     6,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     2,     2,     1,     1,     3,     4,
       4,     5,     1,     2,     2,     3,     1,     2,     2,     3,
       1,     2,     1,     2,     1,     2,     1,     2,     1,     2,
       1,     2,     2,     3,     1,     2,     2,     3,     1,     2,
       2,     3,     1,     2,     2,     3,     1,     2,     2,     3,
       1,     2,     2,     3,     1,     1,     2,     2,     3,     1,
       2,     2,     3,     3,     4,     2,     3,     5,     1,     2,
       1,     1,     3,     3,     3,     5,     3,     5,     3,     2,
       1,     3,     2,     3,     2,     3,     4,     3,     2,     3,
       5,     1,     3,     5,     5,     8,     0,     5,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     6,     4,     3,     4,
       1,     4,     7,     5,     8,     2,     1,     3,     3,     5,
       6,     7,     1,     1,     3,     3,     1,     1,     2,     4,
       0,     1,     1,     1,     2,     3,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1
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
  YYUSE (yyoutput);
  YYUSE (yylocationp);
  YYUSE (parm);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
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
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (parm);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
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
            vdi->e()->addAnnotation(createDocComment((yylsp[-1]),(yyvsp[-1].sValue)));
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
      { (yyval.item)=notInDatafile(&(yyloc),parm,"variable declaration") ? (yyvsp[0].item) : nullptr; }
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

  case 36: /* vardecl_item: ti_expr_and_id annotations  */
      { if ((yyvsp[-1].vardeclexpr) && (yyvsp[0].expressions1d)) (yyvsp[-1].vardeclexpr)->addAnnotations(*(yyvsp[0].expressions1d));
        if ((yyvsp[-1].vardeclexpr))
          (yyval.item) = new VarDeclI((yyloc),(yyvsp[-1].vardeclexpr));
        delete (yyvsp[0].expressions1d);
      }
    break;

  case 37: /* vardecl_item: ti_expr_and_id annotations "=" expr  */
      { if ((yyvsp[-3].vardeclexpr)) (yyvsp[-3].vardeclexpr)->e((yyvsp[0].expression));
        if ((yyvsp[-3].vardeclexpr) && (yyvsp[-2].expressions1d)) (yyvsp[-3].vardeclexpr)->addAnnotations(*(yyvsp[-2].expressions1d));
        if ((yyvsp[-3].vardeclexpr))
          (yyval.item) = new VarDeclI((yyloc),(yyvsp[-3].vardeclexpr));
        delete (yyvsp[-2].expressions1d);
      }
    break;

  case 38: /* vardecl_item: "enum" "identifier" annotations  */
      {
        TypeInst* ti = new TypeInst((yyloc),Type::parsetint());
        ti->setIsEnum(true);
        VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[-1].sValue));
        if ((yyvsp[-1].sValue) && (yyvsp[0].expressions1d))
          vd->addAnnotations(*(yyvsp[0].expressions1d));
        free((yyvsp[-1].sValue));
        (yyval.item) = new VarDeclI((yyloc),vd);
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
            e = new Call((yyloc), ASTString("enumFromConstructors"), {al});
          }
          VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[-3].sValue),e);
          (yyval.item) = new VarDeclI((yyloc),vd);
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
        Call* sl = new Call((yyloc), constants().ids.anonEnumFromStrings, args);
        VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[-5].sValue),sl);
        if ((yyvsp[-5].sValue) && (yyvsp[-4].expressions1d))
          vd->addAnnotations(*(yyvsp[-4].expressions1d));
        free((yyvsp[-5].sValue));
        delete (yyvsp[-1].expressions1d);
        (yyval.item) = new VarDeclI((yyloc),vd);
      }
    break;

  case 41: /* enum_init: enum_construct  */
      {
        (yyval.expressions1d) = new std::vector<Expression*>({(yyvsp[0].expression)});
      }
    break;

  case 42: /* enum_init: enum_init "++" enum_construct  */
      {
        (yyval.expressions1d) = (yyvsp[-2].expressions1d);
        if ((yyval.expressions1d)) {
          (yyval.expressions1d)->push_back((yyvsp[0].expression));
        }
      }
    break;

  case 43: /* enum_construct: '{' enum_id_list '}'  */
      {
        (yyval.expression) = new SetLit((yyloc), *(yyvsp[-1].expressions1d));
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 44: /* enum_construct: "identifier" '(' expr ')'  */
      {
        vector<Expression*> args({(yyvsp[-1].expression)});
        (yyval.expression) = new Call((yyloc), ASTString((yyvsp[-3].sValue)), args);
        free((yyvsp[-3].sValue));
      }
    break;

  case 45: /* string_lit_list: %empty  */
      { (yyval.expressions1d) = new std::vector<Expression*>(); }
    break;

  case 46: /* string_lit_list: "string literal"  */
      { (yyval.expressions1d) = new std::vector<Expression*>();
        (yyval.expressions1d)->push_back(new StringLit((yyloc), (yyvsp[0].sValue))); free((yyvsp[0].sValue));
      }
    break;

  case 47: /* string_lit_list: string_lit_list ',' "string literal"  */
      { (yyval.expressions1d) = (yyvsp[-2].expressions1d);
        if ((yyval.expressions1d)) (yyval.expressions1d)->push_back(new StringLit((yyloc), (yyvsp[0].sValue)));
        free((yyvsp[0].sValue));
      }
    break;

  case 48: /* enum_id_list: %empty  */
      { (yyval.expressions1d) = new std::vector<Expression*>(); }
    break;

  case 49: /* enum_id_list: "identifier"  */
      { (yyval.expressions1d) = new std::vector<Expression*>();
        (yyval.expressions1d)->push_back(new Id((yyloc),(yyvsp[0].sValue),nullptr)); free((yyvsp[0].sValue));
      }
    break;

  case 50: /* enum_id_list: enum_id_list ',' "identifier"  */
      { (yyval.expressions1d) = (yyvsp[-2].expressions1d); if ((yyval.expressions1d)) (yyval.expressions1d)->push_back(new Id((yyloc),(yyvsp[0].sValue),nullptr)); free((yyvsp[0].sValue)); }
    break;

  case 51: /* assign_item: "identifier" "=" expr  */
      { (yyval.item) = new AssignI((yyloc),(yyvsp[-2].sValue),(yyvsp[0].expression));
        free((yyvsp[-2].sValue));
      }
    break;

  case 52: /* constraint_item: "constraint" expr  */
      { (yyval.item) = new ConstraintI((yyloc),(yyvsp[0].expression));}
    break;

  case 53: /* constraint_item: "constraint" "::" string_expr expr  */
      { (yyval.item) = new ConstraintI((yyloc),(yyvsp[0].expression));
        if ((yyvsp[0].expression) && (yyvsp[-1].expression))
          (yyval.item)->cast<ConstraintI>()->e()->ann().add(new Call((yylsp[-2]), ASTString("mzn_constraint_name"), {(yyvsp[-1].expression)}));
      }
    break;

  case 54: /* solve_item: "solve" annotations "satisfy"  */
      { (yyval.item) = SolveI::sat((yyloc));
        if ((yyval.item) && (yyvsp[-1].expressions1d)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[-1].expressions1d));
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 55: /* solve_item: "solve" annotations "minimize" expr  */
      { (yyval.item) = SolveI::min((yyloc),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-2].expressions1d)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[-2].expressions1d));
        delete (yyvsp[-2].expressions1d);
      }
    break;

  case 56: /* solve_item: "solve" annotations "maximize" expr  */
      { (yyval.item) = SolveI::max((yyloc),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-2].expressions1d)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[-2].expressions1d));
        delete (yyvsp[-2].expressions1d);
      }
    break;

  case 57: /* output_item: "output" expr  */
      { (yyval.item) = new OutputI((yyloc),(yyvsp[0].expression));}
    break;

  case 58: /* predicate_item: "predicate" "identifier" params annotations operation_item_tail  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[-2].vardeclexprs)) (yyval.item) = new FunctionI((yyloc),(yyvsp[-3].sValue),new TypeInst((yyloc),
                                   Type::varbool()),*(yyvsp[-2].vardeclexprs),(yyvsp[0].expression),pp->isSTDLib);
        if ((yyval.item) && (yyvsp[-1].expressions1d)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expressions1d));
        free((yyvsp[-3].sValue));
        delete (yyvsp[-2].vardeclexprs);
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 59: /* predicate_item: "test" "identifier" params annotations operation_item_tail  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[-2].vardeclexprs)) (yyval.item) = new FunctionI((yyloc),(yyvsp[-3].sValue),new TypeInst((yyloc),
                                   Type::parbool()),*(yyvsp[-2].vardeclexprs),(yyvsp[0].expression),pp->isSTDLib);
        if ((yyval.item) && (yyvsp[-1].expressions1d)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expressions1d));
        free((yyvsp[-3].sValue));
        delete (yyvsp[-2].vardeclexprs);
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 60: /* predicate_item: "predicate" "identifier" "^-1" params annotations operation_item_tail  */
      { if ((yyvsp[-2].vardeclexprs)) (yyval.item) = new FunctionI((yyloc),std::string((yyvsp[-4].sValue))+"⁻¹",new TypeInst((yyloc),
                                   Type::varbool()),*(yyvsp[-2].vardeclexprs),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-1].expressions1d)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expressions1d));
        free((yyvsp[-4].sValue));
        delete (yyvsp[-2].vardeclexprs);
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 61: /* predicate_item: "test" "identifier" "^-1" params annotations operation_item_tail  */
      { if ((yyvsp[-2].vardeclexprs)) (yyval.item) = new FunctionI((yyloc),std::string((yyvsp[-4].sValue))+"⁻¹",new TypeInst((yyloc),
                                   Type::parbool()),*(yyvsp[-2].vardeclexprs),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-1].expressions1d)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expressions1d));
        free((yyvsp[-4].sValue));
        delete (yyvsp[-2].vardeclexprs);
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 62: /* function_item: "function" ti_expr ':' id_or_quoted_op params annotations operation_item_tail  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[-2].vardeclexprs)) (yyval.item) = new FunctionI((yyloc),(yyvsp[-3].sValue),(yyvsp[-5].tiexpr),*(yyvsp[-2].vardeclexprs),(yyvsp[0].expression),pp->isSTDLib);
        if ((yyval.item) && (yyvsp[-1].expressions1d)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expressions1d));
        free((yyvsp[-3].sValue));
        delete (yyvsp[-2].vardeclexprs);
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 63: /* function_item: ti_expr ':' "identifier" '(' params_list ')' annotations operation_item_tail  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[-3].vardeclexprs)) (yyval.item) = new FunctionI((yyloc),(yyvsp[-5].sValue),(yyvsp[-7].tiexpr),*(yyvsp[-3].vardeclexprs),(yyvsp[0].expression),pp->isSTDLib);
        if ((yyval.item) && (yyvsp[-1].expressions1d)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expressions1d));
        free((yyvsp[-5].sValue));
        delete (yyvsp[-3].vardeclexprs);
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 64: /* annotation_item: "annotation" "identifier" params  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        TypeInst* ti=new TypeInst((yylsp[-2]),Type::ann());
        if ((yyvsp[0].vardeclexprs)==nullptr || (yyvsp[0].vardeclexprs)->empty()) {
          VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[-1].sValue));
          (yyval.item) = new VarDeclI((yyloc),vd);
        } else {
          (yyval.item) = new FunctionI((yyloc),(yyvsp[-1].sValue),ti,*(yyvsp[0].vardeclexprs),nullptr,pp->isSTDLib);
        }
        free((yyvsp[-1].sValue));
        delete (yyvsp[0].vardeclexprs);
      }
    break;

  case 65: /* annotation_item: "annotation" "identifier" params "=" expr  */
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        TypeInst* ti=new TypeInst((yylsp[-4]),Type::ann());
        if ((yyvsp[-2].vardeclexprs)) (yyval.item) = new FunctionI((yyloc),(yyvsp[-3].sValue),ti,*(yyvsp[-2].vardeclexprs),(yyvsp[0].expression),pp->isSTDLib);
        delete (yyvsp[-2].vardeclexprs);
      }
    break;

  case 66: /* operation_item_tail: %empty  */
      { (yyval.expression)=nullptr; }
    break;

  case 67: /* operation_item_tail: "=" expr  */
      { (yyval.expression)=(yyvsp[0].expression); }
    break;

  case 68: /* params: %empty  */
      { (yyval.vardeclexprs)=new vector<VarDecl*>(); }
    break;

  case 69: /* params: '(' params_list ')'  */
      { (yyval.vardeclexprs)=(yyvsp[-1].vardeclexprs); }
    break;

  case 70: /* params: '(' error ')'  */
      { (yyval.vardeclexprs)=new vector<VarDecl*>(); }
    break;

  case 71: /* params_list: %empty  */
      { (yyval.vardeclexprs)=new vector<VarDecl*>(); }
    break;

  case 72: /* params_list: params_list_head comma_or_none  */
      { (yyval.vardeclexprs)=(yyvsp[-1].vardeclexprs); }
    break;

  case 73: /* params_list_head: ti_expr_and_id_or_anon  */
      { (yyval.vardeclexprs)=new vector<VarDecl*>();
        if ((yyvsp[0].vardeclexpr)) (yyvsp[0].vardeclexpr)->toplevel(false);
        if ((yyvsp[0].vardeclexpr)) (yyval.vardeclexprs)->push_back((yyvsp[0].vardeclexpr)); }
    break;

  case 74: /* params_list_head: params_list_head ',' ti_expr_and_id_or_anon  */
      { (yyval.vardeclexprs)=(yyvsp[-2].vardeclexprs);
        if ((yyvsp[0].vardeclexpr)) (yyvsp[0].vardeclexpr)->toplevel(false);
        if ((yyvsp[-2].vardeclexprs) && (yyvsp[0].vardeclexpr)) (yyvsp[-2].vardeclexprs)->push_back((yyvsp[0].vardeclexpr)); }
    break;

  case 77: /* ti_expr_and_id_or_anon: ti_expr_and_id  */
      { (yyval.vardeclexpr)=(yyvsp[0].vardeclexpr); }
    break;

  case 78: /* ti_expr_and_id_or_anon: ti_expr  */
      { if ((yyvsp[0].tiexpr)) (yyval.vardeclexpr)=new VarDecl((yyloc), (yyvsp[0].tiexpr), ""); }
    break;

  case 79: /* ti_expr_and_id: ti_expr ':' "identifier"  */
      { if ((yyvsp[-2].tiexpr) && (yyvsp[0].sValue)) {
          Id* ident = new Id((yylsp[0]), (yyvsp[0].sValue), nullptr);
          (yyval.vardeclexpr) = new VarDecl((yyloc), (yyvsp[-2].tiexpr), ident);
        }
        free((yyvsp[0].sValue));
      }
    break;

  case 80: /* ti_expr_list: ti_expr_list_head comma_or_none  */
      { (yyval.tiexprs)=(yyvsp[-1].tiexprs); }
    break;

  case 81: /* ti_expr_list_head: ti_expr  */
      { (yyval.tiexprs)=new vector<TypeInst*>(); (yyval.tiexprs)->push_back((yyvsp[0].tiexpr)); }
    break;

  case 82: /* ti_expr_list_head: ti_expr_list_head ',' ti_expr  */
      { (yyval.tiexprs)=(yyvsp[-2].tiexprs); if ((yyvsp[-2].tiexprs) && (yyvsp[0].tiexpr)) (yyvsp[-2].tiexprs)->push_back((yyvsp[0].tiexpr)); }
    break;

  case 84: /* ti_expr: "array" "[" ti_expr_list "]" "of" base_ti_expr  */
      {
        (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr) && (yyvsp[-3].tiexprs)) (yyval.tiexpr)->setRanges(*(yyvsp[-3].tiexprs));
        delete (yyvsp[-3].tiexprs);
      }
    break;

  case 85: /* ti_expr: "list" "of" base_ti_expr  */
      {
        (yyval.tiexpr) = (yyvsp[0].tiexpr);
        std::vector<TypeInst*> ti(1);
        ti[0] = new TypeInst((yyloc),Type::parint());
        if ((yyval.tiexpr)) (yyval.tiexpr)->setRanges(ti);
      }
    break;

  case 86: /* base_ti_expr: base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
      }
    break;

  case 87: /* base_ti_expr: "opt" base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      }
    break;

  case 88: /* base_ti_expr: "par" opt_opt base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr) && (yyvsp[-1].bValue)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      }
    break;

  case 89: /* base_ti_expr: "var" opt_opt base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ti(Type::TI_VAR);
          if ((yyvsp[-1].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      }
    break;

  case 90: /* base_ti_expr: "set" "of" base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.st(Type::ST_SET);
          (yyval.tiexpr)->type(tt);
        }
      }
    break;

  case 91: /* base_ti_expr: "opt" "set" "of" base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.st(Type::ST_SET);
          tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      }
    break;

  case 92: /* base_ti_expr: "par" opt_opt "set" "of" base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.st(Type::ST_SET);
          if ((yyvsp[-3].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      }
    break;

  case 93: /* base_ti_expr: "var" opt_opt "set" "of" base_ti_expr_tail  */
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ti(Type::TI_VAR);
          tt.st(Type::ST_SET);
          if ((yyvsp[-3].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      }
    break;

  case 94: /* opt_opt: %empty  */
      { (yyval.bValue) = false; }
    break;

  case 95: /* opt_opt: "opt"  */
      { (yyval.bValue) = true; }
    break;

  case 96: /* base_ti_expr_tail: "int"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::parint()); }
    break;

  case 97: /* base_ti_expr_tail: "bool"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::parbool()); }
    break;

  case 98: /* base_ti_expr_tail: "float"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::parfloat()); }
    break;

  case 99: /* base_ti_expr_tail: "string"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::parstring()); }
    break;

  case 100: /* base_ti_expr_tail: "ann"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::ann()); }
    break;

  case 101: /* base_ti_expr_tail: set_expr  */
        { if ((yyvsp[0].expression)) (yyval.tiexpr) = new TypeInst((yyloc),Type(),(yyvsp[0].expression)); }
    break;

  case 102: /* base_ti_expr_tail: "type-inst identifier"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::top(),
                         new TIId((yyloc), (yyvsp[0].sValue)));
        free((yyvsp[0].sValue));
      }
    break;

  case 103: /* base_ti_expr_tail: "type-inst enum identifier"  */
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::parint(),
          new TIId((yyloc), (yyvsp[0].sValue)));
          free((yyvsp[0].sValue));
      }
    break;

  case 105: /* array_access_expr_list_head: array_access_expr  */
      { (yyval.expressions1d)=new std::vector<MiniZinc::Expression*>; (yyval.expressions1d)->push_back((yyvsp[0].expression)); }
    break;

  case 106: /* array_access_expr_list_head: array_access_expr_list_head ',' array_access_expr  */
      { (yyval.expressions1d)=(yyvsp[-2].expressions1d); if ((yyval.expressions1d) && (yyvsp[0].expression)) (yyval.expressions1d)->push_back((yyvsp[0].expression)); }
    break;

  case 107: /* array_access_expr: expr  */
      { (yyval.expression) = (yyvsp[0].expression); }
    break;

  case 108: /* array_access_expr: ".."  */
      { (yyval.expression)=new SetLit((yyloc), IntSetVal::a(-IntVal::infinity(),IntVal::infinity())); }
    break;

  case 109: /* array_access_expr: ".." expr  */
      { if ((yyvsp[0].expression)==nullptr) {
          (yyval.expression) = nullptr;
        } else if ((yyvsp[0].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a(-IntVal::infinity(),(yyvsp[0].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), IntLit::a(-IntVal::infinity()), BOT_DOTDOT, (yyvsp[0].expression));
        }
      }
    break;

  case 110: /* array_access_expr: expr ".."  */
      { if ((yyvsp[-1].expression)==nullptr) {
          (yyval.expression) = nullptr;
        } else if ((yyvsp[-1].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[-1].expression)->cast<IntLit>()->v(),IntVal::infinity()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-1].expression), BOT_DOTDOT, IntLit::a(IntVal::infinity()));
        }
      }
    break;

  case 112: /* expr_list_head: expr  */
      { (yyval.expressions1d)=new std::vector<MiniZinc::Expression*>; (yyval.expressions1d)->push_back((yyvsp[0].expression)); }
    break;

  case 113: /* expr_list_head: expr_list_head ',' expr  */
      { (yyval.expressions1d)=(yyvsp[-2].expressions1d); if ((yyval.expressions1d) && (yyvsp[0].expression)) (yyval.expressions1d)->push_back((yyvsp[0].expression)); }
    break;

  case 115: /* set_expr: set_expr "::" annotation_expr  */
      { if ((yyvsp[-2].expression) && (yyvsp[0].expression)) (yyvsp[-2].expression)->addAnnotation((yyvsp[0].expression)); (yyval.expression)=(yyvsp[-2].expression); }
    break;

  case 116: /* set_expr: set_expr "union" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_UNION, (yyvsp[0].expression)); }
    break;

  case 117: /* set_expr: set_expr "diff" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIFF, (yyvsp[0].expression)); }
    break;

  case 118: /* set_expr: set_expr "symdiff" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SYMDIFF, (yyvsp[0].expression)); }
    break;

  case 119: /* set_expr: set_expr ".." set_expr  */
      { if ((yyvsp[-2].expression)==nullptr || (yyvsp[0].expression)==nullptr) {
          (yyval.expression) = nullptr;
        } else if ((yyvsp[-2].expression)->isa<IntLit>() && (yyvsp[0].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[-2].expression)->cast<IntLit>()->v(),(yyvsp[0].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DOTDOT, (yyvsp[0].expression));
        }
      }
    break;

  case 120: /* set_expr: "'..'" '(' expr ',' expr ')'  */
      { if ((yyvsp[-3].expression)==nullptr || (yyvsp[-1].expression)==nullptr) {
          (yyval.expression) = nullptr;
        } else if ((yyvsp[-3].expression)->isa<IntLit>() && (yyvsp[-1].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[-3].expression)->cast<IntLit>()->v(),(yyvsp[-1].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-3].expression), BOT_DOTDOT, (yyvsp[-1].expression));
        }
      }
    break;

  case 121: /* set_expr: set_expr "intersect" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_INTERSECT, (yyvsp[0].expression)); }
    break;

  case 122: /* set_expr: set_expr "++" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_PLUSPLUS, (yyvsp[0].expression)); }
    break;

  case 123: /* set_expr: set_expr "+" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_PLUS, (yyvsp[0].expression)); }
    break;

  case 124: /* set_expr: set_expr "-" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MINUS, (yyvsp[0].expression)); }
    break;

  case 125: /* set_expr: set_expr "*" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MULT, (yyvsp[0].expression)); }
    break;

  case 126: /* set_expr: set_expr "/" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIV, (yyvsp[0].expression)); }
    break;

  case 127: /* set_expr: set_expr "div" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IDIV, (yyvsp[0].expression)); }
    break;

  case 128: /* set_expr: set_expr "mod" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MOD, (yyvsp[0].expression)); }
    break;

  case 129: /* set_expr: set_expr "^" set_expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_POW, (yyvsp[0].expression)); }
    break;

  case 130: /* set_expr: set_expr "~+" set_expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~+"), args);
      }
    break;

  case 131: /* set_expr: set_expr "~-" set_expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~-"), args);
      }
    break;

  case 132: /* set_expr: set_expr "~*" set_expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~*"), args);
      }
    break;

  case 133: /* set_expr: set_expr "~=" set_expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~="), args);
      }
    break;

  case 134: /* set_expr: set_expr "quoted identifier" set_expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), (yyvsp[-1].sValue), args);
        free((yyvsp[-1].sValue));
      }
    break;

  case 135: /* set_expr: "+" set_expr  */
      { (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[0].expression)); }
    break;

  case 136: /* set_expr: "-" set_expr  */
      { if ((yyvsp[0].expression) && (yyvsp[0].expression)->isa<IntLit>()) {
          (yyval.expression) = IntLit::a(-(yyvsp[0].expression)->cast<IntLit>()->v());
        } else if ((yyvsp[0].expression) && (yyvsp[0].expression)->isa<FloatLit>()) {
          (yyval.expression) = FloatLit::a(-(yyvsp[0].expression)->cast<FloatLit>()->v());
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_MINUS, (yyvsp[0].expression));
        }
      }
    break;

  case 138: /* expr: expr "::" annotation_expr  */
      { if ((yyvsp[-2].expression) && (yyvsp[0].expression)) (yyvsp[-2].expression)->addAnnotation((yyvsp[0].expression)); (yyval.expression)=(yyvsp[-2].expression); }
    break;

  case 139: /* expr: expr "<->" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_EQUIV, (yyvsp[0].expression)); }
    break;

  case 140: /* expr: expr "->" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IMPL, (yyvsp[0].expression)); }
    break;

  case 141: /* expr: expr "<-" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_RIMPL, (yyvsp[0].expression)); }
    break;

  case 142: /* expr: expr "\\/" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_OR, (yyvsp[0].expression)); }
    break;

  case 143: /* expr: expr "xor" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_XOR, (yyvsp[0].expression)); }
    break;

  case 144: /* expr: expr "/\\" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_AND, (yyvsp[0].expression)); }
    break;

  case 145: /* expr: expr "<" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_LE, (yyvsp[0].expression)); }
    break;

  case 146: /* expr: expr ">" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_GR, (yyvsp[0].expression)); }
    break;

  case 147: /* expr: expr "<=" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_LQ, (yyvsp[0].expression)); }
    break;

  case 148: /* expr: expr ">=" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_GQ, (yyvsp[0].expression)); }
    break;

  case 149: /* expr: expr "=" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_EQ, (yyvsp[0].expression)); }
    break;

  case 150: /* expr: expr "!=" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_NQ, (yyvsp[0].expression)); }
    break;

  case 151: /* expr: expr "in" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IN, (yyvsp[0].expression)); }
    break;

  case 152: /* expr: expr "subset" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SUBSET, (yyvsp[0].expression)); }
    break;

  case 153: /* expr: expr "superset" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SUPERSET, (yyvsp[0].expression)); }
    break;

  case 154: /* expr: expr "union" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_UNION, (yyvsp[0].expression)); }
    break;

  case 155: /* expr: expr "diff" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIFF, (yyvsp[0].expression)); }
    break;

  case 156: /* expr: expr "symdiff" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SYMDIFF, (yyvsp[0].expression)); }
    break;

  case 157: /* expr: expr ".." expr  */
      { if ((yyvsp[-2].expression)==nullptr || (yyvsp[0].expression)==nullptr) {
          (yyval.expression) = nullptr;
        } else if ((yyvsp[-2].expression)->isa<IntLit>() && (yyvsp[0].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[-2].expression)->cast<IntLit>()->v(),(yyvsp[0].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DOTDOT, (yyvsp[0].expression));
        }
      }
    break;

  case 158: /* expr: "'..'" '(' expr ',' expr ')'  */
      { if ((yyvsp[-3].expression)==nullptr || (yyvsp[-1].expression)==nullptr) {
          (yyval.expression) = nullptr;
        } else if ((yyvsp[-3].expression)->isa<IntLit>() && (yyvsp[-1].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[-3].expression)->cast<IntLit>()->v(),(yyvsp[-1].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-3].expression), BOT_DOTDOT, (yyvsp[-1].expression));
        }
      }
    break;

  case 159: /* expr: expr "intersect" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_INTERSECT, (yyvsp[0].expression)); }
    break;

  case 160: /* expr: expr "++" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_PLUSPLUS, (yyvsp[0].expression)); }
    break;

  case 161: /* expr: expr "+" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_PLUS, (yyvsp[0].expression)); }
    break;

  case 162: /* expr: expr "-" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MINUS, (yyvsp[0].expression)); }
    break;

  case 163: /* expr: expr "*" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MULT, (yyvsp[0].expression)); }
    break;

  case 164: /* expr: expr "/" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIV, (yyvsp[0].expression)); }
    break;

  case 165: /* expr: expr "div" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IDIV, (yyvsp[0].expression)); }
    break;

  case 166: /* expr: expr "mod" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MOD, (yyvsp[0].expression)); }
    break;

  case 167: /* expr: expr "^" expr  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_POW, (yyvsp[0].expression)); }
    break;

  case 168: /* expr: expr "~+" expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~+"), args);
      }
    break;

  case 169: /* expr: expr "~-" expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~-"), args);
      }
    break;

  case 170: /* expr: expr "~*" expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~*"), args);
      }
    break;

  case 171: /* expr: expr "~=" expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~="), args);
      }
    break;

  case 172: /* expr: expr "quoted identifier" expr  */
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), (yyvsp[-1].sValue), args);
        free((yyvsp[-1].sValue));
      }
    break;

  case 173: /* expr: "not" expr  */
      { (yyval.expression)=new UnOp((yyloc), UOT_NOT, (yyvsp[0].expression)); }
    break;

  case 174: /* expr: "+" expr  */
      { if (((yyvsp[0].expression) && (yyvsp[0].expression)->isa<IntLit>()) || ((yyvsp[0].expression) && (yyvsp[0].expression)->isa<FloatLit>())) {
          (yyval.expression) = (yyvsp[0].expression);
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[0].expression));
        }
      }
    break;

  case 175: /* expr: "-" expr  */
      { if ((yyvsp[0].expression) && (yyvsp[0].expression)->isa<IntLit>()) {
          (yyval.expression) = IntLit::a(-(yyvsp[0].expression)->cast<IntLit>()->v());
        } else if ((yyvsp[0].expression) && (yyvsp[0].expression)->isa<FloatLit>()) {
          (yyval.expression) = FloatLit::a(-(yyvsp[0].expression)->cast<FloatLit>()->v());
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_MINUS, (yyvsp[0].expression));
        }
      }
    break;

  case 176: /* expr_atom_head: expr_atom_head_nonstring  */
      { (yyval.expression)=(yyvsp[0].expression); }
    break;

  case 177: /* expr_atom_head: string_expr  */
      { (yyval.expression)=(yyvsp[0].expression); }
    break;

  case 178: /* expr_atom_head_nonstring: '(' expr ')'  */
      { (yyval.expression)=(yyvsp[-1].expression); }
    break;

  case 179: /* expr_atom_head_nonstring: '(' expr ')' array_access_tail  */
      { if ((yyvsp[0].expressions2d)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[0].expressions2d)); delete (yyvsp[0].expressions2d); }
    break;

  case 180: /* expr_atom_head_nonstring: '(' expr ')' "^-1"  */
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 181: /* expr_atom_head_nonstring: '(' expr ')' array_access_tail "^-1"  */
      { if ((yyvsp[-1].expressions2d)) (yyval.expression)=new BinOp((yyloc),createArrayAccess((yyloc), (yyvsp[-3].expression), *(yyvsp[-1].expressions2d)), BOT_POW, IntLit::a(-1)); delete (yyvsp[-1].expressions2d); }
    break;

  case 182: /* expr_atom_head_nonstring: "identifier"  */
      { (yyval.expression)=new Id((yyloc), (yyvsp[0].sValue), nullptr); free((yyvsp[0].sValue)); }
    break;

  case 183: /* expr_atom_head_nonstring: "identifier" array_access_tail  */
      { if ((yyvsp[0].expressions2d)) (yyval.expression)=createArrayAccess((yyloc), new Id((yylsp[-1]),(yyvsp[-1].sValue),nullptr), *(yyvsp[0].expressions2d));
        free((yyvsp[-1].sValue)); delete (yyvsp[0].expressions2d); }
    break;

  case 184: /* expr_atom_head_nonstring: "identifier" "^-1"  */
      { (yyval.expression)=new BinOp((yyloc),new Id((yyloc), (yyvsp[-1].sValue), nullptr), BOT_POW, IntLit::a(-1)); free((yyvsp[-1].sValue)); }
    break;

  case 185: /* expr_atom_head_nonstring: "identifier" array_access_tail "^-1"  */
      { if ((yyvsp[-1].expressions2d)) (yyval.expression)=new BinOp((yyloc),createArrayAccess((yyloc), new Id((yylsp[-2]),(yyvsp[-2].sValue),nullptr), *(yyvsp[-1].expressions2d)), BOT_POW, IntLit::a(-1));
        free((yyvsp[-2].sValue)); delete (yyvsp[-1].expressions2d); }
    break;

  case 186: /* expr_atom_head_nonstring: "_"  */
      { (yyval.expression)=new AnonVar((yyloc)); }
    break;

  case 187: /* expr_atom_head_nonstring: "_" array_access_tail  */
      { if ((yyvsp[0].expressions2d)) (yyval.expression)=createArrayAccess((yyloc), new AnonVar((yyloc)), *(yyvsp[0].expressions2d));
        delete (yyvsp[0].expressions2d); }
    break;

  case 188: /* expr_atom_head_nonstring: "_" "^-1"  */
      { (yyval.expression)=new BinOp((yyloc),new AnonVar((yyloc)), BOT_POW, IntLit::a(-1)); }
    break;

  case 189: /* expr_atom_head_nonstring: "_" array_access_tail "^-1"  */
      { if ((yyvsp[-1].expressions2d)) (yyval.expression)=new BinOp((yyloc),createArrayAccess((yyloc), new AnonVar((yyloc)), *(yyvsp[-1].expressions2d)), BOT_POW, IntLit::a(-1));
        delete (yyvsp[-1].expressions2d); }
    break;

  case 190: /* expr_atom_head_nonstring: "bool literal"  */
      { (yyval.expression)=constants().boollit(((yyvsp[0].iValue)!=0)); }
    break;

  case 191: /* expr_atom_head_nonstring: "bool literal" "^-1"  */
      { (yyval.expression)=new BinOp((yyloc),constants().boollit(((yyvsp[-1].iValue)!=0)), BOT_POW, IntLit::a(-1)); }
    break;

  case 192: /* expr_atom_head_nonstring: "integer literal"  */
      { (yyval.expression)=IntLit::a((yyvsp[0].iValue)); }
    break;

  case 193: /* expr_atom_head_nonstring: "integer literal" "^-1"  */
      { (yyval.expression)=new BinOp((yyloc),IntLit::a((yyvsp[-1].iValue)), BOT_POW, IntLit::a(-1)); }
    break;

  case 194: /* expr_atom_head_nonstring: "infinity"  */
      { (yyval.expression)=IntLit::a(IntVal::infinity()); }
    break;

  case 195: /* expr_atom_head_nonstring: "infinity" "^-1"  */
      { (yyval.expression)=new BinOp((yyloc),IntLit::a(IntVal::infinity()), BOT_POW, IntLit::a(-1)); }
    break;

  case 196: /* expr_atom_head_nonstring: "float literal"  */
      { (yyval.expression)=FloatLit::a((yyvsp[0].dValue)); }
    break;

  case 197: /* expr_atom_head_nonstring: "float literal" "^-1"  */
      { (yyval.expression)=new BinOp((yyloc),FloatLit::a((yyvsp[-1].dValue)), BOT_POW, IntLit::a(-1)); }
    break;

  case 198: /* expr_atom_head_nonstring: "<>"  */
      { (yyval.expression)=constants().absent; }
    break;

  case 199: /* expr_atom_head_nonstring: "<>" "^-1"  */
      { (yyval.expression)=constants().absent; }
    break;

  case 201: /* expr_atom_head_nonstring: set_literal array_access_tail  */
      { if ((yyvsp[0].expressions2d)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions2d));
        delete (yyvsp[0].expressions2d); }
    break;

  case 202: /* expr_atom_head_nonstring: set_literal "^-1"  */
      { (yyval.expression) = new BinOp((yyloc),(yyvsp[-1].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 203: /* expr_atom_head_nonstring: set_literal array_access_tail "^-1"  */
      { if ((yyvsp[-1].expressions2d)) (yyval.expression)=new BinOp((yyloc),createArrayAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions2d)), BOT_POW, IntLit::a(-1));
        delete (yyvsp[-1].expressions2d); }
    break;

  case 205: /* expr_atom_head_nonstring: set_comp array_access_tail  */
      { if ((yyvsp[0].expressions2d)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions2d));
        delete (yyvsp[0].expressions2d); }
    break;

  case 206: /* expr_atom_head_nonstring: set_comp "^-1"  */
      { (yyval.expression) = new BinOp((yyloc),(yyvsp[-1].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 207: /* expr_atom_head_nonstring: set_comp array_access_tail "^-1"  */
      { if ((yyvsp[-1].expressions2d)) (yyval.expression)=new BinOp((yyloc),createArrayAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions2d)), BOT_POW, IntLit::a(-1));
        delete (yyvsp[-1].expressions2d); }
    break;

  case 209: /* expr_atom_head_nonstring: simple_array_literal array_access_tail  */
      { if ((yyvsp[0].expressions2d)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions2d));
        delete (yyvsp[0].expressions2d); }
    break;

  case 210: /* expr_atom_head_nonstring: simple_array_literal "^-1"  */
      { (yyval.expression) = new BinOp((yyloc),(yyvsp[-1].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 211: /* expr_atom_head_nonstring: simple_array_literal array_access_tail "^-1"  */
      { if ((yyvsp[-1].expressions2d)) (yyval.expression)=new BinOp((yyloc),createArrayAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions2d)), BOT_POW, IntLit::a(-1));
        delete (yyvsp[-1].expressions2d); }
    break;

  case 213: /* expr_atom_head_nonstring: simple_array_literal_2d array_access_tail  */
      { if ((yyvsp[0].expressions2d)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions2d));
        delete (yyvsp[0].expressions2d); }
    break;

  case 214: /* expr_atom_head_nonstring: simple_array_literal_2d "^-1"  */
      { (yyval.expression) = new BinOp((yyloc),(yyvsp[-1].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 215: /* expr_atom_head_nonstring: simple_array_literal_2d array_access_tail "^-1"  */
      { if ((yyvsp[-1].expressions2d)) (yyval.expression)=new BinOp((yyloc),createArrayAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions2d)), BOT_POW, IntLit::a(-1));
        delete (yyvsp[-1].expressions2d); }
    break;

  case 217: /* expr_atom_head_nonstring: simple_array_comp array_access_tail  */
      { if ((yyvsp[0].expressions2d)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions2d));
        delete (yyvsp[0].expressions2d); }
    break;

  case 218: /* expr_atom_head_nonstring: simple_array_comp "^-1"  */
      { (yyval.expression) = new BinOp((yyloc),(yyvsp[-1].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 219: /* expr_atom_head_nonstring: simple_array_comp array_access_tail "^-1"  */
      { if ((yyvsp[-1].expressions2d)) (yyval.expression)=new BinOp((yyloc),createArrayAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions2d)), BOT_POW, IntLit::a(-1));
        delete (yyvsp[-1].expressions2d); }
    break;

  case 221: /* expr_atom_head_nonstring: if_then_else_expr array_access_tail  */
      { if ((yyvsp[0].expressions2d)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions2d));
        delete (yyvsp[0].expressions2d); }
    break;

  case 222: /* expr_atom_head_nonstring: if_then_else_expr "^-1"  */
      { (yyval.expression) = new BinOp((yyloc),(yyvsp[-1].expression), BOT_POW, IntLit::a(-1)); }
    break;

  case 223: /* expr_atom_head_nonstring: if_then_else_expr array_access_tail "^-1"  */
      { if ((yyvsp[-1].expressions2d)) (yyval.expression)=new BinOp((yyloc),createArrayAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions2d)), BOT_POW, IntLit::a(-1));
        delete (yyvsp[-1].expressions2d); }
    break;

  case 226: /* expr_atom_head_nonstring: call_expr array_access_tail  */
      { if ((yyvsp[0].expressions2d)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expressions2d));
        delete (yyvsp[0].expressions2d); }
    break;

  case 228: /* expr_atom_head_nonstring: call_expr array_access_tail "^-1"  */
      { if ((yyvsp[-1].expressions2d)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[-1].expressions2d));
        delete (yyvsp[-1].expressions2d); }
    break;

  case 229: /* string_expr: "string literal"  */
      { (yyval.expression)=new StringLit((yyloc), (yyvsp[0].sValue)); free((yyvsp[0].sValue)); }
    break;

  case 230: /* string_expr: "interpolated string start" string_quote_rest  */
      { (yyval.expression)=new BinOp((yyloc), new StringLit((yyloc), (yyvsp[-1].sValue)), BOT_PLUSPLUS, (yyvsp[0].expression));
        free((yyvsp[-1].sValue));
      }
    break;

  case 231: /* string_quote_rest: expr_list_head "interpolated string end"  */
      { if ((yyvsp[-1].expressions1d)) (yyval.expression)=new BinOp((yyloc), new Call((yyloc), ASTString("format"), *(yyvsp[-1].expressions1d)), BOT_PLUSPLUS, new StringLit((yyloc),(yyvsp[0].sValue)));
        free((yyvsp[0].sValue));
        delete (yyvsp[-1].expressions1d);
      }
    break;

  case 232: /* string_quote_rest: expr_list_head "interpolated string middle" string_quote_rest  */
      { if ((yyvsp[-2].expressions1d)) (yyval.expression)=new BinOp((yyloc), new Call((yyloc), ASTString("format"), *(yyvsp[-2].expressions1d)), BOT_PLUSPLUS,
                             new BinOp((yyloc), new StringLit((yyloc),(yyvsp[-1].sValue)), BOT_PLUSPLUS, (yyvsp[0].expression)));
        free((yyvsp[-1].sValue));
        delete (yyvsp[-2].expressions1d);
      }
    break;

  case 233: /* array_access_tail: "[" array_access_expr_list "]"  */
      { (yyval.expressions2d)=new std::vector<std::vector<Expression*> >();
        if ((yyvsp[-1].expressions1d)) {
          (yyval.expressions2d)->push_back(*(yyvsp[-1].expressions1d));
          delete (yyvsp[-1].expressions1d);
        }
      }
    break;

  case 234: /* array_access_tail: array_access_tail "[" array_access_expr_list "]"  */
      { (yyval.expressions2d)=(yyvsp[-3].expressions2d);
        if ((yyval.expressions2d) && (yyvsp[-1].expressions1d)) {
          (yyval.expressions2d)->push_back(*(yyvsp[-1].expressions1d));
          delete (yyvsp[-1].expressions1d);
        }
      }
    break;

  case 235: /* set_literal: '{' '}'  */
      { (yyval.expression) = new SetLit((yyloc), std::vector<Expression*>()); }
    break;

  case 236: /* set_literal: '{' expr_list '}'  */
      { if ((yyvsp[-1].expressions1d)) (yyval.expression) = new SetLit((yyloc), *(yyvsp[-1].expressions1d));
        delete (yyvsp[-1].expressions1d); }
    break;

  case 237: /* set_comp: '{' expr '|' comp_tail '}'  */
      { if ((yyvsp[-1].generatorsPointer)) (yyval.expression) = new Comprehension((yyloc), (yyvsp[-3].expression), *(yyvsp[-1].generatorsPointer), true);
        delete (yyvsp[-1].generatorsPointer);
      }
    break;

  case 238: /* comp_tail: generator_list  */
      { if ((yyvsp[0].generators)) (yyval.generatorsPointer)=new Generators; (yyval.generatorsPointer)->g = *(yyvsp[0].generators); delete (yyvsp[0].generators); }
    break;

  case 240: /* generator_list_head: generator  */
      { (yyval.generators)=new std::vector<Generator>; if ((yyvsp[0].generator)) (yyval.generators)->push_back(*(yyvsp[0].generator)); delete (yyvsp[0].generator); }
    break;

  case 241: /* generator_list_head: generator_eq  */
      { (yyval.generators)=new std::vector<Generator>; if ((yyvsp[0].generator)) (yyval.generators)->push_back(*(yyvsp[0].generator)); delete (yyvsp[0].generator); }
    break;

  case 242: /* generator_list_head: generator_eq "where" expr  */
      { (yyval.generators)=new std::vector<Generator>;
        if ((yyvsp[-2].generator)) (yyval.generators)->push_back(*(yyvsp[-2].generator));
        if ((yyvsp[-2].generator) && (yyvsp[0].expression)) (yyval.generators)->push_back(Generator((yyval.generators)->size(),(yyvsp[0].expression)));
        delete (yyvsp[-2].generator);
      }
    break;

  case 243: /* generator_list_head: generator_list_head ',' generator  */
      { (yyval.generators)=(yyvsp[-2].generators); if ((yyval.generators) && (yyvsp[0].generator)) (yyval.generators)->push_back(*(yyvsp[0].generator)); delete (yyvsp[0].generator); }
    break;

  case 244: /* generator_list_head: generator_list_head ',' generator_eq  */
      { (yyval.generators)=(yyvsp[-2].generators); if ((yyval.generators) && (yyvsp[0].generator)) (yyval.generators)->push_back(*(yyvsp[0].generator)); delete (yyvsp[0].generator); }
    break;

  case 245: /* generator_list_head: generator_list_head ',' generator_eq "where" expr  */
      { (yyval.generators)=(yyvsp[-4].generators);
        if ((yyval.generators) && (yyvsp[-2].generator)) (yyval.generators)->push_back(*(yyvsp[-2].generator));
        if ((yyval.generators) && (yyvsp[-2].generator) && (yyvsp[0].expression)) (yyval.generators)->push_back(Generator((yyval.generators)->size(),(yyvsp[0].expression)));
        delete (yyvsp[-2].generator);
      }
    break;

  case 246: /* generator: id_list "in" expr  */
      { if ((yyvsp[-2].strings) && (yyvsp[0].expression)) (yyval.generator)=new Generator(*(yyvsp[-2].strings),(yyvsp[0].expression),nullptr); else (yyval.generator)=nullptr; delete (yyvsp[-2].strings); }
    break;

  case 247: /* generator: id_list "in" expr "where" expr  */
      { if ((yyvsp[-4].strings) && (yyvsp[-2].expression)) (yyval.generator)=new Generator(*(yyvsp[-4].strings),(yyvsp[-2].expression),(yyvsp[0].expression)); else (yyval.generator)=nullptr; delete (yyvsp[-4].strings); }
    break;

  case 248: /* generator_eq: "identifier" "=" expr  */
      { if ((yyvsp[0].expression)) (yyval.generator)=new Generator({(yyvsp[-2].sValue)},nullptr,(yyvsp[0].expression)); else (yyval.generator)=nullptr; free((yyvsp[-2].sValue)); }
    break;

  case 250: /* id_list_head: "identifier"  */
      { (yyval.strings)=new std::vector<std::string>; (yyval.strings)->push_back((yyvsp[0].sValue)); free((yyvsp[0].sValue)); }
    break;

  case 251: /* id_list_head: id_list_head ',' "identifier"  */
      { (yyval.strings)=(yyvsp[-2].strings); if ((yyval.strings) && (yyvsp[0].sValue)) (yyval.strings)->push_back((yyvsp[0].sValue)); free((yyvsp[0].sValue)); }
    break;

  case 252: /* simple_array_literal: "[" "]"  */
      { (yyval.expression)=new ArrayLit((yyloc), std::vector<MiniZinc::Expression*>()); }
    break;

  case 253: /* simple_array_literal: "[" expr_list "]"  */
      { if ((yyvsp[-1].expressions1d)) (yyval.expression)=new ArrayLit((yyloc), *(yyvsp[-1].expressions1d)); delete (yyvsp[-1].expressions1d); }
    break;

  case 254: /* simple_array_literal_2d: "[|" "|]"  */
      { (yyval.expression)=new ArrayLit((yyloc), std::vector<std::vector<Expression*> >()); }
    break;

  case 255: /* simple_array_literal_2d: "[|" simple_array_literal_2d_list "|]"  */
      { if ((yyvsp[-1].expressions2d)) {
          (yyval.expression)=new ArrayLit((yyloc), *(yyvsp[-1].expressions2d));
          for (unsigned int i=1; i<(yyvsp[-1].expressions2d)->size(); i++)
            if ((*(yyvsp[-1].expressions2d))[i].size() != (*(yyvsp[-1].expressions2d))[i-1].size())
              yyerror(&(yylsp[-1]), parm, "syntax error, all sub-arrays of 2d array literal must have the same length");
          delete (yyvsp[-1].expressions2d);
        } else {
          (yyval.expression) = nullptr;
        }
      }
    break;

  case 256: /* simple_array_literal_2d: "[|" simple_array_literal_2d_list '|' "|]"  */
      { if ((yyvsp[-2].expressions2d)) {
          (yyval.expression)=new ArrayLit((yyloc), *(yyvsp[-2].expressions2d));
          for (unsigned int i=1; i<(yyvsp[-2].expressions2d)->size(); i++)
            if ((*(yyvsp[-2].expressions2d))[i].size() != (*(yyvsp[-2].expressions2d))[i-1].size())
              yyerror(&(yylsp[-2]), parm, "syntax error, all sub-arrays of 2d array literal must have the same length");
          delete (yyvsp[-2].expressions2d);
        } else {
          (yyval.expression) = nullptr;
        }
      }
    break;

  case 257: /* simple_array_literal_2d: "[|" simple_array_literal_3d_list "|]"  */
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

  case 258: /* simple_array_literal_3d_list: '|' '|'  */
      { (yyval.expressions3d)=new std::vector<std::vector<std::vector<MiniZinc::Expression*> > >;
      }
    break;

  case 259: /* simple_array_literal_3d_list: '|' simple_array_literal_2d_list '|'  */
      { (yyval.expressions3d)=new std::vector<std::vector<std::vector<MiniZinc::Expression*> > >;
        if ((yyvsp[-1].expressions2d)) (yyval.expressions3d)->push_back(*(yyvsp[-1].expressions2d));
        delete (yyvsp[-1].expressions2d);
      }
    break;

  case 260: /* simple_array_literal_3d_list: simple_array_literal_3d_list ',' '|' simple_array_literal_2d_list '|'  */
      { (yyval.expressions3d)=(yyvsp[-4].expressions3d);
        if ((yyval.expressions3d) && (yyvsp[-1].expressions2d)) (yyval.expressions3d)->push_back(*(yyvsp[-1].expressions2d));
        delete (yyvsp[-1].expressions2d);
      }
    break;

  case 261: /* simple_array_literal_2d_list: expr_list  */
      { (yyval.expressions2d)=new std::vector<std::vector<MiniZinc::Expression*> >;
        if ((yyvsp[0].expressions1d)) (yyval.expressions2d)->push_back(*(yyvsp[0].expressions1d));
        delete (yyvsp[0].expressions1d);
      }
    break;

  case 262: /* simple_array_literal_2d_list: simple_array_literal_2d_list '|' expr_list  */
      { (yyval.expressions2d)=(yyvsp[-2].expressions2d); if ((yyval.expressions2d) && (yyvsp[0].expressions1d)) (yyval.expressions2d)->push_back(*(yyvsp[0].expressions1d)); delete (yyvsp[0].expressions1d); }
    break;

  case 263: /* simple_array_comp: "[" expr '|' comp_tail "]"  */
      { if ((yyvsp[-1].generatorsPointer)) (yyval.expression)=new Comprehension((yyloc), (yyvsp[-3].expression), *(yyvsp[-1].generatorsPointer), false);
        delete (yyvsp[-1].generatorsPointer);
      }
    break;

  case 264: /* if_then_else_expr: "if" expr "then" expr "endif"  */
      {
        std::vector<Expression*> iexps;
        iexps.push_back((yyvsp[-3].expression));
        iexps.push_back((yyvsp[-1].expression));
        (yyval.expression)=new ITE((yyloc), iexps, nullptr);
      }
    break;

  case 265: /* if_then_else_expr: "if" expr "then" expr elseif_list "else" expr "endif"  */
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

  case 266: /* elseif_list: %empty  */
      { (yyval.expressions1d)=new std::vector<MiniZinc::Expression*>; }
    break;

  case 267: /* elseif_list: elseif_list "elseif" expr "then" expr  */
      { (yyval.expressions1d)=(yyvsp[-4].expressions1d); if ((yyval.expressions1d) && (yyvsp[-2].expression) && (yyvsp[0].expression)) { (yyval.expressions1d)->push_back((yyvsp[-2].expression)); (yyval.expressions1d)->push_back((yyvsp[0].expression)); } }
    break;

  case 268: /* quoted_op: "'<->'"  */
      { (yyval.iValue)=BOT_EQUIV; }
    break;

  case 269: /* quoted_op: "'->'"  */
      { (yyval.iValue)=BOT_IMPL; }
    break;

  case 270: /* quoted_op: "'<-'"  */
      { (yyval.iValue)=BOT_RIMPL; }
    break;

  case 271: /* quoted_op: "'\\/'"  */
      { (yyval.iValue)=BOT_OR; }
    break;

  case 272: /* quoted_op: "'xor'"  */
      { (yyval.iValue)=BOT_XOR; }
    break;

  case 273: /* quoted_op: "'/\\'"  */
      { (yyval.iValue)=BOT_AND; }
    break;

  case 274: /* quoted_op: "'<'"  */
      { (yyval.iValue)=BOT_LE; }
    break;

  case 275: /* quoted_op: "'>'"  */
      { (yyval.iValue)=BOT_GR; }
    break;

  case 276: /* quoted_op: "'<='"  */
      { (yyval.iValue)=BOT_LQ; }
    break;

  case 277: /* quoted_op: "'>='"  */
      { (yyval.iValue)=BOT_GQ; }
    break;

  case 278: /* quoted_op: "'='"  */
      { (yyval.iValue)=BOT_EQ; }
    break;

  case 279: /* quoted_op: "'!='"  */
      { (yyval.iValue)=BOT_NQ; }
    break;

  case 280: /* quoted_op: "'in'"  */
      { (yyval.iValue)=BOT_IN; }
    break;

  case 281: /* quoted_op: "'subset'"  */
      { (yyval.iValue)=BOT_SUBSET; }
    break;

  case 282: /* quoted_op: "'superset'"  */
      { (yyval.iValue)=BOT_SUPERSET; }
    break;

  case 283: /* quoted_op: "'union'"  */
      { (yyval.iValue)=BOT_UNION; }
    break;

  case 284: /* quoted_op: "'diff'"  */
      { (yyval.iValue)=BOT_DIFF; }
    break;

  case 285: /* quoted_op: "'symdiff'"  */
      { (yyval.iValue)=BOT_SYMDIFF; }
    break;

  case 286: /* quoted_op: "'+'"  */
      { (yyval.iValue)=BOT_PLUS; }
    break;

  case 287: /* quoted_op: "'-'"  */
      { (yyval.iValue)=BOT_MINUS; }
    break;

  case 288: /* quoted_op: "'*'"  */
      { (yyval.iValue)=BOT_MULT; }
    break;

  case 289: /* quoted_op: "'^'"  */
      { (yyval.iValue)=BOT_POW; }
    break;

  case 290: /* quoted_op: "'/'"  */
      { (yyval.iValue)=BOT_DIV; }
    break;

  case 291: /* quoted_op: "'div'"  */
      { (yyval.iValue)=BOT_IDIV; }
    break;

  case 292: /* quoted_op: "'mod'"  */
      { (yyval.iValue)=BOT_MOD; }
    break;

  case 293: /* quoted_op: "'intersect'"  */
      { (yyval.iValue)=BOT_INTERSECT; }
    break;

  case 294: /* quoted_op: "'++'"  */
      { (yyval.iValue)=BOT_PLUSPLUS; }
    break;

  case 295: /* quoted_op: "'not'"  */
      { (yyval.iValue)=-1; }
    break;

  case 296: /* quoted_op_call: quoted_op '(' expr ',' expr ')'  */
      { if ((yyvsp[-5].iValue)==-1) {
          (yyval.expression)=nullptr;
          yyerror(&(yylsp[-3]), parm, "syntax error, unary operator with two arguments");
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-3].expression),static_cast<BinOpType>((yyvsp[-5].iValue)),(yyvsp[-1].expression));
        }
      }
    break;

  case 297: /* quoted_op_call: quoted_op '(' expr ')'  */
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
          if (uot==UOT_PLUS && (yyvsp[-1].expression) && ((yyvsp[-1].expression)->isa<IntLit>() || (yyvsp[-1].expression)->isa<FloatLit>())) {
            (yyval.expression) = (yyvsp[-1].expression);
          } else if (uot==UOT_MINUS && (yyvsp[-1].expression) && (yyvsp[-1].expression)->isa<IntLit>()) {
            (yyval.expression) = IntLit::a(-(yyvsp[-1].expression)->cast<IntLit>()->v());
          } else if (uot==UOT_MINUS && (yyvsp[-1].expression) && (yyvsp[-1].expression)->isa<FloatLit>()) {
            (yyval.expression) = FloatLit::a(-(yyvsp[-1].expression)->cast<FloatLit>()->v());
          } else {
            (yyval.expression)=new UnOp((yyloc), static_cast<UnOpType>(uot),(yyvsp[-1].expression));
          }
        }
      }
    break;

  case 298: /* call_expr: "identifier" '(' ')'  */
      { (yyval.expression)=new Call((yyloc), (yyvsp[-2].sValue), std::vector<Expression*>()); free((yyvsp[-2].sValue)); }
    break;

  case 299: /* call_expr: "identifier" "^-1" '(' ')'  */
      { (yyval.expression)=new Call((yyloc), std::string((yyvsp[-3].sValue))+"⁻¹", std::vector<Expression*>()); free((yyvsp[-3].sValue)); }
    break;

  case 301: /* call_expr: "identifier" '(' comp_or_expr ')'  */
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
            (yyval.expression)=new Call((yyloc), (yyvsp[-3].sValue), args);
          }
        }
        free((yyvsp[-3].sValue));
        delete (yyvsp[-1].expressionPairs);
      }
    break;

  case 302: /* call_expr: "identifier" '(' comp_or_expr ')' '(' expr ')'  */
      { 
        vector<Generator> gens;
        vector<Id*> ids;
        if ((yyvsp[-4].expressionPairs)) {
          for (unsigned int i=0; i<(yyvsp[-4].expressionPairs)->size(); i++) {
            if (Id* id = Expression::dynamicCast<Id>((*(yyvsp[-4].expressionPairs))[i].first)) {
              if ((*(yyvsp[-4].expressionPairs))[i].second) {
                ParserLocation loc = (*(yyvsp[-4].expressionPairs))[i].second->loc().parserLocation();
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
                    ParserLocation loc = (*(yyvsp[-4].expressionPairs))[i].first->loc().parserLocation();
                    yyerror(&loc, parm, "illegal expression in generator call");
                  }
                }
              } else {
                ParserLocation loc = (*(yyvsp[-4].expressionPairs))[i].first->loc().parserLocation();
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
          (yyval.expression)=new Call((yyloc), (yyvsp[-6].sValue), args);
        }
        free((yyvsp[-6].sValue));
        delete (yyvsp[-4].expressionPairs);
      }
    break;

  case 303: /* call_expr: "identifier" "^-1" '(' comp_or_expr ')'  */
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
            (yyval.expression)=new Call((yyloc), std::string((yyvsp[-4].sValue))+"⁻¹", args);
          }
        }
        free((yyvsp[-4].sValue));
        delete (yyvsp[-1].expressionPairs);
      }
    break;

  case 304: /* call_expr: "identifier" "^-1" '(' comp_or_expr ')' '(' expr ')'  */
      {
        vector<Generator> gens;
        vector<Id*> ids;
        if ((yyvsp[-4].expressionPairs)) {
          for (unsigned int i=0; i<(yyvsp[-4].expressionPairs)->size(); i++) {
            if (Id* id = Expression::dynamicCast<Id>((*(yyvsp[-4].expressionPairs))[i].first)) {
              if ((*(yyvsp[-4].expressionPairs))[i].second) {
                ParserLocation loc = (*(yyvsp[-4].expressionPairs))[i].second->loc().parserLocation();
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
                    ParserLocation loc = (*(yyvsp[-4].expressionPairs))[i].first->loc().parserLocation();
                    yyerror(&loc, parm, "illegal expression in generator call");
                  }
                }
              } else {
                ParserLocation loc = (*(yyvsp[-4].expressionPairs))[i].first->loc().parserLocation();
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
          (yyval.expression)=new Call((yyloc), std::string((yyvsp[-7].sValue))+"⁻¹", args);
        }
        free((yyvsp[-7].sValue));
        delete (yyvsp[-4].expressionPairs);
      }
    break;

  case 306: /* comp_or_expr_head: expr  */
      { (yyval.expressionPairs)=new vector<pair<Expression*,Expression*> >;
        if ((yyvsp[0].expression)) {
          (yyval.expressionPairs)->push_back(pair<Expression*,Expression*>((yyvsp[0].expression),nullptr));
        }
      }
    break;

  case 307: /* comp_or_expr_head: expr "where" expr  */
      { (yyval.expressionPairs)=new vector<pair<Expression*,Expression*> >;
        if ((yyvsp[-2].expression) && (yyvsp[0].expression)) {
          (yyval.expressionPairs)->push_back(pair<Expression*,Expression*>((yyvsp[-2].expression),(yyvsp[0].expression)));
        }
      }
    break;

  case 308: /* comp_or_expr_head: comp_or_expr_head ',' expr  */
      { (yyval.expressionPairs)=(yyvsp[-2].expressionPairs); if ((yyval.expressionPairs) && (yyvsp[0].expression)) (yyval.expressionPairs)->push_back(pair<Expression*,Expression*>((yyvsp[0].expression),nullptr)); }
    break;

  case 309: /* comp_or_expr_head: comp_or_expr_head ',' expr "where" expr  */
      { (yyval.expressionPairs)=(yyvsp[-4].expressionPairs); if ((yyval.expressionPairs) && (yyvsp[-2].expression) && (yyvsp[0].expression)) (yyval.expressionPairs)->push_back(pair<Expression*,Expression*>((yyvsp[-2].expression),(yyvsp[0].expression))); }
    break;

  case 310: /* let_expr: "let" '{' let_vardecl_item_list '}' "in" expr  */
      { if ((yyvsp[-3].expressions1d) && (yyvsp[0].expression)) {
          (yyval.expression)=new Let((yyloc), *(yyvsp[-3].expressions1d), (yyvsp[0].expression)); delete (yyvsp[-3].expressions1d);
        } else {
          (yyval.expression)=nullptr;
        }
      }
    break;

  case 311: /* let_expr: "let" '{' let_vardecl_item_list comma_or_semi '}' "in" expr  */
      { if ((yyvsp[-4].expressions1d) && (yyvsp[0].expression)) {
          (yyval.expression)=new Let((yyloc), *(yyvsp[-4].expressions1d), (yyvsp[0].expression)); delete (yyvsp[-4].expressions1d);
        } else {
          (yyval.expression)=nullptr;
        }
      }
    break;

  case 312: /* let_vardecl_item_list: let_vardecl_item  */
      { (yyval.expressions1d)=new vector<Expression*>; (yyval.expressions1d)->push_back((yyvsp[0].vardeclexpr)); }
    break;

  case 313: /* let_vardecl_item_list: constraint_item  */
      { (yyval.expressions1d)=new vector<Expression*>;
        if ((yyvsp[0].item)) {
          ConstraintI* ce = (yyvsp[0].item)->cast<ConstraintI>();
          (yyval.expressions1d)->push_back(ce->e());
          ce->e(nullptr);
        }
      }
    break;

  case 314: /* let_vardecl_item_list: let_vardecl_item_list comma_or_semi let_vardecl_item  */
      { (yyval.expressions1d)=(yyvsp[-2].expressions1d); if ((yyval.expressions1d) && (yyvsp[0].vardeclexpr)) (yyval.expressions1d)->push_back((yyvsp[0].vardeclexpr)); }
    break;

  case 315: /* let_vardecl_item_list: let_vardecl_item_list comma_or_semi constraint_item  */
      { (yyval.expressions1d)=(yyvsp[-2].expressions1d);
        if ((yyval.expressions1d) && (yyvsp[0].item)) {
          ConstraintI* ce = (yyvsp[0].item)->cast<ConstraintI>();
          (yyval.expressions1d)->push_back(ce->e());
          ce->e(nullptr);
        }
      }
    break;

  case 318: /* let_vardecl_item: ti_expr_and_id annotations  */
      { (yyval.vardeclexpr) = (yyvsp[-1].vardeclexpr);
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->toplevel(false);
        if ((yyval.vardeclexpr) && (yyvsp[0].expressions1d)) (yyval.vardeclexpr)->addAnnotations(*(yyvsp[0].expressions1d));
        delete (yyvsp[0].expressions1d);
      }
    break;

  case 319: /* let_vardecl_item: ti_expr_and_id annotations "=" expr  */
      { if ((yyvsp[-3].vardeclexpr)) (yyvsp[-3].vardeclexpr)->e((yyvsp[0].expression));
        (yyval.vardeclexpr) = (yyvsp[-3].vardeclexpr);
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->loc((yyloc));
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->toplevel(false);
        if ((yyval.vardeclexpr) && (yyvsp[-2].expressions1d)) (yyval.vardeclexpr)->addAnnotations(*(yyvsp[-2].expressions1d));
        delete (yyvsp[-2].expressions1d);
      }
    break;

  case 320: /* annotations: %empty  */
      { (yyval.expressions1d)=nullptr; }
    break;

  case 322: /* annotation_expr: expr_atom_head_nonstring  */
      { (yyval.expression) = (yyvsp[0].expression); }
    break;

  case 323: /* annotation_expr: string_expr  */
      { (yyval.expression) = new Call((yylsp[0]), ASTString("mzn_expression_name"), {(yyvsp[0].expression)}); }
    break;

  case 324: /* ne_annotations: "::" annotation_expr  */
      { (yyval.expressions1d)=new std::vector<Expression*>(1);
        (*(yyval.expressions1d))[0] = (yyvsp[0].expression);
      }
    break;

  case 325: /* ne_annotations: ne_annotations "::" annotation_expr  */
      { (yyval.expressions1d)=(yyvsp[-2].expressions1d); if ((yyval.expressions1d)) (yyval.expressions1d)->push_back((yyvsp[0].expression)); }
    break;

  case 326: /* id_or_quoted_op: "identifier"  */
      { (yyval.sValue)=(yyvsp[0].sValue); }
    break;

  case 327: /* id_or_quoted_op: "identifier" "^-1"  */
      { (yyval.sValue)=strdup((std::string((yyvsp[-1].sValue))+"⁻¹").c_str()); }
    break;

  case 328: /* id_or_quoted_op: "'<->'"  */
      { (yyval.sValue)=strdup("'<->'"); }
    break;

  case 329: /* id_or_quoted_op: "'->'"  */
      { (yyval.sValue)=strdup("'->'"); }
    break;

  case 330: /* id_or_quoted_op: "'<-'"  */
      { (yyval.sValue)=strdup("'<-'"); }
    break;

  case 331: /* id_or_quoted_op: "'\\/'"  */
      { (yyval.sValue)=strdup("'\\/'"); }
    break;

  case 332: /* id_or_quoted_op: "'xor'"  */
      { (yyval.sValue)=strdup("'xor'"); }
    break;

  case 333: /* id_or_quoted_op: "'/\\'"  */
      { (yyval.sValue)=strdup("'/\\'"); }
    break;

  case 334: /* id_or_quoted_op: "'<'"  */
      { (yyval.sValue)=strdup("'<'"); }
    break;

  case 335: /* id_or_quoted_op: "'>'"  */
      { (yyval.sValue)=strdup("'>'"); }
    break;

  case 336: /* id_or_quoted_op: "'<='"  */
      { (yyval.sValue)=strdup("'<='"); }
    break;

  case 337: /* id_or_quoted_op: "'>='"  */
      { (yyval.sValue)=strdup("'>='"); }
    break;

  case 338: /* id_or_quoted_op: "'='"  */
      { (yyval.sValue)=strdup("'='"); }
    break;

  case 339: /* id_or_quoted_op: "'!='"  */
      { (yyval.sValue)=strdup("'!='"); }
    break;

  case 340: /* id_or_quoted_op: "'in'"  */
      { (yyval.sValue)=strdup("'in'"); }
    break;

  case 341: /* id_or_quoted_op: "'subset'"  */
      { (yyval.sValue)=strdup("'subset'"); }
    break;

  case 342: /* id_or_quoted_op: "'superset'"  */
      { (yyval.sValue)=strdup("'superset'"); }
    break;

  case 343: /* id_or_quoted_op: "'union'"  */
      { (yyval.sValue)=strdup("'union'"); }
    break;

  case 344: /* id_or_quoted_op: "'diff'"  */
      { (yyval.sValue)=strdup("'diff'"); }
    break;

  case 345: /* id_or_quoted_op: "'symdiff'"  */
      { (yyval.sValue)=strdup("'symdiff'"); }
    break;

  case 346: /* id_or_quoted_op: "'..'"  */
      { (yyval.sValue)=strdup("'..'"); }
    break;

  case 347: /* id_or_quoted_op: "'+'"  */
      { (yyval.sValue)=strdup("'+'"); }
    break;

  case 348: /* id_or_quoted_op: "'-'"  */
      { (yyval.sValue)=strdup("'-'"); }
    break;

  case 349: /* id_or_quoted_op: "'*'"  */
      { (yyval.sValue)=strdup("'*'"); }
    break;

  case 350: /* id_or_quoted_op: "'^'"  */
      { (yyval.sValue)=strdup("'^'"); }
    break;

  case 351: /* id_or_quoted_op: "'/'"  */
      { (yyval.sValue)=strdup("'/'"); }
    break;

  case 352: /* id_or_quoted_op: "'div'"  */
      { (yyval.sValue)=strdup("'div'"); }
    break;

  case 353: /* id_or_quoted_op: "'mod'"  */
      { (yyval.sValue)=strdup("'mod'"); }
    break;

  case 354: /* id_or_quoted_op: "'intersect'"  */
      { (yyval.sValue)=strdup("'intersect'"); }
    break;

  case 355: /* id_or_quoted_op: "'not'"  */
      { (yyval.sValue)=strdup("'not'"); }
    break;

  case 356: /* id_or_quoted_op: "'++'"  */
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

