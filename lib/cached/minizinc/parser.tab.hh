/* A Bison parser, made by GNU Bison 3.7.4.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_MZN_YY_USERS_TACK_PROGRAMMING_MINIZINC_LIBMZN_BUILD_XCODE_INCLUDE_MINIZINC_PARSER_TAB_HH_INCLUDED
# define YY_MZN_YY_USERS_TACK_PROGRAMMING_MINIZINC_LIBMZN_BUILD_XCODE_INCLUDE_MINIZINC_PARSER_TAB_HH_INCLUDED
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
    MZN_POW_MINUS1 = 353,          /* "^-1"  */
    MZN_NOT = 354,                 /* "not"  */
    MZN_PLUSPLUS = 355,            /* "++"  */
    MZN_COLONCOLON = 356,          /* "::"  */
    PREC_ANNO = 357,               /* PREC_ANNO  */
    MZN_EQUIV_QUOTED = 358,        /* "'<->'"  */
    MZN_IMPL_QUOTED = 359,         /* "'->'"  */
    MZN_RIMPL_QUOTED = 360,        /* "'<-'"  */
    MZN_OR_QUOTED = 361,           /* "'\\/'"  */
    MZN_XOR_QUOTED = 362,          /* "'xor'"  */
    MZN_AND_QUOTED = 363,          /* "'/\\'"  */
    MZN_LE_QUOTED = 364,           /* "'<'"  */
    MZN_GR_QUOTED = 365,           /* "'>'"  */
    MZN_LQ_QUOTED = 366,           /* "'<='"  */
    MZN_GQ_QUOTED = 367,           /* "'>='"  */
    MZN_EQ_QUOTED = 368,           /* "'='"  */
    MZN_NQ_QUOTED = 369,           /* "'!='"  */
    MZN_IN_QUOTED = 370,           /* "'in'"  */
    MZN_SUBSET_QUOTED = 371,       /* "'subset'"  */
    MZN_SUPERSET_QUOTED = 372,     /* "'superset'"  */
    MZN_UNION_QUOTED = 373,        /* "'union'"  */
    MZN_DIFF_QUOTED = 374,         /* "'diff'"  */
    MZN_SYMDIFF_QUOTED = 375,      /* "'symdiff'"  */
    MZN_DOTDOT_QUOTED = 376,       /* "'..'"  */
    MZN_PLUS_QUOTED = 377,         /* "'+'"  */
    MZN_MINUS_QUOTED = 378,        /* "'-'"  */
    MZN_MULT_QUOTED = 379,         /* "'*'"  */
    MZN_DIV_QUOTED = 380,          /* "'/'"  */
    MZN_IDIV_QUOTED = 381,         /* "'div'"  */
    MZN_MOD_QUOTED = 382,          /* "'mod'"  */
    MZN_INTERSECT_QUOTED = 383,    /* "'intersect'"  */
    MZN_POW_QUOTED = 384,          /* "'^'"  */
    MZN_NOT_QUOTED = 385,          /* "'not'"  */
    MZN_COLONCOLON_QUOTED = 386,   /* "'::'"  */
    MZN_PLUSPLUS_QUOTED = 387      /* "'++'"  */
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
         std::vector<MiniZinc::VarDecl*>* vardeclexprs;
         MiniZinc::TypeInst* tiexpr;
         std::vector<MiniZinc::TypeInst*>* tiexprs;
         MiniZinc::Expression* expression;
         std::vector<MiniZinc::Expression*>* expressions1d;
         std::vector<std::vector<MiniZinc::Expression*> >* expressions2d;
         std::vector<std::vector<std::vector<MiniZinc::Expression*> > >* expressions3d;
         MiniZinc::Generator* generator;
         std::vector<MiniZinc::Generator>* generators;
         std::vector<std::string>* strings;
         std::vector<std::pair<MiniZinc::Expression*,MiniZinc::Expression*> >* expressionPairs;
         MiniZinc::Generators* generatorsPointer;
       


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

#endif /* !YY_MZN_YY_USERS_TACK_PROGRAMMING_MINIZINC_LIBMZN_BUILD_XCODE_INCLUDE_MINIZINC_PARSER_TAB_HH_INCLUDED  */
