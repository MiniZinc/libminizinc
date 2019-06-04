/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* This (main) file coordinates flattening and solving.
 * The corresponding modules are flexibly plugged in
 * as derived classes, prospectively from DLLs.
 * A flattening module should provide MinZinc::GetFlattener()
 * A solving module should provide an object of a class derived from SolverFactory.
 * Need to get more flexible for multi-pass & multi-solving stuff  TODO
 */

#include <minizinc/parser.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/json_parser.hh>

#include <minizinc/prettyprinter.hh>

#include <fstream>

using namespace std;

int mzn_yylex_init (void** scanner);
void mzn_yyset_extra (void* user_defined ,void* yyscanner );
int mzn_yyparse(void*);
int mzn_yylex_destroy (void* scanner);

namespace {
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
        contents = MiniZinc::FileUtils::decodeBase64(contents);
        MiniZinc::FileUtils::inflateString(contents);
      }
      return(contents);
    }
    throw(errno);
  }
}

namespace MiniZinc {

  void parse(Env& env,
             Model*& model,
             const vector<string>& filenames,
             const vector<string>& datafiles,
             const std::string& modelString,
             const std::string& modelStringName,
             const vector<string>& ip,
             bool ignoreStdlib,
             bool parseDocComments,
             bool verbose,
             ostream& err,
             std::vector<SyntaxError>& syntaxErrors) {
    
    vector<string> includePaths;
    for (unsigned int i=0; i<ip.size(); i++)
      includePaths.push_back(ip[i]);
    
    vector<ParseWorkItem> files;
    map<string,Model*> seenModels;
    
    string workingDir = FileUtils::working_directory();

    if (filenames.size() > 0) {
      GCLock lock;
      model->setFilename(FileUtils::base_name(filenames[0]));
      if (FileUtils::is_absolute(filenames[0])) {
        files.push_back(ParseWorkItem(model,"",filenames[0]));
      } else {
        files.push_back(ParseWorkItem(model,"",workingDir+"/"+filenames[0]));
      }
      
      
      for (unsigned int i=1; i<filenames.size(); i++) {
        GCLock lock;
        string fullName = filenames[i];
        string baseName = FileUtils::base_name(filenames[i]);
        if (!FileUtils::is_absolute(fullName))
          fullName = workingDir+"/"+fullName;
        bool isFzn = (baseName.compare(baseName.length()-4,4,".fzn")==0);
        if (isFzn) {
          files.push_back(ParseWorkItem(model,"",fullName));
        } else {
          Model* includedModel = new Model;
          includedModel->setFilename(baseName);
          files.push_back(ParseWorkItem(includedModel,"",fullName));
          seenModels.insert(pair<string,Model*>(baseName,includedModel));
          Location loc(ASTString(filenames[i]),0,0,0,0);
          IncludeI* inc = new IncludeI(loc,includedModel->filename());
          inc->m(includedModel,true);
          model->addItem(inc);
        }
      }
      if (!modelString.empty()) {
        Model* includedModel = new Model;
        includedModel->setFilename(modelStringName);
        files.push_back(ParseWorkItem(includedModel,modelString,modelStringName,true));
        seenModels.insert(pair<string,Model*>(modelStringName,includedModel));
        Location loc(ASTString(modelStringName),0,0,0,0);
        IncludeI* inc = new IncludeI(loc,includedModel->filename());
        inc->m(includedModel,true);
        model->addItem(inc);
      }
    } else if (!modelString.empty()) {
      GCLock lock;
      model->setFilename(modelStringName);
      files.push_back(ParseWorkItem(model,modelString,modelStringName,true));
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
      bool isModelString = np.isModelString;
      string f(np.fileName);
      files.pop_back();
      
      std::string s;
      std::string fullname;
      bool isFzn;
      if (!isModelString) {
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
          unsigned int i=0;
          for (; i<includePaths.size(); i++) {
            fullname = includePaths[i]+"/"+f;
            if (FileUtils::file_exists(fullname)) {
              file.open(fullname.c_str(), std::ios::binary);
              if (file.is_open())
                break;
            }
          }
          if (file.is_open() && i<includePaths.size()-1 &&
              parentPath==workingDir &&
              FileUtils::file_path(includePaths[i],workingDir)!=FileUtils::file_path(workingDir) &&
              FileUtils::file_exists(workingDir+"/"+f)) {
            err << "Warning: file " << f << " included from library, but also exists in current working directory" << endl;
          }
          includePaths.pop_back();
        }
        if (!file.is_open()) {
          err << "Error: cannot open file '" << f << "'." << endl;
          goto error;
        }
        if (verbose)
          std::cerr << "processing file '" << fullname << "'" << endl;
        s = get_file_contents(file);
        
        if (m->filepath().size() == 0)
          m->setFilepath(fullname);
        isFzn = (fullname.compare(fullname.length()-4,4,".fzn")==0);
        isFzn |= (fullname.compare(fullname.length()-4,4,".ozn")==0);
        isFzn |= (fullname.compare(fullname.length()-4,4,".szn")==0);
        isFzn |= (fullname.compare(fullname.length()-4,4,".mzc")==0);
      } else {
        isFzn = false;
        fullname = f;
        s = parentPath;
      }
      ParserState pp(fullname,s, err, files, seenModels, m, false, isFzn, parseDocComments);
      mzn_yylex_init(&pp.yyscanner);
      mzn_yyset_extra(&pp, pp.yyscanner);
      mzn_yyparse(&pp);
      if (pp.yyscanner)
        mzn_yylex_destroy(pp.yyscanner);
      if (pp.hadError) {
        for (unsigned int i=0; i<pp.syntaxErrors.size(); i++)
          syntaxErrors.push_back(pp.syntaxErrors[i]);
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
          for (unsigned int i=0; i<pp.syntaxErrors.size(); i++)
            syntaxErrors.push_back(pp.syntaxErrors[i]);
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
               const string& textModel,
               const string& textModelName,
               const vector<string>& ip,
               bool ignoreStdlib,
               bool parseDocComments,
               bool verbose,
               ostream& err) {
    
    if (filenames.empty() && textModel.empty()) {
      err << "Error: no model given" << std::endl;
      return NULL;
    }
    
    Model* model;
    {
      GCLock lock;
      model = new Model();
    }
    std::vector<SyntaxError> se;
    parse(env, model, filenames, datafiles, textModel, textModelName,
          ip, ignoreStdlib, parseDocComments, verbose, err, se);
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
    std::vector<SyntaxError> se;
    parse(env, model, filenames, datafiles, "", "", includePaths,
          ignoreStdlib, parseDocComments, verbose, err, se);
    return model;
  }

  Model* parseFromString(Env& env,
                         const string& text,
                         const string& filename,
                         const vector<string>& ip,
                         bool ignoreStdlib,
                         bool parseDocComments,
                         bool verbose,
                         ostream& err,
                         std::vector<SyntaxError>& syntaxErrors) {
    vector<string> filenames;
    vector<string> datafiles;
    Model* model;
    {
      GCLock lock;
      model = new Model();
    }
    std::vector<SyntaxError> se;
    parse(env, model, filenames, datafiles, text, filename,
          ip, ignoreStdlib, parseDocComments, verbose, err, se);
    return model;
  }
  
}
