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
     MZN_DOC_COMMENT = 265,
     MZN_VAR = 266,
     MZN_PAR = 267,
     MZN_SVAR = 268,
     MZN_ABSENT = 269,
     MZN_ANN = 270,
     MZN_ANNOTATION = 271,
     MZN_ANY = 272,
     MZN_ARRAY = 273,
     MZN_BOOL = 274,
     MZN_CASE = 275,
     MZN_CONSTRAINT = 276,
     MZN_DEFAULT = 277,
     MZN_ELSE = 278,
     MZN_ELSEIF = 279,
     MZN_ENDIF = 280,
     MZN_ENUM = 281,
     MZN_FLOAT = 282,
     MZN_FUNCTION = 283,
     MZN_IF = 284,
     MZN_INCLUDE = 285,
     MZN_INFINITY = 286,
     MZN_INT = 287,
     MZN_LET = 288,
     MZN_LIST = 289,
     MZN_MAXIMIZE = 290,
     MZN_MINIMIZE = 291,
     MZN_OF = 292,
     MZN_OPT = 293,
     MZN_SATISFY = 294,
     MZN_OUTPUT = 295,
     MZN_PREDICATE = 296,
     MZN_RECORD = 297,
     MZN_SET = 298,
     MZN_SOLVE = 299,
     MZN_STRING = 300,
     MZN_TEST = 301,
     MZN_THEN = 302,
     MZN_TUPLE = 303,
     MZN_TYPE = 304,
     MZN_UNDERSCORE = 305,
     MZN_VARIANT_RECORD = 306,
     MZN_WHERE = 307,
     MZN_LEFT_BRACKET = 308,
     MZN_LEFT_2D_BRACKET = 309,
     MZN_RIGHT_BRACKET = 310,
     MZN_RIGHT_2D_BRACKET = 311,
     UNKNOWN_CHAR = 312,
     FLATZINC_IDENTIFIER = 313,
     PREC_ANNO = 314,
     MZN_EQUIV = 315,
     MZN_RIMPL = 316,
     MZN_IMPL = 317,
     MZN_XOR = 318,
     MZN_OR = 319,
     MZN_AND = 320,
     MZN_NQ = 321,
     MZN_EQ = 322,
     MZN_GQ = 323,
     MZN_LQ = 324,
     MZN_GR = 325,
     MZN_LE = 326,
     MZN_SUPERSET = 327,
     MZN_SUBSET = 328,
     MZN_IN = 329,
     MZN_SYMDIFF = 330,
     MZN_DIFF = 331,
     MZN_UNION = 332,
     MZN_DOTDOT = 333,
     MZN_MINUS = 334,
     MZN_PLUS = 335,
     MZN_INTERSECT = 336,
     MZN_MOD = 337,
     MZN_IDIV = 338,
     MZN_DIV = 339,
     MZN_MULT = 340,
     MZN_NOT = 341,
     MZN_PLUSPLUS = 342,
     MZN_COLONCOLON = 343,
     MZN_EQUIV_QUOTED = 344,
     MZN_IMPL_QUOTED = 345,
     MZN_RIMPL_QUOTED = 346,
     MZN_OR_QUOTED = 347,
     MZN_XOR_QUOTED = 348,
     MZN_AND_QUOTED = 349,
     MZN_LE_QUOTED = 350,
     MZN_GR_QUOTED = 351,
     MZN_LQ_QUOTED = 352,
     MZN_GQ_QUOTED = 353,
     MZN_EQ_QUOTED = 354,
     MZN_NQ_QUOTED = 355,
     MZN_IN_QUOTED = 356,
     MZN_SUBSET_QUOTED = 357,
     MZN_SUPERSET_QUOTED = 358,
     MZN_UNION_QUOTED = 359,
     MZN_DIFF_QUOTED = 360,
     MZN_SYMDIFF_QUOTED = 361,
     MZN_DOTDOT_QUOTED = 362,
     MZN_PLUS_QUOTED = 363,
     MZN_MINUS_QUOTED = 364,
     MZN_MULT_QUOTED = 365,
     MZN_DIV_QUOTED = 366,
     MZN_IDIV_QUOTED = 367,
     MZN_MOD_QUOTED = 368,
     MZN_INTERSECT_QUOTED = 369,
     MZN_NOT_QUOTED = 370,
     MZN_COLONCOLON_QUOTED = 371,
     MZN_PLUSPLUS_QUOTED = 372
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
#define MZN_DOC_COMMENT 265
#define MZN_VAR 266
#define MZN_PAR 267
#define MZN_SVAR 268
#define MZN_ABSENT 269
#define MZN_ANN 270
#define MZN_ANNOTATION 271
#define MZN_ANY 272
#define MZN_ARRAY 273
#define MZN_BOOL 274
#define MZN_CASE 275
#define MZN_CONSTRAINT 276
#define MZN_DEFAULT 277
#define MZN_ELSE 278
#define MZN_ELSEIF 279
#define MZN_ENDIF 280
#define MZN_ENUM 281
#define MZN_FLOAT 282
#define MZN_FUNCTION 283
#define MZN_IF 284
#define MZN_INCLUDE 285
#define MZN_INFINITY 286
#define MZN_INT 287
#define MZN_LET 288
#define MZN_LIST 289
#define MZN_MAXIMIZE 290
#define MZN_MINIMIZE 291
#define MZN_OF 292
#define MZN_OPT 293
#define MZN_SATISFY 294
#define MZN_OUTPUT 295
#define MZN_PREDICATE 296
#define MZN_RECORD 297
#define MZN_SET 298
#define MZN_SOLVE 299
#define MZN_STRING 300
#define MZN_TEST 301
#define MZN_THEN 302
#define MZN_TUPLE 303
#define MZN_TYPE 304
#define MZN_UNDERSCORE 305
#define MZN_VARIANT_RECORD 306
#define MZN_WHERE 307
#define MZN_LEFT_BRACKET 308
#define MZN_LEFT_2D_BRACKET 309
#define MZN_RIGHT_BRACKET 310
#define MZN_RIGHT_2D_BRACKET 311
#define UNKNOWN_CHAR 312
#define FLATZINC_IDENTIFIER 313
#define PREC_ANNO 314
#define MZN_EQUIV 315
#define MZN_RIMPL 316
#define MZN_IMPL 317
#define MZN_XOR 318
#define MZN_OR 319
#define MZN_AND 320
#define MZN_NQ 321
#define MZN_EQ 322
#define MZN_GQ 323
#define MZN_LQ 324
#define MZN_GR 325
#define MZN_LE 326
#define MZN_SUPERSET 327
#define MZN_SUBSET 328
#define MZN_IN 329
#define MZN_SYMDIFF 330
#define MZN_DIFF 331
#define MZN_UNION 332
#define MZN_DOTDOT 333
#define MZN_MINUS 334
#define MZN_PLUS 335
#define MZN_INTERSECT 336
#define MZN_MOD 337
#define MZN_IDIV 338
#define MZN_DIV 339
#define MZN_MULT 340
#define MZN_NOT 341
#define MZN_PLUSPLUS 342
#define MZN_COLONCOLON 343
#define MZN_EQUIV_QUOTED 344
#define MZN_IMPL_QUOTED 345
#define MZN_RIMPL_QUOTED 346
#define MZN_OR_QUOTED 347
#define MZN_XOR_QUOTED 348
#define MZN_AND_QUOTED 349
#define MZN_LE_QUOTED 350
#define MZN_GR_QUOTED 351
#define MZN_LQ_QUOTED 352
#define MZN_GQ_QUOTED 353
#define MZN_EQ_QUOTED 354
#define MZN_NQ_QUOTED 355
#define MZN_IN_QUOTED 356
#define MZN_SUBSET_QUOTED 357
#define MZN_SUPERSET_QUOTED 358
#define MZN_UNION_QUOTED 359
#define MZN_DIFF_QUOTED 360
#define MZN_SYMDIFF_QUOTED 361
#define MZN_DOTDOT_QUOTED 362
#define MZN_PLUS_QUOTED 363
#define MZN_MINUS_QUOTED 364
#define MZN_MULT_QUOTED 365
#define MZN_DIV_QUOTED 366
#define MZN_IDIV_QUOTED 367
#define MZN_MOD_QUOTED 368
#define MZN_INTERSECT_QUOTED 369
#define MZN_NOT_QUOTED 370
#define MZN_COLONCOLON_QUOTED 371
#define MZN_PLUSPLUS_QUOTED 372




/* Copy the first part of user declarations.  */


#define YYPARSE_PARAM parm
#define YYLEX_PARAM static_cast<ParserState*>(parm)->yyscanner
#include <iostream>
#include <fstream>
#include <map>
#include <cerrno>

namespace MiniZinc{ class Location; }
#define YYLTYPE MiniZinc::Location
#define YYLTYPE_IS_DECLARED 1
#define YYLTYPE_IS_TRIVIAL 0

#include <minizinc/parser.hh>

using namespace std;
using namespace MiniZinc;

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
    pp->err << "(included from file '" << m->filename() << "')" << endl;
  }
  pp->err << location->filename << ":"
          << location->first_line << ":" << endl;
  pp->printCurrentLine();
  for (unsigned int i=0; i<location->first_column-1; i++)
    pp->err << " ";
  for (unsigned int i=location->first_column; i<=location->last_column; i++)
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
    return(contents);
  }
  throw(errno);
}

Expression* createDocComment(const Location& loc, const std::string& s) {
  std::vector<Expression*> args(1);
  args[0] = new StringLit(loc, s);
  Call* c = new Call(loc, constants().ann.doc_comment, args);
  c->type(Type::ann());
  return c;
}

namespace MiniZinc {

  Model* parseFromString(const string& text,
                         const string& filename,
                         const vector<string>& ip,
                         bool ignoreStdlib,
                         ostream& err) {
    GCLock lock;

    vector<string> includePaths;
    for (unsigned int i=0; i<ip.size(); i++)
      includePaths.push_back(ip[i]);

    vector<pair<string,Model*> > files;
    map<string,Model*> seenModels;

    Model* model = new Model();
    model->setFilename(filename);

    if (!ignoreStdlib) {
      Model* stdlib = new Model;
      stdlib->setFilename("stdlib.mzn");
      files.push_back(pair<string,Model*>("./",stdlib));
      seenModels.insert(pair<string,Model*>("stdlib.mzn",stdlib));
      IncludeI* stdlibinc = new IncludeI(Location(),stdlib->filename());
      stdlibinc->m(stdlib,true);
      model->addItem(stdlibinc);

      Model* builtins = new Model;
      builtins->setFilename("builtins.mzn");
      files.push_back(pair<string,Model*>("./",builtins));
      seenModels.insert(pair<string,Model*>("builtins.mzn",builtins));
      IncludeI* builtinsinc = new IncludeI(Location(),builtins->filename());
      builtinsinc->m(builtins,true);
      model->addItem(builtinsinc);
    }

    model->setFilepath(filename);
    bool isFzn = (filename.compare(filename.length()-4,4,".fzn")==0);
    isFzn |= (filename.compare(filename.length()-4,4,".ozn")==0);
    isFzn |= (filename.compare(filename.length()-4,4,".szn")==0);
    ParserState pp(filename,text, err, files, seenModels, model, false, isFzn);
    yylex_init(&pp.yyscanner);
    yyset_extra(&pp, pp.yyscanner);
    yyparse(&pp);
    if (pp.yyscanner)
    yylex_destroy(pp.yyscanner);
    if (pp.hadError) {
      goto error;
    }

    while (!files.empty()) {
      pair<string,Model*>& np = files.back();
      string parentPath = np.first;
      Model* m = np.second;
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
      if (parentPath=="") {
        fullname = filename;
        file.open(fullname.c_str(), std::ios::binary);
      } else {
        includePaths.push_back(parentPath);
        for (unsigned int i=0; i<includePaths.size(); i++) {
          fullname = includePaths[i]+f;
          file.open(fullname.c_str(), std::ios::binary);
          if (file.is_open())
            break;
        }
        includePaths.pop_back();
      }
      if (!file.is_open()) {
        err << "Error: cannot open file '" << f << "'." << endl;
        goto error;
      }
      std::string s = get_file_contents(file);

      m->setFilepath(fullname);
      bool isFzn = (fullname.compare(fullname.length()-4,4,".fzn")==0);
      isFzn |= (fullname.compare(fullname.length()-4,4,".ozn")==0);
      isFzn |= (fullname.compare(fullname.length()-4,4,".szn")==0);
      ParserState pp(fullname,s, err, files, seenModels, m, false, isFzn);
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

  Model* parse(const string& filename,
               const vector<string>& datafiles,
               const vector<string>& ip,
               bool ignoreStdlib,
               ostream& err) {
    GCLock lock;
    string fileDirname; string fileBasename;
    filepath(filename, fileDirname, fileBasename);

    vector<string> includePaths;
    for (unsigned int i=0; i<ip.size(); i++)
      includePaths.push_back(ip[i]);
    
    vector<pair<string,Model*> > files;
    map<string,Model*> seenModels;
    
    Model* model = new Model();
    model->setFilename(fileBasename);

    if (!ignoreStdlib) {
      Model* stdlib = new Model;
      stdlib->setFilename("stdlib.mzn");
      files.push_back(pair<string,Model*>("./",stdlib));
      seenModels.insert(pair<string,Model*>("stdlib.mzn",stdlib));
      Location stdlibloc;
      stdlibloc.filename=ASTString(filename);
      IncludeI* stdlibinc = 
        new IncludeI(stdlibloc,stdlib->filename());
      stdlibinc->m(stdlib,true);
      model->addItem(stdlibinc);

      Model* builtins = new Model;
      builtins->setFilename("builtins.mzn");
      files.push_back(pair<string,Model*>("./",builtins));
      seenModels.insert(pair<string,Model*>("builtins.mzn",builtins));
      Location builtinsloc;
      builtinsloc.filename=ASTString(filename);
      IncludeI* builtinsinc = 
        new IncludeI(builtinsloc,builtins->filename());
      builtinsinc->m(builtins,true);
      model->addItem(builtinsinc);
    }
    
    files.push_back(pair<string,Model*>("",model));
        
    while (!files.empty()) {
      pair<string,Model*>& np = files.back();
      string parentPath = np.first;
      Model* m = np.second;
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
      if (parentPath=="") {
        fullname = filename;
        file.open(fullname.c_str(), std::ios::binary);
      } else {
        includePaths.push_back(parentPath);
        for (unsigned int i=0; i<includePaths.size(); i++) {
          fullname = includePaths[i]+f;
          file.open(fullname.c_str(), std::ios::binary);
          if (file.is_open())
            break;
        }
        includePaths.pop_back();
      }
      if (!file.is_open()) {
        err << "Error: cannot open file '" << f << "'." << endl;
        goto error;
      }
      std::string s = get_file_contents(file);

      m->setFilepath(fullname);
      bool isFzn = (fullname.compare(fullname.length()-4,4,".fzn")==0);
      isFzn |= (fullname.compare(fullname.length()-4,4,".ozn")==0);
      isFzn |= (fullname.compare(fullname.length()-4,4,".szn")==0);
      ParserState pp(fullname,s, err, files, seenModels, m, false, isFzn);
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
      std::string s;
      if (f.size() > 5 && f.substr(0,5)=="cmd:/") {
        s = f.substr(5);
      } else {
        std::ifstream file;
        file.open(f.c_str(), std::ios::binary);
        if (!file.is_open()) {
          err << "Error: cannot open data file '" << f << "'." << endl;
          goto error;
        }
        s = get_file_contents(file);
      }

      ParserState pp(f, s, err, files, seenModels, model, true, false);
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

  Model* parseData(Model* model,
                   const vector<string>& datafiles,
                   const vector<string>& includePaths,
                   bool ignoreStdlib,
                   ostream& err) {
  GCLock lock;

  vector<pair<string,Model*> > files;
  map<string,Model*> seenModels;
  
  if (!ignoreStdlib) {
    Model* stdlib = new Model;
    stdlib->setFilename("stdlib.mzn");
    files.push_back(pair<string,Model*>("./",stdlib));
    seenModels.insert(pair<string,Model*>("stdlib.mzn",stdlib));
    IncludeI* stdlibinc =
      new IncludeI(Location(),stdlib->filename());
    stdlibinc->m(stdlib,true);
    model->addItem(stdlibinc);
    
    Model* builtins = new Model;
    builtins->setFilename("builtins.mzn");
    files.push_back(pair<string,Model*>("./",builtins));
    seenModels.insert(pair<string,Model*>("builtins.mzn",builtins));
    IncludeI* builtinsinc =
      new IncludeI(Location(),builtins->filename());
    builtinsinc->m(builtins,true);
    model->addItem(builtinsinc);
  }
  
  while (!files.empty()) {
    pair<string,Model*>& np = files.back();
    string parentPath = np.first;
    Model* m = np.second;
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
    if (parentPath=="") {
      err << "Internal error." << std::endl;
      goto error;
    } else {
      for (unsigned int i=0; i<includePaths.size(); i++) {
        fullname = includePaths[i]+f;
        file.open(fullname.c_str(), std::ios::binary);
        if (file.is_open())
          break;
      }
    }
    if (!file.is_open()) {
      err << "Error: cannot open file '" << f << "'." << endl;
      goto error;
    }
    std::string s = get_file_contents(file);
    
    m->setFilepath(fullname);
    bool isFzn = (fullname.compare(fullname.length()-4,4,".fzn")==0);
    isFzn |= (fullname.compare(fullname.length()-4,4,".ozn")==0);
    isFzn |= (fullname.compare(fullname.length()-4,4,".szn")==0);
    ParserState pp(fullname,s, err, files, seenModels, m, false, isFzn);
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
    std::string s;
    if (f.size() > 5 && f.substr(0,5)=="cmd:/") {
      s = f.substr(5);
    } else {
      std::ifstream file;
      file.open(f.c_str(), std::ios::binary);
      if (!file.is_open()) {
        err << "Error: cannot open data file '" << f << "'." << endl;
        goto error;
      }
      s = get_file_contents(file);
    }
    
    ParserState pp(f, s, err, files, seenModels, model, true, false);
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
         MiniZinc::Generator* generator;
         std::vector<MiniZinc::Generator>* generator_v;
         std::vector<std::string>* string_v;
         std::pair<std::vector<MiniZinc::Expression*>,
                   MiniZinc::Expression*>* expression_p;
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
#define YYFINAL  140
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   3366

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  126
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  60
/* YYNRULES -- Number of rules.  */
#define YYNRULES  250
/* YYNRULES -- Number of states.  */
#define YYNSTATES  425

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   372

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     120,   121,     2,     2,   122,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   119,   118,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   123,   125,   124,     2,     2,     2,     2,
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
     115,   116,   117
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    11,    15,    16,    18,
      21,    23,    25,    27,    29,    31,    33,    35,    37,    39,
      41,    44,    47,    52,    56,    59,    63,    68,    73,    76,
      82,    88,    96,   105,   109,   115,   116,   119,   120,   124,
     128,   131,   133,   137,   138,   140,   142,   144,   148,   151,
     153,   157,   159,   166,   170,   172,   175,   179,   183,   188,
     194,   200,   201,   203,   205,   207,   209,   211,   213,   215,
     217,   220,   222,   226,   228,   232,   236,   240,   244,   248,
     255,   259,   263,   267,   271,   275,   279,   283,   287,   291,
     294,   297,   299,   303,   307,   311,   315,   319,   323,   327,
     331,   335,   339,   343,   347,   351,   355,   359,   363,   367,
     371,   375,   379,   386,   390,   394,   398,   402,   406,   410,
     414,   418,   422,   425,   428,   431,   435,   440,   442,   445,
     447,   450,   452,   454,   456,   458,   460,   462,   464,   466,
     468,   471,   473,   476,   478,   481,   483,   486,   488,   490,
     493,   497,   500,   504,   510,   512,   516,   519,   521,   525,
     529,   532,   534,   538,   541,   545,   549,   554,   556,   560,
     566,   575,   576,   582,   584,   586,   588,   590,   592,   594,
     596,   598,   600,   602,   604,   606,   608,   610,   612,   614,
     616,   618,   620,   622,   624,   626,   628,   630,   632,   634,
     636,   643,   648,   652,   654,   659,   667,   669,   673,   680,
     688,   690,   692,   696,   700,   702,   704,   707,   712,   713,
     715,   718,   722,   724,   726,   728,   730,   732,   734,   736,
     738,   740,   742,   744,   746,   748,   750,   752,   754,   756,
     758,   760,   762,   764,   766,   768,   770,   772,   774,   776,
     778
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     127,     0,    -1,   128,    -1,    -1,   129,   130,    -1,   131,
      -1,   129,   118,   131,    -1,    -1,   118,    -1,    10,   132,
      -1,   132,    -1,   133,    -1,   134,    -1,   135,    -1,   136,
      -1,   137,    -1,   138,    -1,   139,    -1,   140,    -1,   141,
      -1,    30,     8,    -1,   148,   183,    -1,   148,   183,    67,
     158,    -1,     6,    67,   158,    -1,    21,   158,    -1,    44,
     183,    39,    -1,    44,   183,    36,   158,    -1,    44,   183,
      35,   158,    -1,    40,   158,    -1,    41,     6,   143,   183,
     142,    -1,    46,     6,   143,   183,   142,    -1,    28,   151,
     119,   185,   143,   183,   142,    -1,   151,   119,     6,   120,
     144,   121,   183,   142,    -1,    16,     6,   143,    -1,    16,
       6,   143,    67,   158,    -1,    -1,    67,   158,    -1,    -1,
     120,   144,   121,    -1,   120,     1,   121,    -1,   145,   146,
      -1,   147,    -1,   145,   122,   147,    -1,    -1,   122,    -1,
     148,    -1,   151,    -1,   151,   119,     6,    -1,   150,   146,
      -1,   151,    -1,   150,   122,   151,    -1,   152,    -1,    18,
      53,   149,    55,    37,   152,    -1,    34,    37,   152,    -1,
     154,    -1,    38,   154,    -1,    12,   153,   154,    -1,    11,
     153,   154,    -1,   153,    43,    37,   154,    -1,    12,   153,
      43,    37,   154,    -1,    11,   153,    43,    37,   154,    -1,
      -1,    38,    -1,    32,    -1,    19,    -1,    27,    -1,    45,
      -1,    15,    -1,   157,    -1,     9,    -1,   156,   146,    -1,
     158,    -1,   156,   122,   158,    -1,   159,    -1,   157,    88,
     159,    -1,   157,    77,   157,    -1,   157,    76,   157,    -1,
     157,    75,   157,    -1,   157,    78,   157,    -1,   107,   120,
     158,   122,   158,   121,    -1,   157,    81,   157,    -1,   157,
      87,   157,    -1,   157,    80,   157,    -1,   157,    79,   157,
      -1,   157,    85,   157,    -1,   157,    84,   157,    -1,   157,
      83,   157,    -1,   157,    82,   157,    -1,   157,     7,   157,
      -1,    80,   157,    -1,    79,   157,    -1,   159,    -1,   158,
      88,   159,    -1,   158,    60,   158,    -1,   158,    62,   158,
      -1,   158,    61,   158,    -1,   158,    64,   158,    -1,   158,
      63,   158,    -1,   158,    65,   158,    -1,   158,    71,   158,
      -1,   158,    70,   158,    -1,   158,    69,   158,    -1,   158,
      68,   158,    -1,   158,    67,   158,    -1,   158,    66,   158,
      -1,   158,    74,   158,    -1,   158,    73,   158,    -1,   158,
      72,   158,    -1,   158,    77,   158,    -1,   158,    76,   158,
      -1,   158,    75,   158,    -1,   158,    78,   158,    -1,   107,
     120,   158,   122,   158,   121,    -1,   158,    81,   158,    -1,
     158,    87,   158,    -1,   158,    80,   158,    -1,   158,    79,
     158,    -1,   158,    85,   158,    -1,   158,    84,   158,    -1,
     158,    83,   158,    -1,   158,    82,   158,    -1,   158,     7,
     158,    -1,    86,   158,    -1,    80,   158,    -1,    79,   158,
      -1,   120,   158,   121,    -1,   120,   158,   121,   160,    -1,
       6,    -1,     6,   160,    -1,    50,    -1,    50,   160,    -1,
       4,    -1,     3,    -1,    31,    -1,     5,    -1,     8,    -1,
      14,    -1,   161,    -1,   162,    -1,   169,    -1,   169,   160,
      -1,   170,    -1,   170,   160,    -1,   172,    -1,   172,   160,
      -1,   173,    -1,   173,   160,    -1,   179,    -1,   177,    -1,
     177,   160,    -1,    53,   155,    55,    -1,   123,   124,    -1,
     123,   155,   124,    -1,   123,   158,   125,   163,   124,    -1,
     164,    -1,   164,    52,   158,    -1,   165,   146,    -1,   166,
      -1,   165,   122,   166,    -1,   167,    74,   158,    -1,   168,
     146,    -1,     6,    -1,   168,   122,     6,    -1,    53,    55,
      -1,    53,   155,    55,    -1,    54,   171,    56,    -1,    54,
     171,   125,    56,    -1,   155,    -1,   171,   125,   155,    -1,
      53,   158,   125,   163,    55,    -1,    29,   158,    47,   158,
     174,    23,   158,    25,    -1,    -1,   174,    24,   158,    47,
     158,    -1,    89,    -1,    90,    -1,    91,    -1,    92,    -1,
      93,    -1,    94,    -1,    95,    -1,    96,    -1,    97,    -1,
      98,    -1,    99,    -1,   100,    -1,   101,    -1,   102,    -1,
     103,    -1,   104,    -1,   105,    -1,   106,    -1,   108,    -1,
     109,    -1,   110,    -1,   111,    -1,   112,    -1,   113,    -1,
     114,    -1,   117,    -1,   115,    -1,   175,   120,   158,   122,
     158,   121,    -1,   175,   120,   158,   121,    -1,     6,   120,
     121,    -1,   176,    -1,     6,   120,   178,   121,    -1,     6,
     120,   178,   121,   120,   158,   121,    -1,   155,    -1,   155,
      52,   158,    -1,    33,   123,   180,   124,    74,   158,    -1,
      33,   123,   180,   181,   124,    74,   158,    -1,   182,    -1,
     136,    -1,   180,   181,   182,    -1,   180,   181,   136,    -1,
     122,    -1,   118,    -1,   148,   183,    -1,   148,   183,    67,
     158,    -1,    -1,   184,    -1,    88,   159,    -1,   184,    88,
     159,    -1,     6,    -1,    89,    -1,    90,    -1,    91,    -1,
      92,    -1,    93,    -1,    94,    -1,    95,    -1,    96,    -1,
      97,    -1,    98,    -1,    99,    -1,   100,    -1,   101,    -1,
     102,    -1,   103,    -1,   104,    -1,   105,    -1,   106,    -1,
     107,    -1,   108,    -1,   109,    -1,   110,    -1,   111,    -1,
     112,    -1,   113,    -1,   114,    -1,   115,    -1,   117,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   616,   616,   618,   620,   623,   628,   634,   634,   636,
     647,   651,   653,   655,   656,   658,   660,   662,   664,   666,
     670,   693,   698,   706,   712,   716,   721,   726,   733,   737,
     745,   755,   762,   771,   783,   791,   792,   797,   798,   800,
     803,   807,   811,   816,   816,   819,   821,   825,   830,   834,
     836,   840,   841,   847,   856,   859,   865,   873,   880,   887,
     894,   905,   906,   910,   912,   914,   916,   918,   920,   922,
     928,   931,   933,   939,   940,   942,   944,   946,   948,   950,
     952,   954,   956,   958,   960,   962,   964,   966,   968,   974,
     976,   991,   992,   994,   996,   998,  1000,  1002,  1004,  1006,
    1008,  1010,  1012,  1014,  1016,  1018,  1020,  1022,  1024,  1026,
    1028,  1030,  1032,  1034,  1036,  1038,  1040,  1042,  1044,  1046,
    1048,  1050,  1056,  1058,  1065,  1078,  1080,  1082,  1084,  1087,
    1089,  1092,  1094,  1096,  1098,  1100,  1102,  1104,  1105,  1106,
    1107,  1110,  1111,  1114,  1115,  1118,  1119,  1122,  1123,  1124,
    1129,  1133,  1135,  1139,  1145,  1147,  1150,  1153,  1155,  1159,
    1162,  1165,  1167,  1171,  1173,  1177,  1184,  1193,  1198,  1202,
    1208,  1222,  1223,  1227,  1229,  1231,  1233,  1235,  1237,  1239,
    1241,  1243,  1245,  1247,  1249,  1251,  1253,  1255,  1257,  1259,
    1261,  1263,  1265,  1267,  1269,  1271,  1273,  1275,  1277,  1279,
    1283,  1291,  1323,  1325,  1326,  1337,  1376,  1381,  1388,  1390,
    1410,  1412,  1418,  1420,  1427,  1427,  1430,  1436,  1447,  1448,
    1451,  1455,  1459,  1461,  1463,  1465,  1467,  1469,  1471,  1473,
    1475,  1477,  1479,  1481,  1483,  1485,  1487,  1489,  1491,  1493,
    1495,  1497,  1499,  1501,  1503,  1505,  1507,  1509,  1511,  1513,
    1515
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
  "MZN_DOC_COMMENT", "MZN_VAR", "MZN_PAR", "MZN_SVAR", "MZN_ABSENT",
  "MZN_ANN", "MZN_ANNOTATION", "MZN_ANY", "MZN_ARRAY", "MZN_BOOL",
  "MZN_CASE", "MZN_CONSTRAINT", "MZN_DEFAULT", "MZN_ELSE", "MZN_ELSEIF",
  "MZN_ENDIF", "MZN_ENUM", "MZN_FLOAT", "MZN_FUNCTION", "MZN_IF",
  "MZN_INCLUDE", "MZN_INFINITY", "MZN_INT", "MZN_LET", "MZN_LIST",
  "MZN_MAXIMIZE", "MZN_MINIMIZE", "MZN_OF", "MZN_OPT", "MZN_SATISFY",
  "MZN_OUTPUT", "MZN_PREDICATE", "MZN_RECORD", "MZN_SET", "MZN_SOLVE",
  "MZN_STRING", "MZN_TEST", "MZN_THEN", "MZN_TUPLE", "MZN_TYPE",
  "MZN_UNDERSCORE", "MZN_VARIANT_RECORD", "MZN_WHERE", "MZN_LEFT_BRACKET",
  "MZN_LEFT_2D_BRACKET", "MZN_RIGHT_BRACKET", "MZN_RIGHT_2D_BRACKET",
  "UNKNOWN_CHAR", "FLATZINC_IDENTIFIER", "PREC_ANNO", "MZN_EQUIV",
  "MZN_RIMPL", "MZN_IMPL", "MZN_XOR", "MZN_OR", "MZN_AND", "MZN_NQ",
  "MZN_EQ", "MZN_GQ", "MZN_LQ", "MZN_GR", "MZN_LE", "MZN_SUPERSET",
  "MZN_SUBSET", "MZN_IN", "MZN_SYMDIFF", "MZN_DIFF", "MZN_UNION",
  "MZN_DOTDOT", "MZN_MINUS", "MZN_PLUS", "MZN_INTERSECT", "MZN_MOD",
  "MZN_IDIV", "MZN_DIV", "MZN_MULT", "MZN_NOT", "MZN_PLUSPLUS",
  "MZN_COLONCOLON", "MZN_EQUIV_QUOTED", "MZN_IMPL_QUOTED",
  "MZN_RIMPL_QUOTED", "MZN_OR_QUOTED", "MZN_XOR_QUOTED", "MZN_AND_QUOTED",
  "MZN_LE_QUOTED", "MZN_GR_QUOTED", "MZN_LQ_QUOTED", "MZN_GQ_QUOTED",
  "MZN_EQ_QUOTED", "MZN_NQ_QUOTED", "MZN_IN_QUOTED", "MZN_SUBSET_QUOTED",
  "MZN_SUPERSET_QUOTED", "MZN_UNION_QUOTED", "MZN_DIFF_QUOTED",
  "MZN_SYMDIFF_QUOTED", "MZN_DOTDOT_QUOTED", "MZN_PLUS_QUOTED",
  "MZN_MINUS_QUOTED", "MZN_MULT_QUOTED", "MZN_DIV_QUOTED",
  "MZN_IDIV_QUOTED", "MZN_MOD_QUOTED", "MZN_INTERSECT_QUOTED",
  "MZN_NOT_QUOTED", "MZN_COLONCOLON_QUOTED", "MZN_PLUSPLUS_QUOTED", "';'",
  "':'", "'('", "')'", "','", "'{'", "'}'", "'|'", "$accept", "model",
  "item_list", "item_list_head", "semi_or_none", "item", "item_tail",
  "include_item", "vardecl_item", "assign_item", "constraint_item",
  "solve_item", "output_item", "predicate_item", "function_item",
  "annotation_item", "operation_item_tail", "params", "params_list",
  "params_list_head", "comma_or_none", "ti_expr_and_id_or_anon",
  "ti_expr_and_id", "ti_expr_list", "ti_expr_list_head", "ti_expr",
  "base_ti_expr", "opt_opt", "base_ti_expr_tail", "expr_list",
  "expr_list_head", "set_expr", "expr", "expr_atom_head",
  "array_access_tail", "set_literal", "set_comp", "comp_tail",
  "generator_list", "generator_list_head", "generator", "id_list",
  "id_list_head", "simple_array_literal", "simple_array_literal_2d",
  "simple_array_literal_2d_list", "simple_array_comp", "if_then_else_expr",
  "elseif_list", "quoted_op", "quoted_op_call", "call_expr",
  "comp_or_expr", "let_expr", "let_vardecl_item_list", "comma_or_semi",
  "let_vardecl_item", "annotations", "ne_annotations", "id_or_quoted_op", 0
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
     365,   366,   367,   368,   369,   370,   371,   372,    59,    58,
      40,    41,    44,   123,   125,   124
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   126,   127,   128,   128,   129,   129,   130,   130,   131,
     131,   132,   132,   132,   132,   132,   132,   132,   132,   132,
     133,   134,   134,   135,   136,   137,   137,   137,   138,   139,
     139,   140,   140,   141,   141,   142,   142,   143,   143,   143,
     144,   145,   145,   146,   146,   147,   147,   148,   149,   150,
     150,   151,   151,   151,   152,   152,   152,   152,   152,   152,
     152,   153,   153,   154,   154,   154,   154,   154,   154,   154,
     155,   156,   156,   157,   157,   157,   157,   157,   157,   157,
     157,   157,   157,   157,   157,   157,   157,   157,   157,   157,
     157,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     160,   161,   161,   162,   163,   163,   164,   165,   165,   166,
     167,   168,   168,   169,   169,   170,   170,   171,   171,   172,
     173,   174,   174,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     176,   176,   177,   177,   177,   177,   178,   178,   179,   179,
     180,   180,   180,   180,   181,   181,   182,   182,   183,   183,
     184,   184,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     3,     0,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     4,     3,     2,     3,     4,     4,     2,     5,
       5,     7,     8,     3,     5,     0,     2,     0,     3,     3,
       2,     1,     3,     0,     1,     1,     1,     3,     2,     1,
       3,     1,     6,     3,     1,     2,     3,     3,     4,     5,
       5,     0,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     1,     3,     1,     3,     3,     3,     3,     3,     6,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     6,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     2,     3,     4,     1,     2,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     1,     2,     1,     2,     1,     2,     1,     1,     2,
       3,     2,     3,     5,     1,     3,     2,     1,     3,     3,
       2,     1,     3,     2,     3,     3,     4,     1,     3,     5,
       8,     0,     5,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       6,     4,     3,     1,     4,     7,     1,     3,     6,     7,
       1,     1,     3,     3,     1,     1,     2,     4,     0,     1,
       2,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,   132,   131,   134,   127,   135,    69,    61,    61,    61,
     136,    67,     0,     0,    64,     0,    65,    61,     0,     0,
     133,    63,     0,     0,    62,     0,     0,   218,    66,     0,
     129,     0,     0,     0,     0,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,     0,   191,   192,   193,   194,   195,   196,
     197,   199,   198,     0,     0,     0,     2,     7,     5,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,   218,
       0,    51,     0,    54,    68,    73,   137,   138,   139,   141,
     143,   145,     0,   203,   148,   147,     0,     0,     0,   128,
       9,    62,     0,     0,    37,    61,   127,     0,     0,     0,
       0,    24,    91,     0,     0,    20,    61,    61,    55,    28,
      37,     0,     0,   219,    37,   130,   163,     0,    43,    71,
     167,    71,     0,    90,    89,     0,     0,   151,     0,    71,
       1,     8,     4,    21,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     140,   142,   144,   146,     0,   149,     0,    23,   202,   206,
       0,     0,    57,     0,    56,     0,    33,     0,    43,    49,
     124,   123,   122,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   211,   218,     0,     0,   210,
      53,   218,   220,     0,     0,    25,     0,   218,   164,    44,
      70,     0,   165,     0,     0,   125,   152,     0,     6,     0,
      47,     0,    88,    77,    76,    75,    78,    83,    82,    80,
      87,    86,    85,    84,    81,    74,     0,   150,     0,   204,
       0,     0,     0,     0,    43,    41,    45,    46,     0,     0,
      44,    48,     0,   121,    93,    95,    94,    97,    96,    98,
     104,   103,   102,   101,   100,    99,   107,   106,   105,   110,
     109,   108,   111,   116,   115,   113,   120,   119,   118,   117,
     114,    92,   222,   223,   224,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,   242,   243,   244,   245,   246,   247,   248,   249,
     250,    37,   171,   216,     0,   215,   214,     0,    61,    35,
      27,    26,   221,    35,    72,   161,     0,   154,    43,   157,
       0,    43,   166,   168,     0,   126,     0,    22,    61,    58,
     201,     0,   207,     0,    60,    59,    39,    38,    44,    40,
      34,    61,    50,     0,   218,     0,     0,    47,     0,     0,
     213,   212,     0,    29,    30,   169,     0,    44,   156,     0,
      44,   160,     0,   153,     0,     0,     0,    42,    52,     0,
      35,     0,     0,   217,   208,     0,    36,   155,   158,   159,
     162,    79,   218,   200,   205,   112,    31,     0,     0,   209,
      35,   170,     0,    32,   172
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    65,    66,    67,   142,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,   383,   176,   263,   264,
     230,   265,    79,   177,   178,    80,    81,    82,    83,   127,
     128,    84,   131,   112,    99,    86,    87,   346,   347,   348,
     349,   350,   351,    88,    89,   132,    90,    91,   375,    92,
      93,    94,   170,    95,   218,   338,   219,   122,   123,   331
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -326
static const yytype_int16 yypact[] =
{
     770,  -326,  -326,  -326,   -44,  -326,  -326,   891,   -25,   -25,
    -326,  -326,    26,   -20,  -326,  2222,  -326,  1254,  2222,    30,
    -326,  -326,   -88,     0,  1738,  2222,    33,   -47,  -326,    34,
     -11,  1859,  2222,  2343,  2343,  -326,  -326,  -326,  -326,  -326,
    -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,
    -326,  -326,  -326,   -76,  -326,  -326,  -326,  -326,  -326,  -326,
    -326,  -326,  -326,  2222,   648,    43,  -326,   -73,  -326,  -326,
    -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,   -47,
     -72,  -326,     7,  -326,    27,  -326,  -326,  -326,   -11,   -11,
     -11,   -11,   -69,  -326,   -11,  -326,  2222,  2222,  1980,  -326,
    -326,  -326,  1496,  1617,   -68,  1254,   -34,  2222,  2222,  2222,
     -67,  3199,  -326,   -65,  3075,  -326,  1012,  1375,  -326,  3199,
     -68,  2464,   -14,   -33,   -68,  -326,  -326,     2,   -64,  2528,
    -326,  3199,   -48,    10,    10,  2222,  2780,  -326,   -63,  2564,
    -326,   770,  -326,    -8,    54,    25,  2343,  2343,  2343,  2343,
    2343,  2343,  2343,  2343,  2343,  2343,  2343,  2343,  2343,  2464,
    -326,  -326,  -326,  -326,  2222,  -326,     8,  3199,  -326,    17,
     -51,    35,  -326,    36,  -326,   405,    12,    19,   -41,  -326,
      29,    29,    29,  2222,  2222,  2222,  2222,  2222,  2222,  2222,
    2222,  2222,  2222,  2222,  2222,  2222,  2222,  2222,  2222,  2222,
    2222,  2222,  2222,  2222,  2222,  2222,  2222,  2222,  2222,  2222,
    2222,  2222,  2464,  3001,  2222,  -326,   -47,   -36,   -98,  -326,
    -326,   -47,  -326,  2222,  2222,  -326,  2464,   -47,  -326,  2222,
    -326,    78,  -326,  2101,  2672,   -11,  -326,    78,  -326,  2222,
     -35,  1738,    -1,    49,    49,    49,   604,   244,   244,    10,
      10,    10,    10,    10,    10,  -326,  2643,  -326,  2222,   -32,
    1738,  1738,   -22,    -2,   -26,  -326,  -326,   -36,  2222,    64,
    1133,  -326,  2751,    38,  3233,  3278,  3278,   233,   233,   516,
     637,   637,   637,   637,   637,   637,   390,   390,   390,   757,
     757,   757,   879,   317,   317,    29,    29,    29,    29,    29,
      29,  -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,
    -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,
    -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,
    -326,   -68,  3199,    55,   115,  -326,  -326,    50,   526,    56,
    3199,  3199,  -326,    56,  3199,  -326,    83,    88,    21,  -326,
      67,    23,  -326,  -326,  2222,  -326,    24,  3199,  1254,  -326,
    -326,  2222,  3199,  2222,  -326,  -326,  -326,  -326,  1133,  -326,
    3199,  1375,  -326,  2222,   -47,     5,  2222,  -326,  2222,    72,
    -326,  -326,  2222,  -326,  -326,  -326,  2222,    78,  -326,  2222,
     141,  -326,  2859,  -326,    32,  2888,  2967,  -326,  -326,  2996,
      56,  2222,  2222,  3199,  3199,  2222,  3199,  3199,  -326,  3199,
    -326,  -326,   -47,  -326,  -326,  -326,  -326,  3154,  3120,  3199,
      56,  -326,  2222,  -326,  3199
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -326,  -326,  -326,  -326,  -326,    13,   148,  -326,  -326,  -326,
    -110,  -326,  -326,  -326,  -326,  -326,  -325,  -109,  -202,  -326,
    -151,  -211,  -111,  -326,  -326,   -16,  -113,    22,   -12,   -18,
    -326,   132,   -15,   118,   -23,  -326,  -326,   -79,  -326,  -326,
    -227,  -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,
    -326,  -326,  -326,  -326,  -326,  -326,  -177,   -77,  -326,  -326
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -62
static const yytype_int16 yytable[] =
{
     111,   113,   143,   114,   220,   216,   215,   125,   232,    96,
     119,   221,   118,   101,   130,   227,   129,   146,   384,    96,
     335,   223,   224,    97,   336,   225,   337,   271,   401,   402,
     102,   103,   104,   105,   146,   116,   184,   117,   115,   120,
     124,   121,    96,   140,   135,   141,   138,   144,   136,   139,
     145,   164,   175,   183,   213,   226,   146,   228,   229,   239,
     240,   236,   241,   257,   266,   160,   161,   162,   163,   258,
     259,   165,   260,   261,   269,   416,    98,   233,   166,   268,
     169,   270,   167,   334,   345,   358,    98,   159,   363,   179,
     172,   174,   180,   181,   182,   423,   368,   158,   159,   366,
     217,   371,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   369,   158,   159,   211,   212,    85,   367,
     234,   377,   376,   382,   378,    85,   212,   150,   151,   152,
     153,   154,   155,   156,   157,    85,   158,   159,   385,   333,
     386,   389,    85,   387,   339,   390,   405,   410,   393,   256,
     343,    85,    85,   412,   238,   100,   394,   397,   356,   267,
     408,   381,     0,     0,     0,   133,   134,     0,   272,   273,
     274,   275,   276,   277,   278,   279,   280,   281,   282,   283,
     284,   285,   286,   287,   288,   289,   290,   291,   292,   293,
     294,   295,   296,   297,   298,   299,   300,   388,     0,   332,
     391,     0,     0,     0,     0,     0,     0,     0,   340,   341,
       0,     0,   355,     0,   344,   353,     0,     0,     0,     0,
      85,    85,   374,    85,   357,     0,     0,   216,   380,   359,
       0,     0,     0,     0,    85,    85,     0,     0,     0,   222,
     184,     0,     0,   362,     0,     0,     0,   266,   364,   365,
       0,   146,     0,   370,   372,     0,     0,   266,   398,    85,
       0,     0,     0,     0,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,   255,   242,   243,
     244,   245,   246,   247,   248,   249,   250,   251,   252,   253,
     254,     0,     0,    85,     0,     0,     0,   400,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,     0,
     211,   212,   217,     0,   184,   153,   154,   155,   156,   157,
     301,   158,   159,     0,     0,   420,     0,     0,     0,   392,
       0,     0,   267,     0,   342,     0,   395,     0,   396,     0,
       0,     0,   267,     0,     0,     0,     0,     0,   399,    85,
       0,   403,     0,   404,     0,     0,     0,   406,     0,     0,
       0,   407,     0,     0,   409,     0,     0,     0,    85,    85,
       0,     0,     0,     0,     0,     0,   417,   418,    85,     0,
     419,     0,     0,     0,     0,     0,     0,   184,   206,   207,
     208,   209,   210,     0,   211,   212,   262,   424,     1,     2,
       3,   106,     0,     5,     6,     0,     8,     9,     0,    10,
      11,     0,     0,    13,    14,     0,     0,     0,     0,     0,
       0,     0,    16,     0,    18,     0,    20,    21,    22,    23,
       0,     0,     0,    24,     0,     0,     0,     0,   -61,     0,
      28,     0,     0,     0,     0,    30,    85,     0,    31,    32,
       0,     0,   -62,   -62,   -62,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,    85,   211,   212,     0,
       0,     0,     0,     0,    33,    34,    85,     0,     0,    85,
       0,     0,     0,     0,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,     0,    62,   184,     0,    63,     0,     0,    64,     1,
       2,     3,   106,     0,     5,     6,     0,     8,     9,     0,
      10,    11,     0,     0,    13,    14,     0,    15,     0,     0,
       0,     0,     0,    16,     0,    18,     0,    20,    21,    22,
      23,     0,     0,     0,    24,     0,     0,     0,     0,     0,
       0,    28,     0,     0,     0,     0,    30,     0,     0,    31,
      32,     0,   191,   192,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,     0,   211,   212,    33,    34,     0,     0,     0,
       0,   146,     0,     0,     0,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,     0,    62,   184,     0,    63,     0,     0,    64,
     379,     1,     2,     3,   106,     0,     5,     0,     0,     0,
       0,     0,    10,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    18,     0,    20,
       0,    22,   -62,   151,   152,   153,   154,   155,   156,   157,
       0,   158,   159,     0,     0,     0,     0,     0,    30,     0,
       0,    31,    32,   -62,   -62,   -62,   -62,   -62,   -62,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,     0,   211,   212,     0,   107,   108,     0,
       0,     0,     0,     0,   109,     0,     0,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,   110,    54,    55,    56,    57,
      58,    59,    60,    61,   184,    62,     0,     0,    63,     0,
       0,    64,   137,     1,     2,     3,     4,     0,     5,     6,
       7,     8,     9,     0,    10,    11,    12,     0,    13,    14,
       0,    15,     0,     0,     0,     0,     0,    16,    17,    18,
      19,    20,    21,    22,    23,     0,     0,     0,    24,     0,
      25,    26,     0,   -61,    27,    28,    29,     0,     0,     0,
      30,     0,     0,    31,    32,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   203,   204,   205,   206,   207,
     208,   209,   210,     0,   211,   212,     0,     0,     0,    33,
      34,     0,     0,     0,     0,     0,     0,     0,     0,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,   184,    62,     0,     0,
      63,     0,     0,    64,     1,     2,     3,     4,     0,     5,
       6,     0,     8,     9,     0,    10,    11,    12,     0,    13,
      14,     0,    15,     0,     0,     0,     0,     0,    16,    17,
      18,    19,    20,    21,    22,    23,     0,     0,     0,    24,
       0,    25,    26,     0,     0,    27,    28,    29,     0,     0,
       0,    30,     0,     0,    31,    32,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   -62,   204,   205,
     206,   207,   208,   209,   210,     0,   211,   212,     0,     0,
      33,    34,     0,     0,     0,     0,     0,     0,     0,     0,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,     0,    62,     0,
       0,    63,     0,     0,    64,     1,     2,     3,   106,     0,
       5,     6,     0,     8,     9,     0,    10,    11,     0,     0,
      13,    14,     0,    15,     0,     0,     0,     0,     0,    16,
       0,    18,     0,    20,    21,    22,    23,     0,     0,     0,
      24,     0,     0,     0,     0,     0,     0,    28,     0,     0,
       0,     0,    30,     0,     0,    31,    32,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    33,    34,     0,     0,     0,     0,     0,     0,     0,
       0,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,     0,    62,
       0,     0,    63,     0,     0,    64,     1,     2,     3,   106,
       0,     5,     6,     0,     8,     9,     0,    10,    11,     0,
       0,    13,    14,     0,     0,     0,     0,     0,     0,     0,
      16,     0,    18,     0,    20,    21,    22,    23,     0,     0,
       0,    24,     0,     0,     0,     0,   -61,     0,    28,     0,
       0,     0,     0,    30,     0,     0,    31,    32,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    33,    34,     0,     0,     0,     0,     0,     0,
       0,     0,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,     0,
      62,     0,     0,    63,     0,     0,    64,     1,     2,     3,
     106,     0,     5,     6,     0,     8,     9,     0,    10,    11,
       0,     0,    13,    14,     0,     0,     0,     0,     0,     0,
       0,    16,     0,    18,     0,    20,    21,    22,    23,     0,
       0,     0,    24,     0,     0,     0,     0,     0,     0,    28,
       0,     0,     0,     0,    30,     0,     0,    31,    32,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    33,    34,     0,     0,     0,     0,     0,
       0,     0,     0,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
       0,    62,     0,     0,    63,     0,     0,    64,     1,     2,
       3,   106,     0,     5,     6,     0,     8,     9,     0,    10,
      11,     0,     0,     0,    14,     0,     0,     0,     0,     0,
       0,     0,    16,     0,    18,     0,    20,    21,    22,     0,
       0,     0,     0,    24,     0,     0,     0,     0,     0,     0,
      28,     0,     0,     0,     0,    30,     0,     0,    31,    32,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    33,    34,     0,     0,     0,     0,
       0,     0,     0,     0,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,     0,    62,     0,     0,    63,     0,     0,    64,     1,
       2,     3,   106,     0,     5,     6,     0,     0,     0,     0,
      10,    11,     0,     0,     0,    14,     0,     0,     0,     0,
       0,     0,     0,    16,     0,    18,     0,    20,    21,    22,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   171,
       0,    28,     0,     0,     0,     0,    30,     0,     0,    31,
      32,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    33,    34,     0,     0,     0,
       0,     0,     0,     0,     0,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,     0,    62,     0,     0,    63,     0,     0,    64,
       1,     2,     3,   106,     0,     5,     6,     0,     0,     0,
       0,    10,    11,     0,     0,     0,    14,     0,     0,     0,
       0,     0,     0,     0,    16,     0,    18,     0,    20,    21,
      22,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     173,     0,    28,     0,     0,     0,     0,    30,     0,     0,
      31,    32,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    33,    34,     0,     0,
       0,     0,     0,     0,     0,     0,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,     0,    62,     0,     0,    63,     0,     0,
      64,     1,     2,     3,   106,     0,     5,     6,     0,     0,
       0,     0,    10,    11,     0,     0,     0,    14,     0,     0,
       0,     0,     0,     0,     0,    16,     0,    18,     0,    20,
      21,    22,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    28,     0,     0,     0,     0,    30,     0,
       0,    31,    32,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    33,    34,     0,
       0,     0,     0,     0,     0,     0,     0,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,     0,    62,     0,     0,    63,     0,
       0,    64,     1,     2,     3,   106,     0,     5,     0,     0,
       0,     0,     0,    10,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    18,     0,
      20,     0,    22,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    30,
       0,     0,    31,    32,   126,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   107,   108,
       0,     0,     0,     0,     0,   109,     0,     0,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,   110,    54,    55,    56,
      57,    58,    59,    60,    61,     0,    62,     0,     0,    63,
       0,     0,    64,     1,     2,     3,   106,     0,     5,     0,
       0,     0,     0,     0,    10,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    18,
       0,    20,     0,    22,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      30,     0,     0,    31,    32,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   107,
     108,     0,     0,     0,     0,     0,   109,     0,     0,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,   110,    54,    55,
      56,    57,    58,    59,    60,    61,     0,    62,     0,     0,
      63,   168,     0,    64,     1,     2,     3,   106,     0,     5,
       0,     0,     0,     0,     0,    10,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      18,     0,    20,     0,    22,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    30,     0,     0,    31,    32,     0,   352,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     107,   108,     0,     0,     0,     0,     0,   109,     0,     0,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,   110,    54,
      55,    56,    57,    58,    59,    60,    61,     0,    62,     0,
       0,    63,     0,     0,    64,     1,     2,     3,   106,     0,
       5,     0,     0,     0,     0,     0,    10,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    18,     0,    20,     0,    22,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    30,     0,     0,    31,    32,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   107,   108,     0,     0,     0,     0,     0,   109,     0,
       0,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,   110,
      54,    55,    56,    57,    58,    59,    60,    61,     0,    62,
       0,     0,    63,     0,     0,    64,     1,     2,     3,   106,
       0,     5,     0,     0,     0,     0,     0,    10,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    18,     0,    20,     0,    22,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    30,     0,     0,    31,    32,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    33,    34,     0,     0,     0,     0,     0,     0,
       0,     0,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,     0,
      62,     0,     0,    63,     0,     0,    64,     1,     2,     3,
     106,     0,     5,     0,     0,     0,     0,     0,    10,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    18,     0,    20,     0,    22,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    30,     0,     0,    31,    32,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   184,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,   184,    54,    55,    56,    57,    58,    59,    60,    61,
       0,    62,     0,     0,    63,     0,     0,    64,   185,   186,
     187,   188,   189,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,     0,   211,   212,     0,     0,     0,
       0,     0,     0,     0,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     184,   211,   212,   231,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   184,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   237,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   185,   186,   187,   188,   189,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,     0,
     211,   212,   185,   186,   187,   188,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   184,   211,
     212,     0,     0,     0,   360,   361,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   184,     0,     0,
       0,     0,     0,     0,   354,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   185,   186,   187,   188,   189,   190,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,     0,   211,   212,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   184,   211,   212,     0,
       0,     0,     0,   373,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   184,     0,     0,     0,     0,
       0,   235,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,     0,   211,   212,   185,   186,
     187,   188,   189,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   184,   211,   212,     0,     0,     0,
     411,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   184,     0,     0,     0,   302,     0,   413,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,     0,   211,   212,   185,   186,   187,   188,
     189,   190,   191,   192,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   184,   211,   212,     0,     0,     0,   414,     0,
     303,   304,   305,   306,   307,   308,   309,   310,   311,   312,
     313,   314,   315,   316,   317,   318,   319,   320,   321,   322,
     323,   324,   325,   326,   327,   328,   329,   415,   330,     0,
       0,     0,   214,     0,     0,     0,     0,   184,     0,     0,
       0,     0,     0,     0,     0,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   184,   211,   212,     0,     0,     0,   422,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   421,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   184,   211,   212,     0,
       0,     0,     0,     0,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     184,   211,   212,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   184,   211,   212,     0,     0,
       0,     0,     0,     0,   186,   187,   188,   189,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,     0,
     211,   212,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   188,   189,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,     0,   211,   212
};

static const yytype_int16 yycheck[] =
{
      15,    17,    79,    18,   117,   116,   116,    30,    56,    53,
      25,   120,    24,    38,    32,   124,    31,     7,   343,    53,
     118,    35,    36,    67,   122,    39,   124,   178,    23,    24,
       8,     9,     6,    53,     7,   123,     7,    37,     8,     6,
       6,    88,    53,     0,   120,   118,    64,   119,    63,    64,
      43,   120,   120,   120,   119,    88,     7,    55,   122,    67,
       6,   124,    37,    55,   175,    88,    89,    90,    91,    52,
     121,    94,    37,    37,    55,   400,   120,   125,    96,    67,
      98,   122,    97,   119,     6,   120,   120,    88,   120,   105,
     102,   103,   107,   108,   109,   420,   122,    87,    88,   121,
     116,    37,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,   264,    87,    88,    87,    88,     0,   121,
     135,     6,    67,    67,    74,     7,    88,    78,    79,    80,
      81,    82,    83,    84,    85,    17,    87,    88,    55,   216,
      52,    74,    24,   122,   221,   122,    74,     6,   124,   164,
     227,    33,    34,   121,   141,     7,   358,   368,   237,   175,
     387,   338,    -1,    -1,    -1,    33,    34,    -1,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   348,    -1,   214,
     351,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   223,   224,
      -1,    -1,   235,    -1,   229,   233,    -1,    -1,    -1,    -1,
     102,   103,   331,   105,   239,    -1,    -1,   338,   338,   241,
      -1,    -1,    -1,    -1,   116,   117,    -1,    -1,    -1,   121,
       7,    -1,    -1,   258,    -1,    -1,    -1,   358,   260,   261,
      -1,     7,    -1,   268,   270,    -1,    -1,   368,   371,   141,
      -1,    -1,    -1,    -1,   146,   147,   148,   149,   150,   151,
     152,   153,   154,   155,   156,   157,   158,   159,   146,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
     158,    -1,    -1,   175,    -1,    -1,    -1,   374,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    -1,
      87,    88,   338,    -1,     7,    81,    82,    83,    84,    85,
     212,    87,    88,    -1,    -1,   412,    -1,    -1,    -1,   354,
      -1,    -1,   358,    -1,   226,    -1,   361,    -1,   363,    -1,
      -1,    -1,   368,    -1,    -1,    -1,    -1,    -1,   373,   241,
      -1,   376,    -1,   378,    -1,    -1,    -1,   382,    -1,    -1,
      -1,   386,    -1,    -1,   389,    -1,    -1,    -1,   260,   261,
      -1,    -1,    -1,    -1,    -1,    -1,   401,   402,   270,    -1,
     405,    -1,    -1,    -1,    -1,    -1,    -1,     7,    81,    82,
      83,    84,    85,    -1,    87,    88,     1,   422,     3,     4,
       5,     6,    -1,     8,     9,    -1,    11,    12,    -1,    14,
      15,    -1,    -1,    18,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    27,    -1,    29,    -1,    31,    32,    33,    34,
      -1,    -1,    -1,    38,    -1,    -1,    -1,    -1,    43,    -1,
      45,    -1,    -1,    -1,    -1,    50,   338,    -1,    53,    54,
      -1,    -1,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,   358,    87,    88,    -1,
      -1,    -1,    -1,    -1,    79,    80,   368,    -1,    -1,   371,
      -1,    -1,    -1,    -1,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,    -1,   117,     7,    -1,   120,    -1,    -1,   123,     3,
       4,     5,     6,    -1,     8,     9,    -1,    11,    12,    -1,
      14,    15,    -1,    -1,    18,    19,    -1,    21,    -1,    -1,
      -1,    -1,    -1,    27,    -1,    29,    -1,    31,    32,    33,
      34,    -1,    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    -1,    -1,    50,    -1,    -1,    53,
      54,    -1,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    -1,    87,    88,    79,    80,    -1,    -1,    -1,
      -1,     7,    -1,    -1,    -1,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,    -1,   117,     7,    -1,   120,    -1,    -1,   123,
     124,     3,     4,     5,     6,    -1,     8,    -1,    -1,    -1,
      -1,    -1,    14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,    -1,    31,
      -1,    33,    78,    79,    80,    81,    82,    83,    84,    85,
      -1,    87,    88,    -1,    -1,    -1,    -1,    -1,    50,    -1,
      -1,    53,    54,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    -1,    87,    88,    -1,    79,    80,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,     7,   117,    -1,    -1,   120,    -1,
      -1,   123,   124,     3,     4,     5,     6,    -1,     8,     9,
      10,    11,    12,    -1,    14,    15,    16,    -1,    18,    19,
      -1,    21,    -1,    -1,    -1,    -1,    -1,    27,    28,    29,
      30,    31,    32,    33,    34,    -1,    -1,    -1,    38,    -1,
      40,    41,    -1,    43,    44,    45,    46,    -1,    -1,    -1,
      50,    -1,    -1,    53,    54,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    78,    79,    80,    81,    82,
      83,    84,    85,    -1,    87,    88,    -1,    -1,    -1,    79,
      80,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,     7,   117,    -1,    -1,
     120,    -1,    -1,   123,     3,     4,     5,     6,    -1,     8,
       9,    -1,    11,    12,    -1,    14,    15,    16,    -1,    18,
      19,    -1,    21,    -1,    -1,    -1,    -1,    -1,    27,    28,
      29,    30,    31,    32,    33,    34,    -1,    -1,    -1,    38,
      -1,    40,    41,    -1,    -1,    44,    45,    46,    -1,    -1,
      -1,    50,    -1,    -1,    53,    54,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    79,    80,
      81,    82,    83,    84,    85,    -1,    87,    88,    -1,    -1,
      79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,    -1,   117,    -1,
      -1,   120,    -1,    -1,   123,     3,     4,     5,     6,    -1,
       8,     9,    -1,    11,    12,    -1,    14,    15,    -1,    -1,
      18,    19,    -1,    21,    -1,    -1,    -1,    -1,    -1,    27,
      -1,    29,    -1,    31,    32,    33,    34,    -1,    -1,    -1,
      38,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
      -1,    -1,    50,    -1,    -1,    53,    54,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,    -1,   117,
      -1,    -1,   120,    -1,    -1,   123,     3,     4,     5,     6,
      -1,     8,     9,    -1,    11,    12,    -1,    14,    15,    -1,
      -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      27,    -1,    29,    -1,    31,    32,    33,    34,    -1,    -1,
      -1,    38,    -1,    -1,    -1,    -1,    43,    -1,    45,    -1,
      -1,    -1,    -1,    50,    -1,    -1,    53,    54,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,    -1,
     117,    -1,    -1,   120,    -1,    -1,   123,     3,     4,     5,
       6,    -1,     8,     9,    -1,    11,    12,    -1,    14,    15,
      -1,    -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    27,    -1,    29,    -1,    31,    32,    33,    34,    -1,
      -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    -1,    -1,    -1,    50,    -1,    -1,    53,    54,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    79,    80,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
      -1,   117,    -1,    -1,   120,    -1,    -1,   123,     3,     4,
       5,     6,    -1,     8,     9,    -1,    11,    12,    -1,    14,
      15,    -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    27,    -1,    29,    -1,    31,    32,    33,    -1,
      -1,    -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    -1,    -1,    -1,    50,    -1,    -1,    53,    54,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    79,    80,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,    -1,   117,    -1,    -1,   120,    -1,    -1,   123,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,
      14,    15,    -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    27,    -1,    29,    -1,    31,    32,    33,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,
      -1,    45,    -1,    -1,    -1,    -1,    50,    -1,    -1,    53,
      54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    79,    80,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,    -1,   117,    -1,    -1,   120,    -1,    -1,   123,
       3,     4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,
      -1,    14,    15,    -1,    -1,    -1,    19,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    27,    -1,    29,    -1,    31,    32,
      33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      43,    -1,    45,    -1,    -1,    -1,    -1,    50,    -1,    -1,
      53,    54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    79,    80,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,    -1,   117,    -1,    -1,   120,    -1,    -1,
     123,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      -1,    -1,    14,    15,    -1,    -1,    -1,    19,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    27,    -1,    29,    -1,    31,
      32,    33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    -1,    -1,    -1,    50,    -1,
      -1,    53,    54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    79,    80,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,    -1,   117,    -1,    -1,   120,    -1,
      -1,   123,     3,     4,     5,     6,    -1,     8,    -1,    -1,
      -1,    -1,    -1,    14,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,    -1,
      31,    -1,    33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,
      -1,    -1,    53,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    79,    80,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,    -1,   117,    -1,    -1,   120,
      -1,    -1,   123,     3,     4,     5,     6,    -1,     8,    -1,
      -1,    -1,    -1,    -1,    14,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,
      -1,    31,    -1,    33,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      50,    -1,    -1,    53,    54,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    79,
      80,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,    -1,   117,    -1,    -1,
     120,   121,    -1,   123,     3,     4,     5,     6,    -1,     8,
      -1,    -1,    -1,    -1,    -1,    14,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      29,    -1,    31,    -1,    33,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    50,    -1,    -1,    53,    54,    -1,    56,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      79,    80,    -1,    -1,    -1,    -1,    -1,    86,    -1,    -1,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,    -1,   117,    -1,
      -1,   120,    -1,    -1,   123,     3,     4,     5,     6,    -1,
       8,    -1,    -1,    -1,    -1,    -1,    14,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    29,    -1,    31,    -1,    33,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    50,    -1,    -1,    53,    54,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    79,    80,    -1,    -1,    -1,    -1,    -1,    86,    -1,
      -1,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,    -1,   117,
      -1,    -1,   120,    -1,    -1,   123,     3,     4,     5,     6,
      -1,     8,    -1,    -1,    -1,    -1,    -1,    14,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    29,    -1,    31,    -1,    33,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    50,    -1,    -1,    53,    54,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,    -1,
     117,    -1,    -1,   120,    -1,    -1,   123,     3,     4,     5,
       6,    -1,     8,    -1,    -1,    -1,    -1,    -1,    14,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    29,    -1,    31,    -1,    33,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    50,    -1,    -1,    53,    54,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,     7,   108,   109,   110,   111,   112,   113,   114,   115,
      -1,   117,    -1,    -1,   120,    -1,    -1,   123,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    -1,    87,    88,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
       7,    87,    88,   125,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   125,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    -1,
      87,    88,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,     7,    87,
      88,    -1,    -1,    -1,   121,   122,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,    -1,
      -1,    -1,    -1,    -1,   122,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    -1,    87,    88,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,     7,    87,    88,    -1,
      -1,    -1,    -1,   122,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,
      -1,   121,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    -1,    87,    88,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,     7,    87,    88,    -1,    -1,    -1,
     121,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     7,    -1,    -1,    -1,     6,    -1,   121,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    -1,    87,    88,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,     7,    87,    88,    -1,    -1,    -1,   121,    -1,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   121,   117,    -1,
      -1,    -1,    47,    -1,    -1,    -1,    -1,     7,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,     7,    87,    88,    -1,    -1,    -1,    47,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    25,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,     7,    87,    88,    -1,
      -1,    -1,    -1,    -1,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
       7,    87,    88,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,     7,    87,    88,    -1,    -1,
      -1,    -1,    -1,    -1,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    -1,
      87,    88,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    -1,    87,    88
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     8,     9,    10,    11,    12,
      14,    15,    16,    18,    19,    21,    27,    28,    29,    30,
      31,    32,    33,    34,    38,    40,    41,    44,    45,    46,
      50,    53,    54,    79,    80,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   117,   120,   123,   127,   128,   129,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   148,
     151,   152,   153,   154,   157,   159,   161,   162,   169,   170,
     172,   173,   175,   176,   177,   179,    53,    67,   120,   160,
     132,    38,   153,   153,     6,    53,     6,    79,    80,    86,
     107,   158,   159,   151,   158,     8,   123,    37,   154,   158,
       6,    88,   183,   184,     6,   160,    55,   155,   156,   158,
     155,   158,   171,   157,   157,   120,   158,   124,   155,   158,
       0,   118,   130,   183,   119,    43,     7,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    87,    88,
     160,   160,   160,   160,   120,   160,   155,   158,   121,   155,
     178,    43,   154,    43,   154,   120,   143,   149,   150,   151,
     158,   158,   158,   120,     7,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    87,    88,   119,    47,   136,   148,   151,   180,   182,
     152,   143,   159,    35,    36,    39,    88,   143,    55,   122,
     146,   125,    56,   125,   158,   121,   124,   125,   131,    67,
       6,    37,   157,   157,   157,   157,   157,   157,   157,   157,
     157,   157,   157,   157,   157,   159,   158,    55,    52,   121,
      37,    37,     1,   144,   145,   147,   148,   151,    67,    55,
     122,   146,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   159,     6,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     117,   185,   158,   183,   119,   118,   122,   124,   181,   183,
     158,   158,   159,   183,   158,     6,   163,   164,   165,   166,
     167,   168,    56,   155,   122,   160,   163,   158,   120,   154,
     121,   122,   158,   120,   154,   154,   121,   121,   122,   146,
     158,    37,   151,   122,   143,   174,    67,     6,    74,   124,
     136,   182,    67,   142,   142,    55,    52,   122,   146,    74,
     122,   146,   158,   124,   144,   158,   158,   147,   152,   158,
     183,    23,    24,   158,   158,    74,   158,   158,   166,   158,
       6,   121,   121,   121,   121,   121,   142,   158,   158,   158,
     183,    25,    47,   142,   158
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
# define YYLEX yylex (&yylval, &yylloc, YYLEX_PARAM)
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
  yylloc.filename = ASTString(static_cast<ParserState*>(parm)->filename);
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
        pp->model->addItem((yyvsp[(1) - (1)].item));
      ;}
    break;

  case 6:

    {
        ParserState* pp = static_cast<ParserState*>(parm);
        pp->model->addItem((yyvsp[(3) - (3)].item));
      ;}
    break;

  case 9:

    { (yyval.item) = (yyvsp[(2) - (2)].item);
        if (FunctionI* fi = (yyval.item)->dyn_cast<FunctionI>()) {
          fi->ann().add(createDocComment((yylsp[(1) - (2)]),(yyvsp[(1) - (2)].sValue)));
        } else if (VarDeclI* vdi = (yyval.item)->dyn_cast<VarDeclI>()) {
          vdi->e()->addAnnotation(createDocComment((yylsp[(1) - (2)]),(yyvsp[(1) - (2)].sValue)));
        } else {
          yyerror(&(yylsp[(2) - (2)]), parm, "documentation comments are only supported for function, predicate and variable declarations");
        }
        free((yyvsp[(1) - (2)].sValue));
      ;}
    break;

  case 10:

    { (yyval.item) = (yyvsp[(1) - (1)].item); ;}
    break;

  case 11:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"include") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 12:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"variable declaration") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 14:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"constraint") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 15:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"solve") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 16:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"output") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 17:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"predicate") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 18:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"predicate") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 19:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"annotation") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 20:

    { ParserState* pp = static_cast<ParserState*>(parm);
        map<string,Model*>::iterator ret = pp->seenModels.find((yyvsp[(2) - (2)].sValue));
        IncludeI* ii = new IncludeI((yyloc),ASTString((yyvsp[(2) - (2)].sValue)));
        (yyval.item) = ii;
        if (ret == pp->seenModels.end()) {
          Model* im = new Model;
          im->setParent(pp->model);
          im->setFilename((yyvsp[(2) - (2)].sValue));
          string fpath, fbase; filepath(pp->filename, fpath, fbase);
          if (fpath=="")
            fpath="./";
          pair<string,Model*> pm(fpath, im);
          pp->files.push_back(pm);
          ii->m(im);
          pp->seenModels.insert(pair<string,Model*>((yyvsp[(2) - (2)].sValue),im));
        } else {
          ii->m(ret->second, false);
        }
        free((yyvsp[(2) - (2)].sValue));
      ;}
    break;

  case 21:

    { if ((yyvsp[(2) - (2)].expression_v)) (yyvsp[(1) - (2)].vardeclexpr)->addAnnotations(*(yyvsp[(2) - (2)].expression_v));
        (yyval.item) = new VarDeclI((yyloc),(yyvsp[(1) - (2)].vardeclexpr));
        delete (yyvsp[(2) - (2)].expression_v);
      ;}
    break;

  case 22:

    { (yyvsp[(1) - (4)].vardeclexpr)->e((yyvsp[(4) - (4)].expression));
        if ((yyvsp[(2) - (4)].expression_v)) (yyvsp[(1) - (4)].vardeclexpr)->addAnnotations(*(yyvsp[(2) - (4)].expression_v));
        (yyval.item) = new VarDeclI((yyloc),(yyvsp[(1) - (4)].vardeclexpr));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 23:

    { (yyval.item) = new AssignI((yyloc),(yyvsp[(1) - (3)].sValue),(yyvsp[(3) - (3)].expression));
        free((yyvsp[(1) - (3)].sValue));
      ;}
    break;

  case 24:

    { (yyval.item) = new ConstraintI((yyloc),(yyvsp[(2) - (2)].expression));;}
    break;

  case 25:

    { (yyval.item) = SolveI::sat((yyloc));
        if ((yyvsp[(2) - (3)].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[(2) - (3)].expression_v));
        delete (yyvsp[(2) - (3)].expression_v);
      ;}
    break;

  case 26:

    { (yyval.item) = SolveI::min((yyloc),(yyvsp[(4) - (4)].expression));
        if ((yyvsp[(2) - (4)].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[(2) - (4)].expression_v));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 27:

    { (yyval.item) = SolveI::max((yyloc),(yyvsp[(4) - (4)].expression));
        if ((yyvsp[(2) - (4)].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[(2) - (4)].expression_v));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 28:

    { (yyval.item) = new OutputI((yyloc),(yyvsp[(2) - (2)].expression));;}
    break;

  case 29:

    { (yyval.item) = new FunctionI((yyloc),(yyvsp[(2) - (5)].sValue),new TypeInst((yyloc),
                           Type::varbool()),*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression));
        if ((yyvsp[(4) - (5)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(4) - (5)].expression_v));
        free((yyvsp[(2) - (5)].sValue));
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
        delete (yyvsp[(4) - (5)].expression_v);
      ;}
    break;

  case 30:

    { (yyval.item) = new FunctionI((yyloc),(yyvsp[(2) - (5)].sValue),new TypeInst((yyloc),
                           Type::parbool()),*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression));
        if ((yyvsp[(4) - (5)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(4) - (5)].expression_v));
        free((yyvsp[(2) - (5)].sValue));
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
        delete (yyvsp[(4) - (5)].expression_v);
      ;}
    break;

  case 31:

    { (yyval.item) = new FunctionI((yyloc),(yyvsp[(4) - (7)].sValue),(yyvsp[(2) - (7)].tiexpr),*(yyvsp[(5) - (7)].vardeclexpr_v),(yyvsp[(7) - (7)].expression));
        if ((yyvsp[(6) - (7)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(6) - (7)].expression_v));
        free((yyvsp[(4) - (7)].sValue));
        delete (yyvsp[(5) - (7)].vardeclexpr_v);
        delete (yyvsp[(6) - (7)].expression_v);
      ;}
    break;

  case 32:

    { (yyval.item) = new FunctionI((yyloc),(yyvsp[(3) - (8)].sValue),(yyvsp[(1) - (8)].tiexpr),*(yyvsp[(5) - (8)].vardeclexpr_v),(yyvsp[(8) - (8)].expression));
        if ((yyvsp[(7) - (8)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(7) - (8)].expression_v));
        free((yyvsp[(3) - (8)].sValue));
        delete (yyvsp[(5) - (8)].vardeclexpr_v);
        delete (yyvsp[(7) - (8)].expression_v);
      ;}
    break;

  case 33:

    {
        TypeInst* ti=new TypeInst((yylsp[(1) - (3)]),Type::ann());
        if ((yyvsp[(3) - (3)].vardeclexpr_v)->empty()) {
          VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[(2) - (3)].sValue));
          (yyval.item) = new VarDeclI((yyloc),vd);
        } else {
          (yyval.item) = new FunctionI((yyloc),(yyvsp[(2) - (3)].sValue),ti,*(yyvsp[(3) - (3)].vardeclexpr_v),NULL);
        }
        free((yyvsp[(2) - (3)].sValue));
        delete (yyvsp[(3) - (3)].vardeclexpr_v);
      ;}
    break;

  case 34:

    { TypeInst* ti=new TypeInst((yylsp[(1) - (5)]),Type::ann());
        (yyval.item) = new FunctionI((yyloc),(yyvsp[(2) - (5)].sValue),ti,*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression));
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
      ;}
    break;

  case 35:

    { (yyval.expression)=NULL; ;}
    break;

  case 36:

    { (yyval.expression)=(yyvsp[(2) - (2)].expression); ;}
    break;

  case 37:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); ;}
    break;

  case 38:

    { (yyval.vardeclexpr_v)=(yyvsp[(2) - (3)].vardeclexpr_v); ;}
    break;

  case 39:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); ;}
    break;

  case 40:

    { (yyval.vardeclexpr_v)=(yyvsp[(1) - (2)].vardeclexpr_v); ;}
    break;

  case 41:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>();
        (yyvsp[(1) - (1)].vardeclexpr)->toplevel(false);
        (yyval.vardeclexpr_v)->push_back((yyvsp[(1) - (1)].vardeclexpr)); ;}
    break;

  case 42:

    { (yyval.vardeclexpr_v)=(yyvsp[(1) - (3)].vardeclexpr_v);
        (yyvsp[(3) - (3)].vardeclexpr)->toplevel(false);
        (yyvsp[(1) - (3)].vardeclexpr_v)->push_back((yyvsp[(3) - (3)].vardeclexpr)); ;}
    break;

  case 45:

    { (yyval.vardeclexpr)=(yyvsp[(1) - (1)].vardeclexpr); ;}
    break;

  case 46:

    { (yyval.vardeclexpr)=new VarDecl((yyloc), (yyvsp[(1) - (1)].tiexpr), ""); ;}
    break;

  case 47:

    { (yyval.vardeclexpr) = new VarDecl((yyloc), (yyvsp[(1) - (3)].tiexpr), (yyvsp[(3) - (3)].sValue));
        free((yyvsp[(3) - (3)].sValue));
      ;}
    break;

  case 48:

    { (yyval.tiexpr_v)=(yyvsp[(1) - (2)].tiexpr_v); ;}
    break;

  case 49:

    { (yyval.tiexpr_v)=new vector<TypeInst*>(); (yyval.tiexpr_v)->push_back((yyvsp[(1) - (1)].tiexpr)); ;}
    break;

  case 50:

    { (yyval.tiexpr_v)=(yyvsp[(1) - (3)].tiexpr_v); (yyvsp[(1) - (3)].tiexpr_v)->push_back((yyvsp[(3) - (3)].tiexpr)); ;}
    break;

  case 52:

    {
        (yyval.tiexpr) = (yyvsp[(6) - (6)].tiexpr);
        (yyval.tiexpr)->setRanges(*(yyvsp[(3) - (6)].tiexpr_v));
        delete (yyvsp[(3) - (6)].tiexpr_v);
      ;}
    break;

  case 53:

    {
        (yyval.tiexpr) = (yyvsp[(3) - (3)].tiexpr);
        std::vector<TypeInst*> ti(1);
        ti[0] = new TypeInst((yyloc),Type::parint());
        (yyval.tiexpr)->setRanges(ti);
      ;}
    break;

  case 54:

    { (yyval.tiexpr) = (yyvsp[(1) - (1)].tiexpr);
      ;}
    break;

  case 55:

    { (yyval.tiexpr) = (yyvsp[(2) - (2)].tiexpr);
        Type tt = (yyval.tiexpr)->type();
        tt.ot(Type::OT_OPTIONAL);
        (yyval.tiexpr)->type(tt);
      ;}
    break;

  case 56:

    { (yyval.tiexpr) = (yyvsp[(3) - (3)].tiexpr);
        if ((yyvsp[(2) - (3)].bValue)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 57:

    { (yyval.tiexpr) = (yyvsp[(3) - (3)].tiexpr);
        Type tt = (yyval.tiexpr)->type();
        tt.ti(Type::TI_VAR);
        if ((yyvsp[(2) - (3)].bValue)) tt.ot(Type::OT_OPTIONAL);
        (yyval.tiexpr)->type(tt);
      ;}
    break;

  case 58:

    { (yyval.tiexpr) = (yyvsp[(4) - (4)].tiexpr);
        Type tt = (yyval.tiexpr)->type();
        tt.st(Type::ST_SET);
        if ((yyvsp[(1) - (4)].bValue)) tt.ot(Type::OT_OPTIONAL);
        (yyval.tiexpr)->type(tt);
      ;}
    break;

  case 59:

    { (yyval.tiexpr) = (yyvsp[(5) - (5)].tiexpr);
        Type tt = (yyval.tiexpr)->type();
        tt.st(Type::ST_SET);
        if ((yyvsp[(2) - (5)].bValue)) tt.ot(Type::OT_OPTIONAL);
        (yyval.tiexpr)->type(tt);
      ;}
    break;

  case 60:

    { (yyval.tiexpr) = (yyvsp[(5) - (5)].tiexpr);
        Type tt = (yyval.tiexpr)->type();
        tt.ti(Type::TI_VAR);
        tt.st(Type::ST_SET);
        if ((yyvsp[(2) - (5)].bValue)) tt.ot(Type::OT_OPTIONAL);
        (yyval.tiexpr)->type(tt);
      ;}
    break;

  case 61:

    { (yyval.bValue) = false; ;}
    break;

  case 62:

    { (yyval.bValue) = true; ;}
    break;

  case 63:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parint()); ;}
    break;

  case 64:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parbool()); ;}
    break;

  case 65:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parfloat()); ;}
    break;

  case 66:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parstring()); ;}
    break;

  case 67:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::ann()); ;}
    break;

  case 68:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type(),(yyvsp[(1) - (1)].expression)); ;}
    break;

  case 69:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::top(),
                         new TIId((yyloc), (yyvsp[(1) - (1)].sValue)));
        free((yyvsp[(1) - (1)].sValue));
      ;}
    break;

  case 71:

    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].expression)); ;}
    break;

  case 72:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); (yyval.expression_v)->push_back((yyvsp[(3) - (3)].expression)); ;}
    break;

  case 74:

    { (yyvsp[(1) - (3)].expression)->addAnnotation((yyvsp[(3) - (3)].expression)); (yyval.expression)=(yyvsp[(1) - (3)].expression); ;}
    break;

  case 75:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_UNION, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 76:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 77:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SYMDIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 78:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DOTDOT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 79:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(3) - (6)].expression), BOT_DOTDOT, (yyvsp[(5) - (6)].expression)); ;}
    break;

  case 80:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_INTERSECT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 81:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUSPLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 82:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 83:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MINUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 84:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MULT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 85:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 86:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IDIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 87:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MOD, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 88:

    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=new Call((yyloc), (yyvsp[(2) - (3)].sValue), args);
        free((yyvsp[(2) - (3)].sValue));
      ;}
    break;

  case 89:

    { (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 90:

    { if ((yyvsp[(2) - (2)].expression)->isa<IntLit>()) {
          (yyvsp[(2) - (2)].expression)->cast<IntLit>()->v(-(yyvsp[(2) - (2)].expression)->cast<IntLit>()->v());
          (yyval.expression) = (yyvsp[(2) - (2)].expression);
        } else if ((yyvsp[(2) - (2)].expression)->isa<FloatLit>()) {
          (yyvsp[(2) - (2)].expression)->cast<FloatLit>()->v(-(yyvsp[(2) - (2)].expression)->cast<FloatLit>()->v());
          (yyval.expression) = (yyvsp[(2) - (2)].expression);
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_MINUS, (yyvsp[(2) - (2)].expression));
        }
      ;}
    break;

  case 92:

    { (yyvsp[(1) - (3)].expression)->addAnnotation((yyvsp[(3) - (3)].expression)); (yyval.expression)=(yyvsp[(1) - (3)].expression); ;}
    break;

  case 93:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_EQUIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 94:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IMPL, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 95:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_RIMPL, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 96:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_OR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 97:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_XOR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 98:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_AND, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 99:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_LE, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 100:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_GR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 101:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_LQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 102:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_GQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 103:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_EQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 104:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_NQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 105:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IN, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 106:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SUBSET, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 107:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SUPERSET, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 108:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_UNION, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 109:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 110:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SYMDIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 111:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DOTDOT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 112:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(3) - (6)].expression), BOT_DOTDOT, (yyvsp[(5) - (6)].expression)); ;}
    break;

  case 113:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_INTERSECT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 114:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUSPLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 115:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 116:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MINUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 117:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MULT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 118:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 119:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IDIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 120:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MOD, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 121:

    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=new Call((yyloc), (yyvsp[(2) - (3)].sValue), args);
        free((yyvsp[(2) - (3)].sValue));
      ;}
    break;

  case 122:

    { (yyval.expression)=new UnOp((yyloc), UOT_NOT, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 123:

    { if ((yyvsp[(2) - (2)].expression)->isa<IntLit>() || (yyvsp[(2) - (2)].expression)->isa<FloatLit>()) {
          (yyval.expression) = (yyvsp[(2) - (2)].expression);
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[(2) - (2)].expression));
        }
      ;}
    break;

  case 124:

    { if ((yyvsp[(2) - (2)].expression)->isa<IntLit>()) {
        (yyvsp[(2) - (2)].expression)->cast<IntLit>()->v(-(yyvsp[(2) - (2)].expression)->cast<IntLit>()->v());
          (yyval.expression) = (yyvsp[(2) - (2)].expression);
        } else if ((yyvsp[(2) - (2)].expression)->isa<FloatLit>()) {
          (yyvsp[(2) - (2)].expression)->cast<FloatLit>()->v(-(yyvsp[(2) - (2)].expression)->cast<FloatLit>()->v());
          (yyval.expression) = (yyvsp[(2) - (2)].expression);
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_MINUS, (yyvsp[(2) - (2)].expression));
        }
      ;}
    break;

  case 125:

    { (yyval.expression)=(yyvsp[(2) - (3)].expression); ;}
    break;

  case 126:

    { (yyval.expression)=new ArrayAccess((yyloc), (yyvsp[(2) - (4)].expression), *(yyvsp[(4) - (4)].expression_v)); delete (yyvsp[(4) - (4)].expression_v); ;}
    break;

  case 127:

    { (yyval.expression)=new Id((yyloc), (yyvsp[(1) - (1)].sValue), NULL); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 128:

    { (yyval.expression)=new ArrayAccess((yyloc), new Id((yylsp[(1) - (2)]),(yyvsp[(1) - (2)].sValue),NULL), *(yyvsp[(2) - (2)].expression_v));
        free((yyvsp[(1) - (2)].sValue)); delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 129:

    { (yyval.expression)=new AnonVar((yyloc)); ;}
    break;

  case 130:

    { (yyval.expression)=new ArrayAccess((yyloc), new AnonVar((yyloc)), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 131:

    { (yyval.expression)=new BoolLit((yyloc), ((yyvsp[(1) - (1)].iValue)!=0)); ;}
    break;

  case 132:

    { (yyval.expression)=new IntLit((yyloc), (yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 133:

    { (yyval.expression)=new IntLit((yyloc), IntVal::infinity); ;}
    break;

  case 134:

    { (yyval.expression)=new FloatLit((yyloc), (yyvsp[(1) - (1)].dValue)); ;}
    break;

  case 135:

    { (yyval.expression)=new StringLit((yyloc), (yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 136:

    { (yyval.expression)=constants().absent; ;}
    break;

  case 140:

    { (yyval.expression)=new ArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 142:

    { (yyval.expression)=new ArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 144:

    { (yyval.expression)=new ArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 146:

    { (yyval.expression)=new ArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 149:

    { (yyval.expression)=new ArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 150:

    { (yyval.expression_v)=(yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 151:

    { (yyval.expression) = new SetLit((yyloc), std::vector<Expression*>()); ;}
    break;

  case 152:

    { (yyval.expression) = new SetLit((yyloc), *(yyvsp[(2) - (3)].expression_v)); delete (yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 153:

    { (yyval.expression) = new Comprehension((yyloc), (yyvsp[(2) - (5)].expression), *(yyvsp[(4) - (5)].generators), true);
        delete (yyvsp[(4) - (5)].generators);
      ;}
    break;

  case 154:

    { (yyval.generators)=new Generators; (yyval.generators)->_g = *(yyvsp[(1) - (1)].generator_v); (yyval.generators)->_w = NULL; delete (yyvsp[(1) - (1)].generator_v); ;}
    break;

  case 155:

    { (yyval.generators)=new Generators; (yyval.generators)->_g = *(yyvsp[(1) - (3)].generator_v); (yyval.generators)->_w = (yyvsp[(3) - (3)].expression); delete (yyvsp[(1) - (3)].generator_v); ;}
    break;

  case 157:

    { (yyval.generator_v)=new std::vector<Generator>; (yyval.generator_v)->push_back(*(yyvsp[(1) - (1)].generator)); delete (yyvsp[(1) - (1)].generator); ;}
    break;

  case 158:

    { (yyval.generator_v)=(yyvsp[(1) - (3)].generator_v); (yyval.generator_v)->push_back(*(yyvsp[(3) - (3)].generator)); delete (yyvsp[(3) - (3)].generator); ;}
    break;

  case 159:

    { (yyval.generator)=new Generator(*(yyvsp[(1) - (3)].string_v),(yyvsp[(3) - (3)].expression)); delete (yyvsp[(1) - (3)].string_v); ;}
    break;

  case 161:

    { (yyval.string_v)=new std::vector<std::string>; (yyval.string_v)->push_back((yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 162:

    { (yyval.string_v)=(yyvsp[(1) - (3)].string_v); (yyval.string_v)->push_back((yyvsp[(3) - (3)].sValue)); free((yyvsp[(3) - (3)].sValue)); ;}
    break;

  case 163:

    { (yyval.expression)=new ArrayLit((yyloc), std::vector<MiniZinc::Expression*>()); ;}
    break;

  case 164:

    { (yyval.expression)=new ArrayLit((yyloc), *(yyvsp[(2) - (3)].expression_v)); delete (yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 165:

    { (yyval.expression)=new ArrayLit((yyloc), *(yyvsp[(2) - (3)].expression_vv));
        for (unsigned int i=1; i<(yyvsp[(2) - (3)].expression_vv)->size(); i++)
          if ((*(yyvsp[(2) - (3)].expression_vv))[i].size() != (*(yyvsp[(2) - (3)].expression_vv))[i-1].size())
            yyerror(&(yylsp[(2) - (3)]), parm, "syntax error, all sub-arrays of 2d array literal must have the same length");
        delete (yyvsp[(2) - (3)].expression_vv);
      ;}
    break;

  case 166:

    { (yyval.expression)=new ArrayLit((yyloc), *(yyvsp[(2) - (4)].expression_vv));
        for (unsigned int i=1; i<(yyvsp[(2) - (4)].expression_vv)->size(); i++)
          if ((*(yyvsp[(2) - (4)].expression_vv))[i].size() != (*(yyvsp[(2) - (4)].expression_vv))[i-1].size())
            yyerror(&(yylsp[(2) - (4)]), parm, "syntax error, all sub-arrays of 2d array literal must have the same length");
        delete (yyvsp[(2) - (4)].expression_vv);
      ;}
    break;

  case 167:

    { (yyval.expression_vv)=new std::vector<std::vector<MiniZinc::Expression*> >;
        (yyval.expression_vv)->push_back(*(yyvsp[(1) - (1)].expression_v));
        delete (yyvsp[(1) - (1)].expression_v);
      ;}
    break;

  case 168:

    { (yyval.expression_vv)=(yyvsp[(1) - (3)].expression_vv); (yyval.expression_vv)->push_back(*(yyvsp[(3) - (3)].expression_v)); delete (yyvsp[(3) - (3)].expression_v); ;}
    break;

  case 169:

    { (yyval.expression)=new Comprehension((yyloc), (yyvsp[(2) - (5)].expression), *(yyvsp[(4) - (5)].generators), false);
        delete (yyvsp[(4) - (5)].generators);
      ;}
    break;

  case 170:

    {
        std::vector<Expression*> iexps;
        iexps.push_back((yyvsp[(2) - (8)].expression));
        iexps.push_back((yyvsp[(4) - (8)].expression));
        for (unsigned int i=0; i<(yyvsp[(5) - (8)].expression_v)->size(); i+=2) {
          iexps.push_back((*(yyvsp[(5) - (8)].expression_v))[i]);
          iexps.push_back((*(yyvsp[(5) - (8)].expression_v))[i+1]);
        }
        (yyval.expression)=new ITE((yyloc), iexps,(yyvsp[(7) - (8)].expression));
        delete (yyvsp[(5) - (8)].expression_v);
      ;}
    break;

  case 171:

    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; ;}
    break;

  case 172:

    { (yyval.expression_v)=(yyvsp[(1) - (5)].expression_v); (yyval.expression_v)->push_back((yyvsp[(3) - (5)].expression)); (yyval.expression_v)->push_back((yyvsp[(5) - (5)].expression)); ;}
    break;

  case 173:

    { (yyval.iValue)=BOT_EQUIV; ;}
    break;

  case 174:

    { (yyval.iValue)=BOT_IMPL; ;}
    break;

  case 175:

    { (yyval.iValue)=BOT_RIMPL; ;}
    break;

  case 176:

    { (yyval.iValue)=BOT_OR; ;}
    break;

  case 177:

    { (yyval.iValue)=BOT_XOR; ;}
    break;

  case 178:

    { (yyval.iValue)=BOT_AND; ;}
    break;

  case 179:

    { (yyval.iValue)=BOT_LE; ;}
    break;

  case 180:

    { (yyval.iValue)=BOT_GR; ;}
    break;

  case 181:

    { (yyval.iValue)=BOT_LQ; ;}
    break;

  case 182:

    { (yyval.iValue)=BOT_GQ; ;}
    break;

  case 183:

    { (yyval.iValue)=BOT_EQ; ;}
    break;

  case 184:

    { (yyval.iValue)=BOT_NQ; ;}
    break;

  case 185:

    { (yyval.iValue)=BOT_IN; ;}
    break;

  case 186:

    { (yyval.iValue)=BOT_SUBSET; ;}
    break;

  case 187:

    { (yyval.iValue)=BOT_SUPERSET; ;}
    break;

  case 188:

    { (yyval.iValue)=BOT_UNION; ;}
    break;

  case 189:

    { (yyval.iValue)=BOT_DIFF; ;}
    break;

  case 190:

    { (yyval.iValue)=BOT_SYMDIFF; ;}
    break;

  case 191:

    { (yyval.iValue)=BOT_PLUS; ;}
    break;

  case 192:

    { (yyval.iValue)=BOT_MINUS; ;}
    break;

  case 193:

    { (yyval.iValue)=BOT_MULT; ;}
    break;

  case 194:

    { (yyval.iValue)=BOT_DIV; ;}
    break;

  case 195:

    { (yyval.iValue)=BOT_IDIV; ;}
    break;

  case 196:

    { (yyval.iValue)=BOT_MOD; ;}
    break;

  case 197:

    { (yyval.iValue)=BOT_INTERSECT; ;}
    break;

  case 198:

    { (yyval.iValue)=BOT_PLUSPLUS; ;}
    break;

  case 199:

    { (yyval.iValue)=-1; ;}
    break;

  case 200:

    { if ((yyvsp[(1) - (6)].iValue)==-1) {
          (yyval.expression)=NULL;
          yyerror(&(yylsp[(3) - (6)]), parm, "syntax error, unary operator with two arguments");
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[(3) - (6)].expression),static_cast<BinOpType>((yyvsp[(1) - (6)].iValue)),(yyvsp[(5) - (6)].expression));
        }
      ;}
    break;

  case 201:

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
          if (uot==UOT_PLUS && ((yyvsp[(3) - (4)].expression)->isa<IntLit>() || (yyvsp[(3) - (4)].expression)->isa<FloatLit>())) {
            (yyval.expression) = (yyvsp[(3) - (4)].expression);
          } else if (uot==UOT_MINUS && (yyvsp[(3) - (4)].expression)->isa<IntLit>()) {
            (yyvsp[(3) - (4)].expression)->cast<IntLit>()->v(-(yyvsp[(3) - (4)].expression)->cast<IntLit>()->v());
          } else if (uot==UOT_MINUS && (yyvsp[(3) - (4)].expression)->isa<FloatLit>()) {
            (yyvsp[(3) - (4)].expression)->cast<FloatLit>()->v(-(yyvsp[(3) - (4)].expression)->cast<FloatLit>()->v());
          } else {
            (yyval.expression)=new UnOp((yyloc), static_cast<UnOpType>(uot),(yyvsp[(3) - (4)].expression));
          }
        }
      ;}
    break;

  case 202:

    { (yyval.expression)=new Call((yyloc), (yyvsp[(1) - (3)].sValue), std::vector<Expression*>()); free((yyvsp[(1) - (3)].sValue)); ;}
    break;

  case 204:

    { 
        if ((yyvsp[(3) - (4)].expression_p)->second) {
          yyerror(&(yylsp[(3) - (4)]), parm, "syntax error, 'where' expression outside generator call");
          (yyval.expression)=NULL;
        } else {
          (yyval.expression)=new Call((yyloc), (yyvsp[(1) - (4)].sValue), (yyvsp[(3) - (4)].expression_p)->first);
        }
        free((yyvsp[(1) - (4)].sValue));
        delete (yyvsp[(3) - (4)].expression_p);
      ;}
    break;

  case 205:

    { 
        vector<Generator> gens;
        vector<ASTString> ids;
        for (unsigned int i=0; i<(yyvsp[(3) - (7)].expression_p)->first.size(); i++) {
          if (Id* id = (yyvsp[(3) - (7)].expression_p)->first[i]->dyn_cast<Id>()) {
            ids.push_back(id->v());
          } else {
            if (BinOp* boe = (yyvsp[(3) - (7)].expression_p)->first[i]->dyn_cast<BinOp>()) {
              Id* id = boe->lhs()->dyn_cast<Id>();
              if (id && boe->op() == BOT_IN) {
                ids.push_back(id->v());
                gens.push_back(Generator(ids,boe->rhs()));
                ids = vector<ASTString>();
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
          Comprehension* ac = new Comprehension((yyloc), (yyvsp[(6) - (7)].expression),g,false);
          vector<Expression*> args; args.push_back(ac);
          (yyval.expression)=new Call((yyloc), (yyvsp[(1) - (7)].sValue), args);
        }
        free((yyvsp[(1) - (7)].sValue));
        delete (yyvsp[(3) - (7)].expression_p);
      ;}
    break;

  case 206:

    { (yyval.expression_p)=new pair<vector<Expression*>,Expression*>;
        (yyval.expression_p)->first=*(yyvsp[(1) - (1)].expression_v); (yyval.expression_p)->second=NULL;
        delete (yyvsp[(1) - (1)].expression_v);
      ;}
    break;

  case 207:

    { (yyval.expression_p)=new pair<vector<Expression*>,Expression*>;
        (yyval.expression_p)->first=*(yyvsp[(1) - (3)].expression_v); (yyval.expression_p)->second=(yyvsp[(3) - (3)].expression);
        delete (yyvsp[(1) - (3)].expression_v);
      ;}
    break;

  case 208:

    { (yyval.expression)=new Let((yyloc), *(yyvsp[(3) - (6)].expression_v), (yyvsp[(6) - (6)].expression)); delete (yyvsp[(3) - (6)].expression_v); ;}
    break;

  case 209:

    { (yyval.expression)=new Let((yyloc), *(yyvsp[(3) - (7)].expression_v), (yyvsp[(7) - (7)].expression)); delete (yyvsp[(3) - (7)].expression_v); ;}
    break;

  case 210:

    { (yyval.expression_v)=new vector<Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].vardeclexpr)); ;}
    break;

  case 211:

    { (yyval.expression_v)=new vector<Expression*>;
        ConstraintI* ce = (yyvsp[(1) - (1)].item)->cast<ConstraintI>();
        (yyval.expression_v)->push_back(ce->e());
        ce->e(NULL);
      ;}
    break;

  case 212:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); (yyval.expression_v)->push_back((yyvsp[(3) - (3)].vardeclexpr)); ;}
    break;

  case 213:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v);
        ConstraintI* ce = (yyvsp[(3) - (3)].item)->cast<ConstraintI>();
        (yyval.expression_v)->push_back(ce->e());
        ce->e(NULL);
      ;}
    break;

  case 216:

    { (yyval.vardeclexpr) = (yyvsp[(1) - (2)].vardeclexpr);
        (yyval.vardeclexpr)->toplevel(false);
        if ((yyvsp[(2) - (2)].expression_v)) (yyval.vardeclexpr)->addAnnotations(*(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v);
      ;}
    break;

  case 217:

    { (yyvsp[(1) - (4)].vardeclexpr)->e((yyvsp[(4) - (4)].expression));
        (yyval.vardeclexpr) = (yyvsp[(1) - (4)].vardeclexpr);
        (yyval.vardeclexpr)->loc((yyloc));
        (yyval.vardeclexpr)->toplevel(false);
        if ((yyvsp[(2) - (4)].expression_v)) (yyval.vardeclexpr)->addAnnotations(*(yyvsp[(2) - (4)].expression_v));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 218:

    { (yyval.expression_v)=NULL; ;}
    break;

  case 220:

    { (yyval.expression_v)=new std::vector<Expression*>(1);
        (*(yyval.expression_v))[0] = (yyvsp[(2) - (2)].expression);
      ;}
    break;

  case 221:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); (yyval.expression_v)->push_back((yyvsp[(3) - (3)].expression)); ;}
    break;

  case 222:

    { (yyval.sValue)=(yyvsp[(1) - (1)].sValue); ;}
    break;

  case 223:

    { (yyval.sValue)=strdup("<->"); ;}
    break;

  case 224:

    { (yyval.sValue)=strdup("->"); ;}
    break;

  case 225:

    { (yyval.sValue)=strdup("<-"); ;}
    break;

  case 226:

    { (yyval.sValue)=strdup("\\/"); ;}
    break;

  case 227:

    { (yyval.sValue)=strdup("xor"); ;}
    break;

  case 228:

    { (yyval.sValue)=strdup("/\\"); ;}
    break;

  case 229:

    { (yyval.sValue)=strdup("<"); ;}
    break;

  case 230:

    { (yyval.sValue)=strdup(">"); ;}
    break;

  case 231:

    { (yyval.sValue)=strdup("<="); ;}
    break;

  case 232:

    { (yyval.sValue)=strdup(">="); ;}
    break;

  case 233:

    { (yyval.sValue)=strdup("="); ;}
    break;

  case 234:

    { (yyval.sValue)=strdup("!="); ;}
    break;

  case 235:

    { (yyval.sValue)=strdup("in"); ;}
    break;

  case 236:

    { (yyval.sValue)=strdup("subset"); ;}
    break;

  case 237:

    { (yyval.sValue)=strdup("superset"); ;}
    break;

  case 238:

    { (yyval.sValue)=strdup("union"); ;}
    break;

  case 239:

    { (yyval.sValue)=strdup("diff"); ;}
    break;

  case 240:

    { (yyval.sValue)=strdup("symdiff"); ;}
    break;

  case 241:

    { (yyval.sValue)=strdup(".."); ;}
    break;

  case 242:

    { (yyval.sValue)=strdup("+"); ;}
    break;

  case 243:

    { (yyval.sValue)=strdup("-"); ;}
    break;

  case 244:

    { (yyval.sValue)=strdup("*"); ;}
    break;

  case 245:

    { (yyval.sValue)=strdup("/"); ;}
    break;

  case 246:

    { (yyval.sValue)=strdup("div"); ;}
    break;

  case 247:

    { (yyval.sValue)=strdup("mod"); ;}
    break;

  case 248:

    { (yyval.sValue)=strdup("intersect"); ;}
    break;

  case 249:

    { (yyval.sValue)=strdup("not"); ;}
    break;

  case 250:

    { (yyval.sValue)=strdup("++"); ;}
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



