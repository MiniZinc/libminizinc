/* A Bison parser, made by GNU Bison 3.7.  */

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

#ifndef YY_MZN_YY_USERS_JDEK0001_DROPBOX_DEVELOPMENT_MINIZINC_BUILD_INCLUDE_MINIZINC_PARSER_TAB_HH_INCLUDED
# define YY_MZN_YY_USERS_JDEK0001_DROPBOX_DEVELOPMENT_MINIZINC_BUILD_INCLUDE_MINIZINC_PARSER_TAB_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
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
    MZN_DOC_COMMENT = 268,         /* "documentation comment"  */
    MZN_DOC_FILE_COMMENT = 269,    /* "file-level documentation comment"  */
    MZN_VAR = 270,                 /* "var"  */
    MZN_PAR = 271,                 /* "par"  */
    MZN_ABSENT = 272,              /* "<>"  */
    MZN_ANN = 273,                 /* "ann"  */
    MZN_ANNOTATION = 274,          /* "annotation"  */
    MZN_ANY = 275,                 /* "any"  */
    MZN_ARRAY = 276,               /* "array"  */
    MZN_BOOL = 277,                /* "bool"  */
    MZN_CASE = 278,                /* "case"  */
    MZN_CONSTRAINT = 279,          /* "constraint"  */
    MZN_DEFAULT = 280,             /* "default"  */
    MZN_ELSE = 281,                /* "else"  */
    MZN_ELSEIF = 282,              /* "elseif"  */
    MZN_ENDIF = 283,               /* "endif"  */
    MZN_ENUM = 284,                /* "enum"  */
    MZN_FLOAT = 285,               /* "float"  */
    MZN_FUNCTION = 286,            /* "function"  */
    MZN_IF = 287,                  /* "if"  */
    MZN_INCLUDE = 288,             /* "include"  */
    MZN_INFINITY = 289,            /* "infinity"  */
    MZN_INT = 290,                 /* "int"  */
    MZN_LET = 291,                 /* "let"  */
    MZN_LIST = 292,                /* "list"  */
    MZN_MAXIMIZE = 293,            /* "maximize"  */
    MZN_MINIMIZE = 294,            /* "minimize"  */
    MZN_OF = 295,                  /* "of"  */
    MZN_OPT = 296,                 /* "opt"  */
    MZN_SATISFY = 297,             /* "satisfy"  */
    MZN_OUTPUT = 298,              /* "output"  */
    MZN_PREDICATE = 299,           /* "predicate"  */
    MZN_RECORD = 300,              /* "record"  */
    MZN_SEARCH = 301,              /* "search"  */
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
    MZN_INVALID_NULL = 320,        /* "null character"  */
    MZN_EQUIV = 321,               /* "<->"  */
    MZN_IMPL = 322,                /* "->"  */
    MZN_RIMPL = 323,               /* "<-"  */
    MZN_OR = 324,                  /* "\\/"  */
    MZN_XOR = 325,                 /* "xor"  */
    MZN_AND = 326,                 /* "/\\"  */
    MZN_LE = 327,                  /* "<"  */
    MZN_GR = 328,                  /* ">"  */
    MZN_LQ = 329,                  /* "<="  */
    MZN_GQ = 330,                  /* ">="  */
    MZN_EQ = 331,                  /* "="  */
    MZN_NQ = 332,                  /* "!="  */
    MZN_ASSIGN = 333,              /* ":="  */
    MZN_IN = 334,                  /* "in"  */
    MZN_SUBSET = 335,              /* "subset"  */
    MZN_SUPERSET = 336,            /* "superset"  */
    MZN_UNION = 337,               /* "union"  */
    MZN_DIFF = 338,                /* "diff"  */
    MZN_SYMDIFF = 339,             /* "symdiff"  */
    MZN_DOTDOT = 340,              /* ".."  */
    MZN_PLUS = 341,                /* "+"  */
    MZN_MINUS = 342,               /* "-"  */
    MZN_MULT = 343,                /* "*"  */
    MZN_DIV = 344,                 /* "/"  */
    MZN_IDIV = 345,                /* "div"  */
    MZN_MOD = 346,                 /* "mod"  */
    MZN_INTERSECT = 347,           /* "intersect"  */
    MZN_NOT = 348,                 /* "not"  */
    MZN_PLUSPLUS = 349,            /* "++"  */
    MZN_COLONCOLON = 350,          /* "::"  */
    PREC_ANNO = 351,               /* PREC_ANNO  */
    MZN_EQUIV_QUOTED = 352,        /* "'<->'"  */
    MZN_IMPL_QUOTED = 353,         /* "'->'"  */
    MZN_RIMPL_QUOTED = 354,        /* "'<-'"  */
    MZN_OR_QUOTED = 355,           /* "'\\/'"  */
    MZN_XOR_QUOTED = 356,          /* "'xor'"  */
    MZN_AND_QUOTED = 357,          /* "'/\\'"  */
    MZN_LE_QUOTED = 358,           /* "'<'"  */
    MZN_GR_QUOTED = 359,           /* "'>'"  */
    MZN_LQ_QUOTED = 360,           /* "'<='"  */
    MZN_GQ_QUOTED = 361,           /* "'>='"  */
    MZN_EQ_QUOTED = 362,           /* "'='"  */
    MZN_NQ_QUOTED = 363,           /* "'!='"  */
    MZN_IN_QUOTED = 364,           /* "'in'"  */
    MZN_SUBSET_QUOTED = 365,       /* "'subset'"  */
    MZN_SUPERSET_QUOTED = 366,     /* "'superset'"  */
    MZN_UNION_QUOTED = 367,        /* "'union'"  */
    MZN_DIFF_QUOTED = 368,         /* "'diff'"  */
    MZN_SYMDIFF_QUOTED = 369,      /* "'symdiff'"  */
    MZN_DOTDOT_QUOTED = 370,       /* "'..'"  */
    MZN_PLUS_QUOTED = 371,         /* "'+'"  */
    MZN_MINUS_QUOTED = 372,        /* "'-'"  */
    MZN_MULT_QUOTED = 373,         /* "'*'"  */
    MZN_DIV_QUOTED = 374,          /* "'/'"  */
    MZN_IDIV_QUOTED = 375,         /* "'div'"  */
    MZN_MOD_QUOTED = 376,          /* "'mod'"  */
    MZN_INTERSECT_QUOTED = 377,    /* "'intersect'"  */
    MZN_NOT_QUOTED = 378,          /* "'not'"  */
    MZN_COLONCOLON_QUOTED = 379,   /* "'::'"  */
    MZN_PLUSPLUS_QUOTED = 380      /* "'++'"  */
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
         std::pair<std::vector<MiniZinc::Expression*>,
                   MiniZinc::Expression*>* expression_p;
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



int yyparse (void *parm);

#endif /* !YY_MZN_YY_USERS_JDEK0001_DROPBOX_DEVELOPMENT_MINIZINC_BUILD_INCLUDE_MINIZINC_PARSER_TAB_HH_INCLUDED  */
