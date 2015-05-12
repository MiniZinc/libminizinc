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




/* Copy the first part of user declarations.  */


#define SCANNER static_cast<ParserState*>(parm)->yyscanner
#include <iostream>
#include <fstream>
#include <map>
#include <cerrno>

namespace MiniZinc{ class Location; }
#define YYLTYPE MiniZinc::Location
#define YYLTYPE_IS_DECLARED 1
#define YYLTYPE_IS_TRIVIAL 0

#include <minizinc/parser.hh>
#include <minizinc/file_utils.hh>

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
  for (int i=0; i<static_cast<int>(location->first_column)-1; i++)
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

Expression* createArrayAccess(const Location& loc, Expression* e, std::vector<std::vector<Expression*> >& idx) {
  Expression* ret = e;
  for (unsigned int i=0; i<idx.size(); i++) {
    ret = new ArrayAccess(loc, ret, idx[i]);
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
    }

    model->setFilepath(filename);
    bool isFzn = (filename.compare(filename.length()-4,4,".fzn")==0);
    isFzn |= (filename.compare(filename.length()-4,4,".ozn")==0);
    isFzn |= (filename.compare(filename.length()-4,4,".szn")==0);
    ParserState pp(filename,text, err, files, seenModels, model, false, isFzn, parseDocComments);
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
    for (unsigned int i=0; i<files.size(); i++)
      delete files[i].second;
    return NULL;
  }

  Model* parse(const string& filename,
               const vector<string>& datafiles,
               const vector<string>& ip,
               bool ignoreStdlib,
               bool parseDocComments,
               bool verbose,
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
    
    for (unsigned int i=0; i<datafiles.size(); i++) {
      string f = datafiles[i];
      std::string s;
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
                   bool parseDocComments,
                   bool verbose,
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
        if (FileUtils::file_exists(fullname)) {
          file.open(fullname.c_str(), std::ios::binary);
          if (file.is_open())
            break;
        }
      }
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
  
  for (unsigned int i=0; i<datafiles.size(); i++) {
    string f = datafiles[i];
    std::string s;
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
         std::vector<std::vector<std::vector<MiniZinc::Expression*> > >* expression_vvv;
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
#define YYFINAL  151
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   4073

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  132
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  64
/* YYNRULES -- Number of rules.  */
#define YYNRULES  268
/* YYNRULES -- Number of states.  */
#define YYNSTATES  458

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   378

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     126,   127,     2,     2,   128,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   125,   124,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   129,   131,   130,     2,     2,     2,     2,
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
     115,   116,   117,   118,   119,   120,   121,   122,   123
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    11,    14,    18,    23,
      27,    29,    32,    33,    35,    38,    40,    42,    44,    46,
      48,    50,    52,    54,    56,    58,    61,    64,    69,    73,
      76,    80,    85,    90,    93,    99,   105,   113,   122,   126,
     132,   133,   136,   137,   141,   145,   146,   149,   151,   155,
     156,   158,   160,   162,   166,   169,   171,   175,   177,   184,
     188,   190,   193,   197,   201,   206,   212,   218,   219,   221,
     223,   225,   227,   229,   231,   233,   235,   238,   240,   244,
     246,   250,   254,   258,   262,   266,   273,   277,   281,   285,
     289,   293,   297,   301,   305,   309,   312,   315,   317,   321,
     325,   329,   333,   337,   341,   345,   349,   353,   357,   361,
     365,   369,   373,   377,   381,   385,   389,   393,   397,   404,
     408,   412,   416,   420,   424,   428,   432,   436,   440,   443,
     446,   449,   453,   458,   460,   463,   465,   468,   470,   472,
     474,   476,   478,   480,   482,   485,   487,   490,   492,   495,
     497,   500,   502,   505,   507,   510,   512,   514,   517,   519,
     522,   525,   529,   533,   538,   541,   545,   551,   553,   557,
     560,   562,   566,   570,   573,   575,   579,   582,   586,   589,
     593,   598,   602,   605,   609,   615,   617,   621,   627,   636,
     637,   643,   645,   647,   649,   651,   653,   655,   657,   659,
     661,   663,   665,   667,   669,   671,   673,   675,   677,   679,
     681,   683,   685,   687,   689,   691,   693,   695,   697,   704,
     709,   713,   715,   720,   728,   730,   734,   741,   749,   751,
     753,   757,   761,   763,   765,   768,   773,   774,   776,   779,
     783,   785,   787,   789,   791,   793,   795,   797,   799,   801,
     803,   805,   807,   809,   811,   813,   815,   817,   819,   821,
     823,   825,   827,   829,   831,   833,   835,   837,   839
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     133,     0,    -1,   134,    -1,    -1,   135,   137,    -1,   138,
      -1,   136,   138,    -1,   135,   124,   138,    -1,   135,   124,
     136,   138,    -1,     1,   124,   138,    -1,    14,    -1,   136,
      14,    -1,    -1,   124,    -1,    13,   139,    -1,   139,    -1,
     140,    -1,   141,    -1,   142,    -1,   143,    -1,   144,    -1,
     145,    -1,   146,    -1,   147,    -1,   148,    -1,    33,     8,
      -1,   155,   193,    -1,   155,   193,    75,   165,    -1,     6,
      75,   165,    -1,    24,   165,    -1,    47,   193,    42,    -1,
      47,   193,    39,   165,    -1,    47,   193,    38,   165,    -1,
      43,   165,    -1,    44,     6,   150,   193,   149,    -1,    49,
       6,   150,   193,   149,    -1,    31,   158,   125,   195,   150,
     193,   149,    -1,   158,   125,     6,   126,   151,   127,   193,
     149,    -1,    19,     6,   150,    -1,    19,     6,   150,    75,
     165,    -1,    -1,    75,   165,    -1,    -1,   126,   151,   127,
      -1,   126,     1,   127,    -1,    -1,   152,   153,    -1,   154,
      -1,   152,   128,   154,    -1,    -1,   128,    -1,   155,    -1,
     158,    -1,   158,   125,     6,    -1,   157,   153,    -1,   158,
      -1,   157,   128,   158,    -1,   159,    -1,    21,    56,   156,
      58,    40,   159,    -1,    37,    40,   159,    -1,   161,    -1,
      41,   161,    -1,    16,   160,   161,    -1,    15,   160,   161,
      -1,   160,    46,    40,   161,    -1,    16,   160,    46,    40,
     161,    -1,    15,   160,    46,    40,   161,    -1,    -1,    41,
      -1,    35,    -1,    22,    -1,    30,    -1,    48,    -1,    18,
      -1,   164,    -1,    12,    -1,   163,   153,    -1,   165,    -1,
     163,   128,   165,    -1,   166,    -1,   164,    93,   166,    -1,
     164,    80,   164,    -1,   164,    81,   164,    -1,   164,    82,
     164,    -1,   164,    83,   164,    -1,   113,   126,   165,   128,
     165,   127,    -1,   164,    90,   164,    -1,   164,    92,   164,
      -1,   164,    84,   164,    -1,   164,    85,   164,    -1,   164,
      86,   164,    -1,   164,    87,   164,    -1,   164,    88,   164,
      -1,   164,    89,   164,    -1,   164,     7,   164,    -1,    84,
     164,    -1,    85,   164,    -1,   166,    -1,   165,    93,   166,
      -1,   165,    65,   165,    -1,   165,    66,   165,    -1,   165,
      67,   165,    -1,   165,    68,   165,    -1,   165,    69,   165,
      -1,   165,    70,   165,    -1,   165,    71,   165,    -1,   165,
      72,   165,    -1,   165,    73,   165,    -1,   165,    74,   165,
      -1,   165,    75,   165,    -1,   165,    76,   165,    -1,   165,
      77,   165,    -1,   165,    78,   165,    -1,   165,    79,   165,
      -1,   165,    80,   165,    -1,   165,    81,   165,    -1,   165,
      82,   165,    -1,   165,    83,   165,    -1,   113,   126,   165,
     128,   165,   127,    -1,   165,    90,   165,    -1,   165,    92,
     165,    -1,   165,    84,   165,    -1,   165,    85,   165,    -1,
     165,    86,   165,    -1,   165,    87,   165,    -1,   165,    88,
     165,    -1,   165,    89,   165,    -1,   165,     7,   165,    -1,
      91,   165,    -1,    84,   165,    -1,    85,   165,    -1,   126,
     165,   127,    -1,   126,   165,   127,   169,    -1,     6,    -1,
       6,   169,    -1,    53,    -1,    53,   169,    -1,     4,    -1,
       3,    -1,    34,    -1,     5,    -1,   167,    -1,    17,    -1,
     170,    -1,   170,   169,    -1,   171,    -1,   171,   169,    -1,
     178,    -1,   178,   169,    -1,   179,    -1,   179,   169,    -1,
     182,    -1,   182,   169,    -1,   183,    -1,   183,   169,    -1,
     189,    -1,   187,    -1,   187,   169,    -1,     8,    -1,     9,
     168,    -1,   163,    11,    -1,   163,    10,   168,    -1,    56,
     162,    58,    -1,   169,    56,   162,    58,    -1,   129,   130,
      -1,   129,   162,   130,    -1,   129,   165,   131,   172,   130,
      -1,   173,    -1,   173,    55,   165,    -1,   174,   153,    -1,
     175,    -1,   174,   128,   175,    -1,   176,    77,   165,    -1,
     177,   153,    -1,     6,    -1,   177,   128,     6,    -1,    56,
      58,    -1,    56,   162,    58,    -1,    57,    59,    -1,    57,
     181,    59,    -1,    57,   181,   131,    59,    -1,    57,   180,
      59,    -1,   131,   131,    -1,   131,   181,   131,    -1,   180,
     128,   131,   181,   131,    -1,   162,    -1,   181,   131,   162,
      -1,    56,   165,   131,   172,    58,    -1,    32,   165,    50,
     165,   184,    26,   165,    28,    -1,    -1,   184,    27,   165,
      50,   165,    -1,    95,    -1,    96,    -1,    97,    -1,    98,
      -1,    99,    -1,   100,    -1,   101,    -1,   102,    -1,   103,
      -1,   104,    -1,   105,    -1,   106,    -1,   107,    -1,   108,
      -1,   109,    -1,   110,    -1,   111,    -1,   112,    -1,   114,
      -1,   115,    -1,   116,    -1,   117,    -1,   118,    -1,   119,
      -1,   120,    -1,   123,    -1,   121,    -1,   185,   126,   165,
     128,   165,   127,    -1,   185,   126,   165,   127,    -1,     6,
     126,   127,    -1,   186,    -1,     6,   126,   188,   127,    -1,
       6,   126,   188,   127,   126,   165,   127,    -1,   162,    -1,
     162,    55,   165,    -1,    36,   129,   190,   130,    77,   165,
      -1,    36,   129,   190,   191,   130,    77,   165,    -1,   192,
      -1,   143,    -1,   190,   191,   192,    -1,   190,   191,   143,
      -1,   128,    -1,   124,    -1,   155,   193,    -1,   155,   193,
      75,   165,    -1,    -1,   194,    -1,    93,   166,    -1,   194,
      93,   166,    -1,     6,    -1,    95,    -1,    96,    -1,    97,
      -1,    98,    -1,    99,    -1,   100,    -1,   101,    -1,   102,
      -1,   103,    -1,   104,    -1,   105,    -1,   106,    -1,   107,
      -1,   108,    -1,   109,    -1,   110,    -1,   111,    -1,   112,
      -1,   113,    -1,   114,    -1,   115,    -1,   116,    -1,   117,
      -1,   118,    -1,   119,    -1,   120,    -1,   121,    -1,   123,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   645,   645,   647,   649,   652,   657,   662,   667,   672,
     675,   683,   692,   692,   694,   710,   714,   716,   718,   719,
     721,   723,   725,   727,   729,   733,   756,   761,   769,   775,
     779,   784,   789,   796,   800,   808,   818,   825,   834,   846,
     854,   855,   860,   861,   863,   868,   869,   873,   877,   882,
     882,   885,   887,   891,   896,   900,   902,   906,   907,   913,
     922,   925,   933,   941,   950,   959,   968,   981,   982,   986,
     988,   990,   992,   994,   996,   998,  1004,  1007,  1009,  1015,
    1016,  1018,  1020,  1022,  1024,  1026,  1028,  1030,  1032,  1034,
    1036,  1038,  1040,  1042,  1044,  1050,  1052,  1067,  1068,  1070,
    1072,  1074,  1076,  1078,  1080,  1082,  1084,  1086,  1088,  1090,
    1092,  1094,  1096,  1098,  1100,  1102,  1104,  1106,  1108,  1110,
    1112,  1114,  1116,  1118,  1120,  1122,  1124,  1126,  1132,  1134,
    1141,  1154,  1156,  1158,  1160,  1163,  1165,  1168,  1170,  1172,
    1174,  1176,  1177,  1179,  1180,  1183,  1184,  1187,  1188,  1191,
    1192,  1195,  1196,  1199,  1200,  1203,  1204,  1205,  1210,  1212,
    1218,  1223,  1231,  1238,  1247,  1249,  1253,  1259,  1261,  1264,
    1267,  1269,  1273,  1276,  1279,  1281,  1285,  1287,  1291,  1293,
    1304,  1315,  1355,  1358,  1363,  1370,  1375,  1379,  1385,  1401,
    1402,  1406,  1408,  1410,  1412,  1414,  1416,  1418,  1420,  1422,
    1424,  1426,  1428,  1430,  1432,  1434,  1436,  1438,  1440,  1442,
    1444,  1446,  1448,  1450,  1452,  1454,  1456,  1458,  1462,  1470,
    1502,  1504,  1505,  1516,  1559,  1564,  1571,  1573,  1577,  1579,
    1587,  1589,  1598,  1598,  1601,  1607,  1618,  1619,  1622,  1626,
    1630,  1632,  1634,  1636,  1638,  1640,  1642,  1644,  1646,  1648,
    1650,  1652,  1654,  1656,  1658,  1660,  1662,  1664,  1666,  1668,
    1670,  1672,  1674,  1676,  1678,  1680,  1682,  1684,  1686
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
  "\"documentation comment\"", "\"file-level documentation comment\"",
  "\"var\"", "\"par\"", "\"<>\"", "\"ann\"", "\"annotation\"", "\"any\"",
  "\"array\"", "\"bool\"", "\"case\"", "\"constraint\"", "\"default\"",
  "\"else\"", "\"elseif\"", "\"endif\"", "\"enum\"", "\"float\"",
  "\"function\"", "\"if\"", "\"include\"", "\"infinity\"", "\"int\"",
  "\"let\"", "\"list\"", "\"maximize\"", "\"minimize\"", "\"of\"",
  "\"opt\"", "\"satisfy\"", "\"output\"", "\"predicate\"", "\"record\"",
  "\"set\"", "\"solve\"", "\"string\"", "\"test\"", "\"then\"",
  "\"tuple\"", "\"type\"", "\"_\"", "\"variant_record\"", "\"where\"",
  "\"[\"", "\"[|\"", "\"]\"", "\"|]\"", "FLATZINC_IDENTIFIER",
  "\"invalid integer literal\"", "\"invalid float literal\"",
  "\"unterminated string\"", "\"null character\"", "\"<->\"", "\"->\"",
  "\"<-\"", "\"\\\\/\"", "\"xor\"", "\"/\\\\\"", "\"<\"", "\">\"",
  "\"<=\"", "\">=\"", "\"=\"", "\"!=\"", "\"in\"", "\"subset\"",
  "\"superset\"", "\"union\"", "\"diff\"", "\"symdiff\"", "\"..\"",
  "\"+\"", "\"-\"", "\"*\"", "\"/\"", "\"div\"", "\"mod\"",
  "\"intersect\"", "\"not\"", "\"++\"", "\"::\"", "PREC_ANNO", "\"'<->'\"",
  "\"'->'\"", "\"'<-'\"", "\"'\\\\/'\"", "\"'xor'\"", "\"'/\\\\'\"",
  "\"'<'\"", "\"'>'\"", "\"'<='\"", "\"'>='\"", "\"'='\"", "\"'!='\"",
  "\"'in'\"", "\"'subset'\"", "\"'superset'\"", "\"'union'\"",
  "\"'diff'\"", "\"'symdiff'\"", "\"'..'\"", "\"'+'\"", "\"'-'\"",
  "\"'*'\"", "\"'/'\"", "\"'div'\"", "\"'mod'\"", "\"'intersect'\"",
  "\"'not'\"", "\"'::'\"", "\"'++'\"", "';'", "':'", "'('", "')'", "','",
  "'{'", "'}'", "'|'", "$accept", "model", "item_list", "item_list_head",
  "doc_file_comments", "semi_or_none", "item", "item_tail", "include_item",
  "vardecl_item", "assign_item", "constraint_item", "solve_item",
  "output_item", "predicate_item", "function_item", "annotation_item",
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
  "quoted_op_call", "call_expr", "comp_or_expr", "let_expr",
  "let_vardecl_item_list", "comma_or_semi", "let_vardecl_item",
  "annotations", "ne_annotations", "id_or_quoted_op", 0
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
     375,   376,   377,   378,    59,    58,    40,    41,    44,   123,
     125,   124
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   132,   133,   134,   134,   135,   135,   135,   135,   135,
     136,   136,   137,   137,   138,   138,   139,   139,   139,   139,
     139,   139,   139,   139,   139,   140,   141,   141,   142,   143,
     144,   144,   144,   145,   146,   146,   147,   147,   148,   148,
     149,   149,   150,   150,   150,   151,   151,   152,   152,   153,
     153,   154,   154,   155,   156,   157,   157,   158,   158,   158,
     159,   159,   159,   159,   159,   159,   159,   160,   160,   161,
     161,   161,   161,   161,   161,   161,   162,   163,   163,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   165,   165,   165,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   167,   167,
     168,   168,   169,   169,   170,   170,   171,   172,   172,   173,
     174,   174,   175,   176,   177,   177,   178,   178,   179,   179,
     179,   179,   180,   180,   180,   181,   181,   182,   183,   184,
     184,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   186,   186,
     187,   187,   187,   187,   188,   188,   189,   189,   190,   190,
     190,   190,   191,   191,   192,   192,   193,   193,   194,   194,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     2,     3,     4,     3,
       1,     2,     0,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     4,     3,     2,
       3,     4,     4,     2,     5,     5,     7,     8,     3,     5,
       0,     2,     0,     3,     3,     0,     2,     1,     3,     0,
       1,     1,     1,     3,     2,     1,     3,     1,     6,     3,
       1,     2,     3,     3,     4,     5,     5,     0,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     3,     1,
       3,     3,     3,     3,     3,     6,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     2,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     6,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     2,
       2,     3,     4,     1,     2,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     2,     1,     2,     1,
       2,     1,     2,     1,     2,     1,     1,     2,     1,     2,
       2,     3,     3,     4,     2,     3,     5,     1,     3,     2,
       1,     3,     3,     2,     1,     3,     2,     3,     2,     3,
       4,     3,     2,     3,     5,     1,     3,     5,     8,     0,
       5,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     6,     4,
       3,     1,     4,     7,     1,     3,     6,     7,     1,     1,
       3,     3,     1,     1,     2,     4,     0,     1,     2,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,   138,   137,   140,   133,   158,     0,    75,    67,
      10,    67,    67,   142,    73,     0,     0,    70,     0,    71,
      67,     0,     0,   139,    69,     0,     0,    68,     0,     0,
     236,    72,     0,   135,     0,     0,     0,     0,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,     0,   209,   210,   211,
     212,   213,   214,   215,   217,   216,     0,     0,     0,     2,
      12,    67,     5,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,   236,     0,    57,     0,    60,    74,    79,
     141,   143,   145,   147,   149,   151,   153,     0,   221,   156,
     155,    67,     0,     0,     0,   134,   133,     0,     0,     0,
       0,     0,    77,    97,   159,    14,    68,     0,     0,    42,
      67,    29,     0,     0,    25,    67,    67,    61,    33,    42,
       0,     0,   237,    42,   136,   176,     0,    49,    77,   178,
       0,   185,     0,     0,    95,    96,     0,     0,   164,     0,
      77,     1,    13,     4,    11,     6,    26,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   144,   146,   148,   150,   152,   154,     0,
     157,     9,     0,    28,   220,   224,     0,     0,   129,   130,
     128,     0,     0,   160,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    63,     0,    62,     0,    38,
       0,    49,    55,     0,     0,   229,   236,     0,     0,   228,
      59,   236,   238,     0,     0,    30,     0,   236,   177,    50,
      76,     0,   182,     0,   181,     0,   179,     0,     0,   131,
     165,     0,    67,     7,     0,    53,     0,    94,    81,    82,
      83,    84,    88,    89,    90,    91,    92,    93,    86,    87,
      80,     0,   162,     0,   222,     0,     0,   161,    78,   127,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   121,
     122,   123,   124,   125,   126,   119,   120,    98,     0,     0,
       0,     0,    49,    47,    51,    52,     0,     0,    50,    54,
     240,   241,   242,   243,   244,   245,   246,   247,   248,   249,
     250,   251,   252,   253,   254,   255,   256,   257,   258,   259,
     260,   261,   262,   263,   264,   265,   266,   267,   268,    42,
     189,   234,     0,   233,   232,     0,    67,    40,    32,    31,
     239,    40,   174,     0,   167,    49,   170,     0,    49,   183,
       0,   180,   186,     0,   132,     0,     8,    27,    45,    64,
     219,     0,   225,     0,   163,     0,    66,    65,    44,    43,
      50,    46,    39,    67,    56,   236,     0,     0,    53,     0,
       0,   231,   230,     0,    34,    35,   187,     0,    50,   169,
       0,    50,   173,     0,     0,   166,     0,     0,     0,     0,
      48,    58,    40,     0,     0,   235,   226,     0,    41,   168,
     171,   172,   175,   184,    85,   236,   218,   223,   118,    36,
       0,     0,   227,    40,   188,     0,    37,   190
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    68,    69,    70,    71,   153,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,   414,   229,   321,
     322,   250,   323,    83,   230,   231,    84,    85,    86,    87,
     141,   137,    88,   112,   113,    90,   114,   105,    91,    92,
     373,   374,   375,   376,   377,   378,    93,    94,   142,   143,
      95,    96,   406,    97,    98,    99,   186,   100,   238,   366,
     239,   131,   132,   359
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -351
static const yytype_int16 yypact[] =
{
     501,  -100,  -351,  -351,  -351,   -43,  -351,  3055,  -351,  1658,
    -351,   -15,   -15,  -351,  -351,    22,   -25,  -351,  3055,  -351,
    2039,  3055,    35,  -351,  -351,   -84,     6,  2547,  3055,    44,
     -42,  -351,    46,    -3,  2674,   763,  3182,  3182,  -351,  -351,
    -351,  -351,  -351,  -351,  -351,  -351,  -351,  -351,  -351,  -351,
    -351,  -351,  -351,  -351,  -351,  -351,   -72,  -351,  -351,  -351,
    -351,  -351,  -351,  -351,  -351,  -351,  3055,  1149,    55,  -351,
     -68,  1404,  -351,  -351,  -351,  -351,  -351,  -351,  -351,  -351,
    -351,  -351,  -351,   -42,   -67,  -351,    11,  -351,    57,  -351,
    -351,    -3,    -3,    -3,    -3,    -3,    -3,   -66,  -351,    -3,
    -351,  1531,  3055,  3055,  2801,     3,   -26,  3055,  3055,  3055,
     -65,     8,  3952,  -351,  -351,  -351,  -351,  2293,  2420,   -64,
    2039,  3952,   -62,  3839,  -351,  1785,  2166,  -351,  3952,   -64,
    3217,    -2,   -28,   -64,     3,  -351,     9,   -60,   390,  -351,
     892,  -351,   -36,   -32,    15,    15,  3055,  3529,  -351,   -52,
    3297,  -351,  1277,  -351,  -351,  -351,    -9,    64,    39,  3182,
    3182,  3182,  3182,  3182,  3182,  3182,  3182,  3182,  3182,  3182,
    3182,  3182,  3217,     3,     3,     3,     3,     3,     3,  3055,
       3,  -351,    23,  3952,  -351,    29,   -41,  3055,    18,    18,
      18,  3055,  3055,  -351,  3055,  3055,  3055,  3055,  3055,  3055,
    3055,  3055,  3055,  3055,  3055,  3055,  3055,  3055,  3055,  3055,
    3055,  3055,  3055,  3055,  3055,  3055,  3055,  3055,  3055,  3055,
    3055,  3055,  3055,  3217,    47,  -351,    48,  -351,   636,    19,
      43,   -35,  -351,  3676,  3055,  -351,   -42,   -23,   -95,  -351,
    -351,   -42,  -351,  3055,  3055,  -351,  3217,   -42,  -351,  3055,
    -351,    91,  -351,     0,  -351,     1,  -351,  2928,  3416,    -3,
    -351,    91,  1404,  -351,  3055,   -21,  2547,    20,   234,   234,
     234,   251,   141,   141,    15,    15,    15,    15,   234,    15,
    -351,  3332,  -351,  3055,     7,    76,  3445,  -351,  3952,    42,
    3980,   626,   626,   753,   753,   880,  1009,  1009,  1009,  1009,
    1009,  1009,    37,    37,    37,   268,   268,   268,   477,   167,
     167,    18,    18,    18,    18,   268,    18,  -351,  2547,  2547,
      24,    25,    28,  -351,  -351,   -23,  3055,   113,  1912,  -351,
    -351,  -351,  -351,  -351,  -351,  -351,  -351,  -351,  -351,  -351,
    -351,  -351,  -351,  -351,  -351,  -351,  -351,  -351,  -351,  -351,
    -351,  -351,  -351,  -351,  -351,  -351,  -351,  -351,  -351,   -64,
    3952,    82,   152,  -351,  -351,    86,  1021,    89,  3952,  3952,
    -351,    89,  -351,   107,   112,    40,  -351,    92,    78,  3055,
    3055,  -351,  -351,  3055,     3,    41,  -351,  3952,  1912,  -351,
    -351,  3055,  3952,  3055,  -351,  3055,  -351,  -351,  -351,  -351,
    1912,  -351,  3952,  2166,  -351,   -42,    12,  3055,  -351,  3055,
      93,  -351,  -351,  3055,  -351,  -351,  -351,  3055,    91,  -351,
    3055,   166,  -351,    79,  3558,  -351,    80,  3642,  3671,  3755,
    -351,  -351,    89,  3055,  3055,  3952,  3952,  3055,  3952,  3952,
    -351,  3952,  -351,  3055,  -351,   -42,  -351,  -351,  -351,  -351,
    3868,  3923,  3952,    89,  -351,  3055,  -351,  3952
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -351,  -351,  -351,  -351,    59,  -351,   -54,   200,  -351,  -351,
    -351,  -118,  -351,  -351,  -351,  -351,  -351,  -350,  -121,  -213,
    -351,  -216,  -187,  -116,  -351,  -351,   -16,  -120,    30,   -22,
     -33,    13,   279,   -18,   262,  -351,    26,   -19,  -351,  -351,
     -47,  -351,  -351,  -203,  -351,  -351,  -351,  -351,  -351,  -129,
    -351,  -351,  -351,  -351,  -351,  -351,  -351,  -351,  -351,  -351,
    -149,   -81,  -351,  -351
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -68
static const yytype_int16 yytable[] =
{
     121,   136,   156,   123,   122,   127,   240,   235,   241,   236,
     128,   253,   247,   102,   134,   329,   138,   155,   192,   193,
     111,   415,   159,   254,   101,   195,   116,   256,   119,   363,
     102,   120,   103,   364,   149,   365,   243,   244,   433,   434,
     245,   117,   118,   124,   195,   125,   126,   181,   147,   150,
     129,   130,   133,   102,   146,   151,   152,   158,   157,   187,
     179,   191,   228,   233,   159,   246,   264,   248,   249,   182,
     265,   185,   173,   174,   175,   176,   177,   178,   260,   266,
     180,   282,   449,   104,   283,   183,   284,   318,   319,   188,
     189,   190,   255,   328,   326,   225,   227,   372,   263,   257,
     104,   327,   362,   456,   232,   388,   401,   171,   172,   237,
     222,   223,   324,   172,   -68,   -68,   -68,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   258,   222,
     223,   379,   380,   393,   394,   223,   194,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   170,   159,   171,
     172,   398,   399,   403,   285,   361,   400,   407,   408,   419,
     367,   281,   422,   409,   413,   416,   371,   417,   418,   420,
     437,   425,   442,   286,   195,   426,   288,   289,   290,   291,
     292,   293,   294,   295,   296,   297,   298,   299,   300,   301,
     302,   303,   304,   305,   306,   307,   308,   309,   310,   311,
     312,   313,   314,   315,   316,   111,   421,   445,   386,   115,
     443,   262,   325,   430,   385,   440,   360,   412,   287,     0,
       0,     0,     0,     0,   382,   368,   369,   166,   167,   168,
     169,   288,     0,   171,   172,     0,     0,     0,   405,     0,
     384,   159,     0,     0,   389,     0,   387,     0,   411,     0,
     236,   423,     0,   217,   218,   219,   220,     0,   159,   222,
     223,     0,    89,     0,     0,   392,     0,     0,     0,     0,
       0,    89,   324,     0,     0,   195,     0,     0,     0,     0,
       0,     0,    89,   431,   324,     0,     0,     0,     0,    89,
       0,     0,     0,     0,     0,     0,   396,   397,    89,    89,
       0,     0,     0,     0,     0,     0,     0,     0,   402,     0,
       0,     0,   404,     0,     0,   144,   145,   163,   164,   165,
     166,   167,   168,   169,   432,     0,   171,   172,     0,     0,
       0,     0,     0,    89,   -68,   164,   165,   166,   167,   168,
     169,     0,     0,   171,   172,     0,   382,     0,     0,     0,
     237,   214,   215,   216,   217,   218,   219,   220,     0,     0,
     222,   223,     0,    89,   453,   424,     0,     0,     0,     0,
       0,     0,   325,   427,     0,   428,     0,   429,     0,    89,
      89,     0,    89,     0,   325,     0,     0,    89,    89,   435,
       0,   436,   242,     0,     0,   438,     0,   195,     0,   439,
       0,     0,   441,     0,     0,     0,     0,     0,     0,     0,
     382,     0,     0,     0,    89,   450,   451,     0,     0,   452,
       0,    89,    89,    89,    89,    89,    89,    89,    89,    89,
      89,    89,    89,    89,   280,     0,     0,   457,   267,   268,
     269,   270,   271,   272,   273,   274,   275,   276,   277,   278,
     279,     0,     0,     0,     0,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,     0,   222,   223,   195,   317,     0,     0,     0,     0,
      89,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    -3,     1,     0,     2,     3,     4,     5,   370,     6,
       7,     0,     0,     8,     9,    10,    11,    12,    13,    14,
      15,   251,    16,    17,    89,    18,     0,     0,    89,     0,
       0,    19,    20,    21,    22,    23,    24,    25,    26,     0,
       0,     0,    27,     0,    28,    29,     0,   -67,    30,    31,
      32,     0,     0,     0,    33,     0,     0,    34,    35,     0,
     -68,   215,   216,   217,   218,   219,   220,     0,     0,   222,
     223,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      89,    89,     0,     0,     0,    36,    37,     0,     0,     0,
      89,     0,     0,     0,     0,     0,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,     0,    65,     0,     0,    66,    89,     0,
      67,     0,     0,   195,     0,     0,     0,   320,     0,     2,
       3,     4,   106,     0,     6,     7,     0,     0,     8,     0,
      89,    11,    12,    13,    14,     0,     0,    16,    17,     0,
       0,     0,    89,     0,     0,    89,    19,     0,    21,     0,
      23,    24,    25,    26,     0,     0,     0,    27,     0,     0,
       0,     0,   -67,     0,    31,     0,     0,     0,     0,    33,
       0,     0,    34,    35,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,     0,   222,   223,
      36,    37,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,     0,    65,
     195,     0,    66,   -45,     0,    67,     2,     3,     4,   106,
       0,     6,     7,     0,     0,     0,     0,     0,     0,     0,
      13,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    21,     0,    23,     0,    25,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    33,     0,     0,    34,
      35,     0,   139,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,     0,   222,   223,   107,   108,     0,
       0,     0,     0,     0,   109,     0,     0,     0,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,   110,    57,    58,    59,
      60,    61,    62,    63,    64,     0,    65,   195,     0,    66,
       0,     0,    67,     0,   140,     2,     3,     4,   106,     0,
       6,     7,     0,     0,     0,     0,     0,     0,     0,    13,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    21,     0,    23,     0,    25,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    33,     0,     0,    34,    35,
       0,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,     0,   222,   223,     0,     0,   107,   108,     0,     0,
       0,     0,     0,   109,     0,     0,     0,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,   110,    57,    58,    59,    60,
      61,    62,    63,    64,     0,    65,   195,     0,    66,     0,
       0,    67,     0,   252,     2,     3,     4,   106,     0,     6,
       7,     0,     0,     8,     0,     0,    11,    12,    13,    14,
       0,     0,    16,    17,     0,    18,     0,     0,     0,     0,
       0,    19,     0,    21,     0,    23,    24,    25,    26,     0,
       0,     0,    27,     0,     0,     0,     0,     0,     0,    31,
       0,     0,     0,     0,    33,     0,     0,    34,    35,     0,
     -68,   -68,   -68,   -68,   -68,   -68,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
       0,   222,   223,     0,     0,    36,    37,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,     0,    65,     0,     0,    66,     0,     0,
      67,   410,     2,     3,     4,   106,     0,     6,     7,     0,
       0,     0,     0,     0,     0,     0,    13,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    21,     0,    23,     0,    25,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    33,     0,     0,    34,    35,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   107,   108,     0,     0,     0,     0,     0,
     109,     0,     0,     0,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,   110,    57,    58,    59,    60,    61,    62,    63,
      64,     0,    65,     0,     0,    66,     0,     0,    67,   148,
       2,     3,     4,     5,     0,     6,     7,     0,     0,     8,
       9,    10,    11,    12,    13,    14,    15,     0,    16,    17,
       0,    18,     0,     0,     0,     0,     0,    19,    20,    21,
      22,    23,    24,    25,    26,     0,     0,     0,    27,     0,
      28,    29,     0,   -67,    30,    31,    32,     0,     0,     0,
      33,     0,     0,    34,    35,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    36,    37,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,     0,
      65,     0,     0,    66,     0,     0,    67,     2,     3,     4,
       5,     0,     6,     7,     0,     0,     8,     9,   154,    11,
      12,    13,    14,    15,     0,    16,    17,     0,    18,     0,
       0,     0,     0,     0,    19,    20,    21,    22,    23,    24,
      25,    26,     0,     0,     0,    27,     0,    28,    29,     0,
       0,    30,    31,    32,     0,     0,     0,    33,     0,     0,
      34,    35,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    36,    37,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,     0,    65,     0,     0,
      66,     0,     0,    67,     2,     3,     4,     5,     0,     6,
       7,     0,     0,     8,     9,     0,    11,    12,    13,    14,
      15,     0,    16,    17,     0,    18,     0,     0,     0,     0,
       0,    19,    20,    21,    22,    23,    24,    25,    26,     0,
       0,     0,    27,     0,    28,    29,     0,     0,    30,    31,
      32,     0,     0,     0,    33,     0,     0,    34,    35,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    36,    37,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,     0,    65,     0,     0,    66,     0,     0,
      67,     2,     3,     4,     5,     0,     6,     7,     0,     0,
       8,     0,     0,    11,    12,    13,    14,    15,     0,    16,
      17,     0,    18,     0,     0,     0,     0,     0,    19,    20,
      21,    22,    23,    24,    25,    26,     0,     0,     0,    27,
       0,    28,    29,     0,     0,    30,    31,    32,     0,     0,
       0,    33,     0,     0,    34,    35,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    36,    37,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
       0,    65,     0,     0,    66,     0,     0,    67,     2,     3,
       4,   106,     0,     6,     7,     0,     0,     8,     0,     0,
      11,    12,    13,    14,     0,     0,    16,    17,     0,    18,
       0,     0,     0,     0,     0,    19,     0,    21,     0,    23,
      24,    25,    26,     0,     0,     0,    27,     0,     0,     0,
       0,     0,     0,    31,     0,     0,     0,     0,    33,     0,
       0,    34,    35,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    36,
      37,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,     0,    65,     0,
       0,    66,     0,     0,    67,     2,     3,     4,   106,     0,
       6,     7,     0,     0,     8,     0,     0,    11,    12,    13,
      14,     0,     0,    16,    17,     0,     0,     0,     0,     0,
       0,     0,    19,     0,    21,     0,    23,    24,    25,    26,
       0,     0,     0,    27,     0,     0,     0,     0,   -67,     0,
      31,     0,     0,     0,     0,    33,     0,     0,    34,    35,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    36,    37,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,     0,    65,     0,     0,    66,     0,
       0,    67,     2,     3,     4,   106,     0,     6,     7,     0,
       0,     8,     0,     0,    11,    12,    13,    14,     0,     0,
      16,    17,     0,     0,     0,     0,     0,     0,     0,    19,
       0,    21,     0,    23,    24,    25,    26,     0,     0,     0,
      27,     0,     0,     0,     0,     0,     0,    31,     0,     0,
       0,     0,    33,     0,     0,    34,    35,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    36,    37,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,     0,    65,     0,     0,    66,     0,     0,    67,     2,
       3,     4,   106,     0,     6,     7,     0,     0,     8,     0,
       0,    11,    12,    13,    14,     0,     0,     0,    17,     0,
       0,     0,     0,     0,     0,     0,    19,     0,    21,     0,
      23,    24,    25,     0,     0,     0,     0,    27,     0,     0,
       0,     0,     0,     0,    31,     0,     0,     0,     0,    33,
       0,     0,    34,    35,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      36,    37,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,     0,    65,
       0,     0,    66,     0,     0,    67,     2,     3,     4,   106,
       0,     6,     7,     0,     0,     8,     0,     0,     0,     0,
      13,    14,     0,     0,     0,    17,     0,     0,     0,     0,
       0,     0,     0,    19,     0,    21,     0,    23,    24,    25,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   224,
       0,    31,     0,     0,     0,     0,    33,     0,     0,    34,
      35,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    36,    37,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,     0,    65,     0,     0,    66,
       0,     0,    67,     2,     3,     4,   106,     0,     6,     7,
       0,     0,     8,     0,     0,     0,     0,    13,    14,     0,
       0,     0,    17,     0,     0,     0,     0,     0,     0,     0,
      19,     0,    21,     0,    23,    24,    25,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   226,     0,    31,     0,
       0,     0,     0,    33,     0,     0,    34,    35,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    36,    37,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,     0,    65,     0,     0,    66,     0,     0,    67,
       2,     3,     4,   106,     0,     6,     7,     0,     0,     8,
       0,     0,     0,     0,    13,    14,     0,     0,     0,    17,
       0,     0,     0,     0,     0,     0,     0,    19,     0,    21,
       0,    23,    24,    25,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    31,     0,     0,     0,     0,
      33,     0,     0,    34,    35,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    36,    37,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,     0,
      65,     0,     0,    66,     0,     0,    67,     2,     3,     4,
     106,     0,     6,     7,     0,     0,     0,     0,     0,     0,
       0,    13,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    21,     0,    23,     0,
      25,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    33,     0,     0,
      34,    35,   135,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   107,   108,
       0,     0,     0,     0,     0,   109,     0,     0,     0,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,   110,    57,    58,
      59,    60,    61,    62,    63,    64,     0,    65,     0,     0,
      66,     0,     0,    67,     2,     3,     4,   106,     0,     6,
       7,     0,     0,     0,     0,     0,     0,     0,    13,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    21,     0,    23,     0,    25,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    33,     0,     0,    34,    35,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   107,   108,     0,     0,     0,
       0,     0,   109,     0,     0,     0,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,   110,    57,    58,    59,    60,    61,
      62,    63,    64,     0,    65,     0,     0,    66,   184,     0,
      67,     2,     3,     4,   106,     0,     6,     7,     0,     0,
       0,     0,     0,     0,     0,    13,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      21,     0,    23,     0,    25,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    33,     0,     0,    34,    35,     0,   381,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   107,   108,     0,     0,     0,     0,     0,   109,
       0,     0,     0,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,   110,    57,    58,    59,    60,    61,    62,    63,    64,
       0,    65,     0,     0,    66,     0,     0,    67,     2,     3,
       4,   106,     0,     6,     7,     0,     0,     0,     0,     0,
       0,     0,    13,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    21,     0,    23,
       0,    25,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    33,     0,
       0,    34,    35,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   107,
     108,     0,     0,     0,     0,     0,   109,     0,     0,     0,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,   110,    57,
      58,    59,    60,    61,    62,    63,    64,     0,    65,     0,
       0,    66,     0,     0,    67,     2,     3,     4,   106,     0,
       6,     7,     0,     0,     0,     0,     0,     0,     0,    13,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    21,     0,    23,     0,    25,     0,
       2,     3,     4,   106,     0,     6,     7,     0,     0,     0,
       0,     0,     0,     0,    13,    33,     0,     0,    34,    35,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    21,
       0,    23,     0,    25,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    36,    37,     0,     0,
      33,     0,     0,    34,    35,     0,     0,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,   195,    65,     0,     0,    66,     0,
       0,    67,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
       0,    57,    58,    59,    60,    61,    62,    63,    64,   195,
      65,     0,     0,    66,     0,     0,    67,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   221,     0,   222,
     223,     0,     0,     0,     0,     0,     0,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   195,   222,   223,     0,     0,   261,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   195,     0,     0,     0,     0,     0,     0,   390,
     391,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,     0,   222,   223,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   195,   222,   223,     0,
       0,     0,     0,     0,   383,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   195,     0,     0,     0,     0,
       0,     0,     0,   395,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
       0,   222,   223,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   195,
     222,   223,     0,     0,     0,     0,   259,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   195,     0,
       0,     0,   330,     0,     0,   444,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,     0,   222,   223,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   195,   222,   223,     0,     0,     0,     0,   446,
       0,   331,   332,   333,   334,   335,   336,   337,   338,   339,
     340,   341,   342,   343,   344,   345,   346,   347,   348,   349,
     350,   351,   352,   353,   354,   355,   356,   357,   447,   358,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   195,   222,   223,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   195,     0,     0,     0,     0,
       0,     0,   448,     0,     0,     0,     0,     0,     0,   234,
       0,     0,     0,     0,     0,     0,   454,     0,     0,     0,
       0,     0,     0,     0,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     195,   222,   223,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   195,
     222,   223,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   455,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,     0,   222,   223,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,     0,   222,   223,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,     0,   222,   223
};

static const yytype_int16 yycheck[] =
{
      18,    34,    83,    21,    20,    27,   126,   125,   129,   125,
      28,   140,   133,    56,    33,   231,    34,    71,    10,    11,
       7,   371,     7,    59,   124,     7,    41,    59,     6,   124,
      56,    56,    75,   128,    67,   130,    38,    39,    26,    27,
      42,    11,    12,     8,     7,   129,    40,   101,    66,    67,
       6,    93,     6,    56,   126,     0,   124,    46,   125,    56,
     126,   126,   126,   125,     7,    93,    75,    58,   128,   102,
       6,   104,    91,    92,    93,    94,    95,    96,   130,    40,
      99,    58,   432,   126,    55,   103,   127,    40,    40,   107,
     108,   109,   128,   128,    75,   117,   118,     6,   152,   131,
     126,    58,   125,   453,   120,   126,   322,    92,    93,   125,
      92,    93,   228,    93,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,   146,    92,
      93,   131,   131,   126,    58,    93,   128,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,     7,    92,
      93,   127,   127,    40,   187,   236,   128,    75,     6,   375,
     241,   179,   378,    77,    75,    58,   247,    55,   128,    77,
      77,   130,     6,   191,     7,   388,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   192,   128,   127,   262,     9,
     131,   152,   228,   400,   261,   418,   234,   366,   192,    -1,
      -1,    -1,    -1,    -1,   257,   243,   244,    86,    87,    88,
      89,   249,    -1,    92,    93,    -1,    -1,    -1,   359,    -1,
     259,     7,    -1,    -1,   266,    -1,   264,    -1,   366,    -1,
     366,   380,    -1,    86,    87,    88,    89,    -1,     7,    92,
      93,    -1,     0,    -1,    -1,   283,    -1,    -1,    -1,    -1,
      -1,     9,   388,    -1,    -1,     7,    -1,    -1,    -1,    -1,
      -1,    -1,    20,   403,   400,    -1,    -1,    -1,    -1,    27,
      -1,    -1,    -1,    -1,    -1,    -1,   318,   319,    36,    37,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   326,    -1,
      -1,    -1,   328,    -1,    -1,    36,    37,    83,    84,    85,
      86,    87,    88,    89,   405,    -1,    92,    93,    -1,    -1,
      -1,    -1,    -1,    71,    83,    84,    85,    86,    87,    88,
      89,    -1,    -1,    92,    93,    -1,   379,    -1,    -1,    -1,
     366,    83,    84,    85,    86,    87,    88,    89,    -1,    -1,
      92,    93,    -1,   101,   445,   383,    -1,    -1,    -1,    -1,
      -1,    -1,   388,   391,    -1,   393,    -1,   395,    -1,   117,
     118,    -1,   120,    -1,   400,    -1,    -1,   125,   126,   407,
      -1,   409,   130,    -1,    -1,   413,    -1,     7,    -1,   417,
      -1,    -1,   420,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     443,    -1,    -1,    -1,   152,   433,   434,    -1,    -1,   437,
      -1,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,    -1,    -1,   455,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,    -1,    -1,    -1,    -1,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    -1,    92,    93,     7,   223,    -1,    -1,    -1,    -1,
     228,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     0,     1,    -1,     3,     4,     5,     6,   246,     8,
       9,    -1,    -1,    12,    13,    14,    15,    16,    17,    18,
      19,   131,    21,    22,   262,    24,    -1,    -1,   266,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    37,    -1,
      -1,    -1,    41,    -1,    43,    44,    -1,    46,    47,    48,
      49,    -1,    -1,    -1,    53,    -1,    -1,    56,    57,    -1,
      83,    84,    85,    86,    87,    88,    89,    -1,    -1,    92,
      93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     318,   319,    -1,    -1,    -1,    84,    85,    -1,    -1,    -1,
     328,    -1,    -1,    -1,    -1,    -1,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,    -1,   123,    -1,    -1,   126,   366,    -1,
     129,    -1,    -1,     7,    -1,    -1,    -1,     1,    -1,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    12,    -1,
     388,    15,    16,    17,    18,    -1,    -1,    21,    22,    -1,
      -1,    -1,   400,    -1,    -1,   403,    30,    -1,    32,    -1,
      34,    35,    36,    37,    -1,    -1,    -1,    41,    -1,    -1,
      -1,    -1,    46,    -1,    48,    -1,    -1,    -1,    -1,    53,
      -1,    -1,    56,    57,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    -1,    92,    93,
      84,    85,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,    -1,   123,
       7,    -1,   126,   127,    -1,   129,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    32,    -1,    34,    -1,    36,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    53,    -1,    -1,    56,
      57,    -1,    59,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    -1,    92,    93,    84,    85,    -1,
      -1,    -1,    -1,    -1,    91,    -1,    -1,    -1,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,    -1,   123,     7,    -1,   126,
      -1,    -1,   129,    -1,   131,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    17,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    32,    -1,    34,    -1,    36,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    53,    -1,    -1,    56,    57,
      -1,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    -1,    92,    93,    -1,    -1,    84,    85,    -1,    -1,
      -1,    -1,    -1,    91,    -1,    -1,    -1,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,    -1,   123,     7,    -1,   126,    -1,
      -1,   129,    -1,   131,     3,     4,     5,     6,    -1,     8,
       9,    -1,    -1,    12,    -1,    -1,    15,    16,    17,    18,
      -1,    -1,    21,    22,    -1,    24,    -1,    -1,    -1,    -1,
      -1,    30,    -1,    32,    -1,    34,    35,    36,    37,    -1,
      -1,    -1,    41,    -1,    -1,    -1,    -1,    -1,    -1,    48,
      -1,    -1,    -1,    -1,    53,    -1,    -1,    56,    57,    -1,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      -1,    92,    93,    -1,    -1,    84,    85,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,    -1,   123,    -1,    -1,   126,    -1,    -1,
     129,   130,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    17,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    32,    -1,    34,    -1,    36,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    53,    -1,    -1,    56,    57,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    85,    -1,    -1,    -1,    -1,    -1,
      91,    -1,    -1,    -1,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,    -1,   123,    -1,    -1,   126,    -1,    -1,   129,   130,
       3,     4,     5,     6,    -1,     8,     9,    -1,    -1,    12,
      13,    14,    15,    16,    17,    18,    19,    -1,    21,    22,
      -1,    24,    -1,    -1,    -1,    -1,    -1,    30,    31,    32,
      33,    34,    35,    36,    37,    -1,    -1,    -1,    41,    -1,
      43,    44,    -1,    46,    47,    48,    49,    -1,    -1,    -1,
      53,    -1,    -1,    56,    57,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    84,    85,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,    -1,
     123,    -1,    -1,   126,    -1,    -1,   129,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    12,    13,    14,    15,
      16,    17,    18,    19,    -1,    21,    22,    -1,    24,    -1,
      -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,    35,
      36,    37,    -1,    -1,    -1,    41,    -1,    43,    44,    -1,
      -1,    47,    48,    49,    -1,    -1,    -1,    53,    -1,    -1,
      56,    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,    -1,   123,    -1,    -1,
     126,    -1,    -1,   129,     3,     4,     5,     6,    -1,     8,
       9,    -1,    -1,    12,    13,    -1,    15,    16,    17,    18,
      19,    -1,    21,    22,    -1,    24,    -1,    -1,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    37,    -1,
      -1,    -1,    41,    -1,    43,    44,    -1,    -1,    47,    48,
      49,    -1,    -1,    -1,    53,    -1,    -1,    56,    57,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    84,    85,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,    -1,   123,    -1,    -1,   126,    -1,    -1,
     129,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      12,    -1,    -1,    15,    16,    17,    18,    19,    -1,    21,
      22,    -1,    24,    -1,    -1,    -1,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    37,    -1,    -1,    -1,    41,
      -1,    43,    44,    -1,    -1,    47,    48,    49,    -1,    -1,
      -1,    53,    -1,    -1,    56,    57,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    84,    85,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
      -1,   123,    -1,    -1,   126,    -1,    -1,   129,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    12,    -1,    -1,
      15,    16,    17,    18,    -1,    -1,    21,    22,    -1,    24,
      -1,    -1,    -1,    -1,    -1,    30,    -1,    32,    -1,    34,
      35,    36,    37,    -1,    -1,    -1,    41,    -1,    -1,    -1,
      -1,    -1,    -1,    48,    -1,    -1,    -1,    -1,    53,    -1,
      -1,    56,    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      85,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,    -1,   123,    -1,
      -1,   126,    -1,    -1,   129,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    12,    -1,    -1,    15,    16,    17,
      18,    -1,    -1,    21,    22,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    30,    -1,    32,    -1,    34,    35,    36,    37,
      -1,    -1,    -1,    41,    -1,    -1,    -1,    -1,    46,    -1,
      48,    -1,    -1,    -1,    -1,    53,    -1,    -1,    56,    57,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    84,    85,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,    -1,   123,    -1,    -1,   126,    -1,
      -1,   129,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    12,    -1,    -1,    15,    16,    17,    18,    -1,    -1,
      21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    30,
      -1,    32,    -1,    34,    35,    36,    37,    -1,    -1,    -1,
      41,    -1,    -1,    -1,    -1,    -1,    -1,    48,    -1,    -1,
      -1,    -1,    53,    -1,    -1,    56,    57,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    85,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,    -1,   123,    -1,    -1,   126,    -1,    -1,   129,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    12,    -1,
      -1,    15,    16,    17,    18,    -1,    -1,    -1,    22,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    30,    -1,    32,    -1,
      34,    35,    36,    -1,    -1,    -1,    -1,    41,    -1,    -1,
      -1,    -1,    -1,    -1,    48,    -1,    -1,    -1,    -1,    53,
      -1,    -1,    56,    57,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      84,    85,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,    -1,   123,
      -1,    -1,   126,    -1,    -1,   129,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    12,    -1,    -1,    -1,    -1,
      17,    18,    -1,    -1,    -1,    22,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    30,    -1,    32,    -1,    34,    35,    36,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      -1,    48,    -1,    -1,    -1,    -1,    53,    -1,    -1,    56,
      57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,    -1,   123,    -1,    -1,   126,
      -1,    -1,   129,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    12,    -1,    -1,    -1,    -1,    17,    18,    -1,
      -1,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      30,    -1,    32,    -1,    34,    35,    36,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,    48,    -1,
      -1,    -1,    -1,    53,    -1,    -1,    56,    57,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    85,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,    -1,   123,    -1,    -1,   126,    -1,    -1,   129,
       3,     4,     5,     6,    -1,     8,     9,    -1,    -1,    12,
      -1,    -1,    -1,    -1,    17,    18,    -1,    -1,    -1,    22,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    30,    -1,    32,
      -1,    34,    35,    36,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    48,    -1,    -1,    -1,    -1,
      53,    -1,    -1,    56,    57,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    84,    85,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,    -1,
     123,    -1,    -1,   126,    -1,    -1,   129,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    32,    -1,    34,    -1,
      36,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,    -1,    -1,
      56,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,
      -1,    -1,    -1,    -1,    -1,    91,    -1,    -1,    -1,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,    -1,   123,    -1,    -1,
     126,    -1,    -1,   129,     3,     4,     5,     6,    -1,     8,
       9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    17,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    32,    -1,    34,    -1,    36,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    53,    -1,    -1,    56,    57,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    84,    85,    -1,    -1,    -1,
      -1,    -1,    91,    -1,    -1,    -1,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,    -1,   123,    -1,    -1,   126,   127,    -1,
     129,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    17,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      32,    -1,    34,    -1,    36,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    53,    -1,    -1,    56,    57,    -1,    59,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    84,    85,    -1,    -1,    -1,    -1,    -1,    91,
      -1,    -1,    -1,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
      -1,   123,    -1,    -1,   126,    -1,    -1,   129,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,    -1,    34,
      -1,    36,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,    -1,
      -1,    56,    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      85,    -1,    -1,    -1,    -1,    -1,    91,    -1,    -1,    -1,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,    -1,   123,    -1,
      -1,   126,    -1,    -1,   129,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    17,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    32,    -1,    34,    -1,    36,    -1,
       3,     4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    17,    53,    -1,    -1,    56,    57,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,
      -1,    34,    -1,    36,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    84,    85,    -1,    -1,
      53,    -1,    -1,    56,    57,    -1,    -1,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,     7,   123,    -1,    -1,   126,    -1,
      -1,   129,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
      -1,   114,   115,   116,   117,   118,   119,   120,   121,     7,
     123,    -1,    -1,   126,    -1,    -1,   129,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    -1,    92,
      93,    -1,    -1,    -1,    -1,    -1,    -1,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,     7,    92,    93,    -1,    -1,   131,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,   127,
     128,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    -1,    92,    93,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,     7,    92,    93,    -1,
      -1,    -1,    -1,    -1,   128,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   128,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      -1,    92,    93,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,     7,
      92,    93,    -1,    -1,    -1,    -1,   127,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,
      -1,    -1,     6,    -1,    -1,   127,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    -1,    92,    93,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,     7,    92,    93,    -1,    -1,    -1,    -1,   127,
      -1,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   127,   123,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,     7,    92,    93,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,
      -1,    -1,   127,    -1,    -1,    -1,    -1,    -1,    -1,    50,
      -1,    -1,    -1,    -1,    -1,    -1,    28,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
       7,    92,    93,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,     7,
      92,    93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    50,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    -1,    92,    93,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    -1,    92,    93,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    -1,    92,    93
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     4,     5,     6,     8,     9,    12,    13,
      14,    15,    16,    17,    18,    19,    21,    22,    24,    30,
      31,    32,    33,    34,    35,    36,    37,    41,    43,    44,
      47,    48,    49,    53,    56,    57,    84,    85,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   123,   126,   129,   133,   134,
     135,   136,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   155,   158,   159,   160,   161,   164,   166,
     167,   170,   171,   178,   179,   182,   183,   185,   186,   187,
     189,   124,    56,    75,   126,   169,     6,    84,    85,    91,
     113,   163,   165,   166,   168,   139,    41,   160,   160,     6,
      56,   165,   158,   165,     8,   129,    40,   161,   165,     6,
      93,   193,   194,     6,   169,    58,   162,   163,   165,    59,
     131,   162,   180,   181,   164,   164,   126,   165,   130,   162,
     165,     0,   124,   137,    14,   138,   193,   125,    46,     7,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    92,    93,   169,   169,   169,   169,   169,   169,   126,
     169,   138,   162,   165,   127,   162,   188,    56,   165,   165,
     165,   126,    10,    11,   128,     7,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    92,    93,    46,   161,    46,   161,   126,   150,
     156,   157,   158,   125,    50,   143,   155,   158,   190,   192,
     159,   150,   166,    38,    39,    42,    93,   150,    58,   128,
     153,   131,   131,   181,    59,   128,    59,   131,   165,   127,
     130,   131,   136,   138,    75,     6,    40,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     166,   165,    58,    55,   127,   162,   165,   168,   165,   165,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   165,   165,   166,    40,    40,
       1,   151,   152,   154,   155,   158,    75,    58,   128,   153,
       6,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   123,   195,
     165,   193,   125,   124,   128,   130,   191,   193,   165,   165,
     166,   193,     6,   172,   173,   174,   175,   176,   177,   131,
     131,    59,   162,   128,   169,   172,   138,   165,   126,   161,
     127,   128,   165,   126,    58,   128,   161,   161,   127,   127,
     128,   153,   165,    40,   158,   150,   184,    75,     6,    77,
     130,   143,   192,    75,   149,   149,    58,    55,   128,   153,
      77,   128,   153,   181,   165,   130,   151,   165,   165,   165,
     154,   159,   193,    26,    27,   165,   165,    77,   165,   165,
     175,   165,     6,   131,   127,   127,   127,   127,   127,   149,
     165,   165,   165,   193,    28,    50,   149,   165
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
        if ((yyvsp[(1) - (1)].item)) pp->model->addItem((yyvsp[(1) - (1)].item));
      ;}
    break;

  case 6:

    {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[(2) - (2)].item)) pp->model->addItem((yyvsp[(2) - (2)].item));
      ;}
    break;

  case 7:

    {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[(3) - (3)].item)) pp->model->addItem((yyvsp[(3) - (3)].item));
      ;}
    break;

  case 8:

    {
        ParserState* pp = static_cast<ParserState*>(parm);
        if ((yyvsp[(4) - (4)].item)) pp->model->addItem((yyvsp[(4) - (4)].item));
      ;}
    break;

  case 10:

    {
        ParserState* pp = static_cast<ParserState*>(parm);
        if (pp->parseDocComments && (yyvsp[(1) - (1)].sValue)) {
          pp->model->addDocComment((yyvsp[(1) - (1)].sValue));
        }
        free((yyvsp[(1) - (1)].sValue));
      ;}
    break;

  case 11:

    {
        ParserState* pp = static_cast<ParserState*>(parm);
        if (pp->parseDocComments && (yyvsp[(2) - (2)].sValue)) {
          pp->model->addDocComment((yyvsp[(2) - (2)].sValue));
        }
        free((yyvsp[(2) - (2)].sValue));
      ;}
    break;

  case 14:

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

  case 15:

    { (yyval.item) = (yyvsp[(1) - (1)].item); ;}
    break;

  case 16:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"include") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 17:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"variable declaration") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 19:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"constraint") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 20:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"solve") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 21:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"output") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 22:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"predicate") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 23:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"predicate") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 24:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"annotation") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 25:

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

  case 26:

    { if ((yyvsp[(1) - (2)].vardeclexpr) && (yyvsp[(2) - (2)].expression_v)) (yyvsp[(1) - (2)].vardeclexpr)->addAnnotations(*(yyvsp[(2) - (2)].expression_v));
        (yyval.item) = new VarDeclI((yyloc),(yyvsp[(1) - (2)].vardeclexpr));
        delete (yyvsp[(2) - (2)].expression_v);
      ;}
    break;

  case 27:

    { if ((yyvsp[(1) - (4)].vardeclexpr)) (yyvsp[(1) - (4)].vardeclexpr)->e((yyvsp[(4) - (4)].expression));
        if ((yyvsp[(1) - (4)].vardeclexpr) && (yyvsp[(2) - (4)].expression_v)) (yyvsp[(1) - (4)].vardeclexpr)->addAnnotations(*(yyvsp[(2) - (4)].expression_v));
        (yyval.item) = new VarDeclI((yyloc),(yyvsp[(1) - (4)].vardeclexpr));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 28:

    { (yyval.item) = new AssignI((yyloc),(yyvsp[(1) - (3)].sValue),(yyvsp[(3) - (3)].expression));
        free((yyvsp[(1) - (3)].sValue));
      ;}
    break;

  case 29:

    { (yyval.item) = new ConstraintI((yyloc),(yyvsp[(2) - (2)].expression));;}
    break;

  case 30:

    { (yyval.item) = SolveI::sat((yyloc));
        if ((yyval.item) && (yyvsp[(2) - (3)].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[(2) - (3)].expression_v));
        delete (yyvsp[(2) - (3)].expression_v);
      ;}
    break;

  case 31:

    { (yyval.item) = SolveI::min((yyloc),(yyvsp[(4) - (4)].expression));
        if ((yyval.item) && (yyvsp[(2) - (4)].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[(2) - (4)].expression_v));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 32:

    { (yyval.item) = SolveI::max((yyloc),(yyvsp[(4) - (4)].expression));
        if ((yyval.item) && (yyvsp[(2) - (4)].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[(2) - (4)].expression_v));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 33:

    { (yyval.item) = new OutputI((yyloc),(yyvsp[(2) - (2)].expression));;}
    break;

  case 34:

    { (yyval.item) = new FunctionI((yyloc),(yyvsp[(2) - (5)].sValue),new TypeInst((yyloc),
                           Type::varbool()),*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression));
        if ((yyval.item) && (yyvsp[(4) - (5)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(4) - (5)].expression_v));
        free((yyvsp[(2) - (5)].sValue));
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
        delete (yyvsp[(4) - (5)].expression_v);
      ;}
    break;

  case 35:

    { (yyval.item) = new FunctionI((yyloc),(yyvsp[(2) - (5)].sValue),new TypeInst((yyloc),
                           Type::parbool()),*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression));
        if ((yyval.item) && (yyvsp[(4) - (5)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(4) - (5)].expression_v));
        free((yyvsp[(2) - (5)].sValue));
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
        delete (yyvsp[(4) - (5)].expression_v);
      ;}
    break;

  case 36:

    { (yyval.item) = new FunctionI((yyloc),(yyvsp[(4) - (7)].sValue),(yyvsp[(2) - (7)].tiexpr),*(yyvsp[(5) - (7)].vardeclexpr_v),(yyvsp[(7) - (7)].expression));
        if ((yyval.item) && (yyvsp[(6) - (7)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(6) - (7)].expression_v));
        free((yyvsp[(4) - (7)].sValue));
        delete (yyvsp[(5) - (7)].vardeclexpr_v);
        delete (yyvsp[(6) - (7)].expression_v);
      ;}
    break;

  case 37:

    { (yyval.item) = new FunctionI((yyloc),(yyvsp[(3) - (8)].sValue),(yyvsp[(1) - (8)].tiexpr),*(yyvsp[(5) - (8)].vardeclexpr_v),(yyvsp[(8) - (8)].expression));
        if ((yyval.item) && (yyvsp[(7) - (8)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(7) - (8)].expression_v));
        free((yyvsp[(3) - (8)].sValue));
        delete (yyvsp[(5) - (8)].vardeclexpr_v);
        delete (yyvsp[(7) - (8)].expression_v);
      ;}
    break;

  case 38:

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

  case 39:

    { TypeInst* ti=new TypeInst((yylsp[(1) - (5)]),Type::ann());
        (yyval.item) = new FunctionI((yyloc),(yyvsp[(2) - (5)].sValue),ti,*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression));
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
      ;}
    break;

  case 40:

    { (yyval.expression)=NULL; ;}
    break;

  case 41:

    { (yyval.expression)=(yyvsp[(2) - (2)].expression); ;}
    break;

  case 42:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); ;}
    break;

  case 43:

    { (yyval.vardeclexpr_v)=(yyvsp[(2) - (3)].vardeclexpr_v); ;}
    break;

  case 44:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); ;}
    break;

  case 45:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); ;}
    break;

  case 46:

    { (yyval.vardeclexpr_v)=(yyvsp[(1) - (2)].vardeclexpr_v); ;}
    break;

  case 47:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>();
        if ((yyvsp[(1) - (1)].vardeclexpr)) (yyvsp[(1) - (1)].vardeclexpr)->toplevel(false);
        if ((yyvsp[(1) - (1)].vardeclexpr)) (yyval.vardeclexpr_v)->push_back((yyvsp[(1) - (1)].vardeclexpr)); ;}
    break;

  case 48:

    { (yyval.vardeclexpr_v)=(yyvsp[(1) - (3)].vardeclexpr_v);
        if ((yyvsp[(3) - (3)].vardeclexpr)) (yyvsp[(3) - (3)].vardeclexpr)->toplevel(false);
        if ((yyvsp[(1) - (3)].vardeclexpr_v) && (yyvsp[(3) - (3)].vardeclexpr)) (yyvsp[(1) - (3)].vardeclexpr_v)->push_back((yyvsp[(3) - (3)].vardeclexpr)); ;}
    break;

  case 51:

    { (yyval.vardeclexpr)=(yyvsp[(1) - (1)].vardeclexpr); ;}
    break;

  case 52:

    { (yyval.vardeclexpr)=new VarDecl((yyloc), (yyvsp[(1) - (1)].tiexpr), ""); ;}
    break;

  case 53:

    { (yyval.vardeclexpr) = new VarDecl((yyloc), (yyvsp[(1) - (3)].tiexpr), (yyvsp[(3) - (3)].sValue));
        free((yyvsp[(3) - (3)].sValue));
      ;}
    break;

  case 54:

    { (yyval.tiexpr_v)=(yyvsp[(1) - (2)].tiexpr_v); ;}
    break;

  case 55:

    { (yyval.tiexpr_v)=new vector<TypeInst*>(); (yyval.tiexpr_v)->push_back((yyvsp[(1) - (1)].tiexpr)); ;}
    break;

  case 56:

    { (yyval.tiexpr_v)=(yyvsp[(1) - (3)].tiexpr_v); if ((yyvsp[(1) - (3)].tiexpr_v) && (yyvsp[(3) - (3)].tiexpr)) (yyvsp[(1) - (3)].tiexpr_v)->push_back((yyvsp[(3) - (3)].tiexpr)); ;}
    break;

  case 58:

    {
        (yyval.tiexpr) = (yyvsp[(6) - (6)].tiexpr);
        if ((yyval.tiexpr) && (yyvsp[(3) - (6)].tiexpr_v)) (yyval.tiexpr)->setRanges(*(yyvsp[(3) - (6)].tiexpr_v));
        delete (yyvsp[(3) - (6)].tiexpr_v);
      ;}
    break;

  case 59:

    {
        (yyval.tiexpr) = (yyvsp[(3) - (3)].tiexpr);
        std::vector<TypeInst*> ti(1);
        ti[0] = new TypeInst((yyloc),Type::parint());
        if ((yyval.tiexpr)) (yyval.tiexpr)->setRanges(ti);
      ;}
    break;

  case 60:

    { (yyval.tiexpr) = (yyvsp[(1) - (1)].tiexpr);
      ;}
    break;

  case 61:

    { (yyval.tiexpr) = (yyvsp[(2) - (2)].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 62:

    { (yyval.tiexpr) = (yyvsp[(3) - (3)].tiexpr);
        if ((yyval.tiexpr) && (yyvsp[(2) - (3)].bValue)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 63:

    { (yyval.tiexpr) = (yyvsp[(3) - (3)].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ti(Type::TI_VAR);
          if ((yyvsp[(2) - (3)].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 64:

    { (yyval.tiexpr) = (yyvsp[(4) - (4)].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.st(Type::ST_SET);
          if ((yyvsp[(1) - (4)].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 65:

    { (yyval.tiexpr) = (yyvsp[(5) - (5)].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.st(Type::ST_SET);
          if ((yyvsp[(2) - (5)].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 66:

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

  case 67:

    { (yyval.bValue) = false; ;}
    break;

  case 68:

    { (yyval.bValue) = true; ;}
    break;

  case 69:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parint()); ;}
    break;

  case 70:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parbool()); ;}
    break;

  case 71:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parfloat()); ;}
    break;

  case 72:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parstring()); ;}
    break;

  case 73:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::ann()); ;}
    break;

  case 74:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type(),(yyvsp[(1) - (1)].expression)); ;}
    break;

  case 75:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::top(),
                         new TIId((yyloc), (yyvsp[(1) - (1)].sValue)));
        free((yyvsp[(1) - (1)].sValue));
      ;}
    break;

  case 77:

    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].expression)); ;}
    break;

  case 78:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); if ((yyval.expression_v) && (yyvsp[(3) - (3)].expression)) (yyval.expression_v)->push_back((yyvsp[(3) - (3)].expression)); ;}
    break;

  case 80:

    { if ((yyvsp[(1) - (3)].expression) && (yyvsp[(3) - (3)].expression)) (yyvsp[(1) - (3)].expression)->addAnnotation((yyvsp[(3) - (3)].expression)); (yyval.expression)=(yyvsp[(1) - (3)].expression); ;}
    break;

  case 81:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_UNION, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 82:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 83:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SYMDIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 84:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DOTDOT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 85:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(3) - (6)].expression), BOT_DOTDOT, (yyvsp[(5) - (6)].expression)); ;}
    break;

  case 86:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_INTERSECT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 87:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUSPLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 88:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 89:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MINUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 90:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MULT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 91:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 92:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IDIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 93:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MOD, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 94:

    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=new Call((yyloc), (yyvsp[(2) - (3)].sValue), args);
        free((yyvsp[(2) - (3)].sValue));
      ;}
    break;

  case 95:

    { (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 96:

    { if ((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<IntLit>()) {
          (yyvsp[(2) - (2)].expression)->cast<IntLit>()->v(-(yyvsp[(2) - (2)].expression)->cast<IntLit>()->v());
          (yyval.expression) = (yyvsp[(2) - (2)].expression);
        } else if ((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<FloatLit>()) {
          (yyvsp[(2) - (2)].expression)->cast<FloatLit>()->v(-(yyvsp[(2) - (2)].expression)->cast<FloatLit>()->v());
          (yyval.expression) = (yyvsp[(2) - (2)].expression);
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_MINUS, (yyvsp[(2) - (2)].expression));
        }
      ;}
    break;

  case 98:

    { if ((yyvsp[(1) - (3)].expression) && (yyvsp[(3) - (3)].expression)) (yyvsp[(1) - (3)].expression)->addAnnotation((yyvsp[(3) - (3)].expression)); (yyval.expression)=(yyvsp[(1) - (3)].expression); ;}
    break;

  case 99:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_EQUIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 100:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IMPL, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 101:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_RIMPL, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 102:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_OR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 103:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_XOR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 104:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_AND, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 105:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_LE, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 106:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_GR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 107:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_LQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 108:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_GQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 109:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_EQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 110:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_NQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 111:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IN, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 112:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SUBSET, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 113:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SUPERSET, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 114:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_UNION, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 115:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 116:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SYMDIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 117:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DOTDOT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 118:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(3) - (6)].expression), BOT_DOTDOT, (yyvsp[(5) - (6)].expression)); ;}
    break;

  case 119:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_INTERSECT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 120:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUSPLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 121:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 122:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MINUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 123:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MULT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 124:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 125:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IDIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 126:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MOD, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 127:

    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=new Call((yyloc), (yyvsp[(2) - (3)].sValue), args);
        free((yyvsp[(2) - (3)].sValue));
      ;}
    break;

  case 128:

    { (yyval.expression)=new UnOp((yyloc), UOT_NOT, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 129:

    { if (((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<IntLit>()) || ((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<FloatLit>())) {
          (yyval.expression) = (yyvsp[(2) - (2)].expression);
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[(2) - (2)].expression));
        }
      ;}
    break;

  case 130:

    { if ((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<IntLit>()) {
          (yyvsp[(2) - (2)].expression)->cast<IntLit>()->v(-(yyvsp[(2) - (2)].expression)->cast<IntLit>()->v());
          (yyval.expression) = (yyvsp[(2) - (2)].expression);
        } else if ((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<FloatLit>()) {
          (yyvsp[(2) - (2)].expression)->cast<FloatLit>()->v(-(yyvsp[(2) - (2)].expression)->cast<FloatLit>()->v());
          (yyval.expression) = (yyvsp[(2) - (2)].expression);
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_MINUS, (yyvsp[(2) - (2)].expression));
        }
      ;}
    break;

  case 131:

    { (yyval.expression)=(yyvsp[(2) - (3)].expression); ;}
    break;

  case 132:

    { (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(2) - (4)].expression), *(yyvsp[(4) - (4)].expression_vv)); delete (yyvsp[(4) - (4)].expression_vv); ;}
    break;

  case 133:

    { (yyval.expression)=new Id((yyloc), (yyvsp[(1) - (1)].sValue), NULL); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 134:

    { (yyval.expression)=createArrayAccess((yyloc), new Id((yylsp[(1) - (2)]),(yyvsp[(1) - (2)].sValue),NULL), *(yyvsp[(2) - (2)].expression_vv));
        free((yyvsp[(1) - (2)].sValue)); delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 135:

    { (yyval.expression)=new AnonVar((yyloc)); ;}
    break;

  case 136:

    { (yyval.expression)=createArrayAccess((yyloc), new AnonVar((yyloc)), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 137:

    { (yyval.expression)=new BoolLit((yyloc), ((yyvsp[(1) - (1)].iValue)!=0)); ;}
    break;

  case 138:

    { (yyval.expression)=new IntLit((yyloc), (yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 139:

    { (yyval.expression)=new IntLit((yyloc), IntVal::infinity); ;}
    break;

  case 140:

    { (yyval.expression)=new FloatLit((yyloc), (yyvsp[(1) - (1)].dValue)); ;}
    break;

  case 142:

    { (yyval.expression)=constants().absent; ;}
    break;

  case 144:

    { (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 146:

    { (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 148:

    { (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 150:

    { (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 152:

    { (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 154:

    { (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 157:

    { (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 158:

    { (yyval.expression)=new StringLit((yyloc), (yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 159:

    { (yyval.expression)=new BinOp((yyloc), new StringLit((yyloc), (yyvsp[(1) - (2)].sValue)), BOT_PLUSPLUS, (yyvsp[(2) - (2)].expression));
        free((yyvsp[(1) - (2)].sValue));
      ;}
    break;

  case 160:

    { (yyval.expression)=new BinOp((yyloc), new Call((yyloc), ASTString("format"), *(yyvsp[(1) - (2)].expression_v)), BOT_PLUSPLUS, new StringLit((yyloc),(yyvsp[(2) - (2)].sValue)));
        free((yyvsp[(2) - (2)].sValue));
        delete (yyvsp[(1) - (2)].expression_v);
      ;}
    break;

  case 161:

    { (yyval.expression)=new BinOp((yyloc), new Call((yyloc), ASTString("format"), *(yyvsp[(1) - (3)].expression_v)), BOT_PLUSPLUS,
                     new BinOp((yyloc), new StringLit((yyloc),(yyvsp[(2) - (3)].sValue)), BOT_PLUSPLUS, (yyvsp[(3) - (3)].expression)));
        free((yyvsp[(2) - (3)].sValue));
        delete (yyvsp[(1) - (3)].expression_v);
      ;}
    break;

  case 162:

    { (yyval.expression_vv)=new std::vector<std::vector<Expression*> >();
        if ((yyvsp[(2) - (3)].expression_v)) {
          (yyval.expression_vv)->push_back(*(yyvsp[(2) - (3)].expression_v));
          delete (yyvsp[(2) - (3)].expression_v);
        }
      ;}
    break;

  case 163:

    { (yyval.expression_vv)=(yyvsp[(1) - (4)].expression_vv);
        if ((yyval.expression_vv) && (yyvsp[(3) - (4)].expression_v)) {
          (yyval.expression_vv)->push_back(*(yyvsp[(3) - (4)].expression_v));
          delete (yyvsp[(3) - (4)].expression_v);
        }
      ;}
    break;

  case 164:

    { (yyval.expression) = new SetLit((yyloc), std::vector<Expression*>()); ;}
    break;

  case 165:

    { (yyval.expression) = new SetLit((yyloc), *(yyvsp[(2) - (3)].expression_v)); delete (yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 166:

    { (yyval.expression) = new Comprehension((yyloc), (yyvsp[(2) - (5)].expression), *(yyvsp[(4) - (5)].generators), true);
        delete (yyvsp[(4) - (5)].generators);
      ;}
    break;

  case 167:

    { (yyval.generators)=new Generators; (yyval.generators)->_g = *(yyvsp[(1) - (1)].generator_v); (yyval.generators)->_w = NULL; delete (yyvsp[(1) - (1)].generator_v); ;}
    break;

  case 168:

    { (yyval.generators)=new Generators; (yyval.generators)->_g = *(yyvsp[(1) - (3)].generator_v); (yyval.generators)->_w = (yyvsp[(3) - (3)].expression); delete (yyvsp[(1) - (3)].generator_v); ;}
    break;

  case 170:

    { (yyval.generator_v)=new std::vector<Generator>; if ((yyvsp[(1) - (1)].generator)) (yyval.generator_v)->push_back(*(yyvsp[(1) - (1)].generator)); delete (yyvsp[(1) - (1)].generator); ;}
    break;

  case 171:

    { (yyval.generator_v)=(yyvsp[(1) - (3)].generator_v); if ((yyvsp[(3) - (3)].generator)) (yyval.generator_v)->push_back(*(yyvsp[(3) - (3)].generator)); delete (yyvsp[(3) - (3)].generator); ;}
    break;

  case 172:

    { if ((yyvsp[(3) - (3)].expression)) (yyval.generator)=new Generator(*(yyvsp[(1) - (3)].string_v),(yyvsp[(3) - (3)].expression)); else (yyval.generator)=NULL; delete (yyvsp[(1) - (3)].string_v); ;}
    break;

  case 174:

    { (yyval.string_v)=new std::vector<std::string>; (yyval.string_v)->push_back((yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 175:

    { (yyval.string_v)=(yyvsp[(1) - (3)].string_v); (yyval.string_v)->push_back((yyvsp[(3) - (3)].sValue)); free((yyvsp[(3) - (3)].sValue)); ;}
    break;

  case 176:

    { (yyval.expression)=new ArrayLit((yyloc), std::vector<MiniZinc::Expression*>()); ;}
    break;

  case 177:

    { (yyval.expression)=new ArrayLit((yyloc), *(yyvsp[(2) - (3)].expression_v)); delete (yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 178:

    { (yyval.expression)=new ArrayLit((yyloc), std::vector<std::vector<Expression*> >()); ;}
    break;

  case 179:

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

  case 180:

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

  case 181:

    {
      if ((yyvsp[(2) - (3)].expression_vvv)) {
        std::vector<std::pair<int,int> > dims(3);
        dims[0] = std::pair<int,int>(1,(yyvsp[(2) - (3)].expression_vvv)->size());
        if ((yyvsp[(2) - (3)].expression_vvv)->size()==0) {
          dims[1] = std::pair<int,int>(1,0);
          dims[2] = std::pair<int,int>(1,0);
        } else {
          dims[1] = std::pair<int,int>(1,(*(yyvsp[(2) - (3)].expression_vvv))[0].size());
          if ((*(yyvsp[(2) - (3)].expression_vvv))[0].size()==0) {
            dims[2] = std::pair<int,int>(1,0);
          } else {
            dims[2] = std::pair<int,int>(1,(*(yyvsp[(2) - (3)].expression_vvv))[0][0].size());
          }
        }
        std::vector<Expression*> a;
        for (unsigned int i=0; i<dims[0].second; i++) {
          if ((*(yyvsp[(2) - (3)].expression_vvv))[i].size() != dims[1].second) {
            yyerror(&(yylsp[(2) - (3)]), parm, "syntax error, all sub-arrays of 3d array literal must have the same length");
          } else {
            for (unsigned int j=0; j<dims[1].second; j++) {
              if ((*(yyvsp[(2) - (3)].expression_vvv))[i][j].size() != dims[2].second) {
                yyerror(&(yylsp[(2) - (3)]), parm, "syntax error, all sub-arrays of 3d array literal must have the same length");
              } else {
                for (unsigned int k=0; k<dims[2].second; k++) {
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

  case 182:

    { (yyval.expression_vvv)=new std::vector<std::vector<std::vector<MiniZinc::Expression*> > >;
      ;}
    break;

  case 183:

    { (yyval.expression_vvv)=new std::vector<std::vector<std::vector<MiniZinc::Expression*> > >;
        (yyval.expression_vvv)->push_back(*(yyvsp[(2) - (3)].expression_vv));
        delete (yyvsp[(2) - (3)].expression_vv);
      ;}
    break;

  case 184:

    { (yyval.expression_vvv)=(yyvsp[(1) - (5)].expression_vvv);
        if ((yyval.expression_vvv)) (yyval.expression_vvv)->push_back(*(yyvsp[(4) - (5)].expression_vv));
        delete (yyvsp[(4) - (5)].expression_vv);
      ;}
    break;

  case 185:

    { (yyval.expression_vv)=new std::vector<std::vector<MiniZinc::Expression*> >;
        (yyval.expression_vv)->push_back(*(yyvsp[(1) - (1)].expression_v));
        delete (yyvsp[(1) - (1)].expression_v);
      ;}
    break;

  case 186:

    { (yyval.expression_vv)=(yyvsp[(1) - (3)].expression_vv); if ((yyval.expression_vv)) (yyval.expression_vv)->push_back(*(yyvsp[(3) - (3)].expression_v)); delete (yyvsp[(3) - (3)].expression_v); ;}
    break;

  case 187:

    { (yyval.expression)=new Comprehension((yyloc), (yyvsp[(2) - (5)].expression), *(yyvsp[(4) - (5)].generators), false);
        delete (yyvsp[(4) - (5)].generators);
      ;}
    break;

  case 188:

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

  case 189:

    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; ;}
    break;

  case 190:

    { (yyval.expression_v)=(yyvsp[(1) - (5)].expression_v); if ((yyval.expression_v)) { (yyval.expression_v)->push_back((yyvsp[(3) - (5)].expression)); (yyval.expression_v)->push_back((yyvsp[(5) - (5)].expression)); } ;}
    break;

  case 191:

    { (yyval.iValue)=BOT_EQUIV; ;}
    break;

  case 192:

    { (yyval.iValue)=BOT_IMPL; ;}
    break;

  case 193:

    { (yyval.iValue)=BOT_RIMPL; ;}
    break;

  case 194:

    { (yyval.iValue)=BOT_OR; ;}
    break;

  case 195:

    { (yyval.iValue)=BOT_XOR; ;}
    break;

  case 196:

    { (yyval.iValue)=BOT_AND; ;}
    break;

  case 197:

    { (yyval.iValue)=BOT_LE; ;}
    break;

  case 198:

    { (yyval.iValue)=BOT_GR; ;}
    break;

  case 199:

    { (yyval.iValue)=BOT_LQ; ;}
    break;

  case 200:

    { (yyval.iValue)=BOT_GQ; ;}
    break;

  case 201:

    { (yyval.iValue)=BOT_EQ; ;}
    break;

  case 202:

    { (yyval.iValue)=BOT_NQ; ;}
    break;

  case 203:

    { (yyval.iValue)=BOT_IN; ;}
    break;

  case 204:

    { (yyval.iValue)=BOT_SUBSET; ;}
    break;

  case 205:

    { (yyval.iValue)=BOT_SUPERSET; ;}
    break;

  case 206:

    { (yyval.iValue)=BOT_UNION; ;}
    break;

  case 207:

    { (yyval.iValue)=BOT_DIFF; ;}
    break;

  case 208:

    { (yyval.iValue)=BOT_SYMDIFF; ;}
    break;

  case 209:

    { (yyval.iValue)=BOT_PLUS; ;}
    break;

  case 210:

    { (yyval.iValue)=BOT_MINUS; ;}
    break;

  case 211:

    { (yyval.iValue)=BOT_MULT; ;}
    break;

  case 212:

    { (yyval.iValue)=BOT_DIV; ;}
    break;

  case 213:

    { (yyval.iValue)=BOT_IDIV; ;}
    break;

  case 214:

    { (yyval.iValue)=BOT_MOD; ;}
    break;

  case 215:

    { (yyval.iValue)=BOT_INTERSECT; ;}
    break;

  case 216:

    { (yyval.iValue)=BOT_PLUSPLUS; ;}
    break;

  case 217:

    { (yyval.iValue)=-1; ;}
    break;

  case 218:

    { if ((yyvsp[(1) - (6)].iValue)==-1) {
          (yyval.expression)=NULL;
          yyerror(&(yylsp[(3) - (6)]), parm, "syntax error, unary operator with two arguments");
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[(3) - (6)].expression),static_cast<BinOpType>((yyvsp[(1) - (6)].iValue)),(yyvsp[(5) - (6)].expression));
        }
      ;}
    break;

  case 219:

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
            (yyvsp[(3) - (4)].expression)->cast<IntLit>()->v(-(yyvsp[(3) - (4)].expression)->cast<IntLit>()->v());
          } else if (uot==UOT_MINUS && (yyvsp[(3) - (4)].expression) && (yyvsp[(3) - (4)].expression)->isa<FloatLit>()) {
            (yyvsp[(3) - (4)].expression)->cast<FloatLit>()->v(-(yyvsp[(3) - (4)].expression)->cast<FloatLit>()->v());
          } else {
            (yyval.expression)=new UnOp((yyloc), static_cast<UnOpType>(uot),(yyvsp[(3) - (4)].expression));
          }
        }
      ;}
    break;

  case 220:

    { (yyval.expression)=new Call((yyloc), (yyvsp[(1) - (3)].sValue), std::vector<Expression*>()); free((yyvsp[(1) - (3)].sValue)); ;}
    break;

  case 222:

    { 
        if ((yyvsp[(3) - (4)].expression_p)==NULL || (yyvsp[(3) - (4)].expression_p)->second) {
          yyerror(&(yylsp[(3) - (4)]), parm, "syntax error, 'where' expression outside generator call");
          (yyval.expression)=NULL;
        } else {
          (yyval.expression)=new Call((yyloc), (yyvsp[(1) - (4)].sValue), (yyvsp[(3) - (4)].expression_p)->first);
        }
        free((yyvsp[(1) - (4)].sValue));
        delete (yyvsp[(3) - (4)].expression_p);
      ;}
    break;

  case 223:

    { 
        vector<Generator> gens;
        vector<ASTString> ids;
        if ((yyvsp[(3) - (7)].expression_p)) {
          for (unsigned int i=0; i<(yyvsp[(3) - (7)].expression_p)->first.size(); i++) {
            if (Id* id = Expression::dyn_cast<Id>((yyvsp[(3) - (7)].expression_p)->first[i])) {
              ids.push_back(id->v());
            } else {
              if (BinOp* boe = Expression::dyn_cast<BinOp>((yyvsp[(3) - (7)].expression_p)->first[i])) {
                if (boe->lhs() && boe->rhs()) {
                  Id* id = Expression::dyn_cast<Id>(boe->lhs());
                  if (id && boe->op() == BOT_IN) {
                    ids.push_back(id->v());
                    gens.push_back(Generator(ids,boe->rhs()));
                    ids = vector<ASTString>();
                  } else {
                    yyerror(&(yylsp[(3) - (7)]), parm, "illegal expression in generator call");
                  }
                }
              } else {
                yyerror(&(yylsp[(3) - (7)]), parm, "illegal expression in generator call");
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
          Generators g; g._g = gens; g._w = (yyvsp[(3) - (7)].expression_p)->second;
          Comprehension* ac = new Comprehension((yyloc), (yyvsp[(6) - (7)].expression),g,false);
          vector<Expression*> args; args.push_back(ac);
          (yyval.expression)=new Call((yyloc), (yyvsp[(1) - (7)].sValue), args);
        }
        free((yyvsp[(1) - (7)].sValue));
        delete (yyvsp[(3) - (7)].expression_p);
      ;}
    break;

  case 224:

    { (yyval.expression_p)=new pair<vector<Expression*>,Expression*>;
        (yyval.expression_p)->first=*(yyvsp[(1) - (1)].expression_v); (yyval.expression_p)->second=NULL;
        delete (yyvsp[(1) - (1)].expression_v);
      ;}
    break;

  case 225:

    { (yyval.expression_p)=new pair<vector<Expression*>,Expression*>;
        (yyval.expression_p)->first=*(yyvsp[(1) - (3)].expression_v); (yyval.expression_p)->second=(yyvsp[(3) - (3)].expression);
        delete (yyvsp[(1) - (3)].expression_v);
      ;}
    break;

  case 226:

    { (yyval.expression)=new Let((yyloc), *(yyvsp[(3) - (6)].expression_v), (yyvsp[(6) - (6)].expression)); delete (yyvsp[(3) - (6)].expression_v); ;}
    break;

  case 227:

    { (yyval.expression)=new Let((yyloc), *(yyvsp[(3) - (7)].expression_v), (yyvsp[(7) - (7)].expression)); delete (yyvsp[(3) - (7)].expression_v); ;}
    break;

  case 228:

    { (yyval.expression_v)=new vector<Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].vardeclexpr)); ;}
    break;

  case 229:

    { (yyval.expression_v)=new vector<Expression*>;
        if ((yyvsp[(1) - (1)].item)) {
          ConstraintI* ce = (yyvsp[(1) - (1)].item)->cast<ConstraintI>();
          (yyval.expression_v)->push_back(ce->e());
          ce->e(NULL);
        }
      ;}
    break;

  case 230:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); (yyval.expression_v)->push_back((yyvsp[(3) - (3)].vardeclexpr)); ;}
    break;

  case 231:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v);
        if ((yyvsp[(3) - (3)].item)) {
          ConstraintI* ce = (yyvsp[(3) - (3)].item)->cast<ConstraintI>();
          (yyval.expression_v)->push_back(ce->e());
          ce->e(NULL);
        }
      ;}
    break;

  case 234:

    { (yyval.vardeclexpr) = (yyvsp[(1) - (2)].vardeclexpr);
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->toplevel(false);
        if ((yyval.vardeclexpr) && (yyvsp[(2) - (2)].expression_v)) (yyval.vardeclexpr)->addAnnotations(*(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v);
      ;}
    break;

  case 235:

    { if ((yyvsp[(1) - (4)].vardeclexpr)) (yyvsp[(1) - (4)].vardeclexpr)->e((yyvsp[(4) - (4)].expression));
        (yyval.vardeclexpr) = (yyvsp[(1) - (4)].vardeclexpr);
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->loc((yyloc));
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->toplevel(false);
        if ((yyval.vardeclexpr) && (yyvsp[(2) - (4)].expression_v)) (yyval.vardeclexpr)->addAnnotations(*(yyvsp[(2) - (4)].expression_v));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 236:

    { (yyval.expression_v)=NULL; ;}
    break;

  case 238:

    { (yyval.expression_v)=new std::vector<Expression*>(1);
        (*(yyval.expression_v))[0] = (yyvsp[(2) - (2)].expression);
      ;}
    break;

  case 239:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); if ((yyval.expression_v)) (yyval.expression_v)->push_back((yyvsp[(3) - (3)].expression)); ;}
    break;

  case 240:

    { (yyval.sValue)=(yyvsp[(1) - (1)].sValue); ;}
    break;

  case 241:

    { (yyval.sValue)=strdup("<->"); ;}
    break;

  case 242:

    { (yyval.sValue)=strdup("->"); ;}
    break;

  case 243:

    { (yyval.sValue)=strdup("<-"); ;}
    break;

  case 244:

    { (yyval.sValue)=strdup("\\/"); ;}
    break;

  case 245:

    { (yyval.sValue)=strdup("xor"); ;}
    break;

  case 246:

    { (yyval.sValue)=strdup("/\\"); ;}
    break;

  case 247:

    { (yyval.sValue)=strdup("<"); ;}
    break;

  case 248:

    { (yyval.sValue)=strdup(">"); ;}
    break;

  case 249:

    { (yyval.sValue)=strdup("<="); ;}
    break;

  case 250:

    { (yyval.sValue)=strdup(">="); ;}
    break;

  case 251:

    { (yyval.sValue)=strdup("="); ;}
    break;

  case 252:

    { (yyval.sValue)=strdup("!="); ;}
    break;

  case 253:

    { (yyval.sValue)=strdup("in"); ;}
    break;

  case 254:

    { (yyval.sValue)=strdup("subset"); ;}
    break;

  case 255:

    { (yyval.sValue)=strdup("superset"); ;}
    break;

  case 256:

    { (yyval.sValue)=strdup("union"); ;}
    break;

  case 257:

    { (yyval.sValue)=strdup("diff"); ;}
    break;

  case 258:

    { (yyval.sValue)=strdup("symdiff"); ;}
    break;

  case 259:

    { (yyval.sValue)=strdup(".."); ;}
    break;

  case 260:

    { (yyval.sValue)=strdup("+"); ;}
    break;

  case 261:

    { (yyval.sValue)=strdup("-"); ;}
    break;

  case 262:

    { (yyval.sValue)=strdup("*"); ;}
    break;

  case 263:

    { (yyval.sValue)=strdup("/"); ;}
    break;

  case 264:

    { (yyval.sValue)=strdup("div"); ;}
    break;

  case 265:

    { (yyval.sValue)=strdup("mod"); ;}
    break;

  case 266:

    { (yyval.sValue)=strdup("intersect"); ;}
    break;

  case 267:

    { (yyval.sValue)=strdup("not"); ;}
    break;

  case 268:

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



