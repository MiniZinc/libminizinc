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
     MZN_ABSENT = 268,
     MZN_ANN = 269,
     MZN_ANNOTATION = 270,
     MZN_ANY = 271,
     MZN_ARRAY = 272,
     MZN_BOOL = 273,
     MZN_CASE = 274,
     MZN_CONSTRAINT = 275,
     MZN_DEFAULT = 276,
     MZN_ELSE = 277,
     MZN_ELSEIF = 278,
     MZN_ENDIF = 279,
     MZN_ENUM = 280,
     MZN_FLOAT = 281,
     MZN_FUNCTION = 282,
     MZN_IF = 283,
     MZN_INCLUDE = 284,
     MZN_INFINITY = 285,
     MZN_INT = 286,
     MZN_LET = 287,
     MZN_LIST = 288,
     MZN_MAXIMIZE = 289,
     MZN_MINIMIZE = 290,
     MZN_OF = 291,
     MZN_OPT = 292,
     MZN_SATISFY = 293,
     MZN_OUTPUT = 294,
     MZN_PREDICATE = 295,
     MZN_RECORD = 296,
     MZN_SET = 297,
     MZN_SOLVE = 298,
     MZN_STRING = 299,
     MZN_TEST = 300,
     MZN_THEN = 301,
     MZN_TUPLE = 302,
     MZN_TYPE = 303,
     MZN_UNDERSCORE = 304,
     MZN_VARIANT_RECORD = 305,
     MZN_WHERE = 306,
     MZN_LEFT_BRACKET = 307,
     MZN_LEFT_2D_BRACKET = 308,
     MZN_RIGHT_BRACKET = 309,
     MZN_RIGHT_2D_BRACKET = 310,
     UNKNOWN_CHAR = 311,
     FLATZINC_IDENTIFIER = 312,
     PREC_ANNO = 313,
     MZN_EQUIV = 314,
     MZN_RIMPL = 315,
     MZN_IMPL = 316,
     MZN_XOR = 317,
     MZN_OR = 318,
     MZN_AND = 319,
     MZN_NQ = 320,
     MZN_EQ = 321,
     MZN_GQ = 322,
     MZN_LQ = 323,
     MZN_GR = 324,
     MZN_LE = 325,
     MZN_SUPERSET = 326,
     MZN_SUBSET = 327,
     MZN_IN = 328,
     MZN_SYMDIFF = 329,
     MZN_DIFF = 330,
     MZN_UNION = 331,
     MZN_DOTDOT = 332,
     MZN_MINUS = 333,
     MZN_PLUS = 334,
     MZN_INTERSECT = 335,
     MZN_MOD = 336,
     MZN_IDIV = 337,
     MZN_DIV = 338,
     MZN_MULT = 339,
     MZN_NOT = 340,
     MZN_PLUSPLUS = 341,
     MZN_COLONCOLON = 342,
     MZN_EQUIV_QUOTED = 343,
     MZN_IMPL_QUOTED = 344,
     MZN_RIMPL_QUOTED = 345,
     MZN_OR_QUOTED = 346,
     MZN_XOR_QUOTED = 347,
     MZN_AND_QUOTED = 348,
     MZN_LE_QUOTED = 349,
     MZN_GR_QUOTED = 350,
     MZN_LQ_QUOTED = 351,
     MZN_GQ_QUOTED = 352,
     MZN_EQ_QUOTED = 353,
     MZN_NQ_QUOTED = 354,
     MZN_IN_QUOTED = 355,
     MZN_SUBSET_QUOTED = 356,
     MZN_SUPERSET_QUOTED = 357,
     MZN_UNION_QUOTED = 358,
     MZN_DIFF_QUOTED = 359,
     MZN_SYMDIFF_QUOTED = 360,
     MZN_DOTDOT_QUOTED = 361,
     MZN_PLUS_QUOTED = 362,
     MZN_MINUS_QUOTED = 363,
     MZN_MULT_QUOTED = 364,
     MZN_DIV_QUOTED = 365,
     MZN_IDIV_QUOTED = 366,
     MZN_MOD_QUOTED = 367,
     MZN_INTERSECT_QUOTED = 368,
     MZN_NOT_QUOTED = 369,
     MZN_COLONCOLON_QUOTED = 370,
     MZN_PLUSPLUS_QUOTED = 371
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
#define MZN_ABSENT 268
#define MZN_ANN 269
#define MZN_ANNOTATION 270
#define MZN_ANY 271
#define MZN_ARRAY 272
#define MZN_BOOL 273
#define MZN_CASE 274
#define MZN_CONSTRAINT 275
#define MZN_DEFAULT 276
#define MZN_ELSE 277
#define MZN_ELSEIF 278
#define MZN_ENDIF 279
#define MZN_ENUM 280
#define MZN_FLOAT 281
#define MZN_FUNCTION 282
#define MZN_IF 283
#define MZN_INCLUDE 284
#define MZN_INFINITY 285
#define MZN_INT 286
#define MZN_LET 287
#define MZN_LIST 288
#define MZN_MAXIMIZE 289
#define MZN_MINIMIZE 290
#define MZN_OF 291
#define MZN_OPT 292
#define MZN_SATISFY 293
#define MZN_OUTPUT 294
#define MZN_PREDICATE 295
#define MZN_RECORD 296
#define MZN_SET 297
#define MZN_SOLVE 298
#define MZN_STRING 299
#define MZN_TEST 300
#define MZN_THEN 301
#define MZN_TUPLE 302
#define MZN_TYPE 303
#define MZN_UNDERSCORE 304
#define MZN_VARIANT_RECORD 305
#define MZN_WHERE 306
#define MZN_LEFT_BRACKET 307
#define MZN_LEFT_2D_BRACKET 308
#define MZN_RIGHT_BRACKET 309
#define MZN_RIGHT_2D_BRACKET 310
#define UNKNOWN_CHAR 311
#define FLATZINC_IDENTIFIER 312
#define PREC_ANNO 313
#define MZN_EQUIV 314
#define MZN_RIMPL 315
#define MZN_IMPL 316
#define MZN_XOR 317
#define MZN_OR 318
#define MZN_AND 319
#define MZN_NQ 320
#define MZN_EQ 321
#define MZN_GQ 322
#define MZN_LQ 323
#define MZN_GR 324
#define MZN_LE 325
#define MZN_SUPERSET 326
#define MZN_SUBSET 327
#define MZN_IN 328
#define MZN_SYMDIFF 329
#define MZN_DIFF 330
#define MZN_UNION 331
#define MZN_DOTDOT 332
#define MZN_MINUS 333
#define MZN_PLUS 334
#define MZN_INTERSECT 335
#define MZN_MOD 336
#define MZN_IDIV 337
#define MZN_DIV 338
#define MZN_MULT 339
#define MZN_NOT 340
#define MZN_PLUSPLUS 341
#define MZN_COLONCOLON 342
#define MZN_EQUIV_QUOTED 343
#define MZN_IMPL_QUOTED 344
#define MZN_RIMPL_QUOTED 345
#define MZN_OR_QUOTED 346
#define MZN_XOR_QUOTED 347
#define MZN_AND_QUOTED 348
#define MZN_LE_QUOTED 349
#define MZN_GR_QUOTED 350
#define MZN_LQ_QUOTED 351
#define MZN_GQ_QUOTED 352
#define MZN_EQ_QUOTED 353
#define MZN_NQ_QUOTED 354
#define MZN_IN_QUOTED 355
#define MZN_SUBSET_QUOTED 356
#define MZN_SUPERSET_QUOTED 357
#define MZN_UNION_QUOTED 358
#define MZN_DIFF_QUOTED 359
#define MZN_SYMDIFF_QUOTED 360
#define MZN_DOTDOT_QUOTED 361
#define MZN_PLUS_QUOTED 362
#define MZN_MINUS_QUOTED 363
#define MZN_MULT_QUOTED 364
#define MZN_DIV_QUOTED 365
#define MZN_IDIV_QUOTED 366
#define MZN_MOD_QUOTED 367
#define MZN_INTERSECT_QUOTED 368
#define MZN_NOT_QUOTED 369
#define MZN_COLONCOLON_QUOTED 370
#define MZN_PLUSPLUS_QUOTED 371




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
    model->_filename = ASTString(filename);

    if (!ignoreStdlib) {
      Model* stdlib = new Model;
      stdlib->_filename = ASTString("stdlib.mzn");
      files.push_back(pair<string,Model*>("./",stdlib));
      seenModels.insert(pair<string,Model*>("stdlib.mzn",stdlib));
      IncludeI* stdlibinc = new IncludeI(Location(),stdlib->_filename);
      stdlibinc->m(stdlib,true);
      model->addItem(stdlibinc);

      Model* builtins = new Model;
      ASTString bn("builtins.mzn");
      builtins->_filename = bn;
      files.push_back(pair<string,Model*>("./",builtins));
      seenModels.insert(pair<string,Model*>("builtins.mzn",builtins));
      IncludeI* builtinsinc = new IncludeI(Location(),builtins->_filename);
      builtinsinc->m(builtins,true);
      model->addItem(builtinsinc);
    }

    model->_filepath = ASTString(filename);
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
      string f(m->_filename.str());

      for (Model* p=m->_parent; p; p=p->_parent) {
        if (f == p->_filename.c_str()) {
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

      m->_filepath = ASTString(fullname);
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
    model->_filename = ASTString(fileBasename);

    if (!ignoreStdlib) {
      Model* stdlib = new Model;
      stdlib->_filename = ASTString("stdlib.mzn");
      files.push_back(pair<string,Model*>("./",stdlib));
      seenModels.insert(pair<string,Model*>("stdlib.mzn",stdlib));
      Location stdlibloc;
      stdlibloc.filename=ASTString(filename);
      IncludeI* stdlibinc = 
        new IncludeI(stdlibloc,stdlib->_filename);
      stdlibinc->m(stdlib,true);
      model->addItem(stdlibinc);

      Model* builtins = new Model;
      ASTString bn("builtins.mzn");
      builtins->_filename = bn;
      files.push_back(pair<string,Model*>("./",builtins));
      seenModels.insert(pair<string,Model*>("builtins.mzn",builtins));
      Location builtinsloc;
      builtinsloc.filename=ASTString(filename);
      IncludeI* builtinsinc = 
        new IncludeI(builtinsloc,builtins->_filename);
      builtinsinc->m(builtins,true);
      model->addItem(builtinsinc);
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
            err << "  " << pe->_filename << std::endl;
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

      m->_filepath = ASTString(fullname);
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
    stdlib->_filename = ASTString("stdlib.mzn");
    files.push_back(pair<string,Model*>("./",stdlib));
    seenModels.insert(pair<string,Model*>("stdlib.mzn",stdlib));
    IncludeI* stdlibinc =
      new IncludeI(Location(),stdlib->_filename);
    stdlibinc->m(stdlib,true);
    model->addItem(stdlibinc);
    
    Model* builtins = new Model;
    ASTString bn("builtins.mzn");
    builtins->_filename = bn;
    files.push_back(pair<string,Model*>("./",builtins));
    seenModels.insert(pair<string,Model*>("builtins.mzn",builtins));
    IncludeI* builtinsinc =
      new IncludeI(Location(),builtins->_filename);
    builtinsinc->m(builtins,true);
    model->addItem(builtinsinc);
  }
  
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
          err << "  " << pe->_filename << std::endl;
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
    
    m->_filepath = ASTString(fullname);
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
#define YYFINAL  137
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   3222

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  125
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  59
/* YYNRULES -- Number of rules.  */
#define YYNRULES  248
/* YYNRULES -- Number of states.  */
#define YYNSTATES  422

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   371

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     119,   120,     2,     2,   121,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   118,   117,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   122,   124,   123,     2,     2,     2,     2,
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
     115,   116
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
     128,   132,   133,   135,   137,   139,   143,   146,   148,   152,
     154,   161,   165,   167,   170,   174,   178,   183,   189,   195,
     196,   198,   200,   202,   204,   206,   208,   210,   212,   215,
     217,   221,   223,   227,   231,   235,   239,   243,   250,   254,
     258,   262,   266,   270,   274,   278,   282,   286,   289,   292,
     294,   298,   302,   306,   310,   314,   318,   322,   326,   330,
     334,   338,   342,   346,   350,   354,   358,   362,   366,   370,
     374,   381,   385,   389,   393,   397,   401,   405,   409,   413,
     417,   420,   423,   426,   430,   435,   437,   440,   442,   445,
     447,   449,   451,   453,   455,   457,   459,   461,   463,   466,
     468,   471,   473,   476,   478,   481,   483,   485,   488,   492,
     495,   499,   505,   507,   511,   514,   516,   520,   524,   527,
     529,   533,   536,   540,   544,   549,   551,   555,   561,   570,
     571,   577,   579,   581,   583,   585,   587,   589,   591,   593,
     595,   597,   599,   601,   603,   605,   607,   609,   611,   613,
     615,   617,   619,   621,   623,   625,   627,   629,   631,   638,
     643,   647,   649,   654,   662,   664,   668,   675,   683,   685,
     687,   691,   695,   697,   699,   702,   707,   708,   710,   713,
     717,   719,   721,   723,   725,   727,   729,   731,   733,   735,
     737,   739,   741,   743,   745,   747,   749,   751,   753,   755,
     757,   759,   761,   763,   765,   767,   769,   771,   773
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     126,     0,    -1,   127,    -1,    -1,   128,   129,    -1,   130,
      -1,   128,   117,   130,    -1,    -1,   117,    -1,   131,    -1,
     132,    -1,   133,    -1,   134,    -1,   135,    -1,   136,    -1,
     137,    -1,   138,    -1,   139,    -1,    29,     8,    -1,   146,
     181,    -1,   146,   181,    66,   156,    -1,     6,    66,   156,
      -1,    20,   156,    -1,    43,   181,    38,    -1,    43,   181,
      35,   156,    -1,    43,   181,    34,   156,    -1,    39,   156,
      -1,    40,     6,   141,   181,   140,    -1,    45,     6,   141,
     181,   140,    -1,    27,   149,   118,   183,   141,   181,   140,
      -1,   149,   118,     6,   119,   142,   120,   181,   140,    -1,
      15,     6,   141,    -1,    15,     6,   141,    66,   156,    -1,
      -1,    66,   156,    -1,    -1,   119,   142,   120,    -1,   119,
       1,   120,    -1,   143,   144,    -1,   145,    -1,   143,   121,
     145,    -1,    -1,   121,    -1,   146,    -1,   149,    -1,   149,
     118,     6,    -1,   148,   144,    -1,   149,    -1,   148,   121,
     149,    -1,   150,    -1,    17,    52,   147,    54,    36,   150,
      -1,    33,    36,   150,    -1,   152,    -1,    37,   152,    -1,
      11,   151,   152,    -1,    10,   151,   152,    -1,   151,    42,
      36,   152,    -1,    11,   151,    42,    36,   152,    -1,    10,
     151,    42,    36,   152,    -1,    -1,    37,    -1,    31,    -1,
      18,    -1,    26,    -1,    44,    -1,    14,    -1,   155,    -1,
       9,    -1,   154,   144,    -1,   156,    -1,   154,   121,   156,
      -1,   157,    -1,   155,    87,   157,    -1,   155,    76,   155,
      -1,   155,    75,   155,    -1,   155,    74,   155,    -1,   155,
      77,   155,    -1,   106,   119,   156,   121,   156,   120,    -1,
     155,    80,   155,    -1,   155,    86,   155,    -1,   155,    79,
     155,    -1,   155,    78,   155,    -1,   155,    84,   155,    -1,
     155,    83,   155,    -1,   155,    82,   155,    -1,   155,    81,
     155,    -1,   155,     7,   155,    -1,    79,   155,    -1,    78,
     155,    -1,   157,    -1,   156,    87,   157,    -1,   156,    59,
     156,    -1,   156,    61,   156,    -1,   156,    60,   156,    -1,
     156,    63,   156,    -1,   156,    62,   156,    -1,   156,    64,
     156,    -1,   156,    70,   156,    -1,   156,    69,   156,    -1,
     156,    68,   156,    -1,   156,    67,   156,    -1,   156,    66,
     156,    -1,   156,    65,   156,    -1,   156,    73,   156,    -1,
     156,    72,   156,    -1,   156,    71,   156,    -1,   156,    76,
     156,    -1,   156,    75,   156,    -1,   156,    74,   156,    -1,
     156,    77,   156,    -1,   106,   119,   156,   121,   156,   120,
      -1,   156,    80,   156,    -1,   156,    86,   156,    -1,   156,
      79,   156,    -1,   156,    78,   156,    -1,   156,    84,   156,
      -1,   156,    83,   156,    -1,   156,    82,   156,    -1,   156,
      81,   156,    -1,   156,     7,   156,    -1,    85,   156,    -1,
      79,   156,    -1,    78,   156,    -1,   119,   156,   120,    -1,
     119,   156,   120,   158,    -1,     6,    -1,     6,   158,    -1,
      49,    -1,    49,   158,    -1,     4,    -1,     3,    -1,    30,
      -1,     5,    -1,     8,    -1,    13,    -1,   159,    -1,   160,
      -1,   167,    -1,   167,   158,    -1,   168,    -1,   168,   158,
      -1,   170,    -1,   170,   158,    -1,   171,    -1,   171,   158,
      -1,   177,    -1,   175,    -1,   175,   158,    -1,    52,   153,
      54,    -1,   122,   123,    -1,   122,   153,   123,    -1,   122,
     156,   124,   161,   123,    -1,   162,    -1,   162,    51,   156,
      -1,   163,   144,    -1,   164,    -1,   163,   121,   164,    -1,
     165,    73,   156,    -1,   166,   144,    -1,     6,    -1,   166,
     121,     6,    -1,    52,    54,    -1,    52,   153,    54,    -1,
      53,   169,    55,    -1,    53,   169,   124,    55,    -1,   153,
      -1,   169,   124,   153,    -1,    52,   156,   124,   161,    54,
      -1,    28,   156,    46,   156,   172,    22,   156,    24,    -1,
      -1,   172,    23,   156,    46,   156,    -1,    88,    -1,    89,
      -1,    90,    -1,    91,    -1,    92,    -1,    93,    -1,    94,
      -1,    95,    -1,    96,    -1,    97,    -1,    98,    -1,    99,
      -1,   100,    -1,   101,    -1,   102,    -1,   103,    -1,   104,
      -1,   105,    -1,   107,    -1,   108,    -1,   109,    -1,   110,
      -1,   111,    -1,   112,    -1,   113,    -1,   116,    -1,   114,
      -1,   173,   119,   156,   121,   156,   120,    -1,   173,   119,
     156,   120,    -1,     6,   119,   120,    -1,   174,    -1,     6,
     119,   176,   120,    -1,     6,   119,   176,   120,   119,   156,
     120,    -1,   153,    -1,   153,    51,   156,    -1,    32,   122,
     178,   123,    73,   156,    -1,    32,   122,   178,   179,   123,
      73,   156,    -1,   180,    -1,   134,    -1,   178,   179,   180,
      -1,   178,   179,   134,    -1,   121,    -1,   117,    -1,   146,
     181,    -1,   146,   181,    66,   156,    -1,    -1,   182,    -1,
      87,   157,    -1,   182,    87,   157,    -1,     6,    -1,    88,
      -1,    89,    -1,    90,    -1,    91,    -1,    92,    -1,    93,
      -1,    94,    -1,    95,    -1,    96,    -1,    97,    -1,    98,
      -1,    99,    -1,   100,    -1,   101,    -1,   102,    -1,   103,
      -1,   104,    -1,   105,    -1,   106,    -1,   107,    -1,   108,
      -1,   109,    -1,   110,    -1,   111,    -1,   112,    -1,   113,
      -1,   114,    -1,   116,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   610,   610,   612,   614,   617,   622,   628,   628,   631,
     633,   635,   636,   638,   640,   642,   644,   646,   650,   673,
     678,   686,   692,   696,   701,   706,   713,   717,   725,   735,
     742,   751,   763,   771,   772,   777,   778,   780,   783,   787,
     791,   796,   796,   799,   801,   805,   810,   814,   816,   820,
     821,   827,   836,   839,   845,   853,   860,   867,   874,   885,
     886,   890,   892,   894,   896,   898,   900,   902,   908,   911,
     913,   919,   920,   922,   924,   926,   928,   930,   932,   934,
     936,   938,   940,   942,   944,   946,   948,   954,   956,   971,
     972,   974,   976,   978,   980,   982,   984,   986,   988,   990,
     992,   994,   996,   998,  1000,  1002,  1004,  1006,  1008,  1010,
    1012,  1014,  1016,  1018,  1020,  1022,  1024,  1026,  1028,  1030,
    1036,  1038,  1045,  1058,  1060,  1062,  1064,  1067,  1069,  1072,
    1074,  1076,  1078,  1080,  1082,  1084,  1085,  1086,  1087,  1090,
    1091,  1094,  1095,  1098,  1099,  1102,  1103,  1104,  1109,  1113,
    1115,  1119,  1125,  1127,  1130,  1133,  1135,  1139,  1142,  1145,
    1147,  1151,  1153,  1157,  1164,  1173,  1178,  1182,  1188,  1202,
    1203,  1207,  1209,  1211,  1213,  1215,  1217,  1219,  1221,  1223,
    1225,  1227,  1229,  1231,  1233,  1235,  1237,  1239,  1241,  1243,
    1245,  1247,  1249,  1251,  1253,  1255,  1257,  1259,  1263,  1271,
    1303,  1305,  1306,  1317,  1356,  1361,  1368,  1370,  1390,  1392,
    1398,  1400,  1407,  1407,  1410,  1416,  1427,  1428,  1431,  1435,
    1439,  1441,  1443,  1445,  1447,  1449,  1451,  1453,  1455,  1457,
    1459,  1461,  1463,  1465,  1467,  1469,  1471,  1473,  1475,  1477,
    1479,  1481,  1483,  1485,  1487,  1489,  1491,  1493,  1495
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
  "MZN_VAR", "MZN_PAR", "MZN_SVAR", "MZN_ABSENT", "MZN_ANN",
  "MZN_ANNOTATION", "MZN_ANY", "MZN_ARRAY", "MZN_BOOL", "MZN_CASE",
  "MZN_CONSTRAINT", "MZN_DEFAULT", "MZN_ELSE", "MZN_ELSEIF", "MZN_ENDIF",
  "MZN_ENUM", "MZN_FLOAT", "MZN_FUNCTION", "MZN_IF", "MZN_INCLUDE",
  "MZN_INFINITY", "MZN_INT", "MZN_LET", "MZN_LIST", "MZN_MAXIMIZE",
  "MZN_MINIMIZE", "MZN_OF", "MZN_OPT", "MZN_SATISFY", "MZN_OUTPUT",
  "MZN_PREDICATE", "MZN_RECORD", "MZN_SET", "MZN_SOLVE", "MZN_STRING",
  "MZN_TEST", "MZN_THEN", "MZN_TUPLE", "MZN_TYPE", "MZN_UNDERSCORE",
  "MZN_VARIANT_RECORD", "MZN_WHERE", "MZN_LEFT_BRACKET",
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
  "item_list", "item_list_head", "semi_or_none", "item", "include_item",
  "vardecl_item", "assign_item", "constraint_item", "solve_item",
  "output_item", "predicate_item", "function_item", "annotation_item",
  "operation_item_tail", "params", "params_list", "params_list_head",
  "comma_or_none", "ti_expr_and_id_or_anon", "ti_expr_and_id",
  "ti_expr_list", "ti_expr_list_head", "ti_expr", "base_ti_expr",
  "opt_opt", "base_ti_expr_tail", "expr_list", "expr_list_head",
  "set_expr", "expr", "expr_atom_head", "array_access_tail", "set_literal",
  "set_comp", "comp_tail", "generator_list", "generator_list_head",
  "generator", "id_list", "id_list_head", "simple_array_literal",
  "simple_array_literal_2d", "simple_array_literal_2d_list",
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
     365,   366,   367,   368,   369,   370,   371,    59,    58,    40,
      41,    44,   123,   125,   124
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   125,   126,   127,   127,   128,   128,   129,   129,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   131,   132,
     132,   133,   134,   135,   135,   135,   136,   137,   137,   138,
     138,   139,   139,   140,   140,   141,   141,   141,   142,   143,
     143,   144,   144,   145,   145,   146,   147,   148,   148,   149,
     149,   149,   150,   150,   150,   150,   150,   150,   150,   151,
     151,   152,   152,   152,   152,   152,   152,   152,   153,   154,
     154,   155,   155,   155,   155,   155,   155,   155,   155,   155,
     155,   155,   155,   155,   155,   155,   155,   155,   155,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   157,   157,   157,   157,   157,   157,   157,
     157,   157,   157,   157,   157,   157,   157,   157,   157,   157,
     157,   157,   157,   157,   157,   157,   157,   157,   158,   159,
     159,   160,   161,   161,   162,   163,   163,   164,   165,   166,
     166,   167,   167,   168,   168,   169,   169,   170,   171,   172,
     172,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   174,   174,
     175,   175,   175,   175,   176,   176,   177,   177,   178,   178,
     178,   178,   179,   179,   180,   180,   181,   181,   182,   182,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     3,     0,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       4,     3,     2,     3,     4,     4,     2,     5,     5,     7,
       8,     3,     5,     0,     2,     0,     3,     3,     2,     1,
       3,     0,     1,     1,     1,     3,     2,     1,     3,     1,
       6,     3,     1,     2,     3,     3,     4,     5,     5,     0,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       3,     1,     3,     3,     3,     3,     3,     6,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     2,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       6,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     2,     2,     3,     4,     1,     2,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       2,     1,     2,     1,     2,     1,     1,     2,     3,     2,
       3,     5,     1,     3,     2,     1,     3,     3,     2,     1,
       3,     2,     3,     3,     4,     1,     3,     5,     8,     0,
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
static const yytype_uint8 yydefact[] =
{
       3,   130,   129,   132,   125,   133,    67,    59,    59,   134,
      65,     0,     0,    62,     0,    63,    59,     0,     0,   131,
      61,     0,     0,    60,     0,     0,   216,    64,     0,   127,
       0,     0,     0,     0,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,     0,   189,   190,   191,   192,   193,   194,   195,
     197,   196,     0,     0,     0,     2,     7,     5,     9,    10,
      11,    12,    13,    14,    15,    16,    17,   216,     0,    49,
       0,    52,    66,    71,   135,   136,   137,   139,   141,   143,
       0,   201,   146,   145,     0,     0,     0,   126,    60,     0,
       0,    35,    59,   125,     0,     0,     0,     0,    22,    89,
       0,     0,    18,    59,    59,    53,    26,    35,     0,     0,
     217,    35,   128,   161,     0,    41,    69,   165,    69,     0,
      88,    87,     0,     0,   149,     0,    69,     1,     8,     4,
      19,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   138,   140,   142,
     144,     0,   147,     0,    21,   200,   204,     0,     0,    55,
       0,    54,     0,    31,     0,    41,    47,   122,   121,   120,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   209,   216,     0,     0,   208,    51,   216,   218,
       0,     0,    23,     0,   216,   162,    42,    68,     0,   163,
       0,     0,   123,   150,     0,     6,     0,    45,     0,    86,
      75,    74,    73,    76,    81,    80,    78,    85,    84,    83,
      82,    79,    72,     0,   148,     0,   202,     0,     0,     0,
       0,    41,    39,    43,    44,     0,     0,    42,    46,     0,
     119,    91,    93,    92,    95,    94,    96,   102,   101,   100,
      99,    98,    97,   105,   104,   103,   108,   107,   106,   109,
     114,   113,   111,   118,   117,   116,   115,   112,    90,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
     241,   242,   243,   244,   245,   246,   247,   248,    35,   169,
     214,     0,   213,   212,     0,    59,    33,    25,    24,   219,
      33,    70,   159,     0,   152,    41,   155,     0,    41,   164,
     166,     0,   124,     0,    20,    59,    56,   199,     0,   205,
       0,    58,    57,    37,    36,    42,    38,    32,    59,    48,
       0,   216,     0,     0,    45,     0,     0,   211,   210,     0,
      27,    28,   167,     0,    42,   154,     0,    42,   158,     0,
     151,     0,     0,     0,    40,    50,     0,    33,     0,     0,
     215,   206,     0,    34,   153,   156,   157,   160,    77,   216,
     198,   203,   110,    29,     0,     0,   207,    33,   168,     0,
      30,   170
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    64,    65,    66,   139,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,   380,   173,   260,   261,   227,
     262,   263,   174,   175,   264,    79,    80,    81,   124,   125,
      82,   128,   109,    97,    84,    85,   343,   344,   345,   346,
     347,   348,    86,    87,   129,    88,    89,   372,    90,    91,
      92,   167,    93,   215,   335,   216,   119,   120,   328
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -329
static const yytype_int16 yypact[] =
{
     770,  -329,  -329,  -329,   -35,  -329,  -329,   -18,   -18,  -329,
    -329,    15,   -29,  -329,  2090,  -329,  1130,  2090,    19,  -329,
    -329,   -84,     3,  1610,  2090,    34,   -46,  -329,    36,    -8,
    1730,  2090,  2210,  2210,  -329,  -329,  -329,  -329,  -329,  -329,
    -329,  -329,  -329,  -329,  -329,  -329,  -329,  -329,  -329,  -329,
    -329,  -329,   -74,  -329,  -329,  -329,  -329,  -329,  -329,  -329,
    -329,  -329,  2090,   649,    46,  -329,   -70,  -329,  -329,  -329,
    -329,  -329,  -329,  -329,  -329,  -329,  -329,   -46,   -68,  -329,
       9,  -329,    47,  -329,  -329,  -329,    -8,    -8,    -8,    -8,
     -67,  -329,    -8,  -329,  2090,  2090,  1850,  -329,  -329,  1370,
    1490,   -66,  1130,   -34,  2090,  2090,  2090,   -64,  3057,  -329,
     -61,  2935,  -329,   890,  1250,  -329,  3057,   -66,  2330,    -6,
     -31,   -66,  -329,  -329,     4,   -60,  2394,  -329,  3057,   -41,
       1,     1,  2090,  2643,  -329,   -63,  2429,  -329,   770,  -329,
      -7,    60,    31,  2210,  2210,  2210,  2210,  2210,  2210,  2210,
    2210,  2210,  2210,  2210,  2210,  2210,  2330,  -329,  -329,  -329,
    -329,  2090,  -329,    16,  3057,  -329,    20,   -48,    37,  -329,
      39,  -329,   408,    11,    28,   -28,  -329,    26,    26,    26,
    2090,  2090,  2090,  2090,  2090,  2090,  2090,  2090,  2090,  2090,
    2090,  2090,  2090,  2090,  2090,  2090,  2090,  2090,  2090,  2090,
    2090,  2090,  2090,  2090,  2090,  2090,  2090,  2090,  2090,  2330,
    2862,  2090,  -329,   -46,   -38,   -97,  -329,  -329,   -46,  -329,
    2090,  2090,  -329,  2330,   -46,  -329,  2090,  -329,    80,  -329,
    1970,  2536,    -8,  -329,    80,  -329,  2090,    -5,  1610,    30,
     235,   235,   235,   249,   382,   382,     1,     1,     1,     1,
       1,     1,  -329,  2507,  -329,  2090,    -4,  1610,  1610,    17,
      18,    14,  -329,  -329,   -38,  2090,   100,  1010,  -329,  2614,
      53,  3091,  3135,  3135,   518,   518,   224,   638,   638,   638,
     638,   638,   638,    23,    23,    23,   313,   313,   313,   396,
     487,   487,    26,    26,    26,    26,    26,    26,  -329,  -329,
    -329,  -329,  -329,  -329,  -329,  -329,  -329,  -329,  -329,  -329,
    -329,  -329,  -329,  -329,  -329,  -329,  -329,  -329,  -329,  -329,
    -329,  -329,  -329,  -329,  -329,  -329,  -329,  -329,   -66,  3057,
      75,   136,  -329,  -329,    70,   528,    84,  3057,  3057,  -329,
      84,  3057,  -329,    92,   101,    32,  -329,    81,    38,  -329,
    -329,  2090,  -329,    35,  3057,  1130,  -329,  -329,  2090,  3057,
    2090,  -329,  -329,  -329,  -329,  1010,  -329,  3057,  1250,  -329,
    2090,   -46,    12,  2090,  -329,  2090,    82,  -329,  -329,  2090,
    -329,  -329,  -329,  2090,    80,  -329,  2090,   151,  -329,  2721,
    -329,    40,  2750,  2828,  -329,  -329,  2857,    84,  2090,  2090,
    3057,  3057,  2090,  3057,  3057,  -329,  3057,  -329,  -329,   -46,
    -329,  -329,  -329,  -329,  3013,  2979,  3057,    84,  -329,  2090,
    -329,  3057
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -329,  -329,  -329,  -329,  -329,    27,  -329,  -329,  -329,  -109,
    -329,  -329,  -329,  -329,  -329,  -328,  -108,  -194,  -329,  -150,
    -169,     7,  -329,  -329,     6,  -113,    29,   -21,   -20,  -329,
     131,   -14,   116,   -24,  -329,  -329,   -33,  -329,  -329,  -185,
    -329,  -329,  -329,  -329,  -329,  -329,  -329,  -329,  -329,  -329,
    -329,  -329,  -329,  -329,  -329,  -135,   -62,  -329,  -329
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -60
static const yytype_int16 yytable[] =
{
     108,   217,   115,   111,   212,   122,    78,    77,   143,   218,
     116,   127,   381,   224,   229,   140,   126,    94,    94,    98,
     332,   101,   110,   102,   333,   268,   334,   112,   220,   221,
     181,    95,   222,   181,   398,   399,    99,   100,   113,   114,
     117,   118,   121,   135,    94,   132,   137,   138,   133,   136,
     141,   142,   161,   172,   143,   180,   223,   210,   225,   236,
     233,   226,   157,   158,   159,   160,   237,   238,   162,   413,
     254,   255,   256,   257,   163,   258,   166,   265,   169,   171,
     331,   164,   266,   230,    96,    96,   342,   155,   156,   420,
     177,   178,   179,   267,   -60,   -60,   -60,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   176,   208,
     209,   366,   208,   209,   355,   360,    83,   156,   231,   214,
     213,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,    83,   155,   156,   365,   368,   363,   364,    83,
     209,   373,   374,   375,    78,    77,   382,   253,    83,    83,
     379,   330,   383,   384,   386,   402,   336,   407,   390,   387,
     409,   391,   340,   130,   131,   235,   269,   270,   271,   272,
     273,   274,   275,   276,   277,   278,   279,   280,   281,   282,
     283,   284,   285,   286,   287,   288,   289,   290,   291,   292,
     293,   294,   295,   296,   297,   385,   394,   329,   388,   405,
     378,   353,     0,     0,     0,     0,   337,   338,   352,     0,
     350,     0,   341,     0,     0,    83,    83,   356,    83,     0,
     371,     0,   354,     0,     0,     0,   377,     0,     0,    83,
      83,   181,     0,     0,   219,     0,   361,   362,     0,     0,
       0,   359,   143,     0,     0,     0,     0,     0,     0,     0,
       0,   367,     0,     0,    83,   395,   143,     0,     0,    83,
      83,    83,    83,    83,    83,    83,    83,    83,    83,    83,
      83,    83,   252,   369,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,     0,    83,   188,
     189,   190,   191,   192,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   397,
     208,   209,   147,   148,   149,   150,   151,   152,   153,   154,
     181,   155,   156,     0,     0,   298,   -60,   148,   149,   150,
     151,   152,   153,   154,     0,   155,   156,   389,     0,   339,
       0,   214,   213,     0,   392,     0,   393,   417,     0,     0,
       0,     0,     0,     0,    83,     0,   396,     0,     0,   400,
       0,   401,     0,     0,     0,   403,     0,     0,     0,   404,
       0,     0,   406,    83,    83,     0,     0,     0,     0,     0,
       0,     0,     0,    83,   414,   415,     0,     0,   416,   143,
     200,   201,   202,   203,   204,   205,   206,   207,     0,   208,
     209,     0,     0,   181,     0,   421,     0,     0,     0,   259,
       0,     1,     2,     3,   103,     0,     5,     6,     7,     8,
       0,     9,    10,     0,     0,    12,    13,     0,     0,     0,
       0,     0,     0,     0,    15,     0,    17,     0,    19,    20,
      21,    22,     0,     0,     0,    23,     0,     0,     0,     0,
     -59,    83,    27,     0,     0,     0,     0,    29,     0,     0,
      30,    31,   150,   151,   152,   153,   154,     0,   155,   156,
       0,    83,     0,   -60,   201,   202,   203,   204,   205,   206,
     207,    83,   208,   209,    83,     0,    32,    33,     0,     0,
       0,     0,     0,     0,   181,     0,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,     0,    61,   181,     0,    62,     0,     0,
      63,     1,     2,     3,   103,     0,     5,     6,     7,     8,
       0,     9,    10,     0,     0,    12,    13,     0,    14,     0,
       0,     0,     0,     0,    15,     0,    17,     0,    19,    20,
      21,    22,     0,     0,     0,    23,     0,   203,   204,   205,
     206,   207,    27,   208,   209,     0,     0,    29,     0,     0,
      30,    31,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,     0,   208,   209,    32,    33,     0,     0,
       0,     0,     0,     0,     0,     0,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,     0,    61,   181,     0,    62,     0,     0,
      63,   376,     1,     2,     3,   103,     0,     5,     0,     0,
       0,     0,     9,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    17,     0,    19,
       0,    21,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    29,     0,
       0,    30,    31,   -60,   -60,   -60,   -60,   -60,   -60,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,     0,   208,   209,     0,   104,   105,     0,
       0,     0,     0,     0,   106,     0,     0,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,   107,    53,    54,    55,    56,
      57,    58,    59,    60,     0,    61,     0,     0,    62,     0,
       0,    63,   134,     1,     2,     3,     4,     0,     5,     6,
       7,     8,     0,     9,    10,    11,     0,    12,    13,     0,
      14,     0,     0,     0,     0,     0,    15,    16,    17,    18,
      19,    20,    21,    22,     0,     0,     0,    23,     0,    24,
      25,     0,   -59,    26,    27,    28,     0,     0,     0,    29,
       0,     0,    30,    31,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    32,    33,
       0,     0,     0,     0,     0,     0,     0,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,     0,    61,     0,     0,    62,
       0,     0,    63,     1,     2,     3,   103,     0,     5,     6,
       7,     8,     0,     9,    10,     0,     0,    12,    13,     0,
      14,     0,     0,     0,     0,     0,    15,     0,    17,     0,
      19,    20,    21,    22,     0,     0,     0,    23,     0,     0,
       0,     0,     0,     0,    27,     0,     0,     0,     0,    29,
       0,     0,    30,    31,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    32,    33,
       0,     0,     0,     0,     0,     0,     0,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,     0,    61,     0,     0,    62,
       0,     0,    63,     1,     2,     3,   103,     0,     5,     6,
       7,     8,     0,     9,    10,     0,     0,    12,    13,     0,
       0,     0,     0,     0,     0,     0,    15,     0,    17,     0,
      19,    20,    21,    22,     0,     0,     0,    23,     0,     0,
       0,     0,   -59,     0,    27,     0,     0,     0,     0,    29,
       0,     0,    30,    31,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    32,    33,
       0,     0,     0,     0,     0,     0,     0,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,     0,    61,     0,     0,    62,
       0,     0,    63,     1,     2,     3,   103,     0,     5,     6,
       7,     8,     0,     9,    10,     0,     0,    12,    13,     0,
       0,     0,     0,     0,     0,     0,    15,     0,    17,     0,
      19,    20,    21,    22,     0,     0,     0,    23,     0,     0,
       0,     0,     0,     0,    27,     0,     0,     0,     0,    29,
       0,     0,    30,    31,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    32,    33,
       0,     0,     0,     0,     0,     0,     0,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,     0,    61,     0,     0,    62,
       0,     0,    63,     1,     2,     3,   103,     0,     5,     6,
       7,     8,     0,     9,    10,     0,     0,     0,    13,     0,
       0,     0,     0,     0,     0,     0,    15,     0,    17,     0,
      19,    20,    21,     0,     0,     0,     0,    23,     0,     0,
       0,     0,     0,     0,    27,     0,     0,     0,     0,    29,
       0,     0,    30,    31,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    32,    33,
       0,     0,     0,     0,     0,     0,     0,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,     0,    61,     0,     0,    62,
       0,     0,    63,     1,     2,     3,   103,     0,     5,     6,
       0,     0,     0,     9,    10,     0,     0,     0,    13,     0,
       0,     0,     0,     0,     0,     0,    15,     0,    17,     0,
      19,    20,    21,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   168,     0,    27,     0,     0,     0,     0,    29,
       0,     0,    30,    31,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    32,    33,
       0,     0,     0,     0,     0,     0,     0,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,     0,    61,     0,     0,    62,
       0,     0,    63,     1,     2,     3,   103,     0,     5,     6,
       0,     0,     0,     9,    10,     0,     0,     0,    13,     0,
       0,     0,     0,     0,     0,     0,    15,     0,    17,     0,
      19,    20,    21,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   170,     0,    27,     0,     0,     0,     0,    29,
       0,     0,    30,    31,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    32,    33,
       0,     0,     0,     0,     0,     0,     0,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,     0,    61,     0,     0,    62,
       0,     0,    63,     1,     2,     3,   103,     0,     5,     6,
       0,     0,     0,     9,    10,     0,     0,     0,    13,     0,
       0,     0,     0,     0,     0,     0,    15,     0,    17,     0,
      19,    20,    21,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    27,     0,     0,     0,     0,    29,
       0,     0,    30,    31,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    32,    33,
       0,     0,     0,     0,     0,     0,     0,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,     0,    61,     0,     0,    62,
       0,     0,    63,     1,     2,     3,   103,     0,     5,     0,
       0,     0,     0,     9,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    17,     0,
      19,     0,    21,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    30,    31,   123,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   104,   105,
       0,     0,     0,     0,     0,   106,     0,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,   107,    53,    54,    55,
      56,    57,    58,    59,    60,     0,    61,     0,     0,    62,
       0,     0,    63,     1,     2,     3,   103,     0,     5,     0,
       0,     0,     0,     9,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    17,     0,
      19,     0,    21,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    30,    31,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   104,   105,
       0,     0,     0,     0,     0,   106,     0,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,   107,    53,    54,    55,
      56,    57,    58,    59,    60,     0,    61,     0,     0,    62,
     165,     0,    63,     1,     2,     3,   103,     0,     5,     0,
       0,     0,     0,     9,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    17,     0,
      19,     0,    21,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    30,    31,     0,   349,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   104,   105,
       0,     0,     0,     0,     0,   106,     0,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,   107,    53,    54,    55,
      56,    57,    58,    59,    60,     0,    61,     0,     0,    62,
       0,     0,    63,     1,     2,     3,   103,     0,     5,     0,
       0,     0,     0,     9,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    17,     0,
      19,     0,    21,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    30,    31,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   104,   105,
       0,     0,     0,     0,     0,   106,     0,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,   107,    53,    54,    55,
      56,    57,    58,    59,    60,     0,    61,     0,     0,    62,
       0,     0,    63,     1,     2,     3,   103,     0,     5,     0,
       0,     0,     0,     9,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    17,     0,
      19,     0,    21,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    30,    31,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    32,    33,
       0,     0,     0,     0,     0,     0,     0,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,     0,    61,     0,     0,    62,
       0,     0,    63,     1,     2,     3,   103,     0,     5,     0,
       0,     0,     0,     9,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    17,     0,
      19,     0,    21,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,    30,    31,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   181,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,   181,    53,    54,    55,
      56,    57,    58,    59,    60,     0,    61,     0,     0,    62,
       0,     0,    63,   182,   183,   184,   185,   186,   187,   188,
     189,   190,   191,   192,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,     0,
     208,   209,     0,     0,     0,     0,     0,     0,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   181,   208,   209,     0,   228,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   181,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   234,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,     0,   208,   209,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   181,   208,   209,     0,     0,     0,   357,   358,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     181,     0,     0,     0,     0,     0,     0,   351,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   182,   183,   184,   185,   186,   187,   188,
     189,   190,   191,   192,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,     0,
     208,   209,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   181,   208,
     209,     0,     0,     0,     0,   370,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   181,     0,     0,
       0,     0,     0,   232,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     182,   183,   184,   185,   186,   187,   188,   189,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,     0,   208,   209,   182,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   181,   208,   209,     0,     0,
       0,   408,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   181,     0,     0,     0,   299,     0,
     410,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,     0,   208,   209,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   181,   208,   209,     0,     0,     0,   411,     0,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   412,   327,     0,
       0,   211,     0,     0,     0,     0,   181,     0,     0,     0,
       0,     0,     0,     0,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     181,   208,   209,     0,     0,   419,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   418,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   181,   208,   209,     0,     0,     0,
       0,     0,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   181,   208,
     209,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   181,   208,   209,     0,     0,     0,     0,     0,
       0,   183,   184,   185,   186,   187,   188,   189,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,     0,   208,   209,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
       0,   208,   209
};

static const yytype_int16 yycheck[] =
{
      14,   114,    23,    17,   113,    29,     0,     0,     7,   117,
      24,    31,   340,   121,    55,    77,    30,    52,    52,    37,
     117,     6,    16,    52,   121,   175,   123,     8,    34,    35,
       7,    66,    38,     7,    22,    23,     7,     8,   122,    36,
       6,    87,     6,    63,    52,   119,     0,   117,    62,    63,
     118,    42,   119,   119,     7,   119,    87,   118,    54,    66,
     123,   121,    86,    87,    88,    89,     6,    36,    92,   397,
      54,    51,   120,    36,    94,    36,    96,    66,    99,   100,
     118,    95,    54,   124,   119,   119,     6,    86,    87,   417,
     104,   105,   106,   121,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,   102,    86,
      87,   261,    86,    87,   119,   119,     0,    87,   132,   113,
     113,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    16,    86,    87,   121,    36,   120,   120,    23,
      87,    66,     6,    73,   138,   138,    54,   161,    32,    33,
      66,   213,    51,   121,    73,    73,   218,     6,   123,   121,
     120,   355,   224,    32,    33,   138,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   345,   365,   211,   348,   384,
     335,   234,    -1,    -1,    -1,    -1,   220,   221,   232,    -1,
     230,    -1,   226,    -1,    -1,    99,   100,   238,   102,    -1,
     328,    -1,   236,    -1,    -1,    -1,   335,    -1,    -1,   113,
     114,     7,    -1,    -1,   118,    -1,   257,   258,    -1,    -1,
      -1,   255,     7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   265,    -1,    -1,   138,   368,     7,    -1,    -1,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   267,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,   155,    -1,   172,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,   371,
      86,    87,    77,    78,    79,    80,    81,    82,    83,    84,
       7,    86,    87,    -1,    -1,   209,    77,    78,    79,    80,
      81,    82,    83,    84,    -1,    86,    87,   351,    -1,   223,
      -1,   335,   335,    -1,   358,    -1,   360,   409,    -1,    -1,
      -1,    -1,    -1,    -1,   238,    -1,   370,    -1,    -1,   373,
      -1,   375,    -1,    -1,    -1,   379,    -1,    -1,    -1,   383,
      -1,    -1,   386,   257,   258,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   267,   398,   399,    -1,    -1,   402,     7,
      77,    78,    79,    80,    81,    82,    83,    84,    -1,    86,
      87,    -1,    -1,     7,    -1,   419,    -1,    -1,    -1,     1,
      -1,     3,     4,     5,     6,    -1,     8,     9,    10,    11,
      -1,    13,    14,    -1,    -1,    17,    18,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    26,    -1,    28,    -1,    30,    31,
      32,    33,    -1,    -1,    -1,    37,    -1,    -1,    -1,    -1,
      42,   335,    44,    -1,    -1,    -1,    -1,    49,    -1,    -1,
      52,    53,    80,    81,    82,    83,    84,    -1,    86,    87,
      -1,   355,    -1,    77,    78,    79,    80,    81,    82,    83,
      84,   365,    86,    87,   368,    -1,    78,    79,    -1,    -1,
      -1,    -1,    -1,    -1,     7,    -1,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,    -1,   116,     7,    -1,   119,    -1,    -1,
     122,     3,     4,     5,     6,    -1,     8,     9,    10,    11,
      -1,    13,    14,    -1,    -1,    17,    18,    -1,    20,    -1,
      -1,    -1,    -1,    -1,    26,    -1,    28,    -1,    30,    31,
      32,    33,    -1,    -1,    -1,    37,    -1,    80,    81,    82,
      83,    84,    44,    86,    87,    -1,    -1,    49,    -1,    -1,
      52,    53,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    -1,    86,    87,    78,    79,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,    -1,   116,     7,    -1,   119,    -1,    -1,
     122,   123,     3,     4,     5,     6,    -1,     8,    -1,    -1,
      -1,    -1,    13,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    28,    -1,    30,
      -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    49,    -1,
      -1,    52,    53,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    -1,    86,    87,    -1,    78,    79,    -1,
      -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,    -1,   116,    -1,    -1,   119,    -1,
      -1,   122,   123,     3,     4,     5,     6,    -1,     8,     9,
      10,    11,    -1,    13,    14,    15,    -1,    17,    18,    -1,
      20,    -1,    -1,    -1,    -1,    -1,    26,    27,    28,    29,
      30,    31,    32,    33,    -1,    -1,    -1,    37,    -1,    39,
      40,    -1,    42,    43,    44,    45,    -1,    -1,    -1,    49,
      -1,    -1,    52,    53,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    79,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,    -1,   116,    -1,    -1,   119,
      -1,    -1,   122,     3,     4,     5,     6,    -1,     8,     9,
      10,    11,    -1,    13,    14,    -1,    -1,    17,    18,    -1,
      20,    -1,    -1,    -1,    -1,    -1,    26,    -1,    28,    -1,
      30,    31,    32,    33,    -1,    -1,    -1,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    44,    -1,    -1,    -1,    -1,    49,
      -1,    -1,    52,    53,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    79,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,    -1,   116,    -1,    -1,   119,
      -1,    -1,   122,     3,     4,     5,     6,    -1,     8,     9,
      10,    11,    -1,    13,    14,    -1,    -1,    17,    18,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    26,    -1,    28,    -1,
      30,    31,    32,    33,    -1,    -1,    -1,    37,    -1,    -1,
      -1,    -1,    42,    -1,    44,    -1,    -1,    -1,    -1,    49,
      -1,    -1,    52,    53,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    79,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,    -1,   116,    -1,    -1,   119,
      -1,    -1,   122,     3,     4,     5,     6,    -1,     8,     9,
      10,    11,    -1,    13,    14,    -1,    -1,    17,    18,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    26,    -1,    28,    -1,
      30,    31,    32,    33,    -1,    -1,    -1,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    44,    -1,    -1,    -1,    -1,    49,
      -1,    -1,    52,    53,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    79,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,    -1,   116,    -1,    -1,   119,
      -1,    -1,   122,     3,     4,     5,     6,    -1,     8,     9,
      10,    11,    -1,    13,    14,    -1,    -1,    -1,    18,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    26,    -1,    28,    -1,
      30,    31,    32,    -1,    -1,    -1,    -1,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    44,    -1,    -1,    -1,    -1,    49,
      -1,    -1,    52,    53,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    79,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,    -1,   116,    -1,    -1,   119,
      -1,    -1,   122,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    -1,    13,    14,    -1,    -1,    -1,    18,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    26,    -1,    28,    -1,
      30,    31,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    42,    -1,    44,    -1,    -1,    -1,    -1,    49,
      -1,    -1,    52,    53,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    79,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,    -1,   116,    -1,    -1,   119,
      -1,    -1,   122,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    -1,    13,    14,    -1,    -1,    -1,    18,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    26,    -1,    28,    -1,
      30,    31,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    42,    -1,    44,    -1,    -1,    -1,    -1,    49,
      -1,    -1,    52,    53,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    79,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,    -1,   116,    -1,    -1,   119,
      -1,    -1,   122,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    -1,    13,    14,    -1,    -1,    -1,    18,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    26,    -1,    28,    -1,
      30,    31,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    44,    -1,    -1,    -1,    -1,    49,
      -1,    -1,    52,    53,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    79,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,    -1,   116,    -1,    -1,   119,
      -1,    -1,   122,     3,     4,     5,     6,    -1,     8,    -1,
      -1,    -1,    -1,    13,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    28,    -1,
      30,    -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      -1,    -1,    52,    53,    54,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    79,
      -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,    -1,   116,    -1,    -1,   119,
      -1,    -1,   122,     3,     4,     5,     6,    -1,     8,    -1,
      -1,    -1,    -1,    13,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    28,    -1,
      30,    -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      -1,    -1,    52,    53,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    79,
      -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,    -1,   116,    -1,    -1,   119,
     120,    -1,   122,     3,     4,     5,     6,    -1,     8,    -1,
      -1,    -1,    -1,    13,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    28,    -1,
      30,    -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      -1,    -1,    52,    53,    -1,    55,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    79,
      -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,    -1,   116,    -1,    -1,   119,
      -1,    -1,   122,     3,     4,     5,     6,    -1,     8,    -1,
      -1,    -1,    -1,    13,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    28,    -1,
      30,    -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      -1,    -1,    52,    53,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    79,
      -1,    -1,    -1,    -1,    -1,    85,    -1,    -1,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,    -1,   116,    -1,    -1,   119,
      -1,    -1,   122,     3,     4,     5,     6,    -1,     8,    -1,
      -1,    -1,    -1,    13,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    28,    -1,
      30,    -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      -1,    -1,    52,    53,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    79,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,    -1,   116,    -1,    -1,   119,
      -1,    -1,   122,     3,     4,     5,     6,    -1,     8,    -1,
      -1,    -1,    -1,    13,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    28,    -1,
      30,    -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      -1,    -1,    52,    53,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,     7,   107,   108,   109,
     110,   111,   112,   113,   114,    -1,   116,    -1,    -1,   119,
      -1,    -1,   122,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    -1,
      86,    87,    -1,    -1,    -1,    -1,    -1,    -1,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,     7,    86,    87,    -1,   124,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   124,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    -1,    86,    87,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,     7,    86,    87,    -1,    -1,    -1,   120,   121,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       7,    -1,    -1,    -1,    -1,    -1,    -1,   121,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    -1,
      86,    87,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,     7,    86,
      87,    -1,    -1,    -1,    -1,   121,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,    -1,
      -1,    -1,    -1,   120,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    -1,    86,    87,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,     7,    86,    87,    -1,    -1,
      -1,   120,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,     6,    -1,
     120,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    -1,    86,    87,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,     7,    86,    87,    -1,    -1,    -1,   120,    -1,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   120,   116,    -1,
      -1,    46,    -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
       7,    86,    87,    -1,    -1,    46,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    24,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,     7,    86,    87,    -1,    -1,    -1,
      -1,    -1,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,     7,    86,
      87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,     7,    86,    87,    -1,    -1,    -1,    -1,    -1,
      -1,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    -1,    86,    87,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      -1,    86,    87
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     8,     9,    10,    11,    13,
      14,    15,    17,    18,    20,    26,    27,    28,    29,    30,
      31,    32,    33,    37,    39,    40,    43,    44,    45,    49,
      52,    53,    78,    79,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   116,   119,   122,   126,   127,   128,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   146,   149,   150,
     151,   152,   155,   157,   159,   160,   167,   168,   170,   171,
     173,   174,   175,   177,    52,    66,   119,   158,    37,   151,
     151,     6,    52,     6,    78,    79,    85,   106,   156,   157,
     149,   156,     8,   122,    36,   152,   156,     6,    87,   181,
     182,     6,   158,    54,   153,   154,   156,   153,   156,   169,
     155,   155,   119,   156,   123,   153,   156,     0,   117,   129,
     181,   118,    42,     7,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    86,    87,   158,   158,   158,
     158,   119,   158,   153,   156,   120,   153,   176,    42,   152,
      42,   152,   119,   141,   147,   148,   149,   156,   156,   156,
     119,     7,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    86,    87,
     118,    46,   134,   146,   149,   178,   180,   150,   141,   157,
      34,    35,    38,    87,   141,    54,   121,   144,   124,    55,
     124,   156,   120,   123,   124,   130,    66,     6,    36,   155,
     155,   155,   155,   155,   155,   155,   155,   155,   155,   155,
     155,   155,   157,   156,    54,    51,   120,    36,    36,     1,
     142,   143,   145,   146,   149,    66,    54,   121,   144,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   157,     6,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   116,   183,   156,
     181,   118,   117,   121,   123,   179,   181,   156,   156,   157,
     181,   156,     6,   161,   162,   163,   164,   165,   166,    55,
     153,   121,   158,   161,   156,   119,   152,   120,   121,   156,
     119,   152,   152,   120,   120,   121,   144,   156,    36,   149,
     121,   141,   172,    66,     6,    73,   123,   134,   180,    66,
     140,   140,    54,    51,   121,   144,    73,   121,   144,   156,
     123,   142,   156,   156,   145,   150,   156,   181,    22,    23,
     156,   156,    73,   156,   156,   164,   156,     6,   120,   120,
     120,   120,   120,   140,   156,   156,   156,   181,    24,    46,
     140,   156
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
  yylloc.filename = static_cast<ParserState*>(parm)->model->filepath();
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

    { (yyval.item)=notInDatafile(&(yyloc),parm,"include") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 10:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"variable declaration") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 12:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"constraint") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 13:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"solve") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 14:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"output") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 15:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"predicate") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 16:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"predicate") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 17:

    { (yyval.item)=notInDatafile(&(yyloc),parm,"annotation") ? (yyvsp[(1) - (1)].item) : NULL; ;}
    break;

  case 18:

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

  case 19:

    { if ((yyvsp[(2) - (2)].expression_v)) (yyvsp[(1) - (2)].vardeclexpr)->addAnnotations(*(yyvsp[(2) - (2)].expression_v));
        (yyval.item) = new VarDeclI((yyloc),(yyvsp[(1) - (2)].vardeclexpr));
        delete (yyvsp[(2) - (2)].expression_v);
      ;}
    break;

  case 20:

    { (yyvsp[(1) - (4)].vardeclexpr)->e((yyvsp[(4) - (4)].expression));
        if ((yyvsp[(2) - (4)].expression_v)) (yyvsp[(1) - (4)].vardeclexpr)->addAnnotations(*(yyvsp[(2) - (4)].expression_v));
        (yyval.item) = new VarDeclI((yyloc),(yyvsp[(1) - (4)].vardeclexpr));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 21:

    { (yyval.item) = new AssignI((yyloc),(yyvsp[(1) - (3)].sValue),(yyvsp[(3) - (3)].expression));
        free((yyvsp[(1) - (3)].sValue));
      ;}
    break;

  case 22:

    { (yyval.item) = new ConstraintI((yyloc),(yyvsp[(2) - (2)].expression));;}
    break;

  case 23:

    { (yyval.item) = SolveI::sat((yyloc));
        if ((yyvsp[(2) - (3)].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[(2) - (3)].expression_v));
        delete (yyvsp[(2) - (3)].expression_v);
      ;}
    break;

  case 24:

    { (yyval.item) = SolveI::min((yyloc),(yyvsp[(4) - (4)].expression));
        if ((yyvsp[(2) - (4)].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[(2) - (4)].expression_v));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 25:

    { (yyval.item) = SolveI::max((yyloc),(yyvsp[(4) - (4)].expression));
        if ((yyvsp[(2) - (4)].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[(2) - (4)].expression_v));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 26:

    { (yyval.item) = new OutputI((yyloc),(yyvsp[(2) - (2)].expression));;}
    break;

  case 27:

    { (yyval.item) = new FunctionI((yyloc),(yyvsp[(2) - (5)].sValue),new TypeInst((yyloc),
                           Type::varbool()),*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression));
        if ((yyvsp[(4) - (5)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(4) - (5)].expression_v));
        free((yyvsp[(2) - (5)].sValue));
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
        delete (yyvsp[(4) - (5)].expression_v);
      ;}
    break;

  case 28:

    { (yyval.item) = new FunctionI((yyloc),(yyvsp[(2) - (5)].sValue),new TypeInst((yyloc),
                           Type::parbool()),*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression));
        if ((yyvsp[(4) - (5)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(4) - (5)].expression_v));
        free((yyvsp[(2) - (5)].sValue));
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
        delete (yyvsp[(4) - (5)].expression_v);
      ;}
    break;

  case 29:

    { (yyval.item) = new FunctionI((yyloc),(yyvsp[(4) - (7)].sValue),(yyvsp[(2) - (7)].tiexpr),*(yyvsp[(5) - (7)].vardeclexpr_v),(yyvsp[(7) - (7)].expression));
        if ((yyvsp[(6) - (7)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(6) - (7)].expression_v));
        free((yyvsp[(4) - (7)].sValue));
        delete (yyvsp[(5) - (7)].vardeclexpr_v);
        delete (yyvsp[(6) - (7)].expression_v);
      ;}
    break;

  case 30:

    { (yyval.item) = new FunctionI((yyloc),(yyvsp[(3) - (8)].sValue),(yyvsp[(1) - (8)].tiexpr),*(yyvsp[(5) - (8)].vardeclexpr_v),(yyvsp[(8) - (8)].expression));
        if ((yyvsp[(7) - (8)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(7) - (8)].expression_v));
        free((yyvsp[(3) - (8)].sValue));
        delete (yyvsp[(5) - (8)].vardeclexpr_v);
        delete (yyvsp[(7) - (8)].expression_v);
      ;}
    break;

  case 31:

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

  case 32:

    { TypeInst* ti=new TypeInst((yylsp[(1) - (5)]),Type::ann());
        (yyval.item) = new FunctionI((yyloc),(yyvsp[(2) - (5)].sValue),ti,*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression));
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
      ;}
    break;

  case 33:

    { (yyval.expression)=NULL; ;}
    break;

  case 34:

    { (yyval.expression)=(yyvsp[(2) - (2)].expression); ;}
    break;

  case 35:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); ;}
    break;

  case 36:

    { (yyval.vardeclexpr_v)=(yyvsp[(2) - (3)].vardeclexpr_v); ;}
    break;

  case 37:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); ;}
    break;

  case 38:

    { (yyval.vardeclexpr_v)=(yyvsp[(1) - (2)].vardeclexpr_v); ;}
    break;

  case 39:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>();
        (yyvsp[(1) - (1)].vardeclexpr)->toplevel(false);
        (yyval.vardeclexpr_v)->push_back((yyvsp[(1) - (1)].vardeclexpr)); ;}
    break;

  case 40:

    { (yyval.vardeclexpr_v)=(yyvsp[(1) - (3)].vardeclexpr_v);
        (yyvsp[(3) - (3)].vardeclexpr)->toplevel(false);
        (yyvsp[(1) - (3)].vardeclexpr_v)->push_back((yyvsp[(3) - (3)].vardeclexpr)); ;}
    break;

  case 43:

    { (yyval.vardeclexpr)=(yyvsp[(1) - (1)].vardeclexpr); ;}
    break;

  case 44:

    { (yyval.vardeclexpr)=new VarDecl((yyloc), (yyvsp[(1) - (1)].tiexpr), ""); ;}
    break;

  case 45:

    { (yyval.vardeclexpr) = new VarDecl((yyloc), (yyvsp[(1) - (3)].tiexpr), (yyvsp[(3) - (3)].sValue));
        free((yyvsp[(3) - (3)].sValue));
      ;}
    break;

  case 46:

    { (yyval.tiexpr_v)=(yyvsp[(1) - (2)].tiexpr_v); ;}
    break;

  case 47:

    { (yyval.tiexpr_v)=new vector<TypeInst*>(); (yyval.tiexpr_v)->push_back((yyvsp[(1) - (1)].tiexpr)); ;}
    break;

  case 48:

    { (yyval.tiexpr_v)=(yyvsp[(1) - (3)].tiexpr_v); (yyvsp[(1) - (3)].tiexpr_v)->push_back((yyvsp[(3) - (3)].tiexpr)); ;}
    break;

  case 50:

    {
        (yyval.tiexpr) = (yyvsp[(6) - (6)].tiexpr);
        (yyval.tiexpr)->setRanges(*(yyvsp[(3) - (6)].tiexpr_v));
        delete (yyvsp[(3) - (6)].tiexpr_v);
      ;}
    break;

  case 51:

    {
        (yyval.tiexpr) = (yyvsp[(3) - (3)].tiexpr);
        std::vector<TypeInst*> ti(1);
        ti[0] = new TypeInst((yyloc),Type::parint());
        (yyval.tiexpr)->setRanges(ti);
      ;}
    break;

  case 52:

    { (yyval.tiexpr) = (yyvsp[(1) - (1)].tiexpr);
      ;}
    break;

  case 53:

    { (yyval.tiexpr) = (yyvsp[(2) - (2)].tiexpr);
        Type tt = (yyval.tiexpr)->type();
        tt.ot(Type::OT_OPTIONAL);
        (yyval.tiexpr)->type(tt);
      ;}
    break;

  case 54:

    { (yyval.tiexpr) = (yyvsp[(3) - (3)].tiexpr);
        if ((yyvsp[(2) - (3)].bValue)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 55:

    { (yyval.tiexpr) = (yyvsp[(3) - (3)].tiexpr);
        Type tt = (yyval.tiexpr)->type();
        tt.ti(Type::TI_VAR);
        if ((yyvsp[(2) - (3)].bValue)) tt.ot(Type::OT_OPTIONAL);
        (yyval.tiexpr)->type(tt);
      ;}
    break;

  case 56:

    { (yyval.tiexpr) = (yyvsp[(4) - (4)].tiexpr);
        Type tt = (yyval.tiexpr)->type();
        tt.st(Type::ST_SET);
        if ((yyvsp[(1) - (4)].bValue)) tt.ot(Type::OT_OPTIONAL);
        (yyval.tiexpr)->type(tt);
      ;}
    break;

  case 57:

    { (yyval.tiexpr) = (yyvsp[(5) - (5)].tiexpr);
        Type tt = (yyval.tiexpr)->type();
        tt.st(Type::ST_SET);
        if ((yyvsp[(2) - (5)].bValue)) tt.ot(Type::OT_OPTIONAL);
        (yyval.tiexpr)->type(tt);
      ;}
    break;

  case 58:

    { (yyval.tiexpr) = (yyvsp[(5) - (5)].tiexpr);
        Type tt = (yyval.tiexpr)->type();
        tt.ti(Type::TI_VAR);
        tt.st(Type::ST_SET);
        if ((yyvsp[(2) - (5)].bValue)) tt.ot(Type::OT_OPTIONAL);
        (yyval.tiexpr)->type(tt);
      ;}
    break;

  case 59:

    { (yyval.bValue) = false; ;}
    break;

  case 60:

    { (yyval.bValue) = true; ;}
    break;

  case 61:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parint()); ;}
    break;

  case 62:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parbool()); ;}
    break;

  case 63:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parfloat()); ;}
    break;

  case 64:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parstring()); ;}
    break;

  case 65:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::ann()); ;}
    break;

  case 66:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type(),(yyvsp[(1) - (1)].expression)); ;}
    break;

  case 67:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::top(),
                         new TIId((yyloc), (yyvsp[(1) - (1)].sValue)));
        free((yyvsp[(1) - (1)].sValue));
      ;}
    break;

  case 69:

    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].expression)); ;}
    break;

  case 70:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); (yyval.expression_v)->push_back((yyvsp[(3) - (3)].expression)); ;}
    break;

  case 72:

    { (yyvsp[(1) - (3)].expression)->addAnnotation((yyvsp[(3) - (3)].expression)); (yyval.expression)=(yyvsp[(1) - (3)].expression); ;}
    break;

  case 73:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_UNION, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 74:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 75:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SYMDIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 76:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DOTDOT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 77:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(3) - (6)].expression), BOT_DOTDOT, (yyvsp[(5) - (6)].expression)); ;}
    break;

  case 78:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_INTERSECT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 79:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUSPLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 80:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 81:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MINUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 82:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MULT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 83:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 84:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IDIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 85:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MOD, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 86:

    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=new Call((yyloc), (yyvsp[(2) - (3)].sValue), args);
        free((yyvsp[(2) - (3)].sValue));
      ;}
    break;

  case 87:

    { (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 88:

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

  case 90:

    { (yyvsp[(1) - (3)].expression)->addAnnotation((yyvsp[(3) - (3)].expression)); (yyval.expression)=(yyvsp[(1) - (3)].expression); ;}
    break;

  case 91:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_EQUIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 92:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IMPL, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 93:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_RIMPL, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 94:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_OR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 95:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_XOR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 96:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_AND, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 97:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_LE, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 98:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_GR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 99:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_LQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 100:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_GQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 101:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_EQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 102:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_NQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 103:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IN, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 104:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SUBSET, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 105:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SUPERSET, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 106:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_UNION, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 107:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 108:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SYMDIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 109:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DOTDOT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 110:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(3) - (6)].expression), BOT_DOTDOT, (yyvsp[(5) - (6)].expression)); ;}
    break;

  case 111:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_INTERSECT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 112:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUSPLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 113:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 114:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MINUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 115:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MULT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 116:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 117:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IDIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 118:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MOD, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 119:

    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=new Call((yyloc), (yyvsp[(2) - (3)].sValue), args);
        free((yyvsp[(2) - (3)].sValue));
      ;}
    break;

  case 120:

    { (yyval.expression)=new UnOp((yyloc), UOT_NOT, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 121:

    { if ((yyvsp[(2) - (2)].expression)->isa<IntLit>() || (yyvsp[(2) - (2)].expression)->isa<FloatLit>()) {
          (yyval.expression) = (yyvsp[(2) - (2)].expression);
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[(2) - (2)].expression));
        }
      ;}
    break;

  case 122:

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

  case 123:

    { (yyval.expression)=(yyvsp[(2) - (3)].expression); ;}
    break;

  case 124:

    { (yyval.expression)=new ArrayAccess((yyloc), (yyvsp[(2) - (4)].expression), *(yyvsp[(4) - (4)].expression_v)); delete (yyvsp[(4) - (4)].expression_v); ;}
    break;

  case 125:

    { (yyval.expression)=new Id((yyloc), (yyvsp[(1) - (1)].sValue), NULL); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 126:

    { (yyval.expression)=new ArrayAccess((yyloc), new Id((yylsp[(1) - (2)]),(yyvsp[(1) - (2)].sValue),NULL), *(yyvsp[(2) - (2)].expression_v));
        free((yyvsp[(1) - (2)].sValue)); delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 127:

    { (yyval.expression)=new AnonVar((yyloc)); ;}
    break;

  case 128:

    { (yyval.expression)=new ArrayAccess((yyloc), new AnonVar((yyloc)), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 129:

    { (yyval.expression)=new BoolLit((yyloc), ((yyvsp[(1) - (1)].iValue)!=0)); ;}
    break;

  case 130:

    { (yyval.expression)=new IntLit((yyloc), (yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 131:

    { (yyval.expression)=new IntLit((yyloc), IntVal::infinity); ;}
    break;

  case 132:

    { (yyval.expression)=new FloatLit((yyloc), (yyvsp[(1) - (1)].dValue)); ;}
    break;

  case 133:

    { (yyval.expression)=new StringLit((yyloc), (yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 134:

    { (yyval.expression)=constants().absent; ;}
    break;

  case 138:

    { (yyval.expression)=new ArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
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

  case 147:

    { (yyval.expression)=new ArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v); ;}
    break;

  case 148:

    { (yyval.expression_v)=(yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 149:

    { (yyval.expression) = new SetLit((yyloc), std::vector<Expression*>()); ;}
    break;

  case 150:

    { (yyval.expression) = new SetLit((yyloc), *(yyvsp[(2) - (3)].expression_v)); delete (yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 151:

    { (yyval.expression) = new Comprehension((yyloc), (yyvsp[(2) - (5)].expression), *(yyvsp[(4) - (5)].generators), true);
        delete (yyvsp[(4) - (5)].generators);
      ;}
    break;

  case 152:

    { (yyval.generators)=new Generators; (yyval.generators)->_g = *(yyvsp[(1) - (1)].generator_v); (yyval.generators)->_w = NULL; delete (yyvsp[(1) - (1)].generator_v); ;}
    break;

  case 153:

    { (yyval.generators)=new Generators; (yyval.generators)->_g = *(yyvsp[(1) - (3)].generator_v); (yyval.generators)->_w = (yyvsp[(3) - (3)].expression); delete (yyvsp[(1) - (3)].generator_v); ;}
    break;

  case 155:

    { (yyval.generator_v)=new std::vector<Generator>; (yyval.generator_v)->push_back(*(yyvsp[(1) - (1)].generator)); delete (yyvsp[(1) - (1)].generator); ;}
    break;

  case 156:

    { (yyval.generator_v)=(yyvsp[(1) - (3)].generator_v); (yyval.generator_v)->push_back(*(yyvsp[(3) - (3)].generator)); delete (yyvsp[(3) - (3)].generator); ;}
    break;

  case 157:

    { (yyval.generator)=new Generator(*(yyvsp[(1) - (3)].string_v),(yyvsp[(3) - (3)].expression)); delete (yyvsp[(1) - (3)].string_v); ;}
    break;

  case 159:

    { (yyval.string_v)=new std::vector<std::string>; (yyval.string_v)->push_back((yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 160:

    { (yyval.string_v)=(yyvsp[(1) - (3)].string_v); (yyval.string_v)->push_back((yyvsp[(3) - (3)].sValue)); free((yyvsp[(3) - (3)].sValue)); ;}
    break;

  case 161:

    { (yyval.expression)=new ArrayLit((yyloc), std::vector<MiniZinc::Expression*>()); ;}
    break;

  case 162:

    { (yyval.expression)=new ArrayLit((yyloc), *(yyvsp[(2) - (3)].expression_v)); delete (yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 163:

    { (yyval.expression)=new ArrayLit((yyloc), *(yyvsp[(2) - (3)].expression_vv));
        for (unsigned int i=1; i<(yyvsp[(2) - (3)].expression_vv)->size(); i++)
          if ((*(yyvsp[(2) - (3)].expression_vv))[i].size() != (*(yyvsp[(2) - (3)].expression_vv))[i-1].size())
            yyerror(&(yylsp[(2) - (3)]), parm, "syntax error, all sub-arrays of 2d array literal must have the same length");
        delete (yyvsp[(2) - (3)].expression_vv);
      ;}
    break;

  case 164:

    { (yyval.expression)=new ArrayLit((yyloc), *(yyvsp[(2) - (4)].expression_vv));
        for (unsigned int i=1; i<(yyvsp[(2) - (4)].expression_vv)->size(); i++)
          if ((*(yyvsp[(2) - (4)].expression_vv))[i].size() != (*(yyvsp[(2) - (4)].expression_vv))[i-1].size())
            yyerror(&(yylsp[(2) - (4)]), parm, "syntax error, all sub-arrays of 2d array literal must have the same length");
        delete (yyvsp[(2) - (4)].expression_vv);
      ;}
    break;

  case 165:

    { (yyval.expression_vv)=new std::vector<std::vector<MiniZinc::Expression*> >;
        (yyval.expression_vv)->push_back(*(yyvsp[(1) - (1)].expression_v));
        delete (yyvsp[(1) - (1)].expression_v);
      ;}
    break;

  case 166:

    { (yyval.expression_vv)=(yyvsp[(1) - (3)].expression_vv); (yyval.expression_vv)->push_back(*(yyvsp[(3) - (3)].expression_v)); delete (yyvsp[(3) - (3)].expression_v); ;}
    break;

  case 167:

    { (yyval.expression)=new Comprehension((yyloc), (yyvsp[(2) - (5)].expression), *(yyvsp[(4) - (5)].generators), false);
        delete (yyvsp[(4) - (5)].generators);
      ;}
    break;

  case 168:

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

  case 169:

    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; ;}
    break;

  case 170:

    { (yyval.expression_v)=(yyvsp[(1) - (5)].expression_v); (yyval.expression_v)->push_back((yyvsp[(3) - (5)].expression)); (yyval.expression_v)->push_back((yyvsp[(5) - (5)].expression)); ;}
    break;

  case 171:

    { (yyval.iValue)=BOT_EQUIV; ;}
    break;

  case 172:

    { (yyval.iValue)=BOT_IMPL; ;}
    break;

  case 173:

    { (yyval.iValue)=BOT_RIMPL; ;}
    break;

  case 174:

    { (yyval.iValue)=BOT_OR; ;}
    break;

  case 175:

    { (yyval.iValue)=BOT_XOR; ;}
    break;

  case 176:

    { (yyval.iValue)=BOT_AND; ;}
    break;

  case 177:

    { (yyval.iValue)=BOT_LE; ;}
    break;

  case 178:

    { (yyval.iValue)=BOT_GR; ;}
    break;

  case 179:

    { (yyval.iValue)=BOT_LQ; ;}
    break;

  case 180:

    { (yyval.iValue)=BOT_GQ; ;}
    break;

  case 181:

    { (yyval.iValue)=BOT_EQ; ;}
    break;

  case 182:

    { (yyval.iValue)=BOT_NQ; ;}
    break;

  case 183:

    { (yyval.iValue)=BOT_IN; ;}
    break;

  case 184:

    { (yyval.iValue)=BOT_SUBSET; ;}
    break;

  case 185:

    { (yyval.iValue)=BOT_SUPERSET; ;}
    break;

  case 186:

    { (yyval.iValue)=BOT_UNION; ;}
    break;

  case 187:

    { (yyval.iValue)=BOT_DIFF; ;}
    break;

  case 188:

    { (yyval.iValue)=BOT_SYMDIFF; ;}
    break;

  case 189:

    { (yyval.iValue)=BOT_PLUS; ;}
    break;

  case 190:

    { (yyval.iValue)=BOT_MINUS; ;}
    break;

  case 191:

    { (yyval.iValue)=BOT_MULT; ;}
    break;

  case 192:

    { (yyval.iValue)=BOT_DIV; ;}
    break;

  case 193:

    { (yyval.iValue)=BOT_IDIV; ;}
    break;

  case 194:

    { (yyval.iValue)=BOT_MOD; ;}
    break;

  case 195:

    { (yyval.iValue)=BOT_INTERSECT; ;}
    break;

  case 196:

    { (yyval.iValue)=BOT_PLUSPLUS; ;}
    break;

  case 197:

    { (yyval.iValue)=-1; ;}
    break;

  case 198:

    { if ((yyvsp[(1) - (6)].iValue)==-1) {
          (yyval.expression)=NULL;
          yyerror(&(yylsp[(3) - (6)]), parm, "syntax error, unary operator with two arguments");
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[(3) - (6)].expression),static_cast<BinOpType>((yyvsp[(1) - (6)].iValue)),(yyvsp[(5) - (6)].expression));
        }
      ;}
    break;

  case 199:

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

  case 200:

    { (yyval.expression)=new Call((yyloc), (yyvsp[(1) - (3)].sValue), std::vector<Expression*>()); free((yyvsp[(1) - (3)].sValue)); ;}
    break;

  case 202:

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

  case 203:

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

  case 204:

    { (yyval.expression_p)=new pair<vector<Expression*>,Expression*>;
        (yyval.expression_p)->first=*(yyvsp[(1) - (1)].expression_v); (yyval.expression_p)->second=NULL;
        delete (yyvsp[(1) - (1)].expression_v);
      ;}
    break;

  case 205:

    { (yyval.expression_p)=new pair<vector<Expression*>,Expression*>;
        (yyval.expression_p)->first=*(yyvsp[(1) - (3)].expression_v); (yyval.expression_p)->second=(yyvsp[(3) - (3)].expression);
        delete (yyvsp[(1) - (3)].expression_v);
      ;}
    break;

  case 206:

    { (yyval.expression)=new Let((yyloc), *(yyvsp[(3) - (6)].expression_v), (yyvsp[(6) - (6)].expression)); delete (yyvsp[(3) - (6)].expression_v); ;}
    break;

  case 207:

    { (yyval.expression)=new Let((yyloc), *(yyvsp[(3) - (7)].expression_v), (yyvsp[(7) - (7)].expression)); delete (yyvsp[(3) - (7)].expression_v); ;}
    break;

  case 208:

    { (yyval.expression_v)=new vector<Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].vardeclexpr)); ;}
    break;

  case 209:

    { (yyval.expression_v)=new vector<Expression*>;
        ConstraintI* ce = (yyvsp[(1) - (1)].item)->cast<ConstraintI>();
        (yyval.expression_v)->push_back(ce->e());
        ce->e(NULL);
      ;}
    break;

  case 210:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); (yyval.expression_v)->push_back((yyvsp[(3) - (3)].vardeclexpr)); ;}
    break;

  case 211:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v);
        ConstraintI* ce = (yyvsp[(3) - (3)].item)->cast<ConstraintI>();
        (yyval.expression_v)->push_back(ce->e());
        ce->e(NULL);
      ;}
    break;

  case 214:

    { (yyval.vardeclexpr) = (yyvsp[(1) - (2)].vardeclexpr);
        (yyval.vardeclexpr)->toplevel(false);
        if ((yyvsp[(2) - (2)].expression_v)) (yyval.vardeclexpr)->addAnnotations(*(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v);
      ;}
    break;

  case 215:

    { (yyvsp[(1) - (4)].vardeclexpr)->e((yyvsp[(4) - (4)].expression));
        (yyval.vardeclexpr) = (yyvsp[(1) - (4)].vardeclexpr);
        (yyval.vardeclexpr)->loc((yyloc));
        (yyval.vardeclexpr)->toplevel(false);
        if ((yyvsp[(2) - (4)].expression_v)) (yyval.vardeclexpr)->addAnnotations(*(yyvsp[(2) - (4)].expression_v));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 216:

    { (yyval.expression_v)=NULL; ;}
    break;

  case 218:

    { (yyval.expression_v)=new std::vector<Expression*>(1);
        (*(yyval.expression_v))[0] = (yyvsp[(2) - (2)].expression);
      ;}
    break;

  case 219:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); (yyval.expression_v)->push_back((yyvsp[(3) - (3)].expression)); ;}
    break;

  case 220:

    { (yyval.sValue)=(yyvsp[(1) - (1)].sValue); ;}
    break;

  case 221:

    { (yyval.sValue)=strdup("@<->"); ;}
    break;

  case 222:

    { (yyval.sValue)=strdup("@->"); ;}
    break;

  case 223:

    { (yyval.sValue)=strdup("@<-"); ;}
    break;

  case 224:

    { (yyval.sValue)=strdup("@\\/"); ;}
    break;

  case 225:

    { (yyval.sValue)=strdup("@xor"); ;}
    break;

  case 226:

    { (yyval.sValue)=strdup("@/\\"); ;}
    break;

  case 227:

    { (yyval.sValue)=strdup("@<"); ;}
    break;

  case 228:

    { (yyval.sValue)=strdup("@>"); ;}
    break;

  case 229:

    { (yyval.sValue)=strdup("@<="); ;}
    break;

  case 230:

    { (yyval.sValue)=strdup("@>="); ;}
    break;

  case 231:

    { (yyval.sValue)=strdup("@="); ;}
    break;

  case 232:

    { (yyval.sValue)=strdup("@!="); ;}
    break;

  case 233:

    { (yyval.sValue)=strdup("@in"); ;}
    break;

  case 234:

    { (yyval.sValue)=strdup("@subset"); ;}
    break;

  case 235:

    { (yyval.sValue)=strdup("@superset"); ;}
    break;

  case 236:

    { (yyval.sValue)=strdup("@union"); ;}
    break;

  case 237:

    { (yyval.sValue)=strdup("@diff"); ;}
    break;

  case 238:

    { (yyval.sValue)=strdup("@symdiff"); ;}
    break;

  case 239:

    { (yyval.sValue)=strdup("@.."); ;}
    break;

  case 240:

    { (yyval.sValue)=strdup("@+"); ;}
    break;

  case 241:

    { (yyval.sValue)=strdup("@-"); ;}
    break;

  case 242:

    { (yyval.sValue)=strdup("@*"); ;}
    break;

  case 243:

    { (yyval.sValue)=strdup("@/"); ;}
    break;

  case 244:

    { (yyval.sValue)=strdup("@div"); ;}
    break;

  case 245:

    { (yyval.sValue)=strdup("@mod"); ;}
    break;

  case 246:

    { (yyval.sValue)=strdup("@intersect"); ;}
    break;

  case 247:

    { (yyval.sValue)=strdup("@not"); ;}
    break;

  case 248:

    { (yyval.sValue)=strdup("@++"); ;}
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



