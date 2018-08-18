/* A Bison parser, made by GNU Bison 3.0.5.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018 Free Software Foundation, Inc.

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

#ifndef YY_REGEX_YY_USERS_JDEK0001_REPOSITORIES_MINIZINC_MINIZINC_BUILD_RELEASE_INCLUDE_MINIZINC_SUPPORT_REGEX_PARSER_TAB_HH_INCLUDED
# define YY_REGEX_YY_USERS_JDEK0001_REPOSITORIES_MINIZINC_MINIZINC_BUILD_RELEASE_INCLUDE_MINIZINC_SUPPORT_REGEX_PARSER_TAB_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int regex_yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    R_INTEGER = 258,
    R_GROUP_OPEN = 259,
    R_GROUP_CLOSE = 260,
    R_STAR = 261,
    R_PLUS = 262,
    R_ANY = 263,
    R_UNION = 264,
    R_OPTIONAL = 265,
    R_QUANT_OPEN = 266,
    R_QUANT_CLOSE = 267,
    R_COMMA = 268,
    R_CLASS_OPEN = 269,
    R_CLASS_CLOSE = 270,
    R_CLASS_RANGE = 271,
    R_CLASS_NEG = 272,
    R_IDENTIFIER = 273
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{


  int iValue;
  char* sValue;
  std::unordered_set<int>* setValue;
  Gecode::REG* rValue;


};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE regex_yylval;

int regex_yyparse (REContext& ctx);

#endif /* !YY_REGEX_YY_USERS_JDEK0001_REPOSITORIES_MINIZINC_MINIZINC_BUILD_RELEASE_INCLUDE_MINIZINC_SUPPORT_REGEX_PARSER_TAB_HH_INCLUDED  */
