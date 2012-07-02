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
/* Tokens.  */
#define MZN_INTEGER_LITERAL 258
#define MZN_BOOL_LITERAL 259
#define MZN_FLOAT_LITERAL 260
#define MZN_IDENTIFIER 261
#define MZN_QUOTED_IDENTIFIER 262
#define MZN_STRING_LITERAL 263
#define MZN_TI_IDENTIFIER 264
#define MZN_VAR 265
#define MZN_PAR 266
#define MZN_SVAR 267
#define MZN_ANN 268
#define MZN_ANNOTATION 269
#define MZN_ANY 270
#define MZN_ARRAY 271
#define MZN_BOOL 272
#define MZN_CASE 273
#define MZN_CONSTRAINT 274
#define MZN_DEFAULT 275
#define MZN_ELSE 276
#define MZN_ELSEIF 277
#define MZN_ENDIF 278
#define MZN_ENUM 279
#define MZN_FLOAT 280
#define MZN_FUNCTION 281
#define MZN_IF 282
#define MZN_INCLUDE 283
#define MZN_INT 284
#define MZN_LET 285
#define MZN_MAXIMIZE 286
#define MZN_MINIMIZE 287
#define MZN_OF 288
#define MZN_SATISFY 289
#define MZN_OUTPUT 290
#define MZN_PREDICATE 291
#define MZN_RECORD 292
#define MZN_SET 293
#define MZN_SOLVE 294
#define MZN_STRING 295
#define MZN_TEST 296
#define MZN_THEN 297
#define MZN_TUPLE 298
#define MZN_TYPE 299
#define MZN_UNDERSCORE 300
#define MZN_VARIANT_RECORD 301
#define MZN_WHERE 302
#define MZN_LEFT_BRACKET 303
#define MZN_LEFT_2D_BRACKET 304
#define MZN_RIGHT_BRACKET 305
#define MZN_RIGHT_2D_BRACKET 306
#define UNKNOWN_CHAR 307
#define PREC_ANNO 308
#define MZN_EQUIV 309
#define MZN_RIMPL 310
#define MZN_IMPL 311
#define MZN_XOR 312
#define MZN_OR 313
#define MZN_AND 314
#define MZN_NQ 315
#define MZN_EQ 316
#define MZN_GQ 317
#define MZN_LQ 318
#define MZN_GR 319
#define MZN_LE 320
#define MZN_SUPERSET 321
#define MZN_SUBSET 322
#define MZN_IN 323
#define MZN_SYMDIFF 324
#define MZN_DIFF 325
#define MZN_UNION 326
#define MZN_DOTDOT 327
#define MZN_MINUS 328
#define MZN_PLUS 329
#define MZN_INTERSECT 330
#define MZN_MOD 331
#define MZN_IDIV 332
#define MZN_DIV 333
#define MZN_MULT 334
#define MZN_NOT 335
#define MZN_COLONCOLON 336
#define MZN_PLUSPLUS 337
#define MZN_EQUIV_QUOTED 338
#define MZN_IMPL_QUOTED 339
#define MZN_RIMPL_QUOTED 340
#define MZN_OR_QUOTED 341
#define MZN_XOR_QUOTED 342
#define MZN_AND_QUOTED 343
#define MZN_LE_QUOTED 344
#define MZN_GR_QUOTED 345
#define MZN_LQ_QUOTED 346
#define MZN_GQ_QUOTED 347
#define MZN_EQ_QUOTED 348
#define MZN_NQ_QUOTED 349
#define MZN_IN_QUOTED 350
#define MZN_SUBSET_QUOTED 351
#define MZN_SUPERSET_QUOTED 352
#define MZN_UNION_QUOTED 353
#define MZN_DIFF_QUOTED 354
#define MZN_SYMDIFF_QUOTED 355
#define MZN_DOTDOT_QUOTED 356
#define MZN_PLUS_QUOTED 357
#define MZN_MINUS_QUOTED 358
#define MZN_MULT_QUOTED 359
#define MZN_DIV_QUOTED 360
#define MZN_IDIV_QUOTED 361
#define MZN_MOD_QUOTED 362
#define MZN_INTERSECT_QUOTED 363
#define MZN_NOT_QUOTED 364
#define MZN_COLONCOLON_QUOTED 365
#define MZN_PLUSPLUS_QUOTED 366




/* Copy the first part of user declarations.  */
#line 36 "lib/parser.yxx"

#define YYPARSE_PARAM parm
#define YYLEX_PARAM static_cast<ParserState*>(parm)->yyscanner
#include <minizinc/parser.hh>
#include <iostream>
#include <fstream>
#include <map>

using namespace std;
using namespace MiniZinc;

namespace MiniZinc{ class Location; }
#define YYLTYPE MiniZinc::Location
#define YYLTYPE_IS_DECLARED 1

#define YYLLOC_DEFAULT(Current, Rhs, N) \
  Current.filename = Rhs[1].filename; \
  Current.first_line = Rhs[1].first_line; \
  Current.first_column = Rhs[1].first_column; \
  Current.last_line = Rhs[N].last_line; \
  Current.last_column = Rhs[N].last_column;

int yyparse(void*);
int yylex(YYSTYPE*, YYLTYPE*, void* scanner);
int yylex_init (void** scanner);
int yylex_destroy (void* scanner);
int yyget_lineno (void* scanner);
void yyset_extra (void* user_defined ,void* yyscanner );

extern int yydebug;

void yyerror(YYLTYPE* location, void* parm, const string& str) {
  ParserState* pp = static_cast<ParserState*>(parm);
  Model* m = pp->model;
  while (m->parent() != NULL) {
    m = m->parent();
    pp->err << "(included from file " << m->filename() << ")" << endl;
  }
  pp->err << "In file " << location->filename << ", line "
          << location->first_line << ":" << endl;
  pp->printCurrentLine();
  for (int i=0; i<location->first_column-1; i++)
    pp->err << " ";
  for (int i=location->first_column; i<=location->last_column; i++)
    pp->err << "^";
  pp->err << std::endl << "Error: " << str << std::endl << std::endl;
  pp->hadError = true;
}

bool notInDatafile(YYLTYPE* location, void* parm, const string& item) {
  ParserState* pp = static_cast<ParserState*>(parm);
  if (pp->isDatafile) {
    yyerror(location,parm,item+" item not allowed in data file");
    return false;
  }
  return true;
}

void filepath(const string& f, string& dirname, string& basename) {
  dirname = ""; basename = f;
  for (size_t p=basename.find_first_of('/');
       p!=string::npos;
       dirname+=basename.substr(0,p+1),
       basename=basename.substr(p+1),
       p=basename.find_first_of('/')
       ) {}
}

#define GETCTX() static_cast<ParserState*>(parm)->ctx

namespace MiniZinc {

  Model* parse(const ASTContext& ctx,
               const string& filename,
               const vector<string>& datafiles,
               const vector<string>& ip,
               bool ignoreStdlib,
               ostream& err) {

    string fileDirname; string fileBasename;
    filepath(filename, fileDirname, fileBasename);

    vector<string> includePaths;
    for (unsigned int i=0; i<ip.size(); i++)
      includePaths.push_back(ip[i]);
    
    vector<pair<string,Model*> > files;
    map<string,Model*> seenModels;
    
    Model* model = new Model;
    model->_filename = ctx.alloc(fileBasename);

    if (!ignoreStdlib) {
      Model* stdlib = new Model;
      stdlib->_filename = ctx.alloc("stdlib.mzn");
      files.push_back(pair<string,Model*>("./",stdlib));
      seenModels.insert(pair<string,Model*>("stdlib.mzn",stdlib));
      Location stdlibloc;
      stdlibloc.filename=ctx.alloc(filename);
      IncludeI* stdlibinc = IncludeI::a(ctx,stdlibloc,stdlib->_filename);
      stdlibinc->setModel(stdlib,true);
      model->addItem(stdlibinc);
    }
    
    files.push_back(pair<string,Model*>("",model));
        
    while (!files.empty()) {
      pair<string,Model*>& np = files.back();
      string parentPath = np.first;
      Model* m = np.second;
      files.pop_back();
      string f(m->_filename);
            
      for (Model* p=m->_parent; p; p=p->_parent) {
        if (f == p->_filename) {
          err << "Error: cyclic includes: " << std::endl;
          for (Model* pe=m; pe; pe=pe->_parent) {
            err << "  " << pe->_filename << std::endl;
          }
          goto error;
        }
      }
      ifstream file;
      string fullname;
      if (parentPath=="") {
        fullname = filename;
        file.open(fullname.c_str());
      } else {
        includePaths.push_back(parentPath);
        for (unsigned int i=0; i<includePaths.size(); i++) {
          fullname = includePaths[i]+f;
          file.open(fullname.c_str());
          if (file.is_open())
            break;
        }
        includePaths.pop_back();
      }
      if (!file.is_open()) {
        err << "Error: cannot open file " << f << endl;
        goto error;
      }
      std::string s = string(istreambuf_iterator<char>(file),
                             istreambuf_iterator<char>());

      m->_filepath = ctx.alloc(fullname);
      ParserState pp(ctx, fullname,s, err, files, seenModels, m, false);
      yylex_init(&pp.yyscanner);
      yyset_extra(&pp, pp.yyscanner);
      yyparse(&pp);
      if (pp.yyscanner)
        yylex_destroy(pp.yyscanner);
      if (pp.hadError) {
        goto error;
      }
    }
    
    for (unsigned int i=0; i<datafiles.size(); i++) {
      string f = datafiles[i];
      std::ifstream file;
      file.open(f.c_str());
      if (!file.is_open()) {
        err << "Error: cannot open data file " << f << endl;
        goto error;
      }
      std::string s = string(istreambuf_iterator<char>(file),
                             istreambuf_iterator<char>());

      ParserState pp(ctx, f, s, err, files, seenModels, model, true);
      yylex_init(&pp.yyscanner);
      yyset_extra(&pp, pp.yyscanner);
      yyparse(&pp);
      if (pp.yyscanner)
        yylex_destroy(pp.yyscanner);
      if (pp.hadError) {
        goto error;
      }
    }
    
    return model;
  error:
    for (unsigned int i=0; i<files.size(); i++)
      delete files[i].second;
    return NULL;
  }
}



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
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
#line 224 "lib/parser.yxx"
{ int iValue; char* sValue; bool bValue; double dValue;
         MiniZinc::Item* item;
         MiniZinc::VarDecl* vardeclexpr;
         std::vector<MiniZinc::VarDecl*>* vardeclexpr_v;
         MiniZinc::TiExpr* tiexpr;
         std::vector<MiniZinc::TiExpr*>* tiexpr_v;
         MiniZinc::IntTiExpr* inttiexpr;
         std::vector<MiniZinc::IntTiExpr*>* inttiexpr_v;
         MiniZinc::BaseTiExpr* basetiexpr;
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
       }
/* Line 193 of yacc.c.  */
#line 527 "lib/parser.tab.cpp"
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
#line 552 "lib/parser.tab.cpp"

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
#define YYFINAL  147
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   3481

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  120
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  60
/* YYNRULES -- Number of rules.  */
#define YYNRULES  228
/* YYNRULES -- Number of states.  */
#define YYNSTATES  402

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   366

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     114,   115,     2,     2,   116,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   113,   112,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   117,   119,   118,     2,     2,     2,     2,
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
     105,   106,   107,   108,   109,   110,   111
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    11,    15,    16,    18,
      20,    22,    24,    26,    28,    30,    32,    34,    36,    39,
      42,    47,    51,    54,    58,    63,    68,    71,    77,    83,
      91,   100,   104,   110,   111,   114,   115,   119,   123,   126,
     128,   132,   133,   135,   139,   142,   144,   148,   150,   157,
     160,   162,   166,   168,   170,   174,   181,   185,   187,   190,
     193,   197,   202,   207,   212,   214,   216,   218,   220,   223,
     225,   228,   230,   234,   236,   240,   244,   248,   252,   256,
     260,   264,   267,   270,   274,   279,   281,   284,   286,   288,
     290,   292,   294,   297,   299,   302,   304,   307,   309,   312,
     314,   316,   318,   322,   326,   330,   334,   338,   342,   346,
     350,   354,   358,   362,   366,   370,   374,   378,   382,   386,
     390,   394,   398,   405,   409,   413,   417,   421,   425,   429,
     433,   437,   441,   444,   447,   450,   454,   459,   461,   464,
     466,   469,   471,   473,   475,   477,   479,   481,   483,   486,
     488,   491,   493,   496,   498,   501,   503,   505,   509,   512,
     516,   522,   524,   528,   531,   533,   537,   541,   544,   546,
     550,   553,   557,   561,   566,   568,   572,   578,   587,   588,
     594,   596,   598,   600,   602,   604,   606,   608,   610,   612,
     614,   616,   618,   620,   622,   624,   626,   628,   630,   632,
     634,   636,   638,   640,   642,   644,   646,   648,   655,   660,
     664,   666,   671,   679,   681,   685,   692,   700,   702,   704,
     708,   712,   714,   716,   719,   724,   725,   727,   730
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     121,     0,    -1,   122,    -1,    -1,   123,   124,    -1,   125,
      -1,   123,   112,   125,    -1,    -1,   112,    -1,   126,    -1,
     127,    -1,   128,    -1,   129,    -1,   130,    -1,   131,    -1,
     132,    -1,   133,    -1,   134,    -1,    28,     8,    -1,   140,
     178,    -1,   140,   178,    61,   153,    -1,     6,    61,   153,
      -1,    19,   153,    -1,    39,   178,    34,    -1,    39,   178,
      32,   153,    -1,    39,   178,    31,   153,    -1,    35,   153,
      -1,    36,     6,   136,   178,   135,    -1,    41,     6,   136,
     178,   135,    -1,    26,   143,   113,     6,   136,   178,   135,
      -1,   143,   113,     6,   114,   137,   115,   178,   135,    -1,
      14,     6,   136,    -1,    14,     6,   136,    61,   153,    -1,
      -1,    61,   153,    -1,    -1,   114,   137,   115,    -1,   114,
       1,   115,    -1,   138,   139,    -1,   140,    -1,   138,   116,
     140,    -1,    -1,   116,    -1,   143,   113,     6,    -1,   142,
     139,    -1,   143,    -1,   142,   116,   143,    -1,   147,    -1,
      16,    48,   144,    50,    33,   147,    -1,   145,   139,    -1,
     146,    -1,   145,   116,   146,    -1,    29,    -1,     6,    -1,
     151,    72,   151,    -1,   101,   114,   151,   116,   151,   115,
      -1,   117,   149,   118,    -1,   148,    -1,    11,   148,    -1,
      10,   148,    -1,    38,    33,   148,    -1,    11,    38,    33,
     148,    -1,    10,    38,    33,   148,    -1,    43,   114,   141,
     115,    -1,    17,    -1,    25,    -1,    40,    -1,    13,    -1,
     117,   118,    -1,   146,    -1,   150,   139,    -1,   153,    -1,
     150,   116,   153,    -1,   152,    -1,   151,    81,   152,    -1,
     151,    74,   151,    -1,   151,    73,   151,    -1,   151,    79,
     151,    -1,   151,    78,   151,    -1,   151,    77,   151,    -1,
     151,    76,   151,    -1,    74,   151,    -1,    73,   151,    -1,
     114,   153,   115,    -1,   114,   153,   115,   155,    -1,     6,
      -1,     6,   155,    -1,     4,    -1,     3,    -1,     5,    -1,
       8,    -1,   164,    -1,   164,   155,    -1,   165,    -1,   165,
     155,    -1,   167,    -1,   167,   155,    -1,   168,    -1,   168,
     155,    -1,   174,    -1,   172,    -1,   154,    -1,   153,    81,
     154,    -1,   153,    54,   153,    -1,   153,    56,   153,    -1,
     153,    55,   153,    -1,   153,    58,   153,    -1,   153,    57,
     153,    -1,   153,    59,   153,    -1,   153,    65,   153,    -1,
     153,    64,   153,    -1,   153,    63,   153,    -1,   153,    62,
     153,    -1,   153,    61,   153,    -1,   153,    60,   153,    -1,
     153,    68,   153,    -1,   153,    67,   153,    -1,   153,    66,
     153,    -1,   153,    71,   153,    -1,   153,    70,   153,    -1,
     153,    69,   153,    -1,   153,    72,   153,    -1,   101,   114,
     153,   116,   153,   115,    -1,   153,    75,   153,    -1,   153,
      82,   153,    -1,   153,    74,   153,    -1,   153,    73,   153,
      -1,   153,    79,   153,    -1,   153,    78,   153,    -1,   153,
      77,   153,    -1,   153,    76,   153,    -1,   153,     7,   153,
      -1,    80,   153,    -1,    74,   153,    -1,    73,   153,    -1,
     114,   153,   115,    -1,   114,   153,   115,   155,    -1,     6,
      -1,     6,   155,    -1,    45,    -1,    45,   155,    -1,     4,
      -1,     3,    -1,     5,    -1,     8,    -1,   156,    -1,   157,
      -1,   164,    -1,   164,   155,    -1,   165,    -1,   165,   155,
      -1,   167,    -1,   167,   155,    -1,   168,    -1,   168,   155,
      -1,   174,    -1,   172,    -1,    48,   149,    50,    -1,   117,
     118,    -1,   117,   149,   118,    -1,   117,   153,   119,   158,
     118,    -1,   159,    -1,   159,    47,   153,    -1,   160,   139,
      -1,   161,    -1,   160,   116,   161,    -1,   162,    68,   153,
      -1,   163,   139,    -1,     6,    -1,   163,   116,     6,    -1,
      48,    50,    -1,    48,   149,    50,    -1,    49,   166,    51,
      -1,    49,   166,   119,    51,    -1,   149,    -1,   166,   119,
     149,    -1,    48,   153,   119,   158,    50,    -1,    27,   153,
      42,   153,   169,    21,   153,    23,    -1,    -1,   169,    22,
     153,    42,   153,    -1,    83,    -1,    84,    -1,    85,    -1,
      86,    -1,    87,    -1,    88,    -1,    89,    -1,    90,    -1,
      91,    -1,    92,    -1,    93,    -1,    94,    -1,    95,    -1,
      96,    -1,    97,    -1,    98,    -1,    99,    -1,   100,    -1,
     102,    -1,   103,    -1,   104,    -1,   105,    -1,   106,    -1,
     107,    -1,   108,    -1,   111,    -1,   109,    -1,   170,   114,
     153,   116,   153,   115,    -1,   170,   114,   153,   115,    -1,
       6,   114,   115,    -1,   171,    -1,     6,   114,   173,   115,
      -1,     6,   114,   173,   115,   114,   153,   115,    -1,   149,
      -1,   149,    47,   153,    -1,    30,   117,   175,   118,    68,
     153,    -1,    30,   117,   175,   176,   118,    68,   153,    -1,
     177,    -1,   129,    -1,   175,   176,   177,    -1,   175,   176,
     129,    -1,   116,    -1,   112,    -1,   140,   178,    -1,   140,
     178,    61,   153,    -1,    -1,   179,    -1,    81,   154,    -1,
     179,    81,   154,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   367,   367,   369,   371,   374,   379,   385,   385,   388,
     390,   392,   393,   395,   397,   399,   401,   403,   407,   430,
     434,   441,   447,   451,   453,   455,   459,   463,   468,   475,
     480,   487,   494,   502,   503,   508,   509,   511,   514,   518,
     520,   523,   523,   526,   531,   535,   537,   541,   542,   549,
     552,   554,   558,   560,   562,   564,   566,   572,   574,   576,
     578,   580,   582,   584,   588,   590,   592,   594,   596,   598,
     601,   604,   606,   612,   613,   615,   617,   619,   621,   623,
     625,   627,   629,   633,   635,   637,   641,   645,   647,   649,
     651,   653,   654,   657,   658,   661,   662,   665,   666,   669,
     670,   675,   676,   678,   680,   682,   684,   686,   688,   690,
     692,   694,   696,   698,   700,   702,   704,   706,   708,   710,
     712,   714,   716,   718,   720,   722,   724,   726,   728,   730,
     732,   734,   740,   742,   744,   748,   750,   752,   754,   757,
     759,   762,   764,   766,   768,   770,   771,   772,   773,   776,
     777,   780,   781,   784,   785,   788,   789,   792,   796,   798,
     802,   808,   810,   813,   816,   818,   822,   825,   828,   830,
     834,   836,   840,   847,   856,   861,   865,   871,   883,   884,
     888,   890,   892,   894,   896,   898,   900,   902,   904,   906,
     908,   910,   912,   914,   916,   918,   920,   922,   924,   926,
     928,   930,   932,   934,   936,   938,   940,   944,   952,   975,
     977,   978,   989,  1028,  1033,  1040,  1042,  1062,  1064,  1070,
    1072,  1079,  1079,  1082,  1086,  1095,  1096,  1099,  1101
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "MZN_INTEGER_LITERAL",
  "MZN_BOOL_LITERAL", "MZN_FLOAT_LITERAL", "MZN_IDENTIFIER",
  "MZN_QUOTED_IDENTIFIER", "MZN_STRING_LITERAL", "MZN_TI_IDENTIFIER",
  "MZN_VAR", "MZN_PAR", "MZN_SVAR", "MZN_ANN", "MZN_ANNOTATION", "MZN_ANY",
  "MZN_ARRAY", "MZN_BOOL", "MZN_CASE", "MZN_CONSTRAINT", "MZN_DEFAULT",
  "MZN_ELSE", "MZN_ELSEIF", "MZN_ENDIF", "MZN_ENUM", "MZN_FLOAT",
  "MZN_FUNCTION", "MZN_IF", "MZN_INCLUDE", "MZN_INT", "MZN_LET",
  "MZN_MAXIMIZE", "MZN_MINIMIZE", "MZN_OF", "MZN_SATISFY", "MZN_OUTPUT",
  "MZN_PREDICATE", "MZN_RECORD", "MZN_SET", "MZN_SOLVE", "MZN_STRING",
  "MZN_TEST", "MZN_THEN", "MZN_TUPLE", "MZN_TYPE", "MZN_UNDERSCORE",
  "MZN_VARIANT_RECORD", "MZN_WHERE", "MZN_LEFT_BRACKET",
  "MZN_LEFT_2D_BRACKET", "MZN_RIGHT_BRACKET", "MZN_RIGHT_2D_BRACKET",
  "UNKNOWN_CHAR", "PREC_ANNO", "MZN_EQUIV", "MZN_RIMPL", "MZN_IMPL",
  "MZN_XOR", "MZN_OR", "MZN_AND", "MZN_NQ", "MZN_EQ", "MZN_GQ", "MZN_LQ",
  "MZN_GR", "MZN_LE", "MZN_SUPERSET", "MZN_SUBSET", "MZN_IN",
  "MZN_SYMDIFF", "MZN_DIFF", "MZN_UNION", "MZN_DOTDOT", "MZN_MINUS",
  "MZN_PLUS", "MZN_INTERSECT", "MZN_MOD", "MZN_IDIV", "MZN_DIV",
  "MZN_MULT", "MZN_NOT", "MZN_COLONCOLON", "MZN_PLUSPLUS",
  "MZN_EQUIV_QUOTED", "MZN_IMPL_QUOTED", "MZN_RIMPL_QUOTED",
  "MZN_OR_QUOTED", "MZN_XOR_QUOTED", "MZN_AND_QUOTED", "MZN_LE_QUOTED",
  "MZN_GR_QUOTED", "MZN_LQ_QUOTED", "MZN_GQ_QUOTED", "MZN_EQ_QUOTED",
  "MZN_NQ_QUOTED", "MZN_IN_QUOTED", "MZN_SUBSET_QUOTED",
  "MZN_SUPERSET_QUOTED", "MZN_UNION_QUOTED", "MZN_DIFF_QUOTED",
  "MZN_SYMDIFF_QUOTED", "MZN_DOTDOT_QUOTED", "MZN_PLUS_QUOTED",
  "MZN_MINUS_QUOTED", "MZN_MULT_QUOTED", "MZN_DIV_QUOTED",
  "MZN_IDIV_QUOTED", "MZN_MOD_QUOTED", "MZN_INTERSECT_QUOTED",
  "MZN_NOT_QUOTED", "MZN_COLONCOLON_QUOTED", "MZN_PLUSPLUS_QUOTED", "';'",
  "':'", "'('", "')'", "','", "'{'", "'}'", "'|'", "$accept", "model",
  "item_list", "item_list_head", "semi_or_none", "item", "include_item",
  "vardecl_item", "assign_item", "constraint_item", "solve_item",
  "output_item", "predicate_item", "function_item", "annotation_item",
  "operation_item_tail", "params", "params_list", "params_list_head",
  "comma_or_none", "ti_expr_and_id", "ti_expr_list", "ti_expr_list_head",
  "ti_expr", "int_ti_expr_list", "int_ti_expr_list_head", "int_ti_expr",
  "base_ti_expr", "base_ti_expr_tail", "expr_list", "expr_list_head",
  "num_expr", "num_expr_atom_head", "expr", "expr_atom_head",
  "array_access_tail", "set_literal", "set_comp", "comp_tail",
  "generator_list", "generator_list_head", "generator", "id_list",
  "id_list_head", "simple_array_literal", "simple_array_literal_2d",
  "simple_array_literal_2d_list", "simple_array_comp", "if_then_else_expr",
  "elseif_list", "quoted_op", "quoted_op_call", "call_expr",
  "comp_or_expr", "let_expr", "let_vardecl_item_list", "comma_or_semi",
  "let_vardecl_item", "annotations", "ne_annotations", 0
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
     365,   366,    59,    58,    40,    41,    44,   123,   125,   124
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   120,   121,   122,   122,   123,   123,   124,   124,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   126,   127,
     127,   128,   129,   130,   130,   130,   131,   132,   132,   133,
     133,   134,   134,   135,   135,   136,   136,   136,   137,   138,
     138,   139,   139,   140,   141,   142,   142,   143,   143,   144,
     145,   145,   146,   146,   146,   146,   146,   147,   147,   147,
     147,   147,   147,   147,   148,   148,   148,   148,   148,   148,
     149,   150,   150,   151,   151,   151,   151,   151,   151,   151,
     151,   151,   151,   152,   152,   152,   152,   152,   152,   152,
     152,   152,   152,   152,   152,   152,   152,   152,   152,   152,
     152,   153,   153,   153,   153,   153,   153,   153,   153,   153,
     153,   153,   153,   153,   153,   153,   153,   153,   153,   153,
     153,   153,   153,   153,   153,   153,   153,   153,   153,   153,
     153,   153,   153,   153,   153,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   154,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   154,   154,   154,   155,   156,   156,
     157,   158,   158,   159,   160,   160,   161,   162,   163,   163,
     164,   164,   165,   165,   166,   166,   167,   168,   169,   169,
     170,   170,   170,   170,   170,   170,   170,   170,   170,   170,
     170,   170,   170,   170,   170,   170,   170,   170,   170,   170,
     170,   170,   170,   170,   170,   170,   170,   171,   171,   172,
     172,   172,   172,   173,   173,   174,   174,   175,   175,   175,
     175,   176,   176,   177,   177,   178,   178,   179,   179
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     3,     0,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       4,     3,     2,     3,     4,     4,     2,     5,     5,     7,
       8,     3,     5,     0,     2,     0,     3,     3,     2,     1,
       3,     0,     1,     3,     2,     1,     3,     1,     6,     2,
       1,     3,     1,     1,     3,     6,     3,     1,     2,     2,
       3,     4,     4,     4,     1,     1,     1,     1,     2,     1,
       2,     1,     3,     1,     3,     3,     3,     3,     3,     3,
       3,     2,     2,     3,     4,     1,     2,     1,     1,     1,
       1,     1,     2,     1,     2,     1,     2,     1,     2,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     6,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     2,     3,     4,     1,     2,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       2,     1,     2,     1,     2,     1,     1,     3,     2,     3,
       5,     1,     3,     2,     1,     3,     3,     2,     1,     3,
       2,     3,     3,     4,     1,     3,     5,     8,     0,     5,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     6,     4,     3,
       1,     4,     7,     1,     3,     6,     7,     1,     1,     3,
       3,     1,     1,     2,     4,     0,     1,     2,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,    88,    87,    89,    85,    90,     0,     0,    67,     0,
       0,    64,     0,    65,     0,     0,     0,    52,     0,     0,
       0,     0,   225,    66,     0,     0,     0,     0,     0,     0,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   194,   195,   196,   197,     0,   198,
     199,   200,   201,   202,   203,   204,   206,   205,     0,     0,
       0,     2,     7,     5,     9,    10,    11,    12,    13,    14,
      15,    16,    17,   225,     0,    69,    47,    57,     0,    73,
      91,    93,    95,    97,     0,   210,   100,    99,     0,     0,
       0,    86,    85,     0,    59,     0,    58,    35,     0,   142,
     141,   143,   137,   144,   139,     0,     0,     0,     0,     0,
       0,    22,   101,   145,   146,   147,   149,   151,   153,   156,
     155,     0,     0,    18,     0,    26,    35,     0,     0,     0,
     226,    35,     0,   170,     0,    41,    71,   174,    71,     0,
      85,    82,    81,     0,     0,    68,     0,     1,     8,     4,
      19,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      92,    94,    96,    98,     0,     0,    21,   209,   213,     0,
       0,     0,     0,    31,     0,     0,    41,    50,   138,   140,
     134,   133,   132,     0,     0,   158,     0,    71,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   148,   150,   152,
     154,     0,     0,   218,   225,     0,     0,   217,   225,    60,
     227,     0,     0,    23,     0,   225,     0,    41,    45,   171,
      42,    70,     0,   172,     0,     0,    83,    56,     6,     0,
      43,    54,    76,    75,    80,    79,    78,    77,    74,     0,
     157,     0,   211,    62,    61,     0,     0,    41,    39,     0,
       0,    42,    49,     0,   135,   159,     0,   131,   103,   105,
     104,   107,   106,   108,   114,   113,   112,   111,   110,   109,
     117,   116,   115,   120,   119,   118,   121,   126,   125,   123,
     130,   129,   128,   127,   102,   124,    35,   178,   223,     0,
     222,   221,     0,     0,    33,    25,    24,   228,    33,    63,
      42,    44,    72,   168,     0,   161,    41,   164,     0,    41,
     173,   175,     0,    84,    20,     0,   208,     0,   214,     0,
      37,    36,    42,    38,    32,     0,    51,     0,   136,     0,
     225,     0,     0,    43,     0,     0,   220,   219,     0,    27,
      28,    46,   176,     0,    42,   163,     0,    42,   167,     0,
       0,     0,     0,    40,    48,     0,   160,    33,     0,     0,
     224,   215,     0,    34,   162,   165,   166,   169,    55,   225,
     207,   212,   122,    29,     0,     0,   216,    33,   177,     0,
      30,   179
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    60,    61,    62,   149,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,   359,   173,   266,   267,   241,
      73,   236,   237,   225,   175,   176,    75,    76,    77,   146,
     135,    78,    79,   138,   112,    91,   113,   114,   324,   325,
     326,   327,   328,   329,   115,   116,   139,   117,   118,   351,
      84,    85,   119,   169,   120,   226,   313,   227,   129,   130
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -313
static const yytype_int16 yypact[] =
{
    1247,  -313,  -313,  -313,   -23,  -313,  1707,  1822,  -313,    17,
     -37,  -313,  2397,  -313,  1477,  2397,    12,  -313,   -91,  2397,
      26,    -4,   -40,  -313,    44,   -63,  2052,  2397,  2771,  2771,
    -313,  -313,  -313,  -313,  -313,  -313,  -313,  -313,  -313,  -313,
    -313,  -313,  -313,  -313,  -313,  -313,  -313,  -313,   -61,  -313,
    -313,  -313,  -313,  -313,  -313,  -313,  -313,  -313,  2397,  1015,
      54,  -313,   -57,  -313,  -313,  -313,  -313,  -313,  -313,  -313,
    -313,  -313,  -313,   -40,   -55,  -313,  -313,  -313,   400,  -313,
      24,    24,    24,    24,   -39,  -313,  -313,  -313,  2397,  2397,
    2167,  -313,   179,    40,  -313,    41,  -313,   -38,  2512,  -313,
    -313,  -313,   -35,  -313,    24,  2397,  2397,  2397,   -25,  2397,
    1131,  3326,  -313,  -313,  -313,    24,    24,    24,    24,  -313,
    -313,    -5,  3250,  -313,  1362,  3326,   -38,  1937,  2627,     3,
      11,   -38,  1477,  -313,    59,    -6,   503,  -313,  3326,   -41,
     -35,    31,    31,  2771,  2970,  -313,    -2,  -313,  1247,  -313,
      58,   109,  2771,  2771,  2771,  2771,  2771,  2771,  2771,  2803,
    -313,  -313,  -313,  -313,  2397,    70,  3326,  -313,    75,     8,
    1937,  1937,   777,    63,  2397,    78,    15,  -313,  -313,  -313,
      20,    20,    20,  2397,  2999,  -313,    19,  2691,  2397,  2397,
    2397,  2397,  2397,  2397,  2397,  2397,  2397,  2397,  2397,  2397,
    2397,  2397,  2397,  2397,  2397,  2397,  2397,  2397,  2397,  2397,
    2397,  2397,  2397,  2397,  2397,  2627,  2397,  -313,  -313,  -313,
    -313,   136,  2397,  -313,   -40,    30,   -49,  -313,   -40,  -313,
    -313,  2397,  2397,  -313,  2627,   -40,    29,    32,  -313,  -313,
    2397,  -313,   140,  -313,  2282,   -17,    24,  -313,  -313,  2397,
      33,   -34,    87,    87,    31,    31,    31,    31,  -313,  2865,
    -313,  2397,    37,  -313,  -313,    38,    52,    99,  -313,  2397,
     137,  2512,  -313,  2897,    24,  -313,   140,  -313,  3361,  3399,
    3399,  2618,  2618,   767,   889,   889,   889,   889,   889,   889,
     387,   387,   387,   592,   592,   592,   617,     5,     5,    20,
      20,    20,    20,    20,  -313,    14,   -38,  3326,   114,   210,
    -313,  -313,   149,   899,   157,  3326,  3326,  -313,   157,  -313,
    1477,  -313,  3326,  -313,   173,   178,   110,  -313,   162,   119,
    -313,  -313,  2771,  -313,  3326,  1477,  -313,  2397,  3326,  2397,
    -313,  -313,  1477,  -313,  3326,  1592,  -313,  2397,  -313,   118,
     -40,    -3,  2397,  -313,  2397,   170,  -313,  -313,  2397,  -313,
    -313,  -313,  -313,  2397,   140,  -313,  2397,   242,  -313,   233,
     138,  3072,  3101,  -313,  -313,  3174,  -313,   157,  2397,  2397,
    3326,  3326,  2397,  3326,  3326,  -313,  3326,  -313,  -313,   -40,
    -313,  -313,  -313,  -313,  3204,  3288,  3326,   157,  -313,  2397,
    -313,  3326
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -313,  -313,  -313,  -313,  -313,   103,  -313,  -313,  -313,  -107,
    -313,  -313,  -313,  -313,  -313,  -312,   -98,   -83,  -313,  -105,
    -102,  -313,  -313,     1,  -313,  -313,   -62,   -89,    42,   -22,
    -313,    89,    95,   -12,  -104,    23,  -313,  -313,   -18,  -313,
    -313,   -97,  -313,  -313,     2,   107,  -313,   276,   343,  -313,
    -313,  -313,   384,  -313,   588,  -313,  -313,   -45,   -21,  -313
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -54
static const yytype_int16 yytable[] =
{
     111,    74,    80,   122,   134,   137,   360,   125,    80,    80,
     243,    98,   188,    88,   136,   121,    80,   223,   378,   379,
     123,   188,   224,    97,   230,    88,   124,   188,   228,   127,
      80,    80,   126,   235,   231,   232,   177,   233,    89,   153,
     154,   128,   155,   156,   157,   158,   144,   159,    94,    96,
     131,   132,   150,   143,   147,   148,   153,   154,   151,   155,
     156,   157,   158,   310,   159,   393,   165,   311,   168,   312,
     268,   272,    88,   170,   171,   164,   172,   166,   244,    90,
     210,   211,   212,   213,   214,   400,   215,   216,   186,   183,
     -53,    90,   234,   180,   181,   182,   216,   184,   187,   332,
      80,   215,   216,   160,   161,   162,   163,    81,   221,   239,
     240,   304,   159,    81,    81,   250,   247,   141,   142,   249,
     260,    81,   261,   262,   269,   178,    80,   179,   270,    80,
     317,   271,   321,   238,    80,    81,    81,   275,   217,   218,
     219,   220,   306,   309,   319,    80,   323,   335,   320,    74,
      80,   339,   259,   340,    80,    80,    80,    80,    80,    80,
      80,    80,   343,   155,   156,   157,   158,   341,   159,   229,
     345,   273,    80,    80,    80,   352,   277,   278,   279,   280,
     281,   282,   283,   284,   285,   286,   287,   288,   289,   290,
     291,   292,   293,   294,   295,   296,   297,   298,   299,   300,
     301,   302,   303,   308,   305,    81,   356,   314,   350,   346,
     307,   224,   263,   264,   318,   342,   353,   354,   358,   315,
     316,   365,   331,   362,   368,   363,   364,    88,   322,   -53,
     366,    81,   245,   268,    81,   367,   376,   334,   382,    81,
     373,   251,   252,   253,   254,   255,   256,   257,   387,   338,
      81,   248,   370,   389,   258,    81,   374,   344,   349,    81,
      81,    81,    81,    81,    81,    81,    81,   385,   357,   333,
       0,     0,     0,    80,     0,     0,    82,    81,    81,    81,
       0,     0,    82,    82,     0,     0,     0,     0,     0,     0,
      82,     0,   -53,    90,   -53,   -53,     0,   348,     0,     0,
       0,     0,     0,     0,    82,    82,   153,   154,     0,   155,
     156,   157,   158,     0,   159,    80,     0,     0,     0,     0,
       0,   361,    80,     0,     0,   371,     0,   372,     0,   377,
       0,     0,     0,     0,    80,   375,     0,    80,     0,     0,
     380,     0,   381,    83,    80,     0,   383,    80,   388,    83,
      83,   384,     0,     0,   386,     0,     0,    83,     0,     0,
       0,     0,     0,     0,     0,     0,   394,   395,   397,     0,
     396,    83,    83,     0,    82,     0,     0,     0,    81,     0,
       0,     0,     0,     0,    86,     0,     0,   401,     0,     0,
      86,    86,     0,     0,   188,     0,     0,     0,    86,     0,
      82,     0,     0,    82,     0,     0,     0,     0,    82,     0,
       0,     0,    86,    86,     0,     0,     0,     0,     0,    82,
      81,   369,     0,     0,    82,     0,     0,    81,    82,    82,
      82,    82,    82,    82,    82,    82,     0,     0,     0,    81,
       0,    83,    81,     0,     0,     0,    82,    82,    82,    81,
       0,     0,    81,   -54,   -54,   -54,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,    83,   215,   216,
      83,     0,   152,   153,   154,    83,   155,   156,   157,   158,
       0,   159,    86,     0,     0,     0,    83,     0,     0,     0,
       0,    83,     0,     0,     0,    83,    83,    83,    83,    83,
      83,    83,    83,     0,     0,     0,     0,     0,    86,     0,
     188,    86,     0,    83,    83,    83,    86,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    86,     0,     0,
       0,     0,    86,     0,     0,     0,    86,    86,    86,    86,
      86,    86,    86,    86,     0,     0,     0,    82,     0,     0,
       0,     0,     0,     0,    86,    86,    86,   189,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,     0,   215,   216,     0,     0,    87,    82,
       0,     0,     0,     0,    87,    87,    82,     0,     0,   188,
       0,     0,    87,     0,     0,     0,     0,     0,    82,     0,
       0,    82,     0,     0,    83,     0,    87,    87,    82,     0,
       0,    82,   242,     0,   188,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    86,    83,     0,     0,     0,
       0,     0,     0,    83,   207,   208,   209,   210,   211,   212,
     213,   214,     0,   215,   216,    83,     0,     0,    83,     0,
       0,     0,     0,     0,     0,    83,    87,     0,    83,   -54,
     208,   209,   210,   211,   212,   213,   214,    86,   215,   216,
       0,     0,     0,     0,    86,     0,     0,     0,     0,     0,
       0,     0,    87,     0,     0,    87,    86,     0,     0,    86,
      87,     0,     0,     0,     0,     0,    86,     0,     0,    86,
       0,    87,     0,     0,     0,     0,    87,     0,     0,     0,
      87,    87,    87,    87,    87,    87,    87,    87,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    87,    87,
      87,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   188,     0,     0,     0,   265,     0,
       1,     2,     3,    92,     0,     5,     0,     6,     7,     0,
       8,     0,     0,    10,    11,     0,     0,     0,     0,     0,
       0,     0,    13,     0,    15,     0,    17,    18,     0,     0,
       0,     0,     0,     0,     0,    21,     0,    23,     0,     0,
      25,     0,     0,     0,     0,    26,    27,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,     0,   215,   216,
      28,    29,     0,     0,     0,     0,     0,     0,     0,    87,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,     0,    57,     0,
       0,    58,     0,     0,    59,     0,   188,     0,     0,     0,
       0,    87,     1,     2,     3,    92,     0,     5,    87,     6,
       7,     0,     8,     0,     0,    10,    11,     0,    12,     0,
      87,     0,     0,    87,    13,     0,    15,     0,    17,    18,
      87,     0,     0,    87,     0,     0,     0,    21,     0,    23,
       0,     0,    25,     0,     0,     0,     0,    26,    27,   -54,
     -54,   -54,   -54,   -54,   -54,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,     0,
     215,   216,    28,    29,     0,     0,     0,     0,     0,     0,
       0,     0,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,     0,
      57,     0,     0,    58,     0,     0,    59,   355,    99,   100,
     101,   102,     0,   103,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    15,     0,     0,    18,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     104,     0,     0,    26,    27,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   105,   106,
       0,     0,     0,     0,     0,   107,     0,     0,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,   108,    49,    50,    51,
      52,    53,    54,    55,    56,     0,    57,     0,     0,   109,
       0,     0,   110,   145,    99,   100,   101,   102,     0,   103,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    15,     0,
       0,    18,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   104,     0,     0,    26,
      27,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   105,   106,     0,     0,     0,     0,
       0,   107,     0,     0,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,   108,    49,    50,    51,    52,    53,    54,    55,
      56,     0,    57,     0,     0,   109,     0,     0,   110,   185,
       1,     2,     3,     4,     0,     5,     0,     6,     7,     0,
       8,     9,     0,    10,    11,     0,    12,     0,     0,     0,
       0,     0,    13,    14,    15,    16,    17,    18,     0,     0,
       0,     0,    19,    20,     0,    21,    22,    23,    24,     0,
      25,     0,     0,     0,     0,    26,    27,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      28,    29,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,     0,    57,     0,
       0,    58,     0,     0,    59,     1,     2,     3,    92,     0,
       5,     0,     6,     7,     0,     8,     0,     0,    10,    11,
       0,    12,     0,     0,     0,     0,     0,    13,     0,    15,
       0,    17,    18,     0,     0,     0,     0,     0,     0,     0,
      21,     0,    23,     0,     0,    25,     0,     0,     0,     0,
      26,    27,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    28,    29,     0,     0,     0,
       0,     0,     0,     0,     0,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,     0,    57,     0,     0,    58,     0,     0,    59,
       1,     2,     3,    92,     0,     5,     0,     6,     7,     0,
       8,     0,     0,    10,    11,     0,     0,     0,     0,     0,
       0,     0,    13,     0,    15,     0,    17,    18,     0,     0,
       0,     0,     0,     0,     0,    21,     0,    23,     0,     0,
      25,     0,     0,     0,     0,    26,    27,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      28,    29,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,     0,    57,     0,
       0,    58,     0,     0,    59,     1,     2,     3,    92,     0,
       5,     0,     6,     7,     0,     8,     0,     0,     0,    11,
       0,     0,     0,     0,     0,     0,     0,    13,     0,    15,
       0,    17,    18,     0,     0,     0,     0,     0,     0,     0,
      21,     0,    23,     0,     0,    25,     0,     0,     0,     0,
      26,    27,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    28,    29,     0,     0,     0,
       0,     0,     0,     0,     0,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,     0,    57,     0,     0,    58,     0,     0,    59,
       1,     2,     3,    92,     0,     5,     0,     0,     0,     0,
       8,     0,     0,     0,    11,     0,     0,     0,     0,     0,
       0,     0,    13,     0,    15,     0,    17,    18,     0,     0,
       0,     0,     0,     0,     0,    93,     0,    23,     0,     0,
       0,     0,     0,     0,     0,    26,    27,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      28,    29,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,     0,    57,     0,
       0,    58,     0,     0,    59,     1,     2,     3,    92,     0,
       5,     0,     0,     0,     0,     8,     0,     0,     0,    11,
       0,     0,     0,     0,     0,     0,     0,    13,     0,    15,
       0,    17,    18,     0,     0,     0,     0,     0,     0,     0,
      95,     0,    23,     0,     0,     0,     0,     0,     0,     0,
      26,    27,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    28,    29,     0,     0,     0,
       0,     0,     0,     0,     0,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,     0,    57,     0,     0,    58,     0,     0,    59,
       1,     2,     3,    92,     0,     5,     0,     0,     0,     0,
       8,     0,     0,     0,    11,     0,     0,     0,     0,     0,
       0,     0,    13,     0,    15,     0,    17,    18,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    23,     0,     0,
       0,     0,     0,     0,     0,    26,    27,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      28,    29,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,     0,    57,     0,
       0,    58,     0,     0,    59,    99,   100,   101,   102,     0,
     103,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    15,
       0,     0,    18,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   104,     0,     0,
      26,    27,   133,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   105,   106,     0,     0,     0,
       0,     0,   107,     0,     0,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,   108,    49,    50,    51,    52,    53,    54,
      55,    56,     0,    57,     0,     0,   109,     0,     0,   110,
      99,   100,   101,   102,     0,   103,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    15,     0,     0,    18,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   104,     0,     0,    26,    27,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     105,   106,     0,     0,     0,     0,     0,   107,     0,     0,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,   108,    49,
      50,    51,    52,    53,    54,    55,    56,     0,    57,     0,
       0,   109,   167,     0,   110,    99,   100,   101,   102,     0,
     103,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    15,
       0,     0,    18,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   104,     0,     0,
      26,    27,     0,   330,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   105,   106,     0,     0,     0,
       0,     0,   107,     0,     0,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,   108,    49,    50,    51,    52,    53,    54,
      55,    56,     0,    57,     0,     0,   109,     0,     0,   110,
      99,   100,   101,   102,     0,   103,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    15,     0,     0,    18,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   104,     0,     0,    26,    27,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     105,   106,     0,     0,     0,     0,     0,   107,     0,     0,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,   108,    49,
      50,    51,    52,    53,    54,    55,    56,     0,    57,     0,
       0,   109,     0,     0,   110,     1,     2,     3,    92,     0,
       5,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    15,
       0,    17,    18,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      26,    27,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    28,    29,     0,     0,     0,
       0,     0,     0,     0,     0,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,     0,    57,     0,   188,    58,     0,     0,   174,
      99,   100,   101,   102,     0,   103,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    15,     0,     0,    18,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   104,     0,     0,    26,    27,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,   188,   215,
     216,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,     0,    49,
      50,    51,    52,    53,    54,    55,    56,     0,    57,     0,
       0,   109,     0,     0,   110,   189,   190,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,     0,   215,   216,     1,     2,     3,   140,     0,     5,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    15,     0,
       0,    18,     0,     0,     0,     0,     1,     2,     3,   140,
     276,     5,     0,     0,     0,     0,     0,     0,     0,    26,
      27,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      15,     0,     0,    18,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    28,    29,     0,     0,     0,     0,
       0,    26,    27,     0,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,   188,    49,    50,    51,    52,    53,    54,    55,
      56,     0,    57,     0,     0,    58,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,   188,    49,    50,    51,    52,    53,
      54,    55,    56,     0,    57,     0,     0,    58,     0,   189,
     190,   191,   192,   193,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,     0,   215,   216,     0,     0,
       0,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   188,   215,   216,
     336,   337,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   188,     0,     0,     0,
       0,     0,     0,   347,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
       0,   215,   216,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   188,
     215,   216,     0,     0,     0,   246,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   188,     0,
       0,     0,     0,     0,   274,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,     0,   215,   216,   189,   190,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   188,   215,   216,     0,     0,     0,   390,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   188,     0,     0,     0,     0,   391,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   398,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,     0,   215,   216,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,     0,   215,   216,     0,     0,   392,
       0,     0,   222,     0,     0,   188,     0,     0,     0,     0,
       0,     0,     0,     0,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     399,   215,   216,   188,     0,     0,     0,     0,     0,     0,
       0,     0,   189,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,   188,   215,
     216,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     189,   190,   191,   192,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   188,   215,   216,     0,
       0,     0,     0,     0,     0,     0,   190,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,     0,   215,   216,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,     0,
     215,   216
};

static const yytype_int16 yycheck[] =
{
      12,     0,     0,    15,    26,    27,   318,    19,     6,     7,
      51,    48,     7,    48,    26,    14,    14,   124,    21,    22,
       8,     7,   124,     6,   128,    48,   117,     7,   126,    33,
      28,    29,     6,   131,    31,    32,    98,    34,    61,    73,
      74,    81,    76,    77,    78,    79,    58,    81,     6,     7,
       6,   114,    73,   114,     0,   112,    73,    74,   113,    76,
      77,    78,    79,   112,    81,   377,    88,   116,    90,   118,
     172,   176,    48,    33,    33,   114,   114,    89,   119,   114,
      75,    76,    77,    78,    79,   397,    81,    82,   110,   114,
     113,   114,    81,   105,   106,   107,    82,   109,   110,   116,
      98,    81,    82,    80,    81,    82,    83,     0,   113,    50,
     116,   215,    81,     6,     7,     6,   118,    28,    29,    61,
      50,    14,    47,   115,    61,   102,   124,   104,    50,   127,
     234,   116,   237,   132,   132,    28,    29,   118,   115,   116,
     117,   118,     6,   113,   115,   143,     6,   114,   116,   148,
     148,   114,   164,   115,   152,   153,   154,   155,   156,   157,
     158,   159,   267,    76,    77,    78,    79,   115,    81,   127,
      33,   183,   170,   171,   172,    61,   188,   189,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   224,   216,    98,   313,   228,   306,   271,
     222,   313,   170,   171,   235,   116,     6,    68,    61,   231,
     232,   326,   244,    50,   329,    47,   116,    48,   240,    50,
      68,   124,   143,   335,   127,   116,   118,   249,    68,   132,
     342,   152,   153,   154,   155,   156,   157,   158,     6,   261,
     143,   148,   335,   115,   159,   148,   345,   269,   276,   152,
     153,   154,   155,   156,   157,   158,   159,   364,   313,   246,
      -1,    -1,    -1,   271,    -1,    -1,     0,   170,   171,   172,
      -1,    -1,     6,     7,    -1,    -1,    -1,    -1,    -1,    -1,
      14,    -1,   113,   114,   115,   116,    -1,   274,    -1,    -1,
      -1,    -1,    -1,    -1,    28,    29,    73,    74,    -1,    76,
      77,    78,    79,    -1,    81,   313,    -1,    -1,    -1,    -1,
      -1,   320,   320,    -1,    -1,   337,    -1,   339,    -1,   350,
      -1,    -1,    -1,    -1,   332,   347,    -1,   335,    -1,    -1,
     352,    -1,   354,     0,   342,    -1,   358,   345,   115,     6,
       7,   363,    -1,    -1,   366,    -1,    -1,    14,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   378,   379,   389,    -1,
     382,    28,    29,    -1,    98,    -1,    -1,    -1,   271,    -1,
      -1,    -1,    -1,    -1,     0,    -1,    -1,   399,    -1,    -1,
       6,     7,    -1,    -1,     7,    -1,    -1,    -1,    14,    -1,
     124,    -1,    -1,   127,    -1,    -1,    -1,    -1,   132,    -1,
      -1,    -1,    28,    29,    -1,    -1,    -1,    -1,    -1,   143,
     313,   332,    -1,    -1,   148,    -1,    -1,   320,   152,   153,
     154,   155,   156,   157,   158,   159,    -1,    -1,    -1,   332,
      -1,    98,   335,    -1,    -1,    -1,   170,   171,   172,   342,
      -1,    -1,   345,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,   124,    81,    82,
     127,    -1,    72,    73,    74,   132,    76,    77,    78,    79,
      -1,    81,    98,    -1,    -1,    -1,   143,    -1,    -1,    -1,
      -1,   148,    -1,    -1,    -1,   152,   153,   154,   155,   156,
     157,   158,   159,    -1,    -1,    -1,    -1,    -1,   124,    -1,
       7,   127,    -1,   170,   171,   172,   132,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   143,    -1,    -1,
      -1,    -1,   148,    -1,    -1,    -1,   152,   153,   154,   155,
     156,   157,   158,   159,    -1,    -1,    -1,   271,    -1,    -1,
      -1,    -1,    -1,    -1,   170,   171,   172,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    -1,    81,    82,    -1,    -1,     0,   313,
      -1,    -1,    -1,    -1,     6,     7,   320,    -1,    -1,     7,
      -1,    -1,    14,    -1,    -1,    -1,    -1,    -1,   332,    -1,
      -1,   335,    -1,    -1,   271,    -1,    28,    29,   342,    -1,
      -1,   345,   119,    -1,     7,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   271,   313,    -1,    -1,    -1,
      -1,    -1,    -1,   320,    72,    73,    74,    75,    76,    77,
      78,    79,    -1,    81,    82,   332,    -1,    -1,   335,    -1,
      -1,    -1,    -1,    -1,    -1,   342,    98,    -1,   345,    72,
      73,    74,    75,    76,    77,    78,    79,   313,    81,    82,
      -1,    -1,    -1,    -1,   320,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   124,    -1,    -1,   127,   332,    -1,    -1,   335,
     132,    -1,    -1,    -1,    -1,    -1,   342,    -1,    -1,   345,
      -1,   143,    -1,    -1,    -1,    -1,   148,    -1,    -1,    -1,
     152,   153,   154,   155,   156,   157,   158,   159,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   170,   171,
     172,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,     1,    -1,
       3,     4,     5,     6,    -1,     8,    -1,    10,    11,    -1,
      13,    -1,    -1,    16,    17,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    25,    -1,    27,    -1,    29,    30,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    38,    -1,    40,    -1,    -1,
      43,    -1,    -1,    -1,    -1,    48,    49,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    -1,    81,    82,
      73,    74,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   271,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,    -1,   111,    -1,
      -1,   114,    -1,    -1,   117,    -1,     7,    -1,    -1,    -1,
      -1,   313,     3,     4,     5,     6,    -1,     8,   320,    10,
      11,    -1,    13,    -1,    -1,    16,    17,    -1,    19,    -1,
     332,    -1,    -1,   335,    25,    -1,    27,    -1,    29,    30,
     342,    -1,    -1,   345,    -1,    -1,    -1,    38,    -1,    40,
      -1,    -1,    43,    -1,    -1,    -1,    -1,    48,    49,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    -1,
      81,    82,    73,    74,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,    -1,
     111,    -1,    -1,   114,    -1,    -1,   117,   118,     3,     4,
       5,     6,    -1,     8,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    27,    -1,    -1,    30,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,
      -1,    -1,    -1,    -1,    -1,    80,    -1,    -1,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,    -1,   111,    -1,    -1,   114,
      -1,    -1,   117,   118,     3,     4,     5,     6,    -1,     8,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,
      -1,    30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    73,    74,    -1,    -1,    -1,    -1,
      -1,    80,    -1,    -1,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,    -1,   111,    -1,    -1,   114,    -1,    -1,   117,   118,
       3,     4,     5,     6,    -1,     8,    -1,    10,    11,    -1,
      13,    14,    -1,    16,    17,    -1,    19,    -1,    -1,    -1,
      -1,    -1,    25,    26,    27,    28,    29,    30,    -1,    -1,
      -1,    -1,    35,    36,    -1,    38,    39,    40,    41,    -1,
      43,    -1,    -1,    -1,    -1,    48,    49,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      73,    74,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,    -1,   111,    -1,
      -1,   114,    -1,    -1,   117,     3,     4,     5,     6,    -1,
       8,    -1,    10,    11,    -1,    13,    -1,    -1,    16,    17,
      -1,    19,    -1,    -1,    -1,    -1,    -1,    25,    -1,    27,
      -1,    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      38,    -1,    40,    -1,    -1,    43,    -1,    -1,    -1,    -1,
      48,    49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    73,    74,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,    -1,   111,    -1,    -1,   114,    -1,    -1,   117,
       3,     4,     5,     6,    -1,     8,    -1,    10,    11,    -1,
      13,    -1,    -1,    16,    17,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    25,    -1,    27,    -1,    29,    30,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    38,    -1,    40,    -1,    -1,
      43,    -1,    -1,    -1,    -1,    48,    49,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      73,    74,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,    -1,   111,    -1,
      -1,   114,    -1,    -1,   117,     3,     4,     5,     6,    -1,
       8,    -1,    10,    11,    -1,    13,    -1,    -1,    -1,    17,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    25,    -1,    27,
      -1,    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      38,    -1,    40,    -1,    -1,    43,    -1,    -1,    -1,    -1,
      48,    49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    73,    74,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,    -1,   111,    -1,    -1,   114,    -1,    -1,   117,
       3,     4,     5,     6,    -1,     8,    -1,    -1,    -1,    -1,
      13,    -1,    -1,    -1,    17,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    25,    -1,    27,    -1,    29,    30,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    38,    -1,    40,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    48,    49,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      73,    74,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,    -1,   111,    -1,
      -1,   114,    -1,    -1,   117,     3,     4,     5,     6,    -1,
       8,    -1,    -1,    -1,    -1,    13,    -1,    -1,    -1,    17,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    25,    -1,    27,
      -1,    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      38,    -1,    40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    73,    74,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,    -1,   111,    -1,    -1,   114,    -1,    -1,   117,
       3,     4,     5,     6,    -1,     8,    -1,    -1,    -1,    -1,
      13,    -1,    -1,    -1,    17,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    25,    -1,    27,    -1,    29,    30,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    48,    49,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      73,    74,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,    -1,   111,    -1,
      -1,   114,    -1,    -1,   117,     3,     4,     5,     6,    -1,
       8,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,
      -1,    -1,    30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
      48,    49,    50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    73,    74,    -1,    -1,    -1,
      -1,    -1,    80,    -1,    -1,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,    -1,   111,    -1,    -1,   114,    -1,    -1,   117,
       3,     4,     5,     6,    -1,     8,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    27,    -1,    -1,    30,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    49,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      73,    74,    -1,    -1,    -1,    -1,    -1,    80,    -1,    -1,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,    -1,   111,    -1,
      -1,   114,   115,    -1,   117,     3,     4,     5,     6,    -1,
       8,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,
      -1,    -1,    30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
      48,    49,    -1,    51,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    73,    74,    -1,    -1,    -1,
      -1,    -1,    80,    -1,    -1,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,    -1,   111,    -1,    -1,   114,    -1,    -1,   117,
       3,     4,     5,     6,    -1,     8,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    27,    -1,    -1,    30,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    49,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      73,    74,    -1,    -1,    -1,    -1,    -1,    80,    -1,    -1,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,    -1,   111,    -1,
      -1,   114,    -1,    -1,   117,     3,     4,     5,     6,    -1,
       8,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,
      -1,    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    73,    74,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,    -1,   111,    -1,     7,   114,    -1,    -1,   117,
       3,     4,     5,     6,    -1,     8,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    27,    -1,    -1,    30,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    49,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,     7,    81,
      82,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,    -1,   102,
     103,   104,   105,   106,   107,   108,   109,    -1,   111,    -1,
      -1,   114,    -1,    -1,   117,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    -1,    81,    82,     3,     4,     5,     6,    -1,     8,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,
      -1,    30,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
     119,     8,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,
      49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      27,    -1,    -1,    30,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    73,    74,    -1,    -1,    -1,    -1,
      -1,    48,    49,    -1,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,     7,   102,   103,   104,   105,   106,   107,   108,
     109,    -1,   111,    -1,    -1,   114,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,     7,   102,   103,   104,   105,   106,
     107,   108,   109,    -1,   111,    -1,    -1,   114,    -1,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    -1,    81,    82,    -1,    -1,
      -1,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,     7,    81,    82,
     115,   116,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,
      -1,    -1,    -1,   116,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      -1,    81,    82,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,     7,
      81,    82,    -1,    -1,    -1,   115,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,
      -1,    -1,    -1,    -1,   115,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    -1,    81,    82,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,     7,    81,    82,    -1,    -1,    -1,   115,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     7,    -1,    -1,    -1,    -1,   115,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    23,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    -1,    81,    82,     7,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    -1,    81,    82,    -1,    -1,   115,
      -1,    -1,    42,    -1,    -1,     7,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      42,    81,    82,     7,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,     7,    81,
      82,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,     7,    81,    82,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    -1,    81,    82,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    -1,
      81,    82
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     8,    10,    11,    13,    14,
      16,    17,    19,    25,    26,    27,    28,    29,    30,    35,
      36,    38,    39,    40,    41,    43,    48,    49,    73,    74,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   111,   114,   117,
     121,   122,   123,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   140,   143,   146,   147,   148,   151,   152,
     164,   165,   167,   168,   170,   171,   172,   174,    48,    61,
     114,   155,     6,    38,   148,    38,   148,     6,    48,     3,
       4,     5,     6,     8,    45,    73,    74,    80,   101,   114,
     117,   153,   154,   156,   157,   164,   165,   167,   168,   172,
     174,   143,   153,     8,   117,   153,     6,    33,    81,   178,
     179,     6,   114,    50,   149,   150,   153,   149,   153,   166,
       6,   151,   151,   114,   153,   118,   149,     0,   112,   124,
     178,   113,    72,    73,    74,    76,    77,    78,    79,    81,
     155,   155,   155,   155,   114,   149,   153,   115,   149,   173,
      33,    33,   114,   136,   117,   144,   145,   146,   155,   155,
     153,   153,   153,   114,   153,   118,   149,   153,     7,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    81,    82,   155,   155,   155,
     155,   113,    42,   129,   140,   143,   175,   177,   136,   148,
     154,    31,    32,    34,    81,   136,   141,   142,   143,    50,
     116,   139,   119,    51,   119,   151,   115,   118,   125,    61,
       6,   151,   151,   151,   151,   151,   151,   151,   152,   153,
      50,    47,   115,   148,   148,     1,   137,   138,   140,    61,
      50,   116,   139,   153,   115,   118,   119,   153,   153,   153,
     153,   153,   153,   153,   153,   153,   153,   153,   153,   153,
     153,   153,   153,   153,   153,   153,   153,   153,   153,   153,
     153,   153,   153,   153,   154,   153,     6,   153,   178,   113,
     112,   116,   118,   176,   178,   153,   153,   154,   178,   115,
     116,   139,   153,     6,   158,   159,   160,   161,   162,   163,
      51,   149,   116,   155,   153,   114,   115,   116,   153,   114,
     115,   115,   116,   139,   153,    33,   146,   116,   155,   158,
     136,   169,    61,     6,    68,   118,   129,   177,    61,   135,
     135,   143,    50,    47,   116,   139,    68,   116,   139,   151,
     137,   153,   153,   140,   147,   153,   118,   178,    21,    22,
     153,   153,    68,   153,   153,   161,   153,     6,   115,   115,
     115,   115,   115,   135,   153,   153,   153,   178,    23,    42,
     135,   153
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
# define YYLEX yylex (&yylval, &yylloc)
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
#line 249 "lib/parser.yxx"
{
  yylloc.filename = static_cast<ParserState*>(parm)->model->filepath();
}
/* Line 1078 of yacc.c.  */
#line 2634 "lib/parser.tab.cpp"
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
#line 375 "lib/parser.yxx"
    {
        ParserState* pp = static_cast<ParserState*>(parm);
        pp->model->addItem((yyvsp[(1) - (1)].item));
      ;}
    break;

  case 6:
#line 380 "lib/parser.yxx"
    {
        ParserState* pp = static_cast<ParserState*>(parm);
        pp->model->addItem((yyvsp[(3) - (3)].item));
      ;}
    break;

  case 9:
#line 389 "lib/parser.yxx"
    { (yyval.item)=notInDatafile(&(yyloc),parm,"include") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 10:
#line 391 "lib/parser.yxx"
    { (yyval.item)=notInDatafile(&(yyloc),parm,"variable declaration") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 12:
#line 394 "lib/parser.yxx"
    { (yyval.item)=notInDatafile(&(yyloc),parm,"constraint") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 13:
#line 396 "lib/parser.yxx"
    { (yyval.item)=notInDatafile(&(yyloc),parm,"solve") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 14:
#line 398 "lib/parser.yxx"
    { (yyval.item)=notInDatafile(&(yyloc),parm,"output") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 15:
#line 400 "lib/parser.yxx"
    { (yyval.item)=notInDatafile(&(yyloc),parm,"predicate") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 16:
#line 402 "lib/parser.yxx"
    { (yyval.item)=notInDatafile(&(yyloc),parm,"predicate") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 17:
#line 404 "lib/parser.yxx"
    { (yyval.item)=notInDatafile(&(yyloc),parm,"annotation") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 18:
#line 408 "lib/parser.yxx"
    { ParserState* pp = static_cast<ParserState*>(parm);
        map<string,Model*>::iterator ret = pp->seenModels.find((yyvsp[(2) - (2)].sValue));
        IncludeI* ii = IncludeI::a(pp->ctx,(yyloc),(yyvsp[(2) - (2)].sValue));
        (yyval.item) = ii;
        if (ret == pp->seenModels.end()) {
          Model* im = new Model;
          im->setParent(pp->model);
          im->setFilename(pp->ctx,(yyvsp[(2) - (2)].sValue));
          string fpath, fbase; filepath(pp->filename, fpath, fbase);
          if (fpath=="")
            fpath="./";
          pair<string,Model*> pm(fpath, im);
          pp->files.push_back(pm);
          ii->setModel(im);
          pp->seenModels.insert(pair<string,Model*>((yyvsp[(2) - (2)].sValue),im));
        } else {
          ii->setModel(ret->second, false);
        }
        free((yyvsp[(2) - (2)].sValue));
      ;}
    break;

  case 19:
#line 431 "lib/parser.yxx"
    { (yyvsp[(1) - (2)].vardeclexpr)->annotate((yyvsp[(2) - (2)].annotation));
        (yyval.item) = VarDeclI::a(GETCTX(),(yyloc),(yyvsp[(1) - (2)].vardeclexpr));
      ;}
    break;

  case 20:
#line 435 "lib/parser.yxx"
    { (yyvsp[(1) - (4)].vardeclexpr)->_e = (yyvsp[(4) - (4)].expression);
        (yyvsp[(1) - (4)].vardeclexpr)->annotate((yyvsp[(2) - (4)].annotation));
        (yyval.item) = VarDeclI::a(GETCTX(),(yyloc),(yyvsp[(1) - (4)].vardeclexpr));
      ;}
    break;

  case 21:
#line 442 "lib/parser.yxx"
    { (yyval.item) = AssignI::a(GETCTX(),(yyloc),(yyvsp[(1) - (3)].sValue),(yyvsp[(3) - (3)].expression));
        free((yyvsp[(1) - (3)].sValue));
      ;}
    break;

  case 22:
#line 448 "lib/parser.yxx"
    { (yyval.item) = ConstraintI::a(GETCTX(),(yyloc),(yyvsp[(2) - (2)].expression));;}
    break;

  case 23:
#line 452 "lib/parser.yxx"
    { (yyval.item) = SolveI::sat(GETCTX(),(yyloc),(yyvsp[(2) - (3)].annotation)); ;}
    break;

  case 24:
#line 454 "lib/parser.yxx"
    { (yyval.item) = SolveI::min(GETCTX(),(yyloc),(yyvsp[(4) - (4)].expression), (yyvsp[(2) - (4)].annotation)); ;}
    break;

  case 25:
#line 456 "lib/parser.yxx"
    { (yyval.item) = SolveI::max(GETCTX(),(yyloc),(yyvsp[(4) - (4)].expression), (yyvsp[(2) - (4)].annotation)); ;}
    break;

  case 26:
#line 460 "lib/parser.yxx"
    { (yyval.item) = OutputI::a(GETCTX(),(yyloc),(yyvsp[(2) - (2)].expression));;}
    break;

  case 27:
#line 464 "lib/parser.yxx"
    { (yyval.item) = PredicateI::a(GETCTX(),(yyloc),(yyvsp[(2) - (5)].sValue),*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression),(yyvsp[(4) - (5)].annotation),false);
        free((yyvsp[(2) - (5)].sValue));
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
      ;}
    break;

  case 28:
#line 469 "lib/parser.yxx"
    { (yyval.item) = PredicateI::a(GETCTX(),(yyloc),(yyvsp[(2) - (5)].sValue),*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression),(yyvsp[(4) - (5)].annotation),true);
        free((yyvsp[(2) - (5)].sValue));
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
      ;}
    break;

  case 29:
#line 476 "lib/parser.yxx"
    { (yyval.item) = FunctionI::a(GETCTX(),(yyloc),(yyvsp[(4) - (7)].sValue),(yyvsp[(2) - (7)].tiexpr),*(yyvsp[(5) - (7)].vardeclexpr_v),(yyvsp[(7) - (7)].expression),(yyvsp[(6) - (7)].annotation));
        free((yyvsp[(4) - (7)].sValue));
        delete (yyvsp[(5) - (7)].vardeclexpr_v);
      ;}
    break;

  case 30:
#line 481 "lib/parser.yxx"
    { (yyval.item) = FunctionI::a(GETCTX(),(yyloc),(yyvsp[(3) - (8)].sValue),(yyvsp[(1) - (8)].tiexpr),*(yyvsp[(5) - (8)].vardeclexpr_v),(yyvsp[(8) - (8)].expression),(yyvsp[(7) - (8)].annotation));
        free((yyvsp[(3) - (8)].sValue));
        delete (yyvsp[(5) - (8)].vardeclexpr_v);
      ;}
    break;

  case 31:
#line 488 "lib/parser.yxx"
    {
        TiExpr* ti=TiExpr::par(GETCTX(),(yylsp[(1) - (3)]),AnnTiExpr::a(GETCTX()));
        (yyval.item) = FunctionI::a(GETCTX(),(yyloc),(yyvsp[(2) - (3)].sValue),ti,*(yyvsp[(3) - (3)].vardeclexpr_v),NULL,NULL);
        free((yyvsp[(2) - (3)].sValue));
        delete (yyvsp[(3) - (3)].vardeclexpr_v);
      ;}
    break;

  case 32:
#line 495 "lib/parser.yxx"
    { TiExpr* ti=TiExpr::par(GETCTX(),(yylsp[(1) - (5)]),AnnTiExpr::a(GETCTX()));
        (yyval.item) = FunctionI::a(GETCTX(),(yyloc),(yyvsp[(2) - (5)].sValue),ti,*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression),NULL);
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
      ;}
    break;

  case 33:
#line 502 "lib/parser.yxx"
    { (yyval.expression)=NULL; ;}
    break;

  case 34:
#line 504 "lib/parser.yxx"
    { (yyval.expression)=(yyvsp[(2) - (2)].expression); ;}
    break;

  case 35:
#line 508 "lib/parser.yxx"
    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); ;}
    break;

  case 36:
#line 510 "lib/parser.yxx"
    { (yyval.vardeclexpr_v)=(yyvsp[(2) - (3)].vardeclexpr_v); ;}
    break;

  case 37:
#line 512 "lib/parser.yxx"
    { (yyval.vardeclexpr_v)=NULL; ;}
    break;

  case 38:
#line 515 "lib/parser.yxx"
    { (yyval.vardeclexpr_v)=(yyvsp[(1) - (2)].vardeclexpr_v); ;}
    break;

  case 39:
#line 519 "lib/parser.yxx"
    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); (yyval.vardeclexpr_v)->push_back((yyvsp[(1) - (1)].vardeclexpr)); ;}
    break;

  case 40:
#line 521 "lib/parser.yxx"
    { (yyval.vardeclexpr_v)=(yyvsp[(1) - (3)].vardeclexpr_v); (yyvsp[(1) - (3)].vardeclexpr_v)->push_back((yyvsp[(3) - (3)].vardeclexpr)); ;}
    break;

  case 43:
#line 527 "lib/parser.yxx"
    { (yyval.vardeclexpr) = VarDecl::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].tiexpr), (yyvsp[(3) - (3)].sValue));
        free((yyvsp[(3) - (3)].sValue));
      ;}
    break;

  case 44:
#line 532 "lib/parser.yxx"
    { (yyval.tiexpr_v)=(yyvsp[(1) - (2)].tiexpr_v); ;}
    break;

  case 45:
#line 536 "lib/parser.yxx"
    { (yyval.tiexpr_v)=new vector<TiExpr*>(); (yyval.tiexpr_v)->push_back((yyvsp[(1) - (1)].tiexpr)); ;}
    break;

  case 46:
#line 538 "lib/parser.yxx"
    { (yyval.tiexpr_v)=(yyvsp[(1) - (3)].tiexpr_v); (yyvsp[(1) - (3)].tiexpr_v)->push_back((yyvsp[(3) - (3)].tiexpr)); ;}
    break;

  case 48:
#line 543 "lib/parser.yxx"
    {
        (yyval.tiexpr) = (yyvsp[(6) - (6)].tiexpr);
        (yyval.tiexpr)->addRanges(GETCTX(),*(yyvsp[(3) - (6)].inttiexpr_v));
        delete (yyvsp[(3) - (6)].inttiexpr_v);
      ;}
    break;

  case 50:
#line 553 "lib/parser.yxx"
    { (yyval.inttiexpr_v)=new std::vector<IntTiExpr*>; (yyval.inttiexpr_v)->push_back((yyvsp[(1) - (1)].inttiexpr)); ;}
    break;

  case 51:
#line 555 "lib/parser.yxx"
    { (yyval.inttiexpr_v)=(yyvsp[(1) - (3)].inttiexpr_v); (yyval.inttiexpr_v)->push_back((yyvsp[(3) - (3)].inttiexpr)); ;}
    break;

  case 52:
#line 559 "lib/parser.yxx"
    { (yyval.inttiexpr) = IntTiExpr::a(GETCTX()); ;}
    break;

  case 53:
#line 561 "lib/parser.yxx"
    { (yyval.inttiexpr) = IntTiExpr::a(GETCTX(),Id::a(GETCTX(),(yylsp[(1) - (1)]),(yyvsp[(1) - (1)].sValue),NULL)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 54:
#line 563 "lib/parser.yxx"
    { (yyval.inttiexpr) = IntTiExpr::a(GETCTX(),SetLit::a(GETCTX(),(yyloc),(yyvsp[(1) - (3)].expression),(yyvsp[(3) - (3)].expression))); ;}
    break;

  case 55:
#line 565 "lib/parser.yxx"
    { (yyval.inttiexpr) = IntTiExpr::a(GETCTX(),SetLit::a(GETCTX(),(yyloc),(yyvsp[(3) - (6)].expression),(yyvsp[(5) - (6)].expression))); ;}
    break;

  case 56:
#line 567 "lib/parser.yxx"
    { (yyval.inttiexpr) = IntTiExpr::a(GETCTX(),SetLit::a(GETCTX(),(yylsp[(2) - (3)]),*(yyvsp[(2) - (3)].expression_v)));
        delete (yyvsp[(2) - (3)].expression_v);
      ;}
    break;

  case 57:
#line 573 "lib/parser.yxx"
    { (yyval.tiexpr) = TiExpr::par(GETCTX(),(yyloc),(yyvsp[(1) - (1)].basetiexpr)); ;}
    break;

  case 58:
#line 575 "lib/parser.yxx"
    { (yyval.tiexpr) = TiExpr::par(GETCTX(),(yyloc),(yyvsp[(2) - (2)].basetiexpr)); ;}
    break;

  case 59:
#line 577 "lib/parser.yxx"
    { (yyval.tiexpr) = TiExpr::var(GETCTX(),(yyloc),(yyvsp[(2) - (2)].basetiexpr)); ;}
    break;

  case 60:
#line 579 "lib/parser.yxx"
    { (yyval.tiexpr) = TiExpr::parset(GETCTX(),(yyloc),(yyvsp[(3) - (3)].basetiexpr)); ;}
    break;

  case 61:
#line 581 "lib/parser.yxx"
    { (yyval.tiexpr) = TiExpr::parset(GETCTX(),(yyloc),(yyvsp[(4) - (4)].basetiexpr)); ;}
    break;

  case 62:
#line 583 "lib/parser.yxx"
    { (yyval.tiexpr) = TiExpr::varset(GETCTX(),(yyloc),(yyvsp[(4) - (4)].basetiexpr)); ;}
    break;

  case 63:
#line 585 "lib/parser.yxx"
    { (yyval.tiexpr) = NULL; ;}
    break;

  case 64:
#line 589 "lib/parser.yxx"
    { (yyval.basetiexpr) = BoolTiExpr::a(GETCTX()); ;}
    break;

  case 65:
#line 591 "lib/parser.yxx"
    { (yyval.basetiexpr) = FloatTiExpr::a(GETCTX()); ;}
    break;

  case 66:
#line 593 "lib/parser.yxx"
    { (yyval.basetiexpr) = StringTiExpr::a(GETCTX()); ;}
    break;

  case 67:
#line 595 "lib/parser.yxx"
    { (yyval.basetiexpr) = AnnTiExpr::a(GETCTX()); ;}
    break;

  case 68:
#line 597 "lib/parser.yxx"
    { (yyval.basetiexpr) = IntTiExpr::a(GETCTX()); ;}
    break;

  case 69:
#line 599 "lib/parser.yxx"
    { (yyval.basetiexpr) = (yyvsp[(1) - (1)].inttiexpr); ;}
    break;

  case 71:
#line 605 "lib/parser.yxx"
    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].expression)); ;}
    break;

  case 72:
#line 607 "lib/parser.yxx"
    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); (yyval.expression_v)->push_back((yyvsp[(3) - (3)].expression)); ;}
    break;

  case 74:
#line 614 "lib/parser.yxx"
    { (yyvsp[(1) - (3)].expression)->annotate(Annotation::a(GETCTX(),(yylsp[(3) - (3)]),(yyvsp[(3) - (3)].expression))); (yyval.expression)=(yyvsp[(1) - (3)].expression); ;}
    break;

  case 75:
#line 616 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 76:
#line 618 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MINUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 77:
#line 620 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MULT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 78:
#line 622 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_DIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 79:
#line 624 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_IDIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 80:
#line 626 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MOD, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 81:
#line 628 "lib/parser.yxx"
    { (yyval.expression)=UnOp::a(GETCTX(),(yyloc), UOT_PLUS, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 82:
#line 630 "lib/parser.yxx"
    { (yyval.expression)=UnOp::a(GETCTX(),(yyloc), UOT_MINUS, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 83:
#line 634 "lib/parser.yxx"
    { (yyval.expression)=(yyvsp[(2) - (3)].expression); ;}
    break;

  case 84:
#line 636 "lib/parser.yxx"
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), (yyvsp[(2) - (4)].expression), *(yyvsp[(4) - (4)].expression_v)); delete (yyvsp[(4) - (4)].expression_v); ;}
    break;

  case 85:
#line 638 "lib/parser.yxx"
    { (yyval.expression)=Id::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].sValue), NULL);
        free((yyvsp[(1) - (1)].sValue));
      ;}
    break;

  case 86:
#line 642 "lib/parser.yxx"
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), Id::a(GETCTX(),(yylsp[(1) - (2)]),(yyvsp[(1) - (2)].sValue),NULL), *(yyvsp[(2) - (2)].expression_v));
        free((yyvsp[(1) - (2)].sValue)); delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 87:
#line 646 "lib/parser.yxx"
    { (yyval.expression)=BoolLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 88:
#line 648 "lib/parser.yxx"
    { (yyval.expression)=IntLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 89:
#line 650 "lib/parser.yxx"
    { (yyval.expression)=FloatLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].dValue)); ;}
    break;

  case 90:
#line 652 "lib/parser.yxx"
    { (yyval.expression)=StringLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 92:
#line 655 "lib/parser.yxx"
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 94:
#line 659 "lib/parser.yxx"
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 96:
#line 663 "lib/parser.yxx"
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 98:
#line 667 "lib/parser.yxx"
    { (yyval.expression)=ArrayAccess::a(GETCTX(),(yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 102:
#line 677 "lib/parser.yxx"
    { (yyvsp[(1) - (3)].expression)->annotate(Annotation::a(GETCTX(),(yylsp[(3) - (3)]),(yyvsp[(3) - (3)].expression))); (yyval.expression)=(yyvsp[(1) - (3)].expression); ;}
    break;

  case 103:
#line 679 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_EQUIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 104:
#line 681 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_IMPL, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 105:
#line 683 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_RIMPL, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 106:
#line 685 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_OR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 107:
#line 687 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_XOR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 108:
#line 689 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_AND, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 109:
#line 691 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_LE, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 110:
#line 693 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_GR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 111:
#line 695 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_LQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 112:
#line 697 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_GQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 113:
#line 699 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_EQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 114:
#line 701 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_NQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 115:
#line 703 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_IN, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 116:
#line 705 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_SUBSET, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 117:
#line 707 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_SUPERSET, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 118:
#line 709 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_UNION, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 119:
#line 711 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_DIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 120:
#line 713 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_SYMDIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 121:
#line 715 "lib/parser.yxx"
    { (yyval.expression)=SetLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (3)].expression), (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 122:
#line 717 "lib/parser.yxx"
    { (yyval.expression)=SetLit::a(GETCTX(), (yyloc), (yyvsp[(3) - (6)].expression), (yyvsp[(5) - (6)].expression)); ;}
    break;

  case 123:
#line 719 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_INTERSECT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 124:
#line 721 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUSPLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 125:
#line 723 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 126:
#line 725 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MINUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 127:
#line 727 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MULT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 128:
#line 729 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_DIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 129:
#line 731 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_IDIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 130:
#line 733 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MOD, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 131:
#line 735 "lib/parser.yxx"
    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=Call::a(GETCTX(), (yyloc), (yyvsp[(2) - (3)].sValue), args);
        free((yyvsp[(2) - (3)].sValue));
      ;}
    break;

  case 132:
#line 741 "lib/parser.yxx"
    { (yyval.expression)=UnOp::a(GETCTX(), (yyloc), UOT_NOT, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 133:
#line 743 "lib/parser.yxx"
    { (yyval.expression)=UnOp::a(GETCTX(), (yyloc), UOT_PLUS, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 134:
#line 745 "lib/parser.yxx"
    { (yyval.expression)=UnOp::a(GETCTX(), (yyloc), UOT_MINUS, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 135:
#line 749 "lib/parser.yxx"
    { (yyval.expression)=(yyvsp[(2) - (3)].expression); ;}
    break;

  case 136:
#line 751 "lib/parser.yxx"
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), (yyvsp[(2) - (4)].expression), *(yyvsp[(4) - (4)].expression_v)); delete (yyvsp[(4) - (4)].expression_v); ;}
    break;

  case 137:
#line 753 "lib/parser.yxx"
    { (yyval.expression)=Id::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].sValue), NULL); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 138:
#line 755 "lib/parser.yxx"
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), Id::a(GETCTX(), (yylsp[(1) - (2)]),(yyvsp[(1) - (2)].sValue),NULL), *(yyvsp[(2) - (2)].expression_v));
        free((yyvsp[(1) - (2)].sValue)); delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 139:
#line 758 "lib/parser.yxx"
    { (yyval.expression)=AnonVar::a(GETCTX(),(yyloc)); ;}
    break;

  case 140:
#line 760 "lib/parser.yxx"
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), AnonVar::a(GETCTX(), (yyloc)), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 141:
#line 763 "lib/parser.yxx"
    { (yyval.expression)=BoolLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 142:
#line 765 "lib/parser.yxx"
    { (yyval.expression)=IntLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 143:
#line 767 "lib/parser.yxx"
    { (yyval.expression)=FloatLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].dValue)); ;}
    break;

  case 144:
#line 769 "lib/parser.yxx"
    { (yyval.expression)=StringLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 148:
#line 774 "lib/parser.yxx"
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 150:
#line 778 "lib/parser.yxx"
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 152:
#line 782 "lib/parser.yxx"
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 154:
#line 786 "lib/parser.yxx"
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 157:
#line 793 "lib/parser.yxx"
    { (yyval.expression_v)=(yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 158:
#line 797 "lib/parser.yxx"
    { (yyval.expression) = SetLit::a(GETCTX(), (yyloc), std::vector<Expression*>()); ;}
    break;

  case 159:
#line 799 "lib/parser.yxx"
    { (yyval.expression) = SetLit::a(GETCTX(), (yyloc), *(yyvsp[(2) - (3)].expression_v)); delete (yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 160:
#line 803 "lib/parser.yxx"
    { (yyval.expression) = Comprehension::a(GETCTX(), (yyloc), (yyvsp[(2) - (5)].expression), *(yyvsp[(4) - (5)].generators), true);
        delete (yyvsp[(4) - (5)].generators);
      ;}
    break;

  case 161:
#line 809 "lib/parser.yxx"
    { (yyval.generators)=new Generators; (yyval.generators)->_g = *(yyvsp[(1) - (1)].generator_v); (yyval.generators)->_w = NULL; delete (yyvsp[(1) - (1)].generator_v); ;}
    break;

  case 162:
#line 811 "lib/parser.yxx"
    { (yyval.generators)=new Generators; (yyval.generators)->_g = *(yyvsp[(1) - (3)].generator_v); (yyval.generators)->_w = (yyvsp[(3) - (3)].expression); delete (yyvsp[(1) - (3)].generator_v); ;}
    break;

  case 164:
#line 817 "lib/parser.yxx"
    { (yyval.generator_v)=new std::vector<Generator*>; (yyval.generator_v)->push_back((yyvsp[(1) - (1)].generator)); ;}
    break;

  case 165:
#line 819 "lib/parser.yxx"
    { (yyval.generator_v)=(yyvsp[(1) - (3)].generator_v); (yyval.generator_v)->push_back((yyvsp[(3) - (3)].generator)); ;}
    break;

  case 166:
#line 823 "lib/parser.yxx"
    { (yyval.generator)=Generator::a(GETCTX(),*(yyvsp[(1) - (3)].string_v),(yyvsp[(3) - (3)].expression)); delete (yyvsp[(1) - (3)].string_v); ;}
    break;

  case 168:
#line 829 "lib/parser.yxx"
    { (yyval.string_v)=new std::vector<std::string>; (yyval.string_v)->push_back((yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 169:
#line 831 "lib/parser.yxx"
    { (yyval.string_v)=(yyvsp[(1) - (3)].string_v); (yyval.string_v)->push_back((yyvsp[(3) - (3)].sValue)); free((yyvsp[(3) - (3)].sValue)); ;}
    break;

  case 170:
#line 835 "lib/parser.yxx"
    { (yyval.expression)=ArrayLit::a(GETCTX(), (yyloc), std::vector<MiniZinc::Expression*>()); ;}
    break;

  case 171:
#line 837 "lib/parser.yxx"
    { (yyval.expression)=ArrayLit::a(GETCTX(), (yyloc), *(yyvsp[(2) - (3)].expression_v)); delete (yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 172:
#line 841 "lib/parser.yxx"
    { (yyval.expression)=ArrayLit::a(GETCTX(), (yyloc), *(yyvsp[(2) - (3)].expression_vv));
        for (unsigned int i=1; i<(yyvsp[(2) - (3)].expression_vv)->size(); i++)
          if ((*(yyvsp[(2) - (3)].expression_vv))[i].size() != (*(yyvsp[(2) - (3)].expression_vv))[i-1].size())
            yyerror(&(yylsp[(2) - (3)]), parm, "syntax error, all sub-arrays of 2d array literal must have the same length");
        delete (yyvsp[(2) - (3)].expression_vv);
      ;}
    break;

  case 173:
#line 848 "lib/parser.yxx"
    { (yyval.expression)=ArrayLit::a(GETCTX(), (yyloc), *(yyvsp[(2) - (4)].expression_vv));
        for (unsigned int i=1; i<(yyvsp[(2) - (4)].expression_vv)->size(); i++)
          if ((*(yyvsp[(2) - (4)].expression_vv))[i].size() != (*(yyvsp[(2) - (4)].expression_vv))[i-1].size())
            yyerror(&(yylsp[(2) - (4)]), parm, "syntax error, all sub-arrays of 2d array literal must have the same length");
        delete (yyvsp[(2) - (4)].expression_vv);
      ;}
    break;

  case 174:
#line 857 "lib/parser.yxx"
    { (yyval.expression_vv)=new std::vector<std::vector<MiniZinc::Expression*> >;
        (yyval.expression_vv)->push_back(*(yyvsp[(1) - (1)].expression_v));
        delete (yyvsp[(1) - (1)].expression_v);
      ;}
    break;

  case 175:
#line 862 "lib/parser.yxx"
    { (yyval.expression_vv)=(yyvsp[(1) - (3)].expression_vv); (yyval.expression_vv)->push_back(*(yyvsp[(3) - (3)].expression_v)); delete (yyvsp[(3) - (3)].expression_v); ;}
    break;

  case 176:
#line 866 "lib/parser.yxx"
    { (yyval.expression)=Comprehension::a(GETCTX(), (yyloc), (yyvsp[(2) - (5)].expression), *(yyvsp[(4) - (5)].generators), false);
        delete (yyvsp[(4) - (5)].generators);
      ;}
    break;

  case 177:
#line 872 "lib/parser.yxx"
    {
        std::vector<ITE::IfThen> iexps;
        iexps.push_back(ITE::IfThen((yyvsp[(2) - (8)].expression),(yyvsp[(4) - (8)].expression)));
        for (unsigned int i=0; i<(yyvsp[(5) - (8)].expression_v)->size(); i+=2) {
          iexps.push_back(ITE::IfThen((*(yyvsp[(5) - (8)].expression_v))[i],(*(yyvsp[(5) - (8)].expression_v))[i+1]));
        }
        (yyval.expression)=ITE::a(GETCTX(), (yyloc), iexps,(yyvsp[(7) - (8)].expression));
        delete (yyvsp[(5) - (8)].expression_v);
      ;}
    break;

  case 178:
#line 883 "lib/parser.yxx"
    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; ;}
    break;

  case 179:
#line 885 "lib/parser.yxx"
    { (yyval.expression_v)=(yyvsp[(1) - (5)].expression_v); (yyval.expression_v)->push_back((yyvsp[(3) - (5)].expression)); (yyval.expression_v)->push_back((yyvsp[(5) - (5)].expression)); ;}
    break;

  case 180:
#line 889 "lib/parser.yxx"
    { (yyval.iValue)=BOT_EQUIV; ;}
    break;

  case 181:
#line 891 "lib/parser.yxx"
    { (yyval.iValue)=BOT_IMPL; ;}
    break;

  case 182:
#line 893 "lib/parser.yxx"
    { (yyval.iValue)=BOT_RIMPL; ;}
    break;

  case 183:
#line 895 "lib/parser.yxx"
    { (yyval.iValue)=BOT_OR; ;}
    break;

  case 184:
#line 897 "lib/parser.yxx"
    { (yyval.iValue)=BOT_XOR; ;}
    break;

  case 185:
#line 899 "lib/parser.yxx"
    { (yyval.iValue)=BOT_AND; ;}
    break;

  case 186:
#line 901 "lib/parser.yxx"
    { (yyval.iValue)=BOT_LE; ;}
    break;

  case 187:
#line 903 "lib/parser.yxx"
    { (yyval.iValue)=BOT_GR; ;}
    break;

  case 188:
#line 905 "lib/parser.yxx"
    { (yyval.iValue)=BOT_LQ; ;}
    break;

  case 189:
#line 907 "lib/parser.yxx"
    { (yyval.iValue)=BOT_GQ; ;}
    break;

  case 190:
#line 909 "lib/parser.yxx"
    { (yyval.iValue)=BOT_EQ; ;}
    break;

  case 191:
#line 911 "lib/parser.yxx"
    { (yyval.iValue)=BOT_NQ; ;}
    break;

  case 192:
#line 913 "lib/parser.yxx"
    { (yyval.iValue)=BOT_IN; ;}
    break;

  case 193:
#line 915 "lib/parser.yxx"
    { (yyval.iValue)=BOT_SUBSET; ;}
    break;

  case 194:
#line 917 "lib/parser.yxx"
    { (yyval.iValue)=BOT_SUPERSET; ;}
    break;

  case 195:
#line 919 "lib/parser.yxx"
    { (yyval.iValue)=BOT_UNION; ;}
    break;

  case 196:
#line 921 "lib/parser.yxx"
    { (yyval.iValue)=BOT_DIFF; ;}
    break;

  case 197:
#line 923 "lib/parser.yxx"
    { (yyval.iValue)=BOT_SYMDIFF; ;}
    break;

  case 198:
#line 925 "lib/parser.yxx"
    { (yyval.iValue)=BOT_PLUS; ;}
    break;

  case 199:
#line 927 "lib/parser.yxx"
    { (yyval.iValue)=BOT_MINUS; ;}
    break;

  case 200:
#line 929 "lib/parser.yxx"
    { (yyval.iValue)=BOT_MULT; ;}
    break;

  case 201:
#line 931 "lib/parser.yxx"
    { (yyval.iValue)=BOT_DIV; ;}
    break;

  case 202:
#line 933 "lib/parser.yxx"
    { (yyval.iValue)=BOT_IDIV; ;}
    break;

  case 203:
#line 935 "lib/parser.yxx"
    { (yyval.iValue)=BOT_MOD; ;}
    break;

  case 204:
#line 937 "lib/parser.yxx"
    { (yyval.iValue)=BOT_INTERSECT; ;}
    break;

  case 205:
#line 939 "lib/parser.yxx"
    { (yyval.iValue)=BOT_PLUSPLUS; ;}
    break;

  case 206:
#line 941 "lib/parser.yxx"
    { (yyval.iValue)=-1; ;}
    break;

  case 207:
#line 945 "lib/parser.yxx"
    { if ((yyvsp[(1) - (6)].iValue)==-1) {
          (yyval.expression)=NULL;
          yyerror(&(yylsp[(3) - (6)]), parm, "syntax error, unary operator with two arguments");
        } else {
          (yyval.expression)=BinOp::a(GETCTX(), (yyloc), (yyvsp[(3) - (6)].expression),static_cast<BinOpType>((yyvsp[(1) - (6)].iValue)),(yyvsp[(5) - (6)].expression));
        }
      ;}
    break;

  case 208:
#line 953 "lib/parser.yxx"
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
        else
          (yyval.expression)=UnOp::a(GETCTX(), (yyloc), static_cast<UnOpType>(uot),(yyvsp[(3) - (4)].expression));
      ;}
    break;

  case 209:
#line 976 "lib/parser.yxx"
    { (yyval.expression)=Call::a(GETCTX(), (yyloc), (yyvsp[(1) - (3)].sValue), std::vector<Expression*>()); free((yyvsp[(1) - (3)].sValue)); ;}
    break;

  case 211:
#line 979 "lib/parser.yxx"
    { 
        if ((yyvsp[(3) - (4)].expression_p)->second) {
          yyerror(&(yylsp[(3) - (4)]), parm, "syntax error, 'where' expression outside generator call");
          (yyval.expression)=NULL;
        } else {
          (yyval.expression)=Call::a(GETCTX(), (yyloc), (yyvsp[(1) - (4)].sValue), (yyvsp[(3) - (4)].expression_p)->first);
        }
        free((yyvsp[(1) - (4)].sValue));
        delete (yyvsp[(3) - (4)].expression_p);
      ;}
    break;

  case 212:
#line 990 "lib/parser.yxx"
    { 
        vector<Generator*> gens;
        vector<string> ids;
        for (unsigned int i=0; i<(yyvsp[(3) - (7)].expression_p)->first.size(); i++) {
          if (Id* id = (yyvsp[(3) - (7)].expression_p)->first[i]->dyn_cast<Id>()) {
            ids.push_back(id->_v);
          } else {
            if (BinOp* boe = (yyvsp[(3) - (7)].expression_p)->first[i]->dyn_cast<BinOp>()) {
              Id* id = boe->_e0->dyn_cast<Id>();
              if (id && boe->_op == BOT_IN) {
                ids.push_back(id->_v);
                gens.push_back(Generator::a(GETCTX(),ids,boe->_e1));
                ids = vector<string>();
              } else {
                yyerror(&(yylsp[(3) - (7)]), parm, "illegal expression in generator call");
              }
            } else {
              yyerror(&(yylsp[(3) - (7)]), parm, "illegal expression in generator call");
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
          Generators g; g._g = gens; g._w = (yyvsp[(3) - (7)].expression_p)->second;
          Comprehension* ac = Comprehension::a(GETCTX(), (yyloc), (yyvsp[(6) - (7)].expression),g,false);
          vector<Expression*> args; args.push_back(ac);
          (yyval.expression)=Call::a(GETCTX(), (yyloc), (yyvsp[(1) - (7)].sValue), args);
        }
        free((yyvsp[(1) - (7)].sValue));
        delete (yyvsp[(3) - (7)].expression_p);
      ;}
    break;

  case 213:
#line 1029 "lib/parser.yxx"
    { (yyval.expression_p)=new pair<vector<Expression*>,Expression*>;
        (yyval.expression_p)->first=*(yyvsp[(1) - (1)].expression_v); (yyval.expression_p)->second=NULL;
        delete (yyvsp[(1) - (1)].expression_v);
      ;}
    break;

  case 214:
#line 1034 "lib/parser.yxx"
    { (yyval.expression_p)=new pair<vector<Expression*>,Expression*>;
        (yyval.expression_p)->first=*(yyvsp[(1) - (3)].expression_v); (yyval.expression_p)->second=(yyvsp[(3) - (3)].expression);
        delete (yyvsp[(1) - (3)].expression_v);
      ;}
    break;

  case 215:
#line 1041 "lib/parser.yxx"
    { (yyval.expression)=Let::a(GETCTX(), (yyloc), *(yyvsp[(3) - (6)].expression_v), (yyvsp[(6) - (6)].expression)); delete (yyvsp[(3) - (6)].expression_v); ;}
    break;

  case 216:
#line 1043 "lib/parser.yxx"
    { (yyval.expression)=Let::a(GETCTX(), (yyloc), *(yyvsp[(3) - (7)].expression_v), (yyvsp[(7) - (7)].expression)); delete (yyvsp[(3) - (7)].expression_v); ;}
    break;

  case 217:
#line 1063 "lib/parser.yxx"
    { (yyval.expression_v)=new vector<Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].vardeclexpr)); ;}
    break;

  case 218:
#line 1065 "lib/parser.yxx"
    { (yyval.expression_v)=new vector<Expression*>;
        ConstraintI* ce = (yyvsp[(1) - (1)].item)->cast<ConstraintI>();
        (yyval.expression_v)->push_back(ce->_e);
        ce->_e=NULL;
      ;}
    break;

  case 219:
#line 1071 "lib/parser.yxx"
    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); (yyval.expression_v)->push_back((yyvsp[(3) - (3)].vardeclexpr)); ;}
    break;

  case 220:
#line 1073 "lib/parser.yxx"
    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v);
        ConstraintI* ce = (yyvsp[(3) - (3)].item)->cast<ConstraintI>();
        (yyval.expression_v)->push_back(ce->_e);
        ce->_e=NULL;
      ;}
    break;

  case 223:
#line 1083 "lib/parser.yxx"
    { (yyval.vardeclexpr) = (yyvsp[(1) - (2)].vardeclexpr);
        (yyval.vardeclexpr)->annotate((yyvsp[(2) - (2)].annotation));
      ;}
    break;

  case 224:
#line 1087 "lib/parser.yxx"
    { (yyvsp[(1) - (4)].vardeclexpr)->_e = (yyvsp[(4) - (4)].expression);
        (yyval.vardeclexpr) = (yyvsp[(1) - (4)].vardeclexpr);
        (yyval.vardeclexpr)->_loc = (yyloc);
        (yyval.vardeclexpr)->annotate((yyvsp[(2) - (4)].annotation));
      ;}
    break;

  case 225:
#line 1095 "lib/parser.yxx"
    { (yyval.annotation)=NULL; ;}
    break;

  case 227:
#line 1100 "lib/parser.yxx"
    { (yyval.annotation)=Annotation::a(GETCTX(), (yylsp[(2) - (2)]), (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 228:
#line 1102 "lib/parser.yxx"
    { (yyval.annotation)=(yyvsp[(1) - (3)].annotation); (yyval.annotation)->merge(Annotation::a(GETCTX(), (yylsp[(3) - (3)]), (yyvsp[(3) - (3)].expression))); ;}
    break;


/* Line 1267 of yacc.c.  */
#line 3981 "lib/parser.tab.cpp"
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



