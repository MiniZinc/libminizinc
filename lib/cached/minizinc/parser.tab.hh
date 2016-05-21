/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
     END = 0,
     MZN_INTEGER_LITERAL = 258,
     MZN_BOOL_LITERAL = 259,
     MZN_FLOAT_LITERAL = 260,
     MZN_IDENTIFIER = 261,
     MZN_QUOTED_IDENTIFIER = 262,
     MZN_STRING_LITERAL = 263,
     MZN_STRING_QUOTE_START = 264,
     MZN_STRING_QUOTE_MID = 265,
     MZN_STRING_QUOTE_END = 266,
     MZN_TI_IDENTIFIER = 267,
     MZN_DOC_COMMENT = 268,
     MZN_DOC_FILE_COMMENT = 269,
     MZN_VAR = 270,
     MZN_PAR = 271,
     MZN_ABSENT = 272,
     MZN_ANN = 273,
     MZN_ANNOTATION = 274,
     MZN_ANY = 275,
     MZN_ARRAY = 276,
     MZN_BOOL = 277,
     MZN_CASE = 278,
     MZN_CONSTRAINT = 279,
     MZN_DEFAULT = 280,
     MZN_ELSE = 281,
     MZN_ELSEIF = 282,
     MZN_ENDIF = 283,
     MZN_ENUM = 284,
     MZN_FLOAT = 285,
     MZN_FUNCTION = 286,
     MZN_IF = 287,
     MZN_INCLUDE = 288,
     MZN_INFINITY = 289,
     MZN_INT = 290,
     MZN_LET = 291,
     MZN_LIST = 292,
     MZN_MAXIMIZE = 293,
     MZN_MINIMIZE = 294,
     MZN_OF = 295,
     MZN_OPT = 296,
     MZN_SATISFY = 297,
     MZN_OUTPUT = 298,
     MZN_PREDICATE = 299,
     MZN_RECORD = 300,
     MZN_SET = 301,
     MZN_SOLVE = 302,
     MZN_STRING = 303,
     MZN_TEST = 304,
     MZN_THEN = 305,
     MZN_TUPLE = 306,
     MZN_TYPE = 307,
     MZN_UNDERSCORE = 308,
     MZN_VARIANT_RECORD = 309,
     MZN_WHERE = 310,
     MZN_LEFT_BRACKET = 311,
     MZN_LEFT_2D_BRACKET = 312,
     MZN_RIGHT_BRACKET = 313,
     MZN_RIGHT_2D_BRACKET = 314,
     FLATZINC_IDENTIFIER = 315,
     MZN_INVALID_INTEGER_LITERAL = 316,
     MZN_INVALID_FLOAT_LITERAL = 317,
     MZN_UNTERMINATED_STRING = 318,
     MZN_INVALID_NULL = 319,
     MZN_EQUIV = 320,
     MZN_IMPL = 321,
     MZN_RIMPL = 322,
     MZN_OR = 323,
     MZN_XOR = 324,
     MZN_AND = 325,
     MZN_LE = 326,
     MZN_GR = 327,
     MZN_LQ = 328,
     MZN_GQ = 329,
     MZN_EQ = 330,
     MZN_NQ = 331,
     MZN_IN = 332,
     MZN_SUBSET = 333,
     MZN_SUPERSET = 334,
     MZN_UNION = 335,
     MZN_DIFF = 336,
     MZN_SYMDIFF = 337,
     MZN_DOTDOT = 338,
     MZN_PLUS = 339,
     MZN_MINUS = 340,
     MZN_MULT = 341,
     MZN_DIV = 342,
     MZN_IDIV = 343,
     MZN_MOD = 344,
     MZN_INTERSECT = 345,
     MZN_NOT = 346,
     MZN_PLUSPLUS = 347,
     MZN_COLONCOLON = 348,
     PREC_ANNO = 349,
     MZN_EQUIV_QUOTED = 350,
     MZN_IMPL_QUOTED = 351,
     MZN_RIMPL_QUOTED = 352,
     MZN_OR_QUOTED = 353,
     MZN_XOR_QUOTED = 354,
     MZN_AND_QUOTED = 355,
     MZN_LE_QUOTED = 356,
     MZN_GR_QUOTED = 357,
     MZN_LQ_QUOTED = 358,
     MZN_GQ_QUOTED = 359,
     MZN_EQ_QUOTED = 360,
     MZN_NQ_QUOTED = 361,
     MZN_IN_QUOTED = 362,
     MZN_SUBSET_QUOTED = 363,
     MZN_SUPERSET_QUOTED = 364,
     MZN_UNION_QUOTED = 365,
     MZN_DIFF_QUOTED = 366,
     MZN_SYMDIFF_QUOTED = 367,
     MZN_DOTDOT_QUOTED = 368,
     MZN_PLUS_QUOTED = 369,
     MZN_MINUS_QUOTED = 370,
     MZN_MULT_QUOTED = 371,
     MZN_DIV_QUOTED = 372,
     MZN_IDIV_QUOTED = 373,
     MZN_MOD_QUOTED = 374,
     MZN_INTERSECT_QUOTED = 375,
     MZN_NOT_QUOTED = 376,
     MZN_COLONCOLON_QUOTED = 377,
     MZN_PLUSPLUS_QUOTED = 378
   };
#endif
/* Tokens.  */
#define END 0
#define MZN_INTEGER_LITERAL 258
#define MZN_BOOL_LITERAL 259
#define MZN_FLOAT_LITERAL 260
#define MZN_IDENTIFIER 261
#define MZN_QUOTED_IDENTIFIER 262
#define MZN_STRING_LITERAL 263
#define MZN_STRING_QUOTE_START 264
#define MZN_STRING_QUOTE_MID 265
#define MZN_STRING_QUOTE_END 266
#define MZN_TI_IDENTIFIER 267
#define MZN_DOC_COMMENT 268
#define MZN_DOC_FILE_COMMENT 269
#define MZN_VAR 270
#define MZN_PAR 271
#define MZN_ABSENT 272
#define MZN_ANN 273
#define MZN_ANNOTATION 274
#define MZN_ANY 275
#define MZN_ARRAY 276
#define MZN_BOOL 277
#define MZN_CASE 278
#define MZN_CONSTRAINT 279
#define MZN_DEFAULT 280
#define MZN_ELSE 281
#define MZN_ELSEIF 282
#define MZN_ENDIF 283
#define MZN_ENUM 284
#define MZN_FLOAT 285
#define MZN_FUNCTION 286
#define MZN_IF 287
#define MZN_INCLUDE 288
#define MZN_INFINITY 289
#define MZN_INT 290
#define MZN_LET 291
#define MZN_LIST 292
#define MZN_MAXIMIZE 293
#define MZN_MINIMIZE 294
#define MZN_OF 295
#define MZN_OPT 296
#define MZN_SATISFY 297
#define MZN_OUTPUT 298
#define MZN_PREDICATE 299
#define MZN_RECORD 300
#define MZN_SET 301
#define MZN_SOLVE 302
#define MZN_STRING 303
#define MZN_TEST 304
#define MZN_THEN 305
#define MZN_TUPLE 306
#define MZN_TYPE 307
#define MZN_UNDERSCORE 308
#define MZN_VARIANT_RECORD 309
#define MZN_WHERE 310
#define MZN_LEFT_BRACKET 311
#define MZN_LEFT_2D_BRACKET 312
#define MZN_RIGHT_BRACKET 313
#define MZN_RIGHT_2D_BRACKET 314
#define FLATZINC_IDENTIFIER 315
#define MZN_INVALID_INTEGER_LITERAL 316
#define MZN_INVALID_FLOAT_LITERAL 317
#define MZN_UNTERMINATED_STRING 318
#define MZN_INVALID_NULL 319
#define MZN_EQUIV 320
#define MZN_IMPL 321
#define MZN_RIMPL 322
#define MZN_OR 323
#define MZN_XOR 324
#define MZN_AND 325
#define MZN_LE 326
#define MZN_GR 327
#define MZN_LQ 328
#define MZN_GQ 329
#define MZN_EQ 330
#define MZN_NQ 331
#define MZN_IN 332
#define MZN_SUBSET 333
#define MZN_SUPERSET 334
#define MZN_UNION 335
#define MZN_DIFF 336
#define MZN_SYMDIFF 337
#define MZN_DOTDOT 338
#define MZN_PLUS 339
#define MZN_MINUS 340
#define MZN_MULT 341
#define MZN_DIV 342
#define MZN_IDIV 343
#define MZN_MOD 344
#define MZN_INTERSECT 345
#define MZN_NOT 346
#define MZN_PLUSPLUS 347
#define MZN_COLONCOLON 348
#define PREC_ANNO 349
#define MZN_EQUIV_QUOTED 350
#define MZN_IMPL_QUOTED 351
#define MZN_RIMPL_QUOTED 352
#define MZN_OR_QUOTED 353
#define MZN_XOR_QUOTED 354
#define MZN_AND_QUOTED 355
#define MZN_LE_QUOTED 356
#define MZN_GR_QUOTED 357
#define MZN_LQ_QUOTED 358
#define MZN_GQ_QUOTED 359
#define MZN_EQ_QUOTED 360
#define MZN_NQ_QUOTED 361
#define MZN_IN_QUOTED 362
#define MZN_SUBSET_QUOTED 363
#define MZN_SUPERSET_QUOTED 364
#define MZN_UNION_QUOTED 365
#define MZN_DIFF_QUOTED 366
#define MZN_SYMDIFF_QUOTED 367
#define MZN_DOTDOT_QUOTED 368
#define MZN_PLUS_QUOTED 369
#define MZN_MINUS_QUOTED 370
#define MZN_MULT_QUOTED 371
#define MZN_DIV_QUOTED 372
#define MZN_IDIV_QUOTED 373
#define MZN_MOD_QUOTED 374
#define MZN_INTERSECT_QUOTED 375
#define MZN_NOT_QUOTED 376
#define MZN_COLONCOLON_QUOTED 377
#define MZN_PLUSPLUS_QUOTED 378




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE

{ long long int iValue; char* sValue; bool bValue; double dValue;
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
       }
/* Line 1529 of yacc.c.  */

	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
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


