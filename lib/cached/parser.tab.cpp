/* A Bison parser, made by GNU Bison 3.6.1.  */

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

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.6.1"

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
  (Current).filename(Rhs[1].filename()); \
  (Current).first_line(Rhs[1].first_line()); \
  (Current).first_column(Rhs[1].first_column()); \
  (Current).last_line(Rhs[N].last_line()); \
  (Current).last_column(Rhs[N].last_column());

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
  while (m->parent() != NULL) {
    m = m->parent();
    pp->err << "(included from file '" << m->filename() << "')" << endl;
  }
  pp->err << location->toString() << ":" << endl;
  pp->printCurrentLine(location->first_column(),location->last_column());
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

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_MZN_YY_USERS_TACK_PROGRAMMING_MINIZINC_LIBMZN_DEVELOP_BUILD_INCLUDE_MINIZINC_PARSER_TAB_HH_INCLUDED
# define YY_MZN_YY_USERS_TACK_PROGRAMMING_MINIZINC_LIBMZN_DEVELOP_BUILD_INCLUDE_MINIZINC_PARSER_TAB_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int mzn_yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    END = 0,                       /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    MZN_INTEGER_LITERAL = 258,     /* "integer literal"  */
    MZN_BOOL_LITERAL = 259,        /* "bool literal"  */
    MZN_FLOAT_LITERAL = 260,       /* "float literal"  */
    MZN_IDENTIFIER = 261,          /* "identifier"  */
    MZN_QUOTED_IDENTIFIER = 262,   /* "quoted identifier"  */
    MZN_STRING_LITERAL = 263,      /* "string literal"  */
    MZN_STRING_QUOTE_START = 264,  /* "interpolated string start"  */
    MZN_STRING_QUOTE_MID = 265,    /* "interpolated string middle"  */
    MZN_STRING_QUOTE_END = 266,    /* "interpolated string end"  */
    MZN_TI_IDENTIFIER = 267,       /* "type-inst identifier"  */
    MZN_TI_ENUM_IDENTIFIER = 268,  /* "type-inst enum identifier"  */
    MZN_DOC_COMMENT = 269,         /* "documentation comment"  */
    MZN_DOC_FILE_COMMENT = 270,    /* "file-level documentation comment"  */
    MZN_VAR = 271,                 /* "var"  */
    MZN_PAR = 272,                 /* "par"  */
    MZN_ABSENT = 273,              /* "<>"  */
    MZN_ANN = 274,                 /* "ann"  */
    MZN_ANNOTATION = 275,          /* "annotation"  */
    MZN_ANY = 276,                 /* "any"  */
    MZN_ARRAY = 277,               /* "array"  */
    MZN_BOOL = 278,                /* "bool"  */
    MZN_CASE = 279,                /* "case"  */
    MZN_CONSTRAINT = 280,          /* "constraint"  */
    MZN_DEFAULT = 281,             /* "default"  */
    MZN_ELSE = 282,                /* "else"  */
    MZN_ELSEIF = 283,              /* "elseif"  */
    MZN_ENDIF = 284,               /* "endif"  */
    MZN_ENUM = 285,                /* "enum"  */
    MZN_FLOAT = 286,               /* "float"  */
    MZN_FUNCTION = 287,            /* "function"  */
    MZN_IF = 288,                  /* "if"  */
    MZN_INCLUDE = 289,             /* "include"  */
    MZN_INFINITY = 290,            /* "infinity"  */
    MZN_INT = 291,                 /* "int"  */
    MZN_LET = 292,                 /* "let"  */
    MZN_LIST = 293,                /* "list"  */
    MZN_MAXIMIZE = 294,            /* "maximize"  */
    MZN_MINIMIZE = 295,            /* "minimize"  */
    MZN_OF = 296,                  /* "of"  */
    MZN_OPT = 297,                 /* "opt"  */
    MZN_SATISFY = 298,             /* "satisfy"  */
    MZN_OUTPUT = 299,              /* "output"  */
    MZN_PREDICATE = 300,           /* "predicate"  */
    MZN_RECORD = 301,              /* "record"  */
    MZN_SET = 302,                 /* "set"  */
    MZN_SOLVE = 303,               /* "solve"  */
    MZN_STRING = 304,              /* "string"  */
    MZN_TEST = 305,                /* "test"  */
    MZN_THEN = 306,                /* "then"  */
    MZN_TUPLE = 307,               /* "tuple"  */
    MZN_TYPE = 308,                /* "type"  */
    MZN_UNDERSCORE = 309,          /* "_"  */
    MZN_VARIANT_RECORD = 310,      /* "variant_record"  */
    MZN_WHERE = 311,               /* "where"  */
    MZN_LEFT_BRACKET = 312,        /* "["  */
    MZN_LEFT_2D_BRACKET = 313,     /* "[|"  */
    MZN_RIGHT_BRACKET = 314,       /* "]"  */
    MZN_RIGHT_2D_BRACKET = 315,    /* "|]"  */
    FLATZINC_IDENTIFIER = 316,     /* FLATZINC_IDENTIFIER  */
    MZN_INVALID_INTEGER_LITERAL = 317, /* "invalid integer literal"  */
    MZN_INVALID_FLOAT_LITERAL = 318, /* "invalid float literal"  */
    MZN_UNTERMINATED_STRING = 319, /* "unterminated string"  */
    MZN_END_OF_LINE_IN_STRING = 320, /* "end of line inside string literal"  */
    MZN_INVALID_NULL = 321,        /* "null character"  */
    MZN_EQUIV = 322,               /* "<->"  */
    MZN_IMPL = 323,                /* "->"  */
    MZN_RIMPL = 324,               /* "<-"  */
    MZN_OR = 325,                  /* "\\/"  */
    MZN_XOR = 326,                 /* "xor"  */
    MZN_AND = 327,                 /* "/\\"  */
    MZN_LE = 328,                  /* "<"  */
    MZN_GR = 329,                  /* ">"  */
    MZN_LQ = 330,                  /* "<="  */
    MZN_GQ = 331,                  /* ">="  */
    MZN_EQ = 332,                  /* "="  */
    MZN_NQ = 333,                  /* "!="  */
    MZN_WEAK_EQ = 334,             /* "~="  */
    MZN_IN = 335,                  /* "in"  */
    MZN_SUBSET = 336,              /* "subset"  */
    MZN_SUPERSET = 337,            /* "superset"  */
    MZN_UNION = 338,               /* "union"  */
    MZN_DIFF = 339,                /* "diff"  */
    MZN_SYMDIFF = 340,             /* "symdiff"  */
    MZN_DOTDOT = 341,              /* ".."  */
    MZN_PLUS = 342,                /* "+"  */
    MZN_MINUS = 343,               /* "-"  */
    MZN_WEAK_PLUS = 344,           /* "~+"  */
    MZN_WEAK_MINUS = 345,          /* "~-"  */
    MZN_MULT = 346,                /* "*"  */
    MZN_DIV = 347,                 /* "/"  */
    MZN_IDIV = 348,                /* "div"  */
    MZN_MOD = 349,                 /* "mod"  */
    MZN_INTERSECT = 350,           /* "intersect"  */
    MZN_WEAK_MULT = 351,           /* "~*"  */
    MZN_POW = 352,                 /* "^"  */
    MZN_NOT = 353,                 /* "not"  */
    MZN_PLUSPLUS = 354,            /* "++"  */
    MZN_COLONCOLON = 355,          /* "::"  */
    PREC_ANNO = 356,               /* PREC_ANNO  */
    MZN_EQUIV_QUOTED = 357,        /* "'<->'"  */
    MZN_IMPL_QUOTED = 358,         /* "'->'"  */
    MZN_RIMPL_QUOTED = 359,        /* "'<-'"  */
    MZN_OR_QUOTED = 360,           /* "'\\/'"  */
    MZN_XOR_QUOTED = 361,          /* "'xor'"  */
    MZN_AND_QUOTED = 362,          /* "'/\\'"  */
    MZN_LE_QUOTED = 363,           /* "'<'"  */
    MZN_GR_QUOTED = 364,           /* "'>'"  */
    MZN_LQ_QUOTED = 365,           /* "'<='"  */
    MZN_GQ_QUOTED = 366,           /* "'>='"  */
    MZN_EQ_QUOTED = 367,           /* "'='"  */
    MZN_NQ_QUOTED = 368,           /* "'!='"  */
    MZN_IN_QUOTED = 369,           /* "'in'"  */
    MZN_SUBSET_QUOTED = 370,       /* "'subset'"  */
    MZN_SUPERSET_QUOTED = 371,     /* "'superset'"  */
    MZN_UNION_QUOTED = 372,        /* "'union'"  */
    MZN_DIFF_QUOTED = 373,         /* "'diff'"  */
    MZN_SYMDIFF_QUOTED = 374,      /* "'symdiff'"  */
    MZN_DOTDOT_QUOTED = 375,       /* "'..'"  */
    MZN_PLUS_QUOTED = 376,         /* "'+'"  */
    MZN_MINUS_QUOTED = 377,        /* "'-'"  */
    MZN_MULT_QUOTED = 378,         /* "'*'"  */
    MZN_DIV_QUOTED = 379,          /* "'/'"  */
    MZN_IDIV_QUOTED = 380,         /* "'div'"  */
    MZN_MOD_QUOTED = 381,          /* "'mod'"  */
    MZN_INTERSECT_QUOTED = 382,    /* "'intersect'"  */
    MZN_POW_QUOTED = 383,          /* "'^'"  */
    MZN_NOT_QUOTED = 384,          /* "'not'"  */
    MZN_COLONCOLON_QUOTED = 385,   /* "'::'"  */
    MZN_PLUSPLUS_QUOTED = 386      /* "'++'"  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
 long long int iValue; char* sValue; bool bValue; double dValue;
         MiniZinc::Item* item;
         MiniZinc::VarDecl* vardeclexpr;
         std::vector<MiniZinc::VarDecl*>* vardeclexpr_v;
         MiniZinc::TypeInst* tiexpr;
         std::vector<MiniZinc::TypeInst*>* tiexpr_v;
         MiniZinc::Expression* expression;
         std::vector<MiniZinc::Expression*>* expression_v;
         std::vector<std::vector<MiniZinc::Expression*> >* expression_vv;
         std::vector<std::vector<std::vector<MiniZinc::Expression*> > >* expression_vvv;
         MiniZinc::Generator* generator;
         std::vector<MiniZinc::Generator>* generator_v;
         std::vector<std::string>* string_v;
         std::vector<std::pair<MiniZinc::Expression*,MiniZinc::Expression*> >* expression_p;
         MiniZinc::Generators* generators;
       


};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



int mzn_yyparse (void *parm);

#endif /* !YY_MZN_YY_USERS_TACK_PROGRAMMING_MINIZINC_LIBMZN_DEVELOP_BUILD_INCLUDE_MINIZINC_PARSER_TAB_HH_INCLUDED  */
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
  YYSYMBOL_MZN_NOT = 98,                   /* "not"  */
  YYSYMBOL_MZN_PLUSPLUS = 99,              /* "++"  */
  YYSYMBOL_MZN_COLONCOLON = 100,           /* "::"  */
  YYSYMBOL_PREC_ANNO = 101,                /* PREC_ANNO  */
  YYSYMBOL_MZN_EQUIV_QUOTED = 102,         /* "'<->'"  */
  YYSYMBOL_MZN_IMPL_QUOTED = 103,          /* "'->'"  */
  YYSYMBOL_MZN_RIMPL_QUOTED = 104,         /* "'<-'"  */
  YYSYMBOL_MZN_OR_QUOTED = 105,            /* "'\\/'"  */
  YYSYMBOL_MZN_XOR_QUOTED = 106,           /* "'xor'"  */
  YYSYMBOL_MZN_AND_QUOTED = 107,           /* "'/\\'"  */
  YYSYMBOL_MZN_LE_QUOTED = 108,            /* "'<'"  */
  YYSYMBOL_MZN_GR_QUOTED = 109,            /* "'>'"  */
  YYSYMBOL_MZN_LQ_QUOTED = 110,            /* "'<='"  */
  YYSYMBOL_MZN_GQ_QUOTED = 111,            /* "'>='"  */
  YYSYMBOL_MZN_EQ_QUOTED = 112,            /* "'='"  */
  YYSYMBOL_MZN_NQ_QUOTED = 113,            /* "'!='"  */
  YYSYMBOL_MZN_IN_QUOTED = 114,            /* "'in'"  */
  YYSYMBOL_MZN_SUBSET_QUOTED = 115,        /* "'subset'"  */
  YYSYMBOL_MZN_SUPERSET_QUOTED = 116,      /* "'superset'"  */
  YYSYMBOL_MZN_UNION_QUOTED = 117,         /* "'union'"  */
  YYSYMBOL_MZN_DIFF_QUOTED = 118,          /* "'diff'"  */
  YYSYMBOL_MZN_SYMDIFF_QUOTED = 119,       /* "'symdiff'"  */
  YYSYMBOL_MZN_DOTDOT_QUOTED = 120,        /* "'..'"  */
  YYSYMBOL_MZN_PLUS_QUOTED = 121,          /* "'+'"  */
  YYSYMBOL_MZN_MINUS_QUOTED = 122,         /* "'-'"  */
  YYSYMBOL_MZN_MULT_QUOTED = 123,          /* "'*'"  */
  YYSYMBOL_MZN_DIV_QUOTED = 124,           /* "'/'"  */
  YYSYMBOL_MZN_IDIV_QUOTED = 125,          /* "'div'"  */
  YYSYMBOL_MZN_MOD_QUOTED = 126,           /* "'mod'"  */
  YYSYMBOL_MZN_INTERSECT_QUOTED = 127,     /* "'intersect'"  */
  YYSYMBOL_MZN_POW_QUOTED = 128,           /* "'^'"  */
  YYSYMBOL_MZN_NOT_QUOTED = 129,           /* "'not'"  */
  YYSYMBOL_MZN_COLONCOLON_QUOTED = 130,    /* "'::'"  */
  YYSYMBOL_MZN_PLUSPLUS_QUOTED = 131,      /* "'++'"  */
  YYSYMBOL_132_ = 132,                     /* ';'  */
  YYSYMBOL_133_ = 133,                     /* '{'  */
  YYSYMBOL_134_ = 134,                     /* '}'  */
  YYSYMBOL_135_ = 135,                     /* '('  */
  YYSYMBOL_136_ = 136,                     /* ')'  */
  YYSYMBOL_137_ = 137,                     /* ','  */
  YYSYMBOL_138_ = 138,                     /* ':'  */
  YYSYMBOL_139_ = 139,                     /* '|'  */
  YYSYMBOL_YYACCEPT = 140,                 /* $accept  */
  YYSYMBOL_model = 141,                    /* model  */
  YYSYMBOL_item_list = 142,                /* item_list  */
  YYSYMBOL_item_list_head = 143,           /* item_list_head  */
  YYSYMBOL_doc_file_comments = 144,        /* doc_file_comments  */
  YYSYMBOL_semi_or_none = 145,             /* semi_or_none  */
  YYSYMBOL_item = 146,                     /* item  */
  YYSYMBOL_item_tail = 147,                /* item_tail  */
  YYSYMBOL_error_item_start = 148,         /* error_item_start  */
  YYSYMBOL_include_item = 149,             /* include_item  */
  YYSYMBOL_vardecl_item = 150,             /* vardecl_item  */
  YYSYMBOL_string_lit_list = 151,          /* string_lit_list  */
  YYSYMBOL_enum_id_list = 152,             /* enum_id_list  */
  YYSYMBOL_assign_item = 153,              /* assign_item  */
  YYSYMBOL_constraint_item = 154,          /* constraint_item  */
  YYSYMBOL_solve_item = 155,               /* solve_item  */
  YYSYMBOL_output_item = 156,              /* output_item  */
  YYSYMBOL_predicate_item = 157,           /* predicate_item  */
  YYSYMBOL_function_item = 158,            /* function_item  */
  YYSYMBOL_annotation_item = 159,          /* annotation_item  */
  YYSYMBOL_operation_item_tail = 160,      /* operation_item_tail  */
  YYSYMBOL_params = 161,                   /* params  */
  YYSYMBOL_params_list = 162,              /* params_list  */
  YYSYMBOL_params_list_head = 163,         /* params_list_head  */
  YYSYMBOL_comma_or_none = 164,            /* comma_or_none  */
  YYSYMBOL_ti_expr_and_id_or_anon = 165,   /* ti_expr_and_id_or_anon  */
  YYSYMBOL_ti_expr_and_id = 166,           /* ti_expr_and_id  */
  YYSYMBOL_ti_expr_list = 167,             /* ti_expr_list  */
  YYSYMBOL_ti_expr_list_head = 168,        /* ti_expr_list_head  */
  YYSYMBOL_ti_expr = 169,                  /* ti_expr  */
  YYSYMBOL_base_ti_expr = 170,             /* base_ti_expr  */
  YYSYMBOL_opt_opt = 171,                  /* opt_opt  */
  YYSYMBOL_base_ti_expr_tail = 172,        /* base_ti_expr_tail  */
  YYSYMBOL_array_access_expr_list = 173,   /* array_access_expr_list  */
  YYSYMBOL_array_access_expr_list_head = 174, /* array_access_expr_list_head  */
  YYSYMBOL_array_access_expr = 175,        /* array_access_expr  */
  YYSYMBOL_expr_list = 176,                /* expr_list  */
  YYSYMBOL_expr_list_head = 177,           /* expr_list_head  */
  YYSYMBOL_set_expr = 178,                 /* set_expr  */
  YYSYMBOL_expr = 179,                     /* expr  */
  YYSYMBOL_expr_atom_head = 180,           /* expr_atom_head  */
  YYSYMBOL_expr_atom_head_nonstring = 181, /* expr_atom_head_nonstring  */
  YYSYMBOL_string_expr = 182,              /* string_expr  */
  YYSYMBOL_string_quote_rest = 183,        /* string_quote_rest  */
  YYSYMBOL_array_access_tail = 184,        /* array_access_tail  */
  YYSYMBOL_set_literal = 185,              /* set_literal  */
  YYSYMBOL_set_comp = 186,                 /* set_comp  */
  YYSYMBOL_comp_tail = 187,                /* comp_tail  */
  YYSYMBOL_generator_list = 188,           /* generator_list  */
  YYSYMBOL_generator_list_head = 189,      /* generator_list_head  */
  YYSYMBOL_generator = 190,                /* generator  */
  YYSYMBOL_generator_eq = 191,             /* generator_eq  */
  YYSYMBOL_id_list = 192,                  /* id_list  */
  YYSYMBOL_id_list_head = 193,             /* id_list_head  */
  YYSYMBOL_simple_array_literal = 194,     /* simple_array_literal  */
  YYSYMBOL_simple_array_literal_2d = 195,  /* simple_array_literal_2d  */
  YYSYMBOL_simple_array_literal_3d_list = 196, /* simple_array_literal_3d_list  */
  YYSYMBOL_simple_array_literal_2d_list = 197, /* simple_array_literal_2d_list  */
  YYSYMBOL_simple_array_comp = 198,        /* simple_array_comp  */
  YYSYMBOL_if_then_else_expr = 199,        /* if_then_else_expr  */
  YYSYMBOL_elseif_list = 200,              /* elseif_list  */
  YYSYMBOL_quoted_op = 201,                /* quoted_op  */
  YYSYMBOL_quoted_op_call = 202,           /* quoted_op_call  */
  YYSYMBOL_call_expr = 203,                /* call_expr  */
  YYSYMBOL_comp_or_expr = 204,             /* comp_or_expr  */
  YYSYMBOL_comp_or_expr_head = 205,        /* comp_or_expr_head  */
  YYSYMBOL_let_expr = 206,                 /* let_expr  */
  YYSYMBOL_let_vardecl_item_list = 207,    /* let_vardecl_item_list  */
  YYSYMBOL_comma_or_semi = 208,            /* comma_or_semi  */
  YYSYMBOL_let_vardecl_item = 209,         /* let_vardecl_item  */
  YYSYMBOL_annotations = 210,              /* annotations  */
  YYSYMBOL_annotation_expr = 211,          /* annotation_expr  */
  YYSYMBOL_ne_annotations = 212,           /* ne_annotations  */
  YYSYMBOL_id_or_quoted_op = 213           /* id_or_quoted_op  */
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
#define YYFINAL  157
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   5058

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  140
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  74
/* YYNRULES -- Number of rules.  */
#define YYNRULES  321
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  540

#define YYMAXUTOK   386


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
     135,   136,     2,     2,   137,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   138,   132,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   133,   139,   134,     2,     2,     2,     2,
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
     125,   126,   127,   128,   129,   130,   131
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   272,   272,   274,   276,   279,   288,   297,   306,   315,
     317,   320,   328,   337,   337,   339,   355,   359,   361,   363,
     364,   366,   368,   370,   372,   374,   377,   377,   377,   378,
     378,   378,   378,   378,   379,   382,   406,   412,   419,   427,
     437,   449,   464,   465,   469,   477,   478,   482,   486,   492,
     494,   501,   506,   511,   518,   522,   530,   540,   547,   556,
     568,   576,   577,   582,   583,   585,   590,   591,   595,   599,
     604,   604,   607,   609,   613,   618,   622,   624,   628,   629,
     635,   644,   647,   655,   663,   672,   681,   690,   703,   704,
     708,   710,   712,   714,   716,   718,   720,   725,   731,   734,
     736,   740,   742,   744,   753,   764,   767,   769,   775,   776,
     778,   780,   782,   784,   793,   802,   804,   806,   808,   810,
     812,   814,   816,   818,   820,   825,   830,   835,   840,   846,
     848,   861,   862,   864,   866,   868,   870,   872,   874,   876,
     878,   880,   882,   884,   886,   888,   890,   892,   894,   896,
     898,   900,   909,   918,   920,   922,   924,   926,   928,   930,
     932,   934,   936,   941,   946,   951,   956,   962,   964,   971,
     983,   985,   989,   991,   993,   995,   998,  1000,  1003,  1005,
    1007,  1009,  1011,  1013,  1014,  1017,  1018,  1021,  1022,  1025,
    1026,  1029,  1030,  1033,  1034,  1037,  1038,  1039,  1044,  1046,
    1052,  1057,  1065,  1072,  1081,  1083,  1088,  1094,  1097,  1100,
    1102,  1104,  1110,  1112,  1114,  1122,  1124,  1127,  1130,  1133,
    1135,  1139,  1141,  1145,  1147,  1158,  1169,  1209,  1212,  1217,
    1224,  1229,  1233,  1239,  1246,  1262,  1263,  1267,  1269,  1271,
    1273,  1275,  1277,  1279,  1281,  1283,  1285,  1287,  1289,  1291,
    1293,  1295,  1297,  1299,  1301,  1303,  1305,  1307,  1309,  1311,
    1313,  1315,  1317,  1319,  1321,  1325,  1333,  1365,  1367,  1368,
    1388,  1443,  1446,  1452,  1458,  1460,  1464,  1471,  1480,  1482,
    1490,  1492,  1501,  1501,  1504,  1510,  1521,  1522,  1525,  1527,
    1531,  1535,  1539,  1541,  1543,  1545,  1547,  1549,  1551,  1553,
    1555,  1557,  1559,  1561,  1563,  1565,  1567,  1569,  1571,  1573,
    1575,  1577,  1579,  1581,  1583,  1585,  1587,  1589,  1591,  1593,
    1595,  1597
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
  "\"div\"", "\"mod\"", "\"intersect\"", "\"~*\"", "\"^\"", "\"not\"",
  "\"++\"", "\"::\"", "PREC_ANNO", "\"'<->'\"", "\"'->'\"", "\"'<-'\"",
  "\"'\\\\/'\"", "\"'xor'\"", "\"'/\\\\'\"", "\"'<'\"", "\"'>'\"",
  "\"'<='\"", "\"'>='\"", "\"'='\"", "\"'!='\"", "\"'in'\"",
  "\"'subset'\"", "\"'superset'\"", "\"'union'\"", "\"'diff'\"",
  "\"'symdiff'\"", "\"'..'\"", "\"'+'\"", "\"'-'\"", "\"'*'\"", "\"'/'\"",
  "\"'div'\"", "\"'mod'\"", "\"'intersect'\"", "\"'^'\"", "\"'not'\"",
  "\"'::'\"", "\"'++'\"", "';'", "'{'", "'}'", "'('", "')'", "','", "':'",
  "'|'", "$accept", "model", "item_list", "item_list_head",
  "doc_file_comments", "semi_or_none", "item", "item_tail",
  "error_item_start", "include_item", "vardecl_item", "string_lit_list",
  "enum_id_list", "assign_item", "constraint_item", "solve_item",
  "output_item", "predicate_item", "function_item", "annotation_item",
  "operation_item_tail", "params", "params_list", "params_list_head",
  "comma_or_none", "ti_expr_and_id_or_anon", "ti_expr_and_id",
  "ti_expr_list", "ti_expr_list_head", "ti_expr", "base_ti_expr",
  "opt_opt", "base_ti_expr_tail", "array_access_expr_list",
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
     385,   386,    59,   123,   125,    40,    41,    44,    58,   124
};
#endif

#define YYPACT_NINF (-402)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-89)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     823,  -103,  -402,  -402,  -402,    -8,  -402,  3485,  -402,  -402,
    1623,  -402,    -9,    -9,  -402,  -402,    33,    -1,  -402,  2820,
      51,  -402,  2155,  3485,    55,  -402,  -402,   -74,    25,  2687,
    3485,    59,   -32,  -402,    73,    23,  2953,   532,  3618,  3618,
    -402,  -402,  -402,  -402,  -402,  -402,  -402,  -402,  -402,  -402,
    -402,  -402,  -402,  -402,  -402,  -402,  -402,  -402,   -53,  -402,
    -402,  -402,  -402,  -402,  -402,  -402,  -402,  -402,  -402,  3086,
    3485,    83,  -402,   -47,  1357,   219,  -402,  -402,  -402,  -402,
    -402,  -402,  -402,  -402,  -402,  -402,   -32,   -48,  -402,    48,
    -402,  3706,  -402,  -402,  -402,    23,    23,    23,    23,    23,
      23,   -44,  -402,    23,  -402,  1490,  3219,  3485,  1090,    40,
     -29,  3485,  3485,  3485,   -37,    11,  4739,  -402,  -402,  -402,
    -402,  2421,  2554,   -34,  2155,    44,  4739,    28,   -36,  4458,
    -402,  1889,  2288,  -402,  4739,   -34,  3653,     2,     4,   -34,
      40,  -402,    54,    14,  3741,  -402,   686,  -402,    -2,   -30,
      31,    31,  3485,  -402,    21,  3776,  4026,  -402,  1224,  -402,
    -402,  -402,  -402,  -402,  -402,  -402,  -402,  -402,  -402,  -402,
    -402,  -402,    35,   152,   118,  3618,  3618,  3618,  3618,  3618,
    3618,  3618,  3618,  3618,  3618,  3618,  3618,  3618,  3618,  3618,
    3618,  3618,  3618,  3653,    40,    40,    40,    40,    40,    40,
    3485,    40,  -402,  3485,   101,    26,  -402,  4776,  4739,  -402,
    4500,    30,    37,  3219,    41,    41,    41,  3485,  3485,  -402,
    3485,  3485,  3485,  3485,  3485,  3485,  3485,  3485,  3485,  3485,
    3485,  3485,  3485,  3485,  3485,  3485,  3485,  3485,  3485,  3485,
    3485,  3485,  3485,  3485,  3485,  3485,  3485,  3485,  3485,  3485,
    3485,  3485,  3485,  3485,  3653,   120,  -402,   121,  -402,   956,
      88,   108,    43,  -402,  3485,    10,  3903,  3485,  -402,   -32,
      45,   -97,  -402,  -402,   -32,  -402,  -402,  -402,  3485,  3485,
    -402,  3653,   -32,  -402,  3485,  -402,   162,  -402,    38,  -402,
      46,  -402,  3352,  3901,  -402,   162,    23,  1357,  -402,  3485,
      47,  2687,    87,  3800,    29,    29,    29,   277,    53,    53,
      53,    53,    39,    39,    39,    39,    29,    39,    31,     8,
    -402,  3867,  4739,  -402,  3219,  -402,  3485,  3485,    58,  3485,
    -402,   130,  3992,  -402,  4739,    90,  4830,  4867,  4867,  4921,
    4921,   809,  4958,  4958,  4958,  4958,  4958,  4958,  4958,  4050,
    4050,  4050,   507,   507,   507,   659,    79,    79,    79,    79,
      57,    57,    57,    57,   507,    57,    41,    24,  -402,  2687,
    2687,    56,    60,    63,  -402,  -402,    45,  3485,   154,  2022,
    -402,  4739,    62,   229,   232,  -402,  -402,  -402,  -402,  -402,
    -402,  -402,  -402,  -402,  -402,  -402,  -402,  -402,  -402,  -402,
    -402,  -402,  -402,  -402,  -402,  -402,  -402,  -402,  -402,  -402,
    -402,  -402,  -402,  -402,  -402,   -34,  4401,   163,   235,  -402,
     167,  -402,  1756,   166,  4739,  4739,  -402,   166,   175,   195,
    -402,   119,  -402,   199,   178,   124,  3485,  3485,  -402,  -402,
    3485,   128,    40,  -402,  4739,  2022,  -402,  -402,  3485,  -402,
    4739,  3485,  4552,  -402,  3485,  -402,  -402,  -402,  -402,  2022,
    -402,  4739,  2288,  -402,  3485,  -402,   -27,  -402,   -90,   -32,
    -402,    27,  3485,  -402,  3485,   186,  -402,  -402,  3485,  -402,
    -402,  3485,  -402,   162,  -402,  3485,  3485,   262,  -402,   131,
    4117,  -402,   136,  4151,  4242,  3485,  4276,  -402,  -402,  4367,
    -402,   265,  -402,   268,   166,  3485,  3485,  4739,  4739,  3485,
    4739,  4739,  -402,   220,  4739,  4594,  -402,  3485,  -402,   -32,
    -402,  -402,  4739,  -402,  -402,  -402,  -402,  -402,  4685,  4648,
    4739,  3485,  3485,   166,  -402,  3485,  4739,  4739,  -402,  4739
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       0,     0,   179,   178,   181,   174,   198,     0,    96,    97,
      88,    11,    88,    88,   182,    94,     0,     0,    91,     0,
       0,    92,    88,     0,     0,   180,    90,     0,     0,    89,
       0,     0,   286,    93,     0,   176,     0,     0,     0,     0,
     237,   238,   239,   240,   241,   242,   243,   244,   245,   246,
     247,   248,   249,   250,   251,   252,   253,   254,     0,   255,
     256,   257,   259,   260,   261,   262,   258,   264,   263,     0,
       0,     0,     2,    13,    88,     5,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,   286,     0,    78,     0,
      81,    95,   108,   170,   171,   183,   185,   187,   189,   191,
     193,     0,   268,   196,   195,    88,     0,     0,     0,   175,
     174,     0,     0,     0,     0,     0,   106,   131,   199,    15,
      89,     0,     0,    63,    88,     0,    49,    38,     0,     0,
      35,    88,    88,    82,    54,    63,     0,     0,   287,    63,
     177,   221,     0,    70,   106,   223,     0,   230,     0,     0,
     129,   130,     0,   204,     0,   106,     0,     1,    14,     4,
      12,     6,    34,    29,    27,    32,    26,    28,    31,    30,
      33,     9,    36,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   184,   186,   188,   190,   192,   194,
       0,   197,    10,   102,     0,    70,    99,   101,    48,   267,
     272,     0,    70,     0,   168,   169,   167,     0,     0,   200,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    84,     0,    83,     0,
      59,     0,    70,    76,     0,     0,     0,     0,   279,   286,
       0,     0,   278,    80,   286,   288,   289,   290,     0,     0,
      51,     0,   286,   222,    71,   105,     0,   227,     0,   226,
       0,   224,     0,     0,   205,     0,   172,    88,     7,     0,
      74,     0,   128,   127,   110,   111,   112,   113,   117,   118,
     124,   125,   119,   120,   121,   122,   115,   126,   123,   116,
     109,     0,   103,   202,    71,    98,   104,     0,   269,    71,
     271,     0,     0,   201,   107,   166,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   165,   145,
     146,   147,   148,   149,   150,   151,   155,   156,   162,   163,
     157,   158,   159,   160,   153,   164,   161,   154,   132,     0,
       0,     0,     0,    70,    68,    72,    73,     0,     0,    71,
      75,    50,     0,    42,    45,   292,   293,   294,   295,   296,
     297,   298,   299,   300,   301,   302,   303,   304,   305,   306,
     307,   308,   309,   310,   311,   312,   313,   314,   316,   317,
     318,   319,   315,   320,   321,    63,   235,   284,     0,   283,
       0,   282,    88,    61,    53,    52,   291,    61,   219,     0,
     207,    70,   209,   210,     0,    70,   228,     0,   225,   231,
       0,     0,   173,     8,    37,    66,    85,   266,     0,   100,
     273,     0,   274,   203,     0,    87,    86,    65,    64,    71,
      67,    60,    88,    77,     0,    43,     0,    46,     0,   286,
     233,     0,     0,    74,     0,     0,   281,   280,     0,    55,
      56,     0,   232,    71,   208,     0,     0,    71,   218,     0,
       0,   206,     0,     0,     0,     0,     0,    69,    79,     0,
      40,     0,    39,     0,    61,     0,     0,   285,   276,     0,
      62,   217,   212,   213,   211,   215,   220,   229,   114,   286,
     265,   270,   275,   152,    41,    44,    47,    57,     0,     0,
     277,     0,     0,    61,   234,     0,   214,   216,    58,   236
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -402,  -402,  -402,  -402,   117,  -402,   -62,   267,  -402,  -402,
    -402,  -402,  -402,  -402,  -123,  -402,  -402,  -402,  -402,  -402,
    -401,  -121,  -167,  -402,  -185,  -178,  -125,  -402,  -402,   -17,
    -130,    49,   -22,    69,  -402,   -41,   -35,    18,   342,   -19,
     317,  -117,  -112,    67,   -25,  -402,  -402,    -7,  -402,  -402,
    -197,  -196,  -402,  -402,  -402,  -402,  -402,  -137,  -402,  -402,
    -402,  -402,  -402,  -402,  -402,  -402,  -402,  -402,  -402,  -133,
     -83,  -170,  -402,  -402
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    71,    72,    73,    74,   159,    75,    76,   171,    77,
      78,   466,   468,    79,    80,    81,    82,    83,    84,    85,
     479,   260,   372,   373,   285,   374,    86,   261,   262,    87,
      88,    89,    90,   204,   205,   206,   147,   143,    91,   116,
     117,    93,    94,   118,   109,    95,    96,   429,   430,   431,
     432,   433,   434,   435,    97,    98,   148,   149,    99,   100,
     471,   101,   102,   103,   211,   212,   104,   271,   422,   272,
     137,   277,   138,   415
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     126,   142,   273,   172,   129,   128,   269,   133,   268,   288,
     140,   134,   161,   264,   274,   175,   382,   144,   282,   275,
     325,   218,   219,   320,   276,   115,   480,   330,   106,   105,
     291,   221,   500,   120,   154,   419,   175,   420,   175,   123,
     421,   278,   279,   202,   502,   280,   175,   503,   221,   106,
     155,   156,     6,     7,   505,   506,   124,   127,   289,   131,
     175,   121,   122,   130,   221,   135,   132,   383,   136,   107,
     194,   195,   196,   197,   198,   199,   275,   380,   201,   139,
     106,   276,   152,   157,   368,   158,   221,   207,   208,   210,
     173,   200,   214,   215,   216,   174,   298,   213,   217,   256,
     258,   259,   266,   527,   281,   265,   108,   263,   193,   292,
     501,   426,   299,   283,   270,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   254,   190,   191,   108,   192,   193,
     192,   193,   538,   293,   375,   290,   191,   275,   192,   193,
     253,   254,   276,   384,   185,   186,   187,   188,   220,   190,
     191,   284,   192,   193,   252,   294,   253,   254,   300,   301,
     323,   369,   370,   324,   275,   377,   328,   378,   428,   276,
     246,   247,   248,   249,   329,   251,   252,   436,   253,   254,
     379,   321,   445,   418,   322,   437,   417,   193,   460,   453,
     254,   423,   457,   451,   207,   462,   458,   464,   332,   427,
     459,   334,   335,   336,   337,   338,   339,   340,   341,   342,
     343,   344,   345,   346,   347,   348,   349,   350,   351,   352,
     353,   354,   355,   356,   357,   358,   359,   360,   361,   362,
     363,   364,   365,   366,   367,   443,   115,   465,   467,   162,
     472,   473,   376,   478,   163,   381,   484,   474,   416,   164,
     488,   165,   481,   166,   482,   485,   483,   439,   486,   424,
     425,   487,   491,   167,   168,   334,   509,   169,   516,   170,
     517,   442,   519,   525,   526,   297,   531,   119,   492,   446,
     444,   497,   331,   449,   175,   333,   512,   513,   441,   477,
       0,     0,     0,     0,   469,     0,     0,   269,     0,   476,
     489,     0,     0,     0,     0,   207,     0,   355,   450,     0,
     452,     0,     0,     0,     0,     0,     0,    92,     0,     0,
     375,     0,     0,     0,     0,     0,     0,    92,     0,     0,
       0,     0,   498,     0,   375,     0,     0,     0,     0,    92,
       0,     0,     0,     0,     0,     0,    92,   455,   456,     0,
       0,     0,     0,     0,     0,    92,    92,     0,   461,     0,
       0,     0,   463,   -89,   181,   182,   183,   184,   185,   186,
     187,   188,     0,   190,   191,     0,   192,   193,     0,     0,
     150,   151,     0,     0,     0,     0,   504,     0,     0,     0,
       0,    92,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   439,     0,     0,     0,   270,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   490,    92,     0,     0,     0,     0,     0,   376,   493,
       0,     0,   494,     0,     0,   496,   533,     0,    92,    92,
       0,    92,   376,     0,     0,   499,     0,     0,    92,    92,
       0,     0,     0,   507,     0,   508,     0,     0,     0,   510,
       0,     0,   511,     0,     0,     0,   514,   515,     0,     0,
       0,     0,     0,     0,     0,    92,   522,     0,     0,     0,
       0,     0,   439,     0,     0,     0,   528,   529,     0,     0,
     530,     0,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
       0,     0,   536,   537,   221,     0,   539,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,     2,     3,     4,   110,     0,
       6,     7,     0,     0,     0,     0,     0,     0,     0,     0,
      14,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    23,     0,    25,     0,    27,
       0,     0,     0,     0,     0,     0,    92,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    35,     0,     0,    36,
      37,     0,   145,   241,   242,   243,   244,   245,   246,   247,
     248,   249,     0,   251,   252,     0,   253,   254,     0,     0,
       0,     0,     0,     0,    92,     0,     0,     0,    92,   111,
     112,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     113,     0,     0,     0,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,   114,    59,    60,    61,    62,    63,    64,    65,
      66,    67,     0,    68,     0,    69,   221,    70,     0,     0,
       0,   146,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    92,    92,     0,     2,
       3,     4,   110,     0,     6,     7,    92,     0,     0,     0,
       0,     0,     0,     0,    14,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    23,
       0,    25,     0,    27,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    92,
      35,     0,     0,    36,    37,   -89,   242,   243,   244,   245,
     246,   247,   248,   249,     0,   251,   252,     0,   253,   254,
       0,     0,    92,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   111,   112,     0,    92,     0,     0,    92,
       0,     0,     0,     0,   113,     0,     0,     0,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,   114,    59,    60,    61,
      62,    63,    64,    65,    66,    67,   221,    68,     0,    69,
       0,    70,     0,    -3,     1,   287,     2,     3,     4,     5,
       0,     6,     7,     0,     0,     8,     9,    10,    11,    12,
      13,    14,    15,    16,     0,    17,    18,     0,    19,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
      27,    28,     0,     0,     0,    29,     0,    30,    31,     0,
     -88,    32,    33,    34,     0,     0,     0,    35,     0,     0,
      36,    37,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,   245,
     246,   247,   248,   249,   250,   251,   252,     0,   253,   254,
      38,    39,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,     0,    68,     0,    69,   371,    70,     2,
       3,     4,   110,     0,     6,     7,     0,     0,     8,     9,
       0,     0,    12,    13,    14,    15,     0,     0,    17,    18,
       0,     0,     0,     0,     0,     0,     0,    21,     0,    23,
       0,    25,    26,    27,    28,     0,     0,     0,    29,     0,
       0,     0,     0,   -88,     0,    33,     0,     0,     0,     0,
      35,     0,     0,    36,    37,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    38,    39,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,     0,    68,     0,    69,
       0,    70,   -66,     2,     3,     4,   110,     0,     6,     7,
       0,     0,     0,     0,     0,     0,     0,     0,    14,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    23,     0,    25,     0,    27,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    35,     0,     0,    36,    37,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   111,   112,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   113,     0,
       0,     0,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
     114,    59,    60,    61,    62,    63,    64,    65,    66,    67,
       0,    68,     0,    69,     0,    70,   209,     2,     3,     4,
       5,     0,     6,     7,     0,     0,     8,     9,    10,    11,
      12,    13,    14,    15,    16,     0,    17,    18,     0,    19,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,    27,    28,     0,     0,     0,    29,     0,    30,    31,
       0,   -88,    32,    33,    34,     0,     0,     0,    35,     0,
       0,    36,    37,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    38,    39,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,     0,    68,     0,    69,     0,    70,
       2,     3,     4,     5,     0,     6,     7,     0,     0,     8,
       9,    10,   160,    12,    13,    14,    15,    16,     0,    17,
      18,     0,    19,     0,     0,     0,     0,    20,    21,    22,
      23,    24,    25,    26,    27,    28,     0,     0,     0,    29,
       0,    30,    31,     0,     0,    32,    33,    34,     0,     0,
       0,    35,     0,     0,    36,    37,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    38,    39,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,     0,    68,     0,
      69,     0,    70,     2,     3,     4,     5,     0,     6,     7,
       0,     0,     8,     9,    10,     0,    12,    13,    14,    15,
      16,     0,    17,    18,     0,    19,     0,     0,     0,     0,
      20,    21,    22,    23,    24,    25,    26,    27,    28,     0,
       0,     0,    29,     0,    30,    31,     0,     0,    32,    33,
      34,     0,     0,     0,    35,     0,     0,    36,    37,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    38,    39,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
       0,    68,     0,    69,     0,    70,     2,     3,     4,     5,
       0,     6,     7,     0,     0,     8,     9,     0,     0,    12,
      13,    14,    15,    16,     0,    17,    18,     0,    19,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
      27,    28,     0,     0,     0,    29,     0,    30,    31,     0,
       0,    32,    33,    34,     0,     0,     0,    35,     0,     0,
      36,    37,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      38,    39,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,     0,    68,     0,    69,     0,    70,     2,
       3,     4,   110,     0,     6,     7,     0,     0,     8,     9,
       0,     0,    12,    13,    14,    15,     0,     0,    17,    18,
       0,    19,     0,     0,     0,     0,     0,    21,     0,    23,
       0,    25,    26,    27,    28,     0,     0,     0,    29,     0,
       0,     0,     0,     0,     0,    33,     0,     0,     0,     0,
      35,     0,     0,    36,    37,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    38,    39,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,     0,    68,     0,    69,
     475,    70,     2,     3,     4,   110,     0,     6,     7,     0,
       0,     8,     9,     0,     0,    12,    13,    14,    15,     0,
       0,    17,    18,     0,    19,     0,     0,     0,     0,     0,
      21,     0,    23,     0,    25,    26,    27,    28,     0,     0,
       0,    29,     0,     0,     0,     0,     0,     0,    33,     0,
       0,     0,     0,    35,     0,     0,    36,    37,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    38,    39,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,     0,
      68,     0,    69,     0,    70,     2,     3,     4,   110,     0,
       6,     7,     0,     0,     8,     9,     0,     0,    12,    13,
      14,    15,     0,     0,    17,    18,     0,     0,     0,     0,
       0,     0,     0,    21,     0,    23,     0,    25,    26,    27,
      28,     0,     0,     0,    29,     0,     0,     0,     0,   -88,
       0,    33,     0,     0,     0,     0,    35,     0,     0,    36,
      37,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    38,
      39,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,     0,    68,     0,    69,     0,    70,     2,     3,
       4,   110,     0,     6,     7,     0,     0,     8,     9,     0,
       0,    12,    13,    14,    15,     0,     0,    17,    18,     0,
       0,     0,     0,     0,     0,     0,    21,     0,    23,     0,
      25,    26,    27,    28,     0,     0,     0,    29,     0,     0,
       0,     0,     0,     0,    33,     0,     0,     0,     0,    35,
       0,     0,    36,    37,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    38,    39,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,     0,    68,     0,    69,     0,
      70,     2,     3,     4,   110,     0,     6,     7,     0,     0,
       8,     9,     0,     0,    12,    13,    14,    15,     0,     0,
       0,    18,     0,     0,     0,     0,     0,     0,     0,    21,
       0,    23,     0,    25,    26,    27,     0,     0,     0,     0,
      29,     0,     0,     0,     0,     0,     0,    33,     0,     0,
       0,     0,    35,     0,     0,    36,    37,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    38,    39,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,     0,    68,
       0,    69,     0,    70,     2,     3,     4,   110,     0,     6,
       7,     0,     0,     8,     9,     0,     0,     0,     0,    14,
      15,     0,     0,     0,    18,     0,     0,     0,     0,     0,
       0,     0,    21,     0,    23,     0,    25,    26,    27,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   255,     0,
      33,     0,     0,     0,     0,    35,     0,     0,    36,    37,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    38,    39,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,     0,    68,     0,    69,     0,    70,     2,     3,     4,
     110,     0,     6,     7,     0,     0,     8,     9,     0,     0,
       0,     0,    14,    15,     0,     0,     0,    18,     0,     0,
       0,     0,     0,     0,     0,    21,     0,    23,     0,    25,
      26,    27,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   257,     0,    33,     0,     0,     0,     0,    35,     0,
       0,    36,    37,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    38,    39,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,     0,    68,     0,    69,     0,    70,
       2,     3,     4,   110,     0,     6,     7,     0,     0,     8,
       9,     0,     0,     0,     0,    14,    15,     0,     0,     0,
      18,     0,     0,     0,     0,     0,     0,     0,    21,     0,
      23,     0,    25,    26,    27,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    33,     0,     0,     0,
       0,    35,     0,     0,    36,    37,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    38,    39,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,     0,    68,     0,
      69,     0,    70,     2,     3,     4,   110,     0,     6,     7,
       0,     0,     0,     0,     0,     0,     0,     0,    14,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    23,     0,    25,     0,    27,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    35,     0,     0,    36,    37,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   111,   112,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   113,     0,
     125,     0,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
     114,    59,    60,    61,    62,    63,    64,    65,    66,    67,
       0,    68,     0,    69,     0,    70,     2,     3,     4,   110,
       0,     6,     7,     0,     0,     0,     0,     0,     0,     0,
       0,    14,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    23,     0,    25,     0,
      27,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    35,     0,     0,
      36,    37,   141,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     111,   112,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   113,     0,     0,     0,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,   114,    59,    60,    61,    62,    63,    64,
      65,    66,    67,     0,    68,     0,    69,     0,    70,     2,
       3,     4,   110,     0,     6,     7,     0,     0,     0,     0,
       0,     0,     0,     0,    14,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    23,
       0,    25,     0,    27,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      35,     0,     0,    36,    37,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   111,   112,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   113,     0,     0,     0,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,   114,    59,    60,    61,
      62,    63,    64,    65,    66,    67,     0,    68,     0,    69,
     153,    70,     2,     3,     4,   110,     0,     6,     7,     0,
       0,     0,     0,     0,     0,     0,     0,    14,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    23,     0,    25,     0,    27,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    35,     0,     0,    36,    37,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   203,   111,   112,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   113,     0,     0,
       0,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,   114,
      59,    60,    61,    62,    63,    64,    65,    66,    67,     0,
      68,     0,    69,     0,    70,     2,     3,     4,   110,     0,
       6,     7,     0,     0,     0,     0,     0,     0,     0,     0,
      14,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    23,     0,    25,     0,    27,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    35,     0,     0,    36,
      37,     0,   438,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   111,
     112,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     113,     0,     0,     0,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,   114,    59,    60,    61,    62,    63,    64,    65,
      66,    67,     0,    68,     0,    69,     0,    70,     2,     3,
       4,   110,     0,     6,     7,     0,     0,     0,     0,     0,
       0,     0,     0,    14,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    23,     0,
      25,     0,    27,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    35,
       0,     0,    36,    37,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   111,   112,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   113,     0,     0,     0,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,   114,    59,    60,    61,    62,
      63,    64,    65,    66,    67,     0,    68,     0,    69,     0,
      70,     2,     3,     4,   110,     0,     6,     7,     0,     0,
       0,     0,     0,     0,     0,     0,    14,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    23,     0,    25,     0,    27,     2,     3,     4,   110,
       0,     6,     7,     0,     0,     0,     0,     0,     0,     0,
       0,    14,    35,     0,     0,    36,    37,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    23,     0,    25,     0,
      27,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    38,    39,    35,     0,     0,
      36,    37,     0,   175,     0,     0,     0,     0,     0,     0,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,   221,    68,
       0,    69,     0,    70,     0,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,     0,    59,    60,    61,    62,    63,    64,
      65,    66,    67,   221,    68,   176,    69,     0,    70,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,     0,   192,   193,   175,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     244,   245,   246,   247,   248,   249,   250,   251,   252,     0,
     253,   254,     0,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   243,   244,   245,   246,   247,   248,
     249,   250,   251,   252,   221,   253,   254,     0,     0,   -89,
     286,     0,     0,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,     0,   192,
     193,     0,     0,     0,     0,     0,     0,     0,   221,   385,
       0,     0,     0,     0,     0,   295,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,   244,   245,   246,   247,
     248,   249,   250,   251,   252,     0,   253,   254,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     244,   245,   246,   247,   248,   249,   250,   251,   252,   221,
     253,   254,     0,   447,   448,   386,   387,   388,   389,   390,
     391,   392,   393,   394,   395,   396,   397,   398,   399,   400,
     401,   402,   403,   404,   405,   406,   407,   408,   409,   410,
     411,   412,   413,   221,   414,     0,     0,     0,   440,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   221,     0,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
       0,   253,   254,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   243,   244,   245,   246,   247,   248,
     249,   250,   251,   252,   221,   253,   254,     0,     0,   454,
     -89,   -89,   -89,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,     0,   253,
     254,     0,     0,     0,     0,     0,     0,     0,   221,     0,
       0,     0,   296,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,   244,   245,   246,   247,
     248,   249,   250,   251,   252,     0,   253,   254,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     244,   245,   246,   247,   248,   249,   250,   251,   252,   221,
     253,   254,     0,   518,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   221,     0,     0,     0,   520,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
       0,   253,   254,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   243,   244,   245,   246,   247,   248,
     249,   250,   251,   252,   221,   253,   254,     0,   521,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   221,     0,
       0,     0,   523,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     470,     0,     0,     0,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,   244,   245,   246,   247,
     248,   249,   250,   251,   252,   221,   253,   254,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     244,   245,   246,   247,   248,   249,   250,   251,   252,     0,
     253,   254,     0,   524,     0,     0,     0,   221,     0,   267,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,   238,   239,   240,   241,   242,   243,   244,   245,   246,
     247,   248,   249,   250,   251,   252,   327,   253,   254,   221,
       0,     0,     0,     0,     0,     0,     0,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,     0,   253,
     254,   221,     0,     0,     0,     0,     0,     0,   495,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
     532,   253,   254,     0,     0,   221,     0,     0,     0,     0,
       0,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
     241,   242,   243,   244,   245,   246,   247,   248,   249,   250,
     251,   252,   221,   253,   254,     0,     0,     0,     0,   535,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   534,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,   238,   239,   240,   241,   242,   243,   244,   245,   246,
     247,   248,   249,   250,   251,   252,   221,   253,   254,     0,
       0,     0,   222,   223,   224,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,   242,   243,   244,   245,   246,   247,   248,   249,
     250,   251,   252,   221,   253,   254,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,   245,
     246,   247,   248,   249,   250,   251,   252,   221,   253,   254,
       0,     0,     0,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   326,   242,   243,   244,   245,   246,   247,   248,
     249,   250,   251,   252,   221,   253,   254,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   221,   253,
     254,     0,     0,     0,     0,     0,     0,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,   244,   245,   246,   247,
     248,   249,   250,   251,   252,   221,   253,   254,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     244,   245,   246,   247,   248,   249,   250,   251,   252,     0,
     253,   254,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   -89,   -89,   -89,   -89,   -89,   -89,   -89,   235,   236,
     237,   238,   239,   240,   241,   242,   243,   244,   245,   246,
     247,   248,   249,   250,   251,   252,     0,   253,   254
};

static const yytype_int16 yycheck[] =
{
      19,    36,   132,    86,    23,    22,   131,    29,   131,   146,
      35,    30,    74,   125,   135,     7,     6,    36,   139,   136,
     205,    10,    11,   193,   136,     7,   427,   212,    57,   132,
      60,     7,    59,    42,    69,   132,     7,   134,     7,     6,
     137,    39,    40,   105,   134,    43,     7,   137,     7,    57,
      69,    70,     8,     9,    27,    28,    57,     6,    60,   133,
       7,    12,    13,     8,     7,     6,    41,    57,   100,    77,
      95,    96,    97,    98,    99,   100,   193,   262,   103,     6,
      57,   193,   135,     0,   254,   132,     7,   106,   107,   108,
     138,   135,   111,   112,   113,    47,   158,    57,   135,   121,
     122,   135,   138,   504,   100,    77,   135,   124,   100,   139,
     137,   281,    77,    59,   131,    86,    87,    88,    89,    90,
      91,    92,    93,    94,   100,    96,    97,   135,    99,   100,
      99,   100,   533,   152,   259,   137,    97,   254,    99,   100,
      99,   100,   254,   133,    91,    92,    93,    94,   137,    96,
      97,   137,    99,   100,    97,   134,    99,   100,     6,    41,
      59,    41,    41,   137,   281,    77,   136,    59,     6,   281,
      91,    92,    93,    94,   137,    96,    97,   139,    99,   100,
     137,   200,   135,   138,   203,   139,   269,   100,   373,    59,
     100,   274,   136,   135,   213,    41,   136,   135,   217,   282,
     137,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   243,   244,   245,   246,   247,   248,
     249,   250,   251,   252,   253,   297,   218,     8,     6,    20,
      77,     6,   259,    77,    25,   264,   431,    80,   267,    30,
     435,    32,    77,    34,    59,    56,   137,   292,    80,   278,
     279,   137,   134,    44,    45,   284,    80,    48,     6,    50,
     139,   296,   136,     8,     6,   158,    56,    10,   445,   301,
     299,   459,   213,   324,     7,   218,   483,   483,   295,   422,
      -1,    -1,    -1,    -1,   415,    -1,    -1,   422,    -1,   422,
     437,    -1,    -1,    -1,    -1,   324,    -1,   326,   327,    -1,
     329,    -1,    -1,    -1,    -1,    -1,    -1,     0,    -1,    -1,
     445,    -1,    -1,    -1,    -1,    -1,    -1,    10,    -1,    -1,
      -1,    -1,   462,    -1,   459,    -1,    -1,    -1,    -1,    22,
      -1,    -1,    -1,    -1,    -1,    -1,    29,   369,   370,    -1,
      -1,    -1,    -1,    -1,    -1,    38,    39,    -1,   377,    -1,
      -1,    -1,   379,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    -1,    96,    97,    -1,    99,   100,    -1,    -1,
      38,    39,    -1,    -1,    -1,    -1,   469,    -1,    -1,    -1,
      -1,    74,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   436,    -1,    -1,    -1,   422,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   440,   105,    -1,    -1,    -1,    -1,    -1,   445,   448,
      -1,    -1,   451,    -1,    -1,   454,   519,    -1,   121,   122,
      -1,   124,   459,    -1,    -1,   464,    -1,    -1,   131,   132,
      -1,    -1,    -1,   472,    -1,   474,    -1,    -1,    -1,   478,
      -1,    -1,   481,    -1,    -1,    -1,   485,   486,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   158,   495,    -1,    -1,    -1,
      -1,    -1,   517,    -1,    -1,    -1,   505,   506,    -1,    -1,
     509,    -1,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   192,
      -1,    -1,   531,   532,     7,    -1,   535,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    33,    -1,    35,    -1,    37,
      -1,    -1,    -1,    -1,    -1,    -1,   259,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,    -1,    57,
      58,    -1,    60,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    -1,    96,    97,    -1,    99,   100,    -1,    -1,
      -1,    -1,    -1,    -1,   297,    -1,    -1,    -1,   301,    87,
      88,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      98,    -1,    -1,    -1,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,    -1,   131,    -1,   133,     7,   135,    -1,    -1,
      -1,   139,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   369,   370,    -1,     3,
       4,     5,     6,    -1,     8,     9,   379,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    18,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,
      -1,    35,    -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   422,
      54,    -1,    -1,    57,    58,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    -1,    96,    97,    -1,    99,   100,
      -1,    -1,   445,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    87,    88,    -1,   459,    -1,    -1,   462,
      -1,    -1,    -1,    -1,    98,    -1,    -1,    -1,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,     7,   131,    -1,   133,
      -1,   135,    -1,     0,     1,   139,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    -1,    22,    23,    -1,    25,    -1,
      -1,    -1,    -1,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    -1,    -1,    -1,    42,    -1,    44,    45,    -1,
      47,    48,    49,    50,    -1,    -1,    -1,    54,    -1,    -1,
      57,    58,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    -1,    99,   100,
      87,    88,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,    -1,   131,    -1,   133,     1,   135,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    12,    13,
      -1,    -1,    16,    17,    18,    19,    -1,    -1,    22,    23,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,    33,
      -1,    35,    36,    37,    38,    -1,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    47,    -1,    49,    -1,    -1,    -1,    -1,
      54,    -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    87,    88,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,    -1,   131,    -1,   133,
      -1,   135,   136,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    33,    -1,    35,    -1,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,    88,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    98,    -1,
      -1,    -1,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
      -1,   131,    -1,   133,    -1,   135,   136,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    -1,    22,    23,    -1,    25,
      -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    -1,    -1,    -1,    42,    -1,    44,    45,
      -1,    47,    48,    49,    50,    -1,    -1,    -1,    54,    -1,
      -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    87,    88,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,    -1,   131,    -1,   133,    -1,   135,
       3,     4,     5,     6,    -1,     8,     9,    -1,    -1,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    -1,    22,
      23,    -1,    25,    -1,    -1,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    -1,    -1,    -1,    42,
      -1,    44,    45,    -1,    -1,    48,    49,    50,    -1,    -1,
      -1,    54,    -1,    -1,    57,    58,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    87,    88,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,    -1,   131,    -1,
     133,    -1,   135,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    12,    13,    14,    -1,    16,    17,    18,    19,
      20,    -1,    22,    23,    -1,    25,    -1,    -1,    -1,    -1,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    -1,
      -1,    -1,    42,    -1,    44,    45,    -1,    -1,    48,    49,
      50,    -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,    88,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
      -1,   131,    -1,   133,    -1,   135,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    12,    13,    -1,    -1,    16,
      17,    18,    19,    20,    -1,    22,    23,    -1,    25,    -1,
      -1,    -1,    -1,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    -1,    -1,    -1,    42,    -1,    44,    45,    -1,
      -1,    48,    49,    50,    -1,    -1,    -1,    54,    -1,    -1,
      57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      87,    88,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,    -1,   131,    -1,   133,    -1,   135,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    12,    13,
      -1,    -1,    16,    17,    18,    19,    -1,    -1,    22,    23,
      -1,    25,    -1,    -1,    -1,    -1,    -1,    31,    -1,    33,
      -1,    35,    36,    37,    38,    -1,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    -1,    -1,    49,    -1,    -1,    -1,    -1,
      54,    -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    87,    88,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,    -1,   131,    -1,   133,
     134,   135,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    12,    13,    -1,    -1,    16,    17,    18,    19,    -1,
      -1,    22,    23,    -1,    25,    -1,    -1,    -1,    -1,    -1,
      31,    -1,    33,    -1,    35,    36,    37,    38,    -1,    -1,
      -1,    42,    -1,    -1,    -1,    -1,    -1,    -1,    49,    -1,
      -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    87,    88,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,    -1,
     131,    -1,   133,    -1,   135,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    12,    13,    -1,    -1,    16,    17,
      18,    19,    -1,    -1,    22,    23,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    31,    -1,    33,    -1,    35,    36,    37,
      38,    -1,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,
      -1,    49,    -1,    -1,    -1,    -1,    54,    -1,    -1,    57,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,
      88,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,    -1,   131,    -1,   133,    -1,   135,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    12,    13,    -1,
      -1,    16,    17,    18,    19,    -1,    -1,    22,    23,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,    33,    -1,
      35,    36,    37,    38,    -1,    -1,    -1,    42,    -1,    -1,
      -1,    -1,    -1,    -1,    49,    -1,    -1,    -1,    -1,    54,
      -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    87,    88,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,    -1,   131,    -1,   133,    -1,
     135,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      12,    13,    -1,    -1,    16,    17,    18,    19,    -1,    -1,
      -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,
      -1,    33,    -1,    35,    36,    37,    -1,    -1,    -1,    -1,
      42,    -1,    -1,    -1,    -1,    -1,    -1,    49,    -1,    -1,
      -1,    -1,    54,    -1,    -1,    57,    58,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    87,    88,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,    -1,   131,
      -1,   133,    -1,   135,     3,     4,     5,     6,    -1,     8,
       9,    -1,    -1,    12,    13,    -1,    -1,    -1,    -1,    18,
      19,    -1,    -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    31,    -1,    33,    -1,    35,    36,    37,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    -1,
      49,    -1,    -1,    -1,    -1,    54,    -1,    -1,    57,    58,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,    88,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,    -1,   131,    -1,   133,    -1,   135,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    12,    13,    -1,    -1,
      -1,    -1,    18,    19,    -1,    -1,    -1,    23,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    31,    -1,    33,    -1,    35,
      36,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    47,    -1,    49,    -1,    -1,    -1,    -1,    54,    -1,
      -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    87,    88,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,    -1,   131,    -1,   133,    -1,   135,
       3,     4,     5,     6,    -1,     8,     9,    -1,    -1,    12,
      13,    -1,    -1,    -1,    -1,    18,    19,    -1,    -1,    -1,
      23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,
      33,    -1,    35,    36,    37,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    49,    -1,    -1,    -1,
      -1,    54,    -1,    -1,    57,    58,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    87,    88,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,    -1,   131,    -1,
     133,    -1,   135,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    33,    -1,    35,    -1,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,    88,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    98,    -1,
     100,    -1,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
      -1,   131,    -1,   133,    -1,   135,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,    35,    -1,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,    -1,
      57,    58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      87,    88,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    98,    -1,    -1,    -1,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,    -1,   131,    -1,   133,    -1,   135,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    18,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,
      -1,    35,    -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      54,    -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    87,    88,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    98,    -1,    -1,    -1,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,    -1,   131,    -1,   133,
     134,   135,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    33,    -1,    35,    -1,    37,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    87,    88,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    98,    -1,    -1,
      -1,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,    -1,
     131,    -1,   133,    -1,   135,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    33,    -1,    35,    -1,    37,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,    -1,    57,
      58,    -1,    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,
      88,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      98,    -1,    -1,    -1,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,    -1,   131,    -1,   133,    -1,   135,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    18,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,
      35,    -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,
      -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    87,    88,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    98,    -1,    -1,    -1,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,    -1,   131,    -1,   133,    -1,
     135,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    33,    -1,    35,    -1,    37,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    18,    54,    -1,    -1,    57,    58,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,    35,    -1,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    87,    88,    54,    -1,    -1,
      57,    58,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,     7,   131,
      -1,   133,    -1,   135,    -1,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,    -1,   121,   122,   123,   124,   125,   126,
     127,   128,   129,     7,   131,    79,   133,    -1,   135,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    -1,    99,   100,     7,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    -1,
      99,   100,    -1,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,     7,    99,   100,    -1,    -1,    79,
     139,    -1,    -1,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    -1,    99,
     100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,     6,
      -1,    -1,    -1,    -1,    -1,   139,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    -1,    99,   100,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,     7,
      99,   100,    -1,   136,   137,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,     7,   131,    -1,    -1,    -1,   137,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      -1,    99,   100,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,     7,    99,   100,    -1,    -1,   137,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    -1,    99,
     100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,
      -1,    -1,   136,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    -1,    99,   100,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,     7,
      99,   100,    -1,   136,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     7,    -1,    -1,    -1,   136,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      -1,    99,   100,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,     7,    99,   100,    -1,   136,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,
      -1,    -1,   136,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      29,    -1,    -1,    -1,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,     7,    99,   100,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    -1,
      99,   100,    -1,   136,    -1,    -1,    -1,     7,    -1,    51,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    56,    99,   100,     7,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    -1,    99,
     100,     7,    -1,    -1,    -1,    -1,    -1,    -1,    56,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      56,    99,   100,    -1,    -1,     7,    -1,    -1,    -1,    -1,
      -1,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,     7,    99,   100,    -1,    -1,    -1,    -1,    51,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    29,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,     7,    99,   100,    -1,
      -1,    -1,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,     7,    99,   100,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,     7,    99,   100,
      -1,    -1,    -1,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,     7,    99,   100,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,     7,    99,
     100,    -1,    -1,    -1,    -1,    -1,    -1,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,     7,    99,   100,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    -1,
      99,   100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    -1,    99,   100
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     4,     5,     6,     8,     9,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    22,    23,    25,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    42,
      44,    45,    48,    49,    50,    54,    57,    58,    87,    88,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   131,   133,
     135,   141,   142,   143,   144,   146,   147,   149,   150,   153,
     154,   155,   156,   157,   158,   159,   166,   169,   170,   171,
     172,   178,   180,   181,   182,   185,   186,   194,   195,   198,
     199,   201,   202,   203,   206,   132,    57,    77,   135,   184,
       6,    87,    88,    98,   120,   177,   179,   180,   183,   147,
      42,   171,   171,     6,    57,   100,   179,     6,   169,   179,
       8,   133,    41,   172,   179,     6,   100,   210,   212,     6,
     184,    59,   176,   177,   179,    60,   139,   176,   196,   197,
     178,   178,   135,   134,   176,   179,   179,     0,   132,   145,
      15,   146,    20,    25,    30,    32,    34,    44,    45,    48,
      50,   148,   210,   138,    47,     7,    79,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    99,   100,   184,   184,   184,   184,   184,   184,
     135,   184,   146,    86,   173,   174,   175,   179,   179,   136,
     179,   204,   205,    57,   179,   179,   179,   135,    10,    11,
     137,     7,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    99,   100,    47,   172,    47,   172,   135,
     161,   167,   168,   169,   182,    77,   138,    51,   154,   166,
     169,   207,   209,   170,   161,   181,   182,   211,    39,    40,
      43,   100,   161,    59,   137,   164,   139,   139,   197,    60,
     137,    60,   139,   179,   134,   139,   136,   144,   146,    77,
       6,    41,   178,   178,   178,   178,   178,   178,   178,   178,
     178,   178,   178,   178,   178,   178,   178,   178,   178,   178,
     211,   179,   179,    59,   137,   164,    86,    56,   136,   137,
     164,   173,   179,   183,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   211,    41,
      41,     1,   162,   163,   165,   166,   169,    77,    59,   137,
     164,   179,     6,    57,   133,     6,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   131,   213,   179,   210,   138,   132,
     134,   137,   208,   210,   179,   179,   211,   210,     6,   187,
     188,   189,   190,   191,   192,   193,   139,   139,    60,   176,
     137,   187,   184,   146,   179,   135,   172,   136,   137,   175,
     179,   135,   179,    59,   137,   172,   172,   136,   136,   137,
     164,   179,    41,   169,   135,     8,   151,     6,   152,   161,
      29,   200,    77,     6,    80,   134,   154,   209,    77,   160,
     160,    77,    59,   137,   164,    56,    80,   137,   164,   197,
     179,   134,   162,   179,   179,    56,   179,   165,   170,   179,
      59,   137,   134,   137,   210,    27,    28,   179,   179,    80,
     179,   179,   190,   191,   179,   179,     6,   139,   136,   136,
     136,   136,   179,   136,   136,     8,     6,   160,   179,   179,
     179,    56,    56,   210,    29,    51,   179,   179,   160,   179
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   140,   141,   142,   142,   143,   143,   143,   143,   143,
     143,   144,   144,   145,   145,   146,   146,   147,   147,   147,
     147,   147,   147,   147,   147,   147,   148,   148,   148,   148,
     148,   148,   148,   148,   148,   149,   150,   150,   150,   150,
     150,   150,   151,   151,   151,   152,   152,   152,   153,   154,
     154,   155,   155,   155,   156,   157,   157,   158,   158,   159,
     159,   160,   160,   161,   161,   161,   162,   162,   163,   163,
     164,   164,   165,   165,   166,   167,   168,   168,   169,   169,
     169,   170,   170,   170,   170,   170,   170,   170,   171,   171,
     172,   172,   172,   172,   172,   172,   172,   172,   173,   174,
     174,   175,   175,   175,   175,   176,   177,   177,   178,   178,
     178,   178,   178,   178,   178,   178,   178,   178,   178,   178,
     178,   178,   178,   178,   178,   178,   178,   178,   178,   178,
     178,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     180,   180,   181,   181,   181,   181,   181,   181,   181,   181,
     181,   181,   181,   181,   181,   181,   181,   181,   181,   181,
     181,   181,   181,   181,   181,   181,   181,   181,   182,   182,
     183,   183,   184,   184,   185,   185,   186,   187,   188,   189,
     189,   189,   189,   189,   189,   190,   190,   191,   192,   193,
     193,   194,   194,   195,   195,   195,   195,   196,   196,   196,
     197,   197,   198,   199,   199,   200,   200,   201,   201,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     201,   201,   201,   201,   201,   202,   202,   203,   203,   203,
     203,   204,   205,   205,   205,   205,   206,   206,   207,   207,
     207,   207,   208,   208,   209,   209,   210,   210,   211,   211,
     212,   212,   213,   213,   213,   213,   213,   213,   213,   213,
     213,   213,   213,   213,   213,   213,   213,   213,   213,   213,
     213,   213,   213,   213,   213,   213,   213,   213,   213,   213,
     213,   213
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     2,     3,     4,     2,
       3,     1,     2,     0,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     4,     2,     6,
       6,     7,     0,     1,     3,     0,     1,     3,     3,     2,
       4,     3,     4,     4,     2,     5,     5,     7,     8,     3,
       5,     0,     2,     0,     3,     3,     0,     2,     1,     3,
       0,     1,     1,     1,     3,     2,     1,     3,     1,     6,
       3,     1,     2,     3,     3,     4,     5,     5,     0,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       3,     1,     1,     2,     2,     2,     1,     3,     1,     3,
       3,     3,     3,     3,     6,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     6,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     2,     2,
       1,     1,     3,     4,     1,     2,     1,     2,     1,     1,
       1,     1,     1,     1,     2,     1,     2,     1,     2,     1,
       2,     1,     2,     1,     2,     1,     1,     2,     1,     2,
       2,     3,     3,     4,     2,     3,     5,     1,     2,     1,
       1,     3,     3,     3,     5,     3,     5,     3,     2,     1,
       3,     2,     3,     2,     3,     4,     3,     2,     3,     5,
       1,     3,     5,     5,     8,     0,     5,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     6,     4,     3,     1,     4,
       7,     2,     1,     3,     3,     5,     6,     7,     1,     1,
       3,     3,     1,     1,     2,     4,     0,     1,     1,     1,
       2,     3,     1,     1,     1,     1,     1,     1,     1,     1,
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
/* The lookahead symbol.  */
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
    int yynerrs;

    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize;

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
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

  yynerrs = 0;
  yystate = 0;
  yyerrstatus = 0;

  yystacksize = YYINITDEPTH;
  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;


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
  case 5:
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[0].item)) {
          pp->model->addItem((yyvsp[0].item));
          GC::unlock();
          GC::lock();
        }
      }
    break;

  case 6:
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[0].item)) {
          pp->model->addItem((yyvsp[0].item));
          GC::unlock();
          GC::lock();
        }
      }
    break;

  case 7:
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[0].item)) {
          pp->model->addItem((yyvsp[0].item));
          GC::unlock();
          GC::lock();
        }
      }
    break;

  case 8:
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[0].item)) {
          pp->model->addItem((yyvsp[0].item));
          GC::unlock();
          GC::lock();
        }
      }
    break;

  case 9:
{ yyerror(&(yylsp[0]), parm, "unexpected item, expecting ';' or end of file"); YYERROR; }
    break;

  case 11:
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if (pp->parseDocComments && (yyvsp[0].sValue)) {
          pp->model->addDocComment((yyvsp[0].sValue));
        }
        free((yyvsp[0].sValue));
      }
    break;

  case 12:
      {
        ParserState* pp = static_cast<ParserState*>(parm);
        if (pp->parseDocComments && (yyvsp[0].sValue)) {
          pp->model->addDocComment((yyvsp[0].sValue));
        }
        free((yyvsp[0].sValue));
      }
    break;

  case 15:
      { (yyval.item) = (yyvsp[0].item);
        ParserState* pp = static_cast<ParserState*>(parm);
        if (FunctionI* fi = Item::dyn_cast<FunctionI>((yyval.item))) {
          if (pp->parseDocComments) {
            fi->ann().add(createDocComment((yylsp[-1]),(yyvsp[-1].sValue)));
          }
        } else if (VarDeclI* vdi = Item::dyn_cast<VarDeclI>((yyval.item))) {
          if (pp->parseDocComments) {
            vdi->e()->addAnnotation(createDocComment((yylsp[-1]),(yyvsp[-1].sValue)));
          }
        } else {
          yyerror(&(yylsp[0]), parm, "documentation comments are only supported for function, predicate and variable declarations");
        }
        free((yyvsp[-1].sValue));
      }
    break;

  case 16:
      { (yyval.item) = (yyvsp[0].item); }
    break;

  case 17:
      { (yyval.item)=notInDatafile(&(yyloc),parm,"include") ? (yyvsp[0].item) : NULL; }
    break;

  case 18:
      { (yyval.item)=notInDatafile(&(yyloc),parm,"variable declaration") ? (yyvsp[0].item) : NULL; }
    break;

  case 20:
      { (yyval.item)=notInDatafile(&(yyloc),parm,"constraint") ? (yyvsp[0].item) : NULL; }
    break;

  case 21:
      { (yyval.item)=notInDatafile(&(yyloc),parm,"solve") ? (yyvsp[0].item) : NULL; }
    break;

  case 22:
      { (yyval.item)=notInDatafile(&(yyloc),parm,"output") ? (yyvsp[0].item) : NULL; }
    break;

  case 23:
      { (yyval.item)=notInDatafile(&(yyloc),parm,"predicate") ? (yyvsp[0].item) : NULL; }
    break;

  case 24:
      { (yyval.item)=notInDatafile(&(yyloc),parm,"predicate") ? (yyvsp[0].item) : NULL; }
    break;

  case 25:
      { (yyval.item)=notInDatafile(&(yyloc),parm,"annotation") ? (yyvsp[0].item) : NULL; }
    break;

  case 35:
      { ParserState* pp = static_cast<ParserState*>(parm);
        map<string,Model*>::iterator ret = pp->seenModels.find((yyvsp[0].sValue));
        IncludeI* ii = new IncludeI((yyloc),ASTString((yyvsp[0].sValue)));
        (yyval.item) = ii;
        if (ret == pp->seenModels.end()) {
          Model* im = new Model;
          im->setParent(pp->model);
          im->setFilename((yyvsp[0].sValue));
          string fpath = FileUtils::dir_name(pp->filename);
          string fbase = FileUtils::base_name(pp->filename);
          if (fpath=="")
            fpath="./";
          ParseWorkItem pm(im, ii, fpath, (yyvsp[0].sValue));
          pp->files.push_back(pm);
          ii->m(im);
          pp->seenModels.insert(pair<string,Model*>((yyvsp[0].sValue),im));
        } else {
          ii->m(ret->second, false);
        }
        free((yyvsp[0].sValue));
      }
    break;

  case 36:
      { if ((yyvsp[-1].vardeclexpr) && (yyvsp[0].expression_v)) (yyvsp[-1].vardeclexpr)->addAnnotations(*(yyvsp[0].expression_v));
        if ((yyvsp[-1].vardeclexpr))
          (yyval.item) = new VarDeclI((yyloc),(yyvsp[-1].vardeclexpr));
        delete (yyvsp[0].expression_v);
      }
    break;

  case 37:
      { if ((yyvsp[-3].vardeclexpr)) (yyvsp[-3].vardeclexpr)->e((yyvsp[0].expression));
        if ((yyvsp[-3].vardeclexpr) && (yyvsp[-2].expression_v)) (yyvsp[-3].vardeclexpr)->addAnnotations(*(yyvsp[-2].expression_v));
        if ((yyvsp[-3].vardeclexpr))
          (yyval.item) = new VarDeclI((yyloc),(yyvsp[-3].vardeclexpr));
        delete (yyvsp[-2].expression_v);
      }
    break;

  case 38:
      {
        TypeInst* ti = new TypeInst((yyloc),Type::parsetint());
        ti->setIsEnum(true);
        VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[0].sValue));
        free((yyvsp[0].sValue));
        (yyval.item) = new VarDeclI((yyloc),vd);
      }
    break;

  case 39:
      {
        TypeInst* ti = new TypeInst((yyloc),Type::parsetint());
        ti->setIsEnum(true);
        SetLit* sl = new SetLit((yyloc), *(yyvsp[-1].expression_v));
        VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[-4].sValue),sl);
        free((yyvsp[-4].sValue));
        delete (yyvsp[-1].expression_v);
        (yyval.item) = new VarDeclI((yyloc),vd);
      }
    break;

  case 40:
      {
        TypeInst* ti = new TypeInst((yyloc),Type::parsetint());
        ti->setIsEnum(true);
        vector<Expression*> args;
        args.push_back(new ArrayLit((yyloc),*(yyvsp[-1].expression_v)));
        Call* sl = new Call((yyloc), constants().ids.anonEnumFromStrings, args);
        VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[-4].sValue),sl);
        free((yyvsp[-4].sValue));
        delete (yyvsp[-1].expression_v);
        (yyval.item) = new VarDeclI((yyloc),vd);
      }
    break;

  case 41:
      {
        TypeInst* ti = new TypeInst((yyloc),Type::parsetint());
        ti->setIsEnum(true);
        vector<Expression*> args;
        args.push_back((yyvsp[-1].expression));
        Call* sl = new Call((yyloc), ASTString((yyvsp[-3].sValue)), args);
        VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[-5].sValue),sl);
        free((yyvsp[-5].sValue));
        free((yyvsp[-3].sValue));
        (yyval.item) = new VarDeclI((yyloc),vd);
      }
    break;

  case 42:
      { (yyval.expression_v) = new std::vector<Expression*>(); }
    break;

  case 43:
      { (yyval.expression_v) = new std::vector<Expression*>();
        (yyval.expression_v)->push_back(new StringLit((yyloc), (yyvsp[0].sValue))); free((yyvsp[0].sValue));
      }
    break;

  case 44:
      { (yyval.expression_v) = (yyvsp[-2].expression_v);
        if ((yyval.expression_v)) (yyval.expression_v)->push_back(new StringLit((yyloc), (yyvsp[0].sValue)));
        free((yyvsp[0].sValue));
      }
    break;

  case 45:
      { (yyval.expression_v) = new std::vector<Expression*>(); }
    break;

  case 46:
      { (yyval.expression_v) = new std::vector<Expression*>();
        (yyval.expression_v)->push_back(new Id((yyloc),(yyvsp[0].sValue),NULL)); free((yyvsp[0].sValue));
      }
    break;

  case 47:
      { (yyval.expression_v) = (yyvsp[-2].expression_v); if ((yyval.expression_v)) (yyval.expression_v)->push_back(new Id((yyloc),(yyvsp[0].sValue),NULL)); free((yyvsp[0].sValue)); }
    break;

  case 48:
      { (yyval.item) = new AssignI((yyloc),(yyvsp[-2].sValue),(yyvsp[0].expression));
        free((yyvsp[-2].sValue));
      }
    break;

  case 49:
      { (yyval.item) = new ConstraintI((yyloc),(yyvsp[0].expression));}
    break;

  case 50:
      { (yyval.item) = new ConstraintI((yyloc),(yyvsp[0].expression));
        if ((yyvsp[0].expression) && (yyvsp[-1].expression))
          (yyval.item)->cast<ConstraintI>()->e()->ann().add(new Call((yylsp[-2]), ASTString("mzn_constraint_name"), {(yyvsp[-1].expression)}));
      }
    break;

  case 51:
      { (yyval.item) = SolveI::sat((yyloc));
        if ((yyval.item) && (yyvsp[-1].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[-1].expression_v));
        delete (yyvsp[-1].expression_v);
      }
    break;

  case 52:
      { (yyval.item) = SolveI::min((yyloc),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-2].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[-2].expression_v));
        delete (yyvsp[-2].expression_v);
      }
    break;

  case 53:
      { (yyval.item) = SolveI::max((yyloc),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-2].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[-2].expression_v));
        delete (yyvsp[-2].expression_v);
      }
    break;

  case 54:
      { (yyval.item) = new OutputI((yyloc),(yyvsp[0].expression));}
    break;

  case 55:
      { if ((yyvsp[-2].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[-3].sValue),new TypeInst((yyloc),
                                   Type::varbool()),*(yyvsp[-2].vardeclexpr_v),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-1].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expression_v));
        free((yyvsp[-3].sValue));
        delete (yyvsp[-2].vardeclexpr_v);
        delete (yyvsp[-1].expression_v);
      }
    break;

  case 56:
      { if ((yyvsp[-2].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[-3].sValue),new TypeInst((yyloc),
                                   Type::parbool()),*(yyvsp[-2].vardeclexpr_v),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-1].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expression_v));
        free((yyvsp[-3].sValue));
        delete (yyvsp[-2].vardeclexpr_v);
        delete (yyvsp[-1].expression_v);
      }
    break;

  case 57:
      { if ((yyvsp[-2].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[-3].sValue),(yyvsp[-5].tiexpr),*(yyvsp[-2].vardeclexpr_v),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-1].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expression_v));
        free((yyvsp[-3].sValue));
        delete (yyvsp[-2].vardeclexpr_v);
        delete (yyvsp[-1].expression_v);
      }
    break;

  case 58:
      { if ((yyvsp[-3].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[-5].sValue),(yyvsp[-7].tiexpr),*(yyvsp[-3].vardeclexpr_v),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-1].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expression_v));
        free((yyvsp[-5].sValue));
        delete (yyvsp[-3].vardeclexpr_v);
        delete (yyvsp[-1].expression_v);
      }
    break;

  case 59:
      {
        TypeInst* ti=new TypeInst((yylsp[-2]),Type::ann());
        if ((yyvsp[0].vardeclexpr_v)==NULL || (yyvsp[0].vardeclexpr_v)->empty()) {
          VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[-1].sValue));
          (yyval.item) = new VarDeclI((yyloc),vd);
        } else {
          (yyval.item) = new FunctionI((yyloc),(yyvsp[-1].sValue),ti,*(yyvsp[0].vardeclexpr_v),NULL);
        }
        free((yyvsp[-1].sValue));
        delete (yyvsp[0].vardeclexpr_v);
      }
    break;

  case 60:
      { TypeInst* ti=new TypeInst((yylsp[-4]),Type::ann());
        if ((yyvsp[-2].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[-3].sValue),ti,*(yyvsp[-2].vardeclexpr_v),(yyvsp[0].expression));
        delete (yyvsp[-2].vardeclexpr_v);
      }
    break;

  case 61:
      { (yyval.expression)=NULL; }
    break;

  case 62:
      { (yyval.expression)=(yyvsp[0].expression); }
    break;

  case 63:
      { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); }
    break;

  case 64:
      { (yyval.vardeclexpr_v)=(yyvsp[-1].vardeclexpr_v); }
    break;

  case 65:
      { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); }
    break;

  case 66:
      { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); }
    break;

  case 67:
      { (yyval.vardeclexpr_v)=(yyvsp[-1].vardeclexpr_v); }
    break;

  case 68:
      { (yyval.vardeclexpr_v)=new vector<VarDecl*>();
        if ((yyvsp[0].vardeclexpr)) (yyvsp[0].vardeclexpr)->toplevel(false);
        if ((yyvsp[0].vardeclexpr)) (yyval.vardeclexpr_v)->push_back((yyvsp[0].vardeclexpr)); }
    break;

  case 69:
      { (yyval.vardeclexpr_v)=(yyvsp[-2].vardeclexpr_v);
        if ((yyvsp[0].vardeclexpr)) (yyvsp[0].vardeclexpr)->toplevel(false);
        if ((yyvsp[-2].vardeclexpr_v) && (yyvsp[0].vardeclexpr)) (yyvsp[-2].vardeclexpr_v)->push_back((yyvsp[0].vardeclexpr)); }
    break;

  case 72:
      { (yyval.vardeclexpr)=(yyvsp[0].vardeclexpr); }
    break;

  case 73:
      { if ((yyvsp[0].tiexpr)) (yyval.vardeclexpr)=new VarDecl((yyloc), (yyvsp[0].tiexpr), ""); }
    break;

  case 74:
      { if ((yyvsp[-2].tiexpr) && (yyvsp[0].sValue)) (yyval.vardeclexpr) = new VarDecl((yyloc), (yyvsp[-2].tiexpr), (yyvsp[0].sValue));
        free((yyvsp[0].sValue));
      }
    break;

  case 75:
      { (yyval.tiexpr_v)=(yyvsp[-1].tiexpr_v); }
    break;

  case 76:
      { (yyval.tiexpr_v)=new vector<TypeInst*>(); (yyval.tiexpr_v)->push_back((yyvsp[0].tiexpr)); }
    break;

  case 77:
      { (yyval.tiexpr_v)=(yyvsp[-2].tiexpr_v); if ((yyvsp[-2].tiexpr_v) && (yyvsp[0].tiexpr)) (yyvsp[-2].tiexpr_v)->push_back((yyvsp[0].tiexpr)); }
    break;

  case 79:
      {
        (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr) && (yyvsp[-3].tiexpr_v)) (yyval.tiexpr)->setRanges(*(yyvsp[-3].tiexpr_v));
        delete (yyvsp[-3].tiexpr_v);
      }
    break;

  case 80:
      {
        (yyval.tiexpr) = (yyvsp[0].tiexpr);
        std::vector<TypeInst*> ti(1);
        ti[0] = new TypeInst((yyloc),Type::parint());
        if ((yyval.tiexpr)) (yyval.tiexpr)->setRanges(ti);
      }
    break;

  case 81:
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
      }
    break;

  case 82:
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      }
    break;

  case 83:
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr) && (yyvsp[-1].bValue)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      }
    break;

  case 84:
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ti(Type::TI_VAR);
          if ((yyvsp[-1].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      }
    break;

  case 85:
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.st(Type::ST_SET);
          if ((yyvsp[-3].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      }
    break;

  case 86:
      { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.st(Type::ST_SET);
          if ((yyvsp[-3].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      }
    break;

  case 87:
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

  case 88:
      { (yyval.bValue) = false; }
    break;

  case 89:
      { (yyval.bValue) = true; }
    break;

  case 90:
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::parint()); }
    break;

  case 91:
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::parbool()); }
    break;

  case 92:
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::parfloat()); }
    break;

  case 93:
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::parstring()); }
    break;

  case 94:
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::ann()); }
    break;

  case 95:
        { if ((yyvsp[0].expression)) (yyval.tiexpr) = new TypeInst((yyloc),Type(),(yyvsp[0].expression)); }
    break;

  case 96:
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::top(),
                         new TIId((yyloc), (yyvsp[0].sValue)));
        free((yyvsp[0].sValue));
      }
    break;

  case 97:
      { (yyval.tiexpr) = new TypeInst((yyloc),Type::parint(),
          new TIId((yyloc), (yyvsp[0].sValue)));
          free((yyvsp[0].sValue));
      }
    break;

  case 99:
      { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; (yyval.expression_v)->push_back((yyvsp[0].expression)); }
    break;

  case 100:
      { (yyval.expression_v)=(yyvsp[-2].expression_v); if ((yyval.expression_v) && (yyvsp[0].expression)) (yyval.expression_v)->push_back((yyvsp[0].expression)); }
    break;

  case 101:
      { (yyval.expression) = (yyvsp[0].expression); }
    break;

  case 102:
      { (yyval.expression)=new SetLit((yyloc), IntSetVal::a(-IntVal::infinity(),IntVal::infinity())); }
    break;

  case 103:
      { if ((yyvsp[0].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[0].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a(-IntVal::infinity(),(yyvsp[0].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), IntLit::a(-IntVal::infinity()), BOT_DOTDOT, (yyvsp[0].expression));
        }
      }
    break;

  case 104:
      { if ((yyvsp[-1].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[-1].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[-1].expression)->cast<IntLit>()->v(),IntVal::infinity()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-1].expression), BOT_DOTDOT, IntLit::a(IntVal::infinity()));
        }
      }
    break;

  case 106:
      { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; (yyval.expression_v)->push_back((yyvsp[0].expression)); }
    break;

  case 107:
      { (yyval.expression_v)=(yyvsp[-2].expression_v); if ((yyval.expression_v) && (yyvsp[0].expression)) (yyval.expression_v)->push_back((yyvsp[0].expression)); }
    break;

  case 109:
      { if ((yyvsp[-2].expression) && (yyvsp[0].expression)) (yyvsp[-2].expression)->addAnnotation((yyvsp[0].expression)); (yyval.expression)=(yyvsp[-2].expression); }
    break;

  case 110:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_UNION, (yyvsp[0].expression)); }
    break;

  case 111:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIFF, (yyvsp[0].expression)); }
    break;

  case 112:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SYMDIFF, (yyvsp[0].expression)); }
    break;

  case 113:
      { if ((yyvsp[-2].expression)==NULL || (yyvsp[0].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[-2].expression)->isa<IntLit>() && (yyvsp[0].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[-2].expression)->cast<IntLit>()->v(),(yyvsp[0].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DOTDOT, (yyvsp[0].expression));
        }
      }
    break;

  case 114:
      { if ((yyvsp[-3].expression)==NULL || (yyvsp[-1].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[-3].expression)->isa<IntLit>() && (yyvsp[-1].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[-3].expression)->cast<IntLit>()->v(),(yyvsp[-1].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-3].expression), BOT_DOTDOT, (yyvsp[-1].expression));
        }
      }
    break;

  case 115:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_INTERSECT, (yyvsp[0].expression)); }
    break;

  case 116:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_PLUSPLUS, (yyvsp[0].expression)); }
    break;

  case 117:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_PLUS, (yyvsp[0].expression)); }
    break;

  case 118:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MINUS, (yyvsp[0].expression)); }
    break;

  case 119:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MULT, (yyvsp[0].expression)); }
    break;

  case 120:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIV, (yyvsp[0].expression)); }
    break;

  case 121:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IDIV, (yyvsp[0].expression)); }
    break;

  case 122:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MOD, (yyvsp[0].expression)); }
    break;

  case 123:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_POW, (yyvsp[0].expression)); }
    break;

  case 124:
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~+"), args);
      }
    break;

  case 125:
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~-"), args);
      }
    break;

  case 126:
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~*"), args);
      }
    break;

  case 127:
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~="), args);
      }
    break;

  case 128:
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), (yyvsp[-1].sValue), args);
        free((yyvsp[-1].sValue));
      }
    break;

  case 129:
      { (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[0].expression)); }
    break;

  case 130:
      { if ((yyvsp[0].expression) && (yyvsp[0].expression)->isa<IntLit>()) {
          (yyval.expression) = IntLit::a(-(yyvsp[0].expression)->cast<IntLit>()->v());
        } else if ((yyvsp[0].expression) && (yyvsp[0].expression)->isa<FloatLit>()) {
          (yyval.expression) = FloatLit::a(-(yyvsp[0].expression)->cast<FloatLit>()->v());
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_MINUS, (yyvsp[0].expression));
        }
      }
    break;

  case 132:
      { if ((yyvsp[-2].expression) && (yyvsp[0].expression)) (yyvsp[-2].expression)->addAnnotation((yyvsp[0].expression)); (yyval.expression)=(yyvsp[-2].expression); }
    break;

  case 133:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_EQUIV, (yyvsp[0].expression)); }
    break;

  case 134:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IMPL, (yyvsp[0].expression)); }
    break;

  case 135:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_RIMPL, (yyvsp[0].expression)); }
    break;

  case 136:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_OR, (yyvsp[0].expression)); }
    break;

  case 137:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_XOR, (yyvsp[0].expression)); }
    break;

  case 138:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_AND, (yyvsp[0].expression)); }
    break;

  case 139:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_LE, (yyvsp[0].expression)); }
    break;

  case 140:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_GR, (yyvsp[0].expression)); }
    break;

  case 141:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_LQ, (yyvsp[0].expression)); }
    break;

  case 142:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_GQ, (yyvsp[0].expression)); }
    break;

  case 143:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_EQ, (yyvsp[0].expression)); }
    break;

  case 144:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_NQ, (yyvsp[0].expression)); }
    break;

  case 145:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IN, (yyvsp[0].expression)); }
    break;

  case 146:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SUBSET, (yyvsp[0].expression)); }
    break;

  case 147:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SUPERSET, (yyvsp[0].expression)); }
    break;

  case 148:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_UNION, (yyvsp[0].expression)); }
    break;

  case 149:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIFF, (yyvsp[0].expression)); }
    break;

  case 150:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SYMDIFF, (yyvsp[0].expression)); }
    break;

  case 151:
      { if ((yyvsp[-2].expression)==NULL || (yyvsp[0].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[-2].expression)->isa<IntLit>() && (yyvsp[0].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[-2].expression)->cast<IntLit>()->v(),(yyvsp[0].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DOTDOT, (yyvsp[0].expression));
        }
      }
    break;

  case 152:
      { if ((yyvsp[-3].expression)==NULL || (yyvsp[-1].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[-3].expression)->isa<IntLit>() && (yyvsp[-1].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[-3].expression)->cast<IntLit>()->v(),(yyvsp[-1].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-3].expression), BOT_DOTDOT, (yyvsp[-1].expression));
        }
      }
    break;

  case 153:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_INTERSECT, (yyvsp[0].expression)); }
    break;

  case 154:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_PLUSPLUS, (yyvsp[0].expression)); }
    break;

  case 155:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_PLUS, (yyvsp[0].expression)); }
    break;

  case 156:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MINUS, (yyvsp[0].expression)); }
    break;

  case 157:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MULT, (yyvsp[0].expression)); }
    break;

  case 158:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIV, (yyvsp[0].expression)); }
    break;

  case 159:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IDIV, (yyvsp[0].expression)); }
    break;

  case 160:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MOD, (yyvsp[0].expression)); }
    break;

  case 161:
      { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_POW, (yyvsp[0].expression)); }
    break;

  case 162:
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~+"), args);
      }
    break;

  case 163:
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~-"), args);
      }
    break;

  case 164:
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~*"), args);
      }
    break;

  case 165:
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~="), args);
      }
    break;

  case 166:
      { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), (yyvsp[-1].sValue), args);
        free((yyvsp[-1].sValue));
      }
    break;

  case 167:
      { (yyval.expression)=new UnOp((yyloc), UOT_NOT, (yyvsp[0].expression)); }
    break;

  case 168:
      { if (((yyvsp[0].expression) && (yyvsp[0].expression)->isa<IntLit>()) || ((yyvsp[0].expression) && (yyvsp[0].expression)->isa<FloatLit>())) {
          (yyval.expression) = (yyvsp[0].expression);
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[0].expression));
        }
      }
    break;

  case 169:
      { if ((yyvsp[0].expression) && (yyvsp[0].expression)->isa<IntLit>()) {
          (yyval.expression) = IntLit::a(-(yyvsp[0].expression)->cast<IntLit>()->v());
        } else if ((yyvsp[0].expression) && (yyvsp[0].expression)->isa<FloatLit>()) {
          (yyval.expression) = FloatLit::a(-(yyvsp[0].expression)->cast<FloatLit>()->v());
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_MINUS, (yyvsp[0].expression));
        }
      }
    break;

  case 170:
      { (yyval.expression)=(yyvsp[0].expression); }
    break;

  case 171:
      { (yyval.expression)=(yyvsp[0].expression); }
    break;

  case 172:
      { (yyval.expression)=(yyvsp[-1].expression); }
    break;

  case 173:
      { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[0].expression_vv)); delete (yyvsp[0].expression_vv); }
    break;

  case 174:
      { (yyval.expression)=new Id((yyloc), (yyvsp[0].sValue), NULL); free((yyvsp[0].sValue)); }
    break;

  case 175:
      { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), new Id((yylsp[-1]),(yyvsp[-1].sValue),NULL), *(yyvsp[0].expression_vv));
        free((yyvsp[-1].sValue)); delete (yyvsp[0].expression_vv); }
    break;

  case 176:
      { (yyval.expression)=new AnonVar((yyloc)); }
    break;

  case 177:
      { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), new AnonVar((yyloc)), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }
    break;

  case 178:
      { (yyval.expression)=constants().boollit(((yyvsp[0].iValue)!=0)); }
    break;

  case 179:
      { (yyval.expression)=IntLit::a((yyvsp[0].iValue)); }
    break;

  case 180:
      { (yyval.expression)=IntLit::a(IntVal::infinity()); }
    break;

  case 181:
      { (yyval.expression)=FloatLit::a((yyvsp[0].dValue)); }
    break;

  case 182:
      { (yyval.expression)=constants().absent; }
    break;

  case 184:
      { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }
    break;

  case 186:
      { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }
    break;

  case 188:
      { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }
    break;

  case 190:
      { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }
    break;

  case 192:
      { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }
    break;

  case 194:
      { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }
    break;

  case 197:
      { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }
    break;

  case 198:
      { (yyval.expression)=new StringLit((yyloc), (yyvsp[0].sValue)); free((yyvsp[0].sValue)); }
    break;

  case 199:
      { (yyval.expression)=new BinOp((yyloc), new StringLit((yyloc), (yyvsp[-1].sValue)), BOT_PLUSPLUS, (yyvsp[0].expression));
        free((yyvsp[-1].sValue));
      }
    break;

  case 200:
      { if ((yyvsp[-1].expression_v)) (yyval.expression)=new BinOp((yyloc), new Call((yyloc), ASTString("format"), *(yyvsp[-1].expression_v)), BOT_PLUSPLUS, new StringLit((yyloc),(yyvsp[0].sValue)));
        free((yyvsp[0].sValue));
        delete (yyvsp[-1].expression_v);
      }
    break;

  case 201:
      { if ((yyvsp[-2].expression_v)) (yyval.expression)=new BinOp((yyloc), new Call((yyloc), ASTString("format"), *(yyvsp[-2].expression_v)), BOT_PLUSPLUS,
                             new BinOp((yyloc), new StringLit((yyloc),(yyvsp[-1].sValue)), BOT_PLUSPLUS, (yyvsp[0].expression)));
        free((yyvsp[-1].sValue));
        delete (yyvsp[-2].expression_v);
      }
    break;

  case 202:
      { (yyval.expression_vv)=new std::vector<std::vector<Expression*> >();
        if ((yyvsp[-1].expression_v)) {
          (yyval.expression_vv)->push_back(*(yyvsp[-1].expression_v));
          delete (yyvsp[-1].expression_v);
        }
      }
    break;

  case 203:
      { (yyval.expression_vv)=(yyvsp[-3].expression_vv);
        if ((yyval.expression_vv) && (yyvsp[-1].expression_v)) {
          (yyval.expression_vv)->push_back(*(yyvsp[-1].expression_v));
          delete (yyvsp[-1].expression_v);
        }
      }
    break;

  case 204:
      { (yyval.expression) = new SetLit((yyloc), std::vector<Expression*>()); }
    break;

  case 205:
      { if ((yyvsp[-1].expression_v)) (yyval.expression) = new SetLit((yyloc), *(yyvsp[-1].expression_v));
        delete (yyvsp[-1].expression_v); }
    break;

  case 206:
      { if ((yyvsp[-1].generators)) (yyval.expression) = new Comprehension((yyloc), (yyvsp[-3].expression), *(yyvsp[-1].generators), true);
        delete (yyvsp[-1].generators);
      }
    break;

  case 207:
      { if ((yyvsp[0].generator_v)) (yyval.generators)=new Generators; (yyval.generators)->_g = *(yyvsp[0].generator_v); delete (yyvsp[0].generator_v); }
    break;

  case 209:
      { (yyval.generator_v)=new std::vector<Generator>; if ((yyvsp[0].generator)) (yyval.generator_v)->push_back(*(yyvsp[0].generator)); delete (yyvsp[0].generator); }
    break;

  case 210:
      { (yyval.generator_v)=new std::vector<Generator>; if ((yyvsp[0].generator)) (yyval.generator_v)->push_back(*(yyvsp[0].generator)); delete (yyvsp[0].generator); }
    break;

  case 211:
      { (yyval.generator_v)=new std::vector<Generator>;
        if ((yyvsp[-2].generator)) (yyval.generator_v)->push_back(*(yyvsp[-2].generator));
        if ((yyvsp[-2].generator) && (yyvsp[0].expression)) (yyval.generator_v)->push_back(Generator((yyval.generator_v)->size(),(yyvsp[0].expression)));
        delete (yyvsp[-2].generator);
      }
    break;

  case 212:
      { (yyval.generator_v)=(yyvsp[-2].generator_v); if ((yyval.generator_v) && (yyvsp[0].generator)) (yyval.generator_v)->push_back(*(yyvsp[0].generator)); delete (yyvsp[0].generator); }
    break;

  case 213:
      { (yyval.generator_v)=(yyvsp[-2].generator_v); if ((yyval.generator_v) && (yyvsp[0].generator)) (yyval.generator_v)->push_back(*(yyvsp[0].generator)); delete (yyvsp[0].generator); }
    break;

  case 214:
      { (yyval.generator_v)=(yyvsp[-4].generator_v);
        if ((yyval.generator_v) && (yyvsp[-2].generator)) (yyval.generator_v)->push_back(*(yyvsp[-2].generator));
        if ((yyval.generator_v) && (yyvsp[-2].generator) && (yyvsp[0].expression)) (yyval.generator_v)->push_back(Generator((yyval.generator_v)->size(),(yyvsp[0].expression)));
        delete (yyvsp[-2].generator);
      }
    break;

  case 215:
      { if ((yyvsp[-2].string_v) && (yyvsp[0].expression)) (yyval.generator)=new Generator(*(yyvsp[-2].string_v),(yyvsp[0].expression),NULL); else (yyval.generator)=NULL; delete (yyvsp[-2].string_v); }
    break;

  case 216:
      { if ((yyvsp[-4].string_v) && (yyvsp[-2].expression)) (yyval.generator)=new Generator(*(yyvsp[-4].string_v),(yyvsp[-2].expression),(yyvsp[0].expression)); else (yyval.generator)=NULL; delete (yyvsp[-4].string_v); }
    break;

  case 217:
      { if ((yyvsp[0].expression)) (yyval.generator)=new Generator({(yyvsp[-2].sValue)},NULL,(yyvsp[0].expression)); else (yyval.generator)=NULL; free((yyvsp[-2].sValue)); }
    break;

  case 219:
      { (yyval.string_v)=new std::vector<std::string>; (yyval.string_v)->push_back((yyvsp[0].sValue)); free((yyvsp[0].sValue)); }
    break;

  case 220:
      { (yyval.string_v)=(yyvsp[-2].string_v); if ((yyval.string_v) && (yyvsp[0].sValue)) (yyval.string_v)->push_back((yyvsp[0].sValue)); free((yyvsp[0].sValue)); }
    break;

  case 221:
      { (yyval.expression)=new ArrayLit((yyloc), std::vector<MiniZinc::Expression*>()); }
    break;

  case 222:
      { if ((yyvsp[-1].expression_v)) (yyval.expression)=new ArrayLit((yyloc), *(yyvsp[-1].expression_v)); delete (yyvsp[-1].expression_v); }
    break;

  case 223:
      { (yyval.expression)=new ArrayLit((yyloc), std::vector<std::vector<Expression*> >()); }
    break;

  case 224:
      { if ((yyvsp[-1].expression_vv)) {
          (yyval.expression)=new ArrayLit((yyloc), *(yyvsp[-1].expression_vv));
          for (unsigned int i=1; i<(yyvsp[-1].expression_vv)->size(); i++)
            if ((*(yyvsp[-1].expression_vv))[i].size() != (*(yyvsp[-1].expression_vv))[i-1].size())
              yyerror(&(yylsp[-1]), parm, "syntax error, all sub-arrays of 2d array literal must have the same length");
          delete (yyvsp[-1].expression_vv);
        } else {
          (yyval.expression) = NULL;
        }
      }
    break;

  case 225:
      { if ((yyvsp[-2].expression_vv)) {
          (yyval.expression)=new ArrayLit((yyloc), *(yyvsp[-2].expression_vv));
          for (unsigned int i=1; i<(yyvsp[-2].expression_vv)->size(); i++)
            if ((*(yyvsp[-2].expression_vv))[i].size() != (*(yyvsp[-2].expression_vv))[i-1].size())
              yyerror(&(yylsp[-2]), parm, "syntax error, all sub-arrays of 2d array literal must have the same length");
          delete (yyvsp[-2].expression_vv);
        } else {
          (yyval.expression) = NULL;
        }
      }
    break;

  case 226:
    {
      if ((yyvsp[-1].expression_vvv)) {
        std::vector<std::pair<int,int> > dims(3);
        dims[0] = std::pair<int,int>(1,static_cast<int>((yyvsp[-1].expression_vvv)->size()));
        if ((yyvsp[-1].expression_vvv)->size()==0) {
          dims[1] = std::pair<int,int>(1,0);
          dims[2] = std::pair<int,int>(1,0);
        } else {
          dims[1] = std::pair<int,int>(1,static_cast<int>((*(yyvsp[-1].expression_vvv))[0].size()));
          if ((*(yyvsp[-1].expression_vvv))[0].size()==0) {
            dims[2] = std::pair<int,int>(1,0);
          } else {
            dims[2] = std::pair<int,int>(1,static_cast<int>((*(yyvsp[-1].expression_vvv))[0][0].size()));
          }
        }
        std::vector<Expression*> a;
        for (int i=0; i<dims[0].second; i++) {
          if ((*(yyvsp[-1].expression_vvv))[i].size() != dims[1].second) {
            yyerror(&(yylsp[-1]), parm, "syntax error, all sub-arrays of 3d array literal must have the same length");
          } else {
            for (int j=0; j<dims[1].second; j++) {
              if ((*(yyvsp[-1].expression_vvv))[i][j].size() != dims[2].second) {
                yyerror(&(yylsp[-1]), parm, "syntax error, all sub-arrays of 3d array literal must have the same length");
              } else {
                for (int k=0; k<dims[2].second; k++) {
                  a.push_back((*(yyvsp[-1].expression_vvv))[i][j][k]);
                }
              }
            }
          }
        }
        (yyval.expression) = new ArrayLit((yyloc),a,dims);
        delete (yyvsp[-1].expression_vvv);
      } else {
        (yyval.expression) = NULL;
      }
    }
    break;

  case 227:
      { (yyval.expression_vvv)=new std::vector<std::vector<std::vector<MiniZinc::Expression*> > >;
      }
    break;

  case 228:
      { (yyval.expression_vvv)=new std::vector<std::vector<std::vector<MiniZinc::Expression*> > >;
        if ((yyvsp[-1].expression_vv)) (yyval.expression_vvv)->push_back(*(yyvsp[-1].expression_vv));
        delete (yyvsp[-1].expression_vv);
      }
    break;

  case 229:
      { (yyval.expression_vvv)=(yyvsp[-4].expression_vvv);
        if ((yyval.expression_vvv) && (yyvsp[-1].expression_vv)) (yyval.expression_vvv)->push_back(*(yyvsp[-1].expression_vv));
        delete (yyvsp[-1].expression_vv);
      }
    break;

  case 230:
      { (yyval.expression_vv)=new std::vector<std::vector<MiniZinc::Expression*> >;
        if ((yyvsp[0].expression_v)) (yyval.expression_vv)->push_back(*(yyvsp[0].expression_v));
        delete (yyvsp[0].expression_v);
      }
    break;

  case 231:
      { (yyval.expression_vv)=(yyvsp[-2].expression_vv); if ((yyval.expression_vv) && (yyvsp[0].expression_v)) (yyval.expression_vv)->push_back(*(yyvsp[0].expression_v)); delete (yyvsp[0].expression_v); }
    break;

  case 232:
      { if ((yyvsp[-1].generators)) (yyval.expression)=new Comprehension((yyloc), (yyvsp[-3].expression), *(yyvsp[-1].generators), false);
        delete (yyvsp[-1].generators);
      }
    break;

  case 233:
      {
        std::vector<Expression*> iexps;
        iexps.push_back((yyvsp[-3].expression));
        iexps.push_back((yyvsp[-1].expression));
        (yyval.expression)=new ITE((yyloc), iexps, NULL);
      }
    break;

  case 234:
      {
        std::vector<Expression*> iexps;
        iexps.push_back((yyvsp[-6].expression));
        iexps.push_back((yyvsp[-4].expression));
        if ((yyvsp[-3].expression_v)) {
          for (unsigned int i=0; i<(yyvsp[-3].expression_v)->size(); i+=2) {
            iexps.push_back((*(yyvsp[-3].expression_v))[i]);
            iexps.push_back((*(yyvsp[-3].expression_v))[i+1]);
          }
        }
        (yyval.expression)=new ITE((yyloc), iexps,(yyvsp[-1].expression));
        delete (yyvsp[-3].expression_v);
      }
    break;

  case 235:
      { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; }
    break;

  case 236:
      { (yyval.expression_v)=(yyvsp[-4].expression_v); if ((yyval.expression_v) && (yyvsp[-2].expression) && (yyvsp[0].expression)) { (yyval.expression_v)->push_back((yyvsp[-2].expression)); (yyval.expression_v)->push_back((yyvsp[0].expression)); } }
    break;

  case 237:
      { (yyval.iValue)=BOT_EQUIV; }
    break;

  case 238:
      { (yyval.iValue)=BOT_IMPL; }
    break;

  case 239:
      { (yyval.iValue)=BOT_RIMPL; }
    break;

  case 240:
      { (yyval.iValue)=BOT_OR; }
    break;

  case 241:
      { (yyval.iValue)=BOT_XOR; }
    break;

  case 242:
      { (yyval.iValue)=BOT_AND; }
    break;

  case 243:
      { (yyval.iValue)=BOT_LE; }
    break;

  case 244:
      { (yyval.iValue)=BOT_GR; }
    break;

  case 245:
      { (yyval.iValue)=BOT_LQ; }
    break;

  case 246:
      { (yyval.iValue)=BOT_GQ; }
    break;

  case 247:
      { (yyval.iValue)=BOT_EQ; }
    break;

  case 248:
      { (yyval.iValue)=BOT_NQ; }
    break;

  case 249:
      { (yyval.iValue)=BOT_IN; }
    break;

  case 250:
      { (yyval.iValue)=BOT_SUBSET; }
    break;

  case 251:
      { (yyval.iValue)=BOT_SUPERSET; }
    break;

  case 252:
      { (yyval.iValue)=BOT_UNION; }
    break;

  case 253:
      { (yyval.iValue)=BOT_DIFF; }
    break;

  case 254:
      { (yyval.iValue)=BOT_SYMDIFF; }
    break;

  case 255:
      { (yyval.iValue)=BOT_PLUS; }
    break;

  case 256:
      { (yyval.iValue)=BOT_MINUS; }
    break;

  case 257:
      { (yyval.iValue)=BOT_MULT; }
    break;

  case 258:
      { (yyval.iValue)=BOT_POW; }
    break;

  case 259:
      { (yyval.iValue)=BOT_DIV; }
    break;

  case 260:
      { (yyval.iValue)=BOT_IDIV; }
    break;

  case 261:
      { (yyval.iValue)=BOT_MOD; }
    break;

  case 262:
      { (yyval.iValue)=BOT_INTERSECT; }
    break;

  case 263:
      { (yyval.iValue)=BOT_PLUSPLUS; }
    break;

  case 264:
      { (yyval.iValue)=-1; }
    break;

  case 265:
      { if ((yyvsp[-5].iValue)==-1) {
          (yyval.expression)=NULL;
          yyerror(&(yylsp[-3]), parm, "syntax error, unary operator with two arguments");
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-3].expression),static_cast<BinOpType>((yyvsp[-5].iValue)),(yyvsp[-1].expression));
        }
      }
    break;

  case 266:
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
          (yyval.expression)=NULL;
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

  case 267:
      { (yyval.expression)=new Call((yyloc), (yyvsp[-2].sValue), std::vector<Expression*>()); free((yyvsp[-2].sValue)); }
    break;

  case 269:
      { 
        if ((yyvsp[-1].expression_p)!=NULL) {
          bool hadWhere = false;
          std::vector<Expression*> args;
          for (unsigned int i=0; i<(yyvsp[-1].expression_p)->size(); i++) {
            if ((*(yyvsp[-1].expression_p))[i].second) {
              yyerror(&(yylsp[-1]), parm, "syntax error, 'where' expression outside generator call");
              hadWhere = true;
              (yyval.expression)=NULL;
            }
            args.push_back((*(yyvsp[-1].expression_p))[i].first);
          }
          if (!hadWhere) {
            (yyval.expression)=new Call((yyloc), (yyvsp[-3].sValue), args);
          }
        }
        free((yyvsp[-3].sValue));
        delete (yyvsp[-1].expression_p);
      }
    break;

  case 270:
      { 
        vector<Generator> gens;
        vector<Id*> ids;
        if ((yyvsp[-4].expression_p)) {
          for (unsigned int i=0; i<(yyvsp[-4].expression_p)->size(); i++) {
            if (Id* id = Expression::dyn_cast<Id>((*(yyvsp[-4].expression_p))[i].first)) {
              if ((*(yyvsp[-4].expression_p))[i].second) {
                ParserLocation loc = (*(yyvsp[-4].expression_p))[i].second->loc().parserLocation();
                yyerror(&loc, parm, "illegal where expression in generator call");
              }
              ids.push_back(id);
            } else {
              if (BinOp* boe = Expression::dyn_cast<BinOp>((*(yyvsp[-4].expression_p))[i].first)) {
                if (boe->lhs() && boe->rhs()) {
                  Id* id = Expression::dyn_cast<Id>(boe->lhs());
                  if (id && boe->op() == BOT_IN) {
                    ids.push_back(id);
                    gens.push_back(Generator(ids,boe->rhs(),(*(yyvsp[-4].expression_p))[i].second));
                    ids = vector<Id*>();
                  } else if (id && boe->op() == BOT_EQ && ids.empty()) {
                    ids.push_back(id);
                    gens.push_back(Generator(ids,NULL,boe->rhs()));
                    if ((*(yyvsp[-4].expression_p))[i].second) {
                      gens.push_back(Generator(gens.size(),(*(yyvsp[-4].expression_p))[i].second));
                    }
                    ids = vector<Id*>();
                  } else {
                    ParserLocation loc = (*(yyvsp[-4].expression_p))[i].first->loc().parserLocation();
                    yyerror(&loc, parm, "illegal expression in generator call");
                  }
                }
              } else {
                ParserLocation loc = (*(yyvsp[-4].expression_p))[i].first->loc().parserLocation();
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
          (yyval.expression)=NULL;
        } else {
          Generators g; g._g = gens;
          Comprehension* ac = new Comprehension((yyloc), (yyvsp[-1].expression),g,false);
          vector<Expression*> args; args.push_back(ac);
          (yyval.expression)=new Call((yyloc), (yyvsp[-6].sValue), args);
        }
        free((yyvsp[-6].sValue));
        delete (yyvsp[-4].expression_p);
      }
    break;

  case 272:
      { (yyval.expression_p)=new vector<pair<Expression*,Expression*> >;
        if ((yyvsp[0].expression)) {
          (yyval.expression_p)->push_back(pair<Expression*,Expression*>((yyvsp[0].expression),NULL));
        }
      }
    break;

  case 273:
      { (yyval.expression_p)=new vector<pair<Expression*,Expression*> >;
        if ((yyvsp[-2].expression) && (yyvsp[0].expression)) {
          (yyval.expression_p)->push_back(pair<Expression*,Expression*>((yyvsp[-2].expression),(yyvsp[0].expression)));
        }
      }
    break;

  case 274:
      { (yyval.expression_p)=(yyvsp[-2].expression_p); if ((yyval.expression_p) && (yyvsp[0].expression)) (yyval.expression_p)->push_back(pair<Expression*,Expression*>((yyvsp[0].expression),NULL)); }
    break;

  case 275:
      { (yyval.expression_p)=(yyvsp[-4].expression_p); if ((yyval.expression_p) && (yyvsp[-2].expression) && (yyvsp[0].expression)) (yyval.expression_p)->push_back(pair<Expression*,Expression*>((yyvsp[-2].expression),(yyvsp[0].expression))); }
    break;

  case 276:
      { if ((yyvsp[-3].expression_v) && (yyvsp[0].expression)) {
          (yyval.expression)=new Let((yyloc), *(yyvsp[-3].expression_v), (yyvsp[0].expression)); delete (yyvsp[-3].expression_v);
        } else {
          (yyval.expression)=NULL;
        }
      }
    break;

  case 277:
      { if ((yyvsp[-4].expression_v) && (yyvsp[0].expression)) {
          (yyval.expression)=new Let((yyloc), *(yyvsp[-4].expression_v), (yyvsp[0].expression)); delete (yyvsp[-4].expression_v);
        } else {
          (yyval.expression)=NULL;
        }
      }
    break;

  case 278:
      { (yyval.expression_v)=new vector<Expression*>; (yyval.expression_v)->push_back((yyvsp[0].vardeclexpr)); }
    break;

  case 279:
      { (yyval.expression_v)=new vector<Expression*>;
        if ((yyvsp[0].item)) {
          ConstraintI* ce = (yyvsp[0].item)->cast<ConstraintI>();
          (yyval.expression_v)->push_back(ce->e());
          ce->e(NULL);
        }
      }
    break;

  case 280:
      { (yyval.expression_v)=(yyvsp[-2].expression_v); if ((yyval.expression_v) && (yyvsp[0].vardeclexpr)) (yyval.expression_v)->push_back((yyvsp[0].vardeclexpr)); }
    break;

  case 281:
      { (yyval.expression_v)=(yyvsp[-2].expression_v);
        if ((yyval.expression_v) && (yyvsp[0].item)) {
          ConstraintI* ce = (yyvsp[0].item)->cast<ConstraintI>();
          (yyval.expression_v)->push_back(ce->e());
          ce->e(NULL);
        }
      }
    break;

  case 284:
      { (yyval.vardeclexpr) = (yyvsp[-1].vardeclexpr);
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->toplevel(false);
        if ((yyval.vardeclexpr) && (yyvsp[0].expression_v)) (yyval.vardeclexpr)->addAnnotations(*(yyvsp[0].expression_v));
        delete (yyvsp[0].expression_v);
      }
    break;

  case 285:
      { if ((yyvsp[-3].vardeclexpr)) (yyvsp[-3].vardeclexpr)->e((yyvsp[0].expression));
        (yyval.vardeclexpr) = (yyvsp[-3].vardeclexpr);
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->loc((yyloc));
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->toplevel(false);
        if ((yyval.vardeclexpr) && (yyvsp[-2].expression_v)) (yyval.vardeclexpr)->addAnnotations(*(yyvsp[-2].expression_v));
        delete (yyvsp[-2].expression_v);
      }
    break;

  case 286:
      { (yyval.expression_v)=NULL; }
    break;

  case 288:
      { (yyval.expression) = (yyvsp[0].expression); }
    break;

  case 289:
      { (yyval.expression) = new Call((yylsp[0]), ASTString("mzn_expression_name"), {(yyvsp[0].expression)}); }
    break;

  case 290:
      { (yyval.expression_v)=new std::vector<Expression*>(1);
        (*(yyval.expression_v))[0] = (yyvsp[0].expression);
      }
    break;

  case 291:
      { (yyval.expression_v)=(yyvsp[-2].expression_v); if ((yyval.expression_v)) (yyval.expression_v)->push_back((yyvsp[0].expression)); }
    break;

  case 292:
      { (yyval.sValue)=(yyvsp[0].sValue); }
    break;

  case 293:
      { (yyval.sValue)=strdup("'<->'"); }
    break;

  case 294:
      { (yyval.sValue)=strdup("'->'"); }
    break;

  case 295:
      { (yyval.sValue)=strdup("'<-'"); }
    break;

  case 296:
      { (yyval.sValue)=strdup("'\\/'"); }
    break;

  case 297:
      { (yyval.sValue)=strdup("'xor'"); }
    break;

  case 298:
      { (yyval.sValue)=strdup("'/\\'"); }
    break;

  case 299:
      { (yyval.sValue)=strdup("'<'"); }
    break;

  case 300:
      { (yyval.sValue)=strdup("'>'"); }
    break;

  case 301:
      { (yyval.sValue)=strdup("'<='"); }
    break;

  case 302:
      { (yyval.sValue)=strdup("'>='"); }
    break;

  case 303:
      { (yyval.sValue)=strdup("'='"); }
    break;

  case 304:
      { (yyval.sValue)=strdup("'!='"); }
    break;

  case 305:
      { (yyval.sValue)=strdup("'in'"); }
    break;

  case 306:
      { (yyval.sValue)=strdup("'subset'"); }
    break;

  case 307:
      { (yyval.sValue)=strdup("'superset'"); }
    break;

  case 308:
      { (yyval.sValue)=strdup("'union'"); }
    break;

  case 309:
      { (yyval.sValue)=strdup("'diff'"); }
    break;

  case 310:
      { (yyval.sValue)=strdup("'symdiff'"); }
    break;

  case 311:
      { (yyval.sValue)=strdup("'..'"); }
    break;

  case 312:
      { (yyval.sValue)=strdup("'+'"); }
    break;

  case 313:
      { (yyval.sValue)=strdup("'-'"); }
    break;

  case 314:
      { (yyval.sValue)=strdup("'*'"); }
    break;

  case 315:
      { (yyval.sValue)=strdup("'^'"); }
    break;

  case 316:
      { (yyval.sValue)=strdup("'/'"); }
    break;

  case 317:
      { (yyval.sValue)=strdup("'div'"); }
    break;

  case 318:
      { (yyval.sValue)=strdup("'mod'"); }
    break;

  case 319:
      { (yyval.sValue)=strdup("'intersect'"); }
    break;

  case 320:
      { (yyval.sValue)=strdup("'not'"); }
    break;

  case 321:
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
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
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

