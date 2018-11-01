/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 1

/* Substitute the variable and function names.  */
#define yyparse mzn_yyparse
#define yylex   mzn_yylex
#define yyerror mzn_yyerror
#define yylval  mzn_yylval
#define yychar  mzn_yychar
#define yydebug mzn_yydebug
#define yynerrs mzn_yynerrs
#define yylloc mzn_yylloc

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
     MZN_TI_ENUM_IDENTIFIER = 268,
     MZN_DOC_COMMENT = 269,
     MZN_DOC_FILE_COMMENT = 270,
     MZN_VAR = 271,
     MZN_PAR = 272,
     MZN_ABSENT = 273,
     MZN_ANN = 274,
     MZN_ANNOTATION = 275,
     MZN_ANY = 276,
     MZN_ARRAY = 277,
     MZN_BOOL = 278,
     MZN_CASE = 279,
     MZN_CONSTRAINT = 280,
     MZN_DEFAULT = 281,
     MZN_ELSE = 282,
     MZN_ELSEIF = 283,
     MZN_ENDIF = 284,
     MZN_ENUM = 285,
     MZN_FLOAT = 286,
     MZN_FUNCTION = 287,
     MZN_IF = 288,
     MZN_INCLUDE = 289,
     MZN_INFINITY = 290,
     MZN_INT = 291,
     MZN_LET = 292,
     MZN_LIST = 293,
     MZN_MAXIMIZE = 294,
     MZN_MINIMIZE = 295,
     MZN_OF = 296,
     MZN_OPT = 297,
     MZN_SATISFY = 298,
     MZN_OUTPUT = 299,
     MZN_PREDICATE = 300,
     MZN_RECORD = 301,
     MZN_SET = 302,
     MZN_SOLVE = 303,
     MZN_STRING = 304,
     MZN_TEST = 305,
     MZN_THEN = 306,
     MZN_TUPLE = 307,
     MZN_TYPE = 308,
     MZN_UNDERSCORE = 309,
     MZN_VARIANT_RECORD = 310,
     MZN_WHERE = 311,
     MZN_LEFT_BRACKET = 312,
     MZN_LEFT_2D_BRACKET = 313,
     MZN_RIGHT_BRACKET = 314,
     MZN_RIGHT_2D_BRACKET = 315,
     FLATZINC_IDENTIFIER = 316,
     MZN_INVALID_INTEGER_LITERAL = 317,
     MZN_INVALID_FLOAT_LITERAL = 318,
     MZN_UNTERMINATED_STRING = 319,
     MZN_END_OF_LINE_IN_STRING = 320,
     MZN_INVALID_NULL = 321,
     MZN_EQUIV = 322,
     MZN_IMPL = 323,
     MZN_RIMPL = 324,
     MZN_OR = 325,
     MZN_XOR = 326,
     MZN_AND = 327,
     MZN_LE = 328,
     MZN_GR = 329,
     MZN_LQ = 330,
     MZN_GQ = 331,
     MZN_EQ = 332,
     MZN_NQ = 333,
     MZN_WEAK_EQ = 334,
     MZN_IN = 335,
     MZN_SUBSET = 336,
     MZN_SUPERSET = 337,
     MZN_UNION = 338,
     MZN_DIFF = 339,
     MZN_SYMDIFF = 340,
     MZN_DOTDOT = 341,
     MZN_PLUS = 342,
     MZN_MINUS = 343,
     MZN_WEAK_PLUS = 344,
     MZN_WEAK_MINUS = 345,
     MZN_MULT = 346,
     MZN_DIV = 347,
     MZN_IDIV = 348,
     MZN_MOD = 349,
     MZN_INTERSECT = 350,
     MZN_WEAK_MULT = 351,
     MZN_POW = 352,
     MZN_NOT = 353,
     MZN_PLUSPLUS = 354,
     MZN_COLONCOLON = 355,
     PREC_ANNO = 356,
     MZN_EQUIV_QUOTED = 357,
     MZN_IMPL_QUOTED = 358,
     MZN_RIMPL_QUOTED = 359,
     MZN_OR_QUOTED = 360,
     MZN_XOR_QUOTED = 361,
     MZN_AND_QUOTED = 362,
     MZN_LE_QUOTED = 363,
     MZN_GR_QUOTED = 364,
     MZN_LQ_QUOTED = 365,
     MZN_GQ_QUOTED = 366,
     MZN_EQ_QUOTED = 367,
     MZN_NQ_QUOTED = 368,
     MZN_IN_QUOTED = 369,
     MZN_SUBSET_QUOTED = 370,
     MZN_SUPERSET_QUOTED = 371,
     MZN_UNION_QUOTED = 372,
     MZN_DIFF_QUOTED = 373,
     MZN_SYMDIFF_QUOTED = 374,
     MZN_DOTDOT_QUOTED = 375,
     MZN_PLUS_QUOTED = 376,
     MZN_MINUS_QUOTED = 377,
     MZN_MULT_QUOTED = 378,
     MZN_DIV_QUOTED = 379,
     MZN_IDIV_QUOTED = 380,
     MZN_MOD_QUOTED = 381,
     MZN_INTERSECT_QUOTED = 382,
     MZN_POW_QUOTED = 383,
     MZN_NOT_QUOTED = 384,
     MZN_COLONCOLON_QUOTED = 385,
     MZN_PLUSPLUS_QUOTED = 386
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
#define MZN_TI_ENUM_IDENTIFIER 268
#define MZN_DOC_COMMENT 269
#define MZN_DOC_FILE_COMMENT 270
#define MZN_VAR 271
#define MZN_PAR 272
#define MZN_ABSENT 273
#define MZN_ANN 274
#define MZN_ANNOTATION 275
#define MZN_ANY 276
#define MZN_ARRAY 277
#define MZN_BOOL 278
#define MZN_CASE 279
#define MZN_CONSTRAINT 280
#define MZN_DEFAULT 281
#define MZN_ELSE 282
#define MZN_ELSEIF 283
#define MZN_ENDIF 284
#define MZN_ENUM 285
#define MZN_FLOAT 286
#define MZN_FUNCTION 287
#define MZN_IF 288
#define MZN_INCLUDE 289
#define MZN_INFINITY 290
#define MZN_INT 291
#define MZN_LET 292
#define MZN_LIST 293
#define MZN_MAXIMIZE 294
#define MZN_MINIMIZE 295
#define MZN_OF 296
#define MZN_OPT 297
#define MZN_SATISFY 298
#define MZN_OUTPUT 299
#define MZN_PREDICATE 300
#define MZN_RECORD 301
#define MZN_SET 302
#define MZN_SOLVE 303
#define MZN_STRING 304
#define MZN_TEST 305
#define MZN_THEN 306
#define MZN_TUPLE 307
#define MZN_TYPE 308
#define MZN_UNDERSCORE 309
#define MZN_VARIANT_RECORD 310
#define MZN_WHERE 311
#define MZN_LEFT_BRACKET 312
#define MZN_LEFT_2D_BRACKET 313
#define MZN_RIGHT_BRACKET 314
#define MZN_RIGHT_2D_BRACKET 315
#define FLATZINC_IDENTIFIER 316
#define MZN_INVALID_INTEGER_LITERAL 317
#define MZN_INVALID_FLOAT_LITERAL 318
#define MZN_UNTERMINATED_STRING 319
#define MZN_END_OF_LINE_IN_STRING 320
#define MZN_INVALID_NULL 321
#define MZN_EQUIV 322
#define MZN_IMPL 323
#define MZN_RIMPL 324
#define MZN_OR 325
#define MZN_XOR 326
#define MZN_AND 327
#define MZN_LE 328
#define MZN_GR 329
#define MZN_LQ 330
#define MZN_GQ 331
#define MZN_EQ 332
#define MZN_NQ 333
#define MZN_WEAK_EQ 334
#define MZN_IN 335
#define MZN_SUBSET 336
#define MZN_SUPERSET 337
#define MZN_UNION 338
#define MZN_DIFF 339
#define MZN_SYMDIFF 340
#define MZN_DOTDOT 341
#define MZN_PLUS 342
#define MZN_MINUS 343
#define MZN_WEAK_PLUS 344
#define MZN_WEAK_MINUS 345
#define MZN_MULT 346
#define MZN_DIV 347
#define MZN_IDIV 348
#define MZN_MOD 349
#define MZN_INTERSECT 350
#define MZN_WEAK_MULT 351
#define MZN_POW 352
#define MZN_NOT 353
#define MZN_PLUSPLUS 354
#define MZN_COLONCOLON 355
#define PREC_ANNO 356
#define MZN_EQUIV_QUOTED 357
#define MZN_IMPL_QUOTED 358
#define MZN_RIMPL_QUOTED 359
#define MZN_OR_QUOTED 360
#define MZN_XOR_QUOTED 361
#define MZN_AND_QUOTED 362
#define MZN_LE_QUOTED 363
#define MZN_GR_QUOTED 364
#define MZN_LQ_QUOTED 365
#define MZN_GQ_QUOTED 366
#define MZN_EQ_QUOTED 367
#define MZN_NQ_QUOTED 368
#define MZN_IN_QUOTED 369
#define MZN_SUBSET_QUOTED 370
#define MZN_SUPERSET_QUOTED 371
#define MZN_UNION_QUOTED 372
#define MZN_DIFF_QUOTED 373
#define MZN_SYMDIFF_QUOTED 374
#define MZN_DOTDOT_QUOTED 375
#define MZN_PLUS_QUOTED 376
#define MZN_MINUS_QUOTED 377
#define MZN_MULT_QUOTED 378
#define MZN_DIV_QUOTED 379
#define MZN_IDIV_QUOTED 380
#define MZN_MOD_QUOTED 381
#define MZN_INTERSECT_QUOTED 382
#define MZN_POW_QUOTED 383
#define MZN_NOT_QUOTED 384
#define MZN_COLONCOLON_QUOTED 385
#define MZN_PLUSPLUS_QUOTED 386




/* Copy the first part of user declarations.  */


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
  Current.filename(Rhs[1].filename()); \
  Current.first_line(Rhs[1].first_line()); \
  Current.first_column(Rhs[1].first_column()); \
  Current.last_line(Rhs[N].last_line()); \
  Current.last_column(Rhs[N].last_column());

int mzn_yyparse(void*);
int mzn_yylex(YYSTYPE*, YYLTYPE*, void* scanner);
int mzn_yylex_init (void** scanner);
int mzn_yylex_destroy (void* scanner);
int mzn_yyget_lineno (void* scanner);
void mzn_yyset_extra (void* user_defined ,void* yyscanner );

extern int yydebug;

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





/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

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
         std::vector<std::pair<MiniZinc::Expression*,MiniZinc::Expression*> >* expression_p;
         MiniZinc::Generators* generators;
       }
/* Line 193 of yacc.c.  */

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


/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */


#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
    YYLTYPE yyls;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

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
/* YYNRULES -- Number of states.  */
#define YYNSTATES  540

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   386

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
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
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    11,    14,    18,    23,
      26,    30,    32,    35,    36,    38,    41,    43,    45,    47,
      49,    51,    53,    55,    57,    59,    61,    63,    65,    67,
      69,    71,    73,    75,    77,    79,    82,    85,    90,    93,
     100,   107,   115,   116,   118,   122,   123,   125,   129,   133,
     136,   141,   145,   150,   155,   158,   164,   170,   178,   187,
     191,   197,   198,   201,   202,   206,   210,   211,   214,   216,
     220,   221,   223,   225,   227,   231,   234,   236,   240,   242,
     249,   253,   255,   258,   262,   266,   271,   277,   283,   284,
     286,   288,   290,   292,   294,   296,   298,   300,   302,   305,
     307,   311,   313,   315,   318,   321,   324,   326,   330,   332,
     336,   340,   344,   348,   352,   359,   363,   367,   371,   375,
     379,   383,   387,   391,   395,   399,   403,   407,   411,   415,
     418,   421,   423,   427,   431,   435,   439,   443,   447,   451,
     455,   459,   463,   467,   471,   475,   479,   483,   487,   491,
     495,   499,   503,   510,   514,   518,   522,   526,   530,   534,
     538,   542,   546,   550,   554,   558,   562,   566,   569,   572,
     575,   577,   579,   583,   588,   590,   593,   595,   598,   600,
     602,   604,   606,   608,   610,   613,   615,   618,   620,   623,
     625,   628,   630,   633,   635,   638,   640,   642,   645,   647,
     650,   653,   657,   661,   666,   669,   673,   679,   681,   684,
     686,   688,   692,   696,   700,   706,   710,   716,   720,   723,
     725,   729,   732,   736,   739,   743,   748,   752,   755,   759,
     765,   767,   771,   777,   783,   792,   793,   799,   801,   803,
     805,   807,   809,   811,   813,   815,   817,   819,   821,   823,
     825,   827,   829,   831,   833,   835,   837,   839,   841,   843,
     845,   847,   849,   851,   853,   855,   862,   867,   871,   873,
     878,   886,   889,   891,   895,   899,   905,   912,   920,   922,
     924,   928,   932,   934,   936,   939,   944,   945,   947,   949,
     951,   954,   958,   960,   962,   964,   966,   968,   970,   972,
     974,   976,   978,   980,   982,   984,   986,   988,   990,   992,
     994,   996,   998,  1000,  1002,  1004,  1006,  1008,  1010,  1012,
    1014,  1016
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     141,     0,    -1,   142,    -1,    -1,   143,   145,    -1,   146,
      -1,   144,   146,    -1,   143,   132,   146,    -1,   143,   132,
     144,   146,    -1,   146,   148,    -1,     1,   132,   146,    -1,
      15,    -1,   144,    15,    -1,    -1,   132,    -1,    14,   147,
      -1,   147,    -1,   149,    -1,   150,    -1,   153,    -1,   154,
      -1,   155,    -1,   156,    -1,   157,    -1,   158,    -1,   159,
      -1,    34,    -1,    30,    -1,    44,    -1,    25,    -1,    48,
      -1,    45,    -1,    32,    -1,    50,    -1,    20,    -1,    34,
       8,    -1,   166,   210,    -1,   166,   210,    77,   179,    -1,
      30,     6,    -1,    30,     6,    77,   133,   152,   134,    -1,
      30,     6,    77,    57,   151,    59,    -1,    30,     6,    77,
       6,   135,   179,   136,    -1,    -1,     8,    -1,   151,   137,
       8,    -1,    -1,     6,    -1,   152,   137,     6,    -1,     6,
      77,   179,    -1,    25,   179,    -1,    25,   100,   182,   179,
      -1,    48,   210,    43,    -1,    48,   210,    40,   179,    -1,
      48,   210,    39,   179,    -1,    44,   179,    -1,    45,     6,
     161,   210,   160,    -1,    50,     6,   161,   210,   160,    -1,
      32,   169,   138,   213,   161,   210,   160,    -1,   169,   138,
       6,   135,   162,   136,   210,   160,    -1,    20,     6,   161,
      -1,    20,     6,   161,    77,   179,    -1,    -1,    77,   179,
      -1,    -1,   135,   162,   136,    -1,   135,     1,   136,    -1,
      -1,   163,   164,    -1,   165,    -1,   163,   137,   165,    -1,
      -1,   137,    -1,   166,    -1,   169,    -1,   169,   138,     6,
      -1,   168,   164,    -1,   169,    -1,   168,   137,   169,    -1,
     170,    -1,    22,    57,   167,    59,    41,   170,    -1,    38,
      41,   170,    -1,   172,    -1,    42,   172,    -1,    17,   171,
     172,    -1,    16,   171,   172,    -1,   171,    47,    41,   172,
      -1,    17,   171,    47,    41,   172,    -1,    16,   171,    47,
      41,   172,    -1,    -1,    42,    -1,    36,    -1,    23,    -1,
      31,    -1,    49,    -1,    19,    -1,   178,    -1,    12,    -1,
      13,    -1,   174,   164,    -1,   175,    -1,   174,   137,   175,
      -1,   179,    -1,    86,    -1,    86,   179,    -1,   179,    86,
      -1,   177,   164,    -1,   179,    -1,   177,   137,   179,    -1,
     180,    -1,   178,   100,   211,    -1,   178,    83,   178,    -1,
     178,    84,   178,    -1,   178,    85,   178,    -1,   178,    86,
     178,    -1,   120,   135,   179,   137,   179,   136,    -1,   178,
      95,   178,    -1,   178,    99,   178,    -1,   178,    87,   178,
      -1,   178,    88,   178,    -1,   178,    91,   178,    -1,   178,
      92,   178,    -1,   178,    93,   178,    -1,   178,    94,   178,
      -1,   178,    97,   178,    -1,   178,    89,   178,    -1,   178,
      90,   178,    -1,   178,    96,   178,    -1,   178,    79,   178,
      -1,   178,     7,   178,    -1,    87,   178,    -1,    88,   178,
      -1,   180,    -1,   179,   100,   211,    -1,   179,    67,   179,
      -1,   179,    68,   179,    -1,   179,    69,   179,    -1,   179,
      70,   179,    -1,   179,    71,   179,    -1,   179,    72,   179,
      -1,   179,    73,   179,    -1,   179,    74,   179,    -1,   179,
      75,   179,    -1,   179,    76,   179,    -1,   179,    77,   179,
      -1,   179,    78,   179,    -1,   179,    80,   179,    -1,   179,
      81,   179,    -1,   179,    82,   179,    -1,   179,    83,   179,
      -1,   179,    84,   179,    -1,   179,    85,   179,    -1,   179,
      86,   179,    -1,   120,   135,   179,   137,   179,   136,    -1,
     179,    95,   179,    -1,   179,    99,   179,    -1,   179,    87,
     179,    -1,   179,    88,   179,    -1,   179,    91,   179,    -1,
     179,    92,   179,    -1,   179,    93,   179,    -1,   179,    94,
     179,    -1,   179,    97,   179,    -1,   179,    89,   179,    -1,
     179,    90,   179,    -1,   179,    96,   179,    -1,   179,    79,
     179,    -1,   179,     7,   179,    -1,    98,   179,    -1,    87,
     179,    -1,    88,   179,    -1,   181,    -1,   182,    -1,   135,
     179,   136,    -1,   135,   179,   136,   184,    -1,     6,    -1,
       6,   184,    -1,    54,    -1,    54,   184,    -1,     4,    -1,
       3,    -1,    35,    -1,     5,    -1,    18,    -1,   185,    -1,
     185,   184,    -1,   186,    -1,   186,   184,    -1,   194,    -1,
     194,   184,    -1,   195,    -1,   195,   184,    -1,   198,    -1,
     198,   184,    -1,   199,    -1,   199,   184,    -1,   206,    -1,
     203,    -1,   203,   184,    -1,     8,    -1,     9,   183,    -1,
     177,    11,    -1,   177,    10,   183,    -1,    57,   173,    59,
      -1,   184,    57,   173,    59,    -1,   133,   134,    -1,   133,
     176,   134,    -1,   133,   179,   139,   187,   134,    -1,   188,
      -1,   189,   164,    -1,   190,    -1,   191,    -1,   191,    56,
     179,    -1,   189,   137,   190,    -1,   189,   137,   191,    -1,
     189,   137,   191,    56,   179,    -1,   192,    80,   179,    -1,
     192,    80,   179,    56,   179,    -1,     6,    77,   179,    -1,
     193,   164,    -1,     6,    -1,   193,   137,     6,    -1,    57,
      59,    -1,    57,   176,    59,    -1,    58,    60,    -1,    58,
     197,    60,    -1,    58,   197,   139,    60,    -1,    58,   196,
      60,    -1,   139,   139,    -1,   139,   197,   139,    -1,   196,
     137,   139,   197,   139,    -1,   176,    -1,   197,   139,   176,
      -1,    57,   179,   139,   187,    59,    -1,    33,   179,    51,
     179,    29,    -1,    33,   179,    51,   179,   200,    27,   179,
      29,    -1,    -1,   200,    28,   179,    51,   179,    -1,   102,
      -1,   103,    -1,   104,    -1,   105,    -1,   106,    -1,   107,
      -1,   108,    -1,   109,    -1,   110,    -1,   111,    -1,   112,
      -1,   113,    -1,   114,    -1,   115,    -1,   116,    -1,   117,
      -1,   118,    -1,   119,    -1,   121,    -1,   122,    -1,   123,
      -1,   128,    -1,   124,    -1,   125,    -1,   126,    -1,   127,
      -1,   131,    -1,   129,    -1,   201,   135,   179,   137,   179,
     136,    -1,   201,   135,   179,   136,    -1,     6,   135,   136,
      -1,   202,    -1,     6,   135,   204,   136,    -1,     6,   135,
     204,   136,   135,   179,   136,    -1,   205,   164,    -1,   179,
      -1,   179,    56,   179,    -1,   205,   137,   179,    -1,   205,
     137,   179,    56,   179,    -1,    37,   133,   207,   134,    80,
     179,    -1,    37,   133,   207,   208,   134,    80,   179,    -1,
     209,    -1,   154,    -1,   207,   208,   209,    -1,   207,   208,
     154,    -1,   137,    -1,   132,    -1,   166,   210,    -1,   166,
     210,    77,   179,    -1,    -1,   212,    -1,   181,    -1,   182,
      -1,   100,   211,    -1,   212,   100,   211,    -1,     6,    -1,
     102,    -1,   103,    -1,   104,    -1,   105,    -1,   106,    -1,
     107,    -1,   108,    -1,   109,    -1,   110,    -1,   111,    -1,
     112,    -1,   113,    -1,   114,    -1,   115,    -1,   116,    -1,
     117,    -1,   118,    -1,   119,    -1,   120,    -1,   121,    -1,
     122,    -1,   123,    -1,   128,    -1,   124,    -1,   125,    -1,
     126,    -1,   127,    -1,   129,    -1,   131,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   269,   269,   271,   273,   276,   285,   294,   303,   312,
     314,   317,   325,   334,   334,   336,   352,   356,   358,   360,
     361,   363,   365,   367,   369,   371,   374,   374,   374,   375,
     375,   375,   375,   375,   376,   379,   403,   409,   416,   424,
     434,   446,   461,   462,   466,   474,   475,   479,   483,   489,
     491,   498,   503,   508,   515,   519,   527,   537,   544,   553,
     565,   573,   574,   579,   580,   582,   587,   588,   592,   596,
     601,   601,   604,   606,   610,   615,   619,   621,   625,   626,
     632,   641,   644,   652,   660,   669,   678,   687,   700,   701,
     705,   707,   709,   711,   713,   715,   717,   722,   728,   731,
     733,   737,   739,   741,   750,   761,   764,   766,   772,   773,
     775,   777,   779,   781,   790,   799,   801,   803,   805,   807,
     809,   811,   813,   815,   817,   822,   827,   832,   837,   843,
     845,   858,   859,   861,   863,   865,   867,   869,   871,   873,
     875,   877,   879,   881,   883,   885,   887,   889,   891,   893,
     895,   897,   906,   915,   917,   919,   921,   923,   925,   927,
     929,   931,   933,   938,   943,   948,   953,   959,   961,   968,
     980,   982,   986,   988,   990,   992,   995,   997,  1000,  1002,
    1004,  1006,  1008,  1010,  1011,  1014,  1015,  1018,  1019,  1022,
    1023,  1026,  1027,  1030,  1031,  1034,  1035,  1036,  1041,  1043,
    1049,  1054,  1062,  1069,  1078,  1080,  1085,  1091,  1094,  1097,
    1099,  1101,  1107,  1109,  1111,  1119,  1121,  1124,  1127,  1130,
    1132,  1136,  1138,  1142,  1144,  1155,  1166,  1206,  1209,  1214,
    1221,  1226,  1230,  1236,  1243,  1259,  1260,  1264,  1266,  1268,
    1270,  1272,  1274,  1276,  1278,  1280,  1282,  1284,  1286,  1288,
    1290,  1292,  1294,  1296,  1298,  1300,  1302,  1304,  1306,  1308,
    1310,  1312,  1314,  1316,  1318,  1322,  1330,  1362,  1364,  1365,
    1385,  1440,  1443,  1446,  1449,  1451,  1455,  1462,  1471,  1473,
    1481,  1483,  1492,  1492,  1495,  1501,  1512,  1513,  1516,  1518,
    1522,  1526,  1530,  1532,  1534,  1536,  1538,  1540,  1542,  1544,
    1546,  1548,  1550,  1552,  1554,  1556,  1558,  1560,  1562,  1564,
    1566,  1568,  1570,  1572,  1574,  1576,  1578,  1580,  1582,  1584,
    1586,  1588
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "$undefined", "\"integer literal\"",
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
  "ne_annotations", "id_or_quoted_op", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
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
# endif

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

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
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

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
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

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -402
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

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -89
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

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (&yylloc, parm, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, &yylloc, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, &yylloc, SCANNER)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location, parm); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void *parm)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, parm)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    void *parm;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
  YYUSE (parm);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void *parm)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp, parm)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    void *parm;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, parm);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, void *parm)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule, parm)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
    void *parm;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       , parm);
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule, parm); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
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



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
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
	    /* Fall through.  */
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

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, void *parm)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp, parm)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
    void *parm;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (parm);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void *parm);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */






/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *parm)
#else
int
yyparse (parm)
    void *parm;
#endif
#endif
{
  /* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;
/* Location data for the look-ahead symbol.  */
YYLTYPE yylloc;

  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;

  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[2];

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;
#if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 0;
#endif


  /* User initialization code.  */

{
  GCLock lock;
  yylloc.filename(ASTString(static_cast<ParserState*>(parm)->filename));
}
/* Line 1078 of yacc.c.  */

  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
	YYSTACK_RELOCATE (yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 5:

    {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[(1) - (1)].item)) {
          pp->model->addItem((yyvsp[(1) - (1)].item));
          GC::unlock();
          GC::lock();
        }
      ;}
    break;

  case 6:

    {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[(2) - (2)].item)) {
          pp->model->addItem((yyvsp[(2) - (2)].item));
          GC::unlock();
          GC::lock();
        }
      ;}
    break;

  case 7:

    {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[(3) - (3)].item)) {
          pp->model->addItem((yyvsp[(3) - (3)].item));
          GC::unlock();
          GC::lock();
        }
      ;}
    break;

  case 8:

    {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[(4) - (4)].item)) {
          pp->model->addItem((yyvsp[(4) - (4)].item));
          GC::unlock();
          GC::lock();
        }
      ;}
    break;

  case 9:

    { yyerror(&(yylsp[(2) - (2)]), parm, "unexpected item, expecting ';' or end of file"); YYERROR; ;}
    break;

  case 11:

    {
        ParserState* pp = static_cast<ParserState*>(parm);
        if (pp->parseDocComments && (yyvsp[(1) - (1)].sValue)) {
          pp->model->addDocComment((yyvsp[(1) - (1)].sValue));
        }
        free((yyvsp[(1) - (1)].sValue));
      ;}
    break;

  case 12:

    {
        ParserState* pp = static_cast<ParserState*>(parm);
        if (pp->parseDocComments && (yyvsp[(2) - (2)].sValue)) {
          pp->model->addDocComment((yyvsp[(2) - (2)].sValue));
        }
        free((yyvsp[(2) - (2)].sValue));
      ;}
    break;

  case 15:

    { (yyval.item) = (yyvsp[(2) - (2)].item);
        ParserState* pp = static_cast<ParserState*>(parm);
        if (FunctionI* fi = Item::dyn_cast<FunctionI>((yyval.item))) {
          if (pp->parseDocComments) {
            fi->ann().add(createDocComment((yylsp[(1) - (2)]),(yyvsp[(1) - (2)].sValue)));
          }
        } else if (VarDeclI* vdi = Item::dyn_cast<VarDeclI>((yyval.item))) {
          if (pp->parseDocComments) {
            vdi->e()->addAnnotation(createDocComment((yylsp[(1) - (2)]),(yyvsp[(1) - (2)].sValue)));
          }
        } else {
          yyerror(&(yylsp[(2) - (2)]), parm, "documentation comments are only supported for function, predicate and variable declarations");
        }
        free((yyvsp[(1) - (2)].sValue));
      ;}
    break;

  case 16:

    { (yyval.item) = (yyvsp[(1) - (1)].item); ;}
    break;

  case 17:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"include") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 18:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"variable declaration") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 20:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"constraint") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 21:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"solve") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 22:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"output") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 23:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"predicate") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 24:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"predicate") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 25:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"annotation") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 35:

    { ParserState* pp = static_cast<ParserState*>(parm);
        map<string,Model*>::iterator ret = pp->seenModels.find((yyvsp[(2) - (2)].sValue));
        IncludeI* ii = new IncludeI((yyloc),ASTString((yyvsp[(2) - (2)].sValue)));
        (yyval.item) = ii;
        if (ret == pp->seenModels.end()) {
          Model* im = new Model;
          im->setParent(pp->model);
          im->setFilename((yyvsp[(2) - (2)].sValue));
          string fpath = FileUtils::dir_name(pp->filename);
          string fbase = FileUtils::base_name(pp->filename);
          if (fpath=="")
            fpath="./";
          ParseWorkItem pm(im, fpath, (yyvsp[(2) - (2)].sValue));
          pp->files.push_back(pm);
          ii->m(im);
          pp->seenModels.insert(pair<string,Model*>((yyvsp[(2) - (2)].sValue),im));
        } else {
          ii->m(ret->second, false);
        }
        free((yyvsp[(2) - (2)].sValue));
      ;}
    break;

  case 36:

    { if ((yyvsp[(1) - (2)].vardeclexpr) && (yyvsp[(2) - (2)].expression_v)) (yyvsp[(1) - (2)].vardeclexpr)->addAnnotations(*(yyvsp[(2) - (2)].expression_v));
        if ((yyvsp[(1) - (2)].vardeclexpr))
          (yyval.item) = new VarDeclI((yyloc),(yyvsp[(1) - (2)].vardeclexpr));
        delete (yyvsp[(2) - (2)].expression_v);
      ;}
    break;

  case 37:

    { if ((yyvsp[(1) - (4)].vardeclexpr)) (yyvsp[(1) - (4)].vardeclexpr)->e((yyvsp[(4) - (4)].expression));
        if ((yyvsp[(1) - (4)].vardeclexpr) && (yyvsp[(2) - (4)].expression_v)) (yyvsp[(1) - (4)].vardeclexpr)->addAnnotations(*(yyvsp[(2) - (4)].expression_v));
        if ((yyvsp[(1) - (4)].vardeclexpr))
          (yyval.item) = new VarDeclI((yyloc),(yyvsp[(1) - (4)].vardeclexpr));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 38:

    {
        TypeInst* ti = new TypeInst((yyloc),Type::parsetint());
        ti->setIsEnum(true);
        VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[(2) - (2)].sValue));
        free((yyvsp[(2) - (2)].sValue));
        (yyval.item) = new VarDeclI((yyloc),vd);
      ;}
    break;

  case 39:

    {
        TypeInst* ti = new TypeInst((yyloc),Type::parsetint());
        ti->setIsEnum(true);
        SetLit* sl = new SetLit((yyloc), *(yyvsp[(5) - (6)].expression_v));
        VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[(2) - (6)].sValue),sl);
        free((yyvsp[(2) - (6)].sValue));
        delete (yyvsp[(5) - (6)].expression_v);
        (yyval.item) = new VarDeclI((yyloc),vd);
      ;}
    break;

  case 40:

    {
        TypeInst* ti = new TypeInst((yyloc),Type::parsetint());
        ti->setIsEnum(true);
        vector<Expression*> args;
        args.push_back(new ArrayLit((yyloc),*(yyvsp[(5) - (6)].expression_v)));
        Call* sl = new Call((yyloc), constants().ids.anonEnumFromStrings, args);
        VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[(2) - (6)].sValue),sl);
        free((yyvsp[(2) - (6)].sValue));
        delete (yyvsp[(5) - (6)].expression_v);
        (yyval.item) = new VarDeclI((yyloc),vd);
      ;}
    break;

  case 41:

    {
        TypeInst* ti = new TypeInst((yyloc),Type::parsetint());
        ti->setIsEnum(true);
        vector<Expression*> args;
        args.push_back((yyvsp[(6) - (7)].expression));
        Call* sl = new Call((yyloc), ASTString((yyvsp[(4) - (7)].sValue)), args);
        VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[(2) - (7)].sValue),sl);
        free((yyvsp[(2) - (7)].sValue));
        free((yyvsp[(4) - (7)].sValue));
        (yyval.item) = new VarDeclI((yyloc),vd);
      ;}
    break;

  case 42:

    { (yyval.expression_v) = new std::vector<Expression*>(); ;}
    break;

  case 43:

    { (yyval.expression_v) = new std::vector<Expression*>();
        (yyval.expression_v)->push_back(new StringLit((yyloc), (yyvsp[(1) - (1)].sValue))); free((yyvsp[(1) - (1)].sValue));
      ;}
    break;

  case 44:

    { (yyval.expression_v) = (yyvsp[(1) - (3)].expression_v);
        if ((yyval.expression_v)) (yyval.expression_v)->push_back(new StringLit((yyloc), (yyvsp[(3) - (3)].sValue)));
        free((yyvsp[(3) - (3)].sValue));
      ;}
    break;

  case 45:

    { (yyval.expression_v) = new std::vector<Expression*>(); ;}
    break;

  case 46:

    { (yyval.expression_v) = new std::vector<Expression*>();
        (yyval.expression_v)->push_back(new Id((yyloc),(yyvsp[(1) - (1)].sValue),NULL)); free((yyvsp[(1) - (1)].sValue));
      ;}
    break;

  case 47:

    { (yyval.expression_v) = (yyvsp[(1) - (3)].expression_v); if ((yyval.expression_v)) (yyval.expression_v)->push_back(new Id((yyloc),(yyvsp[(3) - (3)].sValue),NULL)); free((yyvsp[(3) - (3)].sValue)); ;}
    break;

  case 48:

    { (yyval.item) = new AssignI((yyloc),(yyvsp[(1) - (3)].sValue),(yyvsp[(3) - (3)].expression));
        free((yyvsp[(1) - (3)].sValue));
      ;}
    break;

  case 49:

    { (yyval.item) = new ConstraintI((yyloc),(yyvsp[(2) - (2)].expression));;}
    break;

  case 50:

    { (yyval.item) = new ConstraintI((yyloc),(yyvsp[(4) - (4)].expression));
        if ((yyval.item) && (yyvsp[(3) - (4)].expression))
          (yyval.item)->cast<ConstraintI>()->e()->ann().add(new Call((yylsp[(2) - (4)]), ASTString("mzn_constraint_name"), {(yyvsp[(3) - (4)].expression)}));
      ;}
    break;

  case 51:

    { (yyval.item) = SolveI::sat((yyloc));
        if ((yyval.item) && (yyvsp[(2) - (3)].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[(2) - (3)].expression_v));
        delete (yyvsp[(2) - (3)].expression_v);
      ;}
    break;

  case 52:

    { (yyval.item) = SolveI::min((yyloc),(yyvsp[(4) - (4)].expression));
        if ((yyval.item) && (yyvsp[(2) - (4)].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[(2) - (4)].expression_v));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 53:

    { (yyval.item) = SolveI::max((yyloc),(yyvsp[(4) - (4)].expression));
        if ((yyval.item) && (yyvsp[(2) - (4)].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[(2) - (4)].expression_v));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 54:

    { (yyval.item) = new OutputI((yyloc),(yyvsp[(2) - (2)].expression));;}
    break;

  case 55:

    { if ((yyvsp[(3) - (5)].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[(2) - (5)].sValue),new TypeInst((yyloc),
                                   Type::varbool()),*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression));
        if ((yyval.item) && (yyvsp[(4) - (5)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(4) - (5)].expression_v));
        free((yyvsp[(2) - (5)].sValue));
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
        delete (yyvsp[(4) - (5)].expression_v);
      ;}
    break;

  case 56:

    { if ((yyvsp[(3) - (5)].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[(2) - (5)].sValue),new TypeInst((yyloc),
                                   Type::parbool()),*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression));
        if ((yyval.item) && (yyvsp[(4) - (5)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(4) - (5)].expression_v));
        free((yyvsp[(2) - (5)].sValue));
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
        delete (yyvsp[(4) - (5)].expression_v);
      ;}
    break;

  case 57:

    { if ((yyvsp[(5) - (7)].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[(4) - (7)].sValue),(yyvsp[(2) - (7)].tiexpr),*(yyvsp[(5) - (7)].vardeclexpr_v),(yyvsp[(7) - (7)].expression));
        if ((yyval.item) && (yyvsp[(6) - (7)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(6) - (7)].expression_v));
        free((yyvsp[(4) - (7)].sValue));
        delete (yyvsp[(5) - (7)].vardeclexpr_v);
        delete (yyvsp[(6) - (7)].expression_v);
      ;}
    break;

  case 58:

    { if ((yyvsp[(5) - (8)].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[(3) - (8)].sValue),(yyvsp[(1) - (8)].tiexpr),*(yyvsp[(5) - (8)].vardeclexpr_v),(yyvsp[(8) - (8)].expression));
        if ((yyval.item) && (yyvsp[(7) - (8)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(7) - (8)].expression_v));
        free((yyvsp[(3) - (8)].sValue));
        delete (yyvsp[(5) - (8)].vardeclexpr_v);
        delete (yyvsp[(7) - (8)].expression_v);
      ;}
    break;

  case 59:

    {
        TypeInst* ti=new TypeInst((yylsp[(1) - (3)]),Type::ann());
        if ((yyvsp[(3) - (3)].vardeclexpr_v)==NULL || (yyvsp[(3) - (3)].vardeclexpr_v)->empty()) {
          VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[(2) - (3)].sValue));
          (yyval.item) = new VarDeclI((yyloc),vd);
        } else {
          (yyval.item) = new FunctionI((yyloc),(yyvsp[(2) - (3)].sValue),ti,*(yyvsp[(3) - (3)].vardeclexpr_v),NULL);
        }
        free((yyvsp[(2) - (3)].sValue));
        delete (yyvsp[(3) - (3)].vardeclexpr_v);
      ;}
    break;

  case 60:

    { TypeInst* ti=new TypeInst((yylsp[(1) - (5)]),Type::ann());
        if ((yyvsp[(3) - (5)].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[(2) - (5)].sValue),ti,*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression));
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
      ;}
    break;

  case 61:

    { (yyval.expression)=NULL; ;}
    break;

  case 62:

    { (yyval.expression)=(yyvsp[(2) - (2)].expression); ;}
    break;

  case 63:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); ;}
    break;

  case 64:

    { (yyval.vardeclexpr_v)=(yyvsp[(2) - (3)].vardeclexpr_v); ;}
    break;

  case 65:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); ;}
    break;

  case 66:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); ;}
    break;

  case 67:

    { (yyval.vardeclexpr_v)=(yyvsp[(1) - (2)].vardeclexpr_v); ;}
    break;

  case 68:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>();
        if ((yyvsp[(1) - (1)].vardeclexpr)) (yyvsp[(1) - (1)].vardeclexpr)->toplevel(false);
        if ((yyvsp[(1) - (1)].vardeclexpr)) (yyval.vardeclexpr_v)->push_back((yyvsp[(1) - (1)].vardeclexpr)); ;}
    break;

  case 69:

    { (yyval.vardeclexpr_v)=(yyvsp[(1) - (3)].vardeclexpr_v);
        if ((yyvsp[(3) - (3)].vardeclexpr)) (yyvsp[(3) - (3)].vardeclexpr)->toplevel(false);
        if ((yyvsp[(1) - (3)].vardeclexpr_v) && (yyvsp[(3) - (3)].vardeclexpr)) (yyvsp[(1) - (3)].vardeclexpr_v)->push_back((yyvsp[(3) - (3)].vardeclexpr)); ;}
    break;

  case 72:

    { (yyval.vardeclexpr)=(yyvsp[(1) - (1)].vardeclexpr); ;}
    break;

  case 73:

    { if ((yyvsp[(1) - (1)].tiexpr)) (yyval.vardeclexpr)=new VarDecl((yyloc), (yyvsp[(1) - (1)].tiexpr), ""); ;}
    break;

  case 74:

    { if ((yyvsp[(1) - (3)].tiexpr) && (yyvsp[(3) - (3)].sValue)) (yyval.vardeclexpr) = new VarDecl((yyloc), (yyvsp[(1) - (3)].tiexpr), (yyvsp[(3) - (3)].sValue));
        free((yyvsp[(3) - (3)].sValue));
      ;}
    break;

  case 75:

    { (yyval.tiexpr_v)=(yyvsp[(1) - (2)].tiexpr_v); ;}
    break;

  case 76:

    { (yyval.tiexpr_v)=new vector<TypeInst*>(); (yyval.tiexpr_v)->push_back((yyvsp[(1) - (1)].tiexpr)); ;}
    break;

  case 77:

    { (yyval.tiexpr_v)=(yyvsp[(1) - (3)].tiexpr_v); if ((yyvsp[(1) - (3)].tiexpr_v) && (yyvsp[(3) - (3)].tiexpr)) (yyvsp[(1) - (3)].tiexpr_v)->push_back((yyvsp[(3) - (3)].tiexpr)); ;}
    break;

  case 79:

    {
        (yyval.tiexpr) = (yyvsp[(6) - (6)].tiexpr);
        if ((yyval.tiexpr) && (yyvsp[(3) - (6)].tiexpr_v)) (yyval.tiexpr)->setRanges(*(yyvsp[(3) - (6)].tiexpr_v));
        delete (yyvsp[(3) - (6)].tiexpr_v);
      ;}
    break;

  case 80:

    {
        (yyval.tiexpr) = (yyvsp[(3) - (3)].tiexpr);
        std::vector<TypeInst*> ti(1);
        ti[0] = new TypeInst((yyloc),Type::parint());
        if ((yyval.tiexpr)) (yyval.tiexpr)->setRanges(ti);
      ;}
    break;

  case 81:

    { (yyval.tiexpr) = (yyvsp[(1) - (1)].tiexpr);
      ;}
    break;

  case 82:

    { (yyval.tiexpr) = (yyvsp[(2) - (2)].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 83:

    { (yyval.tiexpr) = (yyvsp[(3) - (3)].tiexpr);
        if ((yyval.tiexpr) && (yyvsp[(2) - (3)].bValue)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 84:

    { (yyval.tiexpr) = (yyvsp[(3) - (3)].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ti(Type::TI_VAR);
          if ((yyvsp[(2) - (3)].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 85:

    { (yyval.tiexpr) = (yyvsp[(4) - (4)].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.st(Type::ST_SET);
          if ((yyvsp[(1) - (4)].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 86:

    { (yyval.tiexpr) = (yyvsp[(5) - (5)].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.st(Type::ST_SET);
          if ((yyvsp[(2) - (5)].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 87:

    { (yyval.tiexpr) = (yyvsp[(5) - (5)].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ti(Type::TI_VAR);
          tt.st(Type::ST_SET);
          if ((yyvsp[(2) - (5)].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 88:

    { (yyval.bValue) = false; ;}
    break;

  case 89:

    { (yyval.bValue) = true; ;}
    break;

  case 90:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parint()); ;}
    break;

  case 91:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parbool()); ;}
    break;

  case 92:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parfloat()); ;}
    break;

  case 93:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parstring()); ;}
    break;

  case 94:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::ann()); ;}
    break;

  case 95:

    { if ((yyvsp[(1) - (1)].expression)) (yyval.tiexpr) = new TypeInst((yyloc),Type(),(yyvsp[(1) - (1)].expression)); ;}
    break;

  case 96:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::top(),
                         new TIId((yyloc), (yyvsp[(1) - (1)].sValue)));
        free((yyvsp[(1) - (1)].sValue));
      ;}
    break;

  case 97:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parint(),
          new TIId((yyloc), (yyvsp[(1) - (1)].sValue)));
          free((yyvsp[(1) - (1)].sValue));
      ;}
    break;

  case 99:

    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].expression)); ;}
    break;

  case 100:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); if ((yyval.expression_v) && (yyvsp[(3) - (3)].expression)) (yyval.expression_v)->push_back((yyvsp[(3) - (3)].expression)); ;}
    break;

  case 101:

    { (yyval.expression) = (yyvsp[(1) - (1)].expression); ;}
    break;

  case 102:

    { (yyval.expression)=new SetLit((yyloc), IntSetVal::a(-IntVal::infinity(),IntVal::infinity())); ;}
    break;

  case 103:

    { if ((yyvsp[(2) - (2)].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[(2) - (2)].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a(-IntVal::infinity(),(yyvsp[(2) - (2)].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), IntLit::a(-IntVal::infinity()), BOT_DOTDOT, (yyvsp[(2) - (2)].expression));
        }
      ;}
    break;

  case 104:

    { if ((yyvsp[(1) - (2)].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[(1) - (2)].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[(1) - (2)].expression)->cast<IntLit>()->v(),IntVal::infinity()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (2)].expression), BOT_DOTDOT, IntLit::a(IntVal::infinity()));
        }
      ;}
    break;

  case 106:

    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].expression)); ;}
    break;

  case 107:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); if ((yyval.expression_v) && (yyvsp[(3) - (3)].expression)) (yyval.expression_v)->push_back((yyvsp[(3) - (3)].expression)); ;}
    break;

  case 109:

    { if ((yyvsp[(1) - (3)].expression) && (yyvsp[(3) - (3)].expression)) (yyvsp[(1) - (3)].expression)->addAnnotation((yyvsp[(3) - (3)].expression)); (yyval.expression)=(yyvsp[(1) - (3)].expression); ;}
    break;

  case 110:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_UNION, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 111:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 112:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SYMDIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 113:

    { if ((yyvsp[(1) - (3)].expression)==NULL || (yyvsp[(3) - (3)].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[(1) - (3)].expression)->isa<IntLit>() && (yyvsp[(3) - (3)].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[(1) - (3)].expression)->cast<IntLit>()->v(),(yyvsp[(3) - (3)].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DOTDOT, (yyvsp[(3) - (3)].expression));
        }
      ;}
    break;

  case 114:

    { if ((yyvsp[(3) - (6)].expression)==NULL || (yyvsp[(5) - (6)].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[(3) - (6)].expression)->isa<IntLit>() && (yyvsp[(5) - (6)].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[(3) - (6)].expression)->cast<IntLit>()->v(),(yyvsp[(5) - (6)].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[(3) - (6)].expression), BOT_DOTDOT, (yyvsp[(5) - (6)].expression));
        }
      ;}
    break;

  case 115:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_INTERSECT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 116:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUSPLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 117:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 118:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MINUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 119:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MULT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 120:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 121:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IDIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 122:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MOD, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 123:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_POW, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 124:

    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~+"), args);
      ;}
    break;

  case 125:

    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~-"), args);
      ;}
    break;

  case 126:

    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~*"), args);
      ;}
    break;

  case 127:

    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~="), args);
      ;}
    break;

  case 128:

    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=new Call((yyloc), (yyvsp[(2) - (3)].sValue), args);
        free((yyvsp[(2) - (3)].sValue));
      ;}
    break;

  case 129:

    { (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 130:

    { if ((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<IntLit>()) {
          (yyval.expression) = IntLit::a(-(yyvsp[(2) - (2)].expression)->cast<IntLit>()->v());
        } else if ((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<FloatLit>()) {
          (yyval.expression) = FloatLit::a(-(yyvsp[(2) - (2)].expression)->cast<FloatLit>()->v());
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_MINUS, (yyvsp[(2) - (2)].expression));
        }
      ;}
    break;

  case 132:

    { if ((yyvsp[(1) - (3)].expression) && (yyvsp[(3) - (3)].expression)) (yyvsp[(1) - (3)].expression)->addAnnotation((yyvsp[(3) - (3)].expression)); (yyval.expression)=(yyvsp[(1) - (3)].expression); ;}
    break;

  case 133:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_EQUIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 134:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IMPL, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 135:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_RIMPL, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 136:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_OR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 137:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_XOR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 138:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_AND, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 139:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_LE, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 140:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_GR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 141:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_LQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 142:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_GQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 143:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_EQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 144:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_NQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 145:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IN, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 146:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SUBSET, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 147:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SUPERSET, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 148:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_UNION, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 149:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 150:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SYMDIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 151:

    { if ((yyvsp[(1) - (3)].expression)==NULL || (yyvsp[(3) - (3)].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[(1) - (3)].expression)->isa<IntLit>() && (yyvsp[(3) - (3)].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[(1) - (3)].expression)->cast<IntLit>()->v(),(yyvsp[(3) - (3)].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DOTDOT, (yyvsp[(3) - (3)].expression));
        }
      ;}
    break;

  case 152:

    { if ((yyvsp[(3) - (6)].expression)==NULL || (yyvsp[(5) - (6)].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[(3) - (6)].expression)->isa<IntLit>() && (yyvsp[(5) - (6)].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[(3) - (6)].expression)->cast<IntLit>()->v(),(yyvsp[(5) - (6)].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[(3) - (6)].expression), BOT_DOTDOT, (yyvsp[(5) - (6)].expression));
        }
      ;}
    break;

  case 153:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_INTERSECT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 154:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUSPLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 155:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 156:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MINUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 157:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MULT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 158:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 159:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IDIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 160:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MOD, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 161:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_POW, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 162:

    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~+"), args);
      ;}
    break;

  case 163:

    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~-"), args);
      ;}
    break;

  case 164:

    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~*"), args);
      ;}
    break;

  case 165:

    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=new Call((yyloc), ASTString("~="), args);
      ;}
    break;

  case 166:

    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=new Call((yyloc), (yyvsp[(2) - (3)].sValue), args);
        free((yyvsp[(2) - (3)].sValue));
      ;}
    break;

  case 167:

    { (yyval.expression)=new UnOp((yyloc), UOT_NOT, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 168:

    { if (((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<IntLit>()) || ((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<FloatLit>())) {
          (yyval.expression) = (yyvsp[(2) - (2)].expression);
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[(2) - (2)].expression));
        }
      ;}
    break;

  case 169:

    { if ((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<IntLit>()) {
          (yyval.expression) = IntLit::a(-(yyvsp[(2) - (2)].expression)->cast<IntLit>()->v());
        } else if ((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<FloatLit>()) {
          (yyval.expression) = FloatLit::a(-(yyvsp[(2) - (2)].expression)->cast<FloatLit>()->v());
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_MINUS, (yyvsp[(2) - (2)].expression));
        }
      ;}
    break;

  case 170:

    { (yyval.expression)=(yyvsp[(1) - (1)].expression); ;}
    break;

  case 171:

    { (yyval.expression)=(yyvsp[(1) - (1)].expression); ;}
    break;

  case 172:

    { (yyval.expression)=(yyvsp[(2) - (3)].expression); ;}
    break;

  case 173:

    { if ((yyvsp[(4) - (4)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(2) - (4)].expression), *(yyvsp[(4) - (4)].expression_vv)); delete (yyvsp[(4) - (4)].expression_vv); ;}
    break;

  case 174:

    { (yyval.expression)=new Id((yyloc), (yyvsp[(1) - (1)].sValue), NULL); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 175:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), new Id((yylsp[(1) - (2)]),(yyvsp[(1) - (2)].sValue),NULL), *(yyvsp[(2) - (2)].expression_vv));
        free((yyvsp[(1) - (2)].sValue)); delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 176:

    { (yyval.expression)=new AnonVar((yyloc)); ;}
    break;

  case 177:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), new AnonVar((yyloc)), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 178:

    { (yyval.expression)=constants().boollit(((yyvsp[(1) - (1)].iValue)!=0)); ;}
    break;

  case 179:

    { (yyval.expression)=IntLit::a((yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 180:

    { (yyval.expression)=IntLit::a(IntVal::infinity()); ;}
    break;

  case 181:

    { (yyval.expression)=FloatLit::a((yyvsp[(1) - (1)].dValue)); ;}
    break;

  case 182:

    { (yyval.expression)=constants().absent; ;}
    break;

  case 184:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 186:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 188:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 190:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 192:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 194:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 197:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 198:

    { (yyval.expression)=new StringLit((yyloc), (yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 199:

    { (yyval.expression)=new BinOp((yyloc), new StringLit((yyloc), (yyvsp[(1) - (2)].sValue)), BOT_PLUSPLUS, (yyvsp[(2) - (2)].expression));
        free((yyvsp[(1) - (2)].sValue));
      ;}
    break;

  case 200:

    { if ((yyvsp[(1) - (2)].expression_v)) (yyval.expression)=new BinOp((yyloc), new Call((yyloc), ASTString("format"), *(yyvsp[(1) - (2)].expression_v)), BOT_PLUSPLUS, new StringLit((yyloc),(yyvsp[(2) - (2)].sValue)));
        free((yyvsp[(2) - (2)].sValue));
        delete (yyvsp[(1) - (2)].expression_v);
      ;}
    break;

  case 201:

    { if ((yyvsp[(1) - (3)].expression_v)) (yyval.expression)=new BinOp((yyloc), new Call((yyloc), ASTString("format"), *(yyvsp[(1) - (3)].expression_v)), BOT_PLUSPLUS,
                             new BinOp((yyloc), new StringLit((yyloc),(yyvsp[(2) - (3)].sValue)), BOT_PLUSPLUS, (yyvsp[(3) - (3)].expression)));
        free((yyvsp[(2) - (3)].sValue));
        delete (yyvsp[(1) - (3)].expression_v);
      ;}
    break;

  case 202:

    { (yyval.expression_vv)=new std::vector<std::vector<Expression*> >();
        if ((yyvsp[(2) - (3)].expression_v)) {
          (yyval.expression_vv)->push_back(*(yyvsp[(2) - (3)].expression_v));
          delete (yyvsp[(2) - (3)].expression_v);
        }
      ;}
    break;

  case 203:

    { (yyval.expression_vv)=(yyvsp[(1) - (4)].expression_vv);
        if ((yyval.expression_vv) && (yyvsp[(3) - (4)].expression_v)) {
          (yyval.expression_vv)->push_back(*(yyvsp[(3) - (4)].expression_v));
          delete (yyvsp[(3) - (4)].expression_v);
        }
      ;}
    break;

  case 204:

    { (yyval.expression) = new SetLit((yyloc), std::vector<Expression*>()); ;}
    break;

  case 205:

    { if ((yyvsp[(2) - (3)].expression_v)) (yyval.expression) = new SetLit((yyloc), *(yyvsp[(2) - (3)].expression_v));
        delete (yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 206:

    { if ((yyvsp[(4) - (5)].generators)) (yyval.expression) = new Comprehension((yyloc), (yyvsp[(2) - (5)].expression), *(yyvsp[(4) - (5)].generators), true);
        delete (yyvsp[(4) - (5)].generators);
      ;}
    break;

  case 207:

    { if ((yyvsp[(1) - (1)].generator_v)) (yyval.generators)=new Generators; (yyval.generators)->_g = *(yyvsp[(1) - (1)].generator_v); delete (yyvsp[(1) - (1)].generator_v); ;}
    break;

  case 209:

    { (yyval.generator_v)=new std::vector<Generator>; if ((yyvsp[(1) - (1)].generator)) (yyval.generator_v)->push_back(*(yyvsp[(1) - (1)].generator)); delete (yyvsp[(1) - (1)].generator); ;}
    break;

  case 210:

    { (yyval.generator_v)=new std::vector<Generator>; if ((yyvsp[(1) - (1)].generator)) (yyval.generator_v)->push_back(*(yyvsp[(1) - (1)].generator)); delete (yyvsp[(1) - (1)].generator); ;}
    break;

  case 211:

    { (yyval.generator_v)=new std::vector<Generator>;
        if ((yyvsp[(1) - (3)].generator)) (yyval.generator_v)->push_back(*(yyvsp[(1) - (3)].generator));
        if ((yyvsp[(1) - (3)].generator) && (yyvsp[(3) - (3)].expression)) (yyval.generator_v)->push_back(Generator((yyval.generator_v)->size(),(yyvsp[(3) - (3)].expression)));
        delete (yyvsp[(1) - (3)].generator);
      ;}
    break;

  case 212:

    { (yyval.generator_v)=(yyvsp[(1) - (3)].generator_v); if ((yyval.generator_v) && (yyvsp[(3) - (3)].generator)) (yyval.generator_v)->push_back(*(yyvsp[(3) - (3)].generator)); delete (yyvsp[(3) - (3)].generator); ;}
    break;

  case 213:

    { (yyval.generator_v)=(yyvsp[(1) - (3)].generator_v); if ((yyval.generator_v) && (yyvsp[(3) - (3)].generator)) (yyval.generator_v)->push_back(*(yyvsp[(3) - (3)].generator)); delete (yyvsp[(3) - (3)].generator); ;}
    break;

  case 214:

    { (yyval.generator_v)=(yyvsp[(1) - (5)].generator_v);
        if ((yyval.generator_v) && (yyvsp[(3) - (5)].generator)) (yyval.generator_v)->push_back(*(yyvsp[(3) - (5)].generator));
        if ((yyval.generator_v) && (yyvsp[(3) - (5)].generator) && (yyvsp[(5) - (5)].expression)) (yyval.generator_v)->push_back(Generator((yyval.generator_v)->size(),(yyvsp[(5) - (5)].expression)));
        delete (yyvsp[(3) - (5)].generator);
      ;}
    break;

  case 215:

    { if ((yyvsp[(1) - (3)].string_v) && (yyvsp[(3) - (3)].expression)) (yyval.generator)=new Generator(*(yyvsp[(1) - (3)].string_v),(yyvsp[(3) - (3)].expression),NULL); else (yyval.generator)=NULL; delete (yyvsp[(1) - (3)].string_v); ;}
    break;

  case 216:

    { if ((yyvsp[(1) - (5)].string_v) && (yyvsp[(3) - (5)].expression)) (yyval.generator)=new Generator(*(yyvsp[(1) - (5)].string_v),(yyvsp[(3) - (5)].expression),(yyvsp[(5) - (5)].expression)); else (yyval.generator)=NULL; delete (yyvsp[(1) - (5)].string_v); ;}
    break;

  case 217:

    { if ((yyvsp[(3) - (3)].expression)) (yyval.generator)=new Generator({(yyvsp[(1) - (3)].sValue)},NULL,(yyvsp[(3) - (3)].expression)); else (yyval.generator)=NULL; free((yyvsp[(1) - (3)].sValue)); ;}
    break;

  case 219:

    { (yyval.string_v)=new std::vector<std::string>; (yyval.string_v)->push_back((yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 220:

    { (yyval.string_v)=(yyvsp[(1) - (3)].string_v); if ((yyval.string_v) && (yyvsp[(3) - (3)].sValue)) (yyval.string_v)->push_back((yyvsp[(3) - (3)].sValue)); free((yyvsp[(3) - (3)].sValue)); ;}
    break;

  case 221:

    { (yyval.expression)=new ArrayLit((yyloc), std::vector<MiniZinc::Expression*>()); ;}
    break;

  case 222:

    { if ((yyvsp[(2) - (3)].expression_v)) (yyval.expression)=new ArrayLit((yyloc), *(yyvsp[(2) - (3)].expression_v)); delete (yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 223:

    { (yyval.expression)=new ArrayLit((yyloc), std::vector<std::vector<Expression*> >()); ;}
    break;

  case 224:

    { if ((yyvsp[(2) - (3)].expression_vv)) {
          (yyval.expression)=new ArrayLit((yyloc), *(yyvsp[(2) - (3)].expression_vv));
          for (unsigned int i=1; i<(yyvsp[(2) - (3)].expression_vv)->size(); i++)
            if ((*(yyvsp[(2) - (3)].expression_vv))[i].size() != (*(yyvsp[(2) - (3)].expression_vv))[i-1].size())
              yyerror(&(yylsp[(2) - (3)]), parm, "syntax error, all sub-arrays of 2d array literal must have the same length");
          delete (yyvsp[(2) - (3)].expression_vv);
        } else {
          (yyval.expression) = NULL;
        }
      ;}
    break;

  case 225:

    { if ((yyvsp[(2) - (4)].expression_vv)) {
          (yyval.expression)=new ArrayLit((yyloc), *(yyvsp[(2) - (4)].expression_vv));
          for (unsigned int i=1; i<(yyvsp[(2) - (4)].expression_vv)->size(); i++)
            if ((*(yyvsp[(2) - (4)].expression_vv))[i].size() != (*(yyvsp[(2) - (4)].expression_vv))[i-1].size())
              yyerror(&(yylsp[(2) - (4)]), parm, "syntax error, all sub-arrays of 2d array literal must have the same length");
          delete (yyvsp[(2) - (4)].expression_vv);
        } else {
          (yyval.expression) = NULL;
        }
      ;}
    break;

  case 226:

    {
      if ((yyvsp[(2) - (3)].expression_vvv)) {
        std::vector<std::pair<int,int> > dims(3);
        dims[0] = std::pair<int,int>(1,static_cast<int>((yyvsp[(2) - (3)].expression_vvv)->size()));
        if ((yyvsp[(2) - (3)].expression_vvv)->size()==0) {
          dims[1] = std::pair<int,int>(1,0);
          dims[2] = std::pair<int,int>(1,0);
        } else {
          dims[1] = std::pair<int,int>(1,static_cast<int>((*(yyvsp[(2) - (3)].expression_vvv))[0].size()));
          if ((*(yyvsp[(2) - (3)].expression_vvv))[0].size()==0) {
            dims[2] = std::pair<int,int>(1,0);
          } else {
            dims[2] = std::pair<int,int>(1,static_cast<int>((*(yyvsp[(2) - (3)].expression_vvv))[0][0].size()));
          }
        }
        std::vector<Expression*> a;
        for (int i=0; i<dims[0].second; i++) {
          if ((*(yyvsp[(2) - (3)].expression_vvv))[i].size() != dims[1].second) {
            yyerror(&(yylsp[(2) - (3)]), parm, "syntax error, all sub-arrays of 3d array literal must have the same length");
          } else {
            for (int j=0; j<dims[1].second; j++) {
              if ((*(yyvsp[(2) - (3)].expression_vvv))[i][j].size() != dims[2].second) {
                yyerror(&(yylsp[(2) - (3)]), parm, "syntax error, all sub-arrays of 3d array literal must have the same length");
              } else {
                for (int k=0; k<dims[2].second; k++) {
                  a.push_back((*(yyvsp[(2) - (3)].expression_vvv))[i][j][k]);
                }
              }
            }
          }
        }
        (yyval.expression) = new ArrayLit((yyloc),a,dims);
        delete (yyvsp[(2) - (3)].expression_vvv);
      } else {
        (yyval.expression) = NULL;
      }
    ;}
    break;

  case 227:

    { (yyval.expression_vvv)=new std::vector<std::vector<std::vector<MiniZinc::Expression*> > >;
      ;}
    break;

  case 228:

    { (yyval.expression_vvv)=new std::vector<std::vector<std::vector<MiniZinc::Expression*> > >;
        if ((yyvsp[(2) - (3)].expression_vv)) (yyval.expression_vvv)->push_back(*(yyvsp[(2) - (3)].expression_vv));
        delete (yyvsp[(2) - (3)].expression_vv);
      ;}
    break;

  case 229:

    { (yyval.expression_vvv)=(yyvsp[(1) - (5)].expression_vvv);
        if ((yyval.expression_vvv) && (yyvsp[(4) - (5)].expression_vv)) (yyval.expression_vvv)->push_back(*(yyvsp[(4) - (5)].expression_vv));
        delete (yyvsp[(4) - (5)].expression_vv);
      ;}
    break;

  case 230:

    { (yyval.expression_vv)=new std::vector<std::vector<MiniZinc::Expression*> >;
        if ((yyvsp[(1) - (1)].expression_v)) (yyval.expression_vv)->push_back(*(yyvsp[(1) - (1)].expression_v));
        delete (yyvsp[(1) - (1)].expression_v);
      ;}
    break;

  case 231:

    { (yyval.expression_vv)=(yyvsp[(1) - (3)].expression_vv); if ((yyval.expression_vv) && (yyvsp[(3) - (3)].expression_v)) (yyval.expression_vv)->push_back(*(yyvsp[(3) - (3)].expression_v)); delete (yyvsp[(3) - (3)].expression_v); ;}
    break;

  case 232:

    { if ((yyvsp[(4) - (5)].generators)) (yyval.expression)=new Comprehension((yyloc), (yyvsp[(2) - (5)].expression), *(yyvsp[(4) - (5)].generators), false);
        delete (yyvsp[(4) - (5)].generators);
      ;}
    break;

  case 233:

    {
        std::vector<Expression*> iexps;
        iexps.push_back((yyvsp[(2) - (5)].expression));
        iexps.push_back((yyvsp[(4) - (5)].expression));
        (yyval.expression)=new ITE((yyloc), iexps, NULL);
      ;}
    break;

  case 234:

    {
        std::vector<Expression*> iexps;
        iexps.push_back((yyvsp[(2) - (8)].expression));
        iexps.push_back((yyvsp[(4) - (8)].expression));
        if ((yyvsp[(5) - (8)].expression_v)) {
          for (unsigned int i=0; i<(yyvsp[(5) - (8)].expression_v)->size(); i+=2) {
            iexps.push_back((*(yyvsp[(5) - (8)].expression_v))[i]);
            iexps.push_back((*(yyvsp[(5) - (8)].expression_v))[i+1]);
          }
        }
        (yyval.expression)=new ITE((yyloc), iexps,(yyvsp[(7) - (8)].expression));
        delete (yyvsp[(5) - (8)].expression_v);
      ;}
    break;

  case 235:

    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; ;}
    break;

  case 236:

    { (yyval.expression_v)=(yyvsp[(1) - (5)].expression_v); if ((yyval.expression_v) && (yyvsp[(3) - (5)].expression) && (yyvsp[(5) - (5)].expression)) { (yyval.expression_v)->push_back((yyvsp[(3) - (5)].expression)); (yyval.expression_v)->push_back((yyvsp[(5) - (5)].expression)); } ;}
    break;

  case 237:

    { (yyval.iValue)=BOT_EQUIV; ;}
    break;

  case 238:

    { (yyval.iValue)=BOT_IMPL; ;}
    break;

  case 239:

    { (yyval.iValue)=BOT_RIMPL; ;}
    break;

  case 240:

    { (yyval.iValue)=BOT_OR; ;}
    break;

  case 241:

    { (yyval.iValue)=BOT_XOR; ;}
    break;

  case 242:

    { (yyval.iValue)=BOT_AND; ;}
    break;

  case 243:

    { (yyval.iValue)=BOT_LE; ;}
    break;

  case 244:

    { (yyval.iValue)=BOT_GR; ;}
    break;

  case 245:

    { (yyval.iValue)=BOT_LQ; ;}
    break;

  case 246:

    { (yyval.iValue)=BOT_GQ; ;}
    break;

  case 247:

    { (yyval.iValue)=BOT_EQ; ;}
    break;

  case 248:

    { (yyval.iValue)=BOT_NQ; ;}
    break;

  case 249:

    { (yyval.iValue)=BOT_IN; ;}
    break;

  case 250:

    { (yyval.iValue)=BOT_SUBSET; ;}
    break;

  case 251:

    { (yyval.iValue)=BOT_SUPERSET; ;}
    break;

  case 252:

    { (yyval.iValue)=BOT_UNION; ;}
    break;

  case 253:

    { (yyval.iValue)=BOT_DIFF; ;}
    break;

  case 254:

    { (yyval.iValue)=BOT_SYMDIFF; ;}
    break;

  case 255:

    { (yyval.iValue)=BOT_PLUS; ;}
    break;

  case 256:

    { (yyval.iValue)=BOT_MINUS; ;}
    break;

  case 257:

    { (yyval.iValue)=BOT_MULT; ;}
    break;

  case 258:

    { (yyval.iValue)=BOT_POW; ;}
    break;

  case 259:

    { (yyval.iValue)=BOT_DIV; ;}
    break;

  case 260:

    { (yyval.iValue)=BOT_IDIV; ;}
    break;

  case 261:

    { (yyval.iValue)=BOT_MOD; ;}
    break;

  case 262:

    { (yyval.iValue)=BOT_INTERSECT; ;}
    break;

  case 263:

    { (yyval.iValue)=BOT_PLUSPLUS; ;}
    break;

  case 264:

    { (yyval.iValue)=-1; ;}
    break;

  case 265:

    { if ((yyvsp[(1) - (6)].iValue)==-1) {
          (yyval.expression)=NULL;
          yyerror(&(yylsp[(3) - (6)]), parm, "syntax error, unary operator with two arguments");
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[(3) - (6)].expression),static_cast<BinOpType>((yyvsp[(1) - (6)].iValue)),(yyvsp[(5) - (6)].expression));
        }
      ;}
    break;

  case 266:

    { int uot=-1;
        switch ((yyvsp[(1) - (4)].iValue)) {
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
          yyerror(&(yylsp[(3) - (4)]), parm, "syntax error, binary operator with unary argument list");
          break;
        }
        if (uot==-1)
          (yyval.expression)=NULL;
        else {
          if (uot==UOT_PLUS && (yyvsp[(3) - (4)].expression) && ((yyvsp[(3) - (4)].expression)->isa<IntLit>() || (yyvsp[(3) - (4)].expression)->isa<FloatLit>())) {
            (yyval.expression) = (yyvsp[(3) - (4)].expression);
          } else if (uot==UOT_MINUS && (yyvsp[(3) - (4)].expression) && (yyvsp[(3) - (4)].expression)->isa<IntLit>()) {
            (yyval.expression) = IntLit::a(-(yyvsp[(3) - (4)].expression)->cast<IntLit>()->v());
          } else if (uot==UOT_MINUS && (yyvsp[(3) - (4)].expression) && (yyvsp[(3) - (4)].expression)->isa<FloatLit>()) {
            (yyval.expression) = FloatLit::a(-(yyvsp[(3) - (4)].expression)->cast<FloatLit>()->v());
          } else {
            (yyval.expression)=new UnOp((yyloc), static_cast<UnOpType>(uot),(yyvsp[(3) - (4)].expression));
          }
        }
      ;}
    break;

  case 267:

    { (yyval.expression)=new Call((yyloc), (yyvsp[(1) - (3)].sValue), std::vector<Expression*>()); free((yyvsp[(1) - (3)].sValue)); ;}
    break;

  case 269:

    { 
        if ((yyvsp[(3) - (4)].expression_p)!=NULL) {
          bool hadWhere = false;
          std::vector<Expression*> args;
          for (unsigned int i=0; i<(yyvsp[(3) - (4)].expression_p)->size(); i++) {
            if ((*(yyvsp[(3) - (4)].expression_p))[i].second) {
              yyerror(&(yylsp[(3) - (4)]), parm, "syntax error, 'where' expression outside generator call");
              hadWhere = true;
              (yyval.expression)=NULL;
            }
            args.push_back((*(yyvsp[(3) - (4)].expression_p))[i].first);
          }
          if (!hadWhere) {
            (yyval.expression)=new Call((yyloc), (yyvsp[(1) - (4)].sValue), args);
          }
        }
        free((yyvsp[(1) - (4)].sValue));
        delete (yyvsp[(3) - (4)].expression_p);
      ;}
    break;

  case 270:

    { 
        vector<Generator> gens;
        vector<Id*> ids;
        if ((yyvsp[(3) - (7)].expression_p)) {
          for (unsigned int i=0; i<(yyvsp[(3) - (7)].expression_p)->size(); i++) {
            if (Id* id = Expression::dyn_cast<Id>((*(yyvsp[(3) - (7)].expression_p))[i].first)) {
              if ((*(yyvsp[(3) - (7)].expression_p))[i].second) {
                ParserLocation loc = (*(yyvsp[(3) - (7)].expression_p))[i].second->loc().parserLocation();
                yyerror(&loc, parm, "illegal where expression in generator call");
              }
              ids.push_back(id);
            } else {
              if (BinOp* boe = Expression::dyn_cast<BinOp>((*(yyvsp[(3) - (7)].expression_p))[i].first)) {
                if (boe->lhs() && boe->rhs()) {
                  Id* id = Expression::dyn_cast<Id>(boe->lhs());
                  if (id && boe->op() == BOT_IN) {
                    ids.push_back(id);
                    gens.push_back(Generator(ids,boe->rhs(),(*(yyvsp[(3) - (7)].expression_p))[i].second));
                    ids = vector<Id*>();
                  } else if (id && boe->op() == BOT_EQ && ids.empty()) {
                    ids.push_back(id);
                    gens.push_back(Generator(ids,NULL,boe->rhs()));
                    if ((*(yyvsp[(3) - (7)].expression_p))[i].second) {
                      gens.push_back(Generator(gens.size(),(*(yyvsp[(3) - (7)].expression_p))[i].second));
                    }
                    ids = vector<Id*>();
                  } else {
                    ParserLocation loc = (*(yyvsp[(3) - (7)].expression_p))[i].first->loc().parserLocation();
                    yyerror(&loc, parm, "illegal expression in generator call");
                  }
                }
              } else {
                ParserLocation loc = (*(yyvsp[(3) - (7)].expression_p))[i].first->loc().parserLocation();
                yyerror(&loc, parm, "illegal expression in generator call");
              }
            }
          }
        }
        if (ids.size() != 0) {
          yyerror(&(yylsp[(3) - (7)]), parm, "illegal expression in generator call");
        }
        ParserState* pp = static_cast<ParserState*>(parm);
        if (pp->hadError) {
          (yyval.expression)=NULL;
        } else {
          Generators g; g._g = gens;
          Comprehension* ac = new Comprehension((yyloc), (yyvsp[(6) - (7)].expression),g,false);
          vector<Expression*> args; args.push_back(ac);
          (yyval.expression)=new Call((yyloc), (yyvsp[(1) - (7)].sValue), args);
        }
        free((yyvsp[(1) - (7)].sValue));
        delete (yyvsp[(3) - (7)].expression_p);
      ;}
    break;

  case 272:

    { (yyval.expression_p)=new vector<pair<Expression*,Expression*> >;
        (yyval.expression_p)->push_back(pair<Expression*,Expression*>((yyvsp[(1) - (1)].expression),NULL)); ;}
    break;

  case 273:

    { (yyval.expression_p)=new vector<pair<Expression*,Expression*> >;
        (yyval.expression_p)->push_back(pair<Expression*,Expression*>((yyvsp[(1) - (3)].expression),(yyvsp[(3) - (3)].expression))); ;}
    break;

  case 274:

    { (yyval.expression_p)=(yyvsp[(1) - (3)].expression_p); if ((yyval.expression_p) && (yyvsp[(3) - (3)].expression)) (yyval.expression_p)->push_back(pair<Expression*,Expression*>((yyvsp[(3) - (3)].expression),NULL)); ;}
    break;

  case 275:

    { (yyval.expression_p)=(yyvsp[(1) - (5)].expression_p); if ((yyval.expression_p) && (yyvsp[(3) - (5)].expression) && (yyvsp[(5) - (5)].expression)) (yyval.expression_p)->push_back(pair<Expression*,Expression*>((yyvsp[(3) - (5)].expression),(yyvsp[(5) - (5)].expression))); ;}
    break;

  case 276:

    { if ((yyvsp[(3) - (6)].expression_v) && (yyvsp[(6) - (6)].expression)) {
          (yyval.expression)=new Let((yyloc), *(yyvsp[(3) - (6)].expression_v), (yyvsp[(6) - (6)].expression)); delete (yyvsp[(3) - (6)].expression_v);
        } else {
          (yyval.expression)=NULL;
        }
      ;}
    break;

  case 277:

    { if ((yyvsp[(3) - (7)].expression_v) && (yyvsp[(7) - (7)].expression)) {
          (yyval.expression)=new Let((yyloc), *(yyvsp[(3) - (7)].expression_v), (yyvsp[(7) - (7)].expression)); delete (yyvsp[(3) - (7)].expression_v);
        } else {
          (yyval.expression)=NULL;
        }
      ;}
    break;

  case 278:

    { (yyval.expression_v)=new vector<Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].vardeclexpr)); ;}
    break;

  case 279:

    { (yyval.expression_v)=new vector<Expression*>;
        if ((yyvsp[(1) - (1)].item)) {
          ConstraintI* ce = (yyvsp[(1) - (1)].item)->cast<ConstraintI>();
          (yyval.expression_v)->push_back(ce->e());
          ce->e(NULL);
        }
      ;}
    break;

  case 280:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); if ((yyval.expression_v) && (yyvsp[(3) - (3)].vardeclexpr)) (yyval.expression_v)->push_back((yyvsp[(3) - (3)].vardeclexpr)); ;}
    break;

  case 281:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v);
        if ((yyval.expression_v) && (yyvsp[(3) - (3)].item)) {
          ConstraintI* ce = (yyvsp[(3) - (3)].item)->cast<ConstraintI>();
          (yyval.expression_v)->push_back(ce->e());
          ce->e(NULL);
        }
      ;}
    break;

  case 284:

    { (yyval.vardeclexpr) = (yyvsp[(1) - (2)].vardeclexpr);
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->toplevel(false);
        if ((yyval.vardeclexpr) && (yyvsp[(2) - (2)].expression_v)) (yyval.vardeclexpr)->addAnnotations(*(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v);
      ;}
    break;

  case 285:

    { if ((yyvsp[(1) - (4)].vardeclexpr)) (yyvsp[(1) - (4)].vardeclexpr)->e((yyvsp[(4) - (4)].expression));
        (yyval.vardeclexpr) = (yyvsp[(1) - (4)].vardeclexpr);
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->loc((yyloc));
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->toplevel(false);
        if ((yyval.vardeclexpr) && (yyvsp[(2) - (4)].expression_v)) (yyval.vardeclexpr)->addAnnotations(*(yyvsp[(2) - (4)].expression_v));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 286:

    { (yyval.expression_v)=NULL; ;}
    break;

  case 288:

    { (yyval.expression) = (yyvsp[(1) - (1)].expression); ;}
    break;

  case 289:

    { (yyval.expression) = new Call((yylsp[(1) - (1)]), ASTString("mzn_expression_name"), {(yyvsp[(1) - (1)].expression)}); ;}
    break;

  case 290:

    { (yyval.expression_v)=new std::vector<Expression*>(1);
        (*(yyval.expression_v))[0] = (yyvsp[(2) - (2)].expression);
      ;}
    break;

  case 291:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); if ((yyval.expression_v)) (yyval.expression_v)->push_back((yyvsp[(3) - (3)].expression)); ;}
    break;

  case 292:

    { (yyval.sValue)=(yyvsp[(1) - (1)].sValue); ;}
    break;

  case 293:

    { (yyval.sValue)=strdup("'<->'"); ;}
    break;

  case 294:

    { (yyval.sValue)=strdup("'->'"); ;}
    break;

  case 295:

    { (yyval.sValue)=strdup("'<-'"); ;}
    break;

  case 296:

    { (yyval.sValue)=strdup("'\\/'"); ;}
    break;

  case 297:

    { (yyval.sValue)=strdup("'xor'"); ;}
    break;

  case 298:

    { (yyval.sValue)=strdup("'/\\'"); ;}
    break;

  case 299:

    { (yyval.sValue)=strdup("'<'"); ;}
    break;

  case 300:

    { (yyval.sValue)=strdup("'>'"); ;}
    break;

  case 301:

    { (yyval.sValue)=strdup("'<='"); ;}
    break;

  case 302:

    { (yyval.sValue)=strdup("'>='"); ;}
    break;

  case 303:

    { (yyval.sValue)=strdup("'='"); ;}
    break;

  case 304:

    { (yyval.sValue)=strdup("'!='"); ;}
    break;

  case 305:

    { (yyval.sValue)=strdup("'in'"); ;}
    break;

  case 306:

    { (yyval.sValue)=strdup("'subset'"); ;}
    break;

  case 307:

    { (yyval.sValue)=strdup("'superset'"); ;}
    break;

  case 308:

    { (yyval.sValue)=strdup("'union'"); ;}
    break;

  case 309:

    { (yyval.sValue)=strdup("'diff'"); ;}
    break;

  case 310:

    { (yyval.sValue)=strdup("'symdiff'"); ;}
    break;

  case 311:

    { (yyval.sValue)=strdup("'..'"); ;}
    break;

  case 312:

    { (yyval.sValue)=strdup("'+'"); ;}
    break;

  case 313:

    { (yyval.sValue)=strdup("'-'"); ;}
    break;

  case 314:

    { (yyval.sValue)=strdup("'*'"); ;}
    break;

  case 315:

    { (yyval.sValue)=strdup("'^'"); ;}
    break;

  case 316:

    { (yyval.sValue)=strdup("'/'"); ;}
    break;

  case 317:

    { (yyval.sValue)=strdup("'div'"); ;}
    break;

  case 318:

    { (yyval.sValue)=strdup("'mod'"); ;}
    break;

  case 319:

    { (yyval.sValue)=strdup("'intersect'"); ;}
    break;

  case 320:

    { (yyval.sValue)=strdup("'not'"); ;}
    break;

  case 321:

    { (yyval.sValue)=strdup("'++'"); ;}
    break;


/* Line 1267 of yacc.c.  */

      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, parm, YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (&yylloc, parm, yymsg);
	  }
	else
	  {
	    yyerror (&yylloc, parm, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc, parm);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[0] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp, parm);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the look-ahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

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

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, parm, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc, parm);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp, parm);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



