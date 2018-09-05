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


/* Substitute the variable and function names.  */
#define yyparse         mzn_yyparse
#define yylex           mzn_yylex
#define yyerror         mzn_yyerror
#define yydebug         mzn_yydebug
#define yynerrs         mzn_yynerrs


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
#include <minizinc/json_parser.hh>

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
    if (contents.size() > 0 && contents[0]=='@') {
      contents = FileUtils::decodeBase64(contents);
      FileUtils::inflateString(contents);
    }
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
    if (modelText.size() > 0 && modelText[0]=='@') {
      modelText = FileUtils::decodeBase64(modelText);
      FileUtils::inflateString(modelText);
    }
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
      isFzn |= (filename.compare(filename.length()-4,4,".mzc")==0);
    }
    ParserState pp(filename,modelText, err, files, seenModels, model, false, isFzn, parseDocComments);
    mzn_yylex_init(&pp.yyscanner);
    mzn_yyset_extra(&pp, pp.yyscanner);
    mzn_yyparse(&pp);
    if (pp.yyscanner)
    mzn_yylex_destroy(pp.yyscanner);
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
      isFzn |= (fullname.compare(fullname.length()-4,4,".mzc")==0);
      ParserState pp(fullname,s, err, files, seenModels, m, false, isFzn, parseDocComments);
      mzn_yylex_init(&pp.yyscanner);
      mzn_yyset_extra(&pp, pp.yyscanner);
      mzn_yyparse(&pp);
      if (pp.yyscanner)
      mzn_yylex_destroy(pp.yyscanner);
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
      isFzn |= (fullname.compare(fullname.length()-4,4,".mzc")==0);
      ParserState pp(fullname,s, err, files, seenModels, m, false, isFzn, parseDocComments);
      mzn_yylex_init(&pp.yyscanner);
      mzn_yyset_extra(&pp, pp.yyscanner);
      mzn_yyparse(&pp);
      if (pp.yyscanner)
        mzn_yylex_destroy(pp.yyscanner);
      if (pp.hadError) {
        goto error;
      }
    }
    
    for (unsigned int i=0; i<datafiles.size(); i++) {
      GCLock lock;
      string f = datafiles[i];
      if (f.size()>=6 && f.substr(f.size()-5,string::npos)==".json") {
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
        mzn_yylex_init(&pp.yyscanner);
        mzn_yyset_extra(&pp, pp.yyscanner);
        mzn_yyparse(&pp);
        if (pp.yyscanner)
          mzn_yylex_destroy(pp.yyscanner);
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
#ifndef YY_MZN_YY_C_CYGWIN_HOME_TACK_LIBMZN_BUILD_INCLUDE_MINIZINC_PARSER_TAB_HH_INCLUDED
# define YY_MZN_YY_C_CYGWIN_HOME_TACK_LIBMZN_BUILD_INCLUDE_MINIZINC_PARSER_TAB_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int mzn_yydebug;
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
    MZN_IN = 334,
    MZN_SUBSET = 335,
    MZN_SUPERSET = 336,
    MZN_UNION = 337,
    MZN_DIFF = 338,
    MZN_SYMDIFF = 339,
    MZN_DOTDOT = 340,
    MZN_PLUS = 341,
    MZN_MINUS = 342,
    MZN_MULT = 343,
    MZN_DIV = 344,
    MZN_IDIV = 345,
    MZN_MOD = 346,
    MZN_INTERSECT = 347,
    MZN_POW = 348,
    MZN_NOT = 349,
    MZN_PLUSPLUS = 350,
    MZN_COLONCOLON = 351,
    PREC_ANNO = 352,
    MZN_EQUIV_QUOTED = 353,
    MZN_IMPL_QUOTED = 354,
    MZN_RIMPL_QUOTED = 355,
    MZN_OR_QUOTED = 356,
    MZN_XOR_QUOTED = 357,
    MZN_AND_QUOTED = 358,
    MZN_LE_QUOTED = 359,
    MZN_GR_QUOTED = 360,
    MZN_LQ_QUOTED = 361,
    MZN_GQ_QUOTED = 362,
    MZN_EQ_QUOTED = 363,
    MZN_NQ_QUOTED = 364,
    MZN_IN_QUOTED = 365,
    MZN_SUBSET_QUOTED = 366,
    MZN_SUPERSET_QUOTED = 367,
    MZN_UNION_QUOTED = 368,
    MZN_DIFF_QUOTED = 369,
    MZN_SYMDIFF_QUOTED = 370,
    MZN_DOTDOT_QUOTED = 371,
    MZN_PLUS_QUOTED = 372,
    MZN_MINUS_QUOTED = 373,
    MZN_MULT_QUOTED = 374,
    MZN_DIV_QUOTED = 375,
    MZN_IDIV_QUOTED = 376,
    MZN_MOD_QUOTED = 377,
    MZN_INTERSECT_QUOTED = 378,
    MZN_POW_QUOTED = 379,
    MZN_NOT_QUOTED = 380,
    MZN_COLONCOLON_QUOTED = 381,
    MZN_PLUSPLUS_QUOTED = 382
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



int mzn_yyparse (void *parm);

#endif /* !YY_MZN_YY_C_CYGWIN_HOME_TACK_LIBMZN_BUILD_INCLUDE_MINIZINC_PARSER_TAB_HH_INCLUDED  */

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
#define YYFINAL  157
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   4829

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  136
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  73
/* YYNRULES -- Number of rules.  */
#define YYNRULES  310
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  518

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   382

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
     131,   132,     2,     2,   133,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   134,   128,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   129,   135,   130,     2,     2,     2,     2,
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
     125,   126,   127
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   629,   629,   631,   633,   636,   645,   654,   663,   672,
     674,   677,   685,   694,   694,   696,   712,   716,   718,   720,
     721,   723,   725,   727,   729,   731,   734,   734,   734,   735,
     735,   735,   735,   735,   736,   739,   762,   768,   775,   783,
     793,   805,   818,   820,   821,   825,   833,   834,   838,   842,
     848,   850,   857,   862,   867,   874,   878,   886,   896,   903,
     912,   924,   932,   933,   938,   939,   941,   946,   947,   951,
     955,   960,   960,   963,   965,   969,   974,   978,   980,   984,
     985,   991,  1000,  1003,  1011,  1019,  1028,  1037,  1046,  1059,
    1060,  1064,  1066,  1068,  1070,  1072,  1074,  1076,  1081,  1087,
    1090,  1092,  1096,  1098,  1100,  1109,  1120,  1123,  1125,  1131,
    1132,  1134,  1136,  1138,  1140,  1149,  1158,  1160,  1162,  1164,
    1166,  1168,  1170,  1172,  1174,  1176,  1182,  1184,  1197,  1198,
    1200,  1202,  1204,  1206,  1208,  1210,  1212,  1214,  1216,  1218,
    1220,  1222,  1224,  1226,  1228,  1230,  1232,  1234,  1236,  1245,
    1254,  1256,  1258,  1260,  1262,  1264,  1266,  1268,  1270,  1272,
    1278,  1280,  1287,  1299,  1301,  1305,  1307,  1309,  1311,  1314,
    1316,  1319,  1321,  1323,  1325,  1327,  1329,  1330,  1333,  1334,
    1337,  1338,  1341,  1342,  1345,  1346,  1349,  1350,  1353,  1354,
    1355,  1360,  1362,  1368,  1373,  1381,  1388,  1397,  1399,  1404,
    1410,  1413,  1416,  1418,  1422,  1424,  1426,  1429,  1432,  1434,
    1438,  1440,  1444,  1446,  1457,  1468,  1508,  1511,  1516,  1523,
    1528,  1532,  1538,  1545,  1561,  1562,  1566,  1568,  1570,  1572,
    1574,  1576,  1578,  1580,  1582,  1584,  1586,  1588,  1590,  1592,
    1594,  1596,  1598,  1600,  1602,  1604,  1606,  1608,  1610,  1612,
    1614,  1616,  1618,  1620,  1624,  1632,  1664,  1666,  1667,  1687,
    1743,  1746,  1749,  1752,  1754,  1758,  1765,  1774,  1776,  1784,
    1786,  1795,  1795,  1798,  1804,  1815,  1816,  1819,  1821,  1825,
    1829,  1833,  1835,  1837,  1839,  1841,  1843,  1845,  1847,  1849,
    1851,  1853,  1855,  1857,  1859,  1861,  1863,  1865,  1867,  1869,
    1871,  1873,  1875,  1877,  1879,  1881,  1883,  1885,  1887,  1889,
    1891
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
  "\"end of line inside string literal\"", "\"null character\"", "\"<->\"",
  "\"->\"", "\"<-\"", "\"\\\\/\"", "\"xor\"", "\"/\\\\\"", "\"<\"",
  "\">\"", "\"<=\"", "\">=\"", "\"=\"", "\"!=\"", "\"in\"", "\"subset\"",
  "\"superset\"", "\"union\"", "\"diff\"", "\"symdiff\"", "\"..\"",
  "\"+\"", "\"-\"", "\"*\"", "\"/\"", "\"div\"", "\"mod\"",
  "\"intersect\"", "\"^\"", "\"not\"", "\"++\"", "\"::\"", "PREC_ANNO",
  "\"'<->'\"", "\"'->'\"", "\"'<-'\"", "\"'\\\\/'\"", "\"'xor'\"",
  "\"'/\\\\'\"", "\"'<'\"", "\"'>'\"", "\"'<='\"", "\"'>='\"", "\"'='\"",
  "\"'!='\"", "\"'in'\"", "\"'subset'\"", "\"'superset'\"", "\"'union'\"",
  "\"'diff'\"", "\"'symdiff'\"", "\"'..'\"", "\"'+'\"", "\"'-'\"",
  "\"'*'\"", "\"'/'\"", "\"'div'\"", "\"'mod'\"", "\"'intersect'\"",
  "\"'^'\"", "\"'not'\"", "\"'::'\"", "\"'++'\"", "';'", "'{'", "'}'",
  "'('", "')'", "','", "':'", "'|'", "$accept", "model", "item_list",
  "item_list_head", "doc_file_comments", "semi_or_none", "item",
  "item_tail", "error_item_start", "include_item", "vardecl_item",
  "string_lit_list", "enum_id_list", "assign_item", "constraint_item",
  "solve_item", "output_item", "predicate_item", "function_item",
  "annotation_item", "operation_item_tail", "params", "params_list",
  "params_list_head", "comma_or_none", "ti_expr_and_id_or_anon",
  "ti_expr_and_id", "ti_expr_list", "ti_expr_list_head", "ti_expr",
  "base_ti_expr", "opt_opt", "base_ti_expr_tail", "array_access_expr_list",
  "array_access_expr_list_head", "array_access_expr", "expr_list",
  "expr_list_head", "set_expr", "expr", "expr_atom_head",
  "expr_atom_head_nonstring", "string_expr", "string_quote_rest",
  "array_access_tail", "set_literal", "set_comp", "comp_tail",
  "generator_list", "generator_list_head", "generator", "id_list",
  "id_list_head", "simple_array_literal", "simple_array_literal_2d",
  "simple_array_literal_3d_list", "simple_array_literal_2d_list",
  "simple_array_comp", "if_then_else_expr", "elseif_list", "quoted_op",
  "quoted_op_call", "call_expr", "comp_or_expr", "comp_or_expr_head",
  "let_expr", "let_vardecl_item_list", "comma_or_semi", "let_vardecl_item",
  "annotations", "annotation_expr", "ne_annotations", "id_or_quoted_op", YY_NULLPTR
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
     375,   376,   377,   378,   379,   380,   381,   382,    59,   123,
     125,    40,    41,    44,    58,   124
};
# endif

#define YYPACT_NINF -390

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-390)))

#define YYTABLE_NINF -90

#define yytable_value_is_error(Yytable_value) \
  (!!((Yytable_value) == (-90)))

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     787,   -93,  -390,  -390,  -390,    27,  -390,  3369,  -390,  -390,
    1563,  -390,     4,     4,  -390,  -390,    53,    -2,  -390,  2724,
      54,  -390,  2079,  3369,    55,  -390,  -390,   -67,    23,  2595,
    3369,    59,   -29,  -390,    62,    12,  2853,   509,  3498,  3498,
    -390,  -390,  -390,  -390,  -390,  -390,  -390,  -390,  -390,  -390,
    -390,  -390,  -390,  -390,  -390,  -390,  -390,  -390,   -50,  -390,
    -390,  -390,  -390,  -390,  -390,  -390,  -390,  -390,  -390,  2982,
    3369,    82,  -390,   -45,  1305,   214,  -390,  -390,  -390,  -390,
    -390,  -390,  -390,  -390,  -390,  -390,   -29,   -49,  -390,    39,
    -390,  1002,  -390,  -390,  -390,    12,    12,    12,    12,    12,
      12,   -41,  -390,    12,  -390,  1434,  3111,  3369,  1046,    34,
     -26,  3369,  3369,  3369,   -36,     8,  4596,  -390,  -390,  -390,
    -390,  2337,  2466,   -23,  2079,    28,  4596,    37,   -38,  4294,
    -390,  1821,  2208,  -390,  4596,   -23,  3533,    18,     3,   -23,
      34,  -390,    80,     7,  3617,  -390,   654,  -390,   -33,   -32,
      47,    47,  3369,  -390,    16,  3652,  3886,  -390,  1176,  -390,
    -390,  -390,  -390,  -390,  -390,  -390,  -390,  -390,  -390,  -390,
    -390,  -390,    70,   143,   109,  3498,  3498,  3498,  3498,  3498,
    3498,  3498,  3498,  3498,  3498,  3498,  3498,  3498,  3498,  3533,
      34,    34,    34,    34,    34,    34,  3369,    34,  -390,  3369,
      92,    21,  -390,  4646,  4596,  -390,  4335,    24,    26,  3111,
      49,    49,    49,  3369,  3369,  -390,  3369,  3369,  3369,  3369,
    3369,  3369,  3369,  3369,  3369,  3369,  3369,  3369,  3369,  3369,
    3369,  3369,  3369,  3369,  3369,  3369,  3369,  3369,  3369,  3369,
    3369,  3369,  3369,  3369,  3369,  3369,  3533,   111,  -390,   112,
    -390,   916,    78,    98,    29,  -390,  3369,    19,  4126,  3369,
    -390,   -29,    30,   -89,  -390,  -390,   -29,  -390,  -390,  -390,
    3369,  3369,  -390,  3533,   -29,  -390,  3369,  -390,   154,  -390,
      32,  -390,    38,  -390,  3240,  3769,  -390,   154,    12,  1305,
    -390,  3369,    41,  2595,    69,   277,   277,   277,   292,    22,
      22,    35,    35,    35,    35,   277,    47,    33,  -390,  3739,
    4596,  -390,  3111,  -390,  3369,  3369,    43,  3369,  -390,   107,
    3856,  -390,  4596,    72,  4683,  1035,  1035,   775,   775,  1165,
    4733,  4733,  4733,  4733,  4733,  4733,   635,   635,   635,   307,
     307,   307,   443,    31,    31,    42,    42,    42,    42,   307,
      49,    36,  -390,  2595,  2595,    44,    50,    46,  -390,  -390,
      30,  3369,   129,  1950,  -390,  4596,    56,   163,   169,  -390,
    -390,  -390,  -390,  -390,  -390,  -390,  -390,  -390,  -390,  -390,
    -390,  -390,  -390,  -390,  -390,  -390,  -390,  -390,  -390,  -390,
    -390,  -390,  -390,  -390,  -390,  -390,  -390,  -390,  -390,   -23,
    4422,   104,   178,  -390,   106,  -390,  1692,   115,  4596,  4596,
    -390,   115,   116,   127,  -390,    63,  -390,   110,    99,  3369,
    3369,  -390,  -390,  3369,    58,    34,  -390,  4596,  1950,  -390,
    -390,  3369,  -390,  4596,  3369,  4385,  -390,  3369,  -390,  -390,
    -390,  -390,  1950,  -390,  4596,  2208,  -390,  3369,  -390,   -27,
    -390,  -100,   -29,  -390,    20,  3369,  -390,  3369,   152,  -390,
    -390,  3369,  -390,  -390,  3369,  -390,   154,  -390,  3369,   189,
    -390,   100,  3973,  -390,   101,  4003,  4090,  3369,  4120,  -390,
    -390,  4207,  -390,   230,  -390,   235,   115,  3369,  3369,  4596,
    4596,  3369,  4596,  4596,  -390,  4472,  -390,  3369,  -390,   -29,
    -390,  -390,  4596,  -390,  -390,  -390,  -390,  -390,  4509,  4559,
    4596,  3369,   115,  -390,  3369,  4596,  -390,  4596
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,   172,   171,   174,   167,   191,     0,    97,    98,
      89,    11,    89,    89,   175,    95,     0,     0,    92,     0,
       0,    93,    89,     0,     0,   173,    91,     0,     0,    90,
       0,     0,   275,    94,     0,   169,     0,     0,     0,     0,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,     0,   244,
     245,   246,   248,   249,   250,   251,   247,   253,   252,     0,
       0,     0,     2,    13,    89,     5,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,   275,     0,    79,     0,
      82,    96,   109,   163,   164,   176,   178,   180,   182,   184,
     186,     0,   257,   189,   188,    89,     0,     0,     0,   168,
     167,     0,     0,     0,     0,     0,   107,   128,   192,    15,
      90,     0,     0,    64,    89,     0,    50,    38,     0,     0,
      35,    89,    89,    83,    55,    64,     0,     0,   276,    64,
     170,   210,     0,    71,   107,   212,     0,   219,     0,     0,
     126,   127,     0,   197,     0,   107,     0,     1,    14,     4,
      12,     6,    34,    29,    27,    32,    26,    28,    31,    30,
      33,     9,    36,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     177,   179,   181,   183,   185,   187,     0,   190,    10,   103,
       0,    71,   100,   102,    49,   256,   261,     0,    71,     0,
     161,   162,   160,     0,     0,   193,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    85,     0,
      84,     0,    60,     0,    71,    77,     0,     0,     0,     0,
     268,   275,     0,     0,   267,    81,   275,   277,   278,   279,
       0,     0,    52,     0,   275,   211,    72,   106,     0,   216,
       0,   215,     0,   213,     0,     0,   198,     0,   165,    89,
       7,     0,    75,     0,   125,   111,   112,   113,   114,   118,
     119,   120,   121,   122,   123,   116,   124,   117,   110,     0,
     104,   195,    72,    99,   105,     0,   258,    72,   260,     0,
       0,   194,   108,   159,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   152,   153,   154,   155,   156,   157,   150,
     158,   151,   129,     0,     0,     0,     0,    71,    69,    73,
      74,     0,     0,    72,    76,    51,     0,    42,    46,   281,
     282,   283,   284,   285,   286,   287,   288,   289,   290,   291,
     292,   293,   294,   295,   296,   297,   298,   299,   300,   301,
     302,   303,   305,   306,   307,   308,   304,   309,   310,    64,
     224,   273,     0,   272,     0,   271,    89,    62,    54,    53,
     280,    62,   208,     0,   200,    71,   202,     0,    71,   217,
       0,   214,   220,     0,     0,   166,     8,    37,    67,    86,
     255,     0,   101,   262,     0,   263,   196,     0,    88,    87,
      66,    65,    72,    68,    61,    89,    78,     0,    44,     0,
      47,     0,   275,   222,     0,     0,    75,     0,     0,   270,
     269,     0,    56,    57,     0,   221,    72,   201,     0,    72,
     207,     0,     0,   199,     0,     0,     0,     0,     0,    70,
      80,     0,    40,     0,    39,     0,    62,     0,     0,   274,
     265,     0,    63,   206,   203,   204,   209,   218,   115,   275,
     254,   259,   264,   149,    41,    45,    48,    58,     0,     0,
     266,     0,    62,   223,     0,   205,    59,   225
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -390,  -390,  -390,  -390,    84,  -390,   -60,   233,  -390,  -390,
    -390,  -390,  -390,  -390,  -125,  -390,  -390,  -390,  -390,  -390,
    -389,  -123,  -183,  -390,  -188,  -195,  -126,  -390,  -390,   -15,
    -130,    40,   -20,    45,  -390,   -62,   -35,    14,   321,   -19,
     301,  -112,  -110,    51,   -25,  -390,  -390,   -34,  -390,  -390,
    -211,  -390,  -390,  -390,  -390,  -390,  -138,  -390,  -390,  -390,
    -390,  -390,  -390,  -390,  -390,  -390,  -390,  -390,  -150,   -83,
    -166,  -390,  -390
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    71,    72,    73,    74,   159,    75,    76,   171,    77,
      78,   449,   451,    79,    80,    81,    82,    83,    84,    85,
     462,   252,   356,   357,   277,   358,    86,   253,   254,    87,
      88,    89,    90,   200,   201,   202,   147,   143,    91,   116,
     117,    93,    94,   118,   109,    95,    96,   413,   414,   415,
     416,   417,   418,    97,    98,   148,   149,    99,   100,   454,
     101,   102,   103,   207,   208,   104,   263,   406,   264,   137,
     269,   138,   399
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     126,   142,   265,   172,   129,   261,   260,   128,   280,   133,
     140,   134,   266,   313,   161,   256,   274,   144,   214,   215,
     318,   115,   463,   308,   267,   366,   268,   281,   283,   175,
     484,   106,   482,   485,   154,   105,     6,     7,   217,   403,
     175,   404,   175,   217,   405,   198,   120,   487,   488,   217,
     155,   156,   121,   122,   175,   124,   217,   270,   271,   123,
     127,   272,   131,   130,   132,   135,   364,   136,   139,   106,
     190,   191,   192,   193,   194,   195,   367,   267,   197,   268,
     352,   152,   157,   158,   106,   173,   174,   203,   204,   206,
     196,   209,   210,   211,   212,   213,   258,   507,   290,   273,
     282,   248,   250,   284,   107,   108,   483,   410,   251,   255,
     182,   183,   184,   185,   257,   187,   262,   188,   189,   239,
     240,   241,   242,   516,   244,   359,   245,   246,   187,   189,
     188,   189,   246,   285,   267,   244,   268,   245,   246,   275,
     276,   216,   188,   189,   245,   246,   286,   291,   368,   292,
     293,   311,   353,   354,   312,   361,   316,   362,   108,   317,
     412,   267,   363,   268,   402,   189,   436,   419,   246,   443,
     445,   448,   428,   420,   434,   450,   440,   309,   401,   442,
     310,   455,   441,   407,   456,   457,   465,   447,   473,   468,
     203,   411,   461,   464,   320,   496,   466,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   467,   115,   426,
     470,   491,   469,   499,   162,   497,   360,   365,   505,   163,
     400,   506,   289,   119,   164,   474,   165,   479,   166,   422,
     432,   408,   409,   424,   319,   494,   460,   322,   167,   168,
       0,     0,   169,   425,   170,   321,     0,     0,     0,     0,
       0,     0,   427,   429,     0,     0,   452,     0,     0,     0,
     261,   459,   471,     0,   175,     0,     0,     0,     0,     0,
       0,     0,     0,   203,     0,   342,   433,     0,   435,   175,
       0,    92,   359,     0,     0,     0,     0,     0,     0,     0,
       0,    92,     0,     0,   217,   480,   359,     0,     0,     0,
       0,     0,     0,    92,     0,     0,     0,     0,     0,     0,
      92,     0,     0,   438,   439,     0,     0,     0,     0,    92,
      92,     0,   444,     0,     0,     0,     0,     0,   446,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   150,
     151,     0,   179,   180,   181,   182,   183,   184,   185,   486,
     187,     0,   188,   189,     0,    92,     0,   -90,   180,   181,
     182,   183,   184,   185,   422,   187,     0,   188,   189,     0,
       0,   262,   236,   237,   238,   239,   240,   241,   242,     0,
     244,     0,   245,   246,   472,     0,    92,     0,     0,     0,
       0,     0,   475,   360,     0,   476,   512,     0,   478,     0,
       0,     0,    92,    92,     0,    92,     0,   360,   481,     0,
       0,     0,    92,    92,     0,     0,   489,     0,   490,     0,
       0,     0,   492,     0,     0,   493,     0,     0,     0,   495,
     217,     0,     0,     0,     0,     0,     0,     0,   502,    92,
       0,     0,   422,     0,     0,     0,     0,     0,   508,   509,
       0,     0,   510,     0,     0,     0,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
       0,     0,   515,     0,     0,   517,   294,   295,   296,   297,
     298,   299,   300,   301,   302,   303,   304,   305,   306,   307,
       0,     0,     2,     3,     4,   110,     0,     6,     7,     0,
       0,     0,     0,     0,     0,     0,     0,    14,   -90,   237,
     238,   239,   240,   241,   242,     0,   244,     0,   245,   246,
       0,     0,    23,     0,    25,     0,    27,     0,     0,     0,
       0,     0,    92,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    35,     0,     0,    36,    37,     0,   145,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      92,     0,     0,     0,    92,   111,   112,     0,     0,     0,
       0,     0,     0,   113,     0,     0,     0,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,   114,    59,    60,    61,    62,
      63,    64,    65,    66,    67,     0,    68,     0,    69,     0,
      70,     0,   217,     0,   146,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    92,    92,     0,     2,     3,     4,
     110,     0,     6,     7,    92,     0,     0,     0,     0,     0,
       0,     0,    14,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    23,     0,    25,
       0,    27,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    92,    35,     0,
       0,    36,    37,     0,   -90,   -90,   -90,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,    92,
     245,   246,     0,     0,     0,     0,     0,     0,     0,     0,
     111,   112,     0,    92,     0,     0,    92,     0,   113,     0,
       0,     0,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
     114,    59,    60,    61,    62,    63,    64,    65,    66,    67,
       0,    68,   217,    69,     0,    70,     0,    -3,     1,   279,
       2,     3,     4,     5,     0,     6,     7,     0,     0,     8,
       9,    10,    11,    12,    13,    14,    15,    16,     0,    17,
      18,     0,    19,     0,     0,     0,     0,    20,    21,    22,
      23,    24,    25,    26,    27,    28,     0,     0,     0,    29,
       0,    30,    31,     0,   -89,    32,    33,    34,     0,     0,
       0,    35,     0,     0,    36,    37,     0,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,     0,
     245,   246,     0,    38,    39,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,     0,    68,     0,    69,   355,    70,     2,
       3,     4,   110,     0,     6,     7,     0,     0,     8,     9,
       0,     0,    12,    13,    14,    15,     0,     0,    17,    18,
       0,     0,     0,     0,     0,     0,     0,    21,     0,    23,
       0,    25,    26,    27,    28,     0,     0,     0,    29,     0,
       0,     0,     0,   -89,     0,    33,     0,     0,     0,     0,
      35,     0,     0,    36,    37,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    38,    39,     0,     0,     0,     0,     0,   175,
       0,     0,     0,     0,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,   217,    68,     0,    69,     0,    70,   -67,     2,
       3,     4,   110,     0,     6,     7,     0,     0,     0,     0,
       0,     0,     0,     0,    14,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    23,
       0,    25,     0,    27,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,     0,   188,   189,     0,
      35,     0,     0,    36,    37,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,     0,
     245,   246,   111,   112,     0,     0,     0,     0,     0,     0,
     113,     0,     0,     0,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,   114,    59,    60,    61,    62,    63,    64,    65,
      66,    67,   217,    68,     0,    69,     0,    70,   205,     2,
       3,     4,     5,     0,     6,     7,     0,     0,     8,     9,
      10,    11,    12,    13,    14,    15,    16,     0,    17,    18,
       0,    19,     0,     0,     0,     0,    20,    21,    22,    23,
      24,    25,    26,    27,    28,     0,     0,     0,    29,     0,
      30,    31,     0,   -89,    32,    33,    34,     0,     0,     0,
      35,     0,     0,    36,    37,     0,     0,     0,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,     0,
     245,   246,    38,    39,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,     0,    68,     0,    69,     0,    70,     2,     3,
       4,     5,     0,     6,     7,     0,     0,     8,     9,    10,
     160,    12,    13,    14,    15,    16,     0,    17,    18,     0,
      19,     0,     0,     0,     0,    20,    21,    22,    23,    24,
      25,    26,    27,    28,     0,     0,     0,    29,     0,    30,
      31,     0,     0,    32,    33,    34,     0,     0,     0,    35,
       0,     0,    36,    37,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    38,    39,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,     0,    68,     0,    69,     0,    70,     2,     3,     4,
       5,     0,     6,     7,     0,     0,     8,     9,    10,     0,
      12,    13,    14,    15,    16,     0,    17,    18,     0,    19,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,    27,    28,     0,     0,     0,    29,     0,    30,    31,
       0,     0,    32,    33,    34,     0,     0,     0,    35,     0,
       0,    36,    37,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      38,    39,     0,     0,     0,     0,     0,     0,     0,     0,
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
       0,     0,     0,     0,     0,     0,     0,     0,     0,    38,
      39,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,     0,
      68,     0,    69,     0,    70,     2,     3,     4,   110,     0,
       6,     7,     0,     0,     8,     9,     0,     0,    12,    13,
      14,    15,     0,     0,    17,    18,     0,    19,     0,     0,
       0,     0,     0,    21,     0,    23,     0,    25,    26,    27,
      28,     0,     0,     0,    29,     0,     0,     0,     0,     0,
       0,    33,     0,     0,     0,     0,    35,     0,     0,    36,
      37,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    38,    39,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,     0,    68,
       0,    69,   458,    70,     2,     3,     4,   110,     0,     6,
       7,     0,     0,     8,     9,     0,     0,    12,    13,    14,
      15,     0,     0,    17,    18,     0,    19,     0,     0,     0,
       0,     0,    21,     0,    23,     0,    25,    26,    27,    28,
       0,     0,     0,    29,     0,     0,     0,     0,     0,     0,
      33,     0,     0,     0,     0,    35,     0,     0,    36,    37,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    38,    39,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,     0,    68,     0,
      69,     0,    70,     2,     3,     4,   110,     0,     6,     7,
       0,     0,     8,     9,     0,     0,    12,    13,    14,    15,
       0,     0,    17,    18,     0,     0,     0,     0,     0,     0,
       0,    21,     0,    23,     0,    25,    26,    27,    28,     0,
       0,     0,    29,     0,     0,     0,     0,   -89,     0,    33,
       0,     0,     0,     0,    35,     0,     0,    36,    37,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    38,    39,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,     0,    68,     0,    69,
       0,    70,     2,     3,     4,   110,     0,     6,     7,     0,
       0,     8,     9,     0,     0,    12,    13,    14,    15,     0,
       0,    17,    18,     0,     0,     0,     0,     0,     0,     0,
      21,     0,    23,     0,    25,    26,    27,    28,     0,     0,
       0,    29,     0,     0,     0,     0,     0,     0,    33,     0,
       0,     0,     0,    35,     0,     0,    36,    37,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    38,    39,     0,     0,     0,
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
       0,     0,     0,     0,    38,    39,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,     0,    68,     0,    69,     0,    70,
       2,     3,     4,   110,     0,     6,     7,     0,     0,     8,
       9,     0,     0,     0,     0,    14,    15,     0,     0,     0,
      18,     0,     0,     0,     0,     0,     0,     0,    21,     0,
      23,     0,    25,    26,    27,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   247,     0,    33,     0,     0,     0,
       0,    35,     0,     0,    36,    37,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    38,    39,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,     0,    68,     0,    69,     0,    70,     2,
       3,     4,   110,     0,     6,     7,     0,     0,     8,     9,
       0,     0,     0,     0,    14,    15,     0,     0,     0,    18,
       0,     0,     0,     0,     0,     0,     0,    21,     0,    23,
       0,    25,    26,    27,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   249,     0,    33,     0,     0,     0,     0,
      35,     0,     0,    36,    37,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    38,    39,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,     0,    68,     0,    69,     0,    70,     2,     3,
       4,   110,     0,     6,     7,     0,     0,     8,     9,     0,
       0,     0,     0,    14,    15,     0,     0,     0,    18,     0,
       0,     0,     0,     0,     0,     0,    21,     0,    23,     0,
      25,    26,    27,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    33,     0,     0,     0,     0,    35,
       0,     0,    36,    37,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    38,    39,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,     0,    68,     0,    69,     0,    70,     2,     3,     4,
     110,     0,     6,     7,     0,     0,     0,     0,     0,     0,
       0,     0,    14,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    23,     0,    25,
       0,    27,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    35,     0,
       0,    36,    37,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     111,   112,     0,     0,     0,     0,     0,     0,   113,     0,
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
       0,     0,     0,     0,     0,     0,     0,     0,     0,   111,
     112,     0,     0,     0,     0,     0,     0,   113,     0,     0,
       0,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,   114,
      59,    60,    61,    62,    63,    64,    65,    66,    67,     0,
      68,     0,    69,     0,    70,     2,     3,     4,   110,     0,
       6,     7,     0,     0,     0,     0,     0,     0,     0,     0,
      14,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    23,     0,    25,     0,    27,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    35,     0,     0,    36,
      37,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   111,   112,
       0,     0,     0,     0,     0,     0,   113,     0,     0,     0,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,   114,    59,
      60,    61,    62,    63,    64,    65,    66,    67,     0,    68,
       0,    69,   153,    70,     2,     3,     4,   110,     0,     6,
       7,     0,     0,     0,     0,     0,     0,     0,     0,    14,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    23,     0,    25,     0,    27,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    35,     0,     0,    36,    37,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   199,   111,   112,     0,
       0,     0,     0,     0,     0,   113,     0,     0,     0,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,   114,    59,    60,
      61,    62,    63,    64,    65,    66,    67,     0,    68,     0,
      69,     0,    70,     2,     3,     4,   110,     0,     6,     7,
       0,     0,     0,     0,     0,     0,     0,     0,    14,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    23,     0,    25,     0,    27,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    35,     0,     0,    36,    37,     0,
     421,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   111,   112,     0,     0,
       0,     0,     0,     0,   113,     0,     0,     0,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,   114,    59,    60,    61,
      62,    63,    64,    65,    66,    67,     0,    68,     0,    69,
       0,    70,     2,     3,     4,   110,     0,     6,     7,     0,
       0,     0,     0,     0,     0,     0,     0,    14,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    23,     0,    25,     0,    27,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    35,     0,     0,    36,    37,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   111,   112,     0,     0,     0,
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
       0,     0,     0,     0,    38,    39,     0,    35,     0,     0,
      36,    37,     0,     0,     0,     0,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,   217,    68,     0,    69,     0,    70,
       0,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,     0,
      59,    60,    61,    62,    63,    64,    65,    66,    67,   217,
      68,     0,    69,     0,    70,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     244,     0,   245,   246,     0,     0,     0,     0,     0,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   243,   244,   217,   245,   246,     0,
       0,     0,   278,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   217,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   287,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
     242,   243,   244,     0,   245,   246,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
     242,   243,   244,   217,   245,   246,     0,     0,     0,     0,
       0,   430,   431,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   217,     0,     0,     0,     0,     0,     0,
       0,     0,   423,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
       0,   245,   246,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     217,   245,   246,     0,     0,     0,     0,     0,     0,   437,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     217,     0,     0,     0,     0,     0,     0,     0,   288,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,   244,     0,   245,   246,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,   244,   217,   245,   246,
       0,     0,     0,     0,     0,   498,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   217,     0,     0,
       0,     0,   369,     0,     0,   500,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
     241,   242,   243,   244,     0,   245,   246,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
     241,   242,   243,   244,   217,   245,   246,     0,     0,     0,
       0,     0,   501,     0,   370,   371,   372,   373,   374,   375,
     376,   377,   378,   379,   380,   381,   382,   383,   384,   385,
     386,   387,   388,   389,   390,   391,   392,   393,   394,   395,
     396,   397,   503,   398,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     244,   217,   245,   246,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   504,
       0,     0,   217,     0,     0,   259,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   218,   219,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,   238,   239,   240,   241,   242,   243,   244,     0,   245,
     246,   315,   217,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,   217,
     245,   246,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   477,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   453,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,   217,
     245,   246,     0,     0,     0,     0,     0,     0,     0,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   243,   244,   217,   245,   246,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   511,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   513,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   243,   244,   217,   245,   246,     0,
       0,     0,     0,     0,     0,     0,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
     242,   243,   244,   217,   245,   246,     0,     0,     0,     0,
     514,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
     242,   243,   244,   217,   245,   246,     0,     0,     0,     0,
       0,     0,     0,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     217,   245,   246,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   314,   237,   238,   239,   240,   241,   242,   243,   244,
     217,   245,   246,     0,     0,     0,     0,     0,     0,     0,
       0,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,   244,     0,   245,   246,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   -90,   -90,   -90,   -90,
     -90,   -90,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,   244,     0,   245,   246
};

static const yytype_int16 yycheck[] =
{
      19,    36,   132,    86,    23,   131,   131,    22,   146,    29,
      35,    30,   135,   201,    74,   125,   139,    36,    10,    11,
     208,     7,   411,   189,   136,     6,   136,    60,    60,     7,
     130,    57,    59,   133,    69,   128,     8,     9,     7,   128,
       7,   130,     7,     7,   133,   105,    42,    27,    28,     7,
      69,    70,    12,    13,     7,    57,     7,    39,    40,     6,
       6,    43,   129,     8,    41,     6,   254,    96,     6,    57,
      95,    96,    97,    98,    99,   100,    57,   189,   103,   189,
     246,   131,     0,   128,    57,   134,    47,   106,   107,   108,
     131,    57,   111,   112,   113,   131,   134,   486,   158,    96,
     133,   121,   122,   135,    77,   131,   133,   273,   131,   124,
      88,    89,    90,    91,    77,    93,   131,    95,    96,    88,
      89,    90,    91,   512,    93,   251,    95,    96,    93,    96,
      95,    96,    96,   152,   246,    93,   246,    95,    96,    59,
     133,   133,    95,    96,    95,    96,   130,    77,   129,     6,
      41,    59,    41,    41,   133,    77,   132,    59,   131,   133,
       6,   273,   133,   273,   134,    96,    59,   135,    96,   357,
      41,     8,   131,   135,   131,     6,   132,   196,   261,   133,
     199,    77,   132,   266,     6,    79,    59,   131,   130,    79,
     209,   274,    77,    77,   213,     6,   133,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   243,   244,   245,   415,   214,   289,
     418,    79,   133,   132,    20,   135,   251,   256,     8,    25,
     259,     6,   158,    10,    30,   428,    32,   442,    34,   284,
     312,   270,   271,   287,   209,   466,   406,   276,    44,    45,
      -1,    -1,    48,   288,    50,   214,    -1,    -1,    -1,    -1,
      -1,    -1,   291,   293,    -1,    -1,   399,    -1,    -1,    -1,
     406,   406,   420,    -1,     7,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   312,    -1,   314,   315,    -1,   317,     7,
      -1,     0,   428,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    10,    -1,    -1,     7,   445,   442,    -1,    -1,    -1,
      -1,    -1,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,
      29,    -1,    -1,   353,   354,    -1,    -1,    -1,    -1,    38,
      39,    -1,   361,    -1,    -1,    -1,    -1,    -1,   363,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,
      39,    -1,    85,    86,    87,    88,    89,    90,    91,   452,
      93,    -1,    95,    96,    -1,    74,    -1,    85,    86,    87,
      88,    89,    90,    91,   419,    93,    -1,    95,    96,    -1,
      -1,   406,    85,    86,    87,    88,    89,    90,    91,    -1,
      93,    -1,    95,    96,   423,    -1,   105,    -1,    -1,    -1,
      -1,    -1,   431,   428,    -1,   434,   499,    -1,   437,    -1,
      -1,    -1,   121,   122,    -1,   124,    -1,   442,   447,    -1,
      -1,    -1,   131,   132,    -1,    -1,   455,    -1,   457,    -1,
      -1,    -1,   461,    -1,    -1,   464,    -1,    -1,    -1,   468,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   477,   158,
      -1,    -1,   497,    -1,    -1,    -1,    -1,    -1,   487,   488,
      -1,    -1,   491,    -1,    -1,    -1,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
      -1,    -1,   511,    -1,    -1,   514,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
      -1,    -1,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,    85,    86,
      87,    88,    89,    90,    91,    -1,    93,    -1,    95,    96,
      -1,    -1,    33,    -1,    35,    -1,    37,    -1,    -1,    -1,
      -1,    -1,   251,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,    60,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     289,    -1,    -1,    -1,   293,    86,    87,    -1,    -1,    -1,
      -1,    -1,    -1,    94,    -1,    -1,    -1,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,    -1,   127,    -1,   129,    -1,
     131,    -1,     7,    -1,   135,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   353,   354,    -1,     3,     4,     5,
       6,    -1,     8,     9,   363,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,    35,
      -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   406,    54,    -1,
      -1,    57,    58,    -1,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,   428,
      95,    96,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    87,    -1,   442,    -1,    -1,   445,    -1,    94,    -1,
      -1,    -1,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
      -1,   127,     7,   129,    -1,   131,    -1,     0,     1,   135,
       3,     4,     5,     6,    -1,     8,     9,    -1,    -1,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    -1,    22,
      23,    -1,    25,    -1,    -1,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    -1,    -1,    -1,    42,
      -1,    44,    45,    -1,    47,    48,    49,    50,    -1,    -1,
      -1,    54,    -1,    -1,    57,    58,    -1,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    -1,
      95,    96,    -1,    86,    87,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,    -1,   127,    -1,   129,     1,   131,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    12,    13,
      -1,    -1,    16,    17,    18,    19,    -1,    -1,    22,    23,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,    33,
      -1,    35,    36,    37,    38,    -1,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    47,    -1,    49,    -1,    -1,    -1,    -1,
      54,    -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    87,    -1,    -1,    -1,    -1,    -1,     7,
      -1,    -1,    -1,    -1,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,     7,   127,    -1,   129,    -1,   131,   132,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    18,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,
      -1,    35,    -1,    37,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    -1,    95,    96,    -1,
      54,    -1,    -1,    57,    58,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    -1,
      95,    96,    86,    87,    -1,    -1,    -1,    -1,    -1,    -1,
      94,    -1,    -1,    -1,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,     7,   127,    -1,   129,    -1,   131,   132,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    -1,    22,    23,
      -1,    25,    -1,    -1,    -1,    -1,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    -1,    -1,    -1,    42,    -1,
      44,    45,    -1,    47,    48,    49,    50,    -1,    -1,    -1,
      54,    -1,    -1,    57,    58,    -1,    -1,    -1,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    -1,
      95,    96,    86,    87,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,    -1,   127,    -1,   129,    -1,   131,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    -1,    22,    23,    -1,
      25,    -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    -1,    -1,    -1,    42,    -1,    44,
      45,    -1,    -1,    48,    49,    50,    -1,    -1,    -1,    54,
      -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,    -1,   127,    -1,   129,    -1,   131,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    12,    13,    14,    -1,
      16,    17,    18,    19,    20,    -1,    22,    23,    -1,    25,
      -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    -1,    -1,    -1,    42,    -1,    44,    45,
      -1,    -1,    48,    49,    50,    -1,    -1,    -1,    54,    -1,
      -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
      -1,   127,    -1,   129,    -1,   131,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    12,    13,    -1,    -1,    16,
      17,    18,    19,    20,    -1,    22,    23,    -1,    25,    -1,
      -1,    -1,    -1,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    -1,    -1,    -1,    42,    -1,    44,    45,    -1,
      -1,    48,    49,    50,    -1,    -1,    -1,    54,    -1,    -1,
      57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,    -1,
     127,    -1,   129,    -1,   131,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    12,    13,    -1,    -1,    16,    17,
      18,    19,    -1,    -1,    22,    23,    -1,    25,    -1,    -1,
      -1,    -1,    -1,    31,    -1,    33,    -1,    35,    36,    37,
      38,    -1,    -1,    -1,    42,    -1,    -1,    -1,    -1,    -1,
      -1,    49,    -1,    -1,    -1,    -1,    54,    -1,    -1,    57,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    87,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,    -1,   127,
      -1,   129,   130,   131,     3,     4,     5,     6,    -1,     8,
       9,    -1,    -1,    12,    13,    -1,    -1,    16,    17,    18,
      19,    -1,    -1,    22,    23,    -1,    25,    -1,    -1,    -1,
      -1,    -1,    31,    -1,    33,    -1,    35,    36,    37,    38,
      -1,    -1,    -1,    42,    -1,    -1,    -1,    -1,    -1,    -1,
      49,    -1,    -1,    -1,    -1,    54,    -1,    -1,    57,    58,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    87,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,    -1,   127,    -1,
     129,    -1,   131,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    12,    13,    -1,    -1,    16,    17,    18,    19,
      -1,    -1,    22,    23,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    31,    -1,    33,    -1,    35,    36,    37,    38,    -1,
      -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,    49,
      -1,    -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    87,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,    -1,   127,    -1,   129,
      -1,   131,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    12,    13,    -1,    -1,    16,    17,    18,    19,    -1,
      -1,    22,    23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      31,    -1,    33,    -1,    35,    36,    37,    38,    -1,    -1,
      -1,    42,    -1,    -1,    -1,    -1,    -1,    -1,    49,    -1,
      -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    87,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,    -1,   127,    -1,   129,    -1,
     131,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      12,    13,    -1,    -1,    16,    17,    18,    19,    -1,    -1,
      -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,
      -1,    33,    -1,    35,    36,    37,    -1,    -1,    -1,    -1,
      42,    -1,    -1,    -1,    -1,    -1,    -1,    49,    -1,    -1,
      -1,    -1,    54,    -1,    -1,    57,    58,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    87,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,    -1,   127,    -1,   129,    -1,   131,
       3,     4,     5,     6,    -1,     8,     9,    -1,    -1,    12,
      13,    -1,    -1,    -1,    -1,    18,    19,    -1,    -1,    -1,
      23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,
      33,    -1,    35,    36,    37,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    47,    -1,    49,    -1,    -1,    -1,
      -1,    54,    -1,    -1,    57,    58,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    87,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,    -1,   127,    -1,   129,    -1,   131,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    12,    13,
      -1,    -1,    -1,    -1,    18,    19,    -1,    -1,    -1,    23,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,    33,
      -1,    35,    36,    37,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    47,    -1,    49,    -1,    -1,    -1,    -1,
      54,    -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    87,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,    -1,   127,    -1,   129,    -1,   131,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    12,    13,    -1,
      -1,    -1,    -1,    18,    19,    -1,    -1,    -1,    23,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,    33,    -1,
      35,    36,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    49,    -1,    -1,    -1,    -1,    54,
      -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,    -1,   127,    -1,   129,    -1,   131,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,    35,
      -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,
      -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    87,    -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,
      96,    -1,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
      -1,   127,    -1,   129,    -1,   131,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,    35,    -1,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,    -1,
      57,    58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      87,    -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,    -1,
      -1,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,    -1,
     127,    -1,   129,    -1,   131,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    33,    -1,    35,    -1,    37,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,    -1,    57,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    87,
      -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,    -1,    -1,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,    -1,   127,
      -1,   129,   130,   131,     3,     4,     5,     6,    -1,     8,
       9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    33,    -1,    35,    -1,    37,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    54,    -1,    -1,    57,    58,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    85,    86,    87,    -1,
      -1,    -1,    -1,    -1,    -1,    94,    -1,    -1,    -1,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,    -1,   127,    -1,
     129,    -1,   131,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    33,    -1,    35,    -1,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,
      60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    87,    -1,    -1,
      -1,    -1,    -1,    -1,    94,    -1,    -1,    -1,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,    -1,   127,    -1,   129,
      -1,   131,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    33,    -1,    35,    -1,    37,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    87,    -1,    -1,    -1,
      -1,    -1,    -1,    94,    -1,    -1,    -1,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,    -1,   127,    -1,   129,    -1,
     131,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    33,    -1,    35,    -1,    37,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    18,    54,    -1,    -1,    57,    58,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,    35,    -1,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    87,    -1,    54,    -1,    -1,
      57,    58,    -1,    -1,    -1,    -1,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,     7,   127,    -1,   129,    -1,   131,
      -1,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,    -1,
     117,   118,   119,   120,   121,   122,   123,   124,   125,     7,
     127,    -1,   129,    -1,   131,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    -1,    95,    96,    -1,    -1,    -1,    -1,    -1,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,     7,    95,    96,    -1,
      -1,    -1,   135,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   135,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    -1,    95,    96,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,     7,    95,    96,    -1,    -1,    -1,    -1,
      -1,   132,   133,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   133,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      -1,    95,    96,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
       7,    95,    96,    -1,    -1,    -1,    -1,    -1,    -1,   133,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   132,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    -1,    95,    96,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,     7,    95,    96,
      -1,    -1,    -1,    -1,    -1,   132,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,    -1,
      -1,    -1,     6,    -1,    -1,   132,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    -1,    95,    96,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,     7,    95,    96,    -1,    -1,    -1,
      -1,    -1,   132,    -1,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   132,   127,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,     7,    95,    96,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   132,
      -1,    -1,     7,    -1,    -1,    51,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    -1,    95,
      96,    56,     7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,     7,
      95,    96,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    56,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    29,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,     7,
      95,    96,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,     7,    95,    96,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    56,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,     7,    95,    96,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,     7,    95,    96,    -1,    -1,    -1,    -1,
      51,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,     7,    95,    96,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
       7,    95,    96,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
       7,    95,    96,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    -1,    95,    96,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    -1,    95,    96
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     4,     5,     6,     8,     9,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    22,    23,    25,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    42,
      44,    45,    48,    49,    50,    54,    57,    58,    86,    87,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   127,   129,
     131,   137,   138,   139,   140,   142,   143,   145,   146,   149,
     150,   151,   152,   153,   154,   155,   162,   165,   166,   167,
     168,   174,   176,   177,   178,   181,   182,   189,   190,   193,
     194,   196,   197,   198,   201,   128,    57,    77,   131,   180,
       6,    86,    87,    94,   116,   173,   175,   176,   179,   143,
      42,   167,   167,     6,    57,    96,   175,     6,   165,   175,
       8,   129,    41,   168,   175,     6,    96,   205,   207,     6,
     180,    59,   172,   173,   175,    60,   135,   172,   191,   192,
     174,   174,   131,   130,   172,   175,   175,     0,   128,   141,
      15,   142,    20,    25,    30,    32,    34,    44,    45,    48,
      50,   144,   205,   134,    47,     7,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    95,    96,
     180,   180,   180,   180,   180,   180,   131,   180,   142,    85,
     169,   170,   171,   175,   175,   132,   175,   199,   200,    57,
     175,   175,   175,   131,    10,    11,   133,     7,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    95,    96,    47,   168,    47,
     168,   131,   157,   163,   164,   165,   178,    77,   134,    51,
     150,   162,   165,   202,   204,   166,   157,   177,   178,   206,
      39,    40,    43,    96,   157,    59,   133,   160,   135,   135,
     192,    60,   133,    60,   135,   175,   130,   135,   132,   140,
     142,    77,     6,    41,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   206,   175,
     175,    59,   133,   160,    85,    56,   132,   133,   160,   169,
     175,   179,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   206,    41,    41,     1,   158,   159,   161,   162,
     165,    77,    59,   133,   160,   175,     6,    57,   129,     6,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   127,   208,
     175,   205,   134,   128,   130,   133,   203,   205,   175,   175,
     206,   205,     6,   183,   184,   185,   186,   187,   188,   135,
     135,    60,   172,   133,   183,   180,   142,   175,   131,   168,
     132,   133,   171,   175,   131,   175,    59,   133,   168,   168,
     132,   132,   133,   160,   175,    41,   165,   131,     8,   147,
       6,   148,   157,    29,   195,    77,     6,    79,   130,   150,
     204,    77,   156,   156,    77,    59,   133,   160,    79,   133,
     160,   192,   175,   130,   158,   175,   175,    56,   175,   161,
     166,   175,    59,   133,   130,   133,   205,    27,    28,   175,
     175,    79,   175,   175,   186,   175,     6,   135,   132,   132,
     132,   132,   175,   132,   132,     8,     6,   156,   175,   175,
     175,    56,   205,    29,    51,   175,   156,   175
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   136,   137,   138,   138,   139,   139,   139,   139,   139,
     139,   140,   140,   141,   141,   142,   142,   143,   143,   143,
     143,   143,   143,   143,   143,   143,   144,   144,   144,   144,
     144,   144,   144,   144,   144,   145,   146,   146,   146,   146,
     146,   146,   147,   147,   147,   147,   148,   148,   148,   149,
     150,   150,   151,   151,   151,   152,   153,   153,   154,   154,
     155,   155,   156,   156,   157,   157,   157,   158,   158,   159,
     159,   160,   160,   161,   161,   162,   163,   164,   164,   165,
     165,   165,   166,   166,   166,   166,   166,   166,   166,   167,
     167,   168,   168,   168,   168,   168,   168,   168,   168,   169,
     170,   170,   171,   171,   171,   171,   172,   173,   173,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   176,   176,   177,   177,   177,   177,   177,
     177,   177,   177,   177,   177,   177,   177,   177,   177,   177,
     177,   177,   177,   177,   177,   177,   177,   177,   177,   177,
     177,   178,   178,   179,   179,   180,   180,   181,   181,   182,
     183,   184,   185,   185,   186,   186,   186,   187,   188,   188,
     189,   189,   190,   190,   190,   190,   191,   191,   191,   192,
     192,   193,   194,   194,   195,   195,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   197,   197,   198,   198,   198,   198,
     199,   200,   200,   200,   200,   201,   201,   202,   202,   202,
     202,   203,   203,   204,   204,   205,   205,   206,   206,   207,
     207,   208,   208,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   208,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   208,   208,   208,   208,   208,   208,   208,   208,
     208
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     2,     3,     4,     2,
       3,     1,     2,     0,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     4,     2,     6,
       6,     7,     0,     0,     1,     3,     0,     1,     3,     3,
       2,     4,     3,     4,     4,     2,     5,     5,     7,     8,
       3,     5,     0,     2,     0,     3,     3,     0,     2,     1,
       3,     0,     1,     1,     1,     3,     2,     1,     3,     1,
       6,     3,     1,     2,     3,     3,     4,     5,     5,     0,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     3,     1,     1,     2,     2,     2,     1,     3,     1,
       3,     3,     3,     3,     3,     6,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     2,     1,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     6,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     2,     2,     1,     1,     3,     4,     1,     2,     1,
       2,     1,     1,     1,     1,     1,     1,     2,     1,     2,
       1,     2,     1,     2,     1,     2,     1,     2,     1,     1,
       2,     1,     2,     2,     3,     3,     4,     2,     3,     5,
       1,     2,     1,     3,     3,     5,     3,     2,     1,     3,
       2,     3,     2,     3,     4,     3,     2,     3,     5,     1,
       3,     5,     5,     8,     0,     5,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     6,     4,     3,     1,     4,     7,
       2,     1,     3,     3,     5,     6,     7,     1,     1,     3,
       3,     1,     1,     2,     4,     0,     1,     1,     1,     2,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1
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

  case 43:

    { (yyval.expression_v) = new std::vector<Expression*>(); }

    break;

  case 44:

    { (yyval.expression_v) = new std::vector<Expression*>();
        (yyval.expression_v)->push_back(new StringLit((yyloc), (yyvsp[0].sValue))); free((yyvsp[0].sValue));
      }

    break;

  case 45:

    { (yyval.expression_v) = (yyvsp[-2].expression_v);
        if ((yyval.expression_v)) (yyval.expression_v)->push_back(new StringLit((yyloc), (yyvsp[0].sValue)));
        free((yyvsp[0].sValue));
      }

    break;

  case 46:

    { (yyval.expression_v) = new std::vector<Expression*>(); }

    break;

  case 47:

    { (yyval.expression_v) = new std::vector<Expression*>();
        (yyval.expression_v)->push_back(new Id((yyloc),(yyvsp[0].sValue),NULL)); free((yyvsp[0].sValue));
      }

    break;

  case 48:

    { (yyval.expression_v) = (yyvsp[-2].expression_v); if ((yyval.expression_v)) (yyval.expression_v)->push_back(new Id((yyloc),(yyvsp[0].sValue),NULL)); free((yyvsp[0].sValue)); }

    break;

  case 49:

    { (yyval.item) = new AssignI((yyloc),(yyvsp[-2].sValue),(yyvsp[0].expression));
        free((yyvsp[-2].sValue));
      }

    break;

  case 50:

    { (yyval.item) = new ConstraintI((yyloc),(yyvsp[0].expression));}

    break;

  case 51:

    { (yyval.item) = new ConstraintI((yyloc),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-1].expression))
          (yyval.item)->cast<ConstraintI>()->e()->ann().add(new Call((yylsp[-2]), ASTString("mzn_constraint_name"), {(yyvsp[-1].expression)}));
      }

    break;

  case 52:

    { (yyval.item) = SolveI::sat((yyloc));
        if ((yyval.item) && (yyvsp[-1].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[-1].expression_v));
        delete (yyvsp[-1].expression_v);
      }

    break;

  case 53:

    { (yyval.item) = SolveI::min((yyloc),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-2].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[-2].expression_v));
        delete (yyvsp[-2].expression_v);
      }

    break;

  case 54:

    { (yyval.item) = SolveI::max((yyloc),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-2].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[-2].expression_v));
        delete (yyvsp[-2].expression_v);
      }

    break;

  case 55:

    { (yyval.item) = new OutputI((yyloc),(yyvsp[0].expression));}

    break;

  case 56:

    { if ((yyvsp[-2].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[-3].sValue),new TypeInst((yyloc),
                                   Type::varbool()),*(yyvsp[-2].vardeclexpr_v),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-1].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expression_v));
        free((yyvsp[-3].sValue));
        delete (yyvsp[-2].vardeclexpr_v);
        delete (yyvsp[-1].expression_v);
      }

    break;

  case 57:

    { if ((yyvsp[-2].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[-3].sValue),new TypeInst((yyloc),
                                   Type::parbool()),*(yyvsp[-2].vardeclexpr_v),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-1].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expression_v));
        free((yyvsp[-3].sValue));
        delete (yyvsp[-2].vardeclexpr_v);
        delete (yyvsp[-1].expression_v);
      }

    break;

  case 58:

    { if ((yyvsp[-2].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[-3].sValue),(yyvsp[-5].tiexpr),*(yyvsp[-2].vardeclexpr_v),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-1].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expression_v));
        free((yyvsp[-3].sValue));
        delete (yyvsp[-2].vardeclexpr_v);
        delete (yyvsp[-1].expression_v);
      }

    break;

  case 59:

    { if ((yyvsp[-3].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[-5].sValue),(yyvsp[-7].tiexpr),*(yyvsp[-3].vardeclexpr_v),(yyvsp[0].expression));
        if ((yyval.item) && (yyvsp[-1].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[-1].expression_v));
        free((yyvsp[-5].sValue));
        delete (yyvsp[-3].vardeclexpr_v);
        delete (yyvsp[-1].expression_v);
      }

    break;

  case 60:

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

  case 61:

    { TypeInst* ti=new TypeInst((yylsp[-4]),Type::ann());
        if ((yyvsp[-2].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[-3].sValue),ti,*(yyvsp[-2].vardeclexpr_v),(yyvsp[0].expression));
        delete (yyvsp[-2].vardeclexpr_v);
      }

    break;

  case 62:

    { (yyval.expression)=NULL; }

    break;

  case 63:

    { (yyval.expression)=(yyvsp[0].expression); }

    break;

  case 64:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); }

    break;

  case 65:

    { (yyval.vardeclexpr_v)=(yyvsp[-1].vardeclexpr_v); }

    break;

  case 66:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); }

    break;

  case 67:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); }

    break;

  case 68:

    { (yyval.vardeclexpr_v)=(yyvsp[-1].vardeclexpr_v); }

    break;

  case 69:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>();
        if ((yyvsp[0].vardeclexpr)) (yyvsp[0].vardeclexpr)->toplevel(false);
        if ((yyvsp[0].vardeclexpr)) (yyval.vardeclexpr_v)->push_back((yyvsp[0].vardeclexpr)); }

    break;

  case 70:

    { (yyval.vardeclexpr_v)=(yyvsp[-2].vardeclexpr_v);
        if ((yyvsp[0].vardeclexpr)) (yyvsp[0].vardeclexpr)->toplevel(false);
        if ((yyvsp[-2].vardeclexpr_v) && (yyvsp[0].vardeclexpr)) (yyvsp[-2].vardeclexpr_v)->push_back((yyvsp[0].vardeclexpr)); }

    break;

  case 73:

    { (yyval.vardeclexpr)=(yyvsp[0].vardeclexpr); }

    break;

  case 74:

    { if ((yyvsp[0].tiexpr)) (yyval.vardeclexpr)=new VarDecl((yyloc), (yyvsp[0].tiexpr), ""); }

    break;

  case 75:

    { if ((yyvsp[-2].tiexpr) && (yyvsp[0].sValue)) (yyval.vardeclexpr) = new VarDecl((yyloc), (yyvsp[-2].tiexpr), (yyvsp[0].sValue));
        free((yyvsp[0].sValue));
      }

    break;

  case 76:

    { (yyval.tiexpr_v)=(yyvsp[-1].tiexpr_v); }

    break;

  case 77:

    { (yyval.tiexpr_v)=new vector<TypeInst*>(); (yyval.tiexpr_v)->push_back((yyvsp[0].tiexpr)); }

    break;

  case 78:

    { (yyval.tiexpr_v)=(yyvsp[-2].tiexpr_v); if ((yyvsp[-2].tiexpr_v) && (yyvsp[0].tiexpr)) (yyvsp[-2].tiexpr_v)->push_back((yyvsp[0].tiexpr)); }

    break;

  case 80:

    {
        (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr) && (yyvsp[-3].tiexpr_v)) (yyval.tiexpr)->setRanges(*(yyvsp[-3].tiexpr_v));
        delete (yyvsp[-3].tiexpr_v);
      }

    break;

  case 81:

    {
        (yyval.tiexpr) = (yyvsp[0].tiexpr);
        std::vector<TypeInst*> ti(1);
        ti[0] = new TypeInst((yyloc),Type::parint());
        if ((yyval.tiexpr)) (yyval.tiexpr)->setRanges(ti);
      }

    break;

  case 82:

    { (yyval.tiexpr) = (yyvsp[0].tiexpr);
      }

    break;

  case 83:

    { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      }

    break;

  case 84:

    { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr) && (yyvsp[-1].bValue)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      }

    break;

  case 85:

    { (yyval.tiexpr) = (yyvsp[0].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ti(Type::TI_VAR);
          if ((yyvsp[-1].bValue)) tt.ot(Type::OT_OPTIONAL);
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
          tt.st(Type::ST_SET);
          if ((yyvsp[-3].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      }

    break;

  case 88:

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

  case 89:

    { (yyval.bValue) = false; }

    break;

  case 90:

    { (yyval.bValue) = true; }

    break;

  case 91:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parint()); }

    break;

  case 92:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parbool()); }

    break;

  case 93:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parfloat()); }

    break;

  case 94:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parstring()); }

    break;

  case 95:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::ann()); }

    break;

  case 96:

    { if ((yyvsp[0].expression)) (yyval.tiexpr) = new TypeInst((yyloc),Type(),(yyvsp[0].expression)); }

    break;

  case 97:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::top(),
                         new TIId((yyloc), (yyvsp[0].sValue)));
        free((yyvsp[0].sValue));
      }

    break;

  case 98:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parint(),
          new TIId((yyloc), (yyvsp[0].sValue)));
          free((yyvsp[0].sValue));
      }

    break;

  case 100:

    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; (yyval.expression_v)->push_back((yyvsp[0].expression)); }

    break;

  case 101:

    { (yyval.expression_v)=(yyvsp[-2].expression_v); if ((yyval.expression_v) && (yyvsp[0].expression)) (yyval.expression_v)->push_back((yyvsp[0].expression)); }

    break;

  case 102:

    { (yyval.expression) = (yyvsp[0].expression); }

    break;

  case 103:

    { (yyval.expression)=new SetLit((yyloc), IntSetVal::a(-IntVal::infinity(),IntVal::infinity())); }

    break;

  case 104:

    { if ((yyvsp[0].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[0].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a(-IntVal::infinity(),(yyvsp[0].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), IntLit::a(-IntVal::infinity()), BOT_DOTDOT, (yyvsp[0].expression));
        }
      }

    break;

  case 105:

    { if ((yyvsp[-1].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[-1].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[-1].expression)->cast<IntLit>()->v(),IntVal::infinity()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-1].expression), BOT_DOTDOT, IntLit::a(IntVal::infinity()));
        }
      }

    break;

  case 107:

    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; (yyval.expression_v)->push_back((yyvsp[0].expression)); }

    break;

  case 108:

    { (yyval.expression_v)=(yyvsp[-2].expression_v); if ((yyval.expression_v) && (yyvsp[0].expression)) (yyval.expression_v)->push_back((yyvsp[0].expression)); }

    break;

  case 110:

    { if ((yyvsp[-2].expression) && (yyvsp[0].expression)) (yyvsp[-2].expression)->addAnnotation((yyvsp[0].expression)); (yyval.expression)=(yyvsp[-2].expression); }

    break;

  case 111:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_UNION, (yyvsp[0].expression)); }

    break;

  case 112:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIFF, (yyvsp[0].expression)); }

    break;

  case 113:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SYMDIFF, (yyvsp[0].expression)); }

    break;

  case 114:

    { if ((yyvsp[-2].expression)==NULL || (yyvsp[0].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[-2].expression)->isa<IntLit>() && (yyvsp[0].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[-2].expression)->cast<IntLit>()->v(),(yyvsp[0].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DOTDOT, (yyvsp[0].expression));
        }
      }

    break;

  case 115:

    { if ((yyvsp[-3].expression)==NULL || (yyvsp[-1].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[-3].expression)->isa<IntLit>() && (yyvsp[-1].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[-3].expression)->cast<IntLit>()->v(),(yyvsp[-1].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-3].expression), BOT_DOTDOT, (yyvsp[-1].expression));
        }
      }

    break;

  case 116:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_INTERSECT, (yyvsp[0].expression)); }

    break;

  case 117:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_PLUSPLUS, (yyvsp[0].expression)); }

    break;

  case 118:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_PLUS, (yyvsp[0].expression)); }

    break;

  case 119:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MINUS, (yyvsp[0].expression)); }

    break;

  case 120:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MULT, (yyvsp[0].expression)); }

    break;

  case 121:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIV, (yyvsp[0].expression)); }

    break;

  case 122:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IDIV, (yyvsp[0].expression)); }

    break;

  case 123:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MOD, (yyvsp[0].expression)); }

    break;

  case 124:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_POW, (yyvsp[0].expression)); }

    break;

  case 125:

    { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), (yyvsp[-1].sValue), args);
        free((yyvsp[-1].sValue));
      }

    break;

  case 126:

    { (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[0].expression)); }

    break;

  case 127:

    { if ((yyvsp[0].expression) && (yyvsp[0].expression)->isa<IntLit>()) {
          (yyval.expression) = IntLit::a(-(yyvsp[0].expression)->cast<IntLit>()->v());
        } else if ((yyvsp[0].expression) && (yyvsp[0].expression)->isa<FloatLit>()) {
          (yyval.expression) = FloatLit::a(-(yyvsp[0].expression)->cast<FloatLit>()->v());
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_MINUS, (yyvsp[0].expression));
        }
      }

    break;

  case 129:

    { if ((yyvsp[-2].expression) && (yyvsp[0].expression)) (yyvsp[-2].expression)->addAnnotation((yyvsp[0].expression)); (yyval.expression)=(yyvsp[-2].expression); }

    break;

  case 130:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_EQUIV, (yyvsp[0].expression)); }

    break;

  case 131:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IMPL, (yyvsp[0].expression)); }

    break;

  case 132:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_RIMPL, (yyvsp[0].expression)); }

    break;

  case 133:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_OR, (yyvsp[0].expression)); }

    break;

  case 134:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_XOR, (yyvsp[0].expression)); }

    break;

  case 135:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_AND, (yyvsp[0].expression)); }

    break;

  case 136:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_LE, (yyvsp[0].expression)); }

    break;

  case 137:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_GR, (yyvsp[0].expression)); }

    break;

  case 138:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_LQ, (yyvsp[0].expression)); }

    break;

  case 139:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_GQ, (yyvsp[0].expression)); }

    break;

  case 140:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_EQ, (yyvsp[0].expression)); }

    break;

  case 141:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_NQ, (yyvsp[0].expression)); }

    break;

  case 142:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IN, (yyvsp[0].expression)); }

    break;

  case 143:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SUBSET, (yyvsp[0].expression)); }

    break;

  case 144:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SUPERSET, (yyvsp[0].expression)); }

    break;

  case 145:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_UNION, (yyvsp[0].expression)); }

    break;

  case 146:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIFF, (yyvsp[0].expression)); }

    break;

  case 147:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_SYMDIFF, (yyvsp[0].expression)); }

    break;

  case 148:

    { if ((yyvsp[-2].expression)==NULL || (yyvsp[0].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[-2].expression)->isa<IntLit>() && (yyvsp[0].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[-2].expression)->cast<IntLit>()->v(),(yyvsp[0].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DOTDOT, (yyvsp[0].expression));
        }
      }

    break;

  case 149:

    { if ((yyvsp[-3].expression)==NULL || (yyvsp[-1].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[-3].expression)->isa<IntLit>() && (yyvsp[-1].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[-3].expression)->cast<IntLit>()->v(),(yyvsp[-1].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-3].expression), BOT_DOTDOT, (yyvsp[-1].expression));
        }
      }

    break;

  case 150:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_INTERSECT, (yyvsp[0].expression)); }

    break;

  case 151:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_PLUSPLUS, (yyvsp[0].expression)); }

    break;

  case 152:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_PLUS, (yyvsp[0].expression)); }

    break;

  case 153:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MINUS, (yyvsp[0].expression)); }

    break;

  case 154:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MULT, (yyvsp[0].expression)); }

    break;

  case 155:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_DIV, (yyvsp[0].expression)); }

    break;

  case 156:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_IDIV, (yyvsp[0].expression)); }

    break;

  case 157:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_MOD, (yyvsp[0].expression)); }

    break;

  case 158:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[-2].expression), BOT_POW, (yyvsp[0].expression)); }

    break;

  case 159:

    { vector<Expression*> args;
        args.push_back((yyvsp[-2].expression)); args.push_back((yyvsp[0].expression));
        (yyval.expression)=new Call((yyloc), (yyvsp[-1].sValue), args);
        free((yyvsp[-1].sValue));
      }

    break;

  case 160:

    { (yyval.expression)=new UnOp((yyloc), UOT_NOT, (yyvsp[0].expression)); }

    break;

  case 161:

    { if (((yyvsp[0].expression) && (yyvsp[0].expression)->isa<IntLit>()) || ((yyvsp[0].expression) && (yyvsp[0].expression)->isa<FloatLit>())) {
          (yyval.expression) = (yyvsp[0].expression);
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[0].expression));
        }
      }

    break;

  case 162:

    { if ((yyvsp[0].expression) && (yyvsp[0].expression)->isa<IntLit>()) {
          (yyval.expression) = IntLit::a(-(yyvsp[0].expression)->cast<IntLit>()->v());
        } else if ((yyvsp[0].expression) && (yyvsp[0].expression)->isa<FloatLit>()) {
          (yyval.expression) = FloatLit::a(-(yyvsp[0].expression)->cast<FloatLit>()->v());
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_MINUS, (yyvsp[0].expression));
        }
      }

    break;

  case 163:

    { (yyval.expression)=(yyvsp[0].expression); }

    break;

  case 164:

    { (yyval.expression)=(yyvsp[0].expression); }

    break;

  case 165:

    { (yyval.expression)=(yyvsp[-1].expression); }

    break;

  case 166:

    { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-2].expression), *(yyvsp[0].expression_vv)); delete (yyvsp[0].expression_vv); }

    break;

  case 167:

    { (yyval.expression)=new Id((yyloc), (yyvsp[0].sValue), NULL); free((yyvsp[0].sValue)); }

    break;

  case 168:

    { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), new Id((yylsp[-1]),(yyvsp[-1].sValue),NULL), *(yyvsp[0].expression_vv));
        free((yyvsp[-1].sValue)); delete (yyvsp[0].expression_vv); }

    break;

  case 169:

    { (yyval.expression)=new AnonVar((yyloc)); }

    break;

  case 170:

    { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), new AnonVar((yyloc)), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }

    break;

  case 171:

    { (yyval.expression)=constants().boollit(((yyvsp[0].iValue)!=0)); }

    break;

  case 172:

    { (yyval.expression)=IntLit::a((yyvsp[0].iValue)); }

    break;

  case 173:

    { (yyval.expression)=IntLit::a(IntVal::infinity()); }

    break;

  case 174:

    { (yyval.expression)=FloatLit::a((yyvsp[0].dValue)); }

    break;

  case 175:

    { (yyval.expression)=constants().absent; }

    break;

  case 177:

    { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }

    break;

  case 179:

    { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }

    break;

  case 181:

    { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }

    break;

  case 183:

    { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }

    break;

  case 185:

    { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }

    break;

  case 187:

    { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }

    break;

  case 190:

    { if ((yyvsp[0].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[-1].expression), *(yyvsp[0].expression_vv));
        delete (yyvsp[0].expression_vv); }

    break;

  case 191:

    { (yyval.expression)=new StringLit((yyloc), (yyvsp[0].sValue)); free((yyvsp[0].sValue)); }

    break;

  case 192:

    { (yyval.expression)=new BinOp((yyloc), new StringLit((yyloc), (yyvsp[-1].sValue)), BOT_PLUSPLUS, (yyvsp[0].expression));
        free((yyvsp[-1].sValue));
      }

    break;

  case 193:

    { if ((yyvsp[-1].expression_v)) (yyval.expression)=new BinOp((yyloc), new Call((yyloc), ASTString("format"), *(yyvsp[-1].expression_v)), BOT_PLUSPLUS, new StringLit((yyloc),(yyvsp[0].sValue)));
        free((yyvsp[0].sValue));
        delete (yyvsp[-1].expression_v);
      }

    break;

  case 194:

    { if ((yyvsp[-2].expression_v)) (yyval.expression)=new BinOp((yyloc), new Call((yyloc), ASTString("format"), *(yyvsp[-2].expression_v)), BOT_PLUSPLUS,
                             new BinOp((yyloc), new StringLit((yyloc),(yyvsp[-1].sValue)), BOT_PLUSPLUS, (yyvsp[0].expression)));
        free((yyvsp[-1].sValue));
        delete (yyvsp[-2].expression_v);
      }

    break;

  case 195:

    { (yyval.expression_vv)=new std::vector<std::vector<Expression*> >();
        if ((yyvsp[-1].expression_v)) {
          (yyval.expression_vv)->push_back(*(yyvsp[-1].expression_v));
          delete (yyvsp[-1].expression_v);
        }
      }

    break;

  case 196:

    { (yyval.expression_vv)=(yyvsp[-3].expression_vv);
        if ((yyval.expression_vv) && (yyvsp[-1].expression_v)) {
          (yyval.expression_vv)->push_back(*(yyvsp[-1].expression_v));
          delete (yyvsp[-1].expression_v);
        }
      }

    break;

  case 197:

    { (yyval.expression) = new SetLit((yyloc), std::vector<Expression*>()); }

    break;

  case 198:

    { if ((yyvsp[-1].expression_v)) (yyval.expression) = new SetLit((yyloc), *(yyvsp[-1].expression_v));
        delete (yyvsp[-1].expression_v); }

    break;

  case 199:

    { if ((yyvsp[-1].generators)) (yyval.expression) = new Comprehension((yyloc), (yyvsp[-3].expression), *(yyvsp[-1].generators), true);
        delete (yyvsp[-1].generators);
      }

    break;

  case 200:

    { if ((yyvsp[0].generator_v)) (yyval.generators)=new Generators; (yyval.generators)->_g = *(yyvsp[0].generator_v); delete (yyvsp[0].generator_v); }

    break;

  case 202:

    { (yyval.generator_v)=new std::vector<Generator>; if ((yyvsp[0].generator)) (yyval.generator_v)->push_back(*(yyvsp[0].generator)); delete (yyvsp[0].generator); }

    break;

  case 203:

    { (yyval.generator_v)=(yyvsp[-2].generator_v); if ((yyval.generator_v) && (yyvsp[0].generator)) (yyval.generator_v)->push_back(*(yyvsp[0].generator)); delete (yyvsp[0].generator); }

    break;

  case 204:

    { if ((yyvsp[-2].string_v) && (yyvsp[0].expression)) (yyval.generator)=new Generator(*(yyvsp[-2].string_v),(yyvsp[0].expression),NULL); else (yyval.generator)=NULL; delete (yyvsp[-2].string_v); }

    break;

  case 205:

    { if ((yyvsp[-4].string_v) && (yyvsp[-2].expression)) (yyval.generator)=new Generator(*(yyvsp[-4].string_v),(yyvsp[-2].expression),(yyvsp[0].expression)); else (yyval.generator)=NULL; delete (yyvsp[-4].string_v); }

    break;

  case 206:

    { if ((yyvsp[0].expression)) (yyval.generator)=new Generator({(yyvsp[-2].sValue)},NULL,(yyvsp[0].expression)); else (yyval.generator)=NULL; free((yyvsp[-2].sValue)); }

    break;

  case 208:

    { (yyval.string_v)=new std::vector<std::string>; (yyval.string_v)->push_back((yyvsp[0].sValue)); free((yyvsp[0].sValue)); }

    break;

  case 209:

    { (yyval.string_v)=(yyvsp[-2].string_v); if ((yyval.string_v) && (yyvsp[0].sValue)) (yyval.string_v)->push_back((yyvsp[0].sValue)); free((yyvsp[0].sValue)); }

    break;

  case 210:

    { (yyval.expression)=new ArrayLit((yyloc), std::vector<MiniZinc::Expression*>()); }

    break;

  case 211:

    { if ((yyvsp[-1].expression_v)) (yyval.expression)=new ArrayLit((yyloc), *(yyvsp[-1].expression_v)); delete (yyvsp[-1].expression_v); }

    break;

  case 212:

    { (yyval.expression)=new ArrayLit((yyloc), std::vector<std::vector<Expression*> >()); }

    break;

  case 213:

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

  case 214:

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

  case 215:

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

  case 216:

    { (yyval.expression_vvv)=new std::vector<std::vector<std::vector<MiniZinc::Expression*> > >;
      }

    break;

  case 217:

    { (yyval.expression_vvv)=new std::vector<std::vector<std::vector<MiniZinc::Expression*> > >;
        if ((yyvsp[-1].expression_vv)) (yyval.expression_vvv)->push_back(*(yyvsp[-1].expression_vv));
        delete (yyvsp[-1].expression_vv);
      }

    break;

  case 218:

    { (yyval.expression_vvv)=(yyvsp[-4].expression_vvv);
        if ((yyval.expression_vvv) && (yyvsp[-1].expression_vv)) (yyval.expression_vvv)->push_back(*(yyvsp[-1].expression_vv));
        delete (yyvsp[-1].expression_vv);
      }

    break;

  case 219:

    { (yyval.expression_vv)=new std::vector<std::vector<MiniZinc::Expression*> >;
        if ((yyvsp[0].expression_v)) (yyval.expression_vv)->push_back(*(yyvsp[0].expression_v));
        delete (yyvsp[0].expression_v);
      }

    break;

  case 220:

    { (yyval.expression_vv)=(yyvsp[-2].expression_vv); if ((yyval.expression_vv) && (yyvsp[0].expression_v)) (yyval.expression_vv)->push_back(*(yyvsp[0].expression_v)); delete (yyvsp[0].expression_v); }

    break;

  case 221:

    { if ((yyvsp[-1].generators)) (yyval.expression)=new Comprehension((yyloc), (yyvsp[-3].expression), *(yyvsp[-1].generators), false);
        delete (yyvsp[-1].generators);
      }

    break;

  case 222:

    {
        std::vector<Expression*> iexps;
        iexps.push_back((yyvsp[-3].expression));
        iexps.push_back((yyvsp[-1].expression));
        (yyval.expression)=new ITE((yyloc), iexps, NULL);
      }

    break;

  case 223:

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

  case 224:

    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; }

    break;

  case 225:

    { (yyval.expression_v)=(yyvsp[-4].expression_v); if ((yyval.expression_v) && (yyvsp[-2].expression) && (yyvsp[0].expression)) { (yyval.expression_v)->push_back((yyvsp[-2].expression)); (yyval.expression_v)->push_back((yyvsp[0].expression)); } }

    break;

  case 226:

    { (yyval.iValue)=BOT_EQUIV; }

    break;

  case 227:

    { (yyval.iValue)=BOT_IMPL; }

    break;

  case 228:

    { (yyval.iValue)=BOT_RIMPL; }

    break;

  case 229:

    { (yyval.iValue)=BOT_OR; }

    break;

  case 230:

    { (yyval.iValue)=BOT_XOR; }

    break;

  case 231:

    { (yyval.iValue)=BOT_AND; }

    break;

  case 232:

    { (yyval.iValue)=BOT_LE; }

    break;

  case 233:

    { (yyval.iValue)=BOT_GR; }

    break;

  case 234:

    { (yyval.iValue)=BOT_LQ; }

    break;

  case 235:

    { (yyval.iValue)=BOT_GQ; }

    break;

  case 236:

    { (yyval.iValue)=BOT_EQ; }

    break;

  case 237:

    { (yyval.iValue)=BOT_NQ; }

    break;

  case 238:

    { (yyval.iValue)=BOT_IN; }

    break;

  case 239:

    { (yyval.iValue)=BOT_SUBSET; }

    break;

  case 240:

    { (yyval.iValue)=BOT_SUPERSET; }

    break;

  case 241:

    { (yyval.iValue)=BOT_UNION; }

    break;

  case 242:

    { (yyval.iValue)=BOT_DIFF; }

    break;

  case 243:

    { (yyval.iValue)=BOT_SYMDIFF; }

    break;

  case 244:

    { (yyval.iValue)=BOT_PLUS; }

    break;

  case 245:

    { (yyval.iValue)=BOT_MINUS; }

    break;

  case 246:

    { (yyval.iValue)=BOT_MULT; }

    break;

  case 247:

    { (yyval.iValue)=BOT_POW; }

    break;

  case 248:

    { (yyval.iValue)=BOT_DIV; }

    break;

  case 249:

    { (yyval.iValue)=BOT_IDIV; }

    break;

  case 250:

    { (yyval.iValue)=BOT_MOD; }

    break;

  case 251:

    { (yyval.iValue)=BOT_INTERSECT; }

    break;

  case 252:

    { (yyval.iValue)=BOT_PLUSPLUS; }

    break;

  case 253:

    { (yyval.iValue)=-1; }

    break;

  case 254:

    { if ((yyvsp[-5].iValue)==-1) {
          (yyval.expression)=NULL;
          yyerror(&(yylsp[-3]), parm, "syntax error, unary operator with two arguments");
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[-3].expression),static_cast<BinOpType>((yyvsp[-5].iValue)),(yyvsp[-1].expression));
        }
      }

    break;

  case 255:

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

  case 256:

    { (yyval.expression)=new Call((yyloc), (yyvsp[-2].sValue), std::vector<Expression*>()); free((yyvsp[-2].sValue)); }

    break;

  case 258:

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

  case 259:

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

  case 261:

    { (yyval.expression_p)=new vector<pair<Expression*,Expression*> >;
        (yyval.expression_p)->push_back(pair<Expression*,Expression*>((yyvsp[0].expression),NULL)); }

    break;

  case 262:

    { (yyval.expression_p)=new vector<pair<Expression*,Expression*> >;
        (yyval.expression_p)->push_back(pair<Expression*,Expression*>((yyvsp[-2].expression),(yyvsp[0].expression))); }

    break;

  case 263:

    { (yyval.expression_p)=(yyvsp[-2].expression_p); if ((yyval.expression_p) && (yyvsp[0].expression)) (yyval.expression_p)->push_back(pair<Expression*,Expression*>((yyvsp[0].expression),NULL)); }

    break;

  case 264:

    { (yyval.expression_p)=(yyvsp[-4].expression_p); if ((yyval.expression_p) && (yyvsp[-2].expression) && (yyvsp[0].expression)) (yyval.expression_p)->push_back(pair<Expression*,Expression*>((yyvsp[-2].expression),(yyvsp[0].expression))); }

    break;

  case 265:

    { if ((yyvsp[-3].expression_v) && (yyvsp[0].expression)) {
          (yyval.expression)=new Let((yyloc), *(yyvsp[-3].expression_v), (yyvsp[0].expression)); delete (yyvsp[-3].expression_v);
        } else {
          (yyval.expression)=NULL;
        }
      }

    break;

  case 266:

    { if ((yyvsp[-4].expression_v) && (yyvsp[0].expression)) {
          (yyval.expression)=new Let((yyloc), *(yyvsp[-4].expression_v), (yyvsp[0].expression)); delete (yyvsp[-4].expression_v);
        } else {
          (yyval.expression)=NULL;
        }
      }

    break;

  case 267:

    { (yyval.expression_v)=new vector<Expression*>; (yyval.expression_v)->push_back((yyvsp[0].vardeclexpr)); }

    break;

  case 268:

    { (yyval.expression_v)=new vector<Expression*>;
        if ((yyvsp[0].item)) {
          ConstraintI* ce = (yyvsp[0].item)->cast<ConstraintI>();
          (yyval.expression_v)->push_back(ce->e());
          ce->e(NULL);
        }
      }

    break;

  case 269:

    { (yyval.expression_v)=(yyvsp[-2].expression_v); if ((yyval.expression_v) && (yyvsp[0].vardeclexpr)) (yyval.expression_v)->push_back((yyvsp[0].vardeclexpr)); }

    break;

  case 270:

    { (yyval.expression_v)=(yyvsp[-2].expression_v);
        if ((yyval.expression_v) && (yyvsp[0].item)) {
          ConstraintI* ce = (yyvsp[0].item)->cast<ConstraintI>();
          (yyval.expression_v)->push_back(ce->e());
          ce->e(NULL);
        }
      }

    break;

  case 273:

    { (yyval.vardeclexpr) = (yyvsp[-1].vardeclexpr);
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->toplevel(false);
        if ((yyval.vardeclexpr) && (yyvsp[0].expression_v)) (yyval.vardeclexpr)->addAnnotations(*(yyvsp[0].expression_v));
        delete (yyvsp[0].expression_v);
      }

    break;

  case 274:

    { if ((yyvsp[-3].vardeclexpr)) (yyvsp[-3].vardeclexpr)->e((yyvsp[0].expression));
        (yyval.vardeclexpr) = (yyvsp[-3].vardeclexpr);
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->loc((yyloc));
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->toplevel(false);
        if ((yyval.vardeclexpr) && (yyvsp[-2].expression_v)) (yyval.vardeclexpr)->addAnnotations(*(yyvsp[-2].expression_v));
        delete (yyvsp[-2].expression_v);
      }

    break;

  case 275:

    { (yyval.expression_v)=NULL; }

    break;

  case 277:

    { (yyval.expression) = (yyvsp[0].expression); }

    break;

  case 278:

    { (yyval.expression) = new Call((yylsp[0]), ASTString("mzn_expression_name"), {(yyvsp[0].expression)}); }

    break;

  case 279:

    { (yyval.expression_v)=new std::vector<Expression*>(1);
        (*(yyval.expression_v))[0] = (yyvsp[0].expression);
      }

    break;

  case 280:

    { (yyval.expression_v)=(yyvsp[-2].expression_v); if ((yyval.expression_v)) (yyval.expression_v)->push_back((yyvsp[0].expression)); }

    break;

  case 281:

    { (yyval.sValue)=(yyvsp[0].sValue); }

    break;

  case 282:

    { (yyval.sValue)=strdup("'<->'"); }

    break;

  case 283:

    { (yyval.sValue)=strdup("'->'"); }

    break;

  case 284:

    { (yyval.sValue)=strdup("'<-'"); }

    break;

  case 285:

    { (yyval.sValue)=strdup("'\\/'"); }

    break;

  case 286:

    { (yyval.sValue)=strdup("'xor'"); }

    break;

  case 287:

    { (yyval.sValue)=strdup("'/\\'"); }

    break;

  case 288:

    { (yyval.sValue)=strdup("'<'"); }

    break;

  case 289:

    { (yyval.sValue)=strdup("'>'"); }

    break;

  case 290:

    { (yyval.sValue)=strdup("'<='"); }

    break;

  case 291:

    { (yyval.sValue)=strdup("'>='"); }

    break;

  case 292:

    { (yyval.sValue)=strdup("'='"); }

    break;

  case 293:

    { (yyval.sValue)=strdup("'!='"); }

    break;

  case 294:

    { (yyval.sValue)=strdup("'in'"); }

    break;

  case 295:

    { (yyval.sValue)=strdup("'subset'"); }

    break;

  case 296:

    { (yyval.sValue)=strdup("'superset'"); }

    break;

  case 297:

    { (yyval.sValue)=strdup("'union'"); }

    break;

  case 298:

    { (yyval.sValue)=strdup("'diff'"); }

    break;

  case 299:

    { (yyval.sValue)=strdup("'symdiff'"); }

    break;

  case 300:

    { (yyval.sValue)=strdup("'..'"); }

    break;

  case 301:

    { (yyval.sValue)=strdup("'+'"); }

    break;

  case 302:

    { (yyval.sValue)=strdup("'-'"); }

    break;

  case 303:

    { (yyval.sValue)=strdup("'*'"); }

    break;

  case 304:

    { (yyval.sValue)=strdup("'^'"); }

    break;

  case 305:

    { (yyval.sValue)=strdup("'/'"); }

    break;

  case 306:

    { (yyval.sValue)=strdup("'div'"); }

    break;

  case 307:

    { (yyval.sValue)=strdup("'mod'"); }

    break;

  case 308:

    { (yyval.sValue)=strdup("'intersect'"); }

    break;

  case 309:

    { (yyval.sValue)=strdup("'not'"); }

    break;

  case 310:

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
