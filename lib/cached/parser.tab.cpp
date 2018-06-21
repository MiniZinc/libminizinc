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
     MZN_NOT = 348,
     MZN_PLUSPLUS = 349,
     MZN_COLONCOLON = 350,
     PREC_ANNO = 351,
     MZN_EQUIV_QUOTED = 352,
     MZN_IMPL_QUOTED = 353,
     MZN_RIMPL_QUOTED = 354,
     MZN_OR_QUOTED = 355,
     MZN_XOR_QUOTED = 356,
     MZN_AND_QUOTED = 357,
     MZN_LE_QUOTED = 358,
     MZN_GR_QUOTED = 359,
     MZN_LQ_QUOTED = 360,
     MZN_GQ_QUOTED = 361,
     MZN_EQ_QUOTED = 362,
     MZN_NQ_QUOTED = 363,
     MZN_IN_QUOTED = 364,
     MZN_SUBSET_QUOTED = 365,
     MZN_SUPERSET_QUOTED = 366,
     MZN_UNION_QUOTED = 367,
     MZN_DIFF_QUOTED = 368,
     MZN_SYMDIFF_QUOTED = 369,
     MZN_DOTDOT_QUOTED = 370,
     MZN_PLUS_QUOTED = 371,
     MZN_MINUS_QUOTED = 372,
     MZN_MULT_QUOTED = 373,
     MZN_DIV_QUOTED = 374,
     MZN_IDIV_QUOTED = 375,
     MZN_MOD_QUOTED = 376,
     MZN_INTERSECT_QUOTED = 377,
     MZN_NOT_QUOTED = 378,
     MZN_COLONCOLON_QUOTED = 379,
     MZN_PLUSPLUS_QUOTED = 380
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
#define MZN_IN 334
#define MZN_SUBSET 335
#define MZN_SUPERSET 336
#define MZN_UNION 337
#define MZN_DIFF 338
#define MZN_SYMDIFF 339
#define MZN_DOTDOT 340
#define MZN_PLUS 341
#define MZN_MINUS 342
#define MZN_MULT 343
#define MZN_DIV 344
#define MZN_IDIV 345
#define MZN_MOD 346
#define MZN_INTERSECT 347
#define MZN_NOT 348
#define MZN_PLUSPLUS 349
#define MZN_COLONCOLON 350
#define PREC_ANNO 351
#define MZN_EQUIV_QUOTED 352
#define MZN_IMPL_QUOTED 353
#define MZN_RIMPL_QUOTED 354
#define MZN_OR_QUOTED 355
#define MZN_XOR_QUOTED 356
#define MZN_AND_QUOTED 357
#define MZN_LE_QUOTED 358
#define MZN_GR_QUOTED 359
#define MZN_LQ_QUOTED 360
#define MZN_GQ_QUOTED 361
#define MZN_EQ_QUOTED 362
#define MZN_NQ_QUOTED 363
#define MZN_IN_QUOTED 364
#define MZN_SUBSET_QUOTED 365
#define MZN_SUPERSET_QUOTED 366
#define MZN_UNION_QUOTED 367
#define MZN_DIFF_QUOTED 368
#define MZN_SYMDIFF_QUOTED 369
#define MZN_DOTDOT_QUOTED 370
#define MZN_PLUS_QUOTED 371
#define MZN_MINUS_QUOTED 372
#define MZN_MULT_QUOTED 373
#define MZN_DIV_QUOTED 374
#define MZN_IDIV_QUOTED 375
#define MZN_MOD_QUOTED 376
#define MZN_INTERSECT_QUOTED 377
#define MZN_NOT_QUOTED 378
#define MZN_COLONCOLON_QUOTED 379
#define MZN_PLUSPLUS_QUOTED 380




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
#define YYLAST   4827

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  134
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  69
/* YYNRULES -- Number of rules.  */
#define YYNRULES  299
/* YYNRULES -- Number of states.  */
#define YYNSTATES  501

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   380

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     129,   130,     2,     2,   131,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   132,   126,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   127,   133,   128,     2,     2,     2,     2,
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
     125
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    11,    14,    18,    23,
      26,    30,    32,    35,    36,    38,    41,    43,    45,    47,
      49,    51,    53,    55,    57,    59,    61,    63,    65,    67,
      69,    71,    73,    75,    77,    79,    81,    83,    86,    89,
      94,    97,   104,   112,   113,   115,   119,   123,   126,   131,
     135,   140,   145,   148,   154,   160,   168,   177,   181,   187,
     188,   191,   192,   196,   200,   201,   204,   206,   210,   211,
     213,   215,   217,   221,   224,   226,   230,   232,   239,   243,
     245,   248,   252,   256,   261,   267,   273,   274,   276,   278,
     280,   282,   284,   286,   288,   290,   292,   295,   297,   301,
     303,   307,   311,   315,   319,   323,   330,   334,   338,   342,
     346,   350,   354,   358,   362,   366,   369,   372,   374,   378,
     382,   386,   390,   394,   398,   402,   406,   410,   414,   418,
     422,   426,   430,   434,   438,   442,   446,   450,   452,   455,
     458,   462,   469,   473,   477,   481,   485,   489,   493,   497,
     501,   505,   508,   511,   514,   516,   518,   522,   527,   529,
     532,   534,   537,   539,   541,   543,   545,   547,   549,   552,
     554,   557,   559,   562,   564,   567,   569,   572,   574,   577,
     579,   581,   584,   586,   589,   592,   596,   600,   605,   608,
     612,   618,   620,   623,   625,   629,   633,   639,   643,   646,
     648,   652,   655,   659,   662,   666,   671,   675,   678,   682,
     688,   690,   694,   700,   706,   715,   716,   722,   724,   726,
     728,   730,   732,   734,   736,   738,   740,   742,   744,   746,
     748,   750,   752,   754,   756,   758,   760,   762,   764,   766,
     768,   770,   772,   774,   776,   783,   788,   792,   794,   799,
     807,   810,   812,   816,   820,   826,   833,   841,   843,   845,
     849,   853,   855,   857,   860,   865,   866,   868,   870,   872,
     875,   879,   881,   883,   885,   887,   889,   891,   893,   895,
     897,   899,   901,   903,   905,   907,   909,   911,   913,   915,
     917,   919,   921,   923,   925,   927,   929,   931,   933,   935
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     135,     0,    -1,   136,    -1,    -1,   137,   139,    -1,   140,
      -1,   138,   140,    -1,   137,   126,   140,    -1,   137,   126,
     138,   140,    -1,   140,   142,    -1,     1,   126,   140,    -1,
      15,    -1,   138,    15,    -1,    -1,   126,    -1,    14,   141,
      -1,   141,    -1,   143,    -1,   144,    -1,   146,    -1,   147,
      -1,   148,    -1,   149,    -1,   150,    -1,   151,    -1,   152,
      -1,    34,    -1,    30,    -1,     6,    -1,    44,    -1,    25,
      -1,    48,    -1,    45,    -1,    32,    -1,    50,    -1,    20,
      -1,   159,    -1,    34,     8,    -1,   159,   199,    -1,   159,
     199,    77,   169,    -1,    30,     6,    -1,    30,     6,    77,
     127,   145,   128,    -1,    30,     6,    77,     6,   129,   169,
     130,    -1,    -1,     6,    -1,   145,   131,     6,    -1,     6,
      77,   169,    -1,    25,   169,    -1,    25,    95,   172,   169,
      -1,    48,   199,    43,    -1,    48,   199,    40,   169,    -1,
      48,   199,    39,   169,    -1,    44,   169,    -1,    45,     6,
     154,   199,   153,    -1,    50,     6,   154,   199,   153,    -1,
      32,   162,   132,   202,   154,   199,   153,    -1,   162,   132,
       6,   129,   155,   130,   199,   153,    -1,    20,     6,   154,
      -1,    20,     6,   154,    77,   169,    -1,    -1,    77,   169,
      -1,    -1,   129,   155,   130,    -1,   129,     1,   130,    -1,
      -1,   156,   157,    -1,   158,    -1,   156,   131,   158,    -1,
      -1,   131,    -1,   159,    -1,   162,    -1,   162,   132,     6,
      -1,   161,   157,    -1,   162,    -1,   161,   131,   162,    -1,
     163,    -1,    22,    57,   160,    59,    41,   163,    -1,    38,
      41,   163,    -1,   165,    -1,    42,   165,    -1,    17,   164,
     165,    -1,    16,   164,   165,    -1,   164,    47,    41,   165,
      -1,    17,   164,    47,    41,   165,    -1,    16,   164,    47,
      41,   165,    -1,    -1,    42,    -1,    36,    -1,    23,    -1,
      31,    -1,    49,    -1,    19,    -1,   168,    -1,    12,    -1,
      13,    -1,   167,   157,    -1,   169,    -1,   167,   131,   169,
      -1,   170,    -1,   168,    95,   200,    -1,   168,    82,   168,
      -1,   168,    83,   168,    -1,   168,    84,   168,    -1,   168,
      85,   168,    -1,   115,   129,   169,   131,   169,   130,    -1,
     168,    92,   168,    -1,   168,    94,   168,    -1,   168,    86,
     168,    -1,   168,    87,   168,    -1,   168,    88,   168,    -1,
     168,    89,   168,    -1,   168,    90,   168,    -1,   168,    91,
     168,    -1,   168,     7,   168,    -1,    86,   168,    -1,    87,
     168,    -1,   170,    -1,   169,    95,   200,    -1,   169,    67,
     169,    -1,   169,    68,   169,    -1,   169,    69,   169,    -1,
     169,    70,   169,    -1,   169,    71,   169,    -1,   169,    72,
     169,    -1,   169,    73,   169,    -1,   169,    74,   169,    -1,
     169,    75,   169,    -1,   169,    76,   169,    -1,   169,    77,
     169,    -1,   169,    78,   169,    -1,   169,    79,   169,    -1,
     169,    80,   169,    -1,   169,    81,   169,    -1,   169,    82,
     169,    -1,   169,    83,   169,    -1,   169,    84,   169,    -1,
      85,    -1,    85,   169,    -1,   169,    85,    -1,   169,    85,
     169,    -1,   115,   129,   169,   131,   169,   130,    -1,   169,
      92,   169,    -1,   169,    94,   169,    -1,   169,    86,   169,
      -1,   169,    87,   169,    -1,   169,    88,   169,    -1,   169,
      89,   169,    -1,   169,    90,   169,    -1,   169,    91,   169,
      -1,   169,     7,   169,    -1,    93,   169,    -1,    86,   169,
      -1,    87,   169,    -1,   171,    -1,   172,    -1,   129,   169,
     130,    -1,   129,   169,   130,   174,    -1,     6,    -1,     6,
     174,    -1,    54,    -1,    54,   174,    -1,     4,    -1,     3,
      -1,    35,    -1,     5,    -1,    18,    -1,   175,    -1,   175,
     174,    -1,   176,    -1,   176,   174,    -1,   183,    -1,   183,
     174,    -1,   184,    -1,   184,   174,    -1,   187,    -1,   187,
     174,    -1,   188,    -1,   188,   174,    -1,   195,    -1,   192,
      -1,   192,   174,    -1,     8,    -1,     9,   173,    -1,   167,
      11,    -1,   167,    10,   173,    -1,    57,   166,    59,    -1,
     174,    57,   166,    59,    -1,   127,   128,    -1,   127,   166,
     128,    -1,   127,   169,   133,   177,   128,    -1,   178,    -1,
     179,   157,    -1,   180,    -1,   179,   131,   180,    -1,   181,
      79,   169,    -1,   181,    79,   169,    56,   169,    -1,     6,
      77,   169,    -1,   182,   157,    -1,     6,    -1,   182,   131,
       6,    -1,    57,    59,    -1,    57,   166,    59,    -1,    58,
      60,    -1,    58,   186,    60,    -1,    58,   186,   133,    60,
      -1,    58,   185,    60,    -1,   133,   133,    -1,   133,   186,
     133,    -1,   185,   131,   133,   186,   133,    -1,   166,    -1,
     186,   133,   166,    -1,    57,   169,   133,   177,    59,    -1,
      33,   169,    51,   169,    29,    -1,    33,   169,    51,   169,
     189,    27,   169,    29,    -1,    -1,   189,    28,   169,    51,
     169,    -1,    97,    -1,    98,    -1,    99,    -1,   100,    -1,
     101,    -1,   102,    -1,   103,    -1,   104,    -1,   105,    -1,
     106,    -1,   107,    -1,   108,    -1,   109,    -1,   110,    -1,
     111,    -1,   112,    -1,   113,    -1,   114,    -1,   116,    -1,
     117,    -1,   118,    -1,   119,    -1,   120,    -1,   121,    -1,
     122,    -1,   125,    -1,   123,    -1,   190,   129,   169,   131,
     169,   130,    -1,   190,   129,   169,   130,    -1,     6,   129,
     130,    -1,   191,    -1,     6,   129,   193,   130,    -1,     6,
     129,   193,   130,   129,   169,   130,    -1,   194,   157,    -1,
     169,    -1,   169,    56,   169,    -1,   194,   131,   169,    -1,
     194,   131,   169,    56,   169,    -1,    37,   127,   196,   128,
      79,   169,    -1,    37,   127,   196,   197,   128,    79,   169,
      -1,   198,    -1,   147,    -1,   196,   197,   198,    -1,   196,
     197,   147,    -1,   131,    -1,   126,    -1,   159,   199,    -1,
     159,   199,    77,   169,    -1,    -1,   201,    -1,   171,    -1,
     172,    -1,    95,   200,    -1,   201,    95,   200,    -1,     6,
      -1,    97,    -1,    98,    -1,    99,    -1,   100,    -1,   101,
      -1,   102,    -1,   103,    -1,   104,    -1,   105,    -1,   106,
      -1,   107,    -1,   108,    -1,   109,    -1,   110,    -1,   111,
      -1,   112,    -1,   113,    -1,   114,    -1,   115,    -1,   116,
      -1,   117,    -1,   118,    -1,   119,    -1,   120,    -1,   121,
      -1,   122,    -1,   123,    -1,   125,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   620,   620,   622,   624,   627,   636,   645,   654,   663,
     665,   668,   676,   685,   685,   687,   703,   707,   709,   711,
     712,   714,   716,   718,   720,   722,   725,   725,   725,   725,
     726,   726,   726,   726,   726,   727,   727,   730,   753,   759,
     766,   774,   784,   798,   799,   803,   807,   813,   815,   822,
     827,   832,   839,   843,   851,   861,   868,   877,   889,   897,
     898,   903,   904,   906,   911,   912,   916,   920,   925,   925,
     928,   930,   934,   939,   943,   945,   949,   950,   956,   965,
     968,   976,   984,   993,  1002,  1011,  1024,  1025,  1029,  1031,
    1033,  1035,  1037,  1039,  1041,  1046,  1052,  1055,  1057,  1063,
    1064,  1066,  1068,  1070,  1072,  1081,  1090,  1092,  1094,  1096,
    1098,  1100,  1102,  1104,  1106,  1112,  1114,  1127,  1128,  1130,
    1132,  1134,  1136,  1138,  1140,  1142,  1144,  1146,  1148,  1150,
    1152,  1154,  1156,  1158,  1160,  1162,  1164,  1166,  1168,  1177,
    1186,  1195,  1204,  1206,  1208,  1210,  1212,  1214,  1216,  1218,
    1220,  1226,  1228,  1235,  1247,  1249,  1253,  1255,  1257,  1259,
    1262,  1264,  1267,  1269,  1271,  1273,  1275,  1277,  1278,  1281,
    1282,  1285,  1286,  1289,  1290,  1293,  1294,  1297,  1298,  1301,
    1302,  1303,  1308,  1310,  1316,  1321,  1329,  1336,  1345,  1347,
    1352,  1358,  1361,  1364,  1366,  1370,  1372,  1374,  1377,  1380,
    1382,  1386,  1388,  1392,  1394,  1405,  1416,  1456,  1459,  1464,
    1471,  1476,  1480,  1486,  1493,  1509,  1510,  1514,  1516,  1518,
    1520,  1522,  1524,  1526,  1528,  1530,  1532,  1534,  1536,  1538,
    1540,  1542,  1544,  1546,  1548,  1550,  1552,  1554,  1556,  1558,
    1560,  1562,  1564,  1566,  1570,  1578,  1610,  1612,  1613,  1633,
    1689,  1692,  1695,  1698,  1700,  1704,  1711,  1720,  1722,  1730,
    1732,  1741,  1741,  1744,  1750,  1761,  1762,  1765,  1767,  1771,
    1775,  1779,  1781,  1783,  1785,  1787,  1789,  1791,  1793,  1795,
    1797,  1799,  1801,  1803,  1805,  1807,  1809,  1811,  1813,  1815,
    1817,  1819,  1821,  1823,  1825,  1827,  1829,  1831,  1833,  1835
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
  "\">\"", "\"<=\"", "\">=\"", "\"=\"", "\"!=\"", "\"in\"", "\"subset\"",
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
  "doc_file_comments", "semi_or_none", "item", "item_tail",
  "error_item_start", "include_item", "vardecl_item", "enum_id_list",
  "assign_item", "constraint_item", "solve_item", "output_item",
  "predicate_item", "function_item", "annotation_item",
  "operation_item_tail", "params", "params_list", "params_list_head",
  "comma_or_none", "ti_expr_and_id_or_anon", "ti_expr_and_id",
  "ti_expr_list", "ti_expr_list_head", "ti_expr", "base_ti_expr",
  "opt_opt", "base_ti_expr_tail", "expr_list", "expr_list_head",
  "set_expr", "expr", "expr_atom_head", "expr_atom_head_nonstring",
  "string_expr", "string_quote_rest", "array_access_tail", "set_literal",
  "set_comp", "comp_tail", "generator_list", "generator_list_head",
  "generator", "id_list", "id_list_head", "simple_array_literal",
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
     375,   376,   377,   378,   379,   380,    59,   123,   125,    40,
      41,    44,    58,   124
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   134,   135,   136,   136,   137,   137,   137,   137,   137,
     137,   138,   138,   139,   139,   140,   140,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   142,   142,   142,   142,
     142,   142,   142,   142,   142,   142,   142,   143,   144,   144,
     144,   144,   144,   145,   145,   145,   146,   147,   147,   148,
     148,   148,   149,   150,   150,   151,   151,   152,   152,   153,
     153,   154,   154,   154,   155,   155,   156,   156,   157,   157,
     158,   158,   159,   160,   161,   161,   162,   162,   162,   163,
     163,   163,   163,   163,   163,   163,   164,   164,   165,   165,
     165,   165,   165,   165,   165,   165,   166,   167,   167,   168,
     168,   168,   168,   168,   168,   168,   168,   168,   168,   168,
     168,   168,   168,   168,   168,   168,   168,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   170,   170,   171,   171,   171,   171,
     171,   171,   171,   171,   171,   171,   171,   171,   171,   171,
     171,   171,   171,   171,   171,   171,   171,   171,   171,   171,
     171,   171,   172,   172,   173,   173,   174,   174,   175,   175,
     176,   177,   178,   179,   179,   180,   180,   180,   181,   182,
     182,   183,   183,   184,   184,   184,   184,   185,   185,   185,
     186,   186,   187,   188,   188,   189,   189,   190,   190,   190,
     190,   190,   190,   190,   190,   190,   190,   190,   190,   190,
     190,   190,   190,   190,   190,   190,   190,   190,   190,   190,
     190,   190,   190,   190,   191,   191,   192,   192,   192,   192,
     193,   194,   194,   194,   194,   195,   195,   196,   196,   196,
     196,   197,   197,   198,   198,   199,   199,   200,   200,   201,
     201,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     2,     3,     4,     2,
       3,     1,     2,     0,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     2,     4,
       2,     6,     7,     0,     1,     3,     3,     2,     4,     3,
       4,     4,     2,     5,     5,     7,     8,     3,     5,     0,
       2,     0,     3,     3,     0,     2,     1,     3,     0,     1,
       1,     1,     3,     2,     1,     3,     1,     6,     3,     1,
       2,     3,     3,     4,     5,     5,     0,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     3,     1,
       3,     3,     3,     3,     3,     6,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     2,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     1,     2,     2,
       3,     6,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     2,     2,     1,     1,     3,     4,     1,     2,
       1,     2,     1,     1,     1,     1,     1,     1,     2,     1,
       2,     1,     2,     1,     2,     1,     2,     1,     2,     1,
       1,     2,     1,     2,     2,     3,     3,     4,     2,     3,
       5,     1,     2,     1,     3,     3,     5,     3,     2,     1,
       3,     2,     3,     2,     3,     4,     3,     2,     3,     5,
       1,     3,     5,     5,     8,     0,     5,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     6,     4,     3,     1,     4,     7,
       2,     1,     3,     3,     5,     6,     7,     1,     1,     3,
       3,     1,     1,     2,     4,     0,     1,     1,     1,     2,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,   163,   162,   165,   158,   182,     0,    94,    95,
      86,    11,    86,    86,   166,    92,     0,     0,    89,     0,
       0,    90,    86,     0,     0,   164,    88,     0,     0,    87,
       0,     0,   265,    91,     0,   160,     0,     0,     0,     0,
     217,   218,   219,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,     0,   235,
     236,   237,   238,   239,   240,   241,   243,   242,     0,     0,
       0,     2,    13,    86,     5,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,   265,     0,    76,     0,    79,
      93,    99,   154,   155,   167,   169,   171,   173,   175,   177,
       0,   247,   180,   179,    86,     0,     0,     0,   159,   158,
     137,     0,     0,     0,     0,     0,    97,   117,   183,    15,
      87,     0,     0,    61,    86,     0,    47,    40,     0,     0,
      37,    86,    86,    80,    52,    61,     0,     0,   266,    61,
     161,   201,     0,    68,    97,   203,     0,   210,     0,     0,
     115,   116,     0,   188,     0,    97,     0,     1,    14,     4,
      12,     6,   158,    35,    30,    27,    33,    26,    29,    32,
      31,    34,     9,    36,     0,    38,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   168,   170,   172,   174,   176,   178,     0,   181,
      10,     0,    46,   246,   251,     0,    68,     0,   138,   152,
     153,   151,     0,     0,   184,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   139,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    82,     0,    81,     0,
      57,     0,    68,    74,     0,     0,     0,     0,   258,   265,
       0,   257,    78,   265,   267,   268,   269,     0,     0,    49,
       0,   265,   202,    69,    96,     0,   207,     0,   206,     0,
     204,     0,     0,   189,     0,   156,    86,     7,     0,     0,
      72,     0,   114,   101,   102,   103,   104,   108,   109,   110,
     111,   112,   113,   106,   107,   100,     0,   186,     0,   248,
      69,   250,     0,     0,   185,    98,   150,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   140,   144,   145,   146,   147,
     148,   149,   142,   143,   118,     0,     0,     0,     0,    68,
      66,    70,    71,     0,     0,    69,    73,    48,     0,    43,
     271,   272,   273,   274,   275,   276,   277,   278,   279,   280,
     281,   282,   283,   284,   285,   286,   287,   288,   289,   290,
     291,   292,   293,   294,   295,   296,   297,   298,   299,    61,
     215,   263,   262,     0,   261,    86,    59,    51,    50,   270,
      59,   199,     0,   191,    68,   193,     0,    68,   208,     0,
     205,   211,     0,     0,   157,     8,    72,    39,    64,    83,
     245,     0,   252,     0,   253,   187,     0,    85,    84,    63,
      62,    69,    65,    58,    86,    75,     0,    44,     0,   265,
     213,     0,     0,     0,     0,   260,   259,     0,    53,    54,
       0,   212,    69,   192,     0,    69,   198,     0,     0,   190,
       0,     0,     0,     0,     0,    67,    77,     0,    41,     0,
      59,     0,     0,   264,   255,     0,    60,   197,   194,   195,
     200,   209,   105,   265,   244,   249,   254,   141,    42,    45,
      55,     0,     0,   256,     0,    59,   214,     0,   196,    56,
     216
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    70,    71,    72,    73,   159,    74,    75,   172,    76,
      77,   438,    78,    79,    80,    81,    82,    83,    84,   448,
     250,   348,   349,   274,   350,    85,   251,   252,    86,    87,
      88,    89,   147,   143,    90,   116,   117,    92,    93,   118,
     108,    94,    95,   402,   403,   404,   405,   406,   407,    96,
      97,   148,   149,    98,    99,   441,   100,   101,   102,   205,
     206,   103,   260,   395,   261,   137,   266,   138,   389
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -376
static const yytype_int16 yypact[] =
{
     768,  -103,  -376,  -376,  -376,     8,  -376,  3310,  -376,  -376,
    1659,  -376,   -13,   -13,  -376,  -376,    41,   -14,  -376,  2802,
      49,  -376,  2167,  3310,    51,  -376,  -376,   -86,    21,  2675,
    3310,    58,   -27,  -376,    63,    23,  2929,   480,  3564,  3564,
    -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,
    -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,   -51,  -376,
    -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,  3056,  3310,
      81,  -376,   -40,  1278,  1405,  -376,  -376,  -376,  -376,  -376,
    -376,  -376,  -376,  -376,  -376,   -27,   -43,  -376,    50,  -376,
     100,  -376,  -376,  -376,    23,    23,    23,    23,    23,    23,
     -31,  -376,    23,  -376,  1532,  3310,  3310,  1023,    45,   -39,
    3437,  3310,  3310,  3310,   -28,    10,  4646,  -376,  -376,  -376,
    -376,  2421,  2548,   -21,  2167,    28,  4646,    32,   -22,  4348,
    -376,  1913,  2294,  -376,  4646,   -21,  3599,    18,    43,   -21,
      45,  -376,    83,     9,  3681,  -376,   637,  -376,     3,   -34,
      33,    33,  3310,  -376,    17,  3716,  3946,  -376,  1151,  -376,
    -376,  -376,    39,  -376,  -376,  -376,  -376,  -376,  -376,  -376,
    -376,  -376,  -376,  -376,    14,    67,   141,   122,  3564,  3564,
    3564,  3564,  3564,  3564,  3564,  3564,  3564,  3564,  3564,  3564,
    3564,  3599,    45,    45,    45,    45,    45,    45,  3310,    45,
    -376,   105,  4646,  -376,  4388,    36,    40,  3310,    27,    37,
      37,    37,  3310,  3310,  -376,  3310,  3310,  3310,  3310,  3310,
    3310,  3310,  3310,  3310,  3310,  3310,  3310,  3310,  3310,  3310,
    3310,  3310,  3310,  3310,  3310,  3437,  3310,  3310,  3310,  3310,
    3310,  3310,  3310,  3310,  3599,   126,  -376,   129,  -376,   895,
      96,   115,    46,  -376,  3310,    16,  4182,  3310,  -376,   -27,
     -96,  -376,  -376,   -27,  -376,  -376,  -376,  3310,  3310,  -376,
    3599,   -27,  -376,  3310,  -376,   169,  -376,    47,  -376,    93,
    -376,  3183,  3831,  -376,   169,    23,  1278,  -376,   170,  3310,
      98,  2675,   136,   251,   251,   251,   276,    35,    35,    33,
      33,    33,    33,   251,    24,  -376,  3802,  -376,  3310,   103,
    3310,  -376,   174,  3917,  -376,  4646,   142,  4696,   758,   758,
     469,   469,   885,  1012,  1012,  1012,  1012,  1012,  1012,   984,
     984,   984,   291,   291,   291,    27,   162,   162,    37,    37,
      37,    37,   291,    44,  -376,  2675,  2675,   110,   111,   112,
    -376,  -376,    14,  3310,   203,  2040,  -376,  4732,   116,   241,
    -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,
    -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,
    -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,  -376,   -21,
    4474,   178,  -376,   180,  -376,  1786,   183,  4646,  4646,  -376,
     183,   184,   204,  -376,   134,  -376,   187,   137,  3310,  3310,
    -376,  -376,  3310,   139,    45,  -376,  -376,  4646,  2040,  -376,
    -376,  3310,  4646,  3310,  4438,  -376,  3310,  -376,  -376,  -376,
    -376,  2040,  -376,  4646,  2294,  -376,  3310,  -376,   -83,   -27,
    -376,    25,  3310,  3310,   193,  -376,  -376,  3310,  -376,  -376,
    3310,  -376,   169,  -376,  3310,   267,  -376,   143,  4032,  -376,
     144,  4061,  4147,  3310,  4176,  -376,  -376,  4262,  -376,   271,
     183,  3310,  3310,  4646,  4646,  3310,  4646,  4646,  -376,  4524,
    -376,  3310,  -376,   -27,  -376,  -376,  4646,  -376,  -376,  -376,
    -376,  4560,  4610,  4646,  3310,   183,  -376,  3310,  4646,  -376,
    4646
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -376,  -376,  -376,  -376,   120,  -376,   -58,   269,  -376,  -376,
    -376,  -376,  -376,  -124,  -376,  -376,  -376,  -376,  -376,  -375,
    -125,  -138,  -376,  -168,  -150,   -71,  -376,  -376,   -20,  -127,
      54,   -16,   -35,    12,   -30,   -19,   284,  -108,  -109,    69,
     -23,  -376,  -376,     1,  -376,  -376,  -166,  -376,  -376,  -376,
    -376,  -376,  -140,  -376,  -376,  -376,  -376,  -376,  -376,  -376,
    -376,  -376,  -376,  -376,  -107,   -29,  -167,  -376,  -376
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -87
static const yytype_int16 yytable[] =
{
     126,   142,   128,   173,   129,   262,   277,   258,   150,   151,
     263,   134,   140,   133,   271,   161,   254,   144,   105,   115,
     213,   214,   358,   104,   305,   449,   280,   265,   264,   120,
     392,   178,   393,   154,   216,   394,     6,     7,   311,   -28,
     178,   131,   178,   124,   216,   468,   200,   123,   469,   155,
     156,   216,   471,   472,   174,   127,   175,   267,   268,   130,
     259,   269,   132,   278,   135,   105,   121,   122,   136,   139,
     201,   192,   193,   194,   195,   196,   197,   344,   152,   199,
     105,   157,   265,   264,   356,   106,   158,   202,   204,   176,
     107,   208,   209,   210,   211,   490,   105,   177,   198,   281,
     287,   212,   207,   399,   253,   246,   248,   178,   249,   255,
     256,   174,   -87,   236,   237,   238,   239,   240,   241,   191,
     499,   243,   244,   185,   186,   187,   188,   190,   191,   190,
     191,   243,   244,   282,   279,   265,   264,   107,   270,   244,
     273,   215,   272,   359,   289,   283,   288,   290,   292,   293,
     294,   295,   296,   297,   298,   299,   300,   301,   302,   303,
     304,   265,   264,   291,   307,   -28,   309,   345,   107,   216,
     346,   310,   312,   353,   354,   401,   416,   355,   351,   306,
     408,   432,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   313,   190,   191,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     339,   340,   341,   342,   343,   115,   409,   418,   415,   352,
     391,   191,   423,   425,   396,   357,   453,   244,   390,   456,
     429,   430,   400,   431,   434,   436,   411,   437,   397,   398,
     238,   239,   240,   241,   315,   442,   243,   244,   178,   443,
     447,   450,   414,   451,   439,   452,   454,   459,   455,   457,
     417,   445,   475,   480,   483,   419,   481,   489,   286,   119,
     460,   465,   314,   178,    91,   413,   478,     0,   446,   422,
       0,   424,     0,     0,    91,     0,     0,     0,   216,     0,
       0,     0,     0,     0,     0,     0,    91,   466,     0,     0,
       0,     0,     0,    91,     0,     0,     0,     0,     0,     0,
       0,     0,    91,    91,   259,     0,     0,     0,     0,   427,
     428,     0,     0,     0,   433,   435,   182,   183,   184,   185,
     186,   187,   188,     0,     0,   190,   191,   351,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    91,    91,     0,
     351,   -87,   183,   184,   185,   186,   187,   188,     0,     0,
     190,   191,     0,   411,     0,   174,   235,   236,   237,   238,
     239,   240,   241,     0,     0,   243,   244,     0,    91,     0,
       0,     0,     0,   458,     0,     0,     0,     0,   352,     0,
       0,     0,   461,     0,   462,    91,    91,   464,    91,     0,
     470,   352,     0,     0,     0,    91,    91,   467,     0,     0,
       0,     0,     0,   473,   474,     0,     0,     0,   476,     0,
       0,   477,     0,     0,     0,   479,     0,     0,     0,     0,
       0,     0,    91,     0,   486,     0,   411,     0,     0,     0,
       0,     0,   491,   492,   495,     0,   493,     0,     0,     0,
       0,     0,    91,    91,    91,    91,    91,    91,    91,    91,
      91,    91,    91,    91,    91,   498,   216,     0,   500,     0,
       0,     0,     0,     2,     3,     4,   109,     0,     6,     7,
       0,     0,     0,     0,     0,     0,     0,     0,    14,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    23,     0,    25,     0,    27,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    91,    35,     0,     0,    36,    37,     0,
     145,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
     241,   242,     0,   243,   244,   110,   111,   112,     0,     0,
      91,     0,     0,   113,     0,    91,     0,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,   114,    59,    60,    61,    62,
      63,    64,    65,    66,     0,    67,     0,    68,     0,    69,
       0,     0,     0,   146,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    91,
      91,     0,     0,     0,     0,     0,     0,     0,     0,    91,
       2,     3,     4,   109,     0,     6,     7,     0,     0,     0,
       0,     0,     0,     0,     0,    14,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      23,     0,    25,     0,    27,     0,     0,     0,     0,    91,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    35,     0,     0,    36,    37,     0,     0,     0,     0,
       0,     0,    91,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    91,     0,     0,    91,     0,
       0,     0,   110,   111,   112,     0,     0,     0,     0,     0,
     113,     0,     0,     0,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,   114,    59,    60,    61,    62,    63,    64,    65,
      66,     0,    67,     0,    68,   216,    69,     0,    -3,     1,
     276,     2,     3,     4,     5,     0,     6,     7,     0,     0,
       8,     9,    10,    11,    12,    13,    14,    15,    16,     0,
      17,    18,     0,    19,     0,     0,     0,     0,    20,    21,
      22,    23,    24,    25,    26,    27,    28,     0,     0,     0,
      29,     0,    30,    31,     0,   -86,    32,    33,    34,     0,
       0,     0,    35,     0,     0,    36,    37,     0,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
     242,     0,   243,   244,    38,    39,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,   216,    67,     0,    68,   347,    69,     2,     3,
       4,   109,     0,     6,     7,     0,     0,     8,     9,     0,
       0,    12,    13,    14,    15,     0,     0,    17,    18,     0,
       0,     0,     0,     0,     0,     0,    21,     0,    23,     0,
      25,    26,    27,    28,     0,     0,     0,    29,     0,     0,
       0,     0,   -86,     0,    33,     0,     0,     0,     0,    35,
       0,     0,    36,    37,     0,     0,     0,     0,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,     0,   243,
     244,    38,    39,     0,     0,     0,     0,     0,     0,     0,
       0,   216,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,   216,
      67,     0,    68,     0,    69,   -64,     2,     3,     4,   109,
       0,     6,     7,     0,     0,     0,     0,     0,     0,     0,
       0,    14,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    23,     0,    25,     0,
      27,     0,     0,   -87,   -87,   -87,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,    35,   243,   244,
      36,    37,     0,     0,     0,   -87,   -87,   -87,   -87,   -87,
     -87,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,     0,   243,   244,   110,   111,
     112,     0,     0,     0,     0,     0,   113,     0,     0,     0,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,   114,    59,
      60,    61,    62,    63,    64,    65,    66,     0,    67,     0,
      68,     0,    69,   203,     2,     3,     4,     5,     0,     6,
       7,     0,     0,     8,     9,    10,    11,    12,    13,    14,
      15,    16,     0,    17,    18,     0,    19,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,    27,    28,
       0,     0,     0,    29,     0,    30,    31,     0,   -86,    32,
      33,    34,     0,     0,     0,    35,     0,     0,    36,    37,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    38,    39,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,     0,    67,     0,    68,     0,
      69,     2,     3,     4,     5,     0,     6,     7,     0,     0,
       8,     9,    10,   160,    12,    13,    14,    15,    16,     0,
      17,    18,     0,    19,     0,     0,     0,     0,    20,    21,
      22,    23,    24,    25,    26,    27,    28,     0,     0,     0,
      29,     0,    30,    31,     0,     0,    32,    33,    34,     0,
       0,     0,    35,     0,     0,    36,    37,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    38,    39,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,     0,    67,     0,    68,     0,    69,     2,     3,
       4,   162,     0,     6,     7,     0,     0,     8,     9,     0,
       0,    12,    13,    14,    15,   163,     0,    17,    18,     0,
     164,     0,     0,     0,     0,   165,    21,   166,    23,   167,
      25,    26,    27,    28,     0,     0,     0,    29,     0,   168,
     169,     0,   -86,   170,    33,   171,     0,     0,     0,    35,
       0,     0,    36,    37,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    38,    39,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,     0,
      67,     0,    68,     0,    69,     2,     3,     4,     5,     0,
       6,     7,     0,     0,     8,     9,    10,     0,    12,    13,
      14,    15,    16,     0,    17,    18,     0,    19,     0,     0,
       0,     0,    20,    21,    22,    23,    24,    25,    26,    27,
      28,     0,     0,     0,    29,     0,    30,    31,     0,     0,
      32,    33,    34,     0,     0,     0,    35,     0,     0,    36,
      37,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    38,    39,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,     0,    67,     0,    68,
       0,    69,     2,     3,     4,     5,     0,     6,     7,     0,
       0,     8,     9,     0,     0,    12,    13,    14,    15,    16,
       0,    17,    18,     0,    19,     0,     0,     0,     0,    20,
      21,    22,    23,    24,    25,    26,    27,    28,     0,     0,
       0,    29,     0,    30,    31,     0,     0,    32,    33,    34,
       0,     0,     0,    35,     0,     0,    36,    37,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    38,    39,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,     0,    67,     0,    68,     0,    69,     2,
       3,     4,   109,     0,     6,     7,     0,     0,     8,     9,
       0,     0,    12,    13,    14,    15,     0,     0,    17,    18,
       0,    19,     0,     0,     0,     0,     0,    21,     0,    23,
       0,    25,    26,    27,    28,     0,     0,     0,    29,     0,
       0,     0,     0,     0,     0,    33,     0,     0,     0,     0,
      35,     0,     0,    36,    37,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    38,    39,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
       0,    67,     0,    68,   444,    69,     2,     3,     4,   109,
       0,     6,     7,     0,     0,     8,     9,     0,     0,    12,
      13,    14,    15,     0,     0,    17,    18,     0,    19,     0,
       0,     0,     0,     0,    21,     0,    23,     0,    25,    26,
      27,    28,     0,     0,     0,    29,     0,     0,     0,     0,
       0,     0,    33,     0,     0,     0,     0,    35,     0,     0,
      36,    37,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    38,
      39,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,     0,    67,     0,
      68,     0,    69,     2,     3,     4,   109,     0,     6,     7,
       0,     0,     8,     9,     0,     0,    12,    13,    14,    15,
       0,     0,    17,    18,     0,     0,     0,     0,     0,     0,
       0,    21,     0,    23,     0,    25,    26,    27,    28,     0,
       0,     0,    29,     0,     0,     0,     0,   -86,     0,    33,
       0,     0,     0,     0,    35,     0,     0,    36,    37,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    38,    39,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,     0,    67,     0,    68,     0,    69,
       2,     3,     4,   109,     0,     6,     7,     0,     0,     8,
       9,     0,     0,    12,    13,    14,    15,     0,     0,    17,
      18,     0,     0,     0,     0,     0,     0,     0,    21,     0,
      23,     0,    25,    26,    27,    28,     0,     0,     0,    29,
       0,     0,     0,     0,     0,     0,    33,     0,     0,     0,
       0,    35,     0,     0,    36,    37,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    38,    39,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,     0,    67,     0,    68,     0,    69,     2,     3,     4,
     109,     0,     6,     7,     0,     0,     8,     9,     0,     0,
      12,    13,    14,    15,     0,     0,     0,    18,     0,     0,
       0,     0,     0,     0,     0,    21,     0,    23,     0,    25,
      26,    27,     0,     0,     0,     0,    29,     0,     0,     0,
       0,     0,     0,    33,     0,     0,     0,     0,    35,     0,
       0,    36,    37,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      38,    39,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,     0,    67,
       0,    68,     0,    69,     2,     3,     4,   109,     0,     6,
       7,     0,     0,     8,     9,     0,     0,     0,     0,    14,
      15,     0,     0,     0,    18,     0,     0,     0,     0,     0,
       0,     0,    21,     0,    23,     0,    25,    26,    27,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   245,     0,
      33,     0,     0,     0,     0,    35,     0,     0,    36,    37,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    38,    39,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,     0,    67,     0,    68,     0,
      69,     2,     3,     4,   109,     0,     6,     7,     0,     0,
       8,     9,     0,     0,     0,     0,    14,    15,     0,     0,
       0,    18,     0,     0,     0,     0,     0,     0,     0,    21,
       0,    23,     0,    25,    26,    27,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   247,     0,    33,     0,     0,
       0,     0,    35,     0,     0,    36,    37,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    38,    39,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,     0,    67,     0,    68,     0,    69,     2,     3,
       4,   109,     0,     6,     7,     0,     0,     8,     9,     0,
       0,     0,     0,    14,    15,     0,     0,     0,    18,     0,
       0,     0,     0,     0,     0,     0,    21,     0,    23,     0,
      25,    26,    27,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    33,     0,     0,     0,     0,    35,
       0,     0,    36,    37,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    38,    39,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,     0,
      67,     0,    68,     0,    69,     2,     3,     4,   109,     0,
       6,     7,     0,     0,     0,     0,     0,     0,     0,     0,
      14,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    23,     0,    25,     0,    27,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    35,     0,     0,    36,
      37,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   110,   111,   112,
       0,     0,     0,     0,     0,   113,     0,   125,     0,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,   114,    59,    60,
      61,    62,    63,    64,    65,    66,     0,    67,     0,    68,
       0,    69,     2,     3,     4,   109,     0,     6,     7,     0,
       0,     0,     0,     0,     0,     0,     0,    14,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    23,     0,    25,     0,    27,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    35,     0,     0,    36,    37,   141,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   110,   111,   112,     0,     0,     0,
       0,     0,   113,     0,     0,     0,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,   114,    59,    60,    61,    62,    63,
      64,    65,    66,     0,    67,     0,    68,     0,    69,     2,
       3,     4,   109,     0,     6,     7,     0,     0,     0,     0,
       0,     0,     0,     0,    14,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    23,
       0,    25,     0,    27,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      35,     0,     0,    36,    37,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   110,   111,   112,     0,     0,     0,     0,     0,   113,
       0,     0,     0,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,   114,    59,    60,    61,    62,    63,    64,    65,    66,
       0,    67,     0,    68,   153,    69,     2,     3,     4,   109,
       0,     6,     7,     0,     0,     0,     0,     0,     0,     0,
       0,    14,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    23,     0,    25,     0,
      27,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    35,     0,     0,
      36,    37,     0,   410,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   110,   111,
     112,     0,     0,     0,     0,     0,   113,     0,     0,     0,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,   114,    59,
      60,    61,    62,    63,    64,    65,    66,     0,    67,     0,
      68,     0,    69,     2,     3,     4,   109,     0,     6,     7,
       0,     0,     0,     0,     0,     0,     0,     0,    14,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    23,     0,    25,     0,    27,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    35,     0,     0,    36,    37,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   110,   111,   112,     0,     0,
       0,     0,     0,   113,     0,     0,     0,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,   114,    59,    60,    61,    62,
      63,    64,    65,    66,     0,    67,     0,    68,     0,    69,
       2,     3,     4,   109,     0,     6,     7,     0,     0,     0,
       0,     0,     0,     0,     0,    14,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      23,     0,    25,     0,    27,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    35,     0,     0,    36,    37,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   -87,   111,   112,     0,     0,     0,     0,     0,
     113,     0,     0,     0,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,   114,    59,    60,    61,    62,    63,    64,    65,
      66,     0,    67,     0,    68,     0,    69,     2,     3,     4,
     109,     0,     6,     7,     0,     0,     0,     0,     0,     0,
       0,     0,    14,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    23,     0,    25,
       0,    27,     2,     3,     4,   109,     0,     6,     7,     0,
       0,     0,     0,     0,     0,     0,     0,    14,    35,     0,
       0,    36,    37,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    23,     0,    25,     0,    27,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      38,    39,     0,    35,     0,     0,    36,    37,     0,     0,
       0,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,   216,    67,
       0,    68,     0,    69,     0,     0,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,     0,    59,    60,    61,    62,    63,
      64,    65,    66,   216,    67,     0,    68,     0,    69,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,     0,   243,   244,     0,     0,     0,
       0,     0,     0,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   216,
     243,   244,     0,     0,   275,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   216,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   284,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,     0,   243,   244,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   216,   243,   244,     0,     0,     0,
       0,     0,   420,   421,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   216,     0,     0,     0,     0,     0,     0,
       0,     0,   412,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   242,
       0,   243,   244,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   216,
     243,   244,     0,     0,     0,     0,     0,     0,   426,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   216,     0,
       0,     0,     0,     0,     0,     0,   285,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,     0,   243,   244,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   216,   243,   244,     0,     0,     0,
       0,     0,   482,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   216,     0,     0,     0,     0,   360,     0,
       0,   484,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   242,
       0,   243,   244,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   216,
     243,   244,     0,     0,     0,     0,     0,   485,     0,   361,
     362,   363,   364,   365,   366,   367,   368,   369,   370,   371,
     372,   373,   374,   375,   376,   377,   378,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   487,   388,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   216,   243,   244,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   488,     0,     0,   216,     0,     0,     0,   257,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
     242,     0,   243,   244,   308,   216,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
     242,   216,   243,   244,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   463,     0,     0,     0,     0,     0,
       0,     0,     0,   440,     0,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
     242,   216,   243,   244,     0,     0,     0,     0,     0,     0,
       0,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   216,   243,   244,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     494,     0,     0,     0,     0,     0,     0,     0,     0,   496,
       0,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   216,   243,   244,
       0,     0,     0,     0,     0,     0,     0,   217,   218,   219,
     220,   221,   222,   223,   224,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,   242,   216,   243,   244,     0,     0,     0,     0,
       0,   497,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   217,   218,   219,
     220,   221,   222,   223,   224,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,   242,   216,   243,   244,     0,     0,     0,     0,
       0,     0,     0,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   216,
     243,   244,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,     0,
     243,   244,     0,     0,     0,     0,     0,     0,     0,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,     0,     0,
     238,   239,   240,   241,   242,     0,   243,   244
};

static const yytype_int16 yycheck[] =
{
      19,    36,    22,    74,    23,   132,   146,   131,    38,    39,
     135,    30,    35,    29,   139,    73,   125,    36,    57,     7,
      10,    11,     6,   126,   191,   400,    60,   136,   136,    42,
     126,     7,   128,    68,     7,   131,     8,     9,   206,     0,
       7,   127,     7,    57,     7,   128,   104,     6,   131,    68,
      69,     7,    27,    28,    74,     6,    85,    39,    40,     8,
     131,    43,    41,    60,     6,    57,    12,    13,    95,     6,
     105,    94,    95,    96,    97,    98,    99,   244,   129,   102,
      57,     0,   191,   191,   252,    77,   126,   106,   107,   132,
     129,   110,   111,   112,   113,   470,    57,    47,   129,   133,
     158,   129,    57,   270,   124,   121,   122,     7,   129,    77,
     132,   131,    85,    86,    87,    88,    89,    90,    91,    95,
     495,    94,    95,    88,    89,    90,    91,    94,    95,    94,
      95,    94,    95,   152,   131,   244,   244,   129,    95,    95,
     131,   131,    59,   127,    77,   128,   132,     6,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   270,   270,    41,    59,   126,   130,    41,   129,     7,
      41,   131,   207,    77,    59,     6,     6,   131,   249,   198,
     133,   349,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,   212,    94,    95,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   243,   213,   133,   129,   286,   249,
     259,    95,   129,    59,   263,   254,   404,    95,   257,   407,
     130,   130,   271,   131,    41,   129,   281,     6,   267,   268,
      88,    89,    90,    91,   273,    77,    94,    95,     7,    79,
      77,    77,   285,    59,   389,   131,    79,   128,   131,   409,
     289,   395,    79,     6,   130,   291,   133,     6,   158,    10,
     418,   431,   213,     7,     0,   284,   452,    -1,   395,   308,
      -1,   310,    -1,    -1,    10,    -1,    -1,    -1,     7,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    22,   434,    -1,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    38,    39,   395,    -1,    -1,    -1,    -1,   345,
     346,    -1,    -1,    -1,   353,   355,    85,    86,    87,    88,
      89,    90,    91,    -1,    -1,    94,    95,   418,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,    -1,
     431,    85,    86,    87,    88,    89,    90,    91,    -1,    -1,
      94,    95,    -1,   408,    -1,   395,    85,    86,    87,    88,
      89,    90,    91,    -1,    -1,    94,    95,    -1,   104,    -1,
      -1,    -1,    -1,   412,    -1,    -1,    -1,    -1,   418,    -1,
      -1,    -1,   421,    -1,   423,   121,   122,   426,   124,    -1,
     439,   431,    -1,    -1,    -1,   131,   132,   436,    -1,    -1,
      -1,    -1,    -1,   442,   443,    -1,    -1,    -1,   447,    -1,
      -1,   450,    -1,    -1,    -1,   454,    -1,    -1,    -1,    -1,
      -1,    -1,   158,    -1,   463,    -1,   481,    -1,    -1,    -1,
      -1,    -1,   471,   472,   483,    -1,   475,    -1,    -1,    -1,
      -1,    -1,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   494,     7,    -1,   497,    -1,
      -1,    -1,    -1,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    33,    -1,    35,    -1,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   249,    54,    -1,    -1,    57,    58,    -1,
      60,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    -1,    94,    95,    85,    86,    87,    -1,    -1,
     286,    -1,    -1,    93,    -1,   291,    -1,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,    -1,   125,    -1,   127,    -1,   129,
      -1,    -1,    -1,   133,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   345,
     346,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   355,
       3,     4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    18,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      33,    -1,    35,    -1,    37,    -1,    -1,    -1,    -1,   395,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    54,    -1,    -1,    57,    58,    -1,    -1,    -1,    -1,
      -1,    -1,   418,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   431,    -1,    -1,   434,    -1,
      -1,    -1,    85,    86,    87,    -1,    -1,    -1,    -1,    -1,
      93,    -1,    -1,    -1,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,    -1,   125,    -1,   127,     7,   129,    -1,     0,     1,
     133,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    -1,
      22,    23,    -1,    25,    -1,    -1,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    -1,    -1,
      42,    -1,    44,    45,    -1,    47,    48,    49,    50,    -1,
      -1,    -1,    54,    -1,    -1,    57,    58,    -1,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    -1,    94,    95,    86,    87,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,     7,   125,    -1,   127,     1,   129,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    12,    13,    -1,
      -1,    16,    17,    18,    19,    -1,    -1,    22,    23,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,    33,    -1,
      35,    36,    37,    38,    -1,    -1,    -1,    42,    -1,    -1,
      -1,    -1,    47,    -1,    49,    -1,    -1,    -1,    -1,    54,
      -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    -1,    94,
      95,    86,    87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     7,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,     7,
     125,    -1,   127,    -1,   129,   130,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,    35,    -1,
      37,    -1,    -1,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    54,    94,    95,
      57,    58,    -1,    -1,    -1,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    -1,    94,    95,    85,    86,
      87,    -1,    -1,    -1,    -1,    -1,    93,    -1,    -1,    -1,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,    -1,   125,    -1,
     127,    -1,   129,   130,     3,     4,     5,     6,    -1,     8,
       9,    -1,    -1,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    -1,    22,    23,    -1,    25,    -1,    -1,    -1,
      -1,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      -1,    -1,    -1,    42,    -1,    44,    45,    -1,    47,    48,
      49,    50,    -1,    -1,    -1,    54,    -1,    -1,    57,    58,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    87,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,    -1,   125,    -1,   127,    -1,
     129,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    -1,
      22,    23,    -1,    25,    -1,    -1,    -1,    -1,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    -1,    -1,
      42,    -1,    44,    45,    -1,    -1,    48,    49,    50,    -1,
      -1,    -1,    54,    -1,    -1,    57,    58,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    87,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,    -1,   125,    -1,   127,    -1,   129,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    12,    13,    -1,
      -1,    16,    17,    18,    19,    20,    -1,    22,    23,    -1,
      25,    -1,    -1,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    -1,    -1,    -1,    42,    -1,    44,
      45,    -1,    47,    48,    49,    50,    -1,    -1,    -1,    54,
      -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,    -1,
     125,    -1,   127,    -1,   129,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    12,    13,    14,    -1,    16,    17,
      18,    19,    20,    -1,    22,    23,    -1,    25,    -1,    -1,
      -1,    -1,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    -1,    -1,    -1,    42,    -1,    44,    45,    -1,    -1,
      48,    49,    50,    -1,    -1,    -1,    54,    -1,    -1,    57,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    87,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,    -1,   125,    -1,   127,
      -1,   129,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    12,    13,    -1,    -1,    16,    17,    18,    19,    20,
      -1,    22,    23,    -1,    25,    -1,    -1,    -1,    -1,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    -1,    -1,
      -1,    42,    -1,    44,    45,    -1,    -1,    48,    49,    50,
      -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    87,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,    -1,   125,    -1,   127,    -1,   129,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    12,    13,
      -1,    -1,    16,    17,    18,    19,    -1,    -1,    22,    23,
      -1,    25,    -1,    -1,    -1,    -1,    -1,    31,    -1,    33,
      -1,    35,    36,    37,    38,    -1,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    -1,    -1,    49,    -1,    -1,    -1,    -1,
      54,    -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    87,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
      -1,   125,    -1,   127,   128,   129,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    12,    13,    -1,    -1,    16,
      17,    18,    19,    -1,    -1,    22,    23,    -1,    25,    -1,
      -1,    -1,    -1,    -1,    31,    -1,    33,    -1,    35,    36,
      37,    38,    -1,    -1,    -1,    42,    -1,    -1,    -1,    -1,
      -1,    -1,    49,    -1,    -1,    -1,    -1,    54,    -1,    -1,
      57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,    -1,   125,    -1,
     127,    -1,   129,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    12,    13,    -1,    -1,    16,    17,    18,    19,
      -1,    -1,    22,    23,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    31,    -1,    33,    -1,    35,    36,    37,    38,    -1,
      -1,    -1,    42,    -1,    -1,    -1,    -1,    47,    -1,    49,
      -1,    -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    87,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,    -1,   125,    -1,   127,    -1,   129,
       3,     4,     5,     6,    -1,     8,     9,    -1,    -1,    12,
      13,    -1,    -1,    16,    17,    18,    19,    -1,    -1,    22,
      23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,
      33,    -1,    35,    36,    37,    38,    -1,    -1,    -1,    42,
      -1,    -1,    -1,    -1,    -1,    -1,    49,    -1,    -1,    -1,
      -1,    54,    -1,    -1,    57,    58,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    86,    87,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,    -1,   125,    -1,   127,    -1,   129,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    12,    13,    -1,    -1,
      16,    17,    18,    19,    -1,    -1,    -1,    23,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    31,    -1,    33,    -1,    35,
      36,    37,    -1,    -1,    -1,    -1,    42,    -1,    -1,    -1,
      -1,    -1,    -1,    49,    -1,    -1,    -1,    -1,    54,    -1,
      -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,    -1,   125,
      -1,   127,    -1,   129,     3,     4,     5,     6,    -1,     8,
       9,    -1,    -1,    12,    13,    -1,    -1,    -1,    -1,    18,
      19,    -1,    -1,    -1,    23,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    31,    -1,    33,    -1,    35,    36,    37,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    -1,
      49,    -1,    -1,    -1,    -1,    54,    -1,    -1,    57,    58,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    87,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,    -1,   125,    -1,   127,    -1,
     129,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      12,    13,    -1,    -1,    -1,    -1,    18,    19,    -1,    -1,
      -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,
      -1,    33,    -1,    35,    36,    37,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    47,    -1,    49,    -1,    -1,
      -1,    -1,    54,    -1,    -1,    57,    58,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    87,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,    -1,   125,    -1,   127,    -1,   129,     3,     4,
       5,     6,    -1,     8,     9,    -1,    -1,    12,    13,    -1,
      -1,    -1,    -1,    18,    19,    -1,    -1,    -1,    23,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,    33,    -1,
      35,    36,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    49,    -1,    -1,    -1,    -1,    54,
      -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,    -1,
     125,    -1,   127,    -1,   129,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    33,    -1,    35,    -1,    37,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,    -1,    57,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    86,    87,
      -1,    -1,    -1,    -1,    -1,    93,    -1,    95,    -1,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,    -1,   125,    -1,   127,
      -1,   129,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    33,    -1,    35,    -1,    37,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    54,    -1,    -1,    57,    58,    59,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    85,    86,    87,    -1,    -1,    -1,
      -1,    -1,    93,    -1,    -1,    -1,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,    -1,   125,    -1,   127,    -1,   129,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    18,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,
      -1,    35,    -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      54,    -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    85,    86,    87,    -1,    -1,    -1,    -1,    -1,    93,
      -1,    -1,    -1,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
      -1,   125,    -1,   127,   128,   129,     3,     4,     5,     6,
      -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,    35,    -1,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,    -1,
      57,    58,    -1,    60,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,    86,
      87,    -1,    -1,    -1,    -1,    -1,    93,    -1,    -1,    -1,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,    -1,   125,    -1,
     127,    -1,   129,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    33,    -1,    35,    -1,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    54,    -1,    -1,    57,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    85,    86,    87,    -1,    -1,
      -1,    -1,    -1,    93,    -1,    -1,    -1,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,    -1,   125,    -1,   127,    -1,   129,
       3,     4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    18,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      33,    -1,    35,    -1,    37,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    54,    -1,    -1,    57,    58,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    85,    86,    87,    -1,    -1,    -1,    -1,    -1,
      93,    -1,    -1,    -1,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,    -1,   125,    -1,   127,    -1,   129,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,    35,
      -1,    37,     3,     4,     5,     6,    -1,     8,     9,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,    54,    -1,
      -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    33,    -1,    35,    -1,    37,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    87,    -1,    54,    -1,    -1,    57,    58,    -1,    -1,
      -1,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,     7,   125,
      -1,   127,    -1,   129,    -1,    -1,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,    -1,   116,   117,   118,   119,   120,
     121,   122,   123,     7,   125,    -1,   127,    -1,   129,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    -1,    94,    95,    -1,    -1,    -1,
      -1,    -1,    -1,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,     7,
      94,    95,    -1,    -1,   133,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   133,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    -1,    94,    95,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,     7,    94,    95,    -1,    -1,    -1,
      -1,    -1,   130,   131,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   131,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      -1,    94,    95,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,     7,
      94,    95,    -1,    -1,    -1,    -1,    -1,    -1,   131,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   130,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    -1,    94,    95,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,     7,    94,    95,    -1,    -1,    -1,
      -1,    -1,   130,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     7,    -1,    -1,    -1,    -1,     6,    -1,
      -1,   130,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      -1,    94,    95,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,     7,
      94,    95,    -1,    -1,    -1,    -1,    -1,   130,    -1,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   130,   125,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,     7,    94,    95,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   130,    -1,    -1,     7,    -1,    -1,    -1,    51,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    -1,    94,    95,    56,     7,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,     7,    94,    95,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    56,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    29,    -1,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,     7,    94,    95,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,     7,    94,    95,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      56,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,
      -1,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,     7,    94,    95,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,     7,    94,    95,    -1,    -1,    -1,    -1,
      -1,    51,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,     7,    94,    95,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,     7,
      94,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    -1,
      94,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    -1,    -1,
      88,    89,    90,    91,    92,    -1,    94,    95
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     4,     5,     6,     8,     9,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    22,    23,    25,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    42,
      44,    45,    48,    49,    50,    54,    57,    58,    86,    87,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   125,   127,   129,
     135,   136,   137,   138,   140,   141,   143,   144,   146,   147,
     148,   149,   150,   151,   152,   159,   162,   163,   164,   165,
     168,   170,   171,   172,   175,   176,   183,   184,   187,   188,
     190,   191,   192,   195,   126,    57,    77,   129,   174,     6,
      85,    86,    87,    93,   115,   167,   169,   170,   173,   141,
      42,   164,   164,     6,    57,    95,   169,     6,   162,   169,
       8,   127,    41,   165,   169,     6,    95,   199,   201,     6,
     174,    59,   166,   167,   169,    60,   133,   166,   185,   186,
     168,   168,   129,   128,   166,   169,   169,     0,   126,   139,
      15,   140,     6,    20,    25,    30,    32,    34,    44,    45,
      48,    50,   142,   159,   162,   199,   132,    47,     7,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      94,    95,   174,   174,   174,   174,   174,   174,   129,   174,
     140,   166,   169,   130,   169,   193,   194,    57,   169,   169,
     169,   169,   129,    10,    11,   131,     7,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    94,    95,    47,   165,    47,   165,   129,
     154,   160,   161,   162,   172,    77,   132,    51,   147,   159,
     196,   198,   163,   154,   171,   172,   200,    39,    40,    43,
      95,   154,    59,   131,   157,   133,   133,   186,    60,   131,
      60,   133,   169,   128,   133,   130,   138,   140,   132,    77,
       6,    41,   168,   168,   168,   168,   168,   168,   168,   168,
     168,   168,   168,   168,   168,   200,   169,    59,    56,   130,
     131,   157,   166,   169,   173,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   200,    41,    41,     1,   155,   156,
     158,   159,   162,    77,    59,   131,   157,   169,     6,   127,
       6,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   125,   202,
     169,   199,   126,   128,   131,   197,   199,   169,   169,   200,
     199,     6,   177,   178,   179,   180,   181,   182,   133,   133,
      60,   166,   131,   177,   174,   140,     6,   169,   129,   165,
     130,   131,   169,   129,   169,    59,   131,   165,   165,   130,
     130,   131,   157,   169,    41,   162,   129,     6,   145,   154,
      29,   189,    77,    79,   128,   147,   198,    77,   153,   153,
      77,    59,   131,   157,    79,   131,   157,   186,   169,   128,
     155,   169,   169,    56,   169,   158,   163,   169,   128,   131,
     199,    27,    28,   169,   169,    79,   169,   169,   180,   169,
       6,   133,   130,   130,   130,   130,   169,   130,   130,     6,
     153,   169,   169,   169,    56,   199,    29,    51,   169,   153,
     169
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

  case 37:

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

  case 38:

    { if ((yyvsp[(1) - (2)].vardeclexpr) && (yyvsp[(2) - (2)].expression_v)) (yyvsp[(1) - (2)].vardeclexpr)->addAnnotations(*(yyvsp[(2) - (2)].expression_v));
        if ((yyvsp[(1) - (2)].vardeclexpr))
          (yyval.item) = new VarDeclI((yyloc),(yyvsp[(1) - (2)].vardeclexpr));
        delete (yyvsp[(2) - (2)].expression_v);
      ;}
    break;

  case 39:

    { if ((yyvsp[(1) - (4)].vardeclexpr)) (yyvsp[(1) - (4)].vardeclexpr)->e((yyvsp[(4) - (4)].expression));
        if ((yyvsp[(1) - (4)].vardeclexpr) && (yyvsp[(2) - (4)].expression_v)) (yyvsp[(1) - (4)].vardeclexpr)->addAnnotations(*(yyvsp[(2) - (4)].expression_v));
        if ((yyvsp[(1) - (4)].vardeclexpr))
          (yyval.item) = new VarDeclI((yyloc),(yyvsp[(1) - (4)].vardeclexpr));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 40:

    {
        TypeInst* ti = new TypeInst((yyloc),Type::parsetint());
        ti->setIsEnum(true);
        VarDecl* vd = new VarDecl((yyloc),ti,(yyvsp[(2) - (2)].sValue));
        free((yyvsp[(2) - (2)].sValue));
        (yyval.item) = new VarDeclI((yyloc),vd);
      ;}
    break;

  case 41:

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

  case 42:

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

  case 43:

    { (yyval.expression_v) = new std::vector<Expression*>(); ;}
    break;

  case 44:

    { (yyval.expression_v) = new std::vector<Expression*>();
        (yyval.expression_v)->push_back(new Id((yyloc),(yyvsp[(1) - (1)].sValue),NULL)); free((yyvsp[(1) - (1)].sValue));
      ;}
    break;

  case 45:

    { (yyval.expression_v) = (yyvsp[(1) - (3)].expression_v); if ((yyval.expression_v)) (yyval.expression_v)->push_back(new Id((yyloc),(yyvsp[(3) - (3)].sValue),NULL)); free((yyvsp[(3) - (3)].sValue)); ;}
    break;

  case 46:

    { (yyval.item) = new AssignI((yyloc),(yyvsp[(1) - (3)].sValue),(yyvsp[(3) - (3)].expression));
        free((yyvsp[(1) - (3)].sValue));
      ;}
    break;

  case 47:

    { (yyval.item) = new ConstraintI((yyloc),(yyvsp[(2) - (2)].expression));;}
    break;

  case 48:

    { (yyval.item) = new ConstraintI((yyloc),(yyvsp[(4) - (4)].expression));
        if ((yyval.item) && (yyvsp[(3) - (4)].expression))
          (yyval.item)->cast<ConstraintI>()->e()->ann().add(new Call((yylsp[(2) - (4)]), ASTString("mzn_constraint_name"), {(yyvsp[(3) - (4)].expression)}));
      ;}
    break;

  case 49:

    { (yyval.item) = SolveI::sat((yyloc));
        if ((yyval.item) && (yyvsp[(2) - (3)].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[(2) - (3)].expression_v));
        delete (yyvsp[(2) - (3)].expression_v);
      ;}
    break;

  case 50:

    { (yyval.item) = SolveI::min((yyloc),(yyvsp[(4) - (4)].expression));
        if ((yyval.item) && (yyvsp[(2) - (4)].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[(2) - (4)].expression_v));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 51:

    { (yyval.item) = SolveI::max((yyloc),(yyvsp[(4) - (4)].expression));
        if ((yyval.item) && (yyvsp[(2) - (4)].expression_v)) (yyval.item)->cast<SolveI>()->ann().add(*(yyvsp[(2) - (4)].expression_v));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 52:

    { (yyval.item) = new OutputI((yyloc),(yyvsp[(2) - (2)].expression));;}
    break;

  case 53:

    { if ((yyvsp[(3) - (5)].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[(2) - (5)].sValue),new TypeInst((yyloc),
                                   Type::varbool()),*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression));
        if ((yyval.item) && (yyvsp[(4) - (5)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(4) - (5)].expression_v));
        free((yyvsp[(2) - (5)].sValue));
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
        delete (yyvsp[(4) - (5)].expression_v);
      ;}
    break;

  case 54:

    { if ((yyvsp[(3) - (5)].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[(2) - (5)].sValue),new TypeInst((yyloc),
                                   Type::parbool()),*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression));
        if ((yyval.item) && (yyvsp[(4) - (5)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(4) - (5)].expression_v));
        free((yyvsp[(2) - (5)].sValue));
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
        delete (yyvsp[(4) - (5)].expression_v);
      ;}
    break;

  case 55:

    { if ((yyvsp[(5) - (7)].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[(4) - (7)].sValue),(yyvsp[(2) - (7)].tiexpr),*(yyvsp[(5) - (7)].vardeclexpr_v),(yyvsp[(7) - (7)].expression));
        if ((yyval.item) && (yyvsp[(6) - (7)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(6) - (7)].expression_v));
        free((yyvsp[(4) - (7)].sValue));
        delete (yyvsp[(5) - (7)].vardeclexpr_v);
        delete (yyvsp[(6) - (7)].expression_v);
      ;}
    break;

  case 56:

    { if ((yyvsp[(5) - (8)].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[(3) - (8)].sValue),(yyvsp[(1) - (8)].tiexpr),*(yyvsp[(5) - (8)].vardeclexpr_v),(yyvsp[(8) - (8)].expression));
        if ((yyval.item) && (yyvsp[(7) - (8)].expression_v)) (yyval.item)->cast<FunctionI>()->ann().add(*(yyvsp[(7) - (8)].expression_v));
        free((yyvsp[(3) - (8)].sValue));
        delete (yyvsp[(5) - (8)].vardeclexpr_v);
        delete (yyvsp[(7) - (8)].expression_v);
      ;}
    break;

  case 57:

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

  case 58:

    { TypeInst* ti=new TypeInst((yylsp[(1) - (5)]),Type::ann());
        if ((yyvsp[(3) - (5)].vardeclexpr_v)) (yyval.item) = new FunctionI((yyloc),(yyvsp[(2) - (5)].sValue),ti,*(yyvsp[(3) - (5)].vardeclexpr_v),(yyvsp[(5) - (5)].expression));
        delete (yyvsp[(3) - (5)].vardeclexpr_v);
      ;}
    break;

  case 59:

    { (yyval.expression)=NULL; ;}
    break;

  case 60:

    { (yyval.expression)=(yyvsp[(2) - (2)].expression); ;}
    break;

  case 61:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); ;}
    break;

  case 62:

    { (yyval.vardeclexpr_v)=(yyvsp[(2) - (3)].vardeclexpr_v); ;}
    break;

  case 63:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); ;}
    break;

  case 64:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>(); ;}
    break;

  case 65:

    { (yyval.vardeclexpr_v)=(yyvsp[(1) - (2)].vardeclexpr_v); ;}
    break;

  case 66:

    { (yyval.vardeclexpr_v)=new vector<VarDecl*>();
        if ((yyvsp[(1) - (1)].vardeclexpr)) (yyvsp[(1) - (1)].vardeclexpr)->toplevel(false);
        if ((yyvsp[(1) - (1)].vardeclexpr)) (yyval.vardeclexpr_v)->push_back((yyvsp[(1) - (1)].vardeclexpr)); ;}
    break;

  case 67:

    { (yyval.vardeclexpr_v)=(yyvsp[(1) - (3)].vardeclexpr_v);
        if ((yyvsp[(3) - (3)].vardeclexpr)) (yyvsp[(3) - (3)].vardeclexpr)->toplevel(false);
        if ((yyvsp[(1) - (3)].vardeclexpr_v) && (yyvsp[(3) - (3)].vardeclexpr)) (yyvsp[(1) - (3)].vardeclexpr_v)->push_back((yyvsp[(3) - (3)].vardeclexpr)); ;}
    break;

  case 70:

    { (yyval.vardeclexpr)=(yyvsp[(1) - (1)].vardeclexpr); ;}
    break;

  case 71:

    { if ((yyvsp[(1) - (1)].tiexpr)) (yyval.vardeclexpr)=new VarDecl((yyloc), (yyvsp[(1) - (1)].tiexpr), ""); ;}
    break;

  case 72:

    { if ((yyvsp[(1) - (3)].tiexpr) && (yyvsp[(3) - (3)].sValue)) (yyval.vardeclexpr) = new VarDecl((yyloc), (yyvsp[(1) - (3)].tiexpr), (yyvsp[(3) - (3)].sValue));
        free((yyvsp[(3) - (3)].sValue));
      ;}
    break;

  case 73:

    { (yyval.tiexpr_v)=(yyvsp[(1) - (2)].tiexpr_v); ;}
    break;

  case 74:

    { (yyval.tiexpr_v)=new vector<TypeInst*>(); (yyval.tiexpr_v)->push_back((yyvsp[(1) - (1)].tiexpr)); ;}
    break;

  case 75:

    { (yyval.tiexpr_v)=(yyvsp[(1) - (3)].tiexpr_v); if ((yyvsp[(1) - (3)].tiexpr_v) && (yyvsp[(3) - (3)].tiexpr)) (yyvsp[(1) - (3)].tiexpr_v)->push_back((yyvsp[(3) - (3)].tiexpr)); ;}
    break;

  case 77:

    {
        (yyval.tiexpr) = (yyvsp[(6) - (6)].tiexpr);
        if ((yyval.tiexpr) && (yyvsp[(3) - (6)].tiexpr_v)) (yyval.tiexpr)->setRanges(*(yyvsp[(3) - (6)].tiexpr_v));
        delete (yyvsp[(3) - (6)].tiexpr_v);
      ;}
    break;

  case 78:

    {
        (yyval.tiexpr) = (yyvsp[(3) - (3)].tiexpr);
        std::vector<TypeInst*> ti(1);
        ti[0] = new TypeInst((yyloc),Type::parint());
        if ((yyval.tiexpr)) (yyval.tiexpr)->setRanges(ti);
      ;}
    break;

  case 79:

    { (yyval.tiexpr) = (yyvsp[(1) - (1)].tiexpr);
      ;}
    break;

  case 80:

    { (yyval.tiexpr) = (yyvsp[(2) - (2)].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 81:

    { (yyval.tiexpr) = (yyvsp[(3) - (3)].tiexpr);
        if ((yyval.tiexpr) && (yyvsp[(2) - (3)].bValue)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 82:

    { (yyval.tiexpr) = (yyvsp[(3) - (3)].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.ti(Type::TI_VAR);
          if ((yyvsp[(2) - (3)].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 83:

    { (yyval.tiexpr) = (yyvsp[(4) - (4)].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.st(Type::ST_SET);
          if ((yyvsp[(1) - (4)].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 84:

    { (yyval.tiexpr) = (yyvsp[(5) - (5)].tiexpr);
        if ((yyval.tiexpr)) {
          Type tt = (yyval.tiexpr)->type();
          tt.st(Type::ST_SET);
          if ((yyvsp[(2) - (5)].bValue)) tt.ot(Type::OT_OPTIONAL);
          (yyval.tiexpr)->type(tt);
        }
      ;}
    break;

  case 85:

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

  case 86:

    { (yyval.bValue) = false; ;}
    break;

  case 87:

    { (yyval.bValue) = true; ;}
    break;

  case 88:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parint()); ;}
    break;

  case 89:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parbool()); ;}
    break;

  case 90:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parfloat()); ;}
    break;

  case 91:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parstring()); ;}
    break;

  case 92:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::ann()); ;}
    break;

  case 93:

    { if ((yyvsp[(1) - (1)].expression)) (yyval.tiexpr) = new TypeInst((yyloc),Type(),(yyvsp[(1) - (1)].expression)); ;}
    break;

  case 94:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::top(),
                         new TIId((yyloc), (yyvsp[(1) - (1)].sValue)));
        free((yyvsp[(1) - (1)].sValue));
      ;}
    break;

  case 95:

    { (yyval.tiexpr) = new TypeInst((yyloc),Type::parint(),
          new TIId((yyloc), (yyvsp[(1) - (1)].sValue)));
          free((yyvsp[(1) - (1)].sValue));
      ;}
    break;

  case 97:

    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].expression)); ;}
    break;

  case 98:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); if ((yyval.expression_v) && (yyvsp[(3) - (3)].expression)) (yyval.expression_v)->push_back((yyvsp[(3) - (3)].expression)); ;}
    break;

  case 100:

    { if ((yyvsp[(1) - (3)].expression) && (yyvsp[(3) - (3)].expression)) (yyvsp[(1) - (3)].expression)->addAnnotation((yyvsp[(3) - (3)].expression)); (yyval.expression)=(yyvsp[(1) - (3)].expression); ;}
    break;

  case 101:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_UNION, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 102:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 103:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SYMDIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 104:

    { if ((yyvsp[(1) - (3)].expression)==NULL || (yyvsp[(3) - (3)].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[(1) - (3)].expression)->isa<IntLit>() && (yyvsp[(3) - (3)].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[(1) - (3)].expression)->cast<IntLit>()->v(),(yyvsp[(3) - (3)].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DOTDOT, (yyvsp[(3) - (3)].expression));
        }
      ;}
    break;

  case 105:

    { if ((yyvsp[(3) - (6)].expression)==NULL || (yyvsp[(5) - (6)].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[(3) - (6)].expression)->isa<IntLit>() && (yyvsp[(5) - (6)].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[(3) - (6)].expression)->cast<IntLit>()->v(),(yyvsp[(5) - (6)].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[(3) - (6)].expression), BOT_DOTDOT, (yyvsp[(5) - (6)].expression));
        }
      ;}
    break;

  case 106:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_INTERSECT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 107:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUSPLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 108:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 109:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MINUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 110:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MULT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 111:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 112:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IDIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 113:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MOD, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 114:

    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=new Call((yyloc), (yyvsp[(2) - (3)].sValue), args);
        free((yyvsp[(2) - (3)].sValue));
      ;}
    break;

  case 115:

    { (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 116:

    { if ((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<IntLit>()) {
          (yyval.expression) = IntLit::a(-(yyvsp[(2) - (2)].expression)->cast<IntLit>()->v());
        } else if ((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<FloatLit>()) {
          (yyval.expression) = FloatLit::a(-(yyvsp[(2) - (2)].expression)->cast<FloatLit>()->v());
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_MINUS, (yyvsp[(2) - (2)].expression));
        }
      ;}
    break;

  case 118:

    { if ((yyvsp[(1) - (3)].expression) && (yyvsp[(3) - (3)].expression)) (yyvsp[(1) - (3)].expression)->addAnnotation((yyvsp[(3) - (3)].expression)); (yyval.expression)=(yyvsp[(1) - (3)].expression); ;}
    break;

  case 119:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_EQUIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 120:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IMPL, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 121:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_RIMPL, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 122:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_OR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 123:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_XOR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 124:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_AND, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 125:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_LE, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 126:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_GR, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 127:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_LQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 128:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_GQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 129:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_EQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 130:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_NQ, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 131:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IN, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 132:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SUBSET, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 133:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SUPERSET, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 134:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_UNION, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 135:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 136:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_SYMDIFF, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 137:

    { (yyval.expression)=new SetLit((yyloc), IntSetVal::a(-IntVal::infinity(),IntVal::infinity())); ;}
    break;

  case 138:

    { if ((yyvsp[(2) - (2)].expression)==NULL) {
        (yyval.expression) = NULL;
      } else if ((yyvsp[(2) - (2)].expression)->isa<IntLit>()) {
        (yyval.expression)=new SetLit((yyloc), IntSetVal::a(-IntVal::infinity(),(yyvsp[(2) - (2)].expression)->cast<IntLit>()->v()));
      } else {
        (yyval.expression)=new BinOp((yyloc), IntLit::a(-IntVal::infinity()), BOT_DOTDOT, (yyvsp[(2) - (2)].expression));
        }
      ;}
    break;

  case 139:

    { if ((yyvsp[(1) - (2)].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[(1) - (2)].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[(1) - (2)].expression)->cast<IntLit>()->v(),IntVal::infinity()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (2)].expression), BOT_DOTDOT, IntLit::a(IntVal::infinity()));
        }
      ;}
    break;

  case 140:

    { if ((yyvsp[(1) - (3)].expression)==NULL || (yyvsp[(3) - (3)].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[(1) - (3)].expression)->isa<IntLit>() && (yyvsp[(3) - (3)].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[(1) - (3)].expression)->cast<IntLit>()->v(),(yyvsp[(3) - (3)].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DOTDOT, (yyvsp[(3) - (3)].expression));
        }
      ;}
    break;

  case 141:

    { if ((yyvsp[(3) - (6)].expression)==NULL || (yyvsp[(5) - (6)].expression)==NULL) {
          (yyval.expression) = NULL;
        } else if ((yyvsp[(3) - (6)].expression)->isa<IntLit>() && (yyvsp[(5) - (6)].expression)->isa<IntLit>()) {
          (yyval.expression)=new SetLit((yyloc), IntSetVal::a((yyvsp[(3) - (6)].expression)->cast<IntLit>()->v(),(yyvsp[(5) - (6)].expression)->cast<IntLit>()->v()));
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[(3) - (6)].expression), BOT_DOTDOT, (yyvsp[(5) - (6)].expression));
        }
      ;}
    break;

  case 142:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_INTERSECT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 143:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUSPLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 144:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_PLUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 145:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MINUS, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 146:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MULT, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 147:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_DIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 148:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_IDIV, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 149:

    { (yyval.expression)=new BinOp((yyloc), (yyvsp[(1) - (3)].expression), BOT_MOD, (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 150:

    { vector<Expression*> args;
        args.push_back((yyvsp[(1) - (3)].expression)); args.push_back((yyvsp[(3) - (3)].expression));
        (yyval.expression)=new Call((yyloc), (yyvsp[(2) - (3)].sValue), args);
        free((yyvsp[(2) - (3)].sValue));
      ;}
    break;

  case 151:

    { (yyval.expression)=new UnOp((yyloc), UOT_NOT, (yyvsp[(2) - (2)].expression)); ;}
    break;

  case 152:

    { if (((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<IntLit>()) || ((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<FloatLit>())) {
          (yyval.expression) = (yyvsp[(2) - (2)].expression);
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_PLUS, (yyvsp[(2) - (2)].expression));
        }
      ;}
    break;

  case 153:

    { if ((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<IntLit>()) {
          (yyval.expression) = IntLit::a(-(yyvsp[(2) - (2)].expression)->cast<IntLit>()->v());
        } else if ((yyvsp[(2) - (2)].expression) && (yyvsp[(2) - (2)].expression)->isa<FloatLit>()) {
          (yyval.expression) = FloatLit::a(-(yyvsp[(2) - (2)].expression)->cast<FloatLit>()->v());
        } else {
          (yyval.expression)=new UnOp((yyloc), UOT_MINUS, (yyvsp[(2) - (2)].expression));
        }
      ;}
    break;

  case 154:

    { (yyval.expression)=(yyvsp[(1) - (1)].expression); ;}
    break;

  case 155:

    { (yyval.expression)=(yyvsp[(1) - (1)].expression); ;}
    break;

  case 156:

    { (yyval.expression)=(yyvsp[(2) - (3)].expression); ;}
    break;

  case 157:

    { if ((yyvsp[(4) - (4)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(2) - (4)].expression), *(yyvsp[(4) - (4)].expression_vv)); delete (yyvsp[(4) - (4)].expression_vv); ;}
    break;

  case 158:

    { (yyval.expression)=new Id((yyloc), (yyvsp[(1) - (1)].sValue), NULL); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 159:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), new Id((yylsp[(1) - (2)]),(yyvsp[(1) - (2)].sValue),NULL), *(yyvsp[(2) - (2)].expression_vv));
        free((yyvsp[(1) - (2)].sValue)); delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 160:

    { (yyval.expression)=new AnonVar((yyloc)); ;}
    break;

  case 161:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), new AnonVar((yyloc)), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 162:

    { (yyval.expression)=constants().boollit(((yyvsp[(1) - (1)].iValue)!=0)); ;}
    break;

  case 163:

    { (yyval.expression)=IntLit::a((yyvsp[(1) - (1)].iValue)); ;}
    break;

  case 164:

    { (yyval.expression)=IntLit::a(IntVal::infinity()); ;}
    break;

  case 165:

    { (yyval.expression)=FloatLit::a((yyvsp[(1) - (1)].dValue)); ;}
    break;

  case 166:

    { (yyval.expression)=constants().absent; ;}
    break;

  case 168:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 170:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 172:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 174:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 176:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 178:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 181:

    { if ((yyvsp[(2) - (2)].expression_vv)) (yyval.expression)=createArrayAccess((yyloc), (yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].expression_vv));
        delete (yyvsp[(2) - (2)].expression_vv); ;}
    break;

  case 182:

    { (yyval.expression)=new StringLit((yyloc), (yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 183:

    { (yyval.expression)=new BinOp((yyloc), new StringLit((yyloc), (yyvsp[(1) - (2)].sValue)), BOT_PLUSPLUS, (yyvsp[(2) - (2)].expression));
        free((yyvsp[(1) - (2)].sValue));
      ;}
    break;

  case 184:

    { if ((yyvsp[(1) - (2)].expression_v)) (yyval.expression)=new BinOp((yyloc), new Call((yyloc), ASTString("format"), *(yyvsp[(1) - (2)].expression_v)), BOT_PLUSPLUS, new StringLit((yyloc),(yyvsp[(2) - (2)].sValue)));
        free((yyvsp[(2) - (2)].sValue));
        delete (yyvsp[(1) - (2)].expression_v);
      ;}
    break;

  case 185:

    { if ((yyvsp[(1) - (3)].expression_v)) (yyval.expression)=new BinOp((yyloc), new Call((yyloc), ASTString("format"), *(yyvsp[(1) - (3)].expression_v)), BOT_PLUSPLUS,
                             new BinOp((yyloc), new StringLit((yyloc),(yyvsp[(2) - (3)].sValue)), BOT_PLUSPLUS, (yyvsp[(3) - (3)].expression)));
        free((yyvsp[(2) - (3)].sValue));
        delete (yyvsp[(1) - (3)].expression_v);
      ;}
    break;

  case 186:

    { (yyval.expression_vv)=new std::vector<std::vector<Expression*> >();
        if ((yyvsp[(2) - (3)].expression_v)) {
          (yyval.expression_vv)->push_back(*(yyvsp[(2) - (3)].expression_v));
          delete (yyvsp[(2) - (3)].expression_v);
        }
      ;}
    break;

  case 187:

    { (yyval.expression_vv)=(yyvsp[(1) - (4)].expression_vv);
        if ((yyval.expression_vv) && (yyvsp[(3) - (4)].expression_v)) {
          (yyval.expression_vv)->push_back(*(yyvsp[(3) - (4)].expression_v));
          delete (yyvsp[(3) - (4)].expression_v);
        }
      ;}
    break;

  case 188:

    { (yyval.expression) = new SetLit((yyloc), std::vector<Expression*>()); ;}
    break;

  case 189:

    { if ((yyvsp[(2) - (3)].expression_v)) (yyval.expression) = new SetLit((yyloc), *(yyvsp[(2) - (3)].expression_v));
        delete (yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 190:

    { if ((yyvsp[(4) - (5)].generators)) (yyval.expression) = new Comprehension((yyloc), (yyvsp[(2) - (5)].expression), *(yyvsp[(4) - (5)].generators), true);
        delete (yyvsp[(4) - (5)].generators);
      ;}
    break;

  case 191:

    { if ((yyvsp[(1) - (1)].generator_v)) (yyval.generators)=new Generators; (yyval.generators)->_g = *(yyvsp[(1) - (1)].generator_v); delete (yyvsp[(1) - (1)].generator_v); ;}
    break;

  case 193:

    { (yyval.generator_v)=new std::vector<Generator>; if ((yyvsp[(1) - (1)].generator)) (yyval.generator_v)->push_back(*(yyvsp[(1) - (1)].generator)); delete (yyvsp[(1) - (1)].generator); ;}
    break;

  case 194:

    { (yyval.generator_v)=(yyvsp[(1) - (3)].generator_v); if ((yyval.generator_v) && (yyvsp[(3) - (3)].generator)) (yyval.generator_v)->push_back(*(yyvsp[(3) - (3)].generator)); delete (yyvsp[(3) - (3)].generator); ;}
    break;

  case 195:

    { if ((yyvsp[(1) - (3)].string_v) && (yyvsp[(3) - (3)].expression)) (yyval.generator)=new Generator(*(yyvsp[(1) - (3)].string_v),(yyvsp[(3) - (3)].expression),NULL); else (yyval.generator)=NULL; delete (yyvsp[(1) - (3)].string_v); ;}
    break;

  case 196:

    { if ((yyvsp[(1) - (5)].string_v) && (yyvsp[(3) - (5)].expression)) (yyval.generator)=new Generator(*(yyvsp[(1) - (5)].string_v),(yyvsp[(3) - (5)].expression),(yyvsp[(5) - (5)].expression)); else (yyval.generator)=NULL; delete (yyvsp[(1) - (5)].string_v); ;}
    break;

  case 197:

    { if ((yyvsp[(3) - (3)].expression)) (yyval.generator)=new Generator({(yyvsp[(1) - (3)].sValue)},NULL,(yyvsp[(3) - (3)].expression)); else (yyval.generator)=NULL; free((yyvsp[(1) - (3)].sValue)); ;}
    break;

  case 199:

    { (yyval.string_v)=new std::vector<std::string>; (yyval.string_v)->push_back((yyvsp[(1) - (1)].sValue)); free((yyvsp[(1) - (1)].sValue)); ;}
    break;

  case 200:

    { (yyval.string_v)=(yyvsp[(1) - (3)].string_v); if ((yyval.string_v) && (yyvsp[(3) - (3)].sValue)) (yyval.string_v)->push_back((yyvsp[(3) - (3)].sValue)); free((yyvsp[(3) - (3)].sValue)); ;}
    break;

  case 201:

    { (yyval.expression)=new ArrayLit((yyloc), std::vector<MiniZinc::Expression*>()); ;}
    break;

  case 202:

    { if ((yyvsp[(2) - (3)].expression_v)) (yyval.expression)=new ArrayLit((yyloc), *(yyvsp[(2) - (3)].expression_v)); delete (yyvsp[(2) - (3)].expression_v); ;}
    break;

  case 203:

    { (yyval.expression)=new ArrayLit((yyloc), std::vector<std::vector<Expression*> >()); ;}
    break;

  case 204:

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

  case 205:

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

  case 206:

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

  case 207:

    { (yyval.expression_vvv)=new std::vector<std::vector<std::vector<MiniZinc::Expression*> > >;
      ;}
    break;

  case 208:

    { (yyval.expression_vvv)=new std::vector<std::vector<std::vector<MiniZinc::Expression*> > >;
        if ((yyvsp[(2) - (3)].expression_vv)) (yyval.expression_vvv)->push_back(*(yyvsp[(2) - (3)].expression_vv));
        delete (yyvsp[(2) - (3)].expression_vv);
      ;}
    break;

  case 209:

    { (yyval.expression_vvv)=(yyvsp[(1) - (5)].expression_vvv);
        if ((yyval.expression_vvv) && (yyvsp[(4) - (5)].expression_vv)) (yyval.expression_vvv)->push_back(*(yyvsp[(4) - (5)].expression_vv));
        delete (yyvsp[(4) - (5)].expression_vv);
      ;}
    break;

  case 210:

    { (yyval.expression_vv)=new std::vector<std::vector<MiniZinc::Expression*> >;
        if ((yyvsp[(1) - (1)].expression_v)) (yyval.expression_vv)->push_back(*(yyvsp[(1) - (1)].expression_v));
        delete (yyvsp[(1) - (1)].expression_v);
      ;}
    break;

  case 211:

    { (yyval.expression_vv)=(yyvsp[(1) - (3)].expression_vv); if ((yyval.expression_vv) && (yyvsp[(3) - (3)].expression_v)) (yyval.expression_vv)->push_back(*(yyvsp[(3) - (3)].expression_v)); delete (yyvsp[(3) - (3)].expression_v); ;}
    break;

  case 212:

    { if ((yyvsp[(4) - (5)].generators)) (yyval.expression)=new Comprehension((yyloc), (yyvsp[(2) - (5)].expression), *(yyvsp[(4) - (5)].generators), false);
        delete (yyvsp[(4) - (5)].generators);
      ;}
    break;

  case 213:

    {
        std::vector<Expression*> iexps;
        iexps.push_back((yyvsp[(2) - (5)].expression));
        iexps.push_back((yyvsp[(4) - (5)].expression));
        (yyval.expression)=new ITE((yyloc), iexps, NULL);
      ;}
    break;

  case 214:

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

  case 215:

    { (yyval.expression_v)=new std::vector<MiniZinc::Expression*>; ;}
    break;

  case 216:

    { (yyval.expression_v)=(yyvsp[(1) - (5)].expression_v); if ((yyval.expression_v) && (yyvsp[(3) - (5)].expression) && (yyvsp[(5) - (5)].expression)) { (yyval.expression_v)->push_back((yyvsp[(3) - (5)].expression)); (yyval.expression_v)->push_back((yyvsp[(5) - (5)].expression)); } ;}
    break;

  case 217:

    { (yyval.iValue)=BOT_EQUIV; ;}
    break;

  case 218:

    { (yyval.iValue)=BOT_IMPL; ;}
    break;

  case 219:

    { (yyval.iValue)=BOT_RIMPL; ;}
    break;

  case 220:

    { (yyval.iValue)=BOT_OR; ;}
    break;

  case 221:

    { (yyval.iValue)=BOT_XOR; ;}
    break;

  case 222:

    { (yyval.iValue)=BOT_AND; ;}
    break;

  case 223:

    { (yyval.iValue)=BOT_LE; ;}
    break;

  case 224:

    { (yyval.iValue)=BOT_GR; ;}
    break;

  case 225:

    { (yyval.iValue)=BOT_LQ; ;}
    break;

  case 226:

    { (yyval.iValue)=BOT_GQ; ;}
    break;

  case 227:

    { (yyval.iValue)=BOT_EQ; ;}
    break;

  case 228:

    { (yyval.iValue)=BOT_NQ; ;}
    break;

  case 229:

    { (yyval.iValue)=BOT_IN; ;}
    break;

  case 230:

    { (yyval.iValue)=BOT_SUBSET; ;}
    break;

  case 231:

    { (yyval.iValue)=BOT_SUPERSET; ;}
    break;

  case 232:

    { (yyval.iValue)=BOT_UNION; ;}
    break;

  case 233:

    { (yyval.iValue)=BOT_DIFF; ;}
    break;

  case 234:

    { (yyval.iValue)=BOT_SYMDIFF; ;}
    break;

  case 235:

    { (yyval.iValue)=BOT_PLUS; ;}
    break;

  case 236:

    { (yyval.iValue)=BOT_MINUS; ;}
    break;

  case 237:

    { (yyval.iValue)=BOT_MULT; ;}
    break;

  case 238:

    { (yyval.iValue)=BOT_DIV; ;}
    break;

  case 239:

    { (yyval.iValue)=BOT_IDIV; ;}
    break;

  case 240:

    { (yyval.iValue)=BOT_MOD; ;}
    break;

  case 241:

    { (yyval.iValue)=BOT_INTERSECT; ;}
    break;

  case 242:

    { (yyval.iValue)=BOT_PLUSPLUS; ;}
    break;

  case 243:

    { (yyval.iValue)=-1; ;}
    break;

  case 244:

    { if ((yyvsp[(1) - (6)].iValue)==-1) {
          (yyval.expression)=NULL;
          yyerror(&(yylsp[(3) - (6)]), parm, "syntax error, unary operator with two arguments");
        } else {
          (yyval.expression)=new BinOp((yyloc), (yyvsp[(3) - (6)].expression),static_cast<BinOpType>((yyvsp[(1) - (6)].iValue)),(yyvsp[(5) - (6)].expression));
        }
      ;}
    break;

  case 245:

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

  case 246:

    { (yyval.expression)=new Call((yyloc), (yyvsp[(1) - (3)].sValue), std::vector<Expression*>()); free((yyvsp[(1) - (3)].sValue)); ;}
    break;

  case 248:

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

  case 249:

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
                    if ((*(yyvsp[(3) - (7)].expression_p))[i].second) {
                      ParserLocation loc = (*(yyvsp[(3) - (7)].expression_p))[i].second->loc().parserLocation();
                      yyerror(&loc, parm, "illegal where expression in generator call");
                    }
                    ids.push_back(id);
                    gens.push_back(Generator(ids,NULL,boe->rhs()));
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

  case 251:

    { (yyval.expression_p)=new vector<pair<Expression*,Expression*> >;
        (yyval.expression_p)->push_back(pair<Expression*,Expression*>((yyvsp[(1) - (1)].expression),NULL)); ;}
    break;

  case 252:

    { (yyval.expression_p)=new vector<pair<Expression*,Expression*> >;
        (yyval.expression_p)->push_back(pair<Expression*,Expression*>((yyvsp[(1) - (3)].expression),(yyvsp[(3) - (3)].expression))); ;}
    break;

  case 253:

    { (yyval.expression_p)=(yyvsp[(1) - (3)].expression_p); if ((yyval.expression_p) && (yyvsp[(3) - (3)].expression)) (yyval.expression_p)->push_back(pair<Expression*,Expression*>((yyvsp[(3) - (3)].expression),NULL)); ;}
    break;

  case 254:

    { (yyval.expression_p)=(yyvsp[(1) - (5)].expression_p); if ((yyval.expression_p) && (yyvsp[(3) - (5)].expression) && (yyvsp[(5) - (5)].expression)) (yyval.expression_p)->push_back(pair<Expression*,Expression*>((yyvsp[(3) - (5)].expression),(yyvsp[(5) - (5)].expression))); ;}
    break;

  case 255:

    { if ((yyvsp[(3) - (6)].expression_v) && (yyvsp[(6) - (6)].expression)) {
          (yyval.expression)=new Let((yyloc), *(yyvsp[(3) - (6)].expression_v), (yyvsp[(6) - (6)].expression)); delete (yyvsp[(3) - (6)].expression_v);
        } else {
          (yyval.expression)=NULL;
        }
      ;}
    break;

  case 256:

    { if ((yyvsp[(3) - (7)].expression_v) && (yyvsp[(7) - (7)].expression)) {
          (yyval.expression)=new Let((yyloc), *(yyvsp[(3) - (7)].expression_v), (yyvsp[(7) - (7)].expression)); delete (yyvsp[(3) - (7)].expression_v);
        } else {
          (yyval.expression)=NULL;
        }
      ;}
    break;

  case 257:

    { (yyval.expression_v)=new vector<Expression*>; (yyval.expression_v)->push_back((yyvsp[(1) - (1)].vardeclexpr)); ;}
    break;

  case 258:

    { (yyval.expression_v)=new vector<Expression*>;
        if ((yyvsp[(1) - (1)].item)) {
          ConstraintI* ce = (yyvsp[(1) - (1)].item)->cast<ConstraintI>();
          (yyval.expression_v)->push_back(ce->e());
          ce->e(NULL);
        }
      ;}
    break;

  case 259:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); if ((yyval.expression_v) && (yyvsp[(3) - (3)].vardeclexpr)) (yyval.expression_v)->push_back((yyvsp[(3) - (3)].vardeclexpr)); ;}
    break;

  case 260:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v);
        if ((yyval.expression_v) && (yyvsp[(3) - (3)].item)) {
          ConstraintI* ce = (yyvsp[(3) - (3)].item)->cast<ConstraintI>();
          (yyval.expression_v)->push_back(ce->e());
          ce->e(NULL);
        }
      ;}
    break;

  case 263:

    { (yyval.vardeclexpr) = (yyvsp[(1) - (2)].vardeclexpr);
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->toplevel(false);
        if ((yyval.vardeclexpr) && (yyvsp[(2) - (2)].expression_v)) (yyval.vardeclexpr)->addAnnotations(*(yyvsp[(2) - (2)].expression_v));
        delete (yyvsp[(2) - (2)].expression_v);
      ;}
    break;

  case 264:

    { if ((yyvsp[(1) - (4)].vardeclexpr)) (yyvsp[(1) - (4)].vardeclexpr)->e((yyvsp[(4) - (4)].expression));
        (yyval.vardeclexpr) = (yyvsp[(1) - (4)].vardeclexpr);
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->loc((yyloc));
        if ((yyval.vardeclexpr)) (yyval.vardeclexpr)->toplevel(false);
        if ((yyval.vardeclexpr) && (yyvsp[(2) - (4)].expression_v)) (yyval.vardeclexpr)->addAnnotations(*(yyvsp[(2) - (4)].expression_v));
        delete (yyvsp[(2) - (4)].expression_v);
      ;}
    break;

  case 265:

    { (yyval.expression_v)=NULL; ;}
    break;

  case 267:

    { (yyval.expression) = (yyvsp[(1) - (1)].expression); ;}
    break;

  case 268:

    { (yyval.expression) = new Call((yylsp[(1) - (1)]), ASTString("mzn_expression_name"), {(yyvsp[(1) - (1)].expression)}); ;}
    break;

  case 269:

    { (yyval.expression_v)=new std::vector<Expression*>(1);
        (*(yyval.expression_v))[0] = (yyvsp[(2) - (2)].expression);
      ;}
    break;

  case 270:

    { (yyval.expression_v)=(yyvsp[(1) - (3)].expression_v); if ((yyval.expression_v)) (yyval.expression_v)->push_back((yyvsp[(3) - (3)].expression)); ;}
    break;

  case 271:

    { (yyval.sValue)=(yyvsp[(1) - (1)].sValue); ;}
    break;

  case 272:

    { (yyval.sValue)=strdup("'<->'"); ;}
    break;

  case 273:

    { (yyval.sValue)=strdup("'->'"); ;}
    break;

  case 274:

    { (yyval.sValue)=strdup("'<-'"); ;}
    break;

  case 275:

    { (yyval.sValue)=strdup("'\\/'"); ;}
    break;

  case 276:

    { (yyval.sValue)=strdup("'xor'"); ;}
    break;

  case 277:

    { (yyval.sValue)=strdup("'/\\'"); ;}
    break;

  case 278:

    { (yyval.sValue)=strdup("'<'"); ;}
    break;

  case 279:

    { (yyval.sValue)=strdup("'>'"); ;}
    break;

  case 280:

    { (yyval.sValue)=strdup("'<='"); ;}
    break;

  case 281:

    { (yyval.sValue)=strdup("'>='"); ;}
    break;

  case 282:

    { (yyval.sValue)=strdup("'='"); ;}
    break;

  case 283:

    { (yyval.sValue)=strdup("'!='"); ;}
    break;

  case 284:

    { (yyval.sValue)=strdup("'in'"); ;}
    break;

  case 285:

    { (yyval.sValue)=strdup("'subset'"); ;}
    break;

  case 286:

    { (yyval.sValue)=strdup("'superset'"); ;}
    break;

  case 287:

    { (yyval.sValue)=strdup("'union'"); ;}
    break;

  case 288:

    { (yyval.sValue)=strdup("'diff'"); ;}
    break;

  case 289:

    { (yyval.sValue)=strdup("'symdiff'"); ;}
    break;

  case 290:

    { (yyval.sValue)=strdup("'..'"); ;}
    break;

  case 291:

    { (yyval.sValue)=strdup("'+'"); ;}
    break;

  case 292:

    { (yyval.sValue)=strdup("'-'"); ;}
    break;

  case 293:

    { (yyval.sValue)=strdup("'*'"); ;}
    break;

  case 294:

    { (yyval.sValue)=strdup("'/'"); ;}
    break;

  case 295:

    { (yyval.sValue)=strdup("'div'"); ;}
    break;

  case 296:

    { (yyval.sValue)=strdup("'mod'"); ;}
    break;

  case 297:

    { (yyval.sValue)=strdup("'intersect'"); ;}
    break;

  case 298:

    { (yyval.sValue)=strdup("'not'"); ;}
    break;

  case 299:

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



