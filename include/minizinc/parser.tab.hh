
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     MZN_INTEGER_LITERAL = 258,
     MZN_BOOL_LITERAL = 259,
     MZN_FLOAT_LITERAL = 260,
     MZN_IDENTIFIER = 261,
     MZN_QUOTED_IDENTIFIER = 262,
     MZN_STRING_LITERAL = 263,
     MZN_TI_IDENTIFIER = 264,
     MZN_VAR = 265,
     MZN_PAR = 266,
     MZN_SVAR = 267,
     MZN_ANN = 268,
     MZN_ANNOTATION = 269,
     MZN_ANY = 270,
     MZN_ARRAY = 271,
     MZN_BOOL = 272,
     MZN_CASE = 273,
     MZN_CONSTRAINT = 274,
     MZN_DEFAULT = 275,
     MZN_ELSE = 276,
     MZN_ELSEIF = 277,
     MZN_ENDIF = 278,
     MZN_ENUM = 279,
     MZN_FLOAT = 280,
     MZN_FUNCTION = 281,
     MZN_IF = 282,
     MZN_INCLUDE = 283,
     MZN_INT = 284,
     MZN_LET = 285,
     MZN_MAXIMIZE = 286,
     MZN_MINIMIZE = 287,
     MZN_OF = 288,
     MZN_SATISFY = 289,
     MZN_OUTPUT = 290,
     MZN_PREDICATE = 291,
     MZN_RECORD = 292,
     MZN_SET = 293,
     MZN_SOLVE = 294,
     MZN_STRING = 295,
     MZN_TEST = 296,
     MZN_THEN = 297,
     MZN_TUPLE = 298,
     MZN_TYPE = 299,
     MZN_UNDERSCORE = 300,
     MZN_VARIANT_RECORD = 301,
     MZN_WHERE = 302,
     MZN_LEFT_BRACKET = 303,
     MZN_LEFT_2D_BRACKET = 304,
     MZN_RIGHT_BRACKET = 305,
     MZN_RIGHT_2D_BRACKET = 306,
     UNKNOWN_CHAR = 307,
     PREC_ANNO = 308,
     MZN_EQUIV = 309,
     MZN_RIMPL = 310,
     MZN_IMPL = 311,
     MZN_XOR = 312,
     MZN_OR = 313,
     MZN_AND = 314,
     MZN_NQ = 315,
     MZN_EQ = 316,
     MZN_GQ = 317,
     MZN_LQ = 318,
     MZN_GR = 319,
     MZN_LE = 320,
     MZN_SUPERSET = 321,
     MZN_SUBSET = 322,
     MZN_IN = 323,
     MZN_SYMDIFF = 324,
     MZN_DIFF = 325,
     MZN_UNION = 326,
     MZN_DOTDOT = 327,
     MZN_MINUS = 328,
     MZN_PLUS = 329,
     MZN_INTERSECT = 330,
     MZN_MOD = 331,
     MZN_IDIV = 332,
     MZN_DIV = 333,
     MZN_MULT = 334,
     MZN_NOT = 335,
     MZN_COLONCOLON = 336,
     MZN_PLUSPLUS = 337,
     MZN_EQUIV_QUOTED = 338,
     MZN_IMPL_QUOTED = 339,
     MZN_RIMPL_QUOTED = 340,
     MZN_OR_QUOTED = 341,
     MZN_XOR_QUOTED = 342,
     MZN_AND_QUOTED = 343,
     MZN_LE_QUOTED = 344,
     MZN_GR_QUOTED = 345,
     MZN_LQ_QUOTED = 346,
     MZN_GQ_QUOTED = 347,
     MZN_EQ_QUOTED = 348,
     MZN_NQ_QUOTED = 349,
     MZN_IN_QUOTED = 350,
     MZN_SUBSET_QUOTED = 351,
     MZN_SUPERSET_QUOTED = 352,
     MZN_UNION_QUOTED = 353,
     MZN_DIFF_QUOTED = 354,
     MZN_SYMDIFF_QUOTED = 355,
     MZN_DOTDOT_QUOTED = 356,
     MZN_PLUS_QUOTED = 357,
     MZN_MINUS_QUOTED = 358,
     MZN_MULT_QUOTED = 359,
     MZN_DIV_QUOTED = 360,
     MZN_IDIV_QUOTED = 361,
     MZN_MOD_QUOTED = 362,
     MZN_INTERSECT_QUOTED = 363,
     MZN_NOT_QUOTED = 364,
     MZN_COLONCOLON_QUOTED = 365,
     MZN_PLUSPLUS_QUOTED = 366
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 224 "lib/parser.yxx"
 int iValue; char* sValue; bool bValue; double dValue;
         MiniZinc::Item* item;
         MiniZinc::VarDecl* vardeclexpr;
         std::vector<MiniZinc::VarDecl*>* vardeclexpr_v;
         MiniZinc::TypeInst* tiexpr;
         std::vector<MiniZinc::TypeInst*>* tiexpr_v;
         MiniZinc::Expression* expression;
         std::vector<MiniZinc::Expression*>* expression_v;
         std::vector<std::vector<MiniZinc::Expression*> >* expression_vv;
         MiniZinc::Generator* generator;
         std::vector<MiniZinc::Generator*>* generator_v;
         std::vector<std::string>* string_v;
         std::pair<std::vector<MiniZinc::Expression*>,
                   MiniZinc::Expression*>* expression_p;
         MiniZinc::Annotation* annotation;
         MiniZinc::Generators* generators;
       


/* Line 1676 of yacc.c  */
#line 183 "include/minizinc/parser.tab.hh"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif



#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



