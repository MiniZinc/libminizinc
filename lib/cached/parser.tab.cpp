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
     MZN_SVAR = 272,
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
     MZN_EQUIV = 319,
     MZN_IMPL = 320,
     MZN_RIMPL = 321,
     MZN_OR = 322,
     MZN_XOR = 323,
     MZN_AND = 324,
     MZN_LE = 325,
     MZN_GR = 326,
     MZN_LQ = 327,
     MZN_GQ = 328,
     MZN_EQ = 329,
     MZN_NQ = 330,
     MZN_IN = 331,
     MZN_SUBSET = 332,
     MZN_SUPERSET = 333,
     MZN_UNION = 334,
     MZN_DIFF = 335,
     MZN_SYMDIFF = 336,
     MZN_DOTDOT = 337,
     MZN_PLUS = 338,
     MZN_MINUS = 339,
     MZN_MULT = 340,
     MZN_DIV = 341,
     MZN_IDIV = 342,
     MZN_MOD = 343,
     MZN_INTERSECT = 344,
     MZN_NOT = 345,
     MZN_PLUSPLUS = 346,
     MZN_COLONCOLON = 347,
     PREC_ANNO = 348,
     MZN_EQUIV_QUOTED = 349,
     MZN_IMPL_QUOTED = 350,
     MZN_RIMPL_QUOTED = 351,
     MZN_OR_QUOTED = 352,
     MZN_XOR_QUOTED = 353,
     MZN_AND_QUOTED = 354,
     MZN_LE_QUOTED = 355,
     MZN_GR_QUOTED = 356,
     MZN_LQ_QUOTED = 357,
     MZN_GQ_QUOTED = 358,
     MZN_EQ_QUOTED = 359,
     MZN_NQ_QUOTED = 360,
     MZN_IN_QUOTED = 361,
     MZN_SUBSET_QUOTED = 362,
     MZN_SUPERSET_QUOTED = 363,
     MZN_UNION_QUOTED = 364,
     MZN_DIFF_QUOTED = 365,
     MZN_SYMDIFF_QUOTED = 366,
     MZN_DOTDOT_QUOTED = 367,
     MZN_PLUS_QUOTED = 368,
     MZN_MINUS_QUOTED = 369,
     MZN_MULT_QUOTED = 370,
     MZN_DIV_QUOTED = 371,
     MZN_IDIV_QUOTED = 372,
     MZN_MOD_QUOTED = 373,
     MZN_INTERSECT_QUOTED = 374,
     MZN_NOT_QUOTED = 375,
     MZN_COLONCOLON_QUOTED = 376,
     MZN_PLUSPLUS_QUOTED = 377
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
#define MZN_SVAR 272
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
#define MZN_EQUIV 319
#define MZN_IMPL 320
#define MZN_RIMPL 321
#define MZN_OR 322
#define MZN_XOR 323
#define MZN_AND 324
#define MZN_LE 325
#define MZN_GR 326
#define MZN_LQ 327
#define MZN_GQ 328
#define MZN_EQ 329
#define MZN_NQ 330
#define MZN_IN 331
#define MZN_SUBSET 332
#define MZN_SUPERSET 333
#define MZN_UNION 334
#define MZN_DIFF 335
#define MZN_SYMDIFF 336
#define MZN_DOTDOT 337
#define MZN_PLUS 338
#define MZN_MINUS 339
#define MZN_MULT 340
#define MZN_DIV 341
#define MZN_IDIV 342
#define MZN_MOD 343
#define MZN_INTERSECT 344
#define MZN_NOT 345
#define MZN_PLUSPLUS 346
#define MZN_COLONCOLON 347
#define PREC_ANNO 348
#define MZN_EQUIV_QUOTED 349
#define MZN_IMPL_QUOTED 350
#define MZN_RIMPL_QUOTED 351
#define MZN_OR_QUOTED 352
#define MZN_XOR_QUOTED 353
#define MZN_AND_QUOTED 354
#define MZN_LE_QUOTED 355
#define MZN_GR_QUOTED 356
#define MZN_LQ_QUOTED 357
#define MZN_GQ_QUOTED 358
#define MZN_EQ_QUOTED 359
#define MZN_NQ_QUOTED 360
#define MZN_IN_QUOTED 361
#define MZN_SUBSET_QUOTED 362
#define MZN_SUPERSET_QUOTED 363
#define MZN_UNION_QUOTED 364
#define MZN_DIFF_QUOTED 365
#define MZN_SYMDIFF_QUOTED 366
#define MZN_DOTDOT_QUOTED 367
#define MZN_PLUS_QUOTED 368
#define MZN_MINUS_QUOTED 369
#define MZN_MULT_QUOTED 370
#define MZN_DIV_QUOTED 371
#define MZN_IDIV_QUOTED 372
#define MZN_MOD_QUOTED 373
#define MZN_INTERSECT_QUOTED 374
#define MZN_NOT_QUOTED 375
#define MZN_COLONCOLON_QUOTED 376
#define MZN_PLUSPLUS_QUOTED 377




/* Copy the first part of user declarations.  */


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

namespace MiniZinc {

  Model* parseFromString(const string& text,
                         const string& filename,
                         const vector<string>& ip,
                         bool ignoreStdlib,
                         bool parseDocComments,
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
        if (!file.is_open()) {
          err << "Error: cannot open data file '" << f << "'." << endl;
          goto error;
        }
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
      if (!file.is_open()) {
        err << "Error: cannot open data file '" << f << "'." << endl;
        goto error;
      }
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
#define YYFINAL  148
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   3788

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  131
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  63
/* YYNRULES -- Number of rules.  */
#define YYNRULES  261
/* YYNRULES -- Number of states.  */
#define YYNSTATES  444

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   377

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     125,   126,     2,     2,   127,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   124,   123,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   128,   130,   129,     2,     2,     2,     2,
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
     115,   116,   117,   118,   119,   120,   121,   122
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
     132,   133,   136,   137,   141,   145,   148,   150,   154,   155,
     157,   159,   161,   165,   168,   170,   174,   176,   183,   187,
     189,   192,   196,   200,   205,   211,   217,   218,   220,   222,
     224,   226,   228,   230,   232,   234,   237,   239,   243,   245,
     249,   253,   257,   261,   265,   272,   276,   280,   284,   288,
     292,   296,   300,   304,   308,   311,   314,   316,   320,   324,
     328,   332,   336,   340,   344,   348,   352,   356,   360,   364,
     368,   372,   376,   380,   384,   388,   392,   396,   403,   407,
     411,   415,   419,   423,   427,   431,   435,   439,   442,   445,
     448,   452,   457,   459,   462,   464,   467,   469,   471,   473,
     475,   477,   479,   481,   484,   486,   489,   491,   494,   496,
     499,   501,   504,   506,   509,   511,   513,   516,   518,   521,
     524,   528,   532,   535,   539,   545,   547,   551,   554,   556,
     560,   564,   567,   569,   573,   576,   580,   584,   589,   591,
     595,   601,   610,   611,   617,   619,   621,   623,   625,   627,
     629,   631,   633,   635,   637,   639,   641,   643,   645,   647,
     649,   651,   653,   655,   657,   659,   661,   663,   665,   667,
     669,   671,   678,   683,   687,   689,   694,   702,   704,   708,
     715,   723,   725,   727,   731,   735,   737,   739,   742,   747,
     748,   750,   753,   757,   759,   761,   763,   765,   767,   769,
     771,   773,   775,   777,   779,   781,   783,   785,   787,   789,
     791,   793,   795,   797,   799,   801,   803,   805,   807,   809,
     811,   813
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     132,     0,    -1,   133,    -1,    -1,   134,   136,    -1,   137,
      -1,   135,   137,    -1,   134,   123,   137,    -1,   134,   123,
     135,   137,    -1,     1,   123,   137,    -1,    14,    -1,   135,
      14,    -1,    -1,   123,    -1,    13,   138,    -1,   138,    -1,
     139,    -1,   140,    -1,   141,    -1,   142,    -1,   143,    -1,
     144,    -1,   145,    -1,   146,    -1,   147,    -1,    34,     8,
      -1,   154,   191,    -1,   154,   191,    74,   164,    -1,     6,
      74,   164,    -1,    25,   164,    -1,    48,   191,    43,    -1,
      48,   191,    40,   164,    -1,    48,   191,    39,   164,    -1,
      44,   164,    -1,    45,     6,   149,   191,   148,    -1,    50,
       6,   149,   191,   148,    -1,    32,   157,   124,   193,   149,
     191,   148,    -1,   157,   124,     6,   125,   150,   126,   191,
     148,    -1,    20,     6,   149,    -1,    20,     6,   149,    74,
     164,    -1,    -1,    74,   164,    -1,    -1,   125,   150,   126,
      -1,   125,     1,   126,    -1,   151,   152,    -1,   153,    -1,
     151,   127,   153,    -1,    -1,   127,    -1,   154,    -1,   157,
      -1,   157,   124,     6,    -1,   156,   152,    -1,   157,    -1,
     156,   127,   157,    -1,   158,    -1,    22,    57,   155,    59,
      41,   158,    -1,    38,    41,   158,    -1,   160,    -1,    42,
     160,    -1,    16,   159,   160,    -1,    15,   159,   160,    -1,
     159,    47,    41,   160,    -1,    16,   159,    47,    41,   160,
      -1,    15,   159,    47,    41,   160,    -1,    -1,    42,    -1,
      36,    -1,    23,    -1,    31,    -1,    49,    -1,    19,    -1,
     163,    -1,    12,    -1,   162,   152,    -1,   164,    -1,   162,
     127,   164,    -1,   165,    -1,   163,    92,   165,    -1,   163,
      79,   163,    -1,   163,    80,   163,    -1,   163,    81,   163,
      -1,   163,    82,   163,    -1,   112,   125,   164,   127,   164,
     126,    -1,   163,    89,   163,    -1,   163,    91,   163,    -1,
     163,    83,   163,    -1,   163,    84,   163,    -1,   163,    85,
     163,    -1,   163,    86,   163,    -1,   163,    87,   163,    -1,
     163,    88,   163,    -1,   163,     7,   163,    -1,    83,   163,
      -1,    84,   163,    -1,   165,    -1,   164,    92,   165,    -1,
     164,    64,   164,    -1,   164,    65,   164,    -1,   164,    66,
     164,    -1,   164,    67,   164,    -1,   164,    68,   164,    -1,
     164,    69,   164,    -1,   164,    70,   164,    -1,   164,    71,
     164,    -1,   164,    72,   164,    -1,   164,    73,   164,    -1,
     164,    74,   164,    -1,   164,    75,   164,    -1,   164,    76,
     164,    -1,   164,    77,   164,    -1,   164,    78,   164,    -1,
     164,    79,   164,    -1,   164,    80,   164,    -1,   164,    81,
     164,    -1,   164,    82,   164,    -1,   112,   125,   164,   127,
     164,   126,    -1,   164,    89,   164,    -1,   164,    91,   164,
      -1,   164,    83,   164,    -1,   164,    84,   164,    -1,   164,
      85,   164,    -1,   164,    86,   164,    -1,   164,    87,   164,
      -1,   164,    88,   164,    -1,   164,     7,   164,    -1,    90,
     164,    -1,    83,   164,    -1,    84,   164,    -1,   125,   164,
     126,    -1,   125,   164,   126,   168,    -1,     6,    -1,     6,
     168,    -1,    54,    -1,    54,   168,    -1,     4,    -1,     3,
      -1,    35,    -1,     5,    -1,   166,    -1,    18,    -1,   169,
      -1,   169,   168,    -1,   170,    -1,   170,   168,    -1,   177,
      -1,   177,   168,    -1,   178,    -1,   178,   168,    -1,   180,
      -1,   180,   168,    -1,   181,    -1,   181,   168,    -1,   187,
      -1,   185,    -1,   185,   168,    -1,     8,    -1,     9,   167,
      -1,   162,    11,    -1,   162,    10,   167,    -1,    57,   161,
      59,    -1,   128,   129,    -1,   128,   161,   129,    -1,   128,
     164,   130,   171,   129,    -1,   172,    -1,   172,    56,   164,
      -1,   173,   152,    -1,   174,    -1,   173,   127,   174,    -1,
     175,    76,   164,    -1,   176,   152,    -1,     6,    -1,   176,
     127,     6,    -1,    57,    59,    -1,    57,   161,    59,    -1,
      58,   179,    60,    -1,    58,   179,   130,    60,    -1,   161,
      -1,   179,   130,   161,    -1,    57,   164,   130,   171,    59,
      -1,    33,   164,    51,   164,   182,    27,   164,    29,    -1,
      -1,   182,    28,   164,    51,   164,    -1,    94,    -1,    95,
      -1,    96,    -1,    97,    -1,    98,    -1,    99,    -1,   100,
      -1,   101,    -1,   102,    -1,   103,    -1,   104,    -1,   105,
      -1,   106,    -1,   107,    -1,   108,    -1,   109,    -1,   110,
      -1,   111,    -1,   113,    -1,   114,    -1,   115,    -1,   116,
      -1,   117,    -1,   118,    -1,   119,    -1,   122,    -1,   120,
      -1,   183,   125,   164,   127,   164,   126,    -1,   183,   125,
     164,   126,    -1,     6,   125,   126,    -1,   184,    -1,     6,
     125,   186,   126,    -1,     6,   125,   186,   126,   125,   164,
     126,    -1,   161,    -1,   161,    56,   164,    -1,    37,   128,
     188,   129,    76,   164,    -1,    37,   128,   188,   189,   129,
      76,   164,    -1,   190,    -1,   142,    -1,   188,   189,   190,
      -1,   188,   189,   142,    -1,   127,    -1,   123,    -1,   154,
     191,    -1,   154,   191,    74,   164,    -1,    -1,   192,    -1,
      92,   165,    -1,   192,    92,   165,    -1,     6,    -1,    94,
      -1,    95,    -1,    96,    -1,    97,    -1,    98,    -1,    99,
      -1,   100,    -1,   101,    -1,   102,    -1,   103,    -1,   104,
      -1,   105,    -1,   106,    -1,   107,    -1,   108,    -1,   109,
      -1,   110,    -1,   111,    -1,   112,    -1,   113,    -1,   114,
      -1,   115,    -1,   116,    -1,   117,    -1,   118,    -1,   119,
      -1,   120,    -1,   122,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   608,   608,   610,   612,   615,   620,   625,   630,   635,
     638,   646,   655,   655,   657,   673,   677,   679,   681,   682,
     684,   686,   688,   690,   692,   696,   719,   724,   732,   738,
     742,   747,   752,   759,   763,   771,   781,   788,   797,   809,
     817,   818,   823,   824,   826,   829,   833,   837,   842,   842,
     845,   847,   851,   856,   860,   862,   866,   867,   873,   882,
     885,   893,   901,   910,   919,   928,   941,   942,   946,   948,
     950,   952,   954,   956,   958,   964,   967,   969,   975,   976,
     978,   980,   982,   984,   986,   988,   990,   992,   994,   996,
     998,  1000,  1002,  1004,  1010,  1012,  1027,  1028,  1030,  1032,
    1034,  1036,  1038,  1040,  1042,  1044,  1046,  1048,  1050,  1052,
    1054,  1056,  1058,  1060,  1062,  1064,  1066,  1068,  1070,  1072,
    1074,  1076,  1078,  1080,  1082,  1084,  1086,  1092,  1094,  1101,
    1114,  1116,  1118,  1120,  1123,  1125,  1128,  1130,  1132,  1134,
    1136,  1137,  1139,  1140,  1143,  1144,  1147,  1148,  1151,  1152,
    1155,  1156,  1159,  1160,  1163,  1164,  1165,  1170,  1172,  1178,
    1183,  1191,  1195,  1197,  1201,  1207,  1209,  1212,  1215,  1217,
    1221,  1224,  1227,  1229,  1233,  1235,  1239,  1248,  1259,  1264,
    1268,  1274,  1290,  1291,  1295,  1297,  1299,  1301,  1303,  1305,
    1307,  1309,  1311,  1313,  1315,  1317,  1319,  1321,  1323,  1325,
    1327,  1329,  1331,  1333,  1335,  1337,  1339,  1341,  1343,  1345,
    1347,  1351,  1359,  1391,  1393,  1394,  1405,  1446,  1451,  1458,
    1460,  1486,  1488,  1496,  1498,  1507,  1507,  1510,  1516,  1527,
    1528,  1531,  1535,  1539,  1541,  1543,  1545,  1547,  1549,  1551,
    1553,  1555,  1557,  1559,  1561,  1563,  1565,  1567,  1569,  1571,
    1573,  1575,  1577,  1579,  1581,  1583,  1585,  1587,  1589,  1591,
    1593,  1595
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
  "\"var\"", "\"par\"", "\"svar\"", "\"<>\"", "\"ann\"", "\"annotation\"",
  "\"any\"", "\"array\"", "\"bool\"", "\"case\"", "\"constraint\"",
  "\"default\"", "\"else\"", "\"elseif\"", "\"endif\"", "\"enum\"",
  "\"float\"", "\"function\"", "\"if\"", "\"include\"", "\"infinity\"",
  "\"int\"", "\"let\"", "\"list\"", "\"maximize\"", "\"minimize\"",
  "\"of\"", "\"opt\"", "\"satisfy\"", "\"output\"", "\"predicate\"",
  "\"record\"", "\"set\"", "\"solve\"", "\"string\"", "\"test\"",
  "\"then\"", "\"tuple\"", "\"type\"", "\"_\"", "\"variant_record\"",
  "\"where\"", "\"[\"", "\"[|\"", "\"]\"", "\"|]\"", "FLATZINC_IDENTIFIER",
  "\"invalid integer literal\"", "\"invalid float literal\"", "\"<->\"",
  "\"->\"", "\"<-\"", "\"\\\\/\"", "\"xor\"", "\"/\\\\\"", "\"<\"",
  "\">\"", "\"<=\"", "\">=\"", "\"=\"", "\"!=\"", "\"in\"", "\"subset\"",
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
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,    59,    58,    40,    41,    44,   123,   125,
     124
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   131,   132,   133,   133,   134,   134,   134,   134,   134,
     135,   135,   136,   136,   137,   137,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   139,   140,   140,   141,   142,
     143,   143,   143,   144,   145,   145,   146,   146,   147,   147,
     148,   148,   149,   149,   149,   150,   151,   151,   152,   152,
     153,   153,   154,   155,   156,   156,   157,   157,   157,   158,
     158,   158,   158,   158,   158,   158,   159,   159,   160,   160,
     160,   160,   160,   160,   160,   161,   162,   162,   163,   163,
     163,   163,   163,   163,   163,   163,   163,   163,   163,   163,
     163,   163,   163,   163,   163,   163,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   165,   165,   166,   166,   167,
     167,   168,   169,   169,   170,   171,   171,   172,   173,   173,
     174,   175,   176,   176,   177,   177,   178,   178,   179,   179,
     180,   181,   182,   182,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   184,   184,   185,   185,   185,   185,   186,   186,   187,
     187,   188,   188,   188,   188,   189,   189,   190,   190,   191,
     191,   192,   192,   193,   193,   193,   193,   193,   193,   193,
     193,   193,   193,   193,   193,   193,   193,   193,   193,   193,
     193,   193,   193,   193,   193,   193,   193,   193,   193,   193,
     193,   193
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     2,     3,     4,     3,
       1,     2,     0,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     4,     3,     2,
       3,     4,     4,     2,     5,     5,     7,     8,     3,     5,
       0,     2,     0,     3,     3,     2,     1,     3,     0,     1,
       1,     1,     3,     2,     1,     3,     1,     6,     3,     1,
       2,     3,     3,     4,     5,     5,     0,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     3,     1,     3,
       3,     3,     3,     3,     6,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     2,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     6,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     2,     2,
       3,     4,     1,     2,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     2,     1,     2,     1,     2,
       1,     2,     1,     2,     1,     1,     2,     1,     2,     2,
       3,     3,     2,     3,     5,     1,     3,     2,     1,     3,
       3,     2,     1,     3,     2,     3,     3,     4,     1,     3,
       5,     8,     0,     5,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     6,     4,     3,     1,     4,     7,     1,     3,     6,
       7,     1,     1,     3,     3,     1,     1,     2,     4,     0,
       1,     2,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,   137,   136,   139,   132,   157,     0,    74,    66,
      10,    66,    66,   141,    72,     0,     0,    69,     0,    70,
      66,     0,     0,   138,    68,     0,     0,    67,     0,     0,
     229,    71,     0,   134,     0,     0,     0,     0,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,     0,   202,   203,   204,
     205,   206,   207,   208,   210,   209,     0,     0,     0,     2,
      12,    66,     5,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,   229,     0,    56,     0,    59,    73,    78,
     140,   142,   144,   146,   148,   150,   152,     0,   214,   155,
     154,    66,     0,     0,     0,   133,   132,     0,     0,     0,
       0,     0,    76,    96,   158,    14,    67,     0,     0,    42,
      66,    29,     0,     0,    25,    66,    66,    60,    33,    42,
       0,     0,   230,    42,   135,   174,     0,    48,    76,   178,
       0,    94,    95,     0,     0,   162,     0,    76,     1,    13,
       4,    11,     6,    26,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     143,   145,   147,   149,   151,   153,     0,   156,     9,     0,
      28,   213,   217,     0,   128,   129,   127,     0,     0,   159,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    62,     0,    61,     0,    38,     0,    48,    54,     0,
       0,   222,   229,     0,     0,   221,    58,   229,   231,     0,
       0,    30,     0,   229,   175,    49,    75,     0,   176,     0,
       0,   130,   163,     0,    66,     7,     0,    52,     0,    93,
      80,    81,    82,    83,    87,    88,    89,    90,    91,    92,
      85,    86,    79,     0,   161,     0,   215,     0,   160,    77,
     126,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     120,   121,   122,   123,   124,   125,   118,   119,    97,     0,
       0,     0,     0,    48,    46,    50,    51,     0,     0,    49,
      53,   233,   234,   235,   236,   237,   238,   239,   240,   241,
     242,   243,   244,   245,   246,   247,   248,   249,   250,   251,
     252,   253,   254,   255,   256,   257,   258,   259,   260,   261,
      42,   182,   227,     0,   226,   225,     0,    66,    40,    32,
      31,   232,    40,   172,     0,   165,    48,   168,     0,    48,
     177,   179,     0,   131,     0,     8,    27,    66,    63,   212,
       0,   218,     0,     0,    65,    64,    44,    43,    49,    45,
      39,    66,    55,   229,     0,     0,    52,     0,     0,   224,
     223,     0,    34,    35,   180,     0,    49,   167,     0,    49,
     171,     0,   164,     0,     0,     0,     0,    47,    57,    40,
       0,     0,   228,   219,     0,    41,   166,   169,   170,   173,
      84,   229,   211,   216,   117,    36,     0,     0,   220,    40,
     181,     0,    37,   183
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    68,    69,    70,    71,   150,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,   402,   225,   312,
     313,   246,   314,    83,   226,   227,    84,    85,    86,    87,
     136,   137,    88,   112,   113,    90,   114,   105,    91,    92,
     364,   365,   366,   367,   368,   369,    93,    94,   140,    95,
      96,   394,    97,    98,    99,   183,   100,   234,   357,   235,
     131,   132,   350
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -341
static const yytype_int16 yypact[] =
{
     441,   -95,  -341,  -341,  -341,   -42,  -341,  2724,  -341,  1338,
    -341,    -9,    -9,  -341,  -341,    32,   -13,  -341,  2724,  -341,
    1716,  2724,    31,  -341,  -341,   -87,     4,  2220,  2724,    40,
     -45,  -341,    46,    -4,  2346,  2724,  2850,  2850,  -341,  -341,
    -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,
    -341,  -341,  -341,  -341,  -341,  -341,   -71,  -341,  -341,  -341,
    -341,  -341,  -341,  -341,  -341,  -341,  2724,   833,    55,  -341,
     -67,  1086,  -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,
    -341,  -341,  -341,   -45,   -66,  -341,    10,  -341,   792,  -341,
    -341,    -4,    -4,    -4,    -4,    -4,    -4,   -65,  -341,    -4,
    -341,  1212,  2724,  2724,  2472,  -341,   -38,  2724,  2724,  2724,
     -64,    24,  3662,  -341,  -341,  -341,  -341,  1968,  2094,   -63,
    1716,  3662,   -61,  3530,  -341,  1464,  1842,  -341,  3662,   -63,
    2885,    -3,   -33,   -63,  -341,  -341,    11,   -56,  2964,  -341,
     -36,     5,     5,  2724,  3223,  -341,   -55,  2999,  -341,   960,
    -341,  -341,  -341,    -1,    70,    36,  2850,  2850,  2850,  2850,
    2850,  2850,  2850,  2850,  2850,  2850,  2850,  2850,  2850,  2885,
    -341,  -341,  -341,  -341,  -341,  -341,  2724,  -341,  -341,    22,
    3662,  -341,    26,   -40,    68,    68,    68,  2724,  2724,  -341,
    2724,  2724,  2724,  2724,  2724,  2724,  2724,  2724,  2724,  2724,
    2724,  2724,  2724,  2724,  2724,  2724,  2724,  2724,  2724,  2724,
    2724,  2724,  2724,  2724,  2724,  2724,  2724,  2724,  2724,  2885,
      43,  -341,    47,  -341,   580,    21,    42,   -22,  -341,  3452,
    2724,  -341,   -45,   -21,  -100,  -341,  -341,   -45,  -341,  2724,
    2724,  -341,  2885,   -45,  -341,  2724,  -341,    96,  -341,  2598,
    3111,    -4,  -341,    96,  1086,  -341,  2724,   -17,  2220,    18,
     159,   159,   159,   566,    44,    44,     5,     5,     5,     5,
       5,     5,  -341,  3082,  -341,  2724,     3,  3194,  -341,  3662,
      60,  3696,   245,   245,   696,   696,   822,   949,   949,   949,
     949,   949,   949,    35,    35,    35,  1074,  1074,  1074,  1200,
     259,   259,    68,    68,    68,    68,    68,    68,  -341,  2220,
    2220,   -20,     8,    27,  -341,  -341,   -21,  2724,   116,  1590,
    -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,
    -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,
    -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,
     -63,  3662,    88,   157,  -341,  -341,    89,   706,    90,  3662,
    3662,  -341,    90,  -341,   108,   112,    74,  -341,    94,    77,
    -341,  -341,  2724,  -341,    76,  -341,  3662,  1716,  -341,  -341,
    2724,  3662,  2724,  2724,  -341,  -341,  -341,  -341,  1590,  -341,
    3662,  1842,  -341,   -45,    -2,  2724,  -341,  2724,    95,  -341,
    -341,  2724,  -341,  -341,  -341,  2724,    96,  -341,  2724,   200,
    -341,  3306,  -341,    81,  3335,  3418,  3447,  -341,  -341,    90,
    2724,  2724,  3662,  3662,  2724,  3662,  3662,  -341,  3662,  -341,
    -341,   -45,  -341,  -341,  -341,  -341,  3613,  3579,  3662,    90,
    -341,  2724,  -341,  3662
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -341,  -341,  -341,  -341,    61,  -341,   -51,   202,  -341,  -341,
    -341,  -118,  -341,  -341,  -341,  -341,  -341,  -340,  -120,  -168,
    -341,  -213,  -175,  -117,  -341,  -341,   -16,  -121,    19,   -25,
     -24,    14,   -19,   -18,   268,  -341,    28,   -27,  -341,  -341,
     -39,  -341,  -341,  -191,  -341,  -341,  -341,  -341,  -341,  -341,
    -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,  -341,  -140,
     -82,  -341,  -341
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -67
static const yytype_int16 yytable[] =
{
     121,   153,   127,   123,   122,   236,   134,   231,   232,   237,
     128,   139,   156,   243,   320,   102,   138,   141,   142,   102,
     152,   111,   403,   354,   248,   420,   421,   355,   101,   356,
     117,   118,   103,   116,   188,   189,   239,   240,   119,   124,
     241,   125,   191,   146,   120,   126,   129,   130,   144,   147,
     178,   156,   133,   102,   143,   148,   149,   155,   154,   242,
     176,   187,   224,   229,   170,   171,   172,   173,   174,   175,
     244,   245,   177,   256,   252,   191,   257,   258,   179,   435,
     182,   274,   275,   104,   309,   180,   276,   104,   310,   184,
     185,   186,   221,   223,   249,   317,   168,   169,   255,   442,
     389,   318,   363,   353,   228,   319,   386,   315,   377,   233,
     169,   -67,   -67,   -67,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   250,   218,   219,   382,   163,
     164,   165,   166,   167,   387,   168,   169,   259,   260,   261,
     262,   263,   264,   265,   266,   267,   268,   269,   270,   271,
     352,   190,   219,   407,   388,   358,   410,   391,   273,   218,
     219,   362,   395,   396,   401,   397,   156,   404,   405,   277,
     408,   424,   279,   280,   281,   282,   283,   284,   285,   286,
     287,   288,   289,   290,   291,   292,   293,   294,   295,   296,
     297,   298,   299,   300,   301,   302,   303,   304,   305,   306,
     307,   406,   111,   375,   409,   412,   429,   431,   316,   413,
     254,   115,   351,   417,   374,   427,   278,   400,     0,     0,
       0,   359,   360,     0,   373,   371,     0,   279,     0,     0,
     393,     0,     0,   378,     0,     0,     0,     0,   376,   399,
     232,   160,   161,   162,   163,   164,   165,   166,   167,     0,
     168,   169,   191,     0,     0,     0,     0,   381,     0,     0,
     315,     0,     0,     0,     0,     0,   191,     0,    89,     0,
     418,   315,     0,     0,     0,     0,     0,    89,     0,     0,
       0,     0,     0,     0,   384,   385,     0,     0,    89,     0,
       0,     0,     0,     0,     0,    89,     0,     0,     0,   390,
       0,     0,     0,   392,    89,    89,     0,     0,     0,     0,
       0,   419,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,     0,   218,   219,     0,    89,
       0,   233,     0,     0,   213,   214,   215,   216,   217,   439,
     218,   219,     0,     0,   411,     0,     0,     0,     0,     0,
       0,   316,   414,     0,   415,   416,     0,     0,     0,    89,
       0,     0,   316,     0,     0,     0,     0,   422,     0,   423,
       0,     0,     0,   425,     0,    89,    89,   426,    89,     0,
     428,     0,     0,    89,    89,     0,     0,     0,   238,     0,
       0,     0,   436,   437,     0,     0,   438,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    89,     0,     0,
       0,     0,     0,   443,    89,    89,    89,    89,    89,    89,
      89,    89,    89,    89,    89,    89,    89,   272,     0,     0,
       0,    -3,     1,     0,     2,     3,     4,     5,     0,     6,
       7,     0,     0,     8,     9,    10,    11,    12,     0,    13,
      14,    15,     0,    16,    17,     0,    18,     0,     0,     0,
       0,     0,    19,    20,    21,    22,    23,    24,    25,    26,
       0,     0,     0,    27,     0,    28,    29,   308,   -66,    30,
      31,    32,    89,     0,     0,    33,     0,     0,    34,    35,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     361,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    89,     0,    36,    37,    89,     0,     0,     0,
       0,     0,     0,     0,     0,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,     0,    65,     0,     0,    66,     0,     0,    67,
       0,     0,     0,   156,     0,     0,     0,    89,    89,     0,
       0,   311,     0,     2,     3,     4,   106,    89,     6,     7,
       0,     0,     8,     0,     0,    11,    12,     0,    13,    14,
       0,     0,    16,    17,     0,     0,     0,     0,     0,     0,
       0,    19,     0,    21,     0,    23,    24,    25,    26,     0,
       0,     0,    27,     0,     0,    89,     0,   -66,     0,    31,
       0,     0,     0,     0,    33,     0,     0,    34,    35,     0,
       0,     0,     0,     0,     0,    89,     0,     0,   -67,   161,
     162,   163,   164,   165,   166,   167,    89,   168,   169,    89,
       0,     0,     0,    36,    37,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,     0,    65,   191,     0,    66,     0,     0,    67,     2,
       3,     4,   106,     0,     6,     7,     0,     0,     8,     0,
       0,    11,    12,     0,    13,    14,     0,     0,    16,    17,
       0,    18,     0,     0,     0,     0,     0,    19,     0,    21,
       0,    23,    24,    25,    26,     0,     0,     0,    27,     0,
       0,     0,     0,     0,     0,    31,     0,     0,     0,     0,
      33,     0,     0,    34,    35,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,     0,   218,   219,    36,
      37,     0,     0,     0,     0,     0,     0,     0,     0,   156,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,     0,    65,   191,
       0,    66,     0,     0,    67,   398,     2,     3,     4,   106,
       0,     6,     7,     0,     0,     0,     0,     0,     0,     0,
       0,    13,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    21,     0,    23,     0,
      25,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,     0,   168,   169,     0,     0,    33,     0,     0,
      34,    35,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,     0,   218,   219,     0,   107,   108,     0,     0,
       0,     0,     0,   109,     0,     0,     0,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,   110,    57,    58,    59,    60,
      61,    62,    63,    64,     0,    65,   191,     0,    66,     0,
       0,    67,   145,     2,     3,     4,     5,     0,     6,     7,
       0,     0,     8,     9,    10,    11,    12,     0,    13,    14,
      15,     0,    16,    17,     0,    18,     0,     0,     0,     0,
       0,    19,    20,    21,    22,    23,    24,    25,    26,     0,
       0,     0,    27,     0,    28,    29,     0,   -66,    30,    31,
      32,     0,     0,     0,    33,     0,     0,    34,    35,   -67,
     -67,   -67,   -67,   -67,   -67,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,     0,
     218,   219,     0,    36,    37,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,   191,    65,     0,     0,    66,     0,     0,    67,     2,
       3,     4,     5,     0,     6,     7,     0,     0,     8,     9,
     151,    11,    12,     0,    13,    14,    15,     0,    16,    17,
       0,    18,     0,     0,     0,     0,     0,    19,    20,    21,
      22,    23,    24,    25,    26,     0,     0,     0,    27,     0,
      28,    29,     0,     0,    30,    31,    32,     0,     0,     0,
      33,     0,     0,    34,    35,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   210,   211,   212,   213,
     214,   215,   216,   217,     0,   218,   219,     0,     0,    36,
      37,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,   191,    65,     0,
       0,    66,     0,     0,    67,     2,     3,     4,     5,     0,
       6,     7,     0,     0,     8,     9,     0,    11,    12,     0,
      13,    14,    15,     0,    16,    17,     0,    18,     0,     0,
       0,     0,     0,    19,    20,    21,    22,    23,    24,    25,
      26,     0,     0,     0,    27,     0,    28,    29,     0,     0,
      30,    31,    32,     0,     0,     0,    33,     0,     0,    34,
      35,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   -67,   211,   212,   213,   214,   215,   216,   217,
       0,   218,   219,     0,     0,    36,    37,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,     0,    65,     0,     0,    66,     0,     0,
      67,     2,     3,     4,     5,     0,     6,     7,     0,     0,
       8,     0,     0,    11,    12,     0,    13,    14,    15,     0,
      16,    17,     0,    18,     0,     0,     0,     0,     0,    19,
      20,    21,    22,    23,    24,    25,    26,     0,     0,     0,
      27,     0,    28,    29,     0,     0,    30,    31,    32,     0,
       0,     0,    33,     0,     0,    34,    35,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    36,    37,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,     0,
      65,     0,     0,    66,     0,     0,    67,     2,     3,     4,
     106,     0,     6,     7,     0,     0,     8,     0,     0,    11,
      12,     0,    13,    14,     0,     0,    16,    17,     0,    18,
       0,     0,     0,     0,     0,    19,     0,    21,     0,    23,
      24,    25,    26,     0,     0,     0,    27,     0,     0,     0,
       0,     0,     0,    31,     0,     0,     0,     0,    33,     0,
       0,    34,    35,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    36,    37,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,     0,    65,     0,     0,    66,
       0,     0,    67,     2,     3,     4,   106,     0,     6,     7,
       0,     0,     8,     0,     0,    11,    12,     0,    13,    14,
       0,     0,    16,    17,     0,     0,     0,     0,     0,     0,
       0,    19,     0,    21,     0,    23,    24,    25,    26,     0,
       0,     0,    27,     0,     0,     0,     0,   -66,     0,    31,
       0,     0,     0,     0,    33,     0,     0,    34,    35,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    36,    37,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,     0,    65,     0,     0,    66,     0,     0,    67,     2,
       3,     4,   106,     0,     6,     7,     0,     0,     8,     0,
       0,    11,    12,     0,    13,    14,     0,     0,    16,    17,
       0,     0,     0,     0,     0,     0,     0,    19,     0,    21,
       0,    23,    24,    25,    26,     0,     0,     0,    27,     0,
       0,     0,     0,     0,     0,    31,     0,     0,     0,     0,
      33,     0,     0,    34,    35,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    36,
      37,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,     0,    65,     0,
       0,    66,     0,     0,    67,     2,     3,     4,   106,     0,
       6,     7,     0,     0,     8,     0,     0,    11,    12,     0,
      13,    14,     0,     0,     0,    17,     0,     0,     0,     0,
       0,     0,     0,    19,     0,    21,     0,    23,    24,    25,
       0,     0,     0,     0,    27,     0,     0,     0,     0,     0,
       0,    31,     0,     0,     0,     0,    33,     0,     0,    34,
      35,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    36,    37,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,     0,    65,     0,     0,    66,     0,     0,
      67,     2,     3,     4,   106,     0,     6,     7,     0,     0,
       8,     0,     0,     0,     0,     0,    13,    14,     0,     0,
       0,    17,     0,     0,     0,     0,     0,     0,     0,    19,
       0,    21,     0,    23,    24,    25,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   220,     0,    31,     0,     0,
       0,     0,    33,     0,     0,    34,    35,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    36,    37,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,     0,
      65,     0,     0,    66,     0,     0,    67,     2,     3,     4,
     106,     0,     6,     7,     0,     0,     8,     0,     0,     0,
       0,     0,    13,    14,     0,     0,     0,    17,     0,     0,
       0,     0,     0,     0,     0,    19,     0,    21,     0,    23,
      24,    25,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   222,     0,    31,     0,     0,     0,     0,    33,     0,
       0,    34,    35,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    36,    37,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,     0,    65,     0,     0,    66,
       0,     0,    67,     2,     3,     4,   106,     0,     6,     7,
       0,     0,     8,     0,     0,     0,     0,     0,    13,    14,
       0,     0,     0,    17,     0,     0,     0,     0,     0,     0,
       0,    19,     0,    21,     0,    23,    24,    25,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    31,
       0,     0,     0,     0,    33,     0,     0,    34,    35,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    36,    37,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,     0,    65,     0,     0,    66,     0,     0,    67,     2,
       3,     4,   106,     0,     6,     7,     0,     0,     0,     0,
       0,     0,     0,     0,    13,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    21,
       0,    23,     0,    25,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      33,     0,     0,    34,    35,   135,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   107,
     108,     0,     0,     0,     0,     0,   109,     0,     0,     0,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,   110,    57,
      58,    59,    60,    61,    62,    63,    64,     0,    65,     0,
       0,    66,     0,     0,    67,     2,     3,     4,   106,     0,
       6,     7,     0,     0,     0,     0,     0,     0,     0,     0,
      13,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    21,     0,    23,     0,    25,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    33,     0,     0,    34,
      35,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   107,   108,     0,     0,     0,
       0,     0,   109,     0,     0,     0,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,   110,    57,    58,    59,    60,    61,
      62,    63,    64,     0,    65,     0,     0,    66,   181,     0,
      67,     2,     3,     4,   106,     0,     6,     7,     0,     0,
       0,     0,     0,     0,     0,     0,    13,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    21,     0,    23,     0,    25,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    33,     0,     0,    34,    35,     0,   370,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   107,   108,     0,     0,     0,     0,     0,   109,     0,
       0,     0,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
     110,    57,    58,    59,    60,    61,    62,    63,    64,     0,
      65,     0,     0,    66,     0,     0,    67,     2,     3,     4,
     106,     0,     6,     7,     0,     0,     0,     0,     0,     0,
       0,     0,    13,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    21,     0,    23,
       0,    25,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    33,     0,
       0,    34,    35,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   107,   108,     0,
       0,     0,     0,     0,   109,     0,     0,     0,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,   110,    57,    58,    59,
      60,    61,    62,    63,    64,     0,    65,     0,     0,    66,
       0,     0,    67,     2,     3,     4,   106,     0,     6,     7,
       0,     0,     0,     0,     0,     0,     0,     0,    13,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    21,     0,    23,     0,    25,     2,     3,
       4,   106,     0,     6,     7,     0,     0,     0,     0,     0,
       0,     0,     0,    13,    33,     0,     0,    34,    35,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    21,     0,
      23,     0,    25,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    36,    37,     0,     0,     0,     0,    33,
       0,     0,    34,    35,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,   191,    65,     0,     0,    66,     0,     0,    67,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,     0,    57,    58,
      59,    60,    61,    62,    63,    64,   191,    65,     0,     0,
      66,     0,     0,    67,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   192,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,     0,   218,   219,     0,     0,     0,
       0,     0,     0,   192,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   191,
     218,   219,     0,     0,   247,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   191,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   253,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,     0,   218,   219,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   191,   218,   219,     0,     0,     0,     0,   379,   380,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     191,     0,     0,     0,     0,     0,     0,     0,   372,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   192,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,     0,   218,   219,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   191,   218,   219,     0,     0,     0,     0,
       0,   383,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   191,     0,     0,     0,     0,     0,     0,   251,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,     0,   218,   219,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   191,   218,   219,     0,     0,
       0,     0,   430,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   191,     0,     0,     0,   321,     0,
       0,   432,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   192,   193,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,     0,   218,
     219,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   191,   218,   219,
       0,     0,     0,     0,   433,     0,   322,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,   339,   340,   341,   342,   343,   344,   345,
     346,   347,   348,   434,   349,     0,     0,     0,     0,     0,
       0,   230,     0,     0,     0,     0,   191,     0,     0,     0,
       0,     0,     0,     0,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     191,   218,   219,     0,     0,     0,     0,     0,     0,     0,
     441,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   440,   192,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   191,
     218,   219,     0,     0,     0,     0,     0,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   191,   218,   219,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,     0,   218,   219,     0,     0,     0,     0,     0,
       0,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,     0,   218,   219
};

static const yytype_int16 yycheck[] =
{
      18,    83,    27,    21,    20,   126,    33,   125,   125,   129,
      28,    35,     7,   133,   227,    57,    34,    36,    37,    57,
      71,     7,   362,   123,    60,    27,    28,   127,   123,   129,
      11,    12,    74,    42,    10,    11,    39,    40,     6,     8,
      43,   128,     7,    67,    57,    41,     6,    92,    66,    67,
     101,     7,     6,    57,   125,     0,   123,    47,   124,    92,
     125,   125,   125,   124,    91,    92,    93,    94,    95,    96,
      59,   127,    99,    74,   129,     7,     6,    41,   102,   419,
     104,    59,    56,   125,    41,   103,   126,   125,    41,   107,
     108,   109,   117,   118,   130,    74,    91,    92,   149,   439,
     313,    59,     6,   124,   120,   127,   126,   224,   125,   125,
      92,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,   143,    91,    92,   125,    85,
      86,    87,    88,    89,   126,    91,    92,   156,   157,   158,
     159,   160,   161,   162,   163,   164,   165,   166,   167,   168,
     232,   127,    92,   366,   127,   237,   369,    41,   176,    91,
      92,   243,    74,     6,    74,    76,     7,    59,    56,   187,
      76,    76,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   127,   188,   254,   127,   129,     6,   126,   224,   377,
     149,     9,   230,   388,   253,   406,   188,   357,    -1,    -1,
      -1,   239,   240,    -1,   251,   249,    -1,   245,    -1,    -1,
     350,    -1,    -1,   258,    -1,    -1,    -1,    -1,   256,   357,
     357,    82,    83,    84,    85,    86,    87,    88,    89,    -1,
      91,    92,     7,    -1,    -1,    -1,    -1,   275,    -1,    -1,
     377,    -1,    -1,    -1,    -1,    -1,     7,    -1,     0,    -1,
     391,   388,    -1,    -1,    -1,    -1,    -1,     9,    -1,    -1,
      -1,    -1,    -1,    -1,   309,   310,    -1,    -1,    20,    -1,
      -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,   317,
      -1,    -1,    -1,   319,    36,    37,    -1,    -1,    -1,    -1,
      -1,   393,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    -1,    91,    92,    -1,    71,
      -1,   357,    -1,    -1,    85,    86,    87,    88,    89,   431,
      91,    92,    -1,    -1,   372,    -1,    -1,    -1,    -1,    -1,
      -1,   377,   380,    -1,   382,   383,    -1,    -1,    -1,   101,
      -1,    -1,   388,    -1,    -1,    -1,    -1,   395,    -1,   397,
      -1,    -1,    -1,   401,    -1,   117,   118,   405,   120,    -1,
     408,    -1,    -1,   125,   126,    -1,    -1,    -1,   130,    -1,
      -1,    -1,   420,   421,    -1,    -1,   424,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   149,    -1,    -1,
      -1,    -1,    -1,   441,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,    -1,    -1,
      -1,     0,     1,    -1,     3,     4,     5,     6,    -1,     8,
       9,    -1,    -1,    12,    13,    14,    15,    16,    -1,    18,
      19,    20,    -1,    22,    23,    -1,    25,    -1,    -1,    -1,
      -1,    -1,    31,    32,    33,    34,    35,    36,    37,    38,
      -1,    -1,    -1,    42,    -1,    44,    45,   219,    47,    48,
      49,    50,   224,    -1,    -1,    54,    -1,    -1,    57,    58,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     242,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   254,    -1,    83,    84,   258,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,    -1,   122,    -1,    -1,   125,    -1,    -1,   128,
      -1,    -1,    -1,     7,    -1,    -1,    -1,   309,   310,    -1,
      -1,     1,    -1,     3,     4,     5,     6,   319,     8,     9,
      -1,    -1,    12,    -1,    -1,    15,    16,    -1,    18,    19,
      -1,    -1,    22,    23,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    31,    -1,    33,    -1,    35,    36,    37,    38,    -1,
      -1,    -1,    42,    -1,    -1,   357,    -1,    47,    -1,    49,
      -1,    -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,
      -1,    -1,    -1,    -1,    -1,   377,    -1,    -1,    82,    83,
      84,    85,    86,    87,    88,    89,   388,    91,    92,   391,
      -1,    -1,    -1,    83,    84,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,    -1,   122,     7,    -1,   125,    -1,    -1,   128,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    12,    -1,
      -1,    15,    16,    -1,    18,    19,    -1,    -1,    22,    23,
      -1,    25,    -1,    -1,    -1,    -1,    -1,    31,    -1,    33,
      -1,    35,    36,    37,    38,    -1,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    -1,    -1,    49,    -1,    -1,    -1,    -1,
      54,    -1,    -1,    57,    58,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    -1,    91,    92,    83,
      84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,    -1,   122,     7,
      -1,   125,    -1,    -1,   128,   129,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,    35,    -1,
      37,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    -1,    91,    92,    -1,    -1,    54,    -1,    -1,
      57,    58,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    -1,    91,    92,    -1,    83,    84,    -1,    -1,
      -1,    -1,    -1,    90,    -1,    -1,    -1,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,    -1,   122,     7,    -1,   125,    -1,
      -1,   128,   129,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    12,    13,    14,    15,    16,    -1,    18,    19,
      20,    -1,    22,    23,    -1,    25,    -1,    -1,    -1,    -1,
      -1,    31,    32,    33,    34,    35,    36,    37,    38,    -1,
      -1,    -1,    42,    -1,    44,    45,    -1,    47,    48,    49,
      50,    -1,    -1,    -1,    54,    -1,    -1,    57,    58,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    -1,
      91,    92,    -1,    83,    84,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,     7,   122,    -1,    -1,   125,    -1,    -1,   128,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    12,    13,
      14,    15,    16,    -1,    18,    19,    20,    -1,    22,    23,
      -1,    25,    -1,    -1,    -1,    -1,    -1,    31,    32,    33,
      34,    35,    36,    37,    38,    -1,    -1,    -1,    42,    -1,
      44,    45,    -1,    -1,    48,    49,    50,    -1,    -1,    -1,
      54,    -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    82,    83,    84,    85,
      86,    87,    88,    89,    -1,    91,    92,    -1,    -1,    83,
      84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,     7,   122,    -1,
      -1,   125,    -1,    -1,   128,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    12,    13,    -1,    15,    16,    -1,
      18,    19,    20,    -1,    22,    23,    -1,    25,    -1,    -1,
      -1,    -1,    -1,    31,    32,    33,    34,    35,    36,    37,
      38,    -1,    -1,    -1,    42,    -1,    44,    45,    -1,    -1,
      48,    49,    50,    -1,    -1,    -1,    54,    -1,    -1,    57,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    82,    83,    84,    85,    86,    87,    88,    89,
      -1,    91,    92,    -1,    -1,    83,    84,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,    -1,   122,    -1,    -1,   125,    -1,    -1,
     128,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      12,    -1,    -1,    15,    16,    -1,    18,    19,    20,    -1,
      22,    23,    -1,    25,    -1,    -1,    -1,    -1,    -1,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    -1,    -1,
      42,    -1,    44,    45,    -1,    -1,    48,    49,    50,    -1,
      -1,    -1,    54,    -1,    -1,    57,    58,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,    -1,
     122,    -1,    -1,   125,    -1,    -1,   128,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    12,    -1,    -1,    15,
      16,    -1,    18,    19,    -1,    -1,    22,    23,    -1,    25,
      -1,    -1,    -1,    -1,    -1,    31,    -1,    33,    -1,    35,
      36,    37,    38,    -1,    -1,    -1,    42,    -1,    -1,    -1,
      -1,    -1,    -1,    49,    -1,    -1,    -1,    -1,    54,    -1,
      -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    84,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,    -1,   122,    -1,    -1,   125,
      -1,    -1,   128,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    12,    -1,    -1,    15,    16,    -1,    18,    19,
      -1,    -1,    22,    23,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    31,    -1,    33,    -1,    35,    36,    37,    38,    -1,
      -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,    49,
      -1,    -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    83,    84,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,    -1,   122,    -1,    -1,   125,    -1,    -1,   128,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    12,    -1,
      -1,    15,    16,    -1,    18,    19,    -1,    -1,    22,    23,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,    33,
      -1,    35,    36,    37,    38,    -1,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    -1,    -1,    49,    -1,    -1,    -1,    -1,
      54,    -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,
      84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,    -1,   122,    -1,
      -1,   125,    -1,    -1,   128,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    12,    -1,    -1,    15,    16,    -1,
      18,    19,    -1,    -1,    -1,    23,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    31,    -1,    33,    -1,    35,    36,    37,
      -1,    -1,    -1,    -1,    42,    -1,    -1,    -1,    -1,    -1,
      -1,    49,    -1,    -1,    -1,    -1,    54,    -1,    -1,    57,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    83,    84,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,    -1,   122,    -1,    -1,   125,    -1,    -1,
     128,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      12,    -1,    -1,    -1,    -1,    -1,    18,    19,    -1,    -1,
      -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,
      -1,    33,    -1,    35,    36,    37,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    47,    -1,    49,    -1,    -1,
      -1,    -1,    54,    -1,    -1,    57,    58,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,    -1,
     122,    -1,    -1,   125,    -1,    -1,   128,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    12,    -1,    -1,    -1,
      -1,    -1,    18,    19,    -1,    -1,    -1,    23,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    31,    -1,    33,    -1,    35,
      36,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    47,    -1,    49,    -1,    -1,    -1,    -1,    54,    -1,
      -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    84,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,    -1,   122,    -1,    -1,   125,
      -1,    -1,   128,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    12,    -1,    -1,    -1,    -1,    -1,    18,    19,
      -1,    -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    31,    -1,    33,    -1,    35,    36,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      -1,    -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    83,    84,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,    -1,   122,    -1,    -1,   125,    -1,    -1,   128,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    18,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,
      -1,    35,    -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      54,    -1,    -1,    57,    58,    59,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,
      84,    -1,    -1,    -1,    -1,    -1,    90,    -1,    -1,    -1,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,    -1,   122,    -1,
      -1,   125,    -1,    -1,   128,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    33,    -1,    35,    -1,    37,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,    -1,    57,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    83,    84,    -1,    -1,    -1,
      -1,    -1,    90,    -1,    -1,    -1,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,    -1,   122,    -1,    -1,   125,   126,    -1,
     128,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    33,    -1,    35,    -1,    37,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    54,    -1,    -1,    57,    58,    -1,    60,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    83,    84,    -1,    -1,    -1,    -1,    -1,    90,    -1,
      -1,    -1,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,    -1,
     122,    -1,    -1,   125,    -1,    -1,   128,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,    35,
      -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,
      -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    84,    -1,
      -1,    -1,    -1,    -1,    90,    -1,    -1,    -1,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,    -1,   122,    -1,    -1,   125,
      -1,    -1,   128,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    33,    -1,    35,    -1,    37,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    18,    54,    -1,    -1,    57,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,
      35,    -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    83,    84,    -1,    -1,    -1,    -1,    54,
      -1,    -1,    57,    58,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,     7,   122,    -1,    -1,   125,    -1,    -1,   128,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,    -1,   113,   114,
     115,   116,   117,   118,   119,   120,     7,   122,    -1,    -1,
     125,    -1,    -1,   128,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    -1,    91,    92,    -1,    -1,    -1,
      -1,    -1,    -1,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,     7,
      91,    92,    -1,    -1,   130,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   130,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    -1,    91,    92,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,     7,    91,    92,    -1,    -1,    -1,    -1,   126,   127,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   127,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    -1,    91,    92,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,     7,    91,    92,    -1,    -1,    -1,    -1,
      -1,   127,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,   126,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    -1,    91,    92,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,     7,    91,    92,    -1,    -1,
      -1,    -1,   126,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,     6,    -1,
      -1,   126,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    -1,    91,
      92,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,     7,    91,    92,
      -1,    -1,    -1,    -1,   126,    -1,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   126,   122,    -1,    -1,    -1,    -1,    -1,
      -1,    51,    -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
       7,    91,    92,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      51,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    29,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,     7,
      91,    92,    -1,    -1,    -1,    -1,    -1,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,     7,    91,    92,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    -1,    91,    92,    -1,    -1,    -1,    -1,    -1,
      -1,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    -1,    91,    92
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     4,     5,     6,     8,     9,    12,    13,
      14,    15,    16,    18,    19,    20,    22,    23,    25,    31,
      32,    33,    34,    35,    36,    37,    38,    42,    44,    45,
      48,    49,    50,    54,    57,    58,    83,    84,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   122,   125,   128,   132,   133,
     134,   135,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   154,   157,   158,   159,   160,   163,   165,
     166,   169,   170,   177,   178,   180,   181,   183,   184,   185,
     187,   123,    57,    74,   125,   168,     6,    83,    84,    90,
     112,   162,   164,   165,   167,   138,    42,   159,   159,     6,
      57,   164,   157,   164,     8,   128,    41,   160,   164,     6,
      92,   191,   192,     6,   168,    59,   161,   162,   164,   161,
     179,   163,   163,   125,   164,   129,   161,   164,     0,   123,
     136,    14,   137,   191,   124,    47,     7,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    91,    92,
     168,   168,   168,   168,   168,   168,   125,   168,   137,   161,
     164,   126,   161,   186,   164,   164,   164,   125,    10,    11,
     127,     7,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    91,    92,
      47,   160,    47,   160,   125,   149,   155,   156,   157,   124,
      51,   142,   154,   157,   188,   190,   158,   149,   165,    39,
      40,    43,    92,   149,    59,   127,   152,   130,    60,   130,
     164,   126,   129,   130,   135,   137,    74,     6,    41,   163,
     163,   163,   163,   163,   163,   163,   163,   163,   163,   163,
     163,   163,   165,   164,    59,    56,   126,   164,   167,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   164,   164,
     164,   164,   164,   164,   164,   164,   164,   164,   165,    41,
      41,     1,   150,   151,   153,   154,   157,    74,    59,   127,
     152,     6,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   122,
     193,   164,   191,   124,   123,   127,   129,   189,   191,   164,
     164,   165,   191,     6,   171,   172,   173,   174,   175,   176,
      60,   161,   127,   168,   171,   137,   164,   125,   160,   126,
     127,   164,   125,   127,   160,   160,   126,   126,   127,   152,
     164,    41,   157,   149,   182,    74,     6,    76,   129,   142,
     190,    74,   148,   148,    59,    56,   127,   152,    76,   127,
     152,   164,   129,   150,   164,   164,   164,   153,   158,   191,
      27,    28,   164,   164,    76,   164,   164,   174,   164,     6,
     126,   126,   126,   126,   126,   148,   164,   164,   164,   191,
      29,    51,   148,   164
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
# define YYLEX yylex (&yylval, &yylloc, parm)
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
        pp->model->addItem((yyvsp[(2) - (2)].item));
      ;}
    break;

  case 7:

    {
        ParserState* pp = static_cast<ParserState*>(parm);
        pp->model->addItem((yyvsp[(3) - (3)].item));
      ;}
    break;

  case 8:

    {
        ParserState* pp = static_cast<ParserState*>(parm);
        pp->model->addItem((yyvsp[(4) - (4)].item));
      ;}
    break;

  case 10:

    {
        ParserState* pp = static_cast<ParserState*>(parm);
        if (pp->parseDocComments) {
          pp->model->addDocComment((yyvsp[(1) - (1)].sValue));
        }
        free((yyvsp[(1) - (1)].sValue));
      ;}
    break;

  case 11:

    {
        ParserState* pp = static_cast<ParserState*>(parm);
        if (pp->parseDocComments) {
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

    { (yyval.vardeclexpr_v)=(yyvsp[(1) - (2)].vardeclexpr_v); ;}
    break;

  case 46:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>();
        if ((yyvsp[(1) - (1)].vardeclexpr)) (yyvsp[(1) - (1)].vardeclexpr)->toplevel(false);
        if ((yyval.vardeclexpr_v)) (yyval.vardeclexpr_v)->push_back((yyvsp[(1) - (1)].vardeclexpr)); ;}
    break;

  case 47:

    { (yyval.vardeclexpr_v)=(yyvsp[(1) - (3)].vardeclexpr_v);
        if ((yyvsp[(3) - (3)].vardeclexpr)) (yyvsp[(3) - (3)].vardeclexpr)->toplevel(false);
        if ((yyvsp[(1) - (3)].vardeclexpr_v)) (yyvsp[(1) - (3)].vardeclexpr_v)->push_back((yyvsp[(3) - (3)].vardeclexpr)); ;}
    break;

  case 50:

    { (yyval.vardeclexpr)=(yyvsp[(1) - (1)].vardeclexpr); ;}
    break;

  case 51:

    { (yyval.vardeclexpr)=new VarDecl((yyloc), (yyvsp[(1) - (1)].tiexpr), ""); ;}
    break;

  case 52:

    { (yyval.vardeclexpr) = new VarDecl((yyloc), (yyvsp[(1) - (3)].tiexpr), (yyvsp[(3) - (3)].sValue));
        free((yyvsp[(3) - (3)].sValue));
      ;}
    break;

  case 53:

    { (yyval.tiexpr_v)=(yyvsp[(1) - (2)].tiexpr_v); ;}
    break;

  case 54:

    { (yyval.tiexpr_v)=new vector<TypeInst*>(); (yyval.tiexpr_v)->push_back((yyvsp[(1) - (1)].tiexpr)); ;}
    break;

  case 55:

    { (yyval.tiexpr_v)=(yyvsp[(1) - (3)].tiexpr_v); if ((yyvsp[(1) - (3)].tiexpr_v)) (yyvsp[(1) - (3)].tiexpr_v)->push_back((yyvsp[(3) - (3)].tiexpr)); ;}
    break;

  case 57:

    {
        (yyval.tiexpr) = (yyvsp[(6) - (6)].tiexpr);
        if ((yyval.tiexpr) && (yyvsp[(3) - (6)].tiexpr_v)) (yyval.tiexpr)->setRanges(*(yyvsp[(3) - (6)].tiexpr_v));
        delete (yyvsp[(3) - (6)].tiexpr_v);
      ;}
    break;

  case 58:

    {
        (yyval.tiexpr) = (yyvsp[(3) - (3)].tiexpr);
        std::vector<TypeInst*> ti(1);
        ti[0] = new TypeInst((yyloc),Type::parint());
        if ((yyval.tiexpr)) (yyval.tiexpr)->setRanges(ti);
      ;}
    break;

  case 59:

    { (yyval.tiexpr) = (yyvsp[(1) - (1)].tiexpr);
      ;}
    break;

  case 60:

    { (yyval.tiexpr) = (yyvsp[(2) - (2)].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 61:

    { (yyval.tiexpr) = (yyvsp[(3) - (3)].tiexpr);
        if ((yyval.tiexpr) && (yyvsp[(2) - (3)].bValue)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 62:

    { (yyval.tiexpr) = (yyvsp[(3) - (3)].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ti(Type::TI_VAR);
          if ((yyvsp[(2) - (3)].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 63:

    { (yyval.tiexpr) = (yyvsp[(4) - (4)].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.st(Type::ST_SET);
          if ((yyvsp[(1) - (4)].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 64:

    { (yyval.tiexpr) = (yyvsp[(5) - (5)].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.st(Type::ST_SET);
          if ((yyvsp[(2) - (5)].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 65:

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

  case 66:

    { (yyval.bValue) = false; ;}
    break;

  case 67:

    { (yyval.bValue) = true; ;}
    break;

  case 68:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parint()); ;}
    break;

  case 69:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parbool()); ;}
    break;

  case 70:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parfloat()); ;}
    break;

  case 71:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parstring()); ;}
    break;

  case 72:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::ann()); ;}
    break;

  case 73:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type(),(yyvsp[(1) - (1)].expression)); ;}
    break;

  case 74:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::top(),
                         new TIId((yyloc), (yyvsp[(1) - (1)].sValue)));
        free((yyvsp[(1) - (1)].sValue));
      ;}
    break;

  case 76:

    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].expression)); ;}
    break;

  case 77:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); if ((yyval.expression_v)) (yyval.expression_v)->push_back((yyvsp[(3) - (3)].expression)); ;}
    break;

  case 79:

    { if ((yyvsp[(1) - (3)].expression) && (yyvsp[(3) - (3)].expression)) (yyvsp[(1) - (3)].expression)->addAnnotation((yyvsp[(3) - (3)].expression)); (yyval.expression)=(yyvsp[(1) - (3)].expression); ;}
    break;

  case 80:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_UNION, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 81:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 82:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SYMDIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 83:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DOTDOT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 84:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(3) - (6)].expression), BOT_DOTDOT, (yyvsp[(5) - (6)].expression)); ;}
    break;

  case 85:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_INTERSECT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 86:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUSPLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 87:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 88:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MINUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 89:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MULT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 90:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 91:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IDIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 92:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MOD, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 93:

    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=new Call((yyloc), (yyvsp[(2) - (3)].sValue), args);
        free((yyvsp[(2) - (3)].sValue));
      ;}
    break;

  case 94:

    { (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 95:

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

  case 97:

    { if ((yyvsp[(1) - (3)].expression) && (yyvsp[(3) - (3)].expression)) (yyvsp[(1) - (3)].expression)->addAnnotation((yyvsp[(3) - (3)].expression)); (yyval.expression)=(yyvsp[(1) - (3)].expression); ;}
    break;

  case 98:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_EQUIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 99:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IMPL, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 100:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_RIMPL, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 101:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_OR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 102:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_XOR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 103:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_AND, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 104:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_LE, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 105:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_GR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 106:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_LQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 107:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_GQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 108:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_EQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 109:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_NQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 110:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IN, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 111:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SUBSET, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 112:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SUPERSET, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 113:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_UNION, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 114:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 115:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SYMDIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 116:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DOTDOT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 117:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(3) - (6)].expression), BOT_DOTDOT, (yyvsp[(5) - (6)].expression)); ;}
    break;

  case 118:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_INTERSECT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 119:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUSPLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 120:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 121:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MINUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 122:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MULT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 123:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 124:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IDIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 125:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MOD, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 126:

    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=new Call((yyloc), (yyvsp[(2) - (3)].sValue), args);
        free((yyvsp[(2) - (3)].sValue));
      ;}
    break;

  case 127:

    { (yyval.expression)=new UnOp((yyloc), UOT_NOT, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 128:

    { if (((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<IntLit>()) || ((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<FloatLit>())) {
          (yyval.expression) = (yyvsp[(2) - (2)].expression);
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[(2) - (2)].expression));
        }
      ;}
    break;

  case 129:

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

  case 130:

    { (yyval.expression)=(yyvsp[(2) - (3)].expression); ;}
    break;

  case 131:

    { (yyval.expression)=new ArrayAccess((yyloc), (yyvsp[(2) - (4)].expression), *(yyvsp[(4) - (4)].expression_v)); delete (yyvsp[(4) - (4)].expression_v); ;}
    break;

  case 132:

    { (yyval.expression)=new Id((yyloc), (yyvsp[(1) - (1)].sValue), NULL); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 133:

    { (yyval.expression)=new ArrayAccess((yyloc), new Id((yylsp[(1) - (2)]),(yyvsp[(1) - (2)].sValue),NULL), *(yyvsp[(2) - (2)].expression_v));
        free((yyvsp[(1) - (2)].sValue)); delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 134:

    { (yyval.expression)=new AnonVar((yyloc)); ;}
    break;

  case 135:

    { (yyval.expression)=new ArrayAccess((yyloc), new AnonVar((yyloc)), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 136:

    { (yyval.expression)=new BoolLit((yyloc), ((yyvsp[(1) - (1)].iValue)!=0)); ;}
    break;

  case 137:

    { (yyval.expression)=new IntLit((yyloc), (yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 138:

    { (yyval.expression)=new IntLit((yyloc), IntVal::infinity); ;}
    break;

  case 139:

    { (yyval.expression)=new FloatLit((yyloc), (yyvsp[(1) - (1)].dValue)); ;}
    break;

  case 141:

    { (yyval.expression)=constants().absent; ;}
    break;

  case 143:

    { (yyval.expression)=new ArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 145:

    { (yyval.expression)=new ArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 147:

    { (yyval.expression)=new ArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 149:

    { (yyval.expression)=new ArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 151:

    { (yyval.expression)=new ArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 153:

    { (yyval.expression)=new ArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 156:

    { (yyval.expression)=new ArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 157:

    { (yyval.expression)=new StringLit((yyloc), (yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 158:

    { (yyval.expression)=new BinOp((yyloc), new StringLit((yyloc), (yyvsp[(1) - (2)].sValue)), BOT_PLUSPLUS, (yyvsp[(2) - (2)].expression));
        free((yyvsp[(1) - (2)].sValue));
      ;}
    break;

  case 159:

    { (yyval.expression)=new BinOp((yyloc), new Call((yyloc), ASTString("format"), *(yyvsp[(1) - (2)].expression_v)), BOT_PLUSPLUS, new StringLit((yyloc),(yyvsp[(2) - (2)].sValue)));
        free((yyvsp[(2) - (2)].sValue));
        delete (yyvsp[(1) - (2)].expression_v);
      ;}
    break;

  case 160:

    { (yyval.expression)=new BinOp((yyloc), new Call((yyloc), ASTString("format"), *(yyvsp[(1) - (3)].expression_v)), BOT_PLUSPLUS,
                     new BinOp((yyloc), new StringLit((yyloc),(yyvsp[(2) - (3)].sValue)), BOT_PLUSPLUS, (yyvsp[(3) - (3)].expression)));
        free((yyvsp[(2) - (3)].sValue));
        delete (yyvsp[(1) - (3)].expression_v);
      ;}
    break;

  case 161:

    { (yyval.expression_v)=(yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 162:

    { (yyval.expression) = new SetLit((yyloc), std::vector<Expression*>()); ;}
    break;

  case 163:

    { (yyval.expression) = new SetLit((yyloc), *(yyvsp[(2) - (3)].expression_v)); delete (yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 164:

    { (yyval.expression) = new Comprehension((yyloc), (yyvsp[(2) - (5)].expression), *(yyvsp[(4) - (5)].generators), true);
        delete (yyvsp[(4) - (5)].generators);
      ;}
    break;

  case 165:

    { (yyval.generators)=new Generators; (yyval.generators)->_g = *(yyvsp[(1) - (1)].generator_v); (yyval.generators)->_w = NULL; delete (yyvsp[(1) - (1)].generator_v); ;}
    break;

  case 166:

    { (yyval.generators)=new Generators; (yyval.generators)->_g = *(yyvsp[(1) - (3)].generator_v); (yyval.generators)->_w = (yyvsp[(3) - (3)].expression); delete (yyvsp[(1) - (3)].generator_v); ;}
    break;

  case 168:

    { (yyval.generator_v)=new std::vector<Generator>; (yyval.generator_v)->push_back(*(yyvsp[(1) - (1)].generator)); delete (yyvsp[(1) - (1)].generator); ;}
    break;

  case 169:

    { (yyval.generator_v)=(yyvsp[(1) - (3)].generator_v); (yyval.generator_v)->push_back(*(yyvsp[(3) - (3)].generator)); delete (yyvsp[(3) - (3)].generator); ;}
    break;

  case 170:

    { (yyval.generator)=new Generator(*(yyvsp[(1) - (3)].string_v),(yyvsp[(3) - (3)].expression)); delete (yyvsp[(1) - (3)].string_v); ;}
    break;

  case 172:

    { (yyval.string_v)=new std::vector<std::string>; (yyval.string_v)->push_back((yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 173:

    { (yyval.string_v)=(yyvsp[(1) - (3)].string_v); (yyval.string_v)->push_back((yyvsp[(3) - (3)].sValue)); free((yyvsp[(3) - (3)].sValue)); ;}
    break;

  case 174:

    { (yyval.expression)=new ArrayLit((yyloc), std::vector<MiniZinc::Expression*>()); ;}
    break;

  case 175:

    { (yyval.expression)=new ArrayLit((yyloc), *(yyvsp[(2) - (3)].expression_v)); delete (yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 176:

    { if ((yyvsp[(2) - (3)].expression_vv)) {
          (yyval.expression)=new ArrayLit((yyloc), *(yyvsp[(2) - (3)].expression_vv));
          for (unsigned int i=1; i<(yyvsp[(2) - (3)].expression_vv)->size(); i++)
            if ((*(yyvsp[(2) - (3)].expression_vv))[i].size() != (*(yyvsp[(2) - (3)].expression_vv))[i-1].size())
              yyerror(&(yylsp[(2) - (3)]), parm, "syntax error, all sub-arrays of 2d array literal must have the same length");
          delete (yyvsp[(2) - (3)].expression_vv);
        }
      ;}
    break;

  case 177:

    { if ((yyvsp[(2) - (4)].expression_vv)) {
          (yyval.expression)=new ArrayLit((yyloc), *(yyvsp[(2) - (4)].expression_vv));
          for (unsigned int i=1; i<(yyvsp[(2) - (4)].expression_vv)->size(); i++)
            if ((*(yyvsp[(2) - (4)].expression_vv))[i].size() != (*(yyvsp[(2) - (4)].expression_vv))[i-1].size())
              yyerror(&(yylsp[(2) - (4)]), parm, "syntax error, all sub-arrays of 2d array literal must have the same length");
          delete (yyvsp[(2) - (4)].expression_vv);
        }
      ;}
    break;

  case 178:

    { (yyval.expression_vv)=new std::vector<std::vector<MiniZinc::Expression*> >;
        (yyval.expression_vv)->push_back(*(yyvsp[(1) - (1)].expression_v));
        delete (yyvsp[(1) - (1)].expression_v);
      ;}
    break;

  case 179:

    { (yyval.expression_vv)=(yyvsp[(1) - (3)].expression_vv); if ((yyval.expression_vv)) (yyval.expression_vv)->push_back(*(yyvsp[(3) - (3)].expression_v)); delete (yyvsp[(3) - (3)].expression_v); ;}
    break;

  case 180:

    { (yyval.expression)=new Comprehension((yyloc), (yyvsp[(2) - (5)].expression), *(yyvsp[(4) - (5)].generators), false);
        delete (yyvsp[(4) - (5)].generators);
      ;}
    break;

  case 181:

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

  case 182:

    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; ;}
    break;

  case 183:

    { (yyval.expression_v)=(yyvsp[(1) - (5)].expression_v); if ((yyval.expression_v)) { (yyval.expression_v)->push_back((yyvsp[(3) - (5)].expression)); (yyval.expression_v)->push_back((yyvsp[(5) - (5)].expression)); } ;}
    break;

  case 184:

    { (yyval.iValue)=BOT_EQUIV; ;}
    break;

  case 185:

    { (yyval.iValue)=BOT_IMPL; ;}
    break;

  case 186:

    { (yyval.iValue)=BOT_RIMPL; ;}
    break;

  case 187:

    { (yyval.iValue)=BOT_OR; ;}
    break;

  case 188:

    { (yyval.iValue)=BOT_XOR; ;}
    break;

  case 189:

    { (yyval.iValue)=BOT_AND; ;}
    break;

  case 190:

    { (yyval.iValue)=BOT_LE; ;}
    break;

  case 191:

    { (yyval.iValue)=BOT_GR; ;}
    break;

  case 192:

    { (yyval.iValue)=BOT_LQ; ;}
    break;

  case 193:

    { (yyval.iValue)=BOT_GQ; ;}
    break;

  case 194:

    { (yyval.iValue)=BOT_EQ; ;}
    break;

  case 195:

    { (yyval.iValue)=BOT_NQ; ;}
    break;

  case 196:

    { (yyval.iValue)=BOT_IN; ;}
    break;

  case 197:

    { (yyval.iValue)=BOT_SUBSET; ;}
    break;

  case 198:

    { (yyval.iValue)=BOT_SUPERSET; ;}
    break;

  case 199:

    { (yyval.iValue)=BOT_UNION; ;}
    break;

  case 200:

    { (yyval.iValue)=BOT_DIFF; ;}
    break;

  case 201:

    { (yyval.iValue)=BOT_SYMDIFF; ;}
    break;

  case 202:

    { (yyval.iValue)=BOT_PLUS; ;}
    break;

  case 203:

    { (yyval.iValue)=BOT_MINUS; ;}
    break;

  case 204:

    { (yyval.iValue)=BOT_MULT; ;}
    break;

  case 205:

    { (yyval.iValue)=BOT_DIV; ;}
    break;

  case 206:

    { (yyval.iValue)=BOT_IDIV; ;}
    break;

  case 207:

    { (yyval.iValue)=BOT_MOD; ;}
    break;

  case 208:

    { (yyval.iValue)=BOT_INTERSECT; ;}
    break;

  case 209:

    { (yyval.iValue)=BOT_PLUSPLUS; ;}
    break;

  case 210:

    { (yyval.iValue)=-1; ;}
    break;

  case 211:

    { if ((yyvsp[(1) - (6)].iValue)==-1) {
          (yyval.expression)=NULL;
          yyerror(&(yylsp[(3) - (6)]), parm, "syntax error, unary operator with two arguments");
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[(3) - (6)].expression),static_cast<BinOpType>((yyvsp[(1) - (6)].iValue)),(yyvsp[(5) - (6)].expression));
        }
      ;}
    break;

  case 212:

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

  case 213:

    { (yyval.expression)=new Call((yyloc), (yyvsp[(1) - (3)].sValue), std::vector<Expression*>()); free((yyvsp[(1) - (3)].sValue)); ;}
    break;

  case 215:

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

  case 216:

    { 
        vector<Generator> gens;
        vector<ASTString> ids;
        if ((yyvsp[(3) - (7)].expression_p)) {
          for (unsigned int i=0; i<(yyvsp[(3) - (7)].expression_p)->first.size(); i++) {
            if (Id* id = Expression::dyn_cast<Id>((yyvsp[(3) - (7)].expression_p)->first[i])) {
              ids.push_back(id->v());
            } else {
              if (BinOp* boe = Expression::dyn_cast<BinOp>((yyvsp[(3) - (7)].expression_p)->first[i])) {
                Id* id = Expression::dyn_cast<Id>(boe->lhs());
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

  case 217:

    { (yyval.expression_p)=new pair<vector<Expression*>,Expression*>;
        (yyval.expression_p)->first=*(yyvsp[(1) - (1)].expression_v); (yyval.expression_p)->second=NULL;
        delete (yyvsp[(1) - (1)].expression_v);
      ;}
    break;

  case 218:

    { (yyval.expression_p)=new pair<vector<Expression*>,Expression*>;
        (yyval.expression_p)->first=*(yyvsp[(1) - (3)].expression_v); (yyval.expression_p)->second=(yyvsp[(3) - (3)].expression);
        delete (yyvsp[(1) - (3)].expression_v);
      ;}
    break;

  case 219:

    { (yyval.expression)=new Let((yyloc), *(yyvsp[(3) - (6)].expression_v), (yyvsp[(6) - (6)].expression)); delete (yyvsp[(3) - (6)].expression_v); ;}
    break;

  case 220:

    { (yyval.expression)=new Let((yyloc), *(yyvsp[(3) - (7)].expression_v), (yyvsp[(7) - (7)].expression)); delete (yyvsp[(3) - (7)].expression_v); ;}
    break;

  case 221:

    { (yyval.expression_v)=new vector<Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].vardeclexpr)); ;}
    break;

  case 222:

    { (yyval.expression_v)=new vector<Expression*>;
        if ((yyvsp[(1) - (1)].item)) {
          ConstraintI* ce = (yyvsp[(1) - (1)].item)->cast<ConstraintI>();
          (yyval.expression_v)->push_back(ce->e());
          ce->e(NULL);
        }
      ;}
    break;

  case 223:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); (yyval.expression_v)->push_back((yyvsp[(3) - (3)].vardeclexpr)); ;}
    break;

  case 224:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v);
        if ((yyvsp[(3) - (3)].item)) {
          ConstraintI* ce = (yyvsp[(3) - (3)].item)->cast<ConstraintI>();
          (yyval.expression_v)->push_back(ce->e());
          ce->e(NULL);
        }
      ;}
    break;

  case 227:

    { (yyval.vardeclexpr) = (yyvsp[(1) - (2)].vardeclexpr);
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->toplevel(false);
        if ((yyval.vardeclexpr) && (yyvsp[(2) - (2)].expression_v)) (yyval.vardeclexpr)->addAnnotations(*(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v);
      ;}
    break;

  case 228:

    { if ((yyvsp[(1) - (4)].vardeclexpr)) (yyvsp[(1) - (4)].vardeclexpr)->e((yyvsp[(4) - (4)].expression));
        (yyval.vardeclexpr) = (yyvsp[(1) - (4)].vardeclexpr);
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->loc((yyloc));
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->toplevel(false);
        if ((yyval.vardeclexpr) && (yyvsp[(2) - (4)].expression_v)) (yyval.vardeclexpr)->addAnnotations(*(yyvsp[(2) - (4)].expression_v));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 229:

    { (yyval.expression_v)=NULL; ;}
    break;

  case 231:

    { (yyval.expression_v)=new std::vector<Expression*>(1);
        (*(yyval.expression_v))[0] = (yyvsp[(2) - (2)].expression);
      ;}
    break;

  case 232:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); if ((yyval.expression_v)) (yyval.expression_v)->push_back((yyvsp[(3) - (3)].expression)); ;}
    break;

  case 233:

    { (yyval.sValue)=(yyvsp[(1) - (1)].sValue); ;}
    break;

  case 234:

    { (yyval.sValue)=strdup("<->"); ;}
    break;

  case 235:

    { (yyval.sValue)=strdup("->"); ;}
    break;

  case 236:

    { (yyval.sValue)=strdup("<-"); ;}
    break;

  case 237:

    { (yyval.sValue)=strdup("\\/"); ;}
    break;

  case 238:

    { (yyval.sValue)=strdup("xor"); ;}
    break;

  case 239:

    { (yyval.sValue)=strdup("/\\"); ;}
    break;

  case 240:

    { (yyval.sValue)=strdup("<"); ;}
    break;

  case 241:

    { (yyval.sValue)=strdup(">"); ;}
    break;

  case 242:

    { (yyval.sValue)=strdup("<="); ;}
    break;

  case 243:

    { (yyval.sValue)=strdup(">="); ;}
    break;

  case 244:

    { (yyval.sValue)=strdup("="); ;}
    break;

  case 245:

    { (yyval.sValue)=strdup("!="); ;}
    break;

  case 246:

    { (yyval.sValue)=strdup("in"); ;}
    break;

  case 247:

    { (yyval.sValue)=strdup("subset"); ;}
    break;

  case 248:

    { (yyval.sValue)=strdup("superset"); ;}
    break;

  case 249:

    { (yyval.sValue)=strdup("union"); ;}
    break;

  case 250:

    { (yyval.sValue)=strdup("diff"); ;}
    break;

  case 251:

    { (yyval.sValue)=strdup("symdiff"); ;}
    break;

  case 252:

    { (yyval.sValue)=strdup(".."); ;}
    break;

  case 253:

    { (yyval.sValue)=strdup("+"); ;}
    break;

  case 254:

    { (yyval.sValue)=strdup("-"); ;}
    break;

  case 255:

    { (yyval.sValue)=strdup("*"); ;}
    break;

  case 256:

    { (yyval.sValue)=strdup("/"); ;}
    break;

  case 257:

    { (yyval.sValue)=strdup("div"); ;}
    break;

  case 258:

    { (yyval.sValue)=strdup("mod"); ;}
    break;

  case 259:

    { (yyval.sValue)=strdup("intersect"); ;}
    break;

  case 260:

    { (yyval.sValue)=strdup("not"); ;}
    break;

  case 261:

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



