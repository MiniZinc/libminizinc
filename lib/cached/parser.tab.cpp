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
#include <minizinc/json_parser.hh>

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
  pp->syntaxErrors.push_back(SyntaxError(*location, str));
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
                         ostream& err,
                         std::vector<SyntaxError>& syntaxErrors) {
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
    bool isFzn;
    if (filename=="") {
      isFzn = false;
    } else {
      isFzn = (filename.compare(filename.length()-4,4,".fzn")==0);
      isFzn |= (filename.compare(filename.length()-4,4,".ozn")==0);
      isFzn |= (filename.compare(filename.length()-4,4,".szn")==0);
    }
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
    
    vector<pair<string,Model*> > files;
    map<string,Model*> seenModels;
    
    if (filenames.size() > 0) {
      GCLock lock;
      string fileDirname; string fileBasename;
      filepath(filenames[0], fileDirname, fileBasename);
      model->setFilename(fileBasename);
      
      files.push_back(pair<string,Model*>(fileDirname,model));
      
      for (unsigned int i=1; i<filenames.size(); i++) {
        GCLock lock;
        string dirName, baseName;
        filepath(filenames[i], dirName, baseName);
        
        Model* includedModel = new Model;
        includedModel->setFilename(baseName);
        files.push_back(pair<string,Model*>(dirName,includedModel));
        seenModels.insert(pair<string,Model*>(baseName,includedModel));
        Location loc;
        loc.filename=ASTString(filenames[i]);
        IncludeI* inc = new IncludeI(loc,includedModel->filename());
        inc->m(includedModel,true);
        model->addItem(inc);
      }
    }
    
    if (!ignoreStdlib) {
      GCLock lock;
      Model* stdlib = new Model;
      stdlib->setFilename("stdlib.mzn");
      files.push_back(pair<string,Model*>("./",stdlib));
      seenModels.insert(pair<string,Model*>("stdlib.mzn",stdlib));
      Location stdlibloc;
      stdlibloc.filename=ASTString(model->filename());
      IncludeI* stdlibinc =
      new IncludeI(stdlibloc,stdlib->filename());
      stdlibinc->m(stdlib,true);
      model->addItem(stdlibinc);
    }
    
    while (!files.empty()) {
      GCLock lock;
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
        if (filenames.size() == 0) {
          err << "Internal error." << endl;
          goto error;
        }
        fullname = parentPath + f;  // filenames[0];
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
#define YYFINAL  153
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   4142

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  132
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  65
/* YYNRULES -- Number of rules.  */
#define YYNRULES  274
/* YYNRULES -- Number of states.  */
#define YYNSTATES  471

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
     127,   128,     2,     2,   129,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   130,   124,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   125,   131,   126,     2,     2,     2,     2,
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
      48,    50,    52,    54,    56,    58,    61,    64,    69,    72,
      79,    87,    88,    90,    94,    98,   101,   105,   110,   115,
     118,   124,   130,   138,   147,   151,   157,   158,   161,   162,
     166,   170,   171,   174,   176,   180,   181,   183,   185,   187,
     191,   194,   196,   200,   202,   209,   213,   215,   218,   222,
     226,   231,   237,   243,   244,   246,   248,   250,   252,   254,
     256,   258,   260,   263,   265,   269,   271,   275,   279,   283,
     287,   291,   298,   302,   306,   310,   314,   318,   322,   326,
     330,   334,   337,   340,   342,   346,   350,   354,   358,   362,
     366,   370,   374,   378,   382,   386,   390,   394,   398,   402,
     406,   410,   414,   418,   422,   429,   433,   437,   441,   445,
     449,   453,   457,   461,   465,   468,   471,   474,   478,   483,
     485,   488,   490,   493,   495,   497,   499,   501,   503,   505,
     507,   510,   512,   515,   517,   520,   522,   525,   527,   530,
     532,   535,   537,   539,   542,   544,   547,   550,   554,   558,
     563,   566,   570,   576,   578,   582,   585,   587,   591,   595,
     598,   600,   604,   607,   611,   614,   618,   623,   627,   630,
     634,   640,   642,   646,   652,   661,   662,   668,   670,   672,
     674,   676,   678,   680,   682,   684,   686,   688,   690,   692,
     694,   696,   698,   700,   702,   704,   706,   708,   710,   712,
     714,   716,   718,   720,   722,   729,   734,   738,   740,   745,
     753,   755,   759,   766,   774,   776,   778,   782,   786,   788,
     790,   793,   798,   799,   801,   804,   808,   810,   812,   814,
     816,   818,   820,   822,   824,   826,   828,   830,   832,   834,
     836,   838,   840,   842,   844,   846,   848,   850,   852,   854,
     856,   858,   860,   862,   864
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     133,     0,    -1,   134,    -1,    -1,   135,   137,    -1,   138,
      -1,   136,   138,    -1,   135,   124,   138,    -1,   135,   124,
     136,   138,    -1,     1,   124,   138,    -1,    14,    -1,   136,
      14,    -1,    -1,   124,    -1,    13,   139,    -1,   139,    -1,
     140,    -1,   141,    -1,   143,    -1,   144,    -1,   145,    -1,
     146,    -1,   147,    -1,   148,    -1,   149,    -1,    33,     8,
      -1,   156,   194,    -1,   156,   194,    75,   166,    -1,    29,
       6,    -1,    29,     6,    75,   125,   142,   126,    -1,    29,
       6,    75,     6,   127,   166,   128,    -1,    -1,     6,    -1,
     142,   129,     6,    -1,     6,    75,   166,    -1,    24,   166,
      -1,    47,   194,    42,    -1,    47,   194,    39,   166,    -1,
      47,   194,    38,   166,    -1,    43,   166,    -1,    44,     6,
     151,   194,   150,    -1,    49,     6,   151,   194,   150,    -1,
      31,   159,   130,   196,   151,   194,   150,    -1,   159,   130,
       6,   127,   152,   128,   194,   150,    -1,    19,     6,   151,
      -1,    19,     6,   151,    75,   166,    -1,    -1,    75,   166,
      -1,    -1,   127,   152,   128,    -1,   127,     1,   128,    -1,
      -1,   153,   154,    -1,   155,    -1,   153,   129,   155,    -1,
      -1,   129,    -1,   156,    -1,   159,    -1,   159,   130,     6,
      -1,   158,   154,    -1,   159,    -1,   158,   129,   159,    -1,
     160,    -1,    21,    56,   157,    58,    40,   160,    -1,    37,
      40,   160,    -1,   162,    -1,    41,   162,    -1,    16,   161,
     162,    -1,    15,   161,   162,    -1,   161,    46,    40,   162,
      -1,    16,   161,    46,    40,   162,    -1,    15,   161,    46,
      40,   162,    -1,    -1,    41,    -1,    35,    -1,    22,    -1,
      30,    -1,    48,    -1,    18,    -1,   165,    -1,    12,    -1,
     164,   154,    -1,   166,    -1,   164,   129,   166,    -1,   167,
      -1,   165,    93,   167,    -1,   165,    80,   165,    -1,   165,
      81,   165,    -1,   165,    82,   165,    -1,   165,    83,   165,
      -1,   113,   127,   166,   129,   166,   128,    -1,   165,    90,
     165,    -1,   165,    92,   165,    -1,   165,    84,   165,    -1,
     165,    85,   165,    -1,   165,    86,   165,    -1,   165,    87,
     165,    -1,   165,    88,   165,    -1,   165,    89,   165,    -1,
     165,     7,   165,    -1,    84,   165,    -1,    85,   165,    -1,
     167,    -1,   166,    93,   167,    -1,   166,    65,   166,    -1,
     166,    66,   166,    -1,   166,    67,   166,    -1,   166,    68,
     166,    -1,   166,    69,   166,    -1,   166,    70,   166,    -1,
     166,    71,   166,    -1,   166,    72,   166,    -1,   166,    73,
     166,    -1,   166,    74,   166,    -1,   166,    75,   166,    -1,
     166,    76,   166,    -1,   166,    77,   166,    -1,   166,    78,
     166,    -1,   166,    79,   166,    -1,   166,    80,   166,    -1,
     166,    81,   166,    -1,   166,    82,   166,    -1,   166,    83,
     166,    -1,   113,   127,   166,   129,   166,   128,    -1,   166,
      90,   166,    -1,   166,    92,   166,    -1,   166,    84,   166,
      -1,   166,    85,   166,    -1,   166,    86,   166,    -1,   166,
      87,   166,    -1,   166,    88,   166,    -1,   166,    89,   166,
      -1,   166,     7,   166,    -1,    91,   166,    -1,    84,   166,
      -1,    85,   166,    -1,   127,   166,   128,    -1,   127,   166,
     128,   170,    -1,     6,    -1,     6,   170,    -1,    53,    -1,
      53,   170,    -1,     4,    -1,     3,    -1,    34,    -1,     5,
      -1,   168,    -1,    17,    -1,   171,    -1,   171,   170,    -1,
     172,    -1,   172,   170,    -1,   179,    -1,   179,   170,    -1,
     180,    -1,   180,   170,    -1,   183,    -1,   183,   170,    -1,
     184,    -1,   184,   170,    -1,   190,    -1,   188,    -1,   188,
     170,    -1,     8,    -1,     9,   169,    -1,   164,    11,    -1,
     164,    10,   169,    -1,    56,   163,    58,    -1,   170,    56,
     163,    58,    -1,   125,   126,    -1,   125,   163,   126,    -1,
     125,   166,   131,   173,   126,    -1,   174,    -1,   174,    55,
     166,    -1,   175,   154,    -1,   176,    -1,   175,   129,   176,
      -1,   177,    77,   166,    -1,   178,   154,    -1,     6,    -1,
     178,   129,     6,    -1,    56,    58,    -1,    56,   163,    58,
      -1,    57,    59,    -1,    57,   182,    59,    -1,    57,   182,
     131,    59,    -1,    57,   181,    59,    -1,   131,   131,    -1,
     131,   182,   131,    -1,   181,   129,   131,   182,   131,    -1,
     163,    -1,   182,   131,   163,    -1,    56,   166,   131,   173,
      58,    -1,    32,   166,    50,   166,   185,    26,   166,    28,
      -1,    -1,   185,    27,   166,    50,   166,    -1,    95,    -1,
      96,    -1,    97,    -1,    98,    -1,    99,    -1,   100,    -1,
     101,    -1,   102,    -1,   103,    -1,   104,    -1,   105,    -1,
     106,    -1,   107,    -1,   108,    -1,   109,    -1,   110,    -1,
     111,    -1,   112,    -1,   114,    -1,   115,    -1,   116,    -1,
     117,    -1,   118,    -1,   119,    -1,   120,    -1,   123,    -1,
     121,    -1,   186,   127,   166,   129,   166,   128,    -1,   186,
     127,   166,   128,    -1,     6,   127,   128,    -1,   187,    -1,
       6,   127,   189,   128,    -1,     6,   127,   189,   128,   127,
     166,   128,    -1,   163,    -1,   163,    55,   166,    -1,    36,
     125,   191,   126,    77,   166,    -1,    36,   125,   191,   192,
     126,    77,   166,    -1,   193,    -1,   144,    -1,   191,   192,
     193,    -1,   191,   192,   144,    -1,   129,    -1,   124,    -1,
     156,   194,    -1,   156,   194,    75,   166,    -1,    -1,   195,
      -1,    93,   167,    -1,   195,    93,   167,    -1,     6,    -1,
      95,    -1,    96,    -1,    97,    -1,    98,    -1,    99,    -1,
     100,    -1,   101,    -1,   102,    -1,   103,    -1,   104,    -1,
     105,    -1,   106,    -1,   107,    -1,   108,    -1,   109,    -1,
     110,    -1,   111,    -1,   112,    -1,   113,    -1,   114,    -1,
     115,    -1,   116,    -1,   117,    -1,   118,    -1,   119,    -1,
     120,    -1,   121,    -1,   123,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   612,   612,   614,   616,   619,   628,   637,   646,   655,
     658,   666,   675,   675,   677,   693,   697,   699,   701,   702,
     704,   706,   708,   710,   712,   716,   739,   745,   752,   760,
     770,   784,   785,   789,   793,   799,   803,   808,   813,   820,
     824,   832,   842,   849,   858,   870,   878,   879,   884,   885,
     887,   892,   893,   897,   901,   906,   906,   909,   911,   915,
     920,   924,   926,   930,   931,   937,   946,   949,   957,   965,
     974,   983,   992,  1005,  1006,  1010,  1012,  1014,  1016,  1018,
    1020,  1022,  1028,  1031,  1033,  1039,  1040,  1042,  1044,  1046,
    1048,  1057,  1066,  1068,  1070,  1072,  1074,  1076,  1078,  1080,
    1082,  1088,  1090,  1104,  1105,  1107,  1109,  1111,  1113,  1115,
    1117,  1119,  1121,  1123,  1125,  1127,  1129,  1131,  1133,  1135,
    1137,  1139,  1141,  1143,  1152,  1161,  1163,  1165,  1167,  1169,
    1171,  1173,  1175,  1177,  1183,  1185,  1192,  1204,  1206,  1208,
    1210,  1213,  1215,  1218,  1220,  1222,  1224,  1226,  1227,  1229,
    1230,  1233,  1234,  1237,  1238,  1241,  1242,  1245,  1246,  1249,
    1250,  1253,  1254,  1255,  1260,  1262,  1268,  1273,  1281,  1288,
    1297,  1299,  1304,  1310,  1312,  1315,  1318,  1320,  1324,  1327,
    1330,  1332,  1336,  1338,  1342,  1344,  1355,  1366,  1406,  1409,
    1414,  1421,  1426,  1430,  1436,  1452,  1453,  1457,  1459,  1461,
    1463,  1465,  1467,  1469,  1471,  1473,  1475,  1477,  1479,  1481,
    1483,  1485,  1487,  1489,  1491,  1493,  1495,  1497,  1499,  1501,
    1503,  1505,  1507,  1509,  1513,  1521,  1554,  1556,  1557,  1568,
    1611,  1617,  1625,  1632,  1641,  1643,  1651,  1653,  1662,  1662,
    1665,  1671,  1682,  1683,  1686,  1690,  1694,  1696,  1698,  1700,
    1702,  1704,  1706,  1708,  1710,  1712,  1714,  1716,  1718,  1720,
    1722,  1724,  1726,  1728,  1730,  1732,  1734,  1736,  1738,  1740,
    1742,  1744,  1746,  1748,  1750
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
  "\"'not'\"", "\"'::'\"", "\"'++'\"", "';'", "'{'", "'}'", "'('", "')'",
  "','", "':'", "'|'", "$accept", "model", "item_list", "item_list_head",
  "doc_file_comments", "semi_or_none", "item", "item_tail", "include_item",
  "vardecl_item", "enum_id_list", "assign_item", "constraint_item",
  "solve_item", "output_item", "predicate_item", "function_item",
  "annotation_item", "operation_item_tail", "params", "params_list",
  "params_list_head", "comma_or_none", "ti_expr_and_id_or_anon",
  "ti_expr_and_id", "ti_expr_list", "ti_expr_list_head", "ti_expr",
  "base_ti_expr", "opt_opt", "base_ti_expr_tail", "expr_list",
  "expr_list_head", "set_expr", "expr", "expr_atom_head", "string_expr",
  "string_quote_rest", "array_access_tail", "set_literal", "set_comp",
  "comp_tail", "generator_list", "generator_list_head", "generator",
  "id_list", "id_list_head", "simple_array_literal",
  "simple_array_literal_2d", "simple_array_literal_3d_list",
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
     375,   376,   377,   378,    59,   123,   125,    40,    41,    44,
      58,   124
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   132,   133,   134,   134,   135,   135,   135,   135,   135,
     136,   136,   137,   137,   138,   138,   139,   139,   139,   139,
     139,   139,   139,   139,   139,   140,   141,   141,   141,   141,
     141,   142,   142,   142,   143,   144,   145,   145,   145,   146,
     147,   147,   148,   148,   149,   149,   150,   150,   151,   151,
     151,   152,   152,   153,   153,   154,   154,   155,   155,   156,
     157,   158,   158,   159,   159,   159,   160,   160,   160,   160,
     160,   160,   160,   161,   161,   162,   162,   162,   162,   162,
     162,   162,   163,   164,   164,   165,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   165,   165,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   168,   168,   169,   169,   170,   170,
     171,   171,   172,   173,   173,   174,   175,   175,   176,   177,
     178,   178,   179,   179,   180,   180,   180,   180,   181,   181,
     181,   182,   182,   183,   184,   185,   185,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   187,   187,   188,   188,   188,   188,
     189,   189,   190,   190,   191,   191,   191,   191,   192,   192,
     193,   193,   194,   194,   195,   195,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
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
       1,     1,     2,     1,     3,     1,     3,     3,     3,     3,
       3,     6,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     2,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     6,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     2,     2,     3,     4,     1,
       2,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       2,     1,     2,     1,     2,     1,     2,     1,     2,     1,
       2,     1,     1,     2,     1,     2,     2,     3,     3,     4,
       2,     3,     5,     1,     3,     2,     1,     3,     3,     2,
       1,     3,     2,     3,     2,     3,     4,     3,     2,     3,
       5,     1,     3,     5,     8,     0,     5,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     6,     4,     3,     1,     4,     7,
       1,     3,     6,     7,     1,     1,     3,     3,     1,     1,
       2,     4,     0,     1,     2,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,   144,   143,   146,   139,   164,     0,    81,    73,
      10,    73,    73,   148,    79,     0,     0,    76,     0,     0,
      77,    73,     0,     0,   145,    75,     0,     0,    74,     0,
       0,   242,    78,     0,   141,     0,     0,     0,     0,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,     0,   215,   216,
     217,   218,   219,   220,   221,   223,   222,     0,     0,     0,
       2,    12,    73,     5,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,   242,     0,    63,     0,    66,    80,
      85,   147,   149,   151,   153,   155,   157,   159,     0,   227,
     162,   161,    73,     0,     0,     0,   140,   139,     0,     0,
       0,     0,     0,    83,   103,   165,    14,    74,     0,     0,
      48,    73,    35,    28,     0,     0,    25,    73,    73,    67,
      39,    48,     0,     0,   243,    48,   142,   182,     0,    55,
      83,   184,     0,   191,     0,     0,   101,   102,     0,   170,
       0,    83,     0,     1,    13,     4,    11,     6,    26,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   150,   152,   154,   156,   158,
     160,     0,   163,     9,     0,    34,   226,   230,     0,     0,
     135,   136,   134,     0,     0,   166,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    69,     0,    68,
       0,    44,     0,    55,    61,     0,     0,     0,   235,   242,
       0,     0,   234,    65,   242,   244,     0,     0,    36,     0,
     242,   183,    56,    82,     0,   188,     0,   187,     0,   185,
       0,     0,   171,     0,   137,    73,     7,     0,    59,     0,
     100,    87,    88,    89,    90,    94,    95,    96,    97,    98,
      99,    92,    93,    86,     0,   168,     0,   228,     0,     0,
     167,    84,   133,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   127,   128,   129,   130,   131,   132,   125,   126,
     104,     0,     0,     0,     0,    55,    53,    57,    58,     0,
       0,    56,    60,     0,    31,   246,   247,   248,   249,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,    48,   195,   240,     0,   239,     0,
     238,    73,    46,    38,    37,   245,    46,   180,     0,   173,
      55,   176,     0,    55,   189,     0,   186,   192,     0,     0,
     138,     8,    27,    51,    70,   225,     0,   231,     0,   169,
       0,    72,    71,    50,    49,    56,    52,    45,    73,    62,
       0,    32,     0,   242,     0,     0,    59,     0,     0,   237,
     236,     0,    40,    41,   193,     0,    56,   175,     0,    56,
     179,     0,     0,   172,     0,     0,     0,     0,    54,    64,
       0,    29,     0,    46,     0,     0,   241,   232,     0,    47,
     174,   177,   178,   181,   190,    91,   242,   224,   229,   124,
      30,    33,    42,     0,     0,   233,    46,   194,     0,    43,
     196
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    69,    70,    71,    72,   155,    73,    74,    75,    76,
     412,    77,    78,    79,    80,    81,    82,    83,   422,   231,
     324,   325,   253,   326,    84,   232,   233,    85,    86,    87,
      88,   143,   139,    89,   113,   114,    91,   115,   106,    92,
      93,   378,   379,   380,   381,   382,   383,    94,    95,   144,
     145,    96,    97,   414,    98,    99,   100,   188,   101,   241,
     371,   242,   133,   134,   364
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -356
static const yytype_int16 yypact[] =
{
     762,   -99,  -356,  -356,  -356,    -1,  -356,  3014,  -356,  1514,
    -356,    -9,    -9,  -356,  -356,    24,   -17,  -356,  3014,    42,
    -356,  2014,  3014,    32,  -356,  -356,   -72,    14,  2514,  3014,
      50,   -36,  -356,    52,     4,  2639,   479,  3139,  3139,  -356,
    -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,
    -356,  -356,  -356,  -356,  -356,  -356,  -356,   -68,  -356,  -356,
    -356,  -356,  -356,  -356,  -356,  -356,  -356,  2764,  3014,    61,
    -356,   -62,  1264,  -356,  -356,  -356,  -356,  -356,  -356,  -356,
    -356,  -356,  -356,  -356,   -36,   -66,  -356,    19,  -356,   234,
    -356,  -356,     4,     4,     4,     4,     4,     4,   -61,  -356,
       4,  -356,  1389,  3014,  3014,  1013,    11,     7,  3014,  3014,
    3014,   -59,     2,  4021,  -356,  -356,  -356,  -356,  2264,  2389,
     -57,  2014,  4021,    -3,   -55,  3908,  -356,  1764,  2139,  -356,
    4021,   -57,  3173,   -11,   -20,   -57,    11,  -356,    18,   -52,
    3254,  -356,   633,  -356,   -23,   -24,    40,    40,  3014,  -356,
     -48,  3288,  3514,  -356,  1139,  -356,  -356,  -356,     5,    73,
      41,  3139,  3139,  3139,  3139,  3139,  3139,  3139,  3139,  3139,
    3139,  3139,  3139,  3139,  3173,    11,    11,    11,    11,    11,
      11,  3014,    11,  -356,    25,  4021,  -356,    27,   -44,  3014,
      44,    44,    44,  3014,  3014,  -356,  3014,  3014,  3014,  3014,
    3014,  3014,  3014,  3014,  3014,  3014,  3014,  3014,  3014,  3014,
    3014,  3014,  3014,  3014,  3014,  3014,  3014,  3014,  3014,  3014,
    3014,  3014,  3014,  3014,  3014,  3173,    45,  -356,    49,  -356,
     887,    54,    77,     9,  -356,    28,  3746,  3014,  -356,   -36,
      22,  -100,  -356,  -356,   -36,  -356,  3014,  3014,  -356,  3173,
     -36,  -356,  3014,  -356,   148,  -356,    30,  -356,    31,  -356,
    2889,  3401,  -356,   148,     4,  1264,  -356,  3014,    37,  2514,
      65,   206,   206,   206,   270,    80,    80,    40,    40,    40,
      40,   206,    12,  -356,  3372,  -356,  3014,    43,   116,  3485,
    -356,  4021,    83,  4049,   752,   752,   877,   877,  1002,  1128,
    1128,  1128,  1128,  1128,  1128,    35,    35,    35,   354,   354,
     354,   368,   150,   150,    44,    44,    44,    44,   354,    16,
    -356,  2514,  2514,    81,    82,    48,  -356,  -356,    22,  3014,
     168,  1889,  -356,    85,   208,  -356,  -356,  -356,  -356,  -356,
    -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,
    -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,
    -356,  -356,  -356,  -356,   -57,  4021,   141,   211,  -356,   143,
    -356,  1639,   146,  4021,  4021,  -356,   146,  -356,   160,   167,
      94,  -356,   147,    96,  3014,  3014,  -356,  -356,  3014,   101,
      11,  -356,  4021,  1889,  -356,  -356,  3014,  4021,  3014,  -356,
    3014,  -356,  -356,  -356,  -356,  1889,  -356,  4021,  2139,  -356,
    3014,  -356,   -88,   -36,    17,  3014,  -356,  3014,   153,  -356,
    -356,  3014,  -356,  -356,  -356,  3014,   148,  -356,  3014,   225,
    -356,   102,  3598,  -356,   104,  3627,  3711,  3740,  -356,  -356,
    3824,  -356,   229,   146,  3014,  3014,  4021,  4021,  3014,  4021,
    4021,  -356,  4021,  -356,  3014,  -356,   -36,  -356,  -356,  -356,
    -356,  -356,  -356,  3937,  3992,  4021,   146,  -356,  3014,  -356,
    4021
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -356,  -356,  -356,  -356,    86,  -356,   -50,   236,  -356,  -356,
    -356,  -356,  -119,  -356,  -356,  -356,  -356,  -356,  -355,  -117,
    -147,  -356,  -224,  -157,  -120,  -356,  -356,   -19,  -122,    34,
     -25,   -34,    13,   -22,   -18,   304,  -356,    56,     3,  -356,
    -356,    -8,  -356,  -356,  -172,  -356,  -356,  -356,  -356,  -356,
    -132,  -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,  -356,
    -356,  -115,   -79,  -356,  -356
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -74
static const yytype_int16 yytable[] =
{
     122,   138,   124,   129,   125,   158,   243,   239,   238,   332,
     256,   130,   194,   195,   244,   146,   147,   140,   250,   161,
     112,   423,   157,   197,   368,   102,   369,   246,   247,   370,
     120,   248,   117,   150,   333,   259,   257,   136,   441,   121,
     126,   442,   197,   444,   445,   118,   119,   161,   123,   151,
     152,   197,   183,   127,   128,   103,   131,   132,   135,   148,
     103,   153,   154,   103,   159,   160,   181,   189,   193,   184,
     230,   187,   235,   249,   104,   236,   251,   252,   262,   268,
     267,   269,   286,   285,   287,   321,   185,   161,   462,   322,
     190,   191,   192,   227,   229,   175,   176,   177,   178,   179,
     180,   406,   234,   182,   266,   174,   258,   260,   240,   225,
     327,   469,   -74,   -74,   -74,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,   105,   224,   225,   329,
     261,   196,   173,   174,   105,   330,   224,   225,   331,   270,
     271,   272,   273,   274,   275,   276,   277,   278,   279,   280,
     281,   282,   367,   334,   377,   288,   427,   197,   174,   430,
     366,   384,   385,   284,   393,   372,   168,   169,   170,   171,
     398,   376,   173,   174,   399,   289,   225,   405,   291,   292,
     293,   294,   295,   296,   297,   298,   299,   300,   301,   302,
     303,   304,   305,   306,   307,   308,   309,   310,   311,   312,
     313,   314,   315,   316,   317,   318,   319,   112,   408,   403,
     404,   328,   410,   161,   411,   391,   415,   416,   424,   365,
     417,   421,   425,   426,   428,   429,   387,   433,   373,   374,
     448,   453,   456,   454,   291,   461,   219,   220,   221,   222,
     265,   161,   224,   225,   394,   116,   434,   413,   438,   392,
     290,   239,   419,   431,   451,   389,   420,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   390,   397,     0,
       0,     0,     0,   327,     0,     0,     0,   161,     0,     0,
       0,     0,     0,     0,     0,   327,   439,     0,     0,   165,
     166,   167,   168,   169,   170,   171,   401,   402,   173,   174,
       0,     0,     0,     0,    90,     0,     0,     0,     0,     0,
       0,   407,   409,    90,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,    90,   173,   174,     0,     0,
       0,     0,    90,     0,   443,     0,     0,     0,     0,     0,
       0,    90,    90,     0,     0,     0,     0,     0,     0,     0,
     387,     0,   240,   -74,   166,   167,   168,   169,   170,   171,
       0,   197,   173,   174,     0,     0,     0,     0,     0,     0,
     432,     0,     0,     0,   328,   197,    90,   466,   435,     0,
     436,     0,   437,     0,     0,     0,   328,     0,     0,     0,
       0,     0,   440,     0,     0,     0,     0,   446,     0,   447,
       0,     0,     0,   449,     0,     0,    90,   450,     0,     0,
     452,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     387,     0,    90,    90,     0,    90,   463,   464,     0,     0,
     465,    90,    90,     0,     0,     0,   245,   216,   217,   218,
     219,   220,   221,   222,     0,     0,   224,   225,     0,     0,
     470,   -74,   217,   218,   219,   220,   221,   222,    90,     0,
     224,   225,     0,     0,     0,    90,    90,    90,    90,    90,
      90,    90,    90,    90,    90,    90,    90,    90,   283,     0,
       0,     0,     2,     3,     4,   107,     0,     6,     7,     0,
       0,     0,     0,     0,     0,     0,    13,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    22,     0,    24,     0,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   320,
       0,     0,    34,     0,    90,    35,    36,     0,   141,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   375,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   108,   109,     0,     0,     0,     0,    90,
     110,     0,     0,    90,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,   111,    58,    59,    60,    61,    62,    63,    64,
      65,     0,    66,     0,    67,     0,    68,     0,     0,     0,
     142,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    90,    90,     0,     0,     0,
       0,     0,     0,     0,     0,    90,     2,     3,     4,   107,
       0,     6,     7,     0,     0,     0,     0,     0,     0,     0,
      13,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    22,     0,    24,     0,    26,
       0,     0,     0,     0,     0,    90,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    34,     0,     0,    35,
      36,     0,     0,     0,     0,     0,     0,    90,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    90,
       0,     0,    90,     0,     0,     0,     0,   108,   109,     0,
       0,     0,     0,     0,   110,     0,     0,     0,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,   111,    58,    59,    60,
      61,    62,    63,    64,    65,     0,    66,     0,    67,   197,
      68,     0,    -3,     1,   255,     2,     3,     4,     5,     0,
       6,     7,     0,     0,     8,     9,    10,    11,    12,    13,
      14,    15,     0,    16,    17,     0,    18,     0,     0,     0,
       0,    19,    20,    21,    22,    23,    24,    25,    26,    27,
       0,     0,     0,    28,     0,    29,    30,     0,   -73,    31,
      32,    33,     0,     0,     0,    34,     0,     0,    35,    36,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,     0,   224,   225,    37,    38,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,   197,    66,     0,    67,   323,    68,
       2,     3,     4,   107,     0,     6,     7,     0,     0,     8,
       0,     0,    11,    12,    13,    14,     0,     0,    16,    17,
       0,     0,     0,     0,     0,     0,     0,    20,     0,    22,
       0,    24,    25,    26,    27,     0,     0,     0,    28,     0,
       0,     0,     0,   -73,     0,    32,     0,     0,     0,     0,
      34,     0,     0,    35,    36,     0,     0,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,     0,   224,
     225,    37,    38,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,   197,
      66,     0,    67,     0,    68,   -51,     2,     3,     4,   107,
       0,     6,     7,     0,     0,     0,     0,     0,     0,     0,
      13,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    22,     0,    24,     0,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    34,     0,     0,    35,
      36,     0,     0,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,     0,   224,   225,     0,   108,   109,     0,
       0,     0,     0,     0,   110,     0,     0,     0,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,   111,    58,    59,    60,
      61,    62,    63,    64,    65,   197,    66,     0,    67,     0,
      68,   186,     2,     3,     4,     5,     0,     6,     7,     0,
       0,     8,     9,    10,    11,    12,    13,    14,    15,     0,
      16,    17,     0,    18,     0,     0,     0,     0,    19,    20,
      21,    22,    23,    24,    25,    26,    27,     0,     0,     0,
      28,     0,    29,    30,     0,   -73,    31,    32,    33,     0,
       0,     0,    34,     0,     0,    35,    36,     0,     0,   -74,
     -74,   -74,   -74,   -74,   -74,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,     0,
     224,   225,     0,    37,    38,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,     0,    66,     0,    67,     0,    68,     2,     3,     4,
       5,     0,     6,     7,     0,     0,     8,     9,   156,    11,
      12,    13,    14,    15,     0,    16,    17,     0,    18,     0,
       0,     0,     0,    19,    20,    21,    22,    23,    24,    25,
      26,    27,     0,     0,     0,    28,     0,    29,    30,     0,
       0,    31,    32,    33,     0,     0,     0,    34,     0,     0,
      35,    36,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    37,    38,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,     0,    66,     0,    67,
       0,    68,     2,     3,     4,     5,     0,     6,     7,     0,
       0,     8,     9,     0,    11,    12,    13,    14,    15,     0,
      16,    17,     0,    18,     0,     0,     0,     0,    19,    20,
      21,    22,    23,    24,    25,    26,    27,     0,     0,     0,
      28,     0,    29,    30,     0,     0,    31,    32,    33,     0,
       0,     0,    34,     0,     0,    35,    36,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    37,    38,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,     0,    66,     0,    67,     0,    68,     2,     3,     4,
       5,     0,     6,     7,     0,     0,     8,     0,     0,    11,
      12,    13,    14,    15,     0,    16,    17,     0,    18,     0,
       0,     0,     0,    19,    20,    21,    22,    23,    24,    25,
      26,    27,     0,     0,     0,    28,     0,    29,    30,     0,
       0,    31,    32,    33,     0,     0,     0,    34,     0,     0,
      35,    36,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    37,    38,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,     0,    66,     0,    67,
       0,    68,     2,     3,     4,   107,     0,     6,     7,     0,
       0,     8,     0,     0,    11,    12,    13,    14,     0,     0,
      16,    17,     0,    18,     0,     0,     0,     0,     0,    20,
       0,    22,     0,    24,    25,    26,    27,     0,     0,     0,
      28,     0,     0,     0,     0,     0,     0,    32,     0,     0,
       0,     0,    34,     0,     0,    35,    36,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    37,    38,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,     0,    66,     0,    67,   418,    68,     2,     3,     4,
     107,     0,     6,     7,     0,     0,     8,     0,     0,    11,
      12,    13,    14,     0,     0,    16,    17,     0,    18,     0,
       0,     0,     0,     0,    20,     0,    22,     0,    24,    25,
      26,    27,     0,     0,     0,    28,     0,     0,     0,     0,
       0,     0,    32,     0,     0,     0,     0,    34,     0,     0,
      35,    36,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    37,    38,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,     0,    66,     0,    67,
       0,    68,     2,     3,     4,   107,     0,     6,     7,     0,
       0,     8,     0,     0,    11,    12,    13,    14,     0,     0,
      16,    17,     0,     0,     0,     0,     0,     0,     0,    20,
       0,    22,     0,    24,    25,    26,    27,     0,     0,     0,
      28,     0,     0,     0,     0,   -73,     0,    32,     0,     0,
       0,     0,    34,     0,     0,    35,    36,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    37,    38,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,     0,    66,     0,    67,     0,    68,     2,     3,     4,
     107,     0,     6,     7,     0,     0,     8,     0,     0,    11,
      12,    13,    14,     0,     0,    16,    17,     0,     0,     0,
       0,     0,     0,     0,    20,     0,    22,     0,    24,    25,
      26,    27,     0,     0,     0,    28,     0,     0,     0,     0,
       0,     0,    32,     0,     0,     0,     0,    34,     0,     0,
      35,    36,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    37,    38,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,     0,    66,     0,    67,
       0,    68,     2,     3,     4,   107,     0,     6,     7,     0,
       0,     8,     0,     0,    11,    12,    13,    14,     0,     0,
       0,    17,     0,     0,     0,     0,     0,     0,     0,    20,
       0,    22,     0,    24,    25,    26,     0,     0,     0,     0,
      28,     0,     0,     0,     0,     0,     0,    32,     0,     0,
       0,     0,    34,     0,     0,    35,    36,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    37,    38,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,     0,    66,     0,    67,     0,    68,     2,     3,     4,
     107,     0,     6,     7,     0,     0,     8,     0,     0,     0,
       0,    13,    14,     0,     0,     0,    17,     0,     0,     0,
       0,     0,     0,     0,    20,     0,    22,     0,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     226,     0,    32,     0,     0,     0,     0,    34,     0,     0,
      35,    36,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    37,    38,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,     0,    66,     0,    67,
       0,    68,     2,     3,     4,   107,     0,     6,     7,     0,
       0,     8,     0,     0,     0,     0,    13,    14,     0,     0,
       0,    17,     0,     0,     0,     0,     0,     0,     0,    20,
       0,    22,     0,    24,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   228,     0,    32,     0,     0,
       0,     0,    34,     0,     0,    35,    36,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    37,    38,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,     0,    66,     0,    67,     0,    68,     2,     3,     4,
     107,     0,     6,     7,     0,     0,     8,     0,     0,     0,
       0,    13,    14,     0,     0,     0,    17,     0,     0,     0,
       0,     0,     0,     0,    20,     0,    22,     0,    24,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    32,     0,     0,     0,     0,    34,     0,     0,
      35,    36,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    37,    38,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,     0,    66,     0,    67,
       0,    68,     2,     3,     4,   107,     0,     6,     7,     0,
       0,     0,     0,     0,     0,     0,    13,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    22,     0,    24,     0,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    34,     0,     0,    35,    36,   137,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   108,   109,     0,     0,     0,     0,     0,
     110,     0,     0,     0,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,   111,    58,    59,    60,    61,    62,    63,    64,
      65,     0,    66,     0,    67,     0,    68,     2,     3,     4,
     107,     0,     6,     7,     0,     0,     0,     0,     0,     0,
       0,    13,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    22,     0,    24,     0,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    34,     0,     0,
      35,    36,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   108,   109,
       0,     0,     0,     0,     0,   110,     0,     0,     0,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,   111,    58,    59,
      60,    61,    62,    63,    64,    65,     0,    66,     0,    67,
     149,    68,     2,     3,     4,   107,     0,     6,     7,     0,
       0,     0,     0,     0,     0,     0,    13,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    22,     0,    24,     0,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    34,     0,     0,    35,    36,     0,   386,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   108,   109,     0,     0,     0,     0,     0,
     110,     0,     0,     0,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,   111,    58,    59,    60,    61,    62,    63,    64,
      65,     0,    66,     0,    67,     0,    68,     2,     3,     4,
     107,     0,     6,     7,     0,     0,     0,     0,     0,     0,
       0,    13,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    22,     0,    24,     0,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    34,     0,     0,
      35,    36,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   108,   109,
       0,     0,     0,     0,     0,   110,     0,     0,     0,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,   111,    58,    59,
      60,    61,    62,    63,    64,    65,     0,    66,     0,    67,
       0,    68,     2,     3,     4,   107,     0,     6,     7,     0,
       0,     0,     0,     0,     0,     0,    13,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    22,     0,    24,     0,    26,     2,     3,     4,   107,
       0,     6,     7,     0,     0,     0,     0,     0,     0,     0,
      13,     0,    34,     0,     0,    35,    36,     0,     0,     0,
       0,     0,     0,     0,     0,    22,     0,    24,     0,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    37,    38,     0,    34,     0,     0,    35,
      36,     0,     0,     0,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,   197,    66,     0,    67,     0,    68,     0,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,     0,    58,    59,    60,
      61,    62,    63,    64,    65,   197,    66,     0,    67,     0,
      68,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,     0,   224,   225,     0,     0,
       0,     0,     0,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   197,
     224,   225,     0,     0,     0,   254,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   197,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   263,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,     0,   224,   225,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   197,   224,   225,     0,     0,     0,     0,     0,
     395,   396,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   197,     0,     0,     0,     0,     0,     0,     0,     0,
     388,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,     0,   224,   225,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   197,   224,   225,     0,     0,
       0,     0,     0,     0,   400,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   197,     0,     0,     0,     0,     0,
       0,     0,   264,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,     0,
     224,   225,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   197,   224,
     225,     0,     0,     0,     0,     0,   455,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   197,     0,     0,
       0,     0,   335,     0,     0,   457,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,     0,   224,   225,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   197,   224,   225,     0,     0,     0,     0,     0,   458,
       0,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   459,   363,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   197,   224,   225,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   197,     0,     0,     0,     0,     0,
       0,     0,   460,     0,     0,     0,     0,     0,   237,     0,
       0,     0,     0,     0,     0,   467,     0,     0,     0,     0,
       0,     0,     0,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   197,
     224,   225,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   197,   224,
     225,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   468,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,     0,   224,   225,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,     0,   224,   225,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
       0,   224,   225
};

static const yytype_int16 yycheck[] =
{
      18,    35,    21,    28,    22,    84,   128,   127,   127,   233,
     142,    29,    10,    11,   131,    37,    38,    35,   135,     7,
       7,   376,    72,     7,   124,   124,   126,    38,    39,   129,
       6,    42,    41,    67,     6,    59,    59,    34,   126,    56,
       8,   129,     7,    26,    27,    11,    12,     7,     6,    67,
      68,     7,   102,   125,    40,    56,     6,    93,     6,   127,
      56,     0,   124,    56,   130,    46,   127,    56,   127,   103,
     127,   105,    75,    93,    75,   130,    58,   129,   126,     6,
      75,    40,    55,    58,   128,    40,   104,     7,   443,    40,
     108,   109,   110,   118,   119,    92,    93,    94,    95,    96,
      97,   325,   121,   100,   154,    93,   129,   131,   127,    93,
     230,   466,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,   127,    92,    93,    75,
     148,   129,    92,    93,   127,    58,    92,    93,   129,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   130,   125,     6,   189,   380,     7,    93,   383,
     239,   131,   131,   181,   127,   244,    86,    87,    88,    89,
     127,   250,    92,    93,    58,   193,    93,   129,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   194,    40,   128,
     128,   230,   127,     7,     6,   265,    75,     6,    58,   237,
      77,    75,    55,   129,    77,   129,   260,   126,   246,   247,
      77,     6,   128,   131,   252,     6,    86,    87,    88,    89,
     154,     7,    92,    93,   269,     9,   393,   364,   405,   267,
     194,   371,   371,   385,   426,   263,   371,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   264,   286,    -1,
      -1,    -1,    -1,   393,    -1,    -1,    -1,     7,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   405,   408,    -1,    -1,    83,
      84,    85,    86,    87,    88,    89,   321,   322,    92,    93,
      -1,    -1,    -1,    -1,     0,    -1,    -1,    -1,    -1,    -1,
      -1,   329,   331,     9,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    21,    92,    93,    -1,    -1,
      -1,    -1,    28,    -1,   413,    -1,    -1,    -1,    -1,    -1,
      -1,    37,    38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     384,    -1,   371,    83,    84,    85,    86,    87,    88,    89,
      -1,     7,    92,    93,    -1,    -1,    -1,    -1,    -1,    -1,
     388,    -1,    -1,    -1,   393,     7,    72,   456,   396,    -1,
     398,    -1,   400,    -1,    -1,    -1,   405,    -1,    -1,    -1,
      -1,    -1,   410,    -1,    -1,    -1,    -1,   415,    -1,   417,
      -1,    -1,    -1,   421,    -1,    -1,   102,   425,    -1,    -1,
     428,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     454,    -1,   118,   119,    -1,   121,   444,   445,    -1,    -1,
     448,   127,   128,    -1,    -1,    -1,   132,    83,    84,    85,
      86,    87,    88,    89,    -1,    -1,    92,    93,    -1,    -1,
     468,    83,    84,    85,    86,    87,    88,    89,   154,    -1,
      92,    93,    -1,    -1,    -1,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,    -1,
      -1,    -1,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    17,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    32,    -1,    34,    -1,    36,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   225,
      -1,    -1,    53,    -1,   230,    56,    57,    -1,    59,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   249,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    85,    -1,    -1,    -1,    -1,   265,
      91,    -1,    -1,   269,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,    -1,   123,    -1,   125,    -1,   127,    -1,    -1,    -1,
     131,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   321,   322,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   331,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    32,    -1,    34,    -1,    36,
      -1,    -1,    -1,    -1,    -1,   371,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    53,    -1,    -1,    56,
      57,    -1,    -1,    -1,    -1,    -1,    -1,   393,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   405,
      -1,    -1,   408,    -1,    -1,    -1,    -1,    84,    85,    -1,
      -1,    -1,    -1,    -1,    91,    -1,    -1,    -1,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,    -1,   123,    -1,   125,     7,
     127,    -1,     0,     1,   131,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    12,    13,    14,    15,    16,    17,
      18,    19,    -1,    21,    22,    -1,    24,    -1,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      -1,    -1,    -1,    41,    -1,    43,    44,    -1,    46,    47,
      48,    49,    -1,    -1,    -1,    53,    -1,    -1,    56,    57,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    -1,    92,    93,    84,    85,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,     7,   123,    -1,   125,     1,   127,
       3,     4,     5,     6,    -1,     8,     9,    -1,    -1,    12,
      -1,    -1,    15,    16,    17,    18,    -1,    -1,    21,    22,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    30,    -1,    32,
      -1,    34,    35,    36,    37,    -1,    -1,    -1,    41,    -1,
      -1,    -1,    -1,    46,    -1,    48,    -1,    -1,    -1,    -1,
      53,    -1,    -1,    56,    57,    -1,    -1,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    -1,    92,
      93,    84,    85,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,     7,
     123,    -1,   125,    -1,   127,   128,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    32,    -1,    34,    -1,    36,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    53,    -1,    -1,    56,
      57,    -1,    -1,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    -1,    92,    93,    -1,    84,    85,    -1,
      -1,    -1,    -1,    -1,    91,    -1,    -1,    -1,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,     7,   123,    -1,   125,    -1,
     127,   128,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    12,    13,    14,    15,    16,    17,    18,    19,    -1,
      21,    22,    -1,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    -1,    -1,    -1,
      41,    -1,    43,    44,    -1,    46,    47,    48,    49,    -1,
      -1,    -1,    53,    -1,    -1,    56,    57,    -1,    -1,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    -1,
      92,    93,    -1,    84,    85,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,    -1,   123,    -1,   125,    -1,   127,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    12,    13,    14,    15,
      16,    17,    18,    19,    -1,    21,    22,    -1,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    -1,    -1,    -1,    41,    -1,    43,    44,    -1,
      -1,    47,    48,    49,    -1,    -1,    -1,    53,    -1,    -1,
      56,    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,    -1,   123,    -1,   125,
      -1,   127,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    12,    13,    -1,    15,    16,    17,    18,    19,    -1,
      21,    22,    -1,    24,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    -1,    -1,    -1,
      41,    -1,    43,    44,    -1,    -1,    47,    48,    49,    -1,
      -1,    -1,    53,    -1,    -1,    56,    57,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    85,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,    -1,   123,    -1,   125,    -1,   127,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    12,    -1,    -1,    15,
      16,    17,    18,    19,    -1,    21,    22,    -1,    24,    -1,
      -1,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    -1,    -1,    -1,    41,    -1,    43,    44,    -1,
      -1,    47,    48,    49,    -1,    -1,    -1,    53,    -1,    -1,
      56,    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,    -1,   123,    -1,   125,
      -1,   127,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    12,    -1,    -1,    15,    16,    17,    18,    -1,    -1,
      21,    22,    -1,    24,    -1,    -1,    -1,    -1,    -1,    30,
      -1,    32,    -1,    34,    35,    36,    37,    -1,    -1,    -1,
      41,    -1,    -1,    -1,    -1,    -1,    -1,    48,    -1,    -1,
      -1,    -1,    53,    -1,    -1,    56,    57,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    85,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,    -1,   123,    -1,   125,   126,   127,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    12,    -1,    -1,    15,
      16,    17,    18,    -1,    -1,    21,    22,    -1,    24,    -1,
      -1,    -1,    -1,    -1,    30,    -1,    32,    -1,    34,    35,
      36,    37,    -1,    -1,    -1,    41,    -1,    -1,    -1,    -1,
      -1,    -1,    48,    -1,    -1,    -1,    -1,    53,    -1,    -1,
      56,    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,    -1,   123,    -1,   125,
      -1,   127,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    12,    -1,    -1,    15,    16,    17,    18,    -1,    -1,
      21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    30,
      -1,    32,    -1,    34,    35,    36,    37,    -1,    -1,    -1,
      41,    -1,    -1,    -1,    -1,    46,    -1,    48,    -1,    -1,
      -1,    -1,    53,    -1,    -1,    56,    57,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    85,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,    -1,   123,    -1,   125,    -1,   127,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    12,    -1,    -1,    15,
      16,    17,    18,    -1,    -1,    21,    22,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    30,    -1,    32,    -1,    34,    35,
      36,    37,    -1,    -1,    -1,    41,    -1,    -1,    -1,    -1,
      -1,    -1,    48,    -1,    -1,    -1,    -1,    53,    -1,    -1,
      56,    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,    -1,   123,    -1,   125,
      -1,   127,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    12,    -1,    -1,    15,    16,    17,    18,    -1,    -1,
      -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    30,
      -1,    32,    -1,    34,    35,    36,    -1,    -1,    -1,    -1,
      41,    -1,    -1,    -1,    -1,    -1,    -1,    48,    -1,    -1,
      -1,    -1,    53,    -1,    -1,    56,    57,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    85,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,    -1,   123,    -1,   125,    -1,   127,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    12,    -1,    -1,    -1,
      -1,    17,    18,    -1,    -1,    -1,    22,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    30,    -1,    32,    -1,    34,    35,
      36,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      46,    -1,    48,    -1,    -1,    -1,    -1,    53,    -1,    -1,
      56,    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,    -1,   123,    -1,   125,
      -1,   127,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    12,    -1,    -1,    -1,    -1,    17,    18,    -1,    -1,
      -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    30,
      -1,    32,    -1,    34,    35,    36,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    -1,    48,    -1,    -1,
      -1,    -1,    53,    -1,    -1,    56,    57,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    85,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,    -1,   123,    -1,   125,    -1,   127,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    12,    -1,    -1,    -1,
      -1,    17,    18,    -1,    -1,    -1,    22,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    30,    -1,    32,    -1,    34,    35,
      36,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    48,    -1,    -1,    -1,    -1,    53,    -1,    -1,
      56,    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,    -1,   123,    -1,   125,
      -1,   127,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    17,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    32,    -1,    34,    -1,    36,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    53,    -1,    -1,    56,    57,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    85,    -1,    -1,    -1,    -1,    -1,
      91,    -1,    -1,    -1,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,    -1,   123,    -1,   125,    -1,   127,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    32,    -1,    34,    -1,
      36,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,    -1,    -1,
      56,    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,
      -1,    -1,    -1,    -1,    -1,    91,    -1,    -1,    -1,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,    -1,   123,    -1,   125,
     126,   127,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    17,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    32,    -1,    34,    -1,    36,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    53,    -1,    -1,    56,    57,    -1,    59,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    85,    -1,    -1,    -1,    -1,    -1,
      91,    -1,    -1,    -1,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,    -1,   123,    -1,   125,    -1,   127,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    32,    -1,    34,    -1,
      36,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,    -1,    -1,
      56,    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,
      -1,    -1,    -1,    -1,    -1,    91,    -1,    -1,    -1,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,    -1,   123,    -1,   125,
      -1,   127,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    17,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    32,    -1,    34,    -1,    36,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      17,    -1,    53,    -1,    -1,    56,    57,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    32,    -1,    34,    -1,    36,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    85,    -1,    53,    -1,    -1,    56,
      57,    -1,    -1,    -1,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,     7,   123,    -1,   125,    -1,   127,    -1,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,    -1,   114,   115,   116,
     117,   118,   119,   120,   121,     7,   123,    -1,   125,    -1,
     127,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    -1,    92,    93,    -1,    -1,
      -1,    -1,    -1,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,     7,
      92,    93,    -1,    -1,    -1,   131,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   131,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    -1,    92,    93,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,     7,    92,    93,    -1,    -1,    -1,    -1,    -1,
     128,   129,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     129,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    -1,    92,    93,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,     7,    92,    93,    -1,    -1,
      -1,    -1,    -1,    -1,   129,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   128,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    -1,
      92,    93,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,     7,    92,
      93,    -1,    -1,    -1,    -1,    -1,   128,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,    -1,
      -1,    -1,     6,    -1,    -1,   128,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    -1,    92,    93,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,     7,    92,    93,    -1,    -1,    -1,    -1,    -1,   128,
      -1,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   128,   123,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,     7,    92,    93,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   128,    -1,    -1,    -1,    -1,    -1,    50,    -1,
      -1,    -1,    -1,    -1,    -1,    28,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,     7,
      92,    93,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,     7,    92,
      93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     7,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    -1,    92,    93,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    -1,    92,    93,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      -1,    92,    93
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     4,     5,     6,     8,     9,    12,    13,
      14,    15,    16,    17,    18,    19,    21,    22,    24,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    41,    43,
      44,    47,    48,    49,    53,    56,    57,    84,    85,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   123,   125,   127,   133,
     134,   135,   136,   138,   139,   140,   141,   143,   144,   145,
     146,   147,   148,   149,   156,   159,   160,   161,   162,   165,
     167,   168,   171,   172,   179,   180,   183,   184,   186,   187,
     188,   190,   124,    56,    75,   127,   170,     6,    84,    85,
      91,   113,   164,   166,   167,   169,   139,    41,   161,   161,
       6,    56,   166,     6,   159,   166,     8,   125,    40,   162,
     166,     6,    93,   194,   195,     6,   170,    58,   163,   164,
     166,    59,   131,   163,   181,   182,   165,   165,   127,   126,
     163,   166,   166,     0,   124,   137,    14,   138,   194,   130,
      46,     7,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    92,    93,   170,   170,   170,   170,   170,
     170,   127,   170,   138,   163,   166,   128,   163,   189,    56,
     166,   166,   166,   127,    10,    11,   129,     7,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    92,    93,    46,   162,    46,   162,
     127,   151,   157,   158,   159,    75,   130,    50,   144,   156,
     159,   191,   193,   160,   151,   167,    38,    39,    42,    93,
     151,    58,   129,   154,   131,   131,   182,    59,   129,    59,
     131,   166,   126,   131,   128,   136,   138,    75,     6,    40,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   165,   165,   167,   166,    58,    55,   128,   163,   166,
     169,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     167,    40,    40,     1,   152,   153,   155,   156,   159,    75,
      58,   129,   154,     6,   125,     6,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   123,   196,   166,   194,   130,   124,   126,
     129,   192,   194,   166,   166,   167,   194,     6,   173,   174,
     175,   176,   177,   178,   131,   131,    59,   163,   129,   173,
     170,   138,   166,   127,   162,   128,   129,   166,   127,    58,
     129,   162,   162,   128,   128,   129,   154,   166,    40,   159,
     127,     6,   142,   151,   185,    75,     6,    77,   126,   144,
     193,    75,   150,   150,    58,    55,   129,   154,    77,   129,
     154,   182,   166,   126,   152,   166,   166,   166,   155,   160,
     166,   126,   129,   194,    26,    27,   166,   166,    77,   166,
     166,   176,   166,     6,   131,   128,   128,   128,   128,   128,
     128,     6,   150,   166,   166,   166,   194,    28,    50,   150,
     166
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
        if ((yyvsp[(1) - (2)].vardeclexpr))
          (yyval.item) = new VarDeclI((yyloc),(yyvsp[(1) - (2)].vardeclexpr));
        delete (yyvsp[(2) - (2)].expression_v);
      ;}
    break;

  case 27:

    { if ((yyvsp[(1) - (4)].vardeclexpr)) (yyvsp[(1) - (4)].vardeclexpr)->e((yyvsp[(4) - (4)].expression));
        if ((yyvsp[(1) - (4)].vardeclexpr) && (yyvsp[(2) - (4)].expression_v)) (yyvsp[(1) - (4)].vardeclexpr)->addAnnotations(*(yyvsp[(2) - (4)].expression_v));
        if ((yyvsp[(1) - (4)].vardeclexpr))
          (yyval.item) = new VarDeclI((yyloc),(yyvsp[(1) - (4)].vardeclexpr));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 28:

    {
        TypeInst* ti = new TypeInst((yyloc),Type::parsetint());
        ti->setIsEnum(true);
        VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[(2) - (2)].sValue));
        free((yyvsp[(2) - (2)].sValue));
        (yyval.item) = new VarDeclI((yyloc),vd);
      ;}
    break;

  case 29:

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

  case 30:

    {
        TypeInst* ti = new TypeInst((yyloc),Type::parsetint());
        ti->setIsEnum(true);
        vector<Expression*> args;
        args.push_back((yyvsp[(6) - (7)].expression));
        Call* sl = new Call((yyloc), ASTString((yyvsp[(4) - (7)].sValue)), args);
        VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[(2) - (7)].sValue),sl);
        free((yyvsp[(2) - (7)].sValue));
        (yyval.item) = new VarDeclI((yyloc),vd);
      ;}
    break;

  case 31:

    { (yyval.expression_v) = new std::vector<Expression*>(); ;}
    break;

  case 32:

    { (yyval.expression_v) = new std::vector<Expression*>();
        (yyval.expression_v)->push_back(new Id((yyloc),(yyvsp[(1) - (1)].sValue),NULL)); free((yyvsp[(1) - (1)].sValue));
      ;}
    break;

  case 33:

    { (yyval.expression_v) = (yyvsp[(1) - (3)].expression_v); if ((yyval.expression_v)) (yyval.expression_v)->push_back(new Id((yyloc),(yyvsp[(3) - (3)].sValue),NULL)); free((yyvsp[(3) - (3)].sValue)); ;}
    break;

  case 34:

    { (yyval.item) = new AssignI((yyloc),(yyvsp[(1) - (3)].sValue),(yyvsp[(3) - (3)].expression));
        free((yyvsp[(1) - (3)].sValue));
      ;}
    break;

  case 35:

    { (yyval.item) = new ConstraintI((yyloc),(yyvsp[(2) - (2)].expression));;}
    break;

  case 36:

    { (yyval.item) = SolveI::sat((yyloc));
        if ((yyval.item) && (yyvsp[(2) - (3)].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[(2) - (3)].expression_v));
        delete (yyvsp[(2) - (3)].expression_v);
      ;}
    break;

  case 37:

    { (yyval.item) = SolveI::min((yyloc),(yyvsp[(4) - (4)].expression));
        if ((yyval.item) && (yyvsp[(2) - (4)].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[(2) - (4)].expression_v));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 38:

    { (yyval.item) = SolveI::max((yyloc),(yyvsp[(4) - (4)].expression));
        if ((yyval.item) && (yyvsp[(2) - (4)].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[(2) - (4)].expression_v));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 39:

    { (yyval.item) = new OutputI((yyloc),(yyvsp[(2) - (2)].expression));;}
    break;

  case 40:

    { if ((yyvsp[(3) - (5)].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[(2) - (5)].sValue),new TypeInst((yyloc),
                                   Type::varbool()),*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression));
        if ((yyval.item) && (yyvsp[(4) - (5)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(4) - (5)].expression_v));
        free((yyvsp[(2) - (5)].sValue));
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
        delete (yyvsp[(4) - (5)].expression_v);
      ;}
    break;

  case 41:

    { if ((yyvsp[(3) - (5)].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[(2) - (5)].sValue),new TypeInst((yyloc),
                                   Type::parbool()),*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression));
        if ((yyval.item) && (yyvsp[(4) - (5)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(4) - (5)].expression_v));
        free((yyvsp[(2) - (5)].sValue));
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
        delete (yyvsp[(4) - (5)].expression_v);
      ;}
    break;

  case 42:

    { if ((yyvsp[(5) - (7)].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[(4) - (7)].sValue),(yyvsp[(2) - (7)].tiexpr),*(yyvsp[(5) - (7)].vardeclexpr_v),(yyvsp[(7) - (7)].expression));
        if ((yyval.item) && (yyvsp[(6) - (7)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(6) - (7)].expression_v));
        free((yyvsp[(4) - (7)].sValue));
        delete (yyvsp[(5) - (7)].vardeclexpr_v);
        delete (yyvsp[(6) - (7)].expression_v);
      ;}
    break;

  case 43:

    { if ((yyvsp[(5) - (8)].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[(3) - (8)].sValue),(yyvsp[(1) - (8)].tiexpr),*(yyvsp[(5) - (8)].vardeclexpr_v),(yyvsp[(8) - (8)].expression));
        if ((yyval.item) && (yyvsp[(7) - (8)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(7) - (8)].expression_v));
        free((yyvsp[(3) - (8)].sValue));
        delete (yyvsp[(5) - (8)].vardeclexpr_v);
        delete (yyvsp[(7) - (8)].expression_v);
      ;}
    break;

  case 44:

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

  case 45:

    { TypeInst* ti=new TypeInst((yylsp[(1) - (5)]),Type::ann());
        if ((yyvsp[(3) - (5)].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[(2) - (5)].sValue),ti,*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression));
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
      ;}
    break;

  case 46:

    { (yyval.expression)=NULL; ;}
    break;

  case 47:

    { (yyval.expression)=(yyvsp[(2) - (2)].expression); ;}
    break;

  case 48:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); ;}
    break;

  case 49:

    { (yyval.vardeclexpr_v)=(yyvsp[(2) - (3)].vardeclexpr_v); ;}
    break;

  case 50:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); ;}
    break;

  case 51:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); ;}
    break;

  case 52:

    { (yyval.vardeclexpr_v)=(yyvsp[(1) - (2)].vardeclexpr_v); ;}
    break;

  case 53:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>();
        if ((yyvsp[(1) - (1)].vardeclexpr)) (yyvsp[(1) - (1)].vardeclexpr)->toplevel(false);
        if ((yyvsp[(1) - (1)].vardeclexpr)) (yyval.vardeclexpr_v)->push_back((yyvsp[(1) - (1)].vardeclexpr)); ;}
    break;

  case 54:

    { (yyval.vardeclexpr_v)=(yyvsp[(1) - (3)].vardeclexpr_v);
        if ((yyvsp[(3) - (3)].vardeclexpr)) (yyvsp[(3) - (3)].vardeclexpr)->toplevel(false);
        if ((yyvsp[(1) - (3)].vardeclexpr_v) && (yyvsp[(3) - (3)].vardeclexpr)) (yyvsp[(1) - (3)].vardeclexpr_v)->push_back((yyvsp[(3) - (3)].vardeclexpr)); ;}
    break;

  case 57:

    { (yyval.vardeclexpr)=(yyvsp[(1) - (1)].vardeclexpr); ;}
    break;

  case 58:

    { if ((yyvsp[(1) - (1)].tiexpr)) (yyval.vardeclexpr)=new VarDecl((yyloc), (yyvsp[(1) - (1)].tiexpr), ""); ;}
    break;

  case 59:

    { if ((yyvsp[(1) - (3)].tiexpr) && (yyvsp[(3) - (3)].sValue)) (yyval.vardeclexpr) = new VarDecl((yyloc), (yyvsp[(1) - (3)].tiexpr), (yyvsp[(3) - (3)].sValue));
        free((yyvsp[(3) - (3)].sValue));
      ;}
    break;

  case 60:

    { (yyval.tiexpr_v)=(yyvsp[(1) - (2)].tiexpr_v); ;}
    break;

  case 61:

    { (yyval.tiexpr_v)=new vector<TypeInst*>(); (yyval.tiexpr_v)->push_back((yyvsp[(1) - (1)].tiexpr)); ;}
    break;

  case 62:

    { (yyval.tiexpr_v)=(yyvsp[(1) - (3)].tiexpr_v); if ((yyvsp[(1) - (3)].tiexpr_v) && (yyvsp[(3) - (3)].tiexpr)) (yyvsp[(1) - (3)].tiexpr_v)->push_back((yyvsp[(3) - (3)].tiexpr)); ;}
    break;

  case 64:

    {
        (yyval.tiexpr) = (yyvsp[(6) - (6)].tiexpr);
        if ((yyval.tiexpr) && (yyvsp[(3) - (6)].tiexpr_v)) (yyval.tiexpr)->setRanges(*(yyvsp[(3) - (6)].tiexpr_v));
        delete (yyvsp[(3) - (6)].tiexpr_v);
      ;}
    break;

  case 65:

    {
        (yyval.tiexpr) = (yyvsp[(3) - (3)].tiexpr);
        std::vector<TypeInst*> ti(1);
        ti[0] = new TypeInst((yyloc),Type::parint());
        if ((yyval.tiexpr)) (yyval.tiexpr)->setRanges(ti);
      ;}
    break;

  case 66:

    { (yyval.tiexpr) = (yyvsp[(1) - (1)].tiexpr);
      ;}
    break;

  case 67:

    { (yyval.tiexpr) = (yyvsp[(2) - (2)].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 68:

    { (yyval.tiexpr) = (yyvsp[(3) - (3)].tiexpr);
        if ((yyval.tiexpr) && (yyvsp[(2) - (3)].bValue)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 69:

    { (yyval.tiexpr) = (yyvsp[(3) - (3)].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ti(Type::TI_VAR);
          if ((yyvsp[(2) - (3)].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 70:

    { (yyval.tiexpr) = (yyvsp[(4) - (4)].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.st(Type::ST_SET);
          if ((yyvsp[(1) - (4)].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 71:

    { (yyval.tiexpr) = (yyvsp[(5) - (5)].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.st(Type::ST_SET);
          if ((yyvsp[(2) - (5)].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 72:

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

  case 73:

    { (yyval.bValue) = false; ;}
    break;

  case 74:

    { (yyval.bValue) = true; ;}
    break;

  case 75:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parint()); ;}
    break;

  case 76:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parbool()); ;}
    break;

  case 77:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parfloat()); ;}
    break;

  case 78:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parstring()); ;}
    break;

  case 79:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::ann()); ;}
    break;

  case 80:

    { if ((yyvsp[(1) - (1)].expression)) (yyval.tiexpr) = new TypeInst((yyloc),Type(),(yyvsp[(1) - (1)].expression)); ;}
    break;

  case 81:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::top(),
                         new TIId((yyloc), (yyvsp[(1) - (1)].sValue)));
        free((yyvsp[(1) - (1)].sValue));
      ;}
    break;

  case 83:

    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].expression)); ;}
    break;

  case 84:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); if ((yyval.expression_v) && (yyvsp[(3) - (3)].expression)) (yyval.expression_v)->push_back((yyvsp[(3) - (3)].expression)); ;}
    break;

  case 86:

    { if ((yyvsp[(1) - (3)].expression) && (yyvsp[(3) - (3)].expression)) (yyvsp[(1) - (3)].expression)->addAnnotation((yyvsp[(3) - (3)].expression)); (yyval.expression)=(yyvsp[(1) - (3)].expression); ;}
    break;

  case 87:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_UNION, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 88:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 89:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SYMDIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 90:

    { if ((yyvsp[(1) - (3)].expression)==NULL || (yyvsp[(3) - (3)].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[(1) - (3)].expression)->isa<IntLit>() && (yyvsp[(3) - (3)].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[(1) - (3)].expression)->cast<IntLit>()->v(),(yyvsp[(3) - (3)].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DOTDOT, (yyvsp[(3) - (3)].expression));
        }
      ;}
    break;

  case 91:

    { if ((yyvsp[(3) - (6)].expression)==NULL || (yyvsp[(5) - (6)].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[(3) - (6)].expression)->isa<IntLit>() && (yyvsp[(5) - (6)].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[(3) - (6)].expression)->cast<IntLit>()->v(),(yyvsp[(5) - (6)].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[(3) - (6)].expression), BOT_DOTDOT, (yyvsp[(5) - (6)].expression));
        }
      ;}
    break;

  case 92:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_INTERSECT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 93:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUSPLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 94:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 95:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MINUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 96:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MULT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 97:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 98:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IDIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 99:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MOD, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 100:

    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=new Call((yyloc), (yyvsp[(2) - (3)].sValue), args);
        free((yyvsp[(2) - (3)].sValue));
      ;}
    break;

  case 101:

    { (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 102:

    { if ((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<IntLit>()) {
          (yyval.expression) = IntLit::a(-(yyvsp[(2) - (2)].expression)->cast<IntLit>()->v());
        } else if ((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<FloatLit>()) {
          (yyvsp[(2) - (2)].expression)->cast<FloatLit>()->v(-(yyvsp[(2) - (2)].expression)->cast<FloatLit>()->v());
          (yyval.expression) = (yyvsp[(2) - (2)].expression);
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_MINUS, (yyvsp[(2) - (2)].expression));
        }
      ;}
    break;

  case 104:

    { if ((yyvsp[(1) - (3)].expression) && (yyvsp[(3) - (3)].expression)) (yyvsp[(1) - (3)].expression)->addAnnotation((yyvsp[(3) - (3)].expression)); (yyval.expression)=(yyvsp[(1) - (3)].expression); ;}
    break;

  case 105:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_EQUIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 106:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IMPL, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 107:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_RIMPL, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 108:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_OR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 109:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_XOR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 110:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_AND, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 111:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_LE, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 112:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_GR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 113:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_LQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 114:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_GQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 115:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_EQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 116:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_NQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 117:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IN, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 118:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SUBSET, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 119:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SUPERSET, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 120:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_UNION, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 121:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 122:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SYMDIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 123:

    { if ((yyvsp[(1) - (3)].expression)==NULL || (yyvsp[(3) - (3)].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[(1) - (3)].expression)->isa<IntLit>() && (yyvsp[(3) - (3)].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[(1) - (3)].expression)->cast<IntLit>()->v(),(yyvsp[(3) - (3)].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DOTDOT, (yyvsp[(3) - (3)].expression));
        }
      ;}
    break;

  case 124:

    { if ((yyvsp[(3) - (6)].expression)==NULL || (yyvsp[(5) - (6)].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[(3) - (6)].expression)->isa<IntLit>() && (yyvsp[(5) - (6)].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[(3) - (6)].expression)->cast<IntLit>()->v(),(yyvsp[(5) - (6)].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[(3) - (6)].expression), BOT_DOTDOT, (yyvsp[(5) - (6)].expression));
        }
      ;}
    break;

  case 125:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_INTERSECT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 126:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUSPLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 127:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 128:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MINUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 129:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MULT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 130:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 131:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IDIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 132:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MOD, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 133:

    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=new Call((yyloc), (yyvsp[(2) - (3)].sValue), args);
        free((yyvsp[(2) - (3)].sValue));
      ;}
    break;

  case 134:

    { (yyval.expression)=new UnOp((yyloc), UOT_NOT, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 135:

    { if (((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<IntLit>()) || ((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<FloatLit>())) {
          (yyval.expression) = (yyvsp[(2) - (2)].expression);
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[(2) - (2)].expression));
        }
      ;}
    break;

  case 136:

    { if ((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<IntLit>()) {
          (yyval.expression) = IntLit::a(-(yyvsp[(2) - (2)].expression)->cast<IntLit>()->v());
        } else if ((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<FloatLit>()) {
          (yyvsp[(2) - (2)].expression)->cast<FloatLit>()->v(-(yyvsp[(2) - (2)].expression)->cast<FloatLit>()->v());
          (yyval.expression) = (yyvsp[(2) - (2)].expression);
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_MINUS, (yyvsp[(2) - (2)].expression));
        }
      ;}
    break;

  case 137:

    { (yyval.expression)=(yyvsp[(2) - (3)].expression); ;}
    break;

  case 138:

    { if ((yyvsp[(4) - (4)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(2) - (4)].expression), *(yyvsp[(4) - (4)].expression_vv)); delete (yyvsp[(4) - (4)].expression_vv); ;}
    break;

  case 139:

    { (yyval.expression)=new Id((yyloc), (yyvsp[(1) - (1)].sValue), NULL); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 140:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), new Id((yylsp[(1) - (2)]),(yyvsp[(1) - (2)].sValue),NULL), *(yyvsp[(2) - (2)].expression_vv));
        free((yyvsp[(1) - (2)].sValue)); delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 141:

    { (yyval.expression)=new AnonVar((yyloc)); ;}
    break;

  case 142:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), new AnonVar((yyloc)), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 143:

    { (yyval.expression)=constants().boollit(((yyvsp[(1) - (1)].iValue)!=0)); ;}
    break;

  case 144:

    { (yyval.expression)=IntLit::a((yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 145:

    { (yyval.expression)=IntLit::a(IntVal::infinity()); ;}
    break;

  case 146:

    { (yyval.expression)=new FloatLit((yyloc), (yyvsp[(1) - (1)].dValue)); ;}
    break;

  case 148:

    { (yyval.expression)=constants().absent; ;}
    break;

  case 150:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 152:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 154:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 156:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 158:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 160:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 163:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 164:

    { (yyval.expression)=new StringLit((yyloc), (yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 165:

    { (yyval.expression)=new BinOp((yyloc), new StringLit((yyloc), (yyvsp[(1) - (2)].sValue)), BOT_PLUSPLUS, (yyvsp[(2) - (2)].expression));
        free((yyvsp[(1) - (2)].sValue));
      ;}
    break;

  case 166:

    { if ((yyvsp[(1) - (2)].expression_v)) (yyval.expression)=new BinOp((yyloc), new Call((yyloc), ASTString("format"), *(yyvsp[(1) - (2)].expression_v)), BOT_PLUSPLUS, new StringLit((yyloc),(yyvsp[(2) - (2)].sValue)));
        free((yyvsp[(2) - (2)].sValue));
        delete (yyvsp[(1) - (2)].expression_v);
      ;}
    break;

  case 167:

    { if ((yyvsp[(1) - (3)].expression_v)) (yyval.expression)=new BinOp((yyloc), new Call((yyloc), ASTString("format"), *(yyvsp[(1) - (3)].expression_v)), BOT_PLUSPLUS,
                             new BinOp((yyloc), new StringLit((yyloc),(yyvsp[(2) - (3)].sValue)), BOT_PLUSPLUS, (yyvsp[(3) - (3)].expression)));
        free((yyvsp[(2) - (3)].sValue));
        delete (yyvsp[(1) - (3)].expression_v);
      ;}
    break;

  case 168:

    { (yyval.expression_vv)=new std::vector<std::vector<Expression*> >();
        if ((yyvsp[(2) - (3)].expression_v)) {
          (yyval.expression_vv)->push_back(*(yyvsp[(2) - (3)].expression_v));
          delete (yyvsp[(2) - (3)].expression_v);
        }
      ;}
    break;

  case 169:

    { (yyval.expression_vv)=(yyvsp[(1) - (4)].expression_vv);
        if ((yyval.expression_vv) && (yyvsp[(3) - (4)].expression_v)) {
          (yyval.expression_vv)->push_back(*(yyvsp[(3) - (4)].expression_v));
          delete (yyvsp[(3) - (4)].expression_v);
        }
      ;}
    break;

  case 170:

    { (yyval.expression) = new SetLit((yyloc), std::vector<Expression*>()); ;}
    break;

  case 171:

    { if ((yyvsp[(2) - (3)].expression_v)) (yyval.expression) = new SetLit((yyloc), *(yyvsp[(2) - (3)].expression_v));
        delete (yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 172:

    { if ((yyvsp[(4) - (5)].generators)) (yyval.expression) = new Comprehension((yyloc), (yyvsp[(2) - (5)].expression), *(yyvsp[(4) - (5)].generators), true);
        delete (yyvsp[(4) - (5)].generators);
      ;}
    break;

  case 173:

    { if ((yyvsp[(1) - (1)].generator_v)) (yyval.generators)=new Generators; (yyval.generators)->_g = *(yyvsp[(1) - (1)].generator_v); (yyval.generators)->_w = NULL; delete (yyvsp[(1) - (1)].generator_v); ;}
    break;

  case 174:

    { if ((yyvsp[(1) - (3)].generator_v)) (yyval.generators)=new Generators; (yyval.generators)->_g = *(yyvsp[(1) - (3)].generator_v); (yyval.generators)->_w = (yyvsp[(3) - (3)].expression); delete (yyvsp[(1) - (3)].generator_v); ;}
    break;

  case 176:

    { (yyval.generator_v)=new std::vector<Generator>; if ((yyvsp[(1) - (1)].generator)) (yyval.generator_v)->push_back(*(yyvsp[(1) - (1)].generator)); delete (yyvsp[(1) - (1)].generator); ;}
    break;

  case 177:

    { (yyval.generator_v)=(yyvsp[(1) - (3)].generator_v); if ((yyval.generator_v) && (yyvsp[(3) - (3)].generator)) (yyval.generator_v)->push_back(*(yyvsp[(3) - (3)].generator)); delete (yyvsp[(3) - (3)].generator); ;}
    break;

  case 178:

    { if ((yyvsp[(1) - (3)].string_v) && (yyvsp[(3) - (3)].expression)) (yyval.generator)=new Generator(*(yyvsp[(1) - (3)].string_v),(yyvsp[(3) - (3)].expression)); else (yyval.generator)=NULL; delete (yyvsp[(1) - (3)].string_v); ;}
    break;

  case 180:

    { (yyval.string_v)=new std::vector<std::string>; (yyval.string_v)->push_back((yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 181:

    { (yyval.string_v)=(yyvsp[(1) - (3)].string_v); if ((yyval.string_v) && (yyvsp[(3) - (3)].sValue)) (yyval.string_v)->push_back((yyvsp[(3) - (3)].sValue)); free((yyvsp[(3) - (3)].sValue)); ;}
    break;

  case 182:

    { (yyval.expression)=new ArrayLit((yyloc), std::vector<MiniZinc::Expression*>()); ;}
    break;

  case 183:

    { if ((yyvsp[(2) - (3)].expression_v)) (yyval.expression)=new ArrayLit((yyloc), *(yyvsp[(2) - (3)].expression_v)); delete (yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 184:

    { (yyval.expression)=new ArrayLit((yyloc), std::vector<std::vector<Expression*> >()); ;}
    break;

  case 185:

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

  case 186:

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

  case 187:

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

  case 188:

    { (yyval.expression_vvv)=new std::vector<std::vector<std::vector<MiniZinc::Expression*> > >;
      ;}
    break;

  case 189:

    { (yyval.expression_vvv)=new std::vector<std::vector<std::vector<MiniZinc::Expression*> > >;
        if ((yyvsp[(2) - (3)].expression_vv)) (yyval.expression_vvv)->push_back(*(yyvsp[(2) - (3)].expression_vv));
        delete (yyvsp[(2) - (3)].expression_vv);
      ;}
    break;

  case 190:

    { (yyval.expression_vvv)=(yyvsp[(1) - (5)].expression_vvv);
        if ((yyval.expression_vvv) && (yyvsp[(4) - (5)].expression_vv)) (yyval.expression_vvv)->push_back(*(yyvsp[(4) - (5)].expression_vv));
        delete (yyvsp[(4) - (5)].expression_vv);
      ;}
    break;

  case 191:

    { (yyval.expression_vv)=new std::vector<std::vector<MiniZinc::Expression*> >;
        if ((yyvsp[(1) - (1)].expression_v)) (yyval.expression_vv)->push_back(*(yyvsp[(1) - (1)].expression_v));
        delete (yyvsp[(1) - (1)].expression_v);
      ;}
    break;

  case 192:

    { (yyval.expression_vv)=(yyvsp[(1) - (3)].expression_vv); if ((yyval.expression_vv) && (yyvsp[(3) - (3)].expression_v)) (yyval.expression_vv)->push_back(*(yyvsp[(3) - (3)].expression_v)); delete (yyvsp[(3) - (3)].expression_v); ;}
    break;

  case 193:

    { if ((yyvsp[(4) - (5)].generators)) (yyval.expression)=new Comprehension((yyloc), (yyvsp[(2) - (5)].expression), *(yyvsp[(4) - (5)].generators), false);
        delete (yyvsp[(4) - (5)].generators);
      ;}
    break;

  case 194:

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

  case 195:

    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; ;}
    break;

  case 196:

    { (yyval.expression_v)=(yyvsp[(1) - (5)].expression_v); if ((yyval.expression_v) && (yyvsp[(3) - (5)].expression) && (yyvsp[(5) - (5)].expression)) { (yyval.expression_v)->push_back((yyvsp[(3) - (5)].expression)); (yyval.expression_v)->push_back((yyvsp[(5) - (5)].expression)); } ;}
    break;

  case 197:

    { (yyval.iValue)=BOT_EQUIV; ;}
    break;

  case 198:

    { (yyval.iValue)=BOT_IMPL; ;}
    break;

  case 199:

    { (yyval.iValue)=BOT_RIMPL; ;}
    break;

  case 200:

    { (yyval.iValue)=BOT_OR; ;}
    break;

  case 201:

    { (yyval.iValue)=BOT_XOR; ;}
    break;

  case 202:

    { (yyval.iValue)=BOT_AND; ;}
    break;

  case 203:

    { (yyval.iValue)=BOT_LE; ;}
    break;

  case 204:

    { (yyval.iValue)=BOT_GR; ;}
    break;

  case 205:

    { (yyval.iValue)=BOT_LQ; ;}
    break;

  case 206:

    { (yyval.iValue)=BOT_GQ; ;}
    break;

  case 207:

    { (yyval.iValue)=BOT_EQ; ;}
    break;

  case 208:

    { (yyval.iValue)=BOT_NQ; ;}
    break;

  case 209:

    { (yyval.iValue)=BOT_IN; ;}
    break;

  case 210:

    { (yyval.iValue)=BOT_SUBSET; ;}
    break;

  case 211:

    { (yyval.iValue)=BOT_SUPERSET; ;}
    break;

  case 212:

    { (yyval.iValue)=BOT_UNION; ;}
    break;

  case 213:

    { (yyval.iValue)=BOT_DIFF; ;}
    break;

  case 214:

    { (yyval.iValue)=BOT_SYMDIFF; ;}
    break;

  case 215:

    { (yyval.iValue)=BOT_PLUS; ;}
    break;

  case 216:

    { (yyval.iValue)=BOT_MINUS; ;}
    break;

  case 217:

    { (yyval.iValue)=BOT_MULT; ;}
    break;

  case 218:

    { (yyval.iValue)=BOT_DIV; ;}
    break;

  case 219:

    { (yyval.iValue)=BOT_IDIV; ;}
    break;

  case 220:

    { (yyval.iValue)=BOT_MOD; ;}
    break;

  case 221:

    { (yyval.iValue)=BOT_INTERSECT; ;}
    break;

  case 222:

    { (yyval.iValue)=BOT_PLUSPLUS; ;}
    break;

  case 223:

    { (yyval.iValue)=-1; ;}
    break;

  case 224:

    { if ((yyvsp[(1) - (6)].iValue)==-1) {
          (yyval.expression)=NULL;
          yyerror(&(yylsp[(3) - (6)]), parm, "syntax error, unary operator with two arguments");
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[(3) - (6)].expression),static_cast<BinOpType>((yyvsp[(1) - (6)].iValue)),(yyvsp[(5) - (6)].expression));
        }
      ;}
    break;

  case 225:

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
            (yyvsp[(3) - (4)].expression)->cast<FloatLit>()->v(-(yyvsp[(3) - (4)].expression)->cast<FloatLit>()->v());
            (yyval.expression) = (yyvsp[(3) - (4)].expression);
          } else {
            (yyval.expression)=new UnOp((yyloc), static_cast<UnOpType>(uot),(yyvsp[(3) - (4)].expression));
          }
        }
      ;}
    break;

  case 226:

    { (yyval.expression)=new Call((yyloc), (yyvsp[(1) - (3)].sValue), std::vector<Expression*>()); free((yyvsp[(1) - (3)].sValue)); ;}
    break;

  case 228:

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

  case 229:

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

  case 230:

    { (yyval.expression_p)=new pair<vector<Expression*>,Expression*>;
        if ((yyvsp[(1) - (1)].expression_v)) (yyval.expression_p)->first=*(yyvsp[(1) - (1)].expression_v);
        (yyval.expression_p)->second=NULL;
        delete (yyvsp[(1) - (1)].expression_v);
      ;}
    break;

  case 231:

    { (yyval.expression_p)=new pair<vector<Expression*>,Expression*>;
        if ((yyvsp[(1) - (3)].expression_v)) (yyval.expression_p)->first=*(yyvsp[(1) - (3)].expression_v);
        (yyval.expression_p)->second=(yyvsp[(3) - (3)].expression);
        delete (yyvsp[(1) - (3)].expression_v);
      ;}
    break;

  case 232:

    { if ((yyvsp[(3) - (6)].expression_v) && (yyvsp[(6) - (6)].expression)) {
          (yyval.expression)=new Let((yyloc), *(yyvsp[(3) - (6)].expression_v), (yyvsp[(6) - (6)].expression)); delete (yyvsp[(3) - (6)].expression_v);
        } else {
          (yyval.expression)=NULL;
        }
      ;}
    break;

  case 233:

    { if ((yyvsp[(3) - (7)].expression_v) && (yyvsp[(7) - (7)].expression)) {
          (yyval.expression)=new Let((yyloc), *(yyvsp[(3) - (7)].expression_v), (yyvsp[(7) - (7)].expression)); delete (yyvsp[(3) - (7)].expression_v);
        } else {
          (yyval.expression)=NULL;
        }
      ;}
    break;

  case 234:

    { (yyval.expression_v)=new vector<Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].vardeclexpr)); ;}
    break;

  case 235:

    { (yyval.expression_v)=new vector<Expression*>;
        if ((yyvsp[(1) - (1)].item)) {
          ConstraintI* ce = (yyvsp[(1) - (1)].item)->cast<ConstraintI>();
          (yyval.expression_v)->push_back(ce->e());
          ce->e(NULL);
        }
      ;}
    break;

  case 236:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); if ((yyval.expression_v) && (yyvsp[(3) - (3)].vardeclexpr)) (yyval.expression_v)->push_back((yyvsp[(3) - (3)].vardeclexpr)); ;}
    break;

  case 237:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v);
        if ((yyval.expression_v) && (yyvsp[(3) - (3)].item)) {
          ConstraintI* ce = (yyvsp[(3) - (3)].item)->cast<ConstraintI>();
          (yyval.expression_v)->push_back(ce->e());
          ce->e(NULL);
        }
      ;}
    break;

  case 240:

    { (yyval.vardeclexpr) = (yyvsp[(1) - (2)].vardeclexpr);
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->toplevel(false);
        if ((yyval.vardeclexpr) && (yyvsp[(2) - (2)].expression_v)) (yyval.vardeclexpr)->addAnnotations(*(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v);
      ;}
    break;

  case 241:

    { if ((yyvsp[(1) - (4)].vardeclexpr)) (yyvsp[(1) - (4)].vardeclexpr)->e((yyvsp[(4) - (4)].expression));
        (yyval.vardeclexpr) = (yyvsp[(1) - (4)].vardeclexpr);
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->loc((yyloc));
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->toplevel(false);
        if ((yyval.vardeclexpr) && (yyvsp[(2) - (4)].expression_v)) (yyval.vardeclexpr)->addAnnotations(*(yyvsp[(2) - (4)].expression_v));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 242:

    { (yyval.expression_v)=NULL; ;}
    break;

  case 244:

    { (yyval.expression_v)=new std::vector<Expression*>(1);
        (*(yyval.expression_v))[0] = (yyvsp[(2) - (2)].expression);
      ;}
    break;

  case 245:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); if ((yyval.expression_v)) (yyval.expression_v)->push_back((yyvsp[(3) - (3)].expression)); ;}
    break;

  case 246:

    { (yyval.sValue)=(yyvsp[(1) - (1)].sValue); ;}
    break;

  case 247:

    { (yyval.sValue)=strdup("'<->'"); ;}
    break;

  case 248:

    { (yyval.sValue)=strdup("'->'"); ;}
    break;

  case 249:

    { (yyval.sValue)=strdup("'<-'"); ;}
    break;

  case 250:

    { (yyval.sValue)=strdup("'\\/'"); ;}
    break;

  case 251:

    { (yyval.sValue)=strdup("'xor'"); ;}
    break;

  case 252:

    { (yyval.sValue)=strdup("'/\\'"); ;}
    break;

  case 253:

    { (yyval.sValue)=strdup("'<'"); ;}
    break;

  case 254:

    { (yyval.sValue)=strdup("'>'"); ;}
    break;

  case 255:

    { (yyval.sValue)=strdup("'<='"); ;}
    break;

  case 256:

    { (yyval.sValue)=strdup("'>='"); ;}
    break;

  case 257:

    { (yyval.sValue)=strdup("'='"); ;}
    break;

  case 258:

    { (yyval.sValue)=strdup("'!='"); ;}
    break;

  case 259:

    { (yyval.sValue)=strdup("'in'"); ;}
    break;

  case 260:

    { (yyval.sValue)=strdup("'subset'"); ;}
    break;

  case 261:

    { (yyval.sValue)=strdup("'superset'"); ;}
    break;

  case 262:

    { (yyval.sValue)=strdup("'union'"); ;}
    break;

  case 263:

    { (yyval.sValue)=strdup("'diff'"); ;}
    break;

  case 264:

    { (yyval.sValue)=strdup("'symdiff'"); ;}
    break;

  case 265:

    { (yyval.sValue)=strdup("'..'"); ;}
    break;

  case 266:

    { (yyval.sValue)=strdup("'+'"); ;}
    break;

  case 267:

    { (yyval.sValue)=strdup("'-'"); ;}
    break;

  case 268:

    { (yyval.sValue)=strdup("'*'"); ;}
    break;

  case 269:

    { (yyval.sValue)=strdup("'/'"); ;}
    break;

  case 270:

    { (yyval.sValue)=strdup("'div'"); ;}
    break;

  case 271:

    { (yyval.sValue)=strdup("'mod'"); ;}
    break;

  case 272:

    { (yyval.sValue)=strdup("'intersect'"); ;}
    break;

  case 273:

    { (yyval.sValue)=strdup("'not'"); ;}
    break;

  case 274:

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



