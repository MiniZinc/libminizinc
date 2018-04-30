/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




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

#include <minizinc/parser.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/json_parser.hh>

using namespace std;
using namespace MiniZinc;

#define YYLLOC_DEFAULT(Current, Rhs, N) \
  Current.filename(Rhs[1].filename()); \
  Current.first_line(Rhs[1].first_line()); \
  Current.first_column(Rhs[1].first_column()); \
  Current.last_line(Rhs[N].last_line()); \
  Current.last_column(Rhs[N].last_column());

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
    pp->err << "(included from file '" << m->filename() << "')" << endl;
  }
  pp->err << location->filename() << ":"
          << location->first_line() << ":" << endl;
  pp->printCurrentLine();
  for (int i=0; i<static_cast<int>(location->first_column())-1; i++)
    pp->err << " ";
  for (unsigned int i=location->first_column(); i<=location->last_column(); i++)
    pp->err << "^";
  pp->err << std::endl << "Error: " << str << std::endl << std::endl;
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

void filepath(const string& f, string& dirname, string& basename) {
  dirname = ""; basename = f;
  for (size_t p=basename.find_first_of('/');
       p!=string::npos;
       dirname+=basename.substr(0,p+1),
       basename=basename.substr(p+1),
       p=basename.find_first_of('/')
       ) {}
}

// fastest way to read a file into a string (especially big files)
// see: http://insanecoding.blogspot.be/2011/11/how-to-read-in-file-in-c.html
std::string get_file_contents(std::ifstream &in)
{
  if (in)
  {
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(static_cast<unsigned int>(in.tellg()));
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    FileUtils::inflateString(contents);
    return(contents);
  }
  throw(errno);
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

namespace MiniZinc {

  Model* parseFromString(const string& text,
                         const string& filename,
                         const vector<string>& ip,
                         bool ignoreStdlib,
                         bool parseDocComments,
                         bool verbose,
                         ostream& err,
                         std::vector<SyntaxError>& syntaxErrors) {
    GCLock lock;
    std::string modelText = text;
    FileUtils::inflateString(modelText);
    vector<string> includePaths;
    for (unsigned int i=0; i<ip.size(); i++)
      includePaths.push_back(ip[i]);

    vector<ParseWorkItem> files;
    map<string,Model*> seenModels;

    Model* model = new Model();
    model->setFilename(filename);

    if (!ignoreStdlib) {
      Model* stdlib = new Model;
      stdlib->setFilename("stdlib.mzn");
      files.push_back(ParseWorkItem(stdlib,"./","stdlib.mzn"));
      seenModels.insert(pair<string,Model*>("stdlib.mzn",stdlib));
      IncludeI* stdlibinc = new IncludeI(Location(),stdlib->filename());
      stdlibinc->m(stdlib,true);
      model->addItem(stdlibinc);
    }

    model->setFilepath(filename);
    bool isFzn;
    if (filename=="") {
      isFzn = false;
    } else {
      isFzn = (filename.compare(filename.length()-4,4,".fzn")==0);
      isFzn |= (filename.compare(filename.length()-4,4,".ozn")==0);
      isFzn |= (filename.compare(filename.length()-4,4,".szn")==0);
    }
    ParserState pp(filename,modelText, err, files, seenModels, model, false, isFzn, parseDocComments);
    yylex_init(&pp.yyscanner);
    yyset_extra(&pp, pp.yyscanner);
    yyparse(&pp);
    if (pp.yyscanner)
    yylex_destroy(pp.yyscanner);
    if (pp.hadError) {
      goto error;
    }

    while (!files.empty()) {
      ParseWorkItem& np = files.back();
      string parentPath = np.dirName;
      Model* m = np.m;
      files.pop_back();
      string f(m->filename().str());

      for (Model* p=m->parent(); p; p=p->parent()) {
        if (f == p->filename().c_str()) {
          err << "Error: cyclic includes: " << std::endl;
          for (Model* pe=m; pe; pe=pe->parent()) {
            err << "  " << pe->filename() << std::endl;
          }
          goto error;
        }
      }
      ifstream file;
      string fullname;
      if (FileUtils::is_absolute(f) || parentPath=="") {
        fullname = f;
        if (FileUtils::file_exists(fullname)) {
          file.open(fullname.c_str(), std::ios::binary);
        }
      } else {
        includePaths.push_back(parentPath);
        for (unsigned int i=0; i<includePaths.size(); i++) {
          fullname = includePaths[i]+f;
          if (FileUtils::file_exists(fullname)) {
            file.open(fullname.c_str(), std::ios::binary);
            if (file.is_open())
              break;
          }
        }
        includePaths.pop_back();
      }
      if (!file.is_open()) {
        err << "Error: cannot open file '" << f << "'." << endl;
        goto error;
      }
      if (verbose)
        std::cerr << "processing file '" << fullname << "'" << endl;
      std::string s = get_file_contents(file);

      m->setFilepath(fullname);
      bool isFzn = (fullname.compare(fullname.length()-4,4,".fzn")==0);
      isFzn |= (fullname.compare(fullname.length()-4,4,".ozn")==0);
      isFzn |= (fullname.compare(fullname.length()-4,4,".szn")==0);
      ParserState pp(fullname,s, err, files, seenModels, m, false, isFzn, parseDocComments);
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
    for (unsigned int i=0; i<pp.syntaxErrors.size(); i++)
      syntaxErrors.push_back(pp.syntaxErrors[i]);
    delete model;
    return NULL;
  }

  void parse(Env& env,
             Model*& model,
             const vector<string>& filenames,
             const vector<string>& datafiles,
             const vector<string>& ip,
             bool ignoreStdlib,
             bool parseDocComments,
             bool verbose,
             ostream& err) {
    
    vector<string> includePaths;
    for (unsigned int i=0; i<ip.size(); i++)
      includePaths.push_back(ip[i]);
    
    vector<ParseWorkItem> files;
    map<string,Model*> seenModels;
    
    if (filenames.size() > 0) {
      GCLock lock;
      string fileDirname; string fileBasename;
      filepath(filenames[0], fileDirname, fileBasename);
      model->setFilename(fileBasename);
      
      files.push_back(ParseWorkItem(model,fileDirname,fileBasename));
      
      for (unsigned int i=1; i<filenames.size(); i++) {
        GCLock lock;
        string dirName, baseName;
        filepath(filenames[i], dirName, baseName);

        bool isFzn = (baseName.compare(baseName.length()-4,4,".fzn")==0);
        if (isFzn) {
          files.push_back(ParseWorkItem(model,dirName,baseName));
        } else {
          Model* includedModel = new Model;
          includedModel->setFilename(baseName);
          files.push_back(ParseWorkItem(includedModel,dirName,baseName));
          seenModels.insert(pair<string,Model*>(baseName,includedModel));
          Location loc(ASTString(filenames[i]),0,0,0,0);
          IncludeI* inc = new IncludeI(loc,includedModel->filename());
          inc->m(includedModel,true);
          model->addItem(inc);
        }
      }
    }
    
    if (!ignoreStdlib) {
      GCLock lock;
      Model* stdlib = new Model;
      stdlib->setFilename("stdlib.mzn");
      files.push_back(ParseWorkItem(stdlib,"./","stdlib.mzn"));
      seenModels.insert(pair<string,Model*>("stdlib.mzn",stdlib));
      Location stdlibloc(ASTString(model->filename()),0,0,0,0);
      IncludeI* stdlibinc =
      new IncludeI(stdlibloc,stdlib->filename());
      stdlibinc->m(stdlib,true);
      model->addItem(stdlibinc);
    }
    
    while (!files.empty()) {
      GCLock lock;
      ParseWorkItem& np = files.back();
      string parentPath = np.dirName;
      Model* m = np.m;
      //      string f(m->filename().str());
      string f(np.fileName);
      files.pop_back();

      for (Model* p=m->parent(); p; p=p->parent()) {
        if (f == p->filename().c_str()) {
          err << "Error: cyclic includes: " << std::endl;
          for (Model* pe=m; pe; pe=pe->parent()) {
            err << "  " << pe->filename() << std::endl;
          }
          goto error;
        }
      }
      ifstream file;
      string fullname;
      if (FileUtils::is_absolute(f) || parentPath=="") {
        if (filenames.size() == 0) {
          err << "Internal error." << endl;
          goto error;
        }
        fullname = f;
        if (FileUtils::file_exists(fullname)) {
          file.open(fullname.c_str(), std::ios::binary);
        }
      } else {
        includePaths.push_back(parentPath);
        for (unsigned int i=0; i<includePaths.size(); i++) {
          fullname = includePaths[i]+f;
          if (FileUtils::file_exists(fullname)) {
            file.open(fullname.c_str(), std::ios::binary);
            if (file.is_open())
              break;
          }
        }
        includePaths.pop_back();
      }
      if (!file.is_open()) {
        err << "Error: cannot open file '" << f << "'." << endl;
        goto error;
      }
      if (verbose)
        std::cerr << "processing file '" << fullname << "'" << endl;
      std::string s = get_file_contents(file);
      
      if (m->filepath().size() == 0)
        m->setFilepath(fullname);
      bool isFzn = (fullname.compare(fullname.length()-4,4,".fzn")==0);
      isFzn |= (fullname.compare(fullname.length()-4,4,".ozn")==0);
      isFzn |= (fullname.compare(fullname.length()-4,4,".szn")==0);
      ParserState pp(fullname,s, err, files, seenModels, m, false, isFzn, parseDocComments);
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
      GCLock lock;
      string f = datafiles[i];
      if (f.size()>6 && f.substr(f.size()-5,string::npos)==".json") {
        JSONParser jp(env.envi());
        jp.parse(model, f);
      } else {
        string s;
        if (f.size() > 5 && f.substr(0,5)=="cmd:/") {
          s = f.substr(5);
        } else {
          std::ifstream file;
          file.open(f.c_str(), std::ios::binary);
          if (!FileUtils::file_exists(f) || !file.is_open()) {
            err << "Error: cannot open data file '" << f << "'." << endl;
            goto error;
          }
          if (verbose)
            std::cerr << "processing data file '" << f << "'" << endl;
          s = get_file_contents(file);
        }
        
        ParserState pp(f, s, err, files, seenModels, model, true, false, parseDocComments);
        yylex_init(&pp.yyscanner);
        yyset_extra(&pp, pp.yyscanner);
        yyparse(&pp);
        if (pp.yyscanner)
          yylex_destroy(pp.yyscanner);
        if (pp.hadError) {
          goto error;
        }
      }
    }
    
    return;
  error:
    delete model;
    model = NULL;
  }
  
  Model* parse(Env& env,
               const vector<string>& filenames,
               const vector<string>& datafiles,
               const vector<string>& ip,
               bool ignoreStdlib,
               bool parseDocComments,
               bool verbose,
               ostream& err) {

    if (filenames.empty()) {
      err << "Error: no model given" << std::endl;
      return NULL;
    }

    Model* model;
    {
      GCLock lock;
      model = new Model();
    }
    parse(env, model, filenames, datafiles,
          ip, ignoreStdlib, parseDocComments, verbose, err);
    return model;
  }

  Model* parseData(Env& env,
                   Model* model,
                   const vector<string>& datafiles,
                   const vector<string>& includePaths,
                   bool ignoreStdlib,
                   bool parseDocComments,
                   bool verbose,
                   ostream& err) {
    
    vector<string> filenames;
    parse(env, model, filenames, datafiles, includePaths,
          ignoreStdlib, parseDocComments, verbose, err);
    return model;
  }


}




# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* In a future release of Bison, this section will be replaced
   by #include "parser.tab.hh".  */
#ifndef YY_YY_USERS_JDEKKER_REPOSITORIES_MINIZINC_LIBMZN_BUILD_MINIZINC_PARSER_TAB_HH_INCLUDED
# define YY_YY_USERS_JDEKKER_REPOSITORIES_MINIZINC_LIBMZN_BUILD_MINIZINC_PARSER_TAB_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
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
    MZN_INVALID_NULL = 320,
    MZN_EQUIV = 321,
    MZN_IMPL = 322,
    MZN_RIMPL = 323,
    MZN_OR = 324,
    MZN_XOR = 325,
    MZN_AND = 326,
    MZN_LE = 327,
    MZN_GR = 328,
    MZN_LQ = 329,
    MZN_GQ = 330,
    MZN_EQ = 331,
    MZN_NQ = 332,
    MZN_IN = 333,
    MZN_SUBSET = 334,
    MZN_SUPERSET = 335,
    MZN_UNION = 336,
    MZN_DIFF = 337,
    MZN_SYMDIFF = 338,
    MZN_DOTDOT = 339,
    MZN_PLUS = 340,
    MZN_MINUS = 341,
    MZN_MULT = 342,
    MZN_DIV = 343,
    MZN_IDIV = 344,
    MZN_MOD = 345,
    MZN_INTERSECT = 346,
    MZN_NOT = 347,
    MZN_PLUSPLUS = 348,
    MZN_COLONCOLON = 349,
    PREC_ANNO = 350,
    MZN_EQUIV_QUOTED = 351,
    MZN_IMPL_QUOTED = 352,
    MZN_RIMPL_QUOTED = 353,
    MZN_OR_QUOTED = 354,
    MZN_XOR_QUOTED = 355,
    MZN_AND_QUOTED = 356,
    MZN_LE_QUOTED = 357,
    MZN_GR_QUOTED = 358,
    MZN_LQ_QUOTED = 359,
    MZN_GQ_QUOTED = 360,
    MZN_EQ_QUOTED = 361,
    MZN_NQ_QUOTED = 362,
    MZN_IN_QUOTED = 363,
    MZN_SUBSET_QUOTED = 364,
    MZN_SUPERSET_QUOTED = 365,
    MZN_UNION_QUOTED = 366,
    MZN_DIFF_QUOTED = 367,
    MZN_SYMDIFF_QUOTED = 368,
    MZN_DOTDOT_QUOTED = 369,
    MZN_PLUS_QUOTED = 370,
    MZN_MINUS_QUOTED = 371,
    MZN_MULT_QUOTED = 372,
    MZN_DIV_QUOTED = 373,
    MZN_IDIV_QUOTED = 374,
    MZN_MOD_QUOTED = 375,
    MZN_INTERSECT_QUOTED = 376,
    MZN_NOT_QUOTED = 377,
    MZN_COLONCOLON_QUOTED = 378,
    MZN_PLUSPLUS_QUOTED = 379
  };
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



int yyparse (void *parm);

#endif /* !YY_YY_USERS_JDEKKER_REPOSITORIES_MINIZINC_LIBMZN_BUILD_MINIZINC_PARSER_TAB_HH_INCLUDED  */

/* Copy the second part of user declarations.  */



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
#else
typedef signed char yytype_int8;
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
# elif ! defined YYSIZE_T
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
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
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

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  155
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   4430

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  133
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  66
/* YYNRULES -- Number of rules.  */
#define YYNRULES  283
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  483

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   379

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     128,   129,     2,     2,   130,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   131,   125,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   126,   132,   127,     2,     2,     2,     2,
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
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   618,   618,   620,   622,   625,   634,   643,   652,   661,
     664,   672,   681,   681,   683,   699,   703,   705,   707,   708,
     710,   712,   714,   716,   718,   722,   745,   751,   758,   766,
     776,   790,   791,   795,   799,   805,   809,   814,   819,   826,
     830,   838,   848,   855,   864,   876,   884,   885,   890,   891,
     893,   898,   899,   903,   907,   912,   912,   915,   917,   921,
     926,   930,   932,   936,   937,   943,   952,   955,   963,   971,
     980,   989,   998,  1011,  1012,  1016,  1018,  1020,  1022,  1024,
    1026,  1028,  1033,  1039,  1042,  1044,  1050,  1051,  1053,  1055,
    1057,  1059,  1068,  1077,  1079,  1081,  1083,  1085,  1087,  1089,
    1091,  1093,  1099,  1101,  1114,  1115,  1117,  1119,  1121,  1123,
    1125,  1127,  1129,  1131,  1133,  1135,  1137,  1139,  1141,  1143,
    1145,  1147,  1149,  1151,  1153,  1155,  1164,  1173,  1182,  1191,
    1193,  1195,  1197,  1199,  1201,  1203,  1205,  1207,  1213,  1215,
    1222,  1233,  1235,  1237,  1239,  1242,  1244,  1247,  1249,  1251,
    1253,  1255,  1256,  1258,  1259,  1262,  1263,  1266,  1267,  1270,
    1271,  1274,  1275,  1278,  1279,  1282,  1283,  1284,  1289,  1291,
    1297,  1302,  1310,  1317,  1326,  1328,  1333,  1339,  1342,  1345,
    1347,  1351,  1353,  1355,  1358,  1361,  1363,  1367,  1369,  1373,
    1375,  1386,  1397,  1437,  1440,  1445,  1452,  1457,  1461,  1467,
    1474,  1490,  1491,  1495,  1497,  1499,  1501,  1503,  1505,  1507,
    1509,  1511,  1513,  1515,  1517,  1519,  1521,  1523,  1525,  1527,
    1529,  1531,  1533,  1535,  1537,  1539,  1541,  1543,  1545,  1547,
    1551,  1559,  1591,  1593,  1594,  1614,  1670,  1673,  1676,  1679,
    1681,  1685,  1692,  1701,  1703,  1711,  1713,  1722,  1722,  1725,
    1731,  1742,  1743,  1746,  1750,  1754,  1756,  1758,  1760,  1762,
    1764,  1766,  1768,  1770,  1772,  1774,  1776,  1778,  1780,  1782,
    1784,  1786,  1788,  1790,  1792,  1794,  1796,  1798,  1800,  1802,
    1804,  1806,  1808,  1810
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
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
  "\"null character\"", "\"<->\"", "\"->\"", "\"<-\"", "\"\\\\/\"",
  "\"xor\"", "\"/\\\\\"", "\"<\"", "\">\"", "\"<=\"", "\">=\"", "\"=\"",
  "\"!=\"", "\"in\"", "\"subset\"", "\"superset\"", "\"union\"",
  "\"diff\"", "\"symdiff\"", "\"..\"", "\"+\"", "\"-\"", "\"*\"", "\"/\"",
  "\"div\"", "\"mod\"", "\"intersect\"", "\"not\"", "\"++\"", "\"::\"",
  "PREC_ANNO", "\"'<->'\"", "\"'->'\"", "\"'<-'\"", "\"'\\\\/'\"",
  "\"'xor'\"", "\"'/\\\\'\"", "\"'<'\"", "\"'>'\"", "\"'<='\"", "\"'>='\"",
  "\"'='\"", "\"'!='\"", "\"'in'\"", "\"'subset'\"", "\"'superset'\"",
  "\"'union'\"", "\"'diff'\"", "\"'symdiff'\"", "\"'..'\"", "\"'+'\"",
  "\"'-'\"", "\"'*'\"", "\"'/'\"", "\"'div'\"", "\"'mod'\"",
  "\"'intersect'\"", "\"'not'\"", "\"'::'\"", "\"'++'\"", "';'", "'{'",
  "'}'", "'('", "')'", "','", "':'", "'|'", "$accept", "model",
  "item_list", "item_list_head", "doc_file_comments", "semi_or_none",
  "item", "item_tail", "include_item", "vardecl_item", "enum_id_list",
  "assign_item", "constraint_item", "solve_item", "output_item",
  "predicate_item", "function_item", "annotation_item",
  "operation_item_tail", "params", "params_list", "params_list_head",
  "comma_or_none", "ti_expr_and_id_or_anon", "ti_expr_and_id",
  "ti_expr_list", "ti_expr_list_head", "ti_expr", "base_ti_expr",
  "opt_opt", "base_ti_expr_tail", "expr_list", "expr_list_head",
  "set_expr", "expr", "expr_atom_head", "string_expr", "string_quote_rest",
  "array_access_tail", "set_literal", "set_comp", "comp_tail",
  "generator_list", "generator_list_head", "generator", "id_list",
  "id_list_head", "simple_array_literal", "simple_array_literal_2d",
  "simple_array_literal_3d_list", "simple_array_literal_2d_list",
  "simple_array_comp", "if_then_else_expr", "elseif_list", "quoted_op",
  "quoted_op_call", "call_expr", "comp_or_expr", "comp_or_expr_head",
  "let_expr", "let_vardecl_item_list", "comma_or_semi", "let_vardecl_item",
  "annotations", "ne_annotations", "id_or_quoted_op", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
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
     375,   376,   377,   378,   379,    59,   123,   125,    40,    41,
      44,    58,   124
};
# endif

#define YYPACT_NINF -374

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-374)))

#define YYTABLE_NINF -74

#define yytable_value_is_error(Yytable_value) \
  (!!((Yytable_value) == (-74)))

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     746,   -97,  -374,  -374,  -374,   -32,  -374,  3016,  -374,  -374,
    1504,  -374,   -10,   -10,  -374,  -374,    32,   -15,  -374,  3016,
      48,  -374,  2008,  3016,    37,  -374,  -374,   -71,    15,  2512,
    3016,    51,   -36,  -374,    53,     3,  2638,   486,  3268,  3268,
    -374,  -374,  -374,  -374,  -374,  -374,  -374,  -374,  -374,  -374,
    -374,  -374,  -374,  -374,  -374,  -374,  -374,  -374,   -66,  -374,
    -374,  -374,  -374,  -374,  -374,  -374,  -374,  -374,  2764,  3016,
      64,  -374,   -60,  1252,  -374,  -374,  -374,  -374,  -374,  -374,
    -374,  -374,  -374,  -374,  -374,   -36,   -65,  -374,    21,  -374,
      34,  -374,  -374,     3,     3,     3,     3,     3,     3,   -61,
    -374,     3,  -374,  1378,  3016,  3016,   999,    19,   -27,  3142,
    3016,  3016,  3016,   -51,    11,  4336,  -374,  -374,  -374,  -374,
    2260,  2386,   -48,  2008,  4336,     5,   -49,  4045,  -374,  1756,
    2134,  -374,  4336,   -48,  3303,    -4,   -11,   -48,    19,  -374,
      25,   -45,  3384,  -374,   616,  -374,   -33,   -34,    12,    12,
    3016,  -374,   -39,  3419,  3647,  -374,  1126,  -374,  -374,  -374,
      13,    88,    54,  3268,  3268,  3268,  3268,  3268,  3268,  3268,
    3268,  3268,  3268,  3268,  3268,  3268,  3303,    19,    19,    19,
      19,    19,    19,  3016,    19,  -374,    43,  4336,  -374,  4081,
     -22,   -16,  3016,   276,    36,    36,    36,  3016,  3016,  -374,
    3016,  3016,  3016,  3016,  3016,  3016,  3016,  3016,  3016,  3016,
    3016,  3016,  3016,  3016,  3016,  3016,  3016,  3016,  3016,  3016,
    3142,  3016,  3016,  3016,  3016,  3016,  3016,  3016,  3016,  3303,
      67,  -374,    91,  -374,   872,    61,    83,    22,  -374,     0,
    3881,  3016,  -374,   -36,    23,   -96,  -374,  -374,   -36,  -374,
    3016,  3016,  -374,  3303,   -36,  -374,  3016,  -374,   141,  -374,
      24,  -374,    26,  -374,  2890,  3533,  -374,   141,     3,  1252,
    -374,  3016,    31,  2512,    57,   412,   412,   412,   423,    46,
      46,    12,    12,    12,    12,   412,     6,  -374,  3504,  -374,
    3016,    33,  3016,  -374,    94,  3618,  -374,  4336,    68,   253,
     736,   736,   604,   604,   862,   988,   988,   988,   988,   988,
     988,   361,   361,   361,   542,   542,   542,   276,    56,    56,
      36,    36,    36,    36,   542,    16,  -374,  2512,  2512,    40,
      41,    35,  -374,  -374,    23,  3016,   122,  1882,  -374,    38,
     165,  -374,  -374,  -374,  -374,  -374,  -374,  -374,  -374,  -374,
    -374,  -374,  -374,  -374,  -374,  -374,  -374,  -374,  -374,  -374,
    -374,  -374,  -374,  -374,  -374,  -374,  -374,  -374,  -374,  -374,
     -48,  4166,    96,   168,  -374,    97,  -374,  1630,   104,  4336,
    4336,  -374,   104,   140,   158,  -374,    89,  -374,   142,    93,
    3016,  3016,  -374,  -374,  3016,    98,    19,  -374,  4336,  1882,
    -374,  -374,  3016,  4336,  3016,  4133,  -374,  3016,  -374,  -374,
    -374,  -374,  1882,  -374,  4336,  2134,  -374,  3016,  -374,   -90,
     -36,  -374,    20,  3016,  -374,  3016,   143,  -374,  -374,  3016,
    -374,  -374,  3016,  -374,   141,  -374,  3016,   212,  -374,    92,
    3732,  -374,    99,  3761,  3846,  3016,  3875,  -374,  -374,  3960,
    -374,   220,   104,  3016,  3016,  4336,  4336,  3016,  4336,  4336,
    -374,  4218,  -374,  3016,  -374,   -36,  -374,  -374,  4336,  -374,
    -374,  -374,  -374,  4251,  4303,  4336,  3016,   104,  -374,  3016,
    4336,  -374,  4336
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,   148,   147,   150,   143,   168,     0,    81,    82,
      73,    10,    73,    73,   152,    79,     0,     0,    76,     0,
       0,    77,    73,     0,     0,   149,    75,     0,     0,    74,
       0,     0,   251,    78,     0,   145,     0,     0,     0,     0,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,     0,   221,
     222,   223,   224,   225,   226,   227,   229,   228,     0,     0,
       0,     2,    12,    73,     5,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,   251,     0,    63,     0,    66,
      80,    86,   151,   153,   155,   157,   159,   161,   163,     0,
     233,   166,   165,    73,     0,     0,     0,   144,   143,   124,
       0,     0,     0,     0,     0,    84,   104,   169,    14,    74,
       0,     0,    48,    73,    35,    28,     0,     0,    25,    73,
      73,    67,    39,    48,     0,     0,   252,    48,   146,   187,
       0,    55,    84,   189,     0,   196,     0,     0,   102,   103,
       0,   174,     0,    84,     0,     1,    13,     4,    11,     6,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   154,   156,   158,
     160,   162,   164,     0,   167,     9,     0,    34,   232,   237,
       0,    55,     0,   125,   139,   140,   138,     0,     0,   170,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     126,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    69,     0,    68,     0,    44,     0,    55,    61,     0,
       0,     0,   244,   251,     0,     0,   243,    65,   251,   253,
       0,     0,    36,     0,   251,   188,    56,    83,     0,   193,
       0,   192,     0,   190,     0,     0,   175,     0,   141,    73,
       7,     0,    59,     0,   101,    88,    89,    90,    91,    95,
      96,    97,    98,    99,   100,    93,    94,    87,     0,   172,
       0,   234,    56,   236,     0,     0,   171,    85,   137,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   127,   131,   132,
     133,   134,   135,   136,   129,   130,   105,     0,     0,     0,
       0,    55,    53,    57,    58,     0,     0,    56,    60,     0,
      31,   255,   256,   257,   258,   259,   260,   261,   262,   263,
     264,   265,   266,   267,   268,   269,   270,   271,   272,   273,
     274,   275,   276,   277,   278,   279,   280,   281,   282,   283,
      48,   201,   249,     0,   248,     0,   247,    73,    46,    38,
      37,   254,    46,   185,     0,   177,    55,   179,     0,    55,
     194,     0,   191,   197,     0,     0,   142,     8,    27,    51,
      70,   231,     0,   238,     0,   239,   173,     0,    72,    71,
      50,    49,    56,    52,    45,    73,    62,     0,    32,     0,
     251,   199,     0,     0,    59,     0,     0,   246,   245,     0,
      40,    41,     0,   198,    56,   178,     0,    56,   184,     0,
       0,   176,     0,     0,     0,     0,     0,    54,    64,     0,
      29,     0,    46,     0,     0,   250,   241,     0,    47,   183,
     180,   181,   186,   195,    92,   251,   230,   235,   240,   128,
      30,    33,    42,     0,     0,   242,     0,    46,   200,     0,
     182,    43,   202
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -374,  -374,  -374,  -374,    71,  -374,   -57,   223,  -374,  -374,
    -374,  -374,  -124,  -374,  -374,  -374,  -374,  -374,  -373,  -119,
    -169,  -374,  -176,  -178,  -121,  -374,  -374,   -20,  -127,    39,
      -9,   -35,    17,   313,   -19,   138,  -374,    42,   -23,  -374,
    -374,   -31,  -374,  -374,  -199,  -374,  -374,  -374,  -374,  -374,
    -137,  -374,  -374,  -374,  -374,  -374,  -374,  -374,  -374,  -374,
    -374,  -374,  -139,   -75,  -374,  -374
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    70,    71,    72,    73,   157,    74,    75,    76,    77,
     419,    78,    79,    80,    81,    82,    83,    84,   430,   235,
     330,   331,   257,   332,    85,   236,   237,    86,    87,    88,
      89,   145,   141,    90,   115,   116,    92,   117,   107,    93,
      94,   384,   385,   386,   387,   388,   389,    95,    96,   146,
     147,    97,    98,   422,    99,   100,   101,   190,   191,   102,
     245,   377,   246,   135,   136,   370
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     124,   140,   126,   247,   127,   242,   339,   260,   243,   431,
     160,   132,   138,   163,   248,   293,   159,   142,   254,   163,
     131,   198,   199,   201,   114,   104,   263,   261,   103,   374,
     104,   375,   119,   152,   376,   250,   251,   450,   122,   252,
     451,   163,   123,   201,   105,   128,   185,   453,   454,   153,
     154,   120,   121,   163,   125,   129,   130,   133,   134,   137,
     104,   338,   150,   201,   155,   156,   161,   183,   162,   186,
     177,   178,   179,   180,   181,   182,   192,   197,   184,   472,
     234,   239,   240,   253,   255,   256,   187,   189,   266,   271,
     193,   194,   195,   196,   272,   273,   106,   262,   264,   270,
     176,   106,   289,   238,   481,   175,   176,   291,   327,   244,
     229,   231,   233,   333,   292,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   340,   175,   176,   228,
     229,   265,   328,   170,   171,   172,   173,   335,    91,   175,
     176,   200,   336,   223,   224,   225,   226,   383,    91,   228,
     229,   176,   337,   406,   373,   413,   390,   294,   391,   399,
      91,   404,   229,   415,   288,   412,   417,    91,   372,   410,
     411,   418,   423,   378,   424,   425,    91,    91,   295,   382,
     429,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   308,   309,   310,   311,   312,   313,   314,   315,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     435,    91,   397,   438,   334,   114,   432,   433,   462,   434,
     436,   457,   371,   437,   463,   441,   471,   269,   465,   393,
     442,   379,   380,   118,   447,   460,   395,   297,   428,     0,
     296,    91,     0,     0,     0,   396,     0,     0,     0,     0,
       0,   420,   398,   427,   439,     0,   243,     0,    91,    91,
     201,    91,     0,     0,   400,     0,     0,    91,    91,     0,
       0,   403,   249,   405,     0,     0,     0,     0,   333,     0,
       0,     0,     0,   201,     0,     0,     0,     0,   448,     0,
       0,   333,     0,     0,    91,     0,     0,     0,     0,     0,
       0,    91,    91,    91,    91,    91,    91,    91,    91,    91,
      91,    91,    91,    91,   287,     0,   414,   416,   408,   409,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   452,   228,   229,     0,     0,
       0,   148,   149,     0,     0,   393,     0,   244,     0,     0,
     -74,   221,   222,   223,   224,   225,   226,   326,   201,   228,
     229,     0,    91,     0,     0,   440,     0,     0,     0,   334,
       0,     0,     0,   443,     0,   444,     0,     0,   446,     0,
     477,   381,   334,     0,     0,     0,     0,     0,   449,     0,
       0,     0,     0,     0,   455,     0,   456,    91,     0,     0,
     458,    91,     0,   459,     0,     0,     0,   461,     0,   163,
       0,     0,     0,     0,     0,     0,   468,     0,   393,     0,
     163,     0,     0,     0,   473,   474,     0,     0,   475,   -74,
     -74,   -74,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,     0,   228,   229,     0,   480,     0,     0,
     482,     0,     0,     0,     0,    91,    91,     0,     0,     0,
       0,     0,     0,     0,     0,    91,   274,   275,   276,   277,
     278,   279,   280,   281,   282,   283,   284,   285,   286,     2,
       3,     4,   108,     0,     6,     7,   167,   168,   169,   170,
     171,   172,   173,     0,    14,   175,   176,   -74,   168,   169,
     170,   171,   172,   173,     0,    91,   175,   176,     0,    23,
       0,    25,     0,    27,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    91,     0,     0,
      35,     0,     0,    36,    37,     0,   143,     0,     0,   201,
      91,     0,     0,    91,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     109,   110,   111,     0,     0,     0,     0,     0,   112,     0,
       0,     0,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
     113,    59,    60,    61,    62,    63,    64,    65,    66,     0,
      67,   201,    68,     0,    69,     0,     0,     0,   144,     2,
       3,     4,   108,     0,     6,     7,   220,   221,   222,   223,
     224,   225,   226,     0,    14,   228,   229,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    23,
       0,    25,     0,    27,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      35,     0,     0,    36,    37,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,     0,   228,   229,     0,
     109,   110,   111,     0,     0,     0,     0,     0,   112,     0,
       0,     0,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
     113,    59,    60,    61,    62,    63,    64,    65,    66,     0,
      67,     0,    68,   201,    69,     0,    -3,     1,   259,     2,
       3,     4,     5,     0,     6,     7,     0,     0,     8,     9,
      10,    11,    12,    13,    14,    15,    16,     0,    17,    18,
       0,    19,     0,     0,     0,     0,    20,    21,    22,    23,
      24,    25,    26,    27,    28,     0,     0,     0,    29,     0,
      30,    31,     0,   -73,    32,    33,    34,     0,     0,     0,
      35,     0,     0,    36,    37,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,   224,   225,   226,   227,     0,   228,
     229,    38,    39,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,   201,
      67,     0,    68,   329,    69,     2,     3,     4,   108,     0,
       6,     7,     0,     0,     8,     9,     0,     0,    12,    13,
      14,    15,     0,     0,    17,    18,     0,     0,     0,     0,
       0,     0,     0,    21,     0,    23,     0,    25,    26,    27,
      28,     0,     0,     0,    29,     0,     0,     0,     0,   -73,
       0,    33,     0,     0,     0,     0,    35,     0,     0,    36,
      37,     0,     0,     0,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,     0,   228,   229,    38,    39,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,   201,    67,     0,    68,     0,
      69,   -51,     2,     3,     4,   108,     0,     6,     7,     0,
       0,     0,     0,     0,     0,     0,     0,    14,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    23,     0,    25,     0,    27,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    35,     0,     0,    36,    37,     0,     0,
     -74,   -74,   -74,   -74,   -74,   -74,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
       0,   228,   229,   109,   110,   111,     0,     0,     0,     0,
       0,   112,     0,     0,     0,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,   113,    59,    60,    61,    62,    63,    64,
      65,    66,     0,    67,     0,    68,     0,    69,   188,     2,
       3,     4,     5,     0,     6,     7,     0,     0,     8,     9,
      10,    11,    12,    13,    14,    15,    16,     0,    17,    18,
       0,    19,     0,     0,     0,     0,    20,    21,    22,    23,
      24,    25,    26,    27,    28,     0,     0,     0,    29,     0,
      30,    31,     0,   -73,    32,    33,    34,     0,     0,     0,
      35,     0,     0,    36,    37,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    38,    39,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,     0,
      67,     0,    68,     0,    69,     2,     3,     4,     5,     0,
       6,     7,     0,     0,     8,     9,    10,   158,    12,    13,
      14,    15,    16,     0,    17,    18,     0,    19,     0,     0,
       0,     0,    20,    21,    22,    23,    24,    25,    26,    27,
      28,     0,     0,     0,    29,     0,    30,    31,     0,     0,
      32,    33,    34,     0,     0,     0,    35,     0,     0,    36,
      37,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    38,    39,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,     0,    67,     0,    68,     0,
      69,     2,     3,     4,     5,     0,     6,     7,     0,     0,
       8,     9,    10,     0,    12,    13,    14,    15,    16,     0,
      17,    18,     0,    19,     0,     0,     0,     0,    20,    21,
      22,    23,    24,    25,    26,    27,    28,     0,     0,     0,
      29,     0,    30,    31,     0,     0,    32,    33,    34,     0,
       0,     0,    35,     0,     0,    36,    37,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    38,    39,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,     0,    67,     0,    68,     0,    69,     2,     3,     4,
       5,     0,     6,     7,     0,     0,     8,     9,     0,     0,
      12,    13,    14,    15,    16,     0,    17,    18,     0,    19,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,    27,    28,     0,     0,     0,    29,     0,    30,    31,
       0,     0,    32,    33,    34,     0,     0,     0,    35,     0,
       0,    36,    37,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    38,
      39,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,     0,    67,     0,
      68,     0,    69,     2,     3,     4,   108,     0,     6,     7,
       0,     0,     8,     9,     0,     0,    12,    13,    14,    15,
       0,     0,    17,    18,     0,    19,     0,     0,     0,     0,
       0,    21,     0,    23,     0,    25,    26,    27,    28,     0,
       0,     0,    29,     0,     0,     0,     0,     0,     0,    33,
       0,     0,     0,     0,    35,     0,     0,    36,    37,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    38,    39,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,     0,    67,     0,    68,   426,    69,     2,
       3,     4,   108,     0,     6,     7,     0,     0,     8,     9,
       0,     0,    12,    13,    14,    15,     0,     0,    17,    18,
       0,    19,     0,     0,     0,     0,     0,    21,     0,    23,
       0,    25,    26,    27,    28,     0,     0,     0,    29,     0,
       0,     0,     0,     0,     0,    33,     0,     0,     0,     0,
      35,     0,     0,    36,    37,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    38,    39,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,     0,
      67,     0,    68,     0,    69,     2,     3,     4,   108,     0,
       6,     7,     0,     0,     8,     9,     0,     0,    12,    13,
      14,    15,     0,     0,    17,    18,     0,     0,     0,     0,
       0,     0,     0,    21,     0,    23,     0,    25,    26,    27,
      28,     0,     0,     0,    29,     0,     0,     0,     0,   -73,
       0,    33,     0,     0,     0,     0,    35,     0,     0,    36,
      37,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    38,    39,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,     0,    67,     0,    68,     0,
      69,     2,     3,     4,   108,     0,     6,     7,     0,     0,
       8,     9,     0,     0,    12,    13,    14,    15,     0,     0,
      17,    18,     0,     0,     0,     0,     0,     0,     0,    21,
       0,    23,     0,    25,    26,    27,    28,     0,     0,     0,
      29,     0,     0,     0,     0,     0,     0,    33,     0,     0,
       0,     0,    35,     0,     0,    36,    37,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    38,    39,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,     0,    67,     0,    68,     0,    69,     2,     3,     4,
     108,     0,     6,     7,     0,     0,     8,     9,     0,     0,
      12,    13,    14,    15,     0,     0,     0,    18,     0,     0,
       0,     0,     0,     0,     0,    21,     0,    23,     0,    25,
      26,    27,     0,     0,     0,     0,    29,     0,     0,     0,
       0,     0,     0,    33,     0,     0,     0,     0,    35,     0,
       0,    36,    37,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    38,
      39,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,     0,    67,     0,
      68,     0,    69,     2,     3,     4,   108,     0,     6,     7,
       0,     0,     8,     9,     0,     0,     0,     0,    14,    15,
       0,     0,     0,    18,     0,     0,     0,     0,     0,     0,
       0,    21,     0,    23,     0,    25,    26,    27,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   230,     0,    33,
       0,     0,     0,     0,    35,     0,     0,    36,    37,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    38,    39,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,     0,    67,     0,    68,     0,    69,     2,
       3,     4,   108,     0,     6,     7,     0,     0,     8,     9,
       0,     0,     0,     0,    14,    15,     0,     0,     0,    18,
       0,     0,     0,     0,     0,     0,     0,    21,     0,    23,
       0,    25,    26,    27,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   232,     0,    33,     0,     0,     0,     0,
      35,     0,     0,    36,    37,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    38,    39,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,     0,
      67,     0,    68,     0,    69,     2,     3,     4,   108,     0,
       6,     7,     0,     0,     8,     9,     0,     0,     0,     0,
      14,    15,     0,     0,     0,    18,     0,     0,     0,     0,
       0,     0,     0,    21,     0,    23,     0,    25,    26,    27,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    33,     0,     0,     0,     0,    35,     0,     0,    36,
      37,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    38,    39,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,     0,    67,     0,    68,     0,
      69,     2,     3,     4,   108,     0,     6,     7,     0,     0,
       0,     0,     0,     0,     0,     0,    14,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    23,     0,    25,     0,    27,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    35,     0,     0,    36,    37,   139,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   109,   110,   111,     0,     0,     0,     0,     0,
     112,     0,     0,     0,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,   113,    59,    60,    61,    62,    63,    64,    65,
      66,     0,    67,     0,    68,     0,    69,     2,     3,     4,
     108,     0,     6,     7,     0,     0,     0,     0,     0,     0,
       0,     0,    14,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    23,     0,    25,
       0,    27,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    35,     0,
       0,    36,    37,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   109,   110,
     111,     0,     0,     0,     0,     0,   112,     0,     0,     0,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,   113,    59,
      60,    61,    62,    63,    64,    65,    66,     0,    67,     0,
      68,   151,    69,     2,     3,     4,   108,     0,     6,     7,
       0,     0,     0,     0,     0,     0,     0,     0,    14,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    23,     0,    25,     0,    27,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    35,     0,     0,    36,    37,     0,
     392,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   109,   110,   111,     0,     0,     0,
       0,     0,   112,     0,     0,     0,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,   113,    59,    60,    61,    62,    63,
      64,    65,    66,     0,    67,     0,    68,     0,    69,     2,
       3,     4,   108,     0,     6,     7,     0,     0,     0,     0,
       0,     0,     0,     0,    14,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    23,
       0,    25,     0,    27,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      35,     0,     0,    36,    37,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     109,   110,   111,     0,     0,     0,     0,     0,   112,     0,
       0,     0,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
     113,    59,    60,    61,    62,    63,    64,    65,    66,     0,
      67,     0,    68,     0,    69,     2,     3,     4,   108,     0,
       6,     7,     0,     0,     0,     0,     0,     0,     0,     0,
      14,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    23,     0,    25,     0,    27,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    35,     0,     0,    36,
      37,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   -74,   110,   111,     0,
       0,     0,     0,     0,   112,     0,     0,     0,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,   113,    59,    60,    61,
      62,    63,    64,    65,    66,     0,    67,     0,    68,     0,
      69,     2,     3,     4,   108,     0,     6,     7,     0,     0,
       0,     0,     0,     0,     0,     0,    14,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    23,     0,    25,     0,    27,     2,     3,     4,   108,
       0,     6,     7,     0,     0,     0,     0,     0,     0,     0,
       0,    14,    35,     0,     0,    36,    37,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    23,     0,    25,     0,
      27,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    38,    39,     0,     0,    35,     0,     0,
      36,    37,     0,     0,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,   201,    67,     0,    68,     0,    69,     0,     0,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,     0,    59,    60,
      61,    62,    63,    64,    65,    66,   201,    67,     0,    68,
       0,    69,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,     0,   228,   229,     0,
       0,     0,     0,     0,     0,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,   224,   225,   226,
     227,   201,   228,   229,     0,     0,   258,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     201,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   267,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,     0,   228,   229,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   201,   228,   229,     0,     0,
       0,     0,     0,   401,   402,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   201,     0,     0,     0,     0,     0,
       0,     0,     0,   394,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
       0,   228,   229,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   201,
     228,   229,     0,     0,     0,     0,     0,     0,   407,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   201,     0,
       0,     0,     0,     0,     0,     0,   268,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,     0,   228,   229,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   201,   228,   229,     0,     0,     0,     0,
       0,   464,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   201,     0,     0,     0,     0,   341,     0,     0,
     466,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,   224,   225,   226,   227,     0,   228,
     229,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   201,   228,   229,
       0,     0,     0,     0,     0,   467,     0,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   469,   369,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   201,   228,   229,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   201,   470,
       0,     0,     0,     0,     0,     0,   241,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   290,   228,   229,
     201,     0,     0,     0,     0,     0,     0,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   201,   228,   229,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   445,
       0,     0,     0,     0,     0,   421,     0,     0,     0,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   201,   228,   229,     0,     0,
       0,     0,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,   224,   225,   226,   227,   201,   228,
     229,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   476,     0,     0,     0,     0,     0,
     478,     0,     0,     0,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     201,   228,   229,     0,     0,     0,     0,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   201,   228,   229,     0,     0,     0,     0,
       0,     0,     0,     0,   479,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,     0,   228,   229,     0,     0,
       0,     0,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,   224,   225,   226,   227,     0,   228,
     229
};

static const yytype_int16 yycheck[] =
{
      19,    36,    22,   130,    23,   129,     6,   144,   129,   382,
      85,    30,    35,     7,   133,   191,    73,    36,   137,     7,
      29,    10,    11,     7,     7,    57,    60,    60,   125,   125,
      57,   127,    42,    68,   130,    39,    40,   127,     6,    43,
     130,     7,    57,     7,    76,     8,   103,    27,    28,    68,
      69,    12,    13,     7,     6,   126,    41,     6,    94,     6,
      57,   237,   128,     7,     0,   125,   131,   128,    47,   104,
      93,    94,    95,    96,    97,    98,    57,   128,   101,   452,
     128,    76,   131,    94,    59,   130,   105,   106,   127,    76,
     109,   110,   111,   112,     6,    41,   128,   130,   132,   156,
      94,   128,    59,   123,   477,    93,    94,   129,    41,   129,
      94,   120,   121,   234,   130,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,   126,    93,    94,    93,
      94,   150,    41,    87,    88,    89,    90,    76,     0,    93,
      94,   130,    59,    87,    88,    89,    90,     6,    10,    93,
      94,    94,   130,    59,   131,   331,   132,   192,   132,   128,
      22,   128,    94,    41,   183,   130,   128,    29,   243,   129,
     129,     6,    76,   248,     6,    78,    38,    39,   197,   254,
      76,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     386,    73,   269,   389,   234,   198,    76,    59,     6,   130,
      78,    78,   241,   130,   132,   127,     6,   156,   129,   264,
     399,   250,   251,    10,   412,   434,   267,   256,   377,    -1,
     198,   103,    -1,    -1,    -1,   268,    -1,    -1,    -1,    -1,
      -1,   370,   271,   377,   391,    -1,   377,    -1,   120,   121,
       7,   123,    -1,    -1,   273,    -1,    -1,   129,   130,    -1,
      -1,   290,   134,   292,    -1,    -1,    -1,    -1,   399,    -1,
      -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,   415,    -1,
      -1,   412,    -1,    -1,   156,    -1,    -1,    -1,    -1,    -1,
      -1,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,    -1,   335,   337,   327,   328,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,   420,    93,    94,    -1,    -1,
      -1,    38,    39,    -1,    -1,   390,    -1,   377,    -1,    -1,
      84,    85,    86,    87,    88,    89,    90,   229,     7,    93,
      94,    -1,   234,    -1,    -1,   394,    -1,    -1,    -1,   399,
      -1,    -1,    -1,   402,    -1,   404,    -1,    -1,   407,    -1,
     465,   253,   412,    -1,    -1,    -1,    -1,    -1,   417,    -1,
      -1,    -1,    -1,    -1,   423,    -1,   425,   269,    -1,    -1,
     429,   273,    -1,   432,    -1,    -1,    -1,   436,    -1,     7,
      -1,    -1,    -1,    -1,    -1,    -1,   445,    -1,   463,    -1,
       7,    -1,    -1,    -1,   453,   454,    -1,    -1,   457,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    -1,    93,    94,    -1,   476,    -1,    -1,
     479,    -1,    -1,    -1,    -1,   327,   328,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   337,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,     3,
       4,     5,     6,    -1,     8,     9,    84,    85,    86,    87,
      88,    89,    90,    -1,    18,    93,    94,    84,    85,    86,
      87,    88,    89,    90,    -1,   377,    93,    94,    -1,    33,
      -1,    35,    -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   399,    -1,    -1,
      54,    -1,    -1,    57,    58,    -1,    60,    -1,    -1,     7,
     412,    -1,    -1,   415,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      84,    85,    86,    -1,    -1,    -1,    -1,    -1,    92,    -1,
      -1,    -1,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,    -1,
     124,     7,   126,    -1,   128,    -1,    -1,    -1,   132,     3,
       4,     5,     6,    -1,     8,     9,    84,    85,    86,    87,
      88,    89,    90,    -1,    18,    93,    94,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,
      -1,    35,    -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      54,    -1,    -1,    57,    58,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    -1,    93,    94,    -1,
      84,    85,    86,    -1,    -1,    -1,    -1,    -1,    92,    -1,
      -1,    -1,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,    -1,
     124,    -1,   126,     7,   128,    -1,     0,     1,   132,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    -1,    22,    23,
      -1,    25,    -1,    -1,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    -1,    -1,    -1,    42,    -1,
      44,    45,    -1,    47,    48,    49,    50,    -1,    -1,    -1,
      54,    -1,    -1,    57,    58,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    -1,    93,
      94,    85,    86,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,     7,
     124,    -1,   126,     1,   128,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    12,    13,    -1,    -1,    16,    17,
      18,    19,    -1,    -1,    22,    23,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    31,    -1,    33,    -1,    35,    36,    37,
      38,    -1,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,
      -1,    49,    -1,    -1,    -1,    -1,    54,    -1,    -1,    57,
      58,    -1,    -1,    -1,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    -1,    93,    94,    85,    86,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,     7,   124,    -1,   126,    -1,
     128,   129,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    33,    -1,    35,    -1,    37,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,    -1,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      -1,    93,    94,    84,    85,    86,    -1,    -1,    -1,    -1,
      -1,    92,    -1,    -1,    -1,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,    -1,   124,    -1,   126,    -1,   128,   129,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    -1,    22,    23,
      -1,    25,    -1,    -1,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    -1,    -1,    -1,    42,    -1,
      44,    45,    -1,    47,    48,    49,    50,    -1,    -1,    -1,
      54,    -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    85,    86,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,    -1,
     124,    -1,   126,    -1,   128,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    -1,    22,    23,    -1,    25,    -1,    -1,
      -1,    -1,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    -1,    -1,    -1,    42,    -1,    44,    45,    -1,    -1,
      48,    49,    50,    -1,    -1,    -1,    54,    -1,    -1,    57,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    86,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,    -1,   124,    -1,   126,    -1,
     128,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      12,    13,    14,    -1,    16,    17,    18,    19,    20,    -1,
      22,    23,    -1,    25,    -1,    -1,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    -1,    -1,
      42,    -1,    44,    45,    -1,    -1,    48,    49,    50,    -1,
      -1,    -1,    54,    -1,    -1,    57,    58,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    85,    86,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,    -1,   124,    -1,   126,    -1,   128,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    12,    13,    -1,    -1,
      16,    17,    18,    19,    20,    -1,    22,    23,    -1,    25,
      -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    -1,    -1,    -1,    42,    -1,    44,    45,
      -1,    -1,    48,    49,    50,    -1,    -1,    -1,    54,    -1,
      -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      86,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,    -1,   124,    -1,
     126,    -1,   128,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    12,    13,    -1,    -1,    16,    17,    18,    19,
      -1,    -1,    22,    23,    -1,    25,    -1,    -1,    -1,    -1,
      -1,    31,    -1,    33,    -1,    35,    36,    37,    38,    -1,
      -1,    -1,    42,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      -1,    -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    85,    86,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,    -1,   124,    -1,   126,   127,   128,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    12,    13,
      -1,    -1,    16,    17,    18,    19,    -1,    -1,    22,    23,
      -1,    25,    -1,    -1,    -1,    -1,    -1,    31,    -1,    33,
      -1,    35,    36,    37,    38,    -1,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    -1,    -1,    49,    -1,    -1,    -1,    -1,
      54,    -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    85,    86,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,    -1,
     124,    -1,   126,    -1,   128,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    12,    13,    -1,    -1,    16,    17,
      18,    19,    -1,    -1,    22,    23,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    31,    -1,    33,    -1,    35,    36,    37,
      38,    -1,    -1,    -1,    42,    -1,    -1,    -1,    -1,    47,
      -1,    49,    -1,    -1,    -1,    -1,    54,    -1,    -1,    57,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    86,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,    -1,   124,    -1,   126,    -1,
     128,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      12,    13,    -1,    -1,    16,    17,    18,    19,    -1,    -1,
      22,    23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,
      -1,    33,    -1,    35,    36,    37,    38,    -1,    -1,    -1,
      42,    -1,    -1,    -1,    -1,    -1,    -1,    49,    -1,    -1,
      -1,    -1,    54,    -1,    -1,    57,    58,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    85,    86,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,    -1,   124,    -1,   126,    -1,   128,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    12,    13,    -1,    -1,
      16,    17,    18,    19,    -1,    -1,    -1,    23,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    31,    -1,    33,    -1,    35,
      36,    37,    -1,    -1,    -1,    -1,    42,    -1,    -1,    -1,
      -1,    -1,    -1,    49,    -1,    -1,    -1,    -1,    54,    -1,
      -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      86,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,    -1,   124,    -1,
     126,    -1,   128,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    12,    13,    -1,    -1,    -1,    -1,    18,    19,
      -1,    -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    31,    -1,    33,    -1,    35,    36,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    -1,    49,
      -1,    -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    85,    86,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,    -1,   124,    -1,   126,    -1,   128,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    12,    13,
      -1,    -1,    -1,    -1,    18,    19,    -1,    -1,    -1,    23,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,    33,
      -1,    35,    36,    37,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    47,    -1,    49,    -1,    -1,    -1,    -1,
      54,    -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    85,    86,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,    -1,
     124,    -1,   126,    -1,   128,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    12,    13,    -1,    -1,    -1,    -1,
      18,    19,    -1,    -1,    -1,    23,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    31,    -1,    33,    -1,    35,    36,    37,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    49,    -1,    -1,    -1,    -1,    54,    -1,    -1,    57,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    86,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,    -1,   124,    -1,   126,    -1,
     128,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    33,    -1,    35,    -1,    37,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    54,    -1,    -1,    57,    58,    59,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    84,    85,    86,    -1,    -1,    -1,    -1,    -1,
      92,    -1,    -1,    -1,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,    -1,   124,    -1,   126,    -1,   128,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,    35,
      -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,
      -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,
      86,    -1,    -1,    -1,    -1,    -1,    92,    -1,    -1,    -1,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,    -1,   124,    -1,
     126,   127,   128,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    33,    -1,    35,    -1,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,
      60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    85,    86,    -1,    -1,    -1,
      -1,    -1,    92,    -1,    -1,    -1,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,    -1,   124,    -1,   126,    -1,   128,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    18,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,
      -1,    35,    -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      54,    -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      84,    85,    86,    -1,    -1,    -1,    -1,    -1,    92,    -1,
      -1,    -1,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,    -1,
     124,    -1,   126,    -1,   128,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    33,    -1,    35,    -1,    37,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,    -1,    57,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    84,    85,    86,    -1,
      -1,    -1,    -1,    -1,    92,    -1,    -1,    -1,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,    -1,   124,    -1,   126,    -1,
     128,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    33,    -1,    35,    -1,    37,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    18,    54,    -1,    -1,    57,    58,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,    35,    -1,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    85,    86,    -1,    -1,    54,    -1,    -1,
      57,    58,    -1,    -1,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,     7,   124,    -1,   126,    -1,   128,    -1,    -1,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,    -1,   115,   116,
     117,   118,   119,   120,   121,   122,     7,   124,    -1,   126,
      -1,   128,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    -1,    93,    94,    -1,
      -1,    -1,    -1,    -1,    -1,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,     7,    93,    94,    -1,    -1,   132,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   132,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    -1,    93,    94,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,     7,    93,    94,    -1,    -1,
      -1,    -1,    -1,   129,   130,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   130,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      -1,    93,    94,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,     7,
      93,    94,    -1,    -1,    -1,    -1,    -1,    -1,   130,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   129,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    -1,    93,    94,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,     7,    93,    94,    -1,    -1,    -1,    -1,
      -1,   129,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     7,    -1,    -1,    -1,    -1,     6,    -1,    -1,
     129,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    -1,    93,
      94,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,     7,    93,    94,
      -1,    -1,    -1,    -1,    -1,   129,    -1,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   129,   124,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,     7,    93,    94,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,   129,
      -1,    -1,    -1,    -1,    -1,    -1,    51,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    56,    93,    94,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,     7,    93,    94,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    56,
      -1,    -1,    -1,    -1,    -1,    29,    -1,    -1,    -1,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,     7,    93,    94,    -1,    -1,
      -1,    -1,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,     7,    93,
      94,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    56,    -1,    -1,    -1,    -1,    -1,
      29,    -1,    -1,    -1,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
       7,    93,    94,    -1,    -1,    -1,    -1,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,     7,    93,    94,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    51,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    -1,    93,    94,    -1,    -1,
      -1,    -1,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    -1,    93,
      94
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     4,     5,     6,     8,     9,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    22,    23,    25,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    42,
      44,    45,    48,    49,    50,    54,    57,    58,    85,    86,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   124,   126,   128,
     134,   135,   136,   137,   139,   140,   141,   142,   144,   145,
     146,   147,   148,   149,   150,   157,   160,   161,   162,   163,
     166,   168,   169,   172,   173,   180,   181,   184,   185,   187,
     188,   189,   192,   125,    57,    76,   128,   171,     6,    84,
      85,    86,    92,   114,   165,   167,   168,   170,   140,    42,
     162,   162,     6,    57,   167,     6,   160,   167,     8,   126,
      41,   163,   167,     6,    94,   196,   197,     6,   171,    59,
     164,   165,   167,    60,   132,   164,   182,   183,   166,   166,
     128,   127,   164,   167,   167,     0,   125,   138,    15,   139,
     196,   131,    47,     7,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    93,    94,   171,   171,   171,
     171,   171,   171,   128,   171,   139,   164,   167,   129,   167,
     190,   191,    57,   167,   167,   167,   167,   128,    10,    11,
     130,     7,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    93,    94,
      47,   163,    47,   163,   128,   152,   158,   159,   160,    76,
     131,    51,   145,   157,   160,   193,   195,   161,   152,   168,
      39,    40,    43,    94,   152,    59,   130,   155,   132,   132,
     183,    60,   130,    60,   132,   167,   127,   132,   129,   137,
     139,    76,     6,    41,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   168,   167,    59,
      56,   129,   130,   155,   164,   167,   170,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   168,    41,    41,     1,
     153,   154,   156,   157,   160,    76,    59,   130,   155,     6,
     126,     6,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   124,
     198,   167,   196,   131,   125,   127,   130,   194,   196,   167,
     167,   168,   196,     6,   174,   175,   176,   177,   178,   179,
     132,   132,    60,   164,   130,   174,   171,   139,   167,   128,
     163,   129,   130,   167,   128,   167,    59,   130,   163,   163,
     129,   129,   130,   155,   167,    41,   160,   128,     6,   143,
     152,    29,   186,    76,     6,    78,   127,   145,   195,    76,
     151,   151,    76,    59,   130,   155,    78,   130,   155,   183,
     167,   127,   153,   167,   167,    56,   167,   156,   161,   167,
     127,   130,   196,    27,    28,   167,   167,    78,   167,   167,
     177,   167,     6,   132,   129,   129,   129,   129,   167,   129,
     129,     6,   151,   167,   167,   167,    56,   196,    29,    51,
     167,   151,   167
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   133,   134,   135,   135,   136,   136,   136,   136,   136,
     137,   137,   138,   138,   139,   139,   140,   140,   140,   140,
     140,   140,   140,   140,   140,   141,   142,   142,   142,   142,
     142,   143,   143,   143,   144,   145,   146,   146,   146,   147,
     148,   148,   149,   149,   150,   150,   151,   151,   152,   152,
     152,   153,   153,   154,   154,   155,   155,   156,   156,   157,
     158,   159,   159,   160,   160,   160,   161,   161,   161,   161,
     161,   161,   161,   162,   162,   163,   163,   163,   163,   163,
     163,   163,   163,   164,   165,   165,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   168,   168,   168,   168,   168,   168,   168,   168,   168,
     168,   168,   168,   168,   168,   168,   168,   168,   168,   168,
     168,   168,   168,   168,   168,   168,   168,   168,   169,   169,
     170,   170,   171,   171,   172,   172,   173,   174,   175,   176,
     176,   177,   177,   177,   178,   179,   179,   180,   180,   181,
     181,   181,   181,   182,   182,   182,   183,   183,   184,   185,
     185,   186,   186,   187,   187,   187,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   187,   187,   187,
     188,   188,   189,   189,   189,   189,   190,   191,   191,   191,
     191,   192,   192,   193,   193,   193,   193,   194,   194,   195,
     195,   196,   196,   197,   197,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     2,     3,     4,     3,
       1,     2,     0,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     4,     2,     6,
       7,     0,     1,     3,     3,     2,     3,     4,     4,     2,
       5,     5,     7,     8,     3,     5,     0,     2,     0,     3,
       3,     0,     2,     1,     3,     0,     1,     1,     1,     3,
       2,     1,     3,     1,     6,     3,     1,     2,     3,     3,
       4,     5,     5,     0,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     3,     1,     3,     3,     3,
       3,     3,     6,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     1,     2,     2,     3,     6,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     2,
       2,     3,     4,     1,     2,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     2,     1,     2,     1,
       2,     1,     2,     1,     2,     1,     1,     2,     1,     2,
       2,     3,     3,     4,     2,     3,     5,     1,     2,     1,
       3,     3,     5,     3,     2,     1,     3,     2,     3,     2,
       3,     4,     3,     2,     3,     5,     1,     3,     5,     5,
       8,     0,     5,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       6,     4,     3,     1,     4,     7,     2,     1,     3,     3,
       5,     6,     7,     1,     1,     3,     3,     1,     1,     2,
       4,     0,     1,     2,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
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

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


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

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  unsigned res = 0;
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

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location, parm); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void *parm)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
  YYUSE (parm);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void *parm)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, parm);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
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
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, void *parm)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       , parm);
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
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
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


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
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
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
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
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
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
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, void *parm)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (parm);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
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

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
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
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
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

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

/* User initialization code.  */

{
  GCLock lock;
  yylloc.filename(ASTString(static_cast<ParserState*>(parm)->filename));
}


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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, &yylloc, SCANNER);
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

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
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
     '$$ = $1'.

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

  case 10:

    {
        ParserState* pp = static_cast<ParserState*>(parm);
        if (pp->parseDocComments && (yyvsp[0].sValue)) {
          pp->model->addDocComment((yyvsp[0].sValue));
        }
        free((yyvsp[0].sValue));
      }

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

  case 14:

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

  case 15:

    { (yyval.item) = (yyvsp[0].item); }

    break;

  case 16:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"include") ? (yyvsp[0].item) : NULL; }

    break;

  case 17:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"variable declaration") ? (yyvsp[0].item) : NULL; }

    break;

  case 19:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"constraint") ? (yyvsp[0].item) : NULL; }

    break;

  case 20:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"solve") ? (yyvsp[0].item) : NULL; }

    break;

  case 21:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"output") ? (yyvsp[0].item) : NULL; }

    break;

  case 22:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"predicate") ? (yyvsp[0].item) : NULL; }

    break;

  case 23:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"predicate") ? (yyvsp[0].item) : NULL; }

    break;

  case 24:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"annotation") ? (yyvsp[0].item) : NULL; }

    break;

  case 25:

    { ParserState* pp = static_cast<ParserState*>(parm);
        map<string,Model*>::iterator ret = pp->seenModels.find((yyvsp[0].sValue));
        IncludeI* ii = new IncludeI((yyloc),ASTString((yyvsp[0].sValue)));
        (yyval.item) = ii;
        if (ret == pp->seenModels.end()) {
          Model* im = new Model;
          im->setParent(pp->model);
          im->setFilename((yyvsp[0].sValue));
          string fpath, fbase; filepath(pp->filename, fpath, fbase);
          if (fpath=="")
            fpath="./";
          ParseWorkItem pm(im, fpath, (yyvsp[0].sValue));
          pp->files.push_back(pm);
          ii->m(im);
          pp->seenModels.insert(pair<string,Model*>((yyvsp[0].sValue),im));
        } else {
          ii->m(ret->second, false);
        }
        free((yyvsp[0].sValue));
      }

    break;

  case 26:

    { if ((yyvsp[-1].vardeclexpr) && (yyvsp[0].expression_v)) (yyvsp[-1].vardeclexpr)->addAnnotations(*(yyvsp[0].expression_v));
        if ((yyvsp[-1].vardeclexpr))
          (yyval.item) = new VarDeclI((yyloc),(yyvsp[-1].vardeclexpr));
        delete (yyvsp[0].expression_v);
      }

    break;

  case 27:

    { if ((yyvsp[-3].vardeclexpr)) (yyvsp[-3].vardeclexpr)->e((yyvsp[0].expression));
        if ((yyvsp[-3].vardeclexpr) && (yyvsp[-2].expression_v)) (yyvsp[-3].vardeclexpr)->addAnnotations(*(yyvsp[-2].expression_v));
        if ((yyvsp[-3].vardeclexpr))
          (yyval.item) = new VarDeclI((yyloc),(yyvsp[-3].vardeclexpr));
        delete (yyvsp[-2].expression_v);
      }

    break;

  case 28:

    {
        TypeInst* ti = new TypeInst((yyloc),Type::parsetint());
        ti->setIsEnum(true);
        VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[0].sValue));
        free((yyvsp[0].sValue));
        (yyval.item) = new VarDeclI((yyloc),vd);
      }

    break;

  case 29:

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

  case 30:

    {
        TypeInst* ti = new TypeInst((yyloc),Type::parsetint());
        ti->setIsEnum(true);
        vector<Expression*> args;
        args.push_back((yyvsp[-1].expression));
        Call* sl = new Call((yyloc), ASTString((yyvsp[-3].sValue)), args);
        VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[-5].sValue),sl);
        free((yyvsp[-5].sValue));
        (yyval.item) = new VarDeclI((yyloc),vd);
      }

    break;

  case 31:

    { (yyval.expression_v) = new std::vector<Expression*>(); }

    break;

  case 32:

    { (yyval.expression_v) = new std::vector<Expression*>();
        (yyval.expression_v)->push_back(new Id((yyloc),(yyvsp[0].sValue),NULL)); free((yyvsp[0].sValue));
      }

    break;

  case 33:

    { (yyval.expression_v) = (yyvsp[-2].expression_v); if ((yyval.expression_v)) (yyval.expression_v)->push_back(new Id((yyloc),(yyvsp[0].sValue),NULL)); free((yyvsp[0].sValue)); }

    break;

  case 34:

    { (yyval.item) = new AssignI((yyloc),(yyvsp[-2].sValue),(yyvsp[0].expression));
        free((yyvsp[-2].sValue));
      }

    break;

  case 35:

    { (yyval.item) = new ConstraintI((yyloc),(yyvsp[0].expression));}

    break;

  case 36:

    { (yyval.item) = SolveI::sat((yyloc));
        if ((yyval.item) && (yyvsp[-1].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[-1].expression_v));
        delete (yyvsp[-1].expression_v);
      }

    break;

  case 37:

    { (yyval.item) = SolveI::min((yyloc),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-2].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[-2].expression_v));
        delete (yyvsp[-2].expression_v);
      }

    break;

  case 38:

    { (yyval.item) = SolveI::max((yyloc),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-2].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[-2].expression_v));
        delete (yyvsp[-2].expression_v);
      }

    break;

  case 39:

    { (yyval.item) = new OutputI((yyloc),(yyvsp[0].expression));}

    break;

  case 40:

    { if ((yyvsp[-2].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[-3].sValue),new TypeInst((yyloc),
                                   Type::varbool()),*(yyvsp[-2].vardeclexpr_v),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-1].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expression_v));
        free((yyvsp[-3].sValue));
        delete (yyvsp[-2].vardeclexpr_v);
        delete (yyvsp[-1].expression_v);
      }

    break;

  case 41:

    { if ((yyvsp[-2].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[-3].sValue),new TypeInst((yyloc),
                                   Type::parbool()),*(yyvsp[-2].vardeclexpr_v),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-1].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expression_v));
        free((yyvsp[-3].sValue));
        delete (yyvsp[-2].vardeclexpr_v);
        delete (yyvsp[-1].expression_v);
      }

    break;

  case 42:

    { if ((yyvsp[-2].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[-3].sValue),(yyvsp[-5].tiexpr),*(yyvsp[-2].vardeclexpr_v),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-1].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expression_v));
        free((yyvsp[-3].sValue));
        delete (yyvsp[-2].vardeclexpr_v);
        delete (yyvsp[-1].expression_v);
      }

    break;

  case 43:

    { if ((yyvsp[-3].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[-5].sValue),(yyvsp[-7].tiexpr),*(yyvsp[-3].vardeclexpr_v),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-1].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expression_v));
        free((yyvsp[-5].sValue));
        delete (yyvsp[-3].vardeclexpr_v);
        delete (yyvsp[-1].expression_v);
      }

    break;

  case 44:

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

  case 45:

    { TypeInst* ti=new TypeInst((yylsp[-4]),Type::ann());
        if ((yyvsp[-2].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[-3].sValue),ti,*(yyvsp[-2].vardeclexpr_v),(yyvsp[0].expression));
        delete (yyvsp[-2].vardeclexpr_v);
      }

    break;

  case 46:

    { (yyval.expression)=NULL; }

    break;

  case 47:

    { (yyval.expression)=(yyvsp[0].expression); }

    break;

  case 48:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); }

    break;

  case 49:

    { (yyval.vardeclexpr_v)=(yyvsp[-1].vardeclexpr_v); }

    break;

  case 50:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); }

    break;

  case 51:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); }

    break;

  case 52:

    { (yyval.vardeclexpr_v)=(yyvsp[-1].vardeclexpr_v); }

    break;

  case 53:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>();
        if ((yyvsp[0].vardeclexpr)) (yyvsp[0].vardeclexpr)->toplevel(false);
        if ((yyvsp[0].vardeclexpr)) (yyval.vardeclexpr_v)->push_back((yyvsp[0].vardeclexpr)); }

    break;

  case 54:

    { (yyval.vardeclexpr_v)=(yyvsp[-2].vardeclexpr_v);
        if ((yyvsp[0].vardeclexpr)) (yyvsp[0].vardeclexpr)->toplevel(false);
        if ((yyvsp[-2].vardeclexpr_v) && (yyvsp[0].vardeclexpr)) (yyvsp[-2].vardeclexpr_v)->push_back((yyvsp[0].vardeclexpr)); }

    break;

  case 57:

    { (yyval.vardeclexpr)=(yyvsp[0].vardeclexpr); }

    break;

  case 58:

    { if ((yyvsp[0].tiexpr)) (yyval.vardeclexpr)=new VarDecl((yyloc), (yyvsp[0].tiexpr), ""); }

    break;

  case 59:

    { if ((yyvsp[-2].tiexpr) && (yyvsp[0].sValue)) (yyval.vardeclexpr) = new VarDecl((yyloc), (yyvsp[-2].tiexpr), (yyvsp[0].sValue));
        free((yyvsp[0].sValue));
      }

    break;

  case 60:

    { (yyval.tiexpr_v)=(yyvsp[-1].tiexpr_v); }

    break;

  case 61:

    { (yyval.tiexpr_v)=new vector<TypeInst*>(); (yyval.tiexpr_v)->push_back((yyvsp[0].tiexpr)); }

    break;

  case 62:

    { (yyval.tiexpr_v)=(yyvsp[-2].tiexpr_v); if ((yyvsp[-2].tiexpr_v) && (yyvsp[0].tiexpr)) (yyvsp[-2].tiexpr_v)->push_back((yyvsp[0].tiexpr)); }

    break;

  case 64:

    {
        (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr) && (yyvsp[-3].tiexpr_v)) (yyval.tiexpr)->setRanges(*(yyvsp[-3].tiexpr_v));
        delete (yyvsp[-3].tiexpr_v);
      }

    break;

  case 65:

    {
        (yyval.tiexpr) = (yyvsp[0].tiexpr);
        std::vector<TypeInst*> ti(1);
        ti[0] = new TypeInst((yyloc),Type::parint());
        if ((yyval.tiexpr)) (yyval.tiexpr)->setRanges(ti);
      }

    break;

  case 66:

    { (yyval.tiexpr) = (yyvsp[0].tiexpr);
      }

    break;

  case 67:

    { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      }

    break;

  case 68:

    { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr) && (yyvsp[-1].bValue)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      }

    break;

  case 69:

    { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ti(Type::TI_VAR);
          if ((yyvsp[-1].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      }

    break;

  case 70:

    { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.st(Type::ST_SET);
          if ((yyvsp[-3].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      }

    break;

  case 71:

    { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.st(Type::ST_SET);
          if ((yyvsp[-3].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      }

    break;

  case 72:

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

  case 73:

    { (yyval.bValue) = false; }

    break;

  case 74:

    { (yyval.bValue) = true; }

    break;

  case 75:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parint()); }

    break;

  case 76:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parbool()); }

    break;

  case 77:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parfloat()); }

    break;

  case 78:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parstring()); }

    break;

  case 79:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::ann()); }

    break;

  case 80:

    { if ((yyvsp[0].expression)) (yyval.tiexpr) = new TypeInst((yyloc),Type(),(yyvsp[0].expression)); }

    break;

  case 81:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::top(),
                         new TIId((yyloc), (yyvsp[0].sValue)));
        free((yyvsp[0].sValue));
      }

    break;

  case 82:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parint(),
          new TIId((yyloc), (yyvsp[0].sValue)));
          free((yyvsp[0].sValue));
      }

    break;

  case 84:

    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; (yyval.expression_v)->push_back((yyvsp[0].expression)); }

    break;

  case 85:

    { (yyval.expression_v)=(yyvsp[-2].expression_v); if ((yyval.expression_v) && (yyvsp[0].expression)) (yyval.expression_v)->push_back((yyvsp[0].expression)); }

    break;

  case 87:

    { if ((yyvsp[-2].expression) && (yyvsp[0].expression)) (yyvsp[-2].expression)->addAnnotation((yyvsp[0].expression)); (yyval.expression)=(yyvsp[-2].expression); }

    break;

  case 88:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_UNION, (yyvsp[0].expression)); }

    break;

  case 89:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIFF, (yyvsp[0].expression)); }

    break;

  case 90:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SYMDIFF, (yyvsp[0].expression)); }

    break;

  case 91:

    { if ((yyvsp[-2].expression)==NULL || (yyvsp[0].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[-2].expression)->isa<IntLit>() && (yyvsp[0].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[-2].expression)->cast<IntLit>()->v(),(yyvsp[0].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DOTDOT, (yyvsp[0].expression));
        }
      }

    break;

  case 92:

    { if ((yyvsp[-3].expression)==NULL || (yyvsp[-1].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[-3].expression)->isa<IntLit>() && (yyvsp[-1].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[-3].expression)->cast<IntLit>()->v(),(yyvsp[-1].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-3].expression), BOT_DOTDOT, (yyvsp[-1].expression));
        }
      }

    break;

  case 93:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_INTERSECT, (yyvsp[0].expression)); }

    break;

  case 94:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_PLUSPLUS, (yyvsp[0].expression)); }

    break;

  case 95:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_PLUS, (yyvsp[0].expression)); }

    break;

  case 96:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MINUS, (yyvsp[0].expression)); }

    break;

  case 97:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MULT, (yyvsp[0].expression)); }

    break;

  case 98:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIV, (yyvsp[0].expression)); }

    break;

  case 99:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IDIV, (yyvsp[0].expression)); }

    break;

  case 100:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MOD, (yyvsp[0].expression)); }

    break;

  case 101:

    { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), (yyvsp[-1].sValue), args);
        free((yyvsp[-1].sValue));
      }

    break;

  case 102:

    { (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[0].expression)); }

    break;

  case 103:

    { if ((yyvsp[0].expression) && (yyvsp[0].expression)->isa<IntLit>()) {
          (yyval.expression) = IntLit::a(-(yyvsp[0].expression)->cast<IntLit>()->v());
        } else if ((yyvsp[0].expression) && (yyvsp[0].expression)->isa<FloatLit>()) {
          (yyval.expression) = FloatLit::a(-(yyvsp[0].expression)->cast<FloatLit>()->v());
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_MINUS, (yyvsp[0].expression));
        }
      }

    break;

  case 105:

    { if ((yyvsp[-2].expression) && (yyvsp[0].expression)) (yyvsp[-2].expression)->addAnnotation((yyvsp[0].expression)); (yyval.expression)=(yyvsp[-2].expression); }

    break;

  case 106:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_EQUIV, (yyvsp[0].expression)); }

    break;

  case 107:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IMPL, (yyvsp[0].expression)); }

    break;

  case 108:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_RIMPL, (yyvsp[0].expression)); }

    break;

  case 109:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_OR, (yyvsp[0].expression)); }

    break;

  case 110:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_XOR, (yyvsp[0].expression)); }

    break;

  case 111:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_AND, (yyvsp[0].expression)); }

    break;

  case 112:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_LE, (yyvsp[0].expression)); }

    break;

  case 113:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_GR, (yyvsp[0].expression)); }

    break;

  case 114:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_LQ, (yyvsp[0].expression)); }

    break;

  case 115:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_GQ, (yyvsp[0].expression)); }

    break;

  case 116:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_EQ, (yyvsp[0].expression)); }

    break;

  case 117:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_NQ, (yyvsp[0].expression)); }

    break;

  case 118:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IN, (yyvsp[0].expression)); }

    break;

  case 119:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SUBSET, (yyvsp[0].expression)); }

    break;

  case 120:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SUPERSET, (yyvsp[0].expression)); }

    break;

  case 121:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_UNION, (yyvsp[0].expression)); }

    break;

  case 122:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIFF, (yyvsp[0].expression)); }

    break;

  case 123:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SYMDIFF, (yyvsp[0].expression)); }

    break;

  case 124:

    { (yyval.expression)=new SetLit((yyloc), IntSetVal::a(-IntVal::infinity(),IntVal::infinity())); }

    break;

  case 125:

    { if ((yyvsp[0].expression)==NULL) {
        (yyval.expression) = NULL;
      } else if ((yyvsp[0].expression)->isa<IntLit>()) {
        (yyval.expression)=new SetLit((yyloc), IntSetVal::a(-IntVal::infinity(),(yyvsp[0].expression)->cast<IntLit>()->v()));
      } else {
        (yyval.expression)=new BinOp((yyloc), IntLit::a(-IntVal::infinity()), BOT_DOTDOT, (yyvsp[0].expression));
        }
      }

    break;

  case 126:

    { if ((yyvsp[-1].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[-1].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[-1].expression)->cast<IntLit>()->v(),IntVal::infinity()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-1].expression), BOT_DOTDOT, IntLit::a(IntVal::infinity()));
        }
      }

    break;

  case 127:

    { if ((yyvsp[-2].expression)==NULL || (yyvsp[0].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[-2].expression)->isa<IntLit>() && (yyvsp[0].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[-2].expression)->cast<IntLit>()->v(),(yyvsp[0].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DOTDOT, (yyvsp[0].expression));
        }
      }

    break;

  case 128:

    { if ((yyvsp[-3].expression)==NULL || (yyvsp[-1].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[-3].expression)->isa<IntLit>() && (yyvsp[-1].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[-3].expression)->cast<IntLit>()->v(),(yyvsp[-1].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-3].expression), BOT_DOTDOT, (yyvsp[-1].expression));
        }
      }

    break;

  case 129:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_INTERSECT, (yyvsp[0].expression)); }

    break;

  case 130:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_PLUSPLUS, (yyvsp[0].expression)); }

    break;

  case 131:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_PLUS, (yyvsp[0].expression)); }

    break;

  case 132:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MINUS, (yyvsp[0].expression)); }

    break;

  case 133:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MULT, (yyvsp[0].expression)); }

    break;

  case 134:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIV, (yyvsp[0].expression)); }

    break;

  case 135:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IDIV, (yyvsp[0].expression)); }

    break;

  case 136:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MOD, (yyvsp[0].expression)); }

    break;

  case 137:

    { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), (yyvsp[-1].sValue), args);
        free((yyvsp[-1].sValue));
      }

    break;

  case 138:

    { (yyval.expression)=new UnOp((yyloc), UOT_NOT, (yyvsp[0].expression)); }

    break;

  case 139:

    { if (((yyvsp[0].expression) && (yyvsp[0].expression)->isa<IntLit>()) || ((yyvsp[0].expression) && (yyvsp[0].expression)->isa<FloatLit>())) {
          (yyval.expression) = (yyvsp[0].expression);
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[0].expression));
        }
      }

    break;

  case 140:

    { if ((yyvsp[0].expression) && (yyvsp[0].expression)->isa<IntLit>()) {
          (yyval.expression) = IntLit::a(-(yyvsp[0].expression)->cast<IntLit>()->v());
        } else if ((yyvsp[0].expression) && (yyvsp[0].expression)->isa<FloatLit>()) {
          (yyval.expression) = FloatLit::a(-(yyvsp[0].expression)->cast<FloatLit>()->v());
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_MINUS, (yyvsp[0].expression));
        }
      }

    break;

  case 141:

    { (yyval.expression)=(yyvsp[-1].expression); }

    break;

  case 142:

    { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[0].expression_vv)); delete (yyvsp[0].expression_vv); }

    break;

  case 143:

    { (yyval.expression)=new Id((yyloc), (yyvsp[0].sValue), NULL); free((yyvsp[0].sValue)); }

    break;

  case 144:

    { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), new Id((yylsp[-1]),(yyvsp[-1].sValue),NULL), *(yyvsp[0].expression_vv));
        free((yyvsp[-1].sValue)); delete (yyvsp[0].expression_vv); }

    break;

  case 145:

    { (yyval.expression)=new AnonVar((yyloc)); }

    break;

  case 146:

    { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), new AnonVar((yyloc)), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }

    break;

  case 147:

    { (yyval.expression)=constants().boollit(((yyvsp[0].iValue)!=0)); }

    break;

  case 148:

    { (yyval.expression)=IntLit::a((yyvsp[0].iValue)); }

    break;

  case 149:

    { (yyval.expression)=IntLit::a(IntVal::infinity()); }

    break;

  case 150:

    { (yyval.expression)=FloatLit::a((yyvsp[0].dValue)); }

    break;

  case 152:

    { (yyval.expression)=constants().absent; }

    break;

  case 154:

    { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }

    break;

  case 156:

    { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }

    break;

  case 158:

    { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }

    break;

  case 160:

    { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }

    break;

  case 162:

    { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }

    break;

  case 164:

    { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }

    break;

  case 167:

    { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }

    break;

  case 168:

    { (yyval.expression)=new StringLit((yyloc), (yyvsp[0].sValue)); free((yyvsp[0].sValue)); }

    break;

  case 169:

    { (yyval.expression)=new BinOp((yyloc), new StringLit((yyloc), (yyvsp[-1].sValue)), BOT_PLUSPLUS, (yyvsp[0].expression));
        free((yyvsp[-1].sValue));
      }

    break;

  case 170:

    { if ((yyvsp[-1].expression_v)) (yyval.expression)=new BinOp((yyloc), new Call((yyloc), ASTString("format"), *(yyvsp[-1].expression_v)), BOT_PLUSPLUS, new StringLit((yyloc),(yyvsp[0].sValue)));
        free((yyvsp[0].sValue));
        delete (yyvsp[-1].expression_v);
      }

    break;

  case 171:

    { if ((yyvsp[-2].expression_v)) (yyval.expression)=new BinOp((yyloc), new Call((yyloc), ASTString("format"), *(yyvsp[-2].expression_v)), BOT_PLUSPLUS,
                             new BinOp((yyloc), new StringLit((yyloc),(yyvsp[-1].sValue)), BOT_PLUSPLUS, (yyvsp[0].expression)));
        free((yyvsp[-1].sValue));
        delete (yyvsp[-2].expression_v);
      }

    break;

  case 172:

    { (yyval.expression_vv)=new std::vector<std::vector<Expression*> >();
        if ((yyvsp[-1].expression_v)) {
          (yyval.expression_vv)->push_back(*(yyvsp[-1].expression_v));
          delete (yyvsp[-1].expression_v);
        }
      }

    break;

  case 173:

    { (yyval.expression_vv)=(yyvsp[-3].expression_vv);
        if ((yyval.expression_vv) && (yyvsp[-1].expression_v)) {
          (yyval.expression_vv)->push_back(*(yyvsp[-1].expression_v));
          delete (yyvsp[-1].expression_v);
        }
      }

    break;

  case 174:

    { (yyval.expression) = new SetLit((yyloc), std::vector<Expression*>()); }

    break;

  case 175:

    { if ((yyvsp[-1].expression_v)) (yyval.expression) = new SetLit((yyloc), *(yyvsp[-1].expression_v));
        delete (yyvsp[-1].expression_v); }

    break;

  case 176:

    { if ((yyvsp[-1].generators)) (yyval.expression) = new Comprehension((yyloc), (yyvsp[-3].expression), *(yyvsp[-1].generators), true);
        delete (yyvsp[-1].generators);
      }

    break;

  case 177:

    { if ((yyvsp[0].generator_v)) (yyval.generators)=new Generators; (yyval.generators)->_g = *(yyvsp[0].generator_v); delete (yyvsp[0].generator_v); }

    break;

  case 179:

    { (yyval.generator_v)=new std::vector<Generator>; if ((yyvsp[0].generator)) (yyval.generator_v)->push_back(*(yyvsp[0].generator)); delete (yyvsp[0].generator); }

    break;

  case 180:

    { (yyval.generator_v)=(yyvsp[-2].generator_v); if ((yyval.generator_v) && (yyvsp[0].generator)) (yyval.generator_v)->push_back(*(yyvsp[0].generator)); delete (yyvsp[0].generator); }

    break;

  case 181:

    { if ((yyvsp[-2].string_v) && (yyvsp[0].expression)) (yyval.generator)=new Generator(*(yyvsp[-2].string_v),(yyvsp[0].expression),NULL); else (yyval.generator)=NULL; delete (yyvsp[-2].string_v); }

    break;

  case 182:

    { if ((yyvsp[-4].string_v) && (yyvsp[-2].expression)) (yyval.generator)=new Generator(*(yyvsp[-4].string_v),(yyvsp[-2].expression),(yyvsp[0].expression)); else (yyval.generator)=NULL; delete (yyvsp[-4].string_v); }

    break;

  case 183:

    { if ((yyvsp[0].expression)) (yyval.generator)=new Generator({(yyvsp[-2].sValue)},NULL,(yyvsp[0].expression)); else (yyval.generator)=NULL; free((yyvsp[-2].sValue)); }

    break;

  case 185:

    { (yyval.string_v)=new std::vector<std::string>; (yyval.string_v)->push_back((yyvsp[0].sValue)); free((yyvsp[0].sValue)); }

    break;

  case 186:

    { (yyval.string_v)=(yyvsp[-2].string_v); if ((yyval.string_v) && (yyvsp[0].sValue)) (yyval.string_v)->push_back((yyvsp[0].sValue)); free((yyvsp[0].sValue)); }

    break;

  case 187:

    { (yyval.expression)=new ArrayLit((yyloc), std::vector<MiniZinc::Expression*>()); }

    break;

  case 188:

    { if ((yyvsp[-1].expression_v)) (yyval.expression)=new ArrayLit((yyloc), *(yyvsp[-1].expression_v)); delete (yyvsp[-1].expression_v); }

    break;

  case 189:

    { (yyval.expression)=new ArrayLit((yyloc), std::vector<std::vector<Expression*> >()); }

    break;

  case 190:

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

  case 191:

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

  case 192:

    {
      if ((yyvsp[-1].expression_vvv)) {
        std::vector<std::pair<int,int> > dims(3);
        dims[0] = std::pair<int,int>(1,(yyvsp[-1].expression_vvv)->size());
        if ((yyvsp[-1].expression_vvv)->size()==0) {
          dims[1] = std::pair<int,int>(1,0);
          dims[2] = std::pair<int,int>(1,0);
        } else {
          dims[1] = std::pair<int,int>(1,(*(yyvsp[-1].expression_vvv))[0].size());
          if ((*(yyvsp[-1].expression_vvv))[0].size()==0) {
            dims[2] = std::pair<int,int>(1,0);
          } else {
            dims[2] = std::pair<int,int>(1,(*(yyvsp[-1].expression_vvv))[0][0].size());
          }
        }
        std::vector<Expression*> a;
        for (unsigned int i=0; i<dims[0].second; i++) {
          if ((*(yyvsp[-1].expression_vvv))[i].size() != dims[1].second) {
            yyerror(&(yylsp[-1]), parm, "syntax error, all sub-arrays of 3d array literal must have the same length");
          } else {
            for (unsigned int j=0; j<dims[1].second; j++) {
              if ((*(yyvsp[-1].expression_vvv))[i][j].size() != dims[2].second) {
                yyerror(&(yylsp[-1]), parm, "syntax error, all sub-arrays of 3d array literal must have the same length");
              } else {
                for (unsigned int k=0; k<dims[2].second; k++) {
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

  case 193:

    { (yyval.expression_vvv)=new std::vector<std::vector<std::vector<MiniZinc::Expression*> > >;
      }

    break;

  case 194:

    { (yyval.expression_vvv)=new std::vector<std::vector<std::vector<MiniZinc::Expression*> > >;
        if ((yyvsp[-1].expression_vv)) (yyval.expression_vvv)->push_back(*(yyvsp[-1].expression_vv));
        delete (yyvsp[-1].expression_vv);
      }

    break;

  case 195:

    { (yyval.expression_vvv)=(yyvsp[-4].expression_vvv);
        if ((yyval.expression_vvv) && (yyvsp[-1].expression_vv)) (yyval.expression_vvv)->push_back(*(yyvsp[-1].expression_vv));
        delete (yyvsp[-1].expression_vv);
      }

    break;

  case 196:

    { (yyval.expression_vv)=new std::vector<std::vector<MiniZinc::Expression*> >;
        if ((yyvsp[0].expression_v)) (yyval.expression_vv)->push_back(*(yyvsp[0].expression_v));
        delete (yyvsp[0].expression_v);
      }

    break;

  case 197:

    { (yyval.expression_vv)=(yyvsp[-2].expression_vv); if ((yyval.expression_vv) && (yyvsp[0].expression_v)) (yyval.expression_vv)->push_back(*(yyvsp[0].expression_v)); delete (yyvsp[0].expression_v); }

    break;

  case 198:

    { if ((yyvsp[-1].generators)) (yyval.expression)=new Comprehension((yyloc), (yyvsp[-3].expression), *(yyvsp[-1].generators), false);
        delete (yyvsp[-1].generators);
      }

    break;

  case 199:

    {
        std::vector<Expression*> iexps;
        iexps.push_back((yyvsp[-3].expression));
        iexps.push_back((yyvsp[-1].expression));
        (yyval.expression)=new ITE((yyloc), iexps, NULL);
      }

    break;

  case 200:

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

  case 201:

    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; }

    break;

  case 202:

    { (yyval.expression_v)=(yyvsp[-4].expression_v); if ((yyval.expression_v) && (yyvsp[-2].expression) && (yyvsp[0].expression)) { (yyval.expression_v)->push_back((yyvsp[-2].expression)); (yyval.expression_v)->push_back((yyvsp[0].expression)); } }

    break;

  case 203:

    { (yyval.iValue)=BOT_EQUIV; }

    break;

  case 204:

    { (yyval.iValue)=BOT_IMPL; }

    break;

  case 205:

    { (yyval.iValue)=BOT_RIMPL; }

    break;

  case 206:

    { (yyval.iValue)=BOT_OR; }

    break;

  case 207:

    { (yyval.iValue)=BOT_XOR; }

    break;

  case 208:

    { (yyval.iValue)=BOT_AND; }

    break;

  case 209:

    { (yyval.iValue)=BOT_LE; }

    break;

  case 210:

    { (yyval.iValue)=BOT_GR; }

    break;

  case 211:

    { (yyval.iValue)=BOT_LQ; }

    break;

  case 212:

    { (yyval.iValue)=BOT_GQ; }

    break;

  case 213:

    { (yyval.iValue)=BOT_EQ; }

    break;

  case 214:

    { (yyval.iValue)=BOT_NQ; }

    break;

  case 215:

    { (yyval.iValue)=BOT_IN; }

    break;

  case 216:

    { (yyval.iValue)=BOT_SUBSET; }

    break;

  case 217:

    { (yyval.iValue)=BOT_SUPERSET; }

    break;

  case 218:

    { (yyval.iValue)=BOT_UNION; }

    break;

  case 219:

    { (yyval.iValue)=BOT_DIFF; }

    break;

  case 220:

    { (yyval.iValue)=BOT_SYMDIFF; }

    break;

  case 221:

    { (yyval.iValue)=BOT_PLUS; }

    break;

  case 222:

    { (yyval.iValue)=BOT_MINUS; }

    break;

  case 223:

    { (yyval.iValue)=BOT_MULT; }

    break;

  case 224:

    { (yyval.iValue)=BOT_DIV; }

    break;

  case 225:

    { (yyval.iValue)=BOT_IDIV; }

    break;

  case 226:

    { (yyval.iValue)=BOT_MOD; }

    break;

  case 227:

    { (yyval.iValue)=BOT_INTERSECT; }

    break;

  case 228:

    { (yyval.iValue)=BOT_PLUSPLUS; }

    break;

  case 229:

    { (yyval.iValue)=-1; }

    break;

  case 230:

    { if ((yyvsp[-5].iValue)==-1) {
          (yyval.expression)=NULL;
          yyerror(&(yylsp[-3]), parm, "syntax error, unary operator with two arguments");
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-3].expression),static_cast<BinOpType>((yyvsp[-5].iValue)),(yyvsp[-1].expression));
        }
      }

    break;

  case 231:

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

  case 232:

    { (yyval.expression)=new Call((yyloc), (yyvsp[-2].sValue), std::vector<Expression*>()); free((yyvsp[-2].sValue)); }

    break;

  case 234:

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

  case 235:

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
                    if ((*(yyvsp[-4].expression_p))[i].second) {
                      ParserLocation loc = (*(yyvsp[-4].expression_p))[i].second->loc().parserLocation();
                      yyerror(&loc, parm, "illegal where expression in generator call");
                    }
                    ids.push_back(id);
                    gens.push_back(Generator(ids,NULL,boe->rhs()));
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

  case 237:

    { (yyval.expression_p)=new vector<pair<Expression*,Expression*> >;
        (yyval.expression_p)->push_back(pair<Expression*,Expression*>((yyvsp[0].expression),NULL)); }

    break;

  case 238:

    { (yyval.expression_p)=new vector<pair<Expression*,Expression*> >;
        (yyval.expression_p)->push_back(pair<Expression*,Expression*>((yyvsp[-2].expression),(yyvsp[0].expression))); }

    break;

  case 239:

    { (yyval.expression_p)=(yyvsp[-2].expression_p); if ((yyval.expression_p) && (yyvsp[0].expression)) (yyval.expression_p)->push_back(pair<Expression*,Expression*>((yyvsp[0].expression),NULL)); }

    break;

  case 240:

    { (yyval.expression_p)=(yyvsp[-4].expression_p); if ((yyval.expression_p) && (yyvsp[-2].expression) && (yyvsp[0].expression)) (yyval.expression_p)->push_back(pair<Expression*,Expression*>((yyvsp[-2].expression),(yyvsp[0].expression))); }

    break;

  case 241:

    { if ((yyvsp[-3].expression_v) && (yyvsp[0].expression)) {
          (yyval.expression)=new Let((yyloc), *(yyvsp[-3].expression_v), (yyvsp[0].expression)); delete (yyvsp[-3].expression_v);
        } else {
          (yyval.expression)=NULL;
        }
      }

    break;

  case 242:

    { if ((yyvsp[-4].expression_v) && (yyvsp[0].expression)) {
          (yyval.expression)=new Let((yyloc), *(yyvsp[-4].expression_v), (yyvsp[0].expression)); delete (yyvsp[-4].expression_v);
        } else {
          (yyval.expression)=NULL;
        }
      }

    break;

  case 243:

    { (yyval.expression_v)=new vector<Expression*>; (yyval.expression_v)->push_back((yyvsp[0].vardeclexpr)); }

    break;

  case 244:

    { (yyval.expression_v)=new vector<Expression*>;
        if ((yyvsp[0].item)) {
          ConstraintI* ce = (yyvsp[0].item)->cast<ConstraintI>();
          (yyval.expression_v)->push_back(ce->e());
          ce->e(NULL);
        }
      }

    break;

  case 245:

    { (yyval.expression_v)=(yyvsp[-2].expression_v); if ((yyval.expression_v) && (yyvsp[0].vardeclexpr)) (yyval.expression_v)->push_back((yyvsp[0].vardeclexpr)); }

    break;

  case 246:

    { (yyval.expression_v)=(yyvsp[-2].expression_v);
        if ((yyval.expression_v) && (yyvsp[0].item)) {
          ConstraintI* ce = (yyvsp[0].item)->cast<ConstraintI>();
          (yyval.expression_v)->push_back(ce->e());
          ce->e(NULL);
        }
      }

    break;

  case 249:

    { (yyval.vardeclexpr) = (yyvsp[-1].vardeclexpr);
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->toplevel(false);
        if ((yyval.vardeclexpr) && (yyvsp[0].expression_v)) (yyval.vardeclexpr)->addAnnotations(*(yyvsp[0].expression_v));
        delete (yyvsp[0].expression_v);
      }

    break;

  case 250:

    { if ((yyvsp[-3].vardeclexpr)) (yyvsp[-3].vardeclexpr)->e((yyvsp[0].expression));
        (yyval.vardeclexpr) = (yyvsp[-3].vardeclexpr);
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->loc((yyloc));
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->toplevel(false);
        if ((yyval.vardeclexpr) && (yyvsp[-2].expression_v)) (yyval.vardeclexpr)->addAnnotations(*(yyvsp[-2].expression_v));
        delete (yyvsp[-2].expression_v);
      }

    break;

  case 251:

    { (yyval.expression_v)=NULL; }

    break;

  case 253:

    { (yyval.expression_v)=new std::vector<Expression*>(1);
        (*(yyval.expression_v))[0] = (yyvsp[0].expression);
      }

    break;

  case 254:

    { (yyval.expression_v)=(yyvsp[-2].expression_v); if ((yyval.expression_v)) (yyval.expression_v)->push_back((yyvsp[0].expression)); }

    break;

  case 255:

    { (yyval.sValue)=(yyvsp[0].sValue); }

    break;

  case 256:

    { (yyval.sValue)=strdup("'<->'"); }

    break;

  case 257:

    { (yyval.sValue)=strdup("'->'"); }

    break;

  case 258:

    { (yyval.sValue)=strdup("'<-'"); }

    break;

  case 259:

    { (yyval.sValue)=strdup("'\\/'"); }

    break;

  case 260:

    { (yyval.sValue)=strdup("'xor'"); }

    break;

  case 261:

    { (yyval.sValue)=strdup("'/\\'"); }

    break;

  case 262:

    { (yyval.sValue)=strdup("'<'"); }

    break;

  case 263:

    { (yyval.sValue)=strdup("'>'"); }

    break;

  case 264:

    { (yyval.sValue)=strdup("'<='"); }

    break;

  case 265:

    { (yyval.sValue)=strdup("'>='"); }

    break;

  case 266:

    { (yyval.sValue)=strdup("'='"); }

    break;

  case 267:

    { (yyval.sValue)=strdup("'!='"); }

    break;

  case 268:

    { (yyval.sValue)=strdup("'in'"); }

    break;

  case 269:

    { (yyval.sValue)=strdup("'subset'"); }

    break;

  case 270:

    { (yyval.sValue)=strdup("'superset'"); }

    break;

  case 271:

    { (yyval.sValue)=strdup("'union'"); }

    break;

  case 272:

    { (yyval.sValue)=strdup("'diff'"); }

    break;

  case 273:

    { (yyval.sValue)=strdup("'symdiff'"); }

    break;

  case 274:

    { (yyval.sValue)=strdup("'..'"); }

    break;

  case 275:

    { (yyval.sValue)=strdup("'+'"); }

    break;

  case 276:

    { (yyval.sValue)=strdup("'-'"); }

    break;

  case 277:

    { (yyval.sValue)=strdup("'*'"); }

    break;

  case 278:

    { (yyval.sValue)=strdup("'/'"); }

    break;

  case 279:

    { (yyval.sValue)=strdup("'div'"); }

    break;

  case 280:

    { (yyval.sValue)=strdup("'mod'"); }

    break;

  case 281:

    { (yyval.sValue)=strdup("'intersect'"); }

    break;

  case 282:

    { (yyval.sValue)=strdup("'not'"); }

    break;

  case 283:

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
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, parm, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (&yylloc, parm, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

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

  yyerror_range[1] = yylsp[1-yylen];
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

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
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

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp, parm);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
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

#if !defined yyoverflow || YYERROR_VERBOSE
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
  return yyresult;
}
