
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
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
    pp->err << "(included from file " << m->filename().str() << ")" << endl;
  }
  pp->err << "In file " << location->filename->str() << ", line "
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
    model->_filename = CtxStringH(ctx,fileBasename);

    if (!ignoreStdlib) {
      Model* stdlib = new Model;
      stdlib->_filename = CtxStringH(ctx,"stdlib.mzn");
      files.push_back(pair<string,Model*>("./",stdlib));
      seenModels.insert(pair<string,Model*>("stdlib.mzn",stdlib));
      Location stdlibloc;
      stdlibloc.filename=CtxString::a(ctx,filename);
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
      string f(m->_filename.str());
            
      for (Model* p=m->_parent; p; p=p->_parent) {
        if (f == p->_filename.c_str()) {
          err << "Error: cyclic includes: " << std::endl;
          for (Model* pe=m; pe; pe=pe->_parent) {
            err << "  " << pe->_filename.c_str() << std::endl;
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

      m->_filepath = CtxStringH(ctx,fullname);
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



/* Line 189 of yacc.c  */
#line 262 "lib/parser.tab.cpp"

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

/* Line 214 of yacc.c  */
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
       


/* Line 214 of yacc.c  */
#line 429 "lib/parser.tab.cpp"
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


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 454 "lib/parser.tab.cpp"

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
# if YYENABLE_NLS
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
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
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
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  149
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   3429

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  120
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  60
/* YYNRULES -- Number of rules.  */
#define YYNRULES  230
/* YYNRULES -- Number of states.  */
#define YYNSTATES  400

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
     128,   132,   133,   135,   139,   141,   148,   151,   153,   157,
     159,   161,   163,   165,   169,   176,   180,   182,   185,   188,
     192,   197,   202,   204,   206,   208,   210,   212,   215,   217,
     219,   221,   224,   227,   229,   233,   235,   239,   243,   247,
     251,   255,   259,   263,   266,   269,   273,   278,   280,   283,
     285,   287,   289,   291,   293,   296,   298,   301,   303,   306,
     308,   311,   313,   315,   317,   321,   325,   329,   333,   337,
     341,   345,   349,   353,   357,   361,   365,   369,   373,   377,
     381,   385,   389,   393,   397,   404,   408,   412,   416,   420,
     424,   428,   432,   436,   440,   443,   446,   449,   453,   458,
     460,   463,   465,   468,   470,   472,   474,   476,   478,   480,
     482,   485,   487,   490,   492,   495,   497,   500,   502,   504,
     508,   511,   515,   521,   523,   527,   530,   532,   536,   540,
     543,   545,   549,   552,   556,   560,   565,   567,   571,   577,
     586,   587,   593,   595,   597,   599,   601,   603,   605,   607,
     609,   611,   613,   615,   617,   619,   621,   623,   625,   627,
     629,   631,   633,   635,   637,   639,   641,   643,   645,   647,
     654,   659,   663,   665,   670,   678,   680,   684,   691,   699,
     701,   703,   707,   711,   713,   715,   718,   723,   724,   726,
     729
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
     178,   135,    -1,    26,   141,   113,     6,   136,   178,   135,
      -1,   141,   113,     6,   114,   137,   115,   178,   135,    -1,
      14,     6,   136,    -1,    14,     6,   136,    61,   153,    -1,
      -1,    61,   153,    -1,    -1,   114,   137,   115,    -1,   114,
       1,   115,    -1,   138,   139,    -1,   140,    -1,   138,   116,
     140,    -1,    -1,   116,    -1,   141,   113,     6,    -1,   146,
      -1,    16,    48,   142,    50,    33,   146,    -1,   143,   139,
      -1,   144,    -1,   143,   116,   144,    -1,   145,    -1,    29,
      -1,     9,    -1,     6,    -1,   151,    72,   151,    -1,   101,
     114,   151,   116,   151,   115,    -1,   117,   149,   118,    -1,
     147,    -1,    11,   147,    -1,    10,   147,    -1,    38,    33,
     147,    -1,    11,    38,    33,   147,    -1,    10,    38,    33,
     147,    -1,    29,    -1,    17,    -1,    25,    -1,    40,    -1,
      13,    -1,   117,   118,    -1,   145,    -1,   148,    -1,     9,
      -1,    15,     9,    -1,   150,   139,    -1,   153,    -1,   150,
     116,   153,    -1,   152,    -1,   151,    81,   152,    -1,   151,
      74,   151,    -1,   151,    73,   151,    -1,   151,    79,   151,
      -1,   151,    78,   151,    -1,   151,    77,   151,    -1,   151,
      76,   151,    -1,    74,   151,    -1,    73,   151,    -1,   114,
     153,   115,    -1,   114,   153,   115,   155,    -1,     6,    -1,
       6,   155,    -1,     4,    -1,     3,    -1,     5,    -1,     8,
      -1,   164,    -1,   164,   155,    -1,   165,    -1,   165,   155,
      -1,   167,    -1,   167,   155,    -1,   168,    -1,   168,   155,
      -1,   174,    -1,   172,    -1,   154,    -1,   153,    81,   154,
      -1,   153,    54,   153,    -1,   153,    56,   153,    -1,   153,
      55,   153,    -1,   153,    58,   153,    -1,   153,    57,   153,
      -1,   153,    59,   153,    -1,   153,    65,   153,    -1,   153,
      64,   153,    -1,   153,    63,   153,    -1,   153,    62,   153,
      -1,   153,    61,   153,    -1,   153,    60,   153,    -1,   153,
      68,   153,    -1,   153,    67,   153,    -1,   153,    66,   153,
      -1,   153,    71,   153,    -1,   153,    70,   153,    -1,   153,
      69,   153,    -1,   153,    72,   153,    -1,   101,   114,   153,
     116,   153,   115,    -1,   153,    75,   153,    -1,   153,    82,
     153,    -1,   153,    74,   153,    -1,   153,    73,   153,    -1,
     153,    79,   153,    -1,   153,    78,   153,    -1,   153,    77,
     153,    -1,   153,    76,   153,    -1,   153,     7,   153,    -1,
      80,   153,    -1,    74,   153,    -1,    73,   153,    -1,   114,
     153,   115,    -1,   114,   153,   115,   155,    -1,     6,    -1,
       6,   155,    -1,    45,    -1,    45,   155,    -1,     4,    -1,
       3,    -1,     5,    -1,     8,    -1,   156,    -1,   157,    -1,
     164,    -1,   164,   155,    -1,   165,    -1,   165,   155,    -1,
     167,    -1,   167,   155,    -1,   168,    -1,   168,   155,    -1,
     174,    -1,   172,    -1,    48,   149,    50,    -1,   117,   118,
      -1,   117,   149,   118,    -1,   117,   153,   119,   158,   118,
      -1,   159,    -1,   159,    47,   153,    -1,   160,   139,    -1,
     161,    -1,   160,   116,   161,    -1,   162,    68,   153,    -1,
     163,   139,    -1,     6,    -1,   163,   116,     6,    -1,    48,
      50,    -1,    48,   149,    50,    -1,    49,   166,    51,    -1,
      49,   166,   119,    51,    -1,   149,    -1,   166,   119,   149,
      -1,    48,   153,   119,   158,    50,    -1,    27,   153,    42,
     153,   169,    21,   153,    23,    -1,    -1,   169,    22,   153,
      42,   153,    -1,    83,    -1,    84,    -1,    85,    -1,    86,
      -1,    87,    -1,    88,    -1,    89,    -1,    90,    -1,    91,
      -1,    92,    -1,    93,    -1,    94,    -1,    95,    -1,    96,
      -1,    97,    -1,    98,    -1,    99,    -1,   100,    -1,   102,
      -1,   103,    -1,   104,    -1,   105,    -1,   106,    -1,   107,
      -1,   108,    -1,   111,    -1,   109,    -1,   170,   114,   153,
     116,   153,   115,    -1,   170,   114,   153,   115,    -1,     6,
     114,   115,    -1,   171,    -1,     6,   114,   173,   115,    -1,
       6,   114,   173,   115,   114,   153,   115,    -1,   149,    -1,
     149,    47,   153,    -1,    30,   117,   175,   118,    68,   153,
      -1,    30,   117,   175,   176,   118,    68,   153,    -1,   177,
      -1,   129,    -1,   175,   176,   177,    -1,   175,   176,   129,
      -1,   116,    -1,   112,    -1,   140,   178,    -1,   140,   178,
      61,   153,    -1,    -1,   179,    -1,    81,   154,    -1,   179,
      81,   154,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   364,   364,   366,   368,   371,   376,   382,   382,   385,
     387,   389,   390,   392,   394,   396,   398,   400,   404,   427,
     431,   438,   444,   448,   450,   452,   456,   460,   466,   474,
     479,   486,   493,   501,   502,   507,   508,   510,   513,   517,
     519,   522,   522,   525,   540,   541,   548,   551,   553,   556,
     558,   560,   564,   566,   568,   570,   576,   578,   580,   582,
     584,   586,   594,   596,   598,   600,   602,   604,   608,   610,
     613,   618,   624,   627,   629,   635,   636,   638,   640,   642,
     644,   646,   648,   650,   652,   656,   658,   660,   664,   668,
     670,   672,   674,   676,   677,   680,   681,   684,   685,   688,
     689,   692,   693,   698,   699,   701,   703,   705,   707,   709,
     711,   713,   715,   717,   719,   721,   723,   725,   727,   729,
     731,   733,   735,   737,   739,   741,   743,   745,   747,   749,
     751,   753,   755,   757,   763,   765,   767,   771,   773,   775,
     777,   780,   782,   785,   787,   789,   791,   793,   794,   795,
     796,   799,   800,   803,   804,   807,   808,   811,   812,   815,
     819,   821,   825,   831,   833,   836,   839,   841,   845,   848,
     851,   853,   857,   859,   863,   870,   879,   884,   888,   894,
     906,   907,   911,   913,   915,   917,   919,   921,   923,   925,
     927,   929,   931,   933,   935,   937,   939,   941,   943,   945,
     947,   949,   951,   953,   955,   957,   959,   961,   963,   967,
     975,   998,  1000,  1001,  1012,  1051,  1056,  1063,  1065,  1085,
    1087,  1093,  1095,  1102,  1102,  1105,  1109,  1118,  1119,  1122,
    1124
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
  "comma_or_none", "ti_expr_and_id", "ti_expr", "int_ti_expr_list",
  "int_ti_expr_list_head", "int_ti_expr", "num_ti_expr", "base_ti_expr",
  "base_ti_expr_tail", "ti_variable_expr_tail", "expr_list",
  "expr_list_head", "num_expr", "num_expr_atom_head", "expr",
  "expr_atom_head", "array_access_tail", "set_literal", "set_comp",
  "comp_tail", "generator_list", "generator_list_head", "generator",
  "id_list", "id_list_head", "simple_array_literal",
  "simple_array_literal_2d", "simple_array_literal_2d_list",
  "simple_array_comp", "if_then_else_expr", "elseif_list", "quoted_op",
  "quoted_op_call", "call_expr", "comp_or_expr", "let_expr",
  "let_vardecl_item_list", "comma_or_semi", "let_vardecl_item",
  "annotations", "ne_annotations", 0
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
     138,   139,   139,   140,   141,   141,   142,   143,   143,   144,
     144,   144,   145,   145,   145,   145,   146,   146,   146,   146,
     146,   146,   147,   147,   147,   147,   147,   147,   147,   147,
     148,   148,   149,   150,   150,   151,   151,   151,   151,   151,
     151,   151,   151,   151,   151,   152,   152,   152,   152,   152,
     152,   152,   152,   152,   152,   152,   152,   152,   152,   152,
     152,   152,   152,   153,   153,   153,   153,   153,   153,   153,
     153,   153,   153,   153,   153,   153,   153,   153,   153,   153,
     153,   153,   153,   153,   153,   153,   153,   153,   153,   153,
     153,   153,   153,   153,   153,   153,   153,   154,   154,   154,
     154,   154,   154,   154,   154,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   154,   154,   154,   154,   154,   155,
     156,   156,   157,   158,   158,   159,   160,   160,   161,   162,
     163,   163,   164,   164,   165,   165,   166,   166,   167,   168,
     169,   169,   170,   170,   170,   170,   170,   170,   170,   170,
     170,   170,   170,   170,   170,   170,   170,   170,   170,   170,
     170,   170,   170,   170,   170,   170,   170,   170,   170,   171,
     171,   172,   172,   172,   172,   173,   173,   174,   174,   175,
     175,   175,   175,   176,   176,   177,   177,   178,   178,   179,
     179
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     3,     0,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       4,     3,     2,     3,     4,     4,     2,     5,     5,     7,
       8,     3,     5,     0,     2,     0,     3,     3,     2,     1,
       3,     0,     1,     3,     1,     6,     2,     1,     3,     1,
       1,     1,     1,     3,     6,     3,     1,     2,     2,     3,
       4,     4,     1,     1,     1,     1,     1,     2,     1,     1,
       1,     2,     2,     1,     3,     1,     3,     3,     3,     3,
       3,     3,     3,     2,     2,     3,     4,     1,     2,     1,
       1,     1,     1,     1,     2,     1,     2,     1,     2,     1,
       2,     1,     1,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     6,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     2,     2,     3,     4,     1,
       2,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       2,     1,     2,     1,     2,     1,     2,     1,     1,     3,
       2,     3,     5,     1,     3,     2,     1,     3,     3,     2,
       1,     3,     2,     3,     3,     4,     1,     3,     5,     8,
       0,     5,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     6,
       4,     3,     1,     4,     7,     1,     3,     6,     7,     1,
       1,     3,     3,     1,     1,     2,     4,     0,     1,     2,
       3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,    90,    89,    91,    87,    92,    70,     0,     0,    66,
       0,     0,     0,    63,     0,    64,     0,     0,     0,    62,
       0,     0,     0,     0,   227,    65,     0,     0,     0,     0,
       0,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,     0,
     200,   201,   202,   203,   204,   205,   206,   208,   207,     0,
       0,     0,     2,     7,     5,     9,    10,    11,    12,    13,
      14,    15,    16,    17,   227,     0,    68,    44,    56,    69,
       0,    75,    93,    95,    97,    99,     0,   212,   102,   101,
       0,     0,     0,    88,    87,     0,    58,     0,    57,    35,
      71,     0,   144,   143,   145,   139,   146,   141,     0,     0,
       0,     0,     0,     0,    22,   103,   147,   148,   149,   151,
     153,   155,   158,   157,     0,     0,    18,     0,    26,    35,
       0,     0,     0,   228,    35,   172,     0,    41,    73,   176,
      73,     0,    87,    84,    83,     0,     0,    67,     0,     1,
       8,     4,    19,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    94,    96,    98,   100,     0,     0,    21,   211,
     215,     0,     0,     0,     0,    31,    51,    50,     0,     0,
      41,    47,    49,   140,   142,   136,   135,   134,     0,     0,
     160,     0,    73,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   150,   152,   154,   156,     0,     0,   220,   227,
       0,     0,   219,   227,    59,   229,     0,     0,    23,     0,
     227,   173,    42,    72,     0,   174,     0,     0,    85,    55,
       6,     0,    43,    53,    78,    77,    82,    81,    80,    79,
      76,     0,   159,     0,   213,    61,    60,     0,     0,    41,
      39,     0,     0,    42,    46,     0,   137,   161,     0,   133,
     105,   107,   106,   109,   108,   110,   116,   115,   114,   113,
     112,   111,   119,   118,   117,   122,   121,   120,   123,   128,
     127,   125,   132,   131,   130,   129,   104,   126,    35,   180,
     225,     0,   224,   223,     0,     0,    33,    25,    24,   230,
      33,    74,   170,     0,   163,    41,   166,     0,    41,   175,
     177,     0,    86,    20,     0,   210,     0,   216,     0,    37,
      36,    42,    38,    32,     0,    48,     0,   138,     0,   227,
       0,     0,    43,     0,     0,   222,   221,     0,    27,    28,
     178,     0,    42,   165,     0,    42,   169,     0,     0,     0,
       0,    40,    45,     0,   162,    33,     0,     0,   226,   217,
       0,    34,   164,   167,   168,   171,    54,   227,   209,   214,
     124,    29,     0,     0,   218,    33,   179,     0,    30,   181
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    61,    62,    63,   151,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,   358,   175,   268,   269,   243,
      74,   230,   179,   180,   181,    76,    77,    78,    79,   148,
     137,    80,    81,   140,   115,    93,   116,   117,   323,   324,
     325,   326,   327,   328,   118,   119,   141,   120,   121,   350,
      86,    87,   122,   171,   123,   231,   315,   232,   132,   133
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -286
static const yytype_int16 yypact[] =
{
    1195,  -286,  -286,  -286,   -39,  -286,  -286,  1655,  1770,  -286,
      13,    12,   -17,  -286,  2345,  -286,  1425,  2345,    21,  -286,
     -80,  2345,    32,    10,   -29,  -286,    47,  2000,  2345,  2719,
    2719,  -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,
    -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,   -52,
    -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,  -286,  2345,
     963,    57,  -286,   -45,  -286,  -286,  -286,  -286,  -286,  -286,
    -286,  -286,  -286,  -286,   -29,   -43,  -286,  -286,  -286,  -286,
     468,  -286,    24,    24,    24,    24,   -46,  -286,  -286,  -286,
    2345,  2345,  2115,  -286,     6,    40,  -286,    43,  -286,   -35,
    -286,  2460,  -286,  -286,  -286,   -23,  -286,    24,  2345,  2345,
    2345,   -34,  2345,  1079,  3274,  -286,  -286,  -286,    24,    24,
      24,    24,  -286,  -286,   -32,  3198,  -286,  1310,  3274,   -35,
    1885,  2575,     8,     5,   -35,  -286,    38,   -27,   232,  -286,
    3274,   -41,   -23,    11,    11,  2719,  2918,  -286,   -25,  -286,
    1195,  -286,    42,    98,  2719,  2719,  2719,  2719,  2719,  2719,
    2719,  2751,  -286,  -286,  -286,  -286,  2345,    59,  3274,  -286,
      60,    -4,  1885,  1885,   732,    51,  -286,  -286,  2345,    63,
       1,  -286,  -286,  -286,  -286,    19,    19,    19,  2345,  2947,
    -286,     0,  2639,  2345,  2345,  2345,  2345,  2345,  2345,  2345,
    2345,  2345,  2345,  2345,  2345,  2345,  2345,  2345,  2345,  2345,
    2345,  2345,  2345,  2345,  2345,  2345,  2345,  2345,  2345,  2345,
    2575,  2345,  -286,  -286,  -286,  -286,   115,  2345,  -286,   -29,
      14,   -88,  -286,   -29,  -286,  -286,  2345,  2345,  -286,  2575,
     -29,  -286,  2345,  -286,   117,  -286,  2230,   298,    24,  -286,
    -286,  2345,    15,   367,   -18,   -18,    11,    11,    11,    11,
    -286,  2813,  -286,  2345,    18,  -286,  -286,    -1,    31,    17,
    -286,  2345,    93,  2460,  -286,  2845,    24,  -286,   117,  -286,
    3309,  3347,  3347,  2566,  2566,   837,   953,   953,   953,   953,
     953,   953,   605,   605,   605,   489,   489,   489,   519,   353,
     353,    19,    19,    19,    19,    19,  -286,    20,   -35,  3274,
      74,   130,  -286,  -286,    79,   847,    87,  3274,  3274,  -286,
      87,  3274,  -286,   100,   104,    39,  -286,    89,    52,  -286,
    -286,  2719,  -286,  3274,  1425,  -286,  2345,  3274,  2345,  -286,
    -286,  1425,  -286,  3274,  1540,  -286,  2345,  -286,    49,   -29,
      29,  2345,  -286,  2345,   101,  -286,  -286,  2345,  -286,  -286,
    -286,  2345,   117,  -286,  2345,   166,  -286,   433,    58,  3020,
    3049,  -286,  -286,  3122,  -286,    87,  2345,  2345,  3274,  3274,
    2345,  3274,  3274,  -286,  3274,  -286,  -286,   -29,  -286,  -286,
    -286,  -286,  3152,  3236,  3274,    87,  -286,  2345,  -286,  3274
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -286,  -286,  -286,  -286,  -286,    25,  -286,  -286,  -286,  -104,
    -286,  -286,  -286,  -286,  -286,  -285,   -93,  -126,  -286,  -172,
    -109,    16,  -286,  -286,   -56,   -57,  -125,    -2,  -286,   -26,
    -286,   -15,    65,   -14,  -114,   -36,  -286,  -286,   -60,  -286,
    -286,  -138,  -286,  -286,     4,   108,  -286,   243,   326,  -286,
    -286,  -286,   378,  -286,   459,  -286,  -286,   -86,   -19,  -286
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -53
static const yytype_int16 yytable[] =
{
     114,   136,   139,   125,    82,    96,    98,   128,   274,    90,
     245,    82,    82,   138,   143,   144,    75,   235,   229,    99,
      82,   100,    91,   228,   312,    90,   193,   193,   313,   126,
     314,   101,   124,    82,    82,   359,   233,   127,   129,   236,
     237,   240,   238,   130,   182,   146,   162,   163,   164,   165,
     376,   377,   131,   134,    90,   152,   -52,   149,   157,   158,
     159,   160,   145,   161,   167,   270,   170,   150,   166,   183,
     153,   184,    90,   172,   -52,    92,   173,   168,   246,   174,
     188,   226,   222,   223,   224,   225,   239,   191,   241,   242,
     391,    92,   161,   249,   185,   186,   187,   342,   189,   192,
     220,   221,   221,   251,   252,    82,   306,   263,    83,   262,
     398,   264,   271,   272,   339,    83,    83,   273,   277,   -52,
      92,   308,   -52,   322,    83,   319,   344,   311,   234,   334,
     247,    82,   338,   341,    82,   351,   352,    83,    83,   253,
     254,   255,   256,   257,   258,   259,   340,   353,   357,    82,
     360,   361,   261,   363,    82,   362,   366,   364,    82,    82,
      82,    82,    82,    82,    82,    82,    75,   374,   365,   380,
     265,   266,   385,   387,   275,   250,    82,    82,    82,   279,
     280,   281,   282,   283,   284,   285,   286,   287,   288,   289,
     290,   291,   292,   293,   294,   295,   296,   297,   298,   299,
     300,   301,   302,   303,   304,   305,   229,   307,   368,    83,
     310,   355,   332,   309,   316,   349,   182,   345,   348,   372,
     330,   320,   317,   318,   383,   270,   260,     0,   321,   356,
       0,     0,   371,     0,     0,    83,     0,   333,    83,   193,
     347,     0,     0,    84,     0,     0,     0,     0,     0,   337,
      84,    84,     0,    83,     0,     0,     0,   343,    83,    84,
       0,     0,    83,    83,    83,    83,    83,    83,    83,    83,
       0,     0,    84,    84,     0,     0,     0,    82,     0,     0,
      83,    83,    83,     0,     0,     0,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,     0,   220,   221,     0,   367,     0,     0,    82,
       0,     0,   369,     0,   370,     0,    85,     0,     0,     0,
     375,     0,   373,    85,    85,    82,     0,   378,    82,   379,
       0,     0,    85,   381,    84,    82,     0,   382,    82,     0,
     384,   244,     0,     0,     0,    85,    85,     0,     0,     0,
     193,     0,   392,   393,     0,     0,   394,     0,   395,     0,
      84,   155,   156,    84,   157,   158,   159,   160,    88,   161,
       0,    83,     0,   399,     0,    88,    88,     0,    84,     0,
       0,     0,     0,    84,    88,     0,     0,    84,    84,    84,
      84,    84,    84,    84,    84,     0,     0,    88,    88,     0,
       0,     0,     0,     0,   331,    84,    84,    84,     0,     0,
       0,     0,     0,    83,     0,     0,     0,    85,   215,   216,
     217,   218,   219,     0,   220,   221,     0,     0,     0,    83,
     155,   156,    83,   157,   158,   159,   160,     0,   161,    83,
       0,     0,    83,    85,     0,     0,    85,     0,     0,    89,
       0,     0,     0,     0,     0,     0,    89,    89,     0,     0,
       0,    85,     0,     0,     0,    89,    85,     0,     0,    88,
      85,    85,    85,    85,    85,    85,    85,    85,    89,    89,
       0,     0,     0,     0,     0,     0,   193,     0,    85,    85,
      85,     0,     0,     0,     0,    88,   155,   156,    88,   157,
     158,   159,   160,     0,   161,     0,    84,     0,     0,     0,
       0,     0,     0,    88,     0,     0,   193,     0,    88,     0,
       0,     0,    88,    88,    88,    88,    88,    88,    88,    88,
     154,   155,   156,     0,   157,   158,   159,   160,   386,   161,
      88,    88,    88,     0,     0,     0,     0,     0,    84,     0,
      89,   212,   213,   214,   215,   216,   217,   218,   219,     0,
     220,   221,     0,     0,    84,     0,     0,    84,     0,     0,
       0,     0,     0,     0,    84,     0,    89,    84,     0,    89,
       0,   -53,   213,   214,   215,   216,   217,   218,   219,    85,
     220,   221,     0,     0,    89,     0,     0,     0,     0,    89,
       0,     0,   193,    89,    89,    89,    89,    89,    89,    89,
      89,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    89,    89,    89,     0,     0,     0,     0,     0,     0,
       0,    85,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    88,     0,     0,     0,     0,     0,    85,     0,     0,
      85,     0,     0,     0,     0,     0,     0,    85,     0,     0,
      85,   -53,   -53,   -53,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,     0,   220,   221,     0,     0,
       0,     0,     0,    88,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    88,
       0,     0,    88,     0,     0,     0,     0,     0,     0,    88,
       0,     0,    88,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    89,   267,     0,     1,     2,     3,    94,     0,
       5,     6,     7,     8,     0,     9,     0,    11,    12,    13,
       0,     0,     0,     0,     0,     0,     0,    15,     0,    17,
       0,    19,    20,     0,     0,     0,     0,     0,     0,     0,
      23,     0,    25,     0,    89,     0,     0,     0,     0,     0,
      27,    28,     0,     0,     0,     0,     0,     0,     0,     0,
      89,     0,     0,    89,     0,     0,     0,     0,     0,     0,
      89,     0,     0,    89,     0,    29,    30,     0,     0,     0,
       0,     0,     0,     0,     0,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,     0,    58,   193,     0,    59,     0,     0,    60,
       1,     2,     3,    94,     0,     5,     6,     7,     8,     0,
       9,     0,    11,    12,    13,     0,    14,     0,     0,     0,
       0,     0,    15,     0,    17,     0,    19,    20,     0,     0,
       0,     0,     0,     0,     0,    23,     0,    25,     0,     0,
       0,     0,     0,     0,     0,    27,    28,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,     0,   220,   221,
      29,    30,     0,     0,     0,     0,     0,     0,     0,     0,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,     0,    58,     0,
     193,    59,     0,     0,    60,   354,   102,   103,   104,   105,
       0,   106,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      17,     0,     0,    20,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   107,     0,
       0,    27,    28,   -53,   -53,   -53,   -53,   -53,   -53,   206,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,     0,   220,   221,   108,   109,     0,     0,
       0,     0,     0,   110,     0,     0,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,   111,    50,    51,    52,    53,    54,
      55,    56,    57,     0,    58,     0,     0,   112,     0,     0,
     113,   147,   102,   103,   104,   105,     0,   106,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    17,     0,     0,    20,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   107,     0,     0,    27,    28,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   108,   109,     0,     0,     0,     0,     0,   110,
       0,     0,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
     111,    50,    51,    52,    53,    54,    55,    56,    57,     0,
      58,     0,     0,   112,     0,     0,   113,   190,     1,     2,
       3,     4,     0,     5,     6,     7,     8,     0,     9,    10,
      11,    12,    13,     0,    14,     0,     0,     0,     0,     0,
      15,    16,    17,    18,    19,    20,     0,     0,     0,     0,
      21,    22,     0,    23,    24,    25,    26,     0,     0,     0,
       0,     0,     0,    27,    28,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    29,    30,
       0,     0,     0,     0,     0,     0,     0,     0,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,     0,    58,     0,     0,    59,
       0,     0,    60,     1,     2,     3,    94,     0,     5,     6,
       7,     8,     0,     9,     0,    11,    12,    13,     0,    14,
       0,     0,     0,     0,     0,    15,     0,    17,     0,    19,
      20,     0,     0,     0,     0,     0,     0,     0,    23,     0,
      25,     0,     0,     0,     0,     0,     0,     0,    27,    28,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    29,    30,     0,     0,     0,     0,     0,
       0,     0,     0,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
       0,    58,     0,     0,    59,     0,     0,    60,     1,     2,
       3,    94,     0,     5,     6,     7,     8,     0,     9,     0,
      11,    12,    13,     0,     0,     0,     0,     0,     0,     0,
      15,     0,    17,     0,    19,    20,     0,     0,     0,     0,
       0,     0,     0,    23,     0,    25,     0,     0,     0,     0,
       0,     0,     0,    27,    28,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    29,    30,
       0,     0,     0,     0,     0,     0,     0,     0,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,     0,    58,     0,     0,    59,
       0,     0,    60,     1,     2,     3,    94,     0,     5,     6,
       7,     8,     0,     9,     0,    11,     0,    13,     0,     0,
       0,     0,     0,     0,     0,    15,     0,    17,     0,    19,
      20,     0,     0,     0,     0,     0,     0,     0,    23,     0,
      25,     0,     0,     0,     0,     0,     0,     0,    27,    28,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    29,    30,     0,     0,     0,     0,     0,
       0,     0,     0,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
       0,    58,     0,     0,    59,     0,     0,    60,     1,     2,
       3,    94,     0,     5,     6,     0,     0,     0,     9,     0,
      11,     0,    13,     0,     0,     0,     0,     0,     0,     0,
      15,     0,    17,     0,    19,    20,     0,     0,     0,     0,
       0,     0,     0,    95,     0,    25,     0,     0,     0,     0,
       0,     0,     0,    27,    28,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    29,    30,
       0,     0,     0,     0,     0,     0,     0,     0,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,     0,    58,     0,     0,    59,
       0,     0,    60,     1,     2,     3,    94,     0,     5,     6,
       0,     0,     0,     9,     0,    11,     0,    13,     0,     0,
       0,     0,     0,     0,     0,    15,     0,    17,     0,    19,
      20,     0,     0,     0,     0,     0,     0,     0,    97,     0,
      25,     0,     0,     0,     0,     0,     0,     0,    27,    28,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    29,    30,     0,     0,     0,     0,     0,
       0,     0,     0,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
       0,    58,     0,     0,    59,     0,     0,    60,     1,     2,
       3,    94,     0,     5,     6,     0,     0,     0,     9,     0,
      11,     0,    13,     0,     0,     0,     0,     0,     0,     0,
      15,     0,    17,     0,    19,    20,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    25,     0,     0,     0,     0,
       0,     0,     0,    27,    28,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    29,    30,
       0,     0,     0,     0,     0,     0,     0,     0,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,     0,    58,     0,     0,    59,
       0,     0,    60,   102,   103,   104,   105,     0,   106,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    17,     0,     0,
      20,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   107,     0,     0,    27,    28,
     135,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   108,   109,     0,     0,     0,     0,     0,
     110,     0,     0,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,   111,    50,    51,    52,    53,    54,    55,    56,    57,
       0,    58,     0,     0,   112,     0,     0,   113,   102,   103,
     104,   105,     0,   106,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    17,     0,     0,    20,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     107,     0,     0,    27,    28,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   108,   109,
       0,     0,     0,     0,     0,   110,     0,     0,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,   111,    50,    51,    52,
      53,    54,    55,    56,    57,     0,    58,     0,     0,   112,
     169,     0,   113,   102,   103,   104,   105,     0,   106,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    17,     0,     0,
      20,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   107,     0,     0,    27,    28,
       0,   329,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   108,   109,     0,     0,     0,     0,     0,
     110,     0,     0,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,   111,    50,    51,    52,    53,    54,    55,    56,    57,
       0,    58,     0,     0,   112,     0,     0,   113,   102,   103,
     104,   105,     0,   106,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    17,     0,     0,    20,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     107,     0,     0,    27,    28,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   108,   109,
       0,     0,     0,     0,     0,   110,     0,     0,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,   111,    50,    51,    52,
      53,    54,    55,    56,    57,     0,    58,     0,     0,   112,
       0,     0,   113,     1,     2,     3,    94,     0,     5,   176,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    17,     0,   177,
      20,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    27,    28,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    29,    30,     0,     0,     0,     0,     0,
       0,     0,     0,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
       0,    58,     0,   193,    59,     0,     0,   178,   102,   103,
     104,   105,     0,   106,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    17,     0,     0,    20,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     107,     0,     0,    27,    28,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   193,   220,   221,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,     0,    50,    51,    52,
      53,    54,    55,    56,    57,     0,    58,     0,     0,   112,
       0,     0,   113,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,     0,
     220,   221,     1,     2,     3,   142,     0,     5,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    17,     0,     0,    20,
       0,     0,     0,     0,     1,     2,     3,   142,   278,     5,
       0,     0,     0,     0,     0,     0,     0,    27,    28,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    17,     0,
       0,    20,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    29,    30,     0,     0,     0,     0,     0,    27,
      28,     0,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
     193,    50,    51,    52,    53,    54,    55,    56,    57,     0,
      58,     0,     0,    59,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,   193,    50,    51,    52,    53,    54,    55,    56,
      57,     0,    58,     0,     0,    59,     0,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,     0,   220,   221,     0,     0,     0,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   193,   220,   221,   335,   336,
<<<<<<< HEAD
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
=======
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
>>>>>>> origin/master
       0,     0,     0,     0,   193,     0,     0,     0,     0,     0,
       0,   346,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,     0,   220,
     221,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   193,   220,   221,
       0,     0,     0,   248,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   193,     0,     0,     0,
       0,     0,   276,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
       0,   220,   221,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   193,
     220,   221,     0,     0,     0,   388,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   193,
       0,     0,     0,     0,   389,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   396,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,     0,   220,   221,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,     0,   220,   221,     0,     0,   390,     0,     0,
     227,     0,     0,   193,     0,     0,     0,     0,     0,     0,
       0,     0,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   397,   220,
     221,   193,     0,     0,     0,     0,     0,     0,     0,     0,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   193,   220,   221,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   193,   220,   221,     0,     0,     0,
       0,     0,     0,     0,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,     0,
     220,   221,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,     0,   220,   221
};

static const yytype_int16 yycheck[] =
{
      14,    27,    28,    17,     0,     7,     8,    21,   180,    48,
      51,     7,     8,    27,    29,    30,     0,   131,   127,     6,
      16,     9,    61,   127,   112,    48,     7,     7,   116,     8,
     118,    48,    16,    29,    30,   320,   129,   117,     6,    31,
      32,   134,    34,    33,   101,    59,    82,    83,    84,    85,
      21,    22,    81,     6,    48,    74,    50,     0,    76,    77,
      78,    79,   114,    81,    90,   174,    92,   112,   114,   105,
     113,   107,    48,    33,   113,   114,    33,    91,   119,   114,
     114,   113,   118,   119,   120,   121,    81,   113,    50,   116,
     375,   114,    81,   118,   108,   109,   110,   269,   112,   113,
      81,    82,    82,    61,     6,   101,   220,    47,     0,    50,
     395,   115,    61,    50,   115,     7,     8,   116,   118,   113,
     114,     6,   116,     6,    16,   239,    33,   113,   130,   114,
     145,   127,   114,   116,   130,    61,     6,    29,    30,   154,
     155,   156,   157,   158,   159,   160,   115,    68,    61,   145,
      50,    47,   166,   325,   150,   116,   328,    68,   154,   155,
     156,   157,   158,   159,   160,   161,   150,   118,   116,    68,
     172,   173,     6,   115,   188,   150,   172,   173,   174,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   315,   221,   334,   101,
     229,   315,   248,   227,   233,   308,   273,   273,   278,   344,
     246,   240,   236,   237,   362,   334,   161,    -1,   242,   315,
      -1,    -1,   341,    -1,    -1,   127,    -1,   251,   130,     7,
     276,    -1,    -1,     0,    -1,    -1,    -1,    -1,    -1,   263,
       7,     8,    -1,   145,    -1,    -1,    -1,   271,   150,    16,
      -1,    -1,   154,   155,   156,   157,   158,   159,   160,   161,
      -1,    -1,    29,    30,    -1,    -1,    -1,   273,    -1,    -1,
     172,   173,   174,    -1,    -1,    -1,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    -1,    81,    82,    -1,   331,    -1,    -1,   315,
      -1,    -1,   336,    -1,   338,    -1,     0,    -1,    -1,    -1,
     349,    -1,   346,     7,     8,   331,    -1,   351,   334,   353,
      -1,    -1,    16,   357,   101,   341,    -1,   361,   344,    -1,
     364,   119,    -1,    -1,    -1,    29,    30,    -1,    -1,    -1,
       7,    -1,   376,   377,    -1,    -1,   380,    -1,   387,    -1,
     127,    73,    74,   130,    76,    77,    78,    79,     0,    81,
      -1,   273,    -1,   397,    -1,     7,     8,    -1,   145,    -1,
      -1,    -1,    -1,   150,    16,    -1,    -1,   154,   155,   156,
     157,   158,   159,   160,   161,    -1,    -1,    29,    30,    -1,
      -1,    -1,    -1,    -1,   116,   172,   173,   174,    -1,    -1,
      -1,    -1,    -1,   315,    -1,    -1,    -1,   101,    75,    76,
      77,    78,    79,    -1,    81,    82,    -1,    -1,    -1,   331,
      73,    74,   334,    76,    77,    78,    79,    -1,    81,   341,
      -1,    -1,   344,   127,    -1,    -1,   130,    -1,    -1,     0,
      -1,    -1,    -1,    -1,    -1,    -1,     7,     8,    -1,    -1,
      -1,   145,    -1,    -1,    -1,    16,   150,    -1,    -1,   101,
     154,   155,   156,   157,   158,   159,   160,   161,    29,    30,
      -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,   172,   173,
     174,    -1,    -1,    -1,    -1,   127,    73,    74,   130,    76,
      77,    78,    79,    -1,    81,    -1,   273,    -1,    -1,    -1,
      -1,    -1,    -1,   145,    -1,    -1,     7,    -1,   150,    -1,
      -1,    -1,   154,   155,   156,   157,   158,   159,   160,   161,
      72,    73,    74,    -1,    76,    77,    78,    79,   115,    81,
     172,   173,   174,    -1,    -1,    -1,    -1,    -1,   315,    -1,
     101,    72,    73,    74,    75,    76,    77,    78,    79,    -1,
      81,    82,    -1,    -1,   331,    -1,    -1,   334,    -1,    -1,
      -1,    -1,    -1,    -1,   341,    -1,   127,   344,    -1,   130,
      -1,    72,    73,    74,    75,    76,    77,    78,    79,   273,
      81,    82,    -1,    -1,   145,    -1,    -1,    -1,    -1,   150,
      -1,    -1,     7,   154,   155,   156,   157,   158,   159,   160,
     161,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   172,   173,   174,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   315,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   273,    -1,    -1,    -1,    -1,    -1,   331,    -1,    -1,
     334,    -1,    -1,    -1,    -1,    -1,    -1,   341,    -1,    -1,
     344,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    -1,    81,    82,    -1,    -1,
      -1,    -1,    -1,   315,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   331,
      -1,    -1,   334,    -1,    -1,    -1,    -1,    -1,    -1,   341,
      -1,    -1,   344,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   273,     1,    -1,     3,     4,     5,     6,    -1,
       8,     9,    10,    11,    -1,    13,    -1,    15,    16,    17,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    25,    -1,    27,
      -1,    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      38,    -1,    40,    -1,   315,    -1,    -1,    -1,    -1,    -1,
      48,    49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     331,    -1,    -1,   334,    -1,    -1,    -1,    -1,    -1,    -1,
     341,    -1,    -1,   344,    -1,    73,    74,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,    -1,   111,     7,    -1,   114,    -1,    -1,   117,
       3,     4,     5,     6,    -1,     8,     9,    10,    11,    -1,
      13,    -1,    15,    16,    17,    -1,    19,    -1,    -1,    -1,
      -1,    -1,    25,    -1,    27,    -1,    29,    30,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    38,    -1,    40,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    48,    49,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    -1,    81,    82,
      73,    74,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,    -1,   111,    -1,
       7,   114,    -1,    -1,   117,   118,     3,     4,     5,     6,
      -1,     8,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      27,    -1,    -1,    30,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    -1,    81,    82,    73,    74,    -1,    -1,
      -1,    -1,    -1,    80,    -1,    -1,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,    -1,   111,    -1,    -1,   114,    -1,    -1,
     117,   118,     3,     4,     5,     6,    -1,     8,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,    30,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    73,    74,    -1,    -1,    -1,    -1,    -1,    80,
      -1,    -1,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,    -1,
     111,    -1,    -1,   114,    -1,    -1,   117,   118,     3,     4,
       5,     6,    -1,     8,     9,    10,    11,    -1,    13,    14,
      15,    16,    17,    -1,    19,    -1,    -1,    -1,    -1,    -1,
      25,    26,    27,    28,    29,    30,    -1,    -1,    -1,    -1,
      35,    36,    -1,    38,    39,    40,    41,    -1,    -1,    -1,
      -1,    -1,    -1,    48,    49,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,    -1,   111,    -1,    -1,   114,
      -1,    -1,   117,     3,     4,     5,     6,    -1,     8,     9,
      10,    11,    -1,    13,    -1,    15,    16,    17,    -1,    19,
      -1,    -1,    -1,    -1,    -1,    25,    -1,    27,    -1,    29,
      30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,    -1,
      40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    49,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    73,    74,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
      -1,   111,    -1,    -1,   114,    -1,    -1,   117,     3,     4,
       5,     6,    -1,     8,     9,    10,    11,    -1,    13,    -1,
      15,    16,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      25,    -1,    27,    -1,    29,    30,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    38,    -1,    40,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    48,    49,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,    -1,   111,    -1,    -1,   114,
      -1,    -1,   117,     3,     4,     5,     6,    -1,     8,     9,
      10,    11,    -1,    13,    -1,    15,    -1,    17,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    25,    -1,    27,    -1,    29,
      30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,    -1,
      40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    49,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    73,    74,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
      -1,   111,    -1,    -1,   114,    -1,    -1,   117,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    -1,    13,    -1,
      15,    -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      25,    -1,    27,    -1,    29,    30,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    38,    -1,    40,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    48,    49,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,    -1,   111,    -1,    -1,   114,
      -1,    -1,   117,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    -1,    13,    -1,    15,    -1,    17,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    25,    -1,    27,    -1,    29,
      30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,    -1,
      40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    49,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    73,    74,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
      -1,   111,    -1,    -1,   114,    -1,    -1,   117,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    -1,    13,    -1,
      15,    -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      25,    -1,    27,    -1,    29,    30,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    40,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    48,    49,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,    -1,   111,    -1,    -1,   114,
      -1,    -1,   117,     3,     4,     5,     6,    -1,     8,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,
      30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    73,    74,    -1,    -1,    -1,    -1,    -1,
      80,    -1,    -1,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
      -1,   111,    -1,    -1,   114,    -1,    -1,   117,     3,     4,
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
     115,    -1,   117,     3,     4,     5,     6,    -1,     8,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,
      30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,
      -1,    51,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    73,    74,    -1,    -1,    -1,    -1,    -1,
      80,    -1,    -1,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
      -1,   111,    -1,    -1,   114,    -1,    -1,   117,     3,     4,
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
      -1,    -1,   117,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,    29,
      30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    49,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    73,    74,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
      -1,   111,    -1,     7,   114,    -1,    -1,   117,     3,     4,
       5,     6,    -1,     8,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    27,    -1,    -1,    30,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,     7,    81,    82,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,    -1,   102,   103,   104,
     105,   106,   107,   108,   109,    -1,   111,    -1,    -1,   114,
      -1,    -1,   117,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    -1,
      81,    82,     3,     4,     5,     6,    -1,     8,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,    30,
      -1,    -1,    -1,    -1,     3,     4,     5,     6,   119,     8,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    49,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,
      -1,    30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    73,    74,    -1,    -1,    -1,    -1,    -1,    48,
      49,    -1,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
       7,   102,   103,   104,   105,   106,   107,   108,   109,    -1,
     111,    -1,    -1,   114,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,     7,   102,   103,   104,   105,   106,   107,   108,
     109,    -1,   111,    -1,    -1,   114,    -1,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    -1,    81,    82,    -1,    -1,    -1,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,     7,    81,    82,   115,   116,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,    -1,
      -1,   116,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    -1,    81,
      82,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,     7,    81,    82,
      -1,    -1,    -1,   115,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,
      -1,    -1,   115,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      -1,    81,    82,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,     7,
      81,    82,    -1,    -1,    -1,   115,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,
      -1,    -1,    -1,    -1,   115,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    23,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    -1,    81,    82,     7,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    -1,    81,    82,    -1,    -1,   115,    -1,    -1,
      42,    -1,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    42,    81,
      82,     7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,     7,    81,    82,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,     7,    81,    82,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    -1,
      81,    82,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    -1,    81,    82
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     8,     9,    10,    11,    13,
      14,    15,    16,    17,    19,    25,    26,    27,    28,    29,
      30,    35,    36,    38,    39,    40,    41,    48,    49,    73,
      74,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   111,   114,
     117,   121,   122,   123,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   140,   141,   145,   146,   147,   148,
     151,   152,   164,   165,   167,   168,   170,   171,   172,   174,
      48,    61,   114,   155,     6,    38,   147,    38,   147,     6,
       9,    48,     3,     4,     5,     6,     8,    45,    73,    74,
      80,   101,   114,   117,   153,   154,   156,   157,   164,   165,
     167,   168,   172,   174,   141,   153,     8,   117,   153,     6,
      33,    81,   178,   179,     6,    50,   149,   150,   153,   149,
     153,   166,     6,   151,   151,   114,   153,   118,   149,     0,
     112,   124,   178,   113,    72,    73,    74,    76,    77,    78,
      79,    81,   155,   155,   155,   155,   114,   149,   153,   115,
     149,   173,    33,    33,   114,   136,     9,    29,   117,   142,
     143,   144,   145,   155,   155,   153,   153,   153,   114,   153,
     118,   149,   153,     7,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      81,    82,   155,   155,   155,   155,   113,    42,   129,   140,
     141,   175,   177,   136,   147,   154,    31,    32,    34,    81,
     136,    50,   116,   139,   119,    51,   119,   151,   115,   118,
     125,    61,     6,   151,   151,   151,   151,   151,   151,   151,
     152,   153,    50,    47,   115,   147,   147,     1,   137,   138,
     140,    61,    50,   116,   139,   153,   115,   118,   119,   153,
     153,   153,   153,   153,   153,   153,   153,   153,   153,   153,
     153,   153,   153,   153,   153,   153,   153,   153,   153,   153,
     153,   153,   153,   153,   153,   153,   154,   153,     6,   153,
     178,   113,   112,   116,   118,   176,   178,   153,   153,   154,
     178,   153,     6,   158,   159,   160,   161,   162,   163,    51,
     149,   116,   155,   153,   114,   115,   116,   153,   114,   115,
     115,   116,   139,   153,    33,   144,   116,   155,   158,   136,
     169,    61,     6,    68,   118,   129,   177,    61,   135,   135,
      50,    47,   116,   139,    68,   116,   139,   151,   137,   153,
     153,   140,   146,   153,   118,   178,    21,    22,   153,   153,
      68,   153,   153,   161,   153,     6,   115,   115,   115,   115,
     115,   135,   153,   153,   153,   178,    23,    42,   135,   153
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
# if YYLTYPE_IS_TRIVIAL
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
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
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
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       , parm);
      YYFPRINTF (stderr, "\n");
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





/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

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
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[2];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

/* User initialization code.  */

/* Line 1242 of yacc.c  */
#line 246 "lib/parser.yxx"
{
  yylloc.filename = static_cast<ParserState*>(parm)->model->filepath().ctxstr();
}

/* Line 1242 of yacc.c  */
#line 2537 "lib/parser.tab.cpp"
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
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
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
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
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

/* Line 1455 of yacc.c  */
#line 372 "lib/parser.yxx"
    {
        ParserState* pp = static_cast<ParserState*>(parm);
        pp->model->addItem((yyvsp[(1) - (1)].item));
      ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 377 "lib/parser.yxx"
    {
        ParserState* pp = static_cast<ParserState*>(parm);
        pp->model->addItem((yyvsp[(3) - (3)].item));
      ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 386 "lib/parser.yxx"
    { (yyval.item)=notInDatafile(&(yyloc),parm,"include") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 388 "lib/parser.yxx"
    { (yyval.item)=notInDatafile(&(yyloc),parm,"variable declaration") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 391 "lib/parser.yxx"
    { (yyval.item)=notInDatafile(&(yyloc),parm,"constraint") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 393 "lib/parser.yxx"
    { (yyval.item)=notInDatafile(&(yyloc),parm,"solve") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 395 "lib/parser.yxx"
    { (yyval.item)=notInDatafile(&(yyloc),parm,"output") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 397 "lib/parser.yxx"
    { (yyval.item)=notInDatafile(&(yyloc),parm,"predicate") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 399 "lib/parser.yxx"
    { (yyval.item)=notInDatafile(&(yyloc),parm,"predicate") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 401 "lib/parser.yxx"
    { (yyval.item)=notInDatafile(&(yyloc),parm,"annotation") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 405 "lib/parser.yxx"
    { ParserState* pp = static_cast<ParserState*>(parm);
        map<string,Model*>::iterator ret = pp->seenModels.find((yyvsp[(2) - (2)].sValue));
        IncludeI* ii = IncludeI::a(pp->ctx,(yyloc),CtxStringH(pp->ctx,(yyvsp[(2) - (2)].sValue)));
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

/* Line 1455 of yacc.c  */
#line 428 "lib/parser.yxx"
    { (yyvsp[(1) - (2)].vardeclexpr)->annotate((yyvsp[(2) - (2)].annotation));
        (yyval.item) = VarDeclI::a(GETCTX(),(yyloc),(yyvsp[(1) - (2)].vardeclexpr));
      ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 432 "lib/parser.yxx"
    { (yyvsp[(1) - (4)].vardeclexpr)->_e = (yyvsp[(4) - (4)].expression);
        (yyvsp[(1) - (4)].vardeclexpr)->annotate((yyvsp[(2) - (4)].annotation));
        (yyval.item) = VarDeclI::a(GETCTX(),(yyloc),(yyvsp[(1) - (4)].vardeclexpr));
      ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 439 "lib/parser.yxx"
    { (yyval.item) = AssignI::a(GETCTX(),(yyloc),(yyvsp[(1) - (3)].sValue),(yyvsp[(3) - (3)].expression));
        free((yyvsp[(1) - (3)].sValue));
      ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 445 "lib/parser.yxx"
    { (yyval.item) = ConstraintI::a(GETCTX(),(yyloc),(yyvsp[(2) - (2)].expression));;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 449 "lib/parser.yxx"
    { (yyval.item) = SolveI::sat(GETCTX(),(yyloc),(yyvsp[(2) - (3)].annotation)); ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 451 "lib/parser.yxx"
    { (yyval.item) = SolveI::min(GETCTX(),(yyloc),(yyvsp[(4) - (4)].expression), (yyvsp[(2) - (4)].annotation)); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 453 "lib/parser.yxx"
    { (yyval.item) = SolveI::max(GETCTX(),(yyloc),(yyvsp[(4) - (4)].expression), (yyvsp[(2) - (4)].annotation)); ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 457 "lib/parser.yxx"
    { (yyval.item) = OutputI::a(GETCTX(),(yyloc),(yyvsp[(2) - (2)].expression));;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 461 "lib/parser.yxx"
    { (yyval.item) = FunctionI::a(GETCTX(),(yyloc),(yyvsp[(2) - (5)].sValue),TypeInst::a(GETCTX(),(yyloc),
                          Type::varbool()),*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression),(yyvsp[(4) - (5)].annotation));
        free((yyvsp[(2) - (5)].sValue));
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
      ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 467 "lib/parser.yxx"
    { (yyval.item) = FunctionI::a(GETCTX(),(yyloc),(yyvsp[(2) - (5)].sValue),TypeInst::a(GETCTX(),(yyloc),
                          Type::parbool()),*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression),(yyvsp[(4) - (5)].annotation));
        free((yyvsp[(2) - (5)].sValue));
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
      ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 475 "lib/parser.yxx"
    { (yyval.item) = FunctionI::a(GETCTX(),(yyloc),(yyvsp[(4) - (7)].sValue),(yyvsp[(2) - (7)].tiexpr),*(yyvsp[(5) - (7)].vardeclexpr_v),(yyvsp[(7) - (7)].expression),(yyvsp[(6) - (7)].annotation));
        free((yyvsp[(4) - (7)].sValue));
        delete (yyvsp[(5) - (7)].vardeclexpr_v);
      ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 480 "lib/parser.yxx"
    { (yyval.item) = FunctionI::a(GETCTX(),(yyloc),(yyvsp[(3) - (8)].sValue),(yyvsp[(1) - (8)].tiexpr),*(yyvsp[(5) - (8)].vardeclexpr_v),(yyvsp[(8) - (8)].expression),(yyvsp[(7) - (8)].annotation));
        free((yyvsp[(3) - (8)].sValue));
        delete (yyvsp[(5) - (8)].vardeclexpr_v);
      ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 487 "lib/parser.yxx"
    {
        TypeInst* ti=TypeInst::a(GETCTX(),(yylsp[(1) - (3)]),Type::ann());
        (yyval.item) = FunctionI::a(GETCTX(),(yyloc),(yyvsp[(2) - (3)].sValue),ti,*(yyvsp[(3) - (3)].vardeclexpr_v),NULL,NULL);
        free((yyvsp[(2) - (3)].sValue));
        delete (yyvsp[(3) - (3)].vardeclexpr_v);
      ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 494 "lib/parser.yxx"
    { TypeInst* ti=TypeInst::a(GETCTX(),(yylsp[(1) - (5)]),Type::ann());
        (yyval.item) = FunctionI::a(GETCTX(),(yyloc),(yyvsp[(2) - (5)].sValue),ti,*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression),NULL);
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
      ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 501 "lib/parser.yxx"
    { (yyval.expression)=NULL; ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 503 "lib/parser.yxx"
    { (yyval.expression)=(yyvsp[(2) - (2)].expression); ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 507 "lib/parser.yxx"
    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 509 "lib/parser.yxx"
    { (yyval.vardeclexpr_v)=(yyvsp[(2) - (3)].vardeclexpr_v); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 511 "lib/parser.yxx"
    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 514 "lib/parser.yxx"
    { (yyval.vardeclexpr_v)=(yyvsp[(1) - (2)].vardeclexpr_v); ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 518 "lib/parser.yxx"
    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); (yyval.vardeclexpr_v)->push_back((yyvsp[(1) - (1)].vardeclexpr)); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 520 "lib/parser.yxx"
    { (yyval.vardeclexpr_v)=(yyvsp[(1) - (3)].vardeclexpr_v); (yyvsp[(1) - (3)].vardeclexpr_v)->push_back((yyvsp[(3) - (3)].vardeclexpr)); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 526 "lib/parser.yxx"
    { (yyval.vardeclexpr) = VarDecl::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].tiexpr), (yyvsp[(3) - (3)].sValue));
        free((yyvsp[(3) - (3)].sValue));
      ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 542 "lib/parser.yxx"
    {
        (yyval.tiexpr) = (yyvsp[(6) - (6)].tiexpr);
        (yyval.tiexpr)->addRanges(GETCTX(),*(yyvsp[(3) - (6)].expression_v));
        delete (yyvsp[(3) - (6)].expression_v);
      ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 552 "lib/parser.yxx"
    { (yyval.expression_v)=new std::vector<Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].expression)); ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 554 "lib/parser.yxx"
    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); (yyval.expression_v)->push_back((yyvsp[(3) - (3)].expression)); ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 557 "lib/parser.yxx"
    { (yyval.expression)=(yyvsp[(1) - (1)].expression); ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 559 "lib/parser.yxx"
    { (yyval.expression)=NULL; ;}
    break;

  case 51:
<<<<<<< HEAD
<<<<<<< HEAD

/* Line 1455 of yacc.c  */
#line 563 "lib/parser.yxx"
    { (yyval.expression) = Id::a(GETCTX(),(yylsp[(1) - (1)]),(yyvsp[(1) - (1)].sValue),NULL); free((yyvsp[(1) - (1)].sValue)); ;}
=======
#line 561 "lib/parser.yxx"
    { (yyval.expression)=TIId::a(GETCTX(),(yylsp[(1) - (1)]),(yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
>>>>>>> origin/master
=======
#line 561 "lib/parser.yxx"
    { (yyval.expression)=TIId::a(GETCTX(),(yylsp[(1) - (1)]),(yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
>>>>>>> origin/master
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 565 "lib/parser.yxx"
    { (yyval.expression) = Id::a(GETCTX(),(yylsp[(1) - (1)]),(yyvsp[(1) - (1)].sValue),NULL); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 567 "lib/parser.yxx"
    { (yyval.expression) = BinOp::a(GETCTX(),(yyloc),(yyvsp[(1) - (3)].expression),BOT_DOTDOT,(yyvsp[(3) - (3)].expression)); ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 569 "lib/parser.yxx"
    { (yyval.expression) = BinOp::a(GETCTX(),(yyloc),(yyvsp[(3) - (6)].expression),BOT_DOTDOT,(yyvsp[(5) - (6)].expression)); ;}
    break;

  case 55:
<<<<<<< HEAD
<<<<<<< HEAD

/* Line 1455 of yacc.c  */
#line 575 "lib/parser.yxx"
    { (yyval.tiexpr) = (yyvsp[(1) - (1)].tiexpr); ;}
=======
=======
>>>>>>> origin/master
#line 571 "lib/parser.yxx"
    { (yyval.expression) = SetLit::a(GETCTX(),(yylsp[(2) - (3)]),*(yyvsp[(2) - (3)].expression_v));
        delete (yyvsp[(2) - (3)].expression_v);
      ;}
<<<<<<< HEAD
>>>>>>> origin/master
=======
>>>>>>> origin/master
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 577 "lib/parser.yxx"
    { (yyval.tiexpr) = (yyvsp[(1) - (1)].tiexpr); ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 579 "lib/parser.yxx"
    { (yyval.tiexpr) = (yyvsp[(2) - (2)].tiexpr); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 581 "lib/parser.yxx"
    { (yyval.tiexpr) = (yyvsp[(2) - (2)].tiexpr); (yyval.tiexpr)->_type._ti = Type::TI_VAR; ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 583 "lib/parser.yxx"
    { (yyval.tiexpr) = (yyvsp[(3) - (3)].tiexpr); (yyval.tiexpr)->_type._st = Type::ST_SET; ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 585 "lib/parser.yxx"
    { (yyval.tiexpr) = (yyvsp[(4) - (4)].tiexpr); (yyval.tiexpr)->_type._st = Type::ST_SET; ;}
    break;

  case 61:
<<<<<<< HEAD
<<<<<<< HEAD

/* Line 1455 of yacc.c  */
#line 593 "lib/parser.yxx"
    { (yyval.tiexpr) = TypeInst::a(GETCTX(),(yyloc),Type::parint()); ;}
=======
=======
>>>>>>> origin/master
#line 587 "lib/parser.yxx"
    { (yyval.tiexpr) = (yyvsp[(4) - (4)].tiexpr); (yyval.tiexpr)->_type._ti = Type::TI_VAR;
        (yyval.tiexpr)->_type._st = Type::ST_SET;
      ;}
<<<<<<< HEAD
>>>>>>> origin/master
=======
>>>>>>> origin/master
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 595 "lib/parser.yxx"
    { (yyval.tiexpr) = TypeInst::a(GETCTX(),(yyloc),Type::parint()); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 597 "lib/parser.yxx"
    { (yyval.tiexpr) = TypeInst::a(GETCTX(),(yyloc),Type::parbool()); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 599 "lib/parser.yxx"
    { (yyval.tiexpr) = TypeInst::a(GETCTX(),(yyloc),Type::parfloat()); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 601 "lib/parser.yxx"
    { (yyval.tiexpr) = TypeInst::a(GETCTX(),(yyloc),Type::parstring()); ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 603 "lib/parser.yxx"
    { (yyval.tiexpr) = TypeInst::a(GETCTX(),(yyloc),Type::ann()); ;}
    break;

  case 67:
#line 605 "lib/parser.yxx"
    { (yyval.tiexpr) = TypeInst::a(GETCTX(),(yyloc),Type::bot(),
                         SetLit::a(GETCTX(), (yyloc), std::vector<Expression*>())); 
      ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 67:

/* Line 1455 of yacc.c  */
#line 607 "lib/parser.yxx"
    { (yyval.tiexpr) = TypeInst::a(GETCTX(),(yyloc),Type(),(yyvsp[(1) - (1)].expression)); ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 613 "lib/parser.yxx"
    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].expression)); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 615 "lib/parser.yxx"
    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); (yyval.expression_v)->push_back((yyvsp[(3) - (3)].expression)); ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 622 "lib/parser.yxx"
    { (yyvsp[(1) - (3)].expression)->annotate(Annotation::a(GETCTX(),(yylsp[(3) - (3)]),(yyvsp[(3) - (3)].expression))); (yyval.expression)=(yyvsp[(1) - (3)].expression); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 624 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 626 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MINUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 628 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MULT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 630 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_DIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 632 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_IDIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 634 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MOD, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 636 "lib/parser.yxx"
    { (yyval.expression)=UnOp::a(GETCTX(),(yyloc), UOT_PLUS, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 638 "lib/parser.yxx"
    { (yyval.expression)=UnOp::a(GETCTX(),(yyloc), UOT_MINUS, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 642 "lib/parser.yxx"
    { (yyval.expression)=(yyvsp[(2) - (3)].expression); ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 644 "lib/parser.yxx"
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), (yyvsp[(2) - (4)].expression), *(yyvsp[(4) - (4)].expression_v)); delete (yyvsp[(4) - (4)].expression_v); ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 646 "lib/parser.yxx"
=======
  case 68:
#line 609 "lib/parser.yxx"
    { (yyval.tiexpr) = TypeInst::a(GETCTX(),(yyloc),Type(),(yyvsp[(1) - (1)].expression)); ;}
    break;

  case 70:
#line 614 "lib/parser.yxx"
    { (yyval.tiexpr) = TypeInst::a(GETCTX(),(yyloc),Type::bot(),
                         TIId::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].sValue)));
        free((yyvsp[(1) - (1)].sValue));
      ;}
    break;

  case 71:
#line 619 "lib/parser.yxx"
    { (yyval.tiexpr) = TypeInst::a(GETCTX(),(yyloc),Type::any(),
                         TIId::a(GETCTX(), (yyloc), (yyvsp[(2) - (2)].sValue)));
        free((yyvsp[(2) - (2)].sValue));
      ;}
    break;

  case 73:
#line 628 "lib/parser.yxx"
    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].expression)); ;}
    break;

  case 74:
#line 630 "lib/parser.yxx"
    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); (yyval.expression_v)->push_back((yyvsp[(3) - (3)].expression)); ;}
    break;

  case 76:
#line 637 "lib/parser.yxx"
    { (yyvsp[(1) - (3)].expression)->annotate(Annotation::a(GETCTX(),(yylsp[(3) - (3)]),(yyvsp[(3) - (3)].expression))); (yyval.expression)=(yyvsp[(1) - (3)].expression); ;}
    break;

  case 77:
#line 639 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 78:
#line 641 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MINUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 79:
#line 643 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MULT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 80:
#line 645 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_DIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 81:
#line 647 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_IDIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 82:
#line 649 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MOD, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 83:
#line 651 "lib/parser.yxx"
    { (yyval.expression)=UnOp::a(GETCTX(),(yyloc), UOT_PLUS, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 84:
#line 653 "lib/parser.yxx"
    { (yyval.expression)=UnOp::a(GETCTX(),(yyloc), UOT_MINUS, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 85:
#line 657 "lib/parser.yxx"
    { (yyval.expression)=(yyvsp[(2) - (3)].expression); ;}
    break;

  case 86:
#line 659 "lib/parser.yxx"
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), (yyvsp[(2) - (4)].expression), *(yyvsp[(4) - (4)].expression_v)); delete (yyvsp[(4) - (4)].expression_v); ;}
    break;

  case 87:
#line 661 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 68:
#line 609 "lib/parser.yxx"
    { (yyval.tiexpr) = TypeInst::a(GETCTX(),(yyloc),Type(),(yyvsp[(1) - (1)].expression)); ;}
    break;

  case 70:
#line 614 "lib/parser.yxx"
    { (yyval.tiexpr) = TypeInst::a(GETCTX(),(yyloc),Type::bot(),
                         TIId::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].sValue)));
        free((yyvsp[(1) - (1)].sValue));
      ;}
    break;

  case 71:
#line 619 "lib/parser.yxx"
    { (yyval.tiexpr) = TypeInst::a(GETCTX(),(yyloc),Type::any(),
                         TIId::a(GETCTX(), (yyloc), (yyvsp[(2) - (2)].sValue)));
        free((yyvsp[(2) - (2)].sValue));
      ;}
    break;

  case 73:
#line 628 "lib/parser.yxx"
    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].expression)); ;}
    break;

  case 74:
#line 630 "lib/parser.yxx"
    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); (yyval.expression_v)->push_back((yyvsp[(3) - (3)].expression)); ;}
    break;

  case 76:
#line 637 "lib/parser.yxx"
    { (yyvsp[(1) - (3)].expression)->annotate(Annotation::a(GETCTX(),(yylsp[(3) - (3)]),(yyvsp[(3) - (3)].expression))); (yyval.expression)=(yyvsp[(1) - (3)].expression); ;}
    break;

  case 77:
#line 639 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 78:
#line 641 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MINUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 79:
#line 643 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MULT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 80:
#line 645 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_DIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 81:
#line 647 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_IDIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 82:
#line 649 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MOD, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 83:
#line 651 "lib/parser.yxx"
    { (yyval.expression)=UnOp::a(GETCTX(),(yyloc), UOT_PLUS, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 84:
#line 653 "lib/parser.yxx"
    { (yyval.expression)=UnOp::a(GETCTX(),(yyloc), UOT_MINUS, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 85:
#line 657 "lib/parser.yxx"
    { (yyval.expression)=(yyvsp[(2) - (3)].expression); ;}
    break;

  case 86:
#line 659 "lib/parser.yxx"
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), (yyvsp[(2) - (4)].expression), *(yyvsp[(4) - (4)].expression_v)); delete (yyvsp[(4) - (4)].expression_v); ;}
    break;

  case 87:
#line 661 "lib/parser.yxx"
>>>>>>> origin/master
    { (yyval.expression)=Id::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].sValue), NULL);
        free((yyvsp[(1) - (1)].sValue));
      ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 84:

/* Line 1455 of yacc.c  */
#line 650 "lib/parser.yxx"
=======
  case 88:
#line 665 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 88:
#line 665 "lib/parser.yxx"
>>>>>>> origin/master
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), Id::a(GETCTX(),(yylsp[(1) - (2)]),(yyvsp[(1) - (2)].sValue),NULL), *(yyvsp[(2) - (2)].expression_v));
        free((yyvsp[(1) - (2)].sValue)); delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 85:

/* Line 1455 of yacc.c  */
#line 654 "lib/parser.yxx"
    { (yyval.expression)=BoolLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 656 "lib/parser.yxx"
    { (yyval.expression)=IntLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 658 "lib/parser.yxx"
    { (yyval.expression)=FloatLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].dValue)); ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 660 "lib/parser.yxx"
    { (yyval.expression)=StringLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 663 "lib/parser.yxx"
=======
  case 89:
#line 669 "lib/parser.yxx"
    { (yyval.expression)=BoolLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 90:
#line 671 "lib/parser.yxx"
    { (yyval.expression)=IntLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 91:
#line 673 "lib/parser.yxx"
    { (yyval.expression)=FloatLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].dValue)); ;}
    break;

  case 92:
#line 675 "lib/parser.yxx"
    { (yyval.expression)=StringLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 94:
#line 678 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 89:
#line 669 "lib/parser.yxx"
    { (yyval.expression)=BoolLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 90:
#line 671 "lib/parser.yxx"
    { (yyval.expression)=IntLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 91:
#line 673 "lib/parser.yxx"
    { (yyval.expression)=FloatLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].dValue)); ;}
    break;

  case 92:
#line 675 "lib/parser.yxx"
    { (yyval.expression)=StringLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 94:
#line 678 "lib/parser.yxx"
>>>>>>> origin/master
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 92:

/* Line 1455 of yacc.c  */
#line 667 "lib/parser.yxx"
=======
  case 96:
#line 682 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 96:
#line 682 "lib/parser.yxx"
>>>>>>> origin/master
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 94:

/* Line 1455 of yacc.c  */
#line 671 "lib/parser.yxx"
=======
  case 98:
#line 686 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 98:
#line 686 "lib/parser.yxx"
>>>>>>> origin/master
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 96:

/* Line 1455 of yacc.c  */
#line 675 "lib/parser.yxx"
=======
  case 100:
#line 690 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 100:
#line 690 "lib/parser.yxx"
>>>>>>> origin/master
    { (yyval.expression)=ArrayAccess::a(GETCTX(),(yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 100:

/* Line 1455 of yacc.c  */
#line 685 "lib/parser.yxx"
    { (yyvsp[(1) - (3)].expression)->annotate(Annotation::a(GETCTX(),(yylsp[(3) - (3)]),(yyvsp[(3) - (3)].expression))); (yyval.expression)=(yyvsp[(1) - (3)].expression); ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 687 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_EQUIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 689 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_IMPL, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 691 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_RIMPL, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 693 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_OR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 695 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_XOR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 697 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_AND, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 699 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_LE, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 701 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_GR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 703 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_LQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 705 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_GQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 707 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_EQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 709 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_NQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 711 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_IN, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 713 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_SUBSET, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 715 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_SUPERSET, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 717 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_UNION, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 719 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_DIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 721 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_SYMDIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 723 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(), (yyloc), (yyvsp[(1) - (3)].expression), BOT_DOTDOT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 725 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(), (yyloc), (yyvsp[(3) - (6)].expression), BOT_DOTDOT, (yyvsp[(5) - (6)].expression)); ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 727 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_INTERSECT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 729 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUSPLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 731 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 733 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MINUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 735 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MULT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 737 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_DIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 739 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_IDIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 741 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MOD, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 743 "lib/parser.yxx"
=======
  case 104:
#line 700 "lib/parser.yxx"
    { (yyvsp[(1) - (3)].expression)->annotate(Annotation::a(GETCTX(),(yylsp[(3) - (3)]),(yyvsp[(3) - (3)].expression))); (yyval.expression)=(yyvsp[(1) - (3)].expression); ;}
    break;

  case 105:
#line 702 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_EQUIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 106:
#line 704 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_IMPL, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 107:
#line 706 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_RIMPL, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 108:
#line 708 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_OR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 109:
#line 710 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_XOR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 110:
#line 712 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_AND, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 111:
#line 714 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_LE, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 112:
#line 716 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_GR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 113:
#line 718 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_LQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 114:
#line 720 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_GQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 115:
#line 722 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_EQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 116:
#line 724 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_NQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 117:
#line 726 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_IN, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 118:
#line 728 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_SUBSET, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 119:
#line 730 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_SUPERSET, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 120:
#line 732 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_UNION, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 121:
#line 734 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_DIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 122:
#line 736 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_SYMDIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 123:
#line 738 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(), (yyloc), (yyvsp[(1) - (3)].expression), BOT_DOTDOT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 124:
#line 740 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(), (yyloc), (yyvsp[(3) - (6)].expression), BOT_DOTDOT, (yyvsp[(5) - (6)].expression)); ;}
    break;

  case 125:
#line 742 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_INTERSECT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 126:
#line 744 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUSPLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 127:
#line 746 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 128:
#line 748 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MINUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 129:
#line 750 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MULT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 130:
#line 752 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_DIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 131:
#line 754 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_IDIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 132:
#line 756 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MOD, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 133:
#line 758 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 104:
#line 700 "lib/parser.yxx"
    { (yyvsp[(1) - (3)].expression)->annotate(Annotation::a(GETCTX(),(yylsp[(3) - (3)]),(yyvsp[(3) - (3)].expression))); (yyval.expression)=(yyvsp[(1) - (3)].expression); ;}
    break;

  case 105:
#line 702 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_EQUIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 106:
#line 704 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_IMPL, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 107:
#line 706 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_RIMPL, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 108:
#line 708 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_OR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 109:
#line 710 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_XOR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 110:
#line 712 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_AND, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 111:
#line 714 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_LE, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 112:
#line 716 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_GR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 113:
#line 718 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_LQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 114:
#line 720 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_GQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 115:
#line 722 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_EQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 116:
#line 724 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_NQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 117:
#line 726 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_IN, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 118:
#line 728 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_SUBSET, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 119:
#line 730 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_SUPERSET, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 120:
#line 732 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_UNION, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 121:
#line 734 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_DIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 122:
#line 736 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_SYMDIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 123:
#line 738 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(), (yyloc), (yyvsp[(1) - (3)].expression), BOT_DOTDOT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 124:
#line 740 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(), (yyloc), (yyvsp[(3) - (6)].expression), BOT_DOTDOT, (yyvsp[(5) - (6)].expression)); ;}
    break;

  case 125:
#line 742 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_INTERSECT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 126:
#line 744 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUSPLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 127:
#line 746 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 128:
#line 748 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MINUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 129:
#line 750 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MULT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 130:
#line 752 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_DIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 131:
#line 754 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_IDIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 132:
#line 756 "lib/parser.yxx"
    { (yyval.expression)=BinOp::a(GETCTX(),(yyloc), (yyvsp[(1) - (3)].expression), BOT_MOD, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 133:
#line 758 "lib/parser.yxx"
>>>>>>> origin/master
    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=Call::a(GETCTX(), (yyloc), (yyvsp[(2) - (3)].sValue), args);
        free((yyvsp[(2) - (3)].sValue));
      ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 130:

/* Line 1455 of yacc.c  */
#line 749 "lib/parser.yxx"
    { (yyval.expression)=UnOp::a(GETCTX(), (yyloc), UOT_NOT, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 751 "lib/parser.yxx"
    { (yyval.expression)=UnOp::a(GETCTX(), (yyloc), UOT_PLUS, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 753 "lib/parser.yxx"
    { (yyval.expression)=UnOp::a(GETCTX(), (yyloc), UOT_MINUS, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 757 "lib/parser.yxx"
    { (yyval.expression)=(yyvsp[(2) - (3)].expression); ;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 759 "lib/parser.yxx"
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), (yyvsp[(2) - (4)].expression), *(yyvsp[(4) - (4)].expression_v)); delete (yyvsp[(4) - (4)].expression_v); ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 761 "lib/parser.yxx"
    { (yyval.expression)=Id::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].sValue), NULL); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 763 "lib/parser.yxx"
=======
  case 134:
#line 764 "lib/parser.yxx"
    { (yyval.expression)=UnOp::a(GETCTX(), (yyloc), UOT_NOT, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 135:
#line 766 "lib/parser.yxx"
    { (yyval.expression)=UnOp::a(GETCTX(), (yyloc), UOT_PLUS, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 136:
#line 768 "lib/parser.yxx"
    { (yyval.expression)=UnOp::a(GETCTX(), (yyloc), UOT_MINUS, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 137:
#line 772 "lib/parser.yxx"
    { (yyval.expression)=(yyvsp[(2) - (3)].expression); ;}
    break;

  case 138:
#line 774 "lib/parser.yxx"
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), (yyvsp[(2) - (4)].expression), *(yyvsp[(4) - (4)].expression_v)); delete (yyvsp[(4) - (4)].expression_v); ;}
    break;

  case 139:
#line 776 "lib/parser.yxx"
    { (yyval.expression)=Id::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].sValue), NULL); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 140:
#line 778 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 134:
#line 764 "lib/parser.yxx"
    { (yyval.expression)=UnOp::a(GETCTX(), (yyloc), UOT_NOT, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 135:
#line 766 "lib/parser.yxx"
    { (yyval.expression)=UnOp::a(GETCTX(), (yyloc), UOT_PLUS, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 136:
#line 768 "lib/parser.yxx"
    { (yyval.expression)=UnOp::a(GETCTX(), (yyloc), UOT_MINUS, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 137:
#line 772 "lib/parser.yxx"
    { (yyval.expression)=(yyvsp[(2) - (3)].expression); ;}
    break;

  case 138:
#line 774 "lib/parser.yxx"
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), (yyvsp[(2) - (4)].expression), *(yyvsp[(4) - (4)].expression_v)); delete (yyvsp[(4) - (4)].expression_v); ;}
    break;

  case 139:
#line 776 "lib/parser.yxx"
    { (yyval.expression)=Id::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].sValue), NULL); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 140:
#line 778 "lib/parser.yxx"
>>>>>>> origin/master
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), Id::a(GETCTX(), (yylsp[(1) - (2)]),(yyvsp[(1) - (2)].sValue),NULL), *(yyvsp[(2) - (2)].expression_v));
        free((yyvsp[(1) - (2)].sValue)); delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 137:

/* Line 1455 of yacc.c  */
#line 766 "lib/parser.yxx"
    { (yyval.expression)=AnonVar::a(GETCTX(),(yyloc)); ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 768 "lib/parser.yxx"
=======
  case 141:
#line 781 "lib/parser.yxx"
    { (yyval.expression)=AnonVar::a(GETCTX(),(yyloc)); ;}
    break;

  case 142:
#line 783 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 141:
#line 781 "lib/parser.yxx"
    { (yyval.expression)=AnonVar::a(GETCTX(),(yyloc)); ;}
    break;

  case 142:
#line 783 "lib/parser.yxx"
>>>>>>> origin/master
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), AnonVar::a(GETCTX(), (yyloc)), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 139:

/* Line 1455 of yacc.c  */
#line 771 "lib/parser.yxx"
    { (yyval.expression)=BoolLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 773 "lib/parser.yxx"
    { (yyval.expression)=IntLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 775 "lib/parser.yxx"
    { (yyval.expression)=FloatLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].dValue)); ;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 777 "lib/parser.yxx"
    { (yyval.expression)=StringLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 782 "lib/parser.yxx"
=======
  case 143:
#line 786 "lib/parser.yxx"
    { (yyval.expression)=BoolLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 144:
#line 788 "lib/parser.yxx"
    { (yyval.expression)=IntLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 145:
#line 790 "lib/parser.yxx"
    { (yyval.expression)=FloatLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].dValue)); ;}
    break;

  case 146:
#line 792 "lib/parser.yxx"
    { (yyval.expression)=StringLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 150:
#line 797 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 143:
#line 786 "lib/parser.yxx"
    { (yyval.expression)=BoolLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 144:
#line 788 "lib/parser.yxx"
    { (yyval.expression)=IntLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 145:
#line 790 "lib/parser.yxx"
    { (yyval.expression)=FloatLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].dValue)); ;}
    break;

  case 146:
#line 792 "lib/parser.yxx"
    { (yyval.expression)=StringLit::a(GETCTX(), (yyloc), (yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 150:
#line 797 "lib/parser.yxx"
>>>>>>> origin/master
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 148:

/* Line 1455 of yacc.c  */
#line 786 "lib/parser.yxx"
=======
  case 152:
#line 801 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 152:
#line 801 "lib/parser.yxx"
>>>>>>> origin/master
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 150:

/* Line 1455 of yacc.c  */
#line 790 "lib/parser.yxx"
=======
  case 154:
#line 805 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 154:
#line 805 "lib/parser.yxx"
>>>>>>> origin/master
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 152:

/* Line 1455 of yacc.c  */
#line 794 "lib/parser.yxx"
=======
  case 156:
#line 809 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 156:
#line 809 "lib/parser.yxx"
>>>>>>> origin/master
    { (yyval.expression)=ArrayAccess::a(GETCTX(), (yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 155:

/* Line 1455 of yacc.c  */
#line 801 "lib/parser.yxx"
    { (yyval.expression_v)=(yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 805 "lib/parser.yxx"
    { (yyval.expression) = SetLit::a(GETCTX(), (yyloc), std::vector<Expression*>()); ;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 807 "lib/parser.yxx"
    { (yyval.expression) = SetLit::a(GETCTX(), (yyloc), *(yyvsp[(2) - (3)].expression_v)); delete (yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 811 "lib/parser.yxx"
=======
  case 159:
#line 816 "lib/parser.yxx"
    { (yyval.expression_v)=(yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 160:
#line 820 "lib/parser.yxx"
    { (yyval.expression) = SetLit::a(GETCTX(), (yyloc), std::vector<Expression*>()); ;}
    break;

  case 161:
#line 822 "lib/parser.yxx"
    { (yyval.expression) = SetLit::a(GETCTX(), (yyloc), *(yyvsp[(2) - (3)].expression_v)); delete (yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 162:
#line 826 "lib/parser.yxx"
>>>>>>> origin/master
    { (yyval.expression) = Comprehension::a(GETCTX(), (yyloc), (yyvsp[(2) - (5)].expression), *(yyvsp[(4) - (5)].generators), true);
        delete (yyvsp[(4) - (5)].generators);
      ;}
    break;

<<<<<<< HEAD
  case 159:

/* Line 1455 of yacc.c  */
#line 817 "lib/parser.yxx"
    { (yyval.generators)=new Generators; (yyval.generators)->_g = *(yyvsp[(1) - (1)].generator_v); (yyval.generators)->_w = NULL; delete (yyvsp[(1) - (1)].generator_v); ;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 819 "lib/parser.yxx"
    { (yyval.generators)=new Generators; (yyval.generators)->_g = *(yyvsp[(1) - (3)].generator_v); (yyval.generators)->_w = (yyvsp[(3) - (3)].expression); delete (yyvsp[(1) - (3)].generator_v); ;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 825 "lib/parser.yxx"
    { (yyval.generator_v)=new std::vector<Generator*>; (yyval.generator_v)->push_back((yyvsp[(1) - (1)].generator)); ;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 827 "lib/parser.yxx"
    { (yyval.generator_v)=(yyvsp[(1) - (3)].generator_v); (yyval.generator_v)->push_back((yyvsp[(3) - (3)].generator)); ;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 831 "lib/parser.yxx"
    { (yyval.generator)=Generator::a(GETCTX(),*(yyvsp[(1) - (3)].string_v),(yyvsp[(3) - (3)].expression)); delete (yyvsp[(1) - (3)].string_v); ;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 837 "lib/parser.yxx"
    { (yyval.string_v)=new std::vector<std::string>; (yyval.string_v)->push_back((yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 839 "lib/parser.yxx"
    { (yyval.string_v)=(yyvsp[(1) - (3)].string_v); (yyval.string_v)->push_back((yyvsp[(3) - (3)].sValue)); free((yyvsp[(3) - (3)].sValue)); ;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 843 "lib/parser.yxx"
    { (yyval.expression)=ArrayLit::a(GETCTX(), (yyloc), std::vector<MiniZinc::Expression*>()); ;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 845 "lib/parser.yxx"
    { (yyval.expression)=ArrayLit::a(GETCTX(), (yyloc), *(yyvsp[(2) - (3)].expression_v)); delete (yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 849 "lib/parser.yxx"
=======
=======
  case 159:
#line 816 "lib/parser.yxx"
    { (yyval.expression_v)=(yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 160:
#line 820 "lib/parser.yxx"
    { (yyval.expression) = SetLit::a(GETCTX(), (yyloc), std::vector<Expression*>()); ;}
    break;

  case 161:
#line 822 "lib/parser.yxx"
    { (yyval.expression) = SetLit::a(GETCTX(), (yyloc), *(yyvsp[(2) - (3)].expression_v)); delete (yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 162:
#line 826 "lib/parser.yxx"
    { (yyval.expression) = Comprehension::a(GETCTX(), (yyloc), (yyvsp[(2) - (5)].expression), *(yyvsp[(4) - (5)].generators), true);
        delete (yyvsp[(4) - (5)].generators);
      ;}
    break;

>>>>>>> origin/master
  case 163:
#line 832 "lib/parser.yxx"
    { (yyval.generators)=new Generators; (yyval.generators)->_g = *(yyvsp[(1) - (1)].generator_v); (yyval.generators)->_w = NULL; delete (yyvsp[(1) - (1)].generator_v); ;}
    break;

  case 164:
#line 834 "lib/parser.yxx"
    { (yyval.generators)=new Generators; (yyval.generators)->_g = *(yyvsp[(1) - (3)].generator_v); (yyval.generators)->_w = (yyvsp[(3) - (3)].expression); delete (yyvsp[(1) - (3)].generator_v); ;}
    break;

  case 166:
#line 840 "lib/parser.yxx"
    { (yyval.generator_v)=new std::vector<Generator*>; (yyval.generator_v)->push_back((yyvsp[(1) - (1)].generator)); ;}
    break;

  case 167:
#line 842 "lib/parser.yxx"
    { (yyval.generator_v)=(yyvsp[(1) - (3)].generator_v); (yyval.generator_v)->push_back((yyvsp[(3) - (3)].generator)); ;}
    break;

  case 168:
#line 846 "lib/parser.yxx"
    { (yyval.generator)=Generator::a(GETCTX(),*(yyvsp[(1) - (3)].string_v),(yyvsp[(3) - (3)].expression)); delete (yyvsp[(1) - (3)].string_v); ;}
    break;

  case 170:
#line 852 "lib/parser.yxx"
    { (yyval.string_v)=new std::vector<std::string>; (yyval.string_v)->push_back((yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 171:
#line 854 "lib/parser.yxx"
    { (yyval.string_v)=(yyvsp[(1) - (3)].string_v); (yyval.string_v)->push_back((yyvsp[(3) - (3)].sValue)); free((yyvsp[(3) - (3)].sValue)); ;}
    break;

  case 172:
#line 858 "lib/parser.yxx"
    { (yyval.expression)=ArrayLit::a(GETCTX(), (yyloc), std::vector<MiniZinc::Expression*>()); ;}
    break;

  case 173:
#line 860 "lib/parser.yxx"
    { (yyval.expression)=ArrayLit::a(GETCTX(), (yyloc), *(yyvsp[(2) - (3)].expression_v)); delete (yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 174:
#line 864 "lib/parser.yxx"
<<<<<<< HEAD
>>>>>>> origin/master
=======
>>>>>>> origin/master
    { (yyval.expression)=ArrayLit::a(GETCTX(), (yyloc), *(yyvsp[(2) - (3)].expression_vv));
        for (unsigned int i=1; i<(yyvsp[(2) - (3)].expression_vv)->size(); i++)
          if ((*(yyvsp[(2) - (3)].expression_vv))[i].size() != (*(yyvsp[(2) - (3)].expression_vv))[i-1].size())
            yyerror(&(yylsp[(2) - (3)]), parm, "syntax error, all sub-arrays of 2d array literal must have the same length");
        delete (yyvsp[(2) - (3)].expression_vv);
      ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 171:

/* Line 1455 of yacc.c  */
#line 856 "lib/parser.yxx"
=======
  case 175:
#line 871 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 175:
#line 871 "lib/parser.yxx"
>>>>>>> origin/master
    { (yyval.expression)=ArrayLit::a(GETCTX(), (yyloc), *(yyvsp[(2) - (4)].expression_vv));
        for (unsigned int i=1; i<(yyvsp[(2) - (4)].expression_vv)->size(); i++)
          if ((*(yyvsp[(2) - (4)].expression_vv))[i].size() != (*(yyvsp[(2) - (4)].expression_vv))[i-1].size())
            yyerror(&(yylsp[(2) - (4)]), parm, "syntax error, all sub-arrays of 2d array literal must have the same length");
        delete (yyvsp[(2) - (4)].expression_vv);
      ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 172:

/* Line 1455 of yacc.c  */
#line 865 "lib/parser.yxx"
=======
  case 176:
#line 880 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 176:
#line 880 "lib/parser.yxx"
>>>>>>> origin/master
    { (yyval.expression_vv)=new std::vector<std::vector<MiniZinc::Expression*> >;
        (yyval.expression_vv)->push_back(*(yyvsp[(1) - (1)].expression_v));
        delete (yyvsp[(1) - (1)].expression_v);
      ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 173:

/* Line 1455 of yacc.c  */
#line 870 "lib/parser.yxx"
    { (yyval.expression_vv)=(yyvsp[(1) - (3)].expression_vv); (yyval.expression_vv)->push_back(*(yyvsp[(3) - (3)].expression_v)); delete (yyvsp[(3) - (3)].expression_v); ;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 874 "lib/parser.yxx"
=======
  case 177:
#line 885 "lib/parser.yxx"
    { (yyval.expression_vv)=(yyvsp[(1) - (3)].expression_vv); (yyval.expression_vv)->push_back(*(yyvsp[(3) - (3)].expression_v)); delete (yyvsp[(3) - (3)].expression_v); ;}
    break;

  case 178:
#line 889 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 177:
#line 885 "lib/parser.yxx"
    { (yyval.expression_vv)=(yyvsp[(1) - (3)].expression_vv); (yyval.expression_vv)->push_back(*(yyvsp[(3) - (3)].expression_v)); delete (yyvsp[(3) - (3)].expression_v); ;}
    break;

  case 178:
#line 889 "lib/parser.yxx"
>>>>>>> origin/master
    { (yyval.expression)=Comprehension::a(GETCTX(), (yyloc), (yyvsp[(2) - (5)].expression), *(yyvsp[(4) - (5)].generators), false);
        delete (yyvsp[(4) - (5)].generators);
      ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 175:

/* Line 1455 of yacc.c  */
#line 880 "lib/parser.yxx"
=======
  case 179:
#line 895 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 179:
#line 895 "lib/parser.yxx"
>>>>>>> origin/master
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

<<<<<<< HEAD
<<<<<<< HEAD
  case 176:

/* Line 1455 of yacc.c  */
#line 891 "lib/parser.yxx"
    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; ;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 893 "lib/parser.yxx"
    { (yyval.expression_v)=(yyvsp[(1) - (5)].expression_v); (yyval.expression_v)->push_back((yyvsp[(3) - (5)].expression)); (yyval.expression_v)->push_back((yyvsp[(5) - (5)].expression)); ;}
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 897 "lib/parser.yxx"
    { (yyval.iValue)=BOT_EQUIV; ;}
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 899 "lib/parser.yxx"
    { (yyval.iValue)=BOT_IMPL; ;}
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 901 "lib/parser.yxx"
    { (yyval.iValue)=BOT_RIMPL; ;}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 903 "lib/parser.yxx"
    { (yyval.iValue)=BOT_OR; ;}
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 905 "lib/parser.yxx"
    { (yyval.iValue)=BOT_XOR; ;}
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 907 "lib/parser.yxx"
    { (yyval.iValue)=BOT_AND; ;}
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 909 "lib/parser.yxx"
    { (yyval.iValue)=BOT_LE; ;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 911 "lib/parser.yxx"
    { (yyval.iValue)=BOT_GR; ;}
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 913 "lib/parser.yxx"
    { (yyval.iValue)=BOT_LQ; ;}
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 915 "lib/parser.yxx"
    { (yyval.iValue)=BOT_GQ; ;}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 917 "lib/parser.yxx"
    { (yyval.iValue)=BOT_EQ; ;}
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 919 "lib/parser.yxx"
    { (yyval.iValue)=BOT_NQ; ;}
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 921 "lib/parser.yxx"
    { (yyval.iValue)=BOT_IN; ;}
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 923 "lib/parser.yxx"
    { (yyval.iValue)=BOT_SUBSET; ;}
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 925 "lib/parser.yxx"
    { (yyval.iValue)=BOT_SUPERSET; ;}
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 927 "lib/parser.yxx"
    { (yyval.iValue)=BOT_UNION; ;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 929 "lib/parser.yxx"
    { (yyval.iValue)=BOT_DIFF; ;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 931 "lib/parser.yxx"
    { (yyval.iValue)=BOT_SYMDIFF; ;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 933 "lib/parser.yxx"
    { (yyval.iValue)=BOT_PLUS; ;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 935 "lib/parser.yxx"
    { (yyval.iValue)=BOT_MINUS; ;}
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 937 "lib/parser.yxx"
    { (yyval.iValue)=BOT_MULT; ;}
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 939 "lib/parser.yxx"
    { (yyval.iValue)=BOT_DIV; ;}
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 941 "lib/parser.yxx"
    { (yyval.iValue)=BOT_IDIV; ;}
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 943 "lib/parser.yxx"
    { (yyval.iValue)=BOT_MOD; ;}
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 945 "lib/parser.yxx"
    { (yyval.iValue)=BOT_INTERSECT; ;}
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 947 "lib/parser.yxx"
    { (yyval.iValue)=BOT_PLUSPLUS; ;}
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 949 "lib/parser.yxx"
    { (yyval.iValue)=-1; ;}
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 953 "lib/parser.yxx"
=======
  case 180:
#line 906 "lib/parser.yxx"
    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; ;}
    break;

  case 181:
#line 908 "lib/parser.yxx"
    { (yyval.expression_v)=(yyvsp[(1) - (5)].expression_v); (yyval.expression_v)->push_back((yyvsp[(3) - (5)].expression)); (yyval.expression_v)->push_back((yyvsp[(5) - (5)].expression)); ;}
    break;

  case 182:
#line 912 "lib/parser.yxx"
    { (yyval.iValue)=BOT_EQUIV; ;}
    break;

  case 183:
#line 914 "lib/parser.yxx"
    { (yyval.iValue)=BOT_IMPL; ;}
    break;

  case 184:
#line 916 "lib/parser.yxx"
    { (yyval.iValue)=BOT_RIMPL; ;}
    break;

  case 185:
#line 918 "lib/parser.yxx"
    { (yyval.iValue)=BOT_OR; ;}
    break;

  case 186:
#line 920 "lib/parser.yxx"
    { (yyval.iValue)=BOT_XOR; ;}
    break;

  case 187:
#line 922 "lib/parser.yxx"
    { (yyval.iValue)=BOT_AND; ;}
    break;

  case 188:
#line 924 "lib/parser.yxx"
    { (yyval.iValue)=BOT_LE; ;}
    break;

  case 189:
#line 926 "lib/parser.yxx"
    { (yyval.iValue)=BOT_GR; ;}
    break;

  case 190:
#line 928 "lib/parser.yxx"
    { (yyval.iValue)=BOT_LQ; ;}
    break;

  case 191:
#line 930 "lib/parser.yxx"
    { (yyval.iValue)=BOT_GQ; ;}
    break;

  case 192:
#line 932 "lib/parser.yxx"
    { (yyval.iValue)=BOT_EQ; ;}
    break;

  case 193:
#line 934 "lib/parser.yxx"
    { (yyval.iValue)=BOT_NQ; ;}
    break;

  case 194:
#line 936 "lib/parser.yxx"
    { (yyval.iValue)=BOT_IN; ;}
    break;

  case 195:
#line 938 "lib/parser.yxx"
    { (yyval.iValue)=BOT_SUBSET; ;}
    break;

  case 196:
#line 940 "lib/parser.yxx"
    { (yyval.iValue)=BOT_SUPERSET; ;}
    break;

  case 197:
#line 942 "lib/parser.yxx"
    { (yyval.iValue)=BOT_UNION; ;}
    break;

  case 198:
#line 944 "lib/parser.yxx"
    { (yyval.iValue)=BOT_DIFF; ;}
    break;

  case 199:
#line 946 "lib/parser.yxx"
    { (yyval.iValue)=BOT_SYMDIFF; ;}
    break;

  case 200:
#line 948 "lib/parser.yxx"
    { (yyval.iValue)=BOT_PLUS; ;}
    break;

  case 201:
#line 950 "lib/parser.yxx"
    { (yyval.iValue)=BOT_MINUS; ;}
    break;

  case 202:
#line 952 "lib/parser.yxx"
    { (yyval.iValue)=BOT_MULT; ;}
    break;

  case 203:
#line 954 "lib/parser.yxx"
    { (yyval.iValue)=BOT_DIV; ;}
    break;

  case 204:
#line 956 "lib/parser.yxx"
    { (yyval.iValue)=BOT_IDIV; ;}
    break;

  case 205:
#line 958 "lib/parser.yxx"
    { (yyval.iValue)=BOT_MOD; ;}
    break;

  case 206:
#line 960 "lib/parser.yxx"
    { (yyval.iValue)=BOT_INTERSECT; ;}
    break;

  case 207:
#line 962 "lib/parser.yxx"
    { (yyval.iValue)=BOT_PLUSPLUS; ;}
    break;

  case 208:
#line 964 "lib/parser.yxx"
    { (yyval.iValue)=-1; ;}
    break;

  case 209:
#line 968 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 180:
#line 906 "lib/parser.yxx"
    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; ;}
    break;

  case 181:
#line 908 "lib/parser.yxx"
    { (yyval.expression_v)=(yyvsp[(1) - (5)].expression_v); (yyval.expression_v)->push_back((yyvsp[(3) - (5)].expression)); (yyval.expression_v)->push_back((yyvsp[(5) - (5)].expression)); ;}
    break;

  case 182:
#line 912 "lib/parser.yxx"
    { (yyval.iValue)=BOT_EQUIV; ;}
    break;

  case 183:
#line 914 "lib/parser.yxx"
    { (yyval.iValue)=BOT_IMPL; ;}
    break;

  case 184:
#line 916 "lib/parser.yxx"
    { (yyval.iValue)=BOT_RIMPL; ;}
    break;

  case 185:
#line 918 "lib/parser.yxx"
    { (yyval.iValue)=BOT_OR; ;}
    break;

  case 186:
#line 920 "lib/parser.yxx"
    { (yyval.iValue)=BOT_XOR; ;}
    break;

  case 187:
#line 922 "lib/parser.yxx"
    { (yyval.iValue)=BOT_AND; ;}
    break;

  case 188:
#line 924 "lib/parser.yxx"
    { (yyval.iValue)=BOT_LE; ;}
    break;

  case 189:
#line 926 "lib/parser.yxx"
    { (yyval.iValue)=BOT_GR; ;}
    break;

  case 190:
#line 928 "lib/parser.yxx"
    { (yyval.iValue)=BOT_LQ; ;}
    break;

  case 191:
#line 930 "lib/parser.yxx"
    { (yyval.iValue)=BOT_GQ; ;}
    break;

  case 192:
#line 932 "lib/parser.yxx"
    { (yyval.iValue)=BOT_EQ; ;}
    break;

  case 193:
#line 934 "lib/parser.yxx"
    { (yyval.iValue)=BOT_NQ; ;}
    break;

  case 194:
#line 936 "lib/parser.yxx"
    { (yyval.iValue)=BOT_IN; ;}
    break;

  case 195:
#line 938 "lib/parser.yxx"
    { (yyval.iValue)=BOT_SUBSET; ;}
    break;

  case 196:
#line 940 "lib/parser.yxx"
    { (yyval.iValue)=BOT_SUPERSET; ;}
    break;

  case 197:
#line 942 "lib/parser.yxx"
    { (yyval.iValue)=BOT_UNION; ;}
    break;

  case 198:
#line 944 "lib/parser.yxx"
    { (yyval.iValue)=BOT_DIFF; ;}
    break;

  case 199:
#line 946 "lib/parser.yxx"
    { (yyval.iValue)=BOT_SYMDIFF; ;}
    break;

  case 200:
#line 948 "lib/parser.yxx"
    { (yyval.iValue)=BOT_PLUS; ;}
    break;

  case 201:
#line 950 "lib/parser.yxx"
    { (yyval.iValue)=BOT_MINUS; ;}
    break;

  case 202:
#line 952 "lib/parser.yxx"
    { (yyval.iValue)=BOT_MULT; ;}
    break;

  case 203:
#line 954 "lib/parser.yxx"
    { (yyval.iValue)=BOT_DIV; ;}
    break;

  case 204:
#line 956 "lib/parser.yxx"
    { (yyval.iValue)=BOT_IDIV; ;}
    break;

  case 205:
#line 958 "lib/parser.yxx"
    { (yyval.iValue)=BOT_MOD; ;}
    break;

  case 206:
#line 960 "lib/parser.yxx"
    { (yyval.iValue)=BOT_INTERSECT; ;}
    break;

  case 207:
#line 962 "lib/parser.yxx"
    { (yyval.iValue)=BOT_PLUSPLUS; ;}
    break;

  case 208:
#line 964 "lib/parser.yxx"
    { (yyval.iValue)=-1; ;}
    break;

  case 209:
#line 968 "lib/parser.yxx"
>>>>>>> origin/master
    { if ((yyvsp[(1) - (6)].iValue)==-1) {
          (yyval.expression)=NULL;
          yyerror(&(yylsp[(3) - (6)]), parm, "syntax error, unary operator with two arguments");
        } else {
          (yyval.expression)=BinOp::a(GETCTX(), (yyloc), (yyvsp[(3) - (6)].expression),static_cast<BinOpType>((yyvsp[(1) - (6)].iValue)),(yyvsp[(5) - (6)].expression));
        }
      ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 206:

/* Line 1455 of yacc.c  */
#line 961 "lib/parser.yxx"
=======
  case 210:
#line 976 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 210:
#line 976 "lib/parser.yxx"
>>>>>>> origin/master
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

<<<<<<< HEAD
<<<<<<< HEAD
  case 207:

/* Line 1455 of yacc.c  */
#line 984 "lib/parser.yxx"
    { (yyval.expression)=Call::a(GETCTX(), (yyloc), (yyvsp[(1) - (3)].sValue), std::vector<Expression*>()); free((yyvsp[(1) - (3)].sValue)); ;}
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 987 "lib/parser.yxx"
=======
  case 211:
#line 999 "lib/parser.yxx"
    { (yyval.expression)=Call::a(GETCTX(), (yyloc), (yyvsp[(1) - (3)].sValue), std::vector<Expression*>()); free((yyvsp[(1) - (3)].sValue)); ;}
    break;

  case 213:
#line 1002 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 211:
#line 999 "lib/parser.yxx"
    { (yyval.expression)=Call::a(GETCTX(), (yyloc), (yyvsp[(1) - (3)].sValue), std::vector<Expression*>()); free((yyvsp[(1) - (3)].sValue)); ;}
    break;

  case 213:
#line 1002 "lib/parser.yxx"
>>>>>>> origin/master
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

<<<<<<< HEAD
<<<<<<< HEAD
  case 210:

/* Line 1455 of yacc.c  */
#line 998 "lib/parser.yxx"
=======
  case 214:
#line 1013 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 214:
#line 1013 "lib/parser.yxx"
>>>>>>> origin/master
    { 
        vector<Generator*> gens;
        vector<CtxStringH> ids;
        for (unsigned int i=0; i<(yyvsp[(3) - (7)].expression_p)->first.size(); i++) {
          if (Id* id = (yyvsp[(3) - (7)].expression_p)->first[i]->dyn_cast<Id>()) {
            ids.push_back(id->_v);
          } else {
            if (BinOp* boe = (yyvsp[(3) - (7)].expression_p)->first[i]->dyn_cast<BinOp>()) {
              Id* id = boe->_e0->dyn_cast<Id>();
              if (id && boe->_op == BOT_IN) {
                ids.push_back(id->_v);
                gens.push_back(Generator::a(GETCTX(),ids,boe->_e1));
                ids = vector<CtxStringH>();
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

<<<<<<< HEAD
<<<<<<< HEAD
  case 211:

/* Line 1455 of yacc.c  */
#line 1037 "lib/parser.yxx"
=======
  case 215:
#line 1052 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 215:
#line 1052 "lib/parser.yxx"
>>>>>>> origin/master
    { (yyval.expression_p)=new pair<vector<Expression*>,Expression*>;
        (yyval.expression_p)->first=*(yyvsp[(1) - (1)].expression_v); (yyval.expression_p)->second=NULL;
        delete (yyvsp[(1) - (1)].expression_v);
      ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 212:

/* Line 1455 of yacc.c  */
#line 1042 "lib/parser.yxx"
=======
  case 216:
#line 1057 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 216:
#line 1057 "lib/parser.yxx"
>>>>>>> origin/master
    { (yyval.expression_p)=new pair<vector<Expression*>,Expression*>;
        (yyval.expression_p)->first=*(yyvsp[(1) - (3)].expression_v); (yyval.expression_p)->second=(yyvsp[(3) - (3)].expression);
        delete (yyvsp[(1) - (3)].expression_v);
      ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 213:

/* Line 1455 of yacc.c  */
#line 1049 "lib/parser.yxx"
    { (yyval.expression)=Let::a(GETCTX(), (yyloc), *(yyvsp[(3) - (6)].expression_v), (yyvsp[(6) - (6)].expression)); delete (yyvsp[(3) - (6)].expression_v); ;}
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 1051 "lib/parser.yxx"
    { (yyval.expression)=Let::a(GETCTX(), (yyloc), *(yyvsp[(3) - (7)].expression_v), (yyvsp[(7) - (7)].expression)); delete (yyvsp[(3) - (7)].expression_v); ;}
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 1071 "lib/parser.yxx"
    { (yyval.expression_v)=new vector<Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].vardeclexpr)); ;}
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 1073 "lib/parser.yxx"
=======
  case 217:
#line 1064 "lib/parser.yxx"
    { (yyval.expression)=Let::a(GETCTX(), (yyloc), *(yyvsp[(3) - (6)].expression_v), (yyvsp[(6) - (6)].expression)); delete (yyvsp[(3) - (6)].expression_v); ;}
    break;

  case 218:
#line 1066 "lib/parser.yxx"
    { (yyval.expression)=Let::a(GETCTX(), (yyloc), *(yyvsp[(3) - (7)].expression_v), (yyvsp[(7) - (7)].expression)); delete (yyvsp[(3) - (7)].expression_v); ;}
    break;

  case 219:
#line 1086 "lib/parser.yxx"
    { (yyval.expression_v)=new vector<Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].vardeclexpr)); ;}
    break;

  case 220:
#line 1088 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 217:
#line 1064 "lib/parser.yxx"
    { (yyval.expression)=Let::a(GETCTX(), (yyloc), *(yyvsp[(3) - (6)].expression_v), (yyvsp[(6) - (6)].expression)); delete (yyvsp[(3) - (6)].expression_v); ;}
    break;

  case 218:
#line 1066 "lib/parser.yxx"
    { (yyval.expression)=Let::a(GETCTX(), (yyloc), *(yyvsp[(3) - (7)].expression_v), (yyvsp[(7) - (7)].expression)); delete (yyvsp[(3) - (7)].expression_v); ;}
    break;

  case 219:
#line 1086 "lib/parser.yxx"
    { (yyval.expression_v)=new vector<Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].vardeclexpr)); ;}
    break;

  case 220:
#line 1088 "lib/parser.yxx"
>>>>>>> origin/master
    { (yyval.expression_v)=new vector<Expression*>;
        ConstraintI* ce = (yyvsp[(1) - (1)].item)->cast<ConstraintI>();
        (yyval.expression_v)->push_back(ce->_e);
        ce->_e=NULL;
      ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 217:

/* Line 1455 of yacc.c  */
#line 1079 "lib/parser.yxx"
    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); (yyval.expression_v)->push_back((yyvsp[(3) - (3)].vardeclexpr)); ;}
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 1081 "lib/parser.yxx"
=======
  case 221:
#line 1094 "lib/parser.yxx"
    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); (yyval.expression_v)->push_back((yyvsp[(3) - (3)].vardeclexpr)); ;}
    break;

  case 222:
#line 1096 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 221:
#line 1094 "lib/parser.yxx"
    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); (yyval.expression_v)->push_back((yyvsp[(3) - (3)].vardeclexpr)); ;}
    break;

  case 222:
#line 1096 "lib/parser.yxx"
>>>>>>> origin/master
    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v);
        ConstraintI* ce = (yyvsp[(3) - (3)].item)->cast<ConstraintI>();
        (yyval.expression_v)->push_back(ce->_e);
        ce->_e=NULL;
      ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 221:

/* Line 1455 of yacc.c  */
#line 1091 "lib/parser.yxx"
=======
  case 225:
#line 1106 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 225:
#line 1106 "lib/parser.yxx"
>>>>>>> origin/master
    { (yyval.vardeclexpr) = (yyvsp[(1) - (2)].vardeclexpr);
        (yyval.vardeclexpr)->annotate((yyvsp[(2) - (2)].annotation));
      ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 222:

/* Line 1455 of yacc.c  */
#line 1095 "lib/parser.yxx"
=======
  case 226:
#line 1110 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 226:
#line 1110 "lib/parser.yxx"
>>>>>>> origin/master
    { (yyvsp[(1) - (4)].vardeclexpr)->_e = (yyvsp[(4) - (4)].expression);
        (yyval.vardeclexpr) = (yyvsp[(1) - (4)].vardeclexpr);
        (yyval.vardeclexpr)->_loc = (yyloc);
        (yyval.vardeclexpr)->annotate((yyvsp[(2) - (4)].annotation));
      ;}
    break;

<<<<<<< HEAD
<<<<<<< HEAD
  case 223:

/* Line 1455 of yacc.c  */
#line 1103 "lib/parser.yxx"
    { (yyval.annotation)=NULL; ;}
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 1108 "lib/parser.yxx"
    { (yyval.annotation)=Annotation::a(GETCTX(), (yylsp[(2) - (2)]), (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 1110 "lib/parser.yxx"
=======
  case 227:
#line 1118 "lib/parser.yxx"
    { (yyval.annotation)=NULL; ;}
    break;

  case 229:
#line 1123 "lib/parser.yxx"
    { (yyval.annotation)=Annotation::a(GETCTX(), (yylsp[(2) - (2)]), (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 230:
#line 1125 "lib/parser.yxx"
>>>>>>> origin/master
=======
  case 227:
#line 1118 "lib/parser.yxx"
    { (yyval.annotation)=NULL; ;}
    break;

  case 229:
#line 1123 "lib/parser.yxx"
    { (yyval.annotation)=Annotation::a(GETCTX(), (yylsp[(2) - (2)]), (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 230:
#line 1125 "lib/parser.yxx"
>>>>>>> origin/master
    { (yyval.annotation)=(yyvsp[(1) - (3)].annotation); (yyval.annotation)->merge(Annotation::a(GETCTX(), (yylsp[(3) - (3)]), (yyvsp[(3) - (3)].expression))); ;}
    break;


<<<<<<< HEAD

/* Line 1455 of yacc.c  */
#line 4266 "lib/parser.tab.cpp"
=======
/* Line 1267 of yacc.c.  */
#line 3985 "lib/parser.tab.cpp"
<<<<<<< HEAD
>>>>>>> origin/master
=======
>>>>>>> origin/master
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
      /* If just tried and failed to reuse lookahead token after an
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

  /* Else will try to reuse lookahead token after shifting the error
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

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, parm, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
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



