/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/prettyprinter.hh>
#include <minizinc/htmlprinter.hh>

#include <minizinc/model.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/copy.hh>

#include <sstream>
#include <cctype>

namespace MiniZinc {

  namespace HtmlDocOutput {
    
    class DocItem {
    public:
      enum DocType { T_PAR=0, T_VAR=1, T_FUN=2 };
      DocItem(const DocType& t0, std::string id0, std::string sig0, std::string doc0) : t(t0), id(id0), sig(sig0), doc(doc0) {}
      DocType t;
      std::string id;
      std::string sig;
      std::string doc;
    };

    typedef UNORDERED_NAMESPACE::unordered_map<FunctionI*,std::string> FunMap;
    
    class Group;

    class GroupMap {
    public:
      typedef std::vector<Group*> Map;
      Map m;
      ~GroupMap();
      Map::iterator find(const std::string& n);
    };
    
    class Group {
    public:
      Group(const std::string& name0, const std::string& fullPath0) : name(name0), fullPath(fullPath0) {}
      std::string name;
      std::string fullPath;
      std::string desc;
      std::string htmlName;
      GroupMap subgroups;
      std::vector<DocItem> items;
      
      std::string getAnchor(int level, int indivFileLevel) {
        if (level < indivFileLevel) {
          return fullPath + ".html";
        } else {
          return "#" + fullPath;
        }
      }
      
      std::string toHTML(int level, int indivFileLevel, Group* parent, int idx, const std::string& basename, bool generateIndex) {
        std::ostringstream oss;

        int realLevel = (level < indivFileLevel) ? 0 : level - indivFileLevel;
        oss << "<div class='mzn-group-level-" << realLevel << "'>\n";
        if (parent) {
          oss << "<div class='mzn-group-nav'>";
          if (idx > 0) {
            oss << "<a class='mzn-nav-prev' href='" << parent->subgroups.m[idx-1]->getAnchor(level-1, indivFileLevel)
            <<"' title='" << parent->subgroups.m[idx-1]->htmlName
            << "'>&#8656;</a> ";
          }
          oss << "<a class='mzn-nav-up' href='" << parent->getAnchor(level-1, indivFileLevel)
              << "' title='" << parent->htmlName
              << "'>&#8679;</a> ";
          if (idx < parent->subgroups.m.size()-1) {
            oss << "<a class='mzn-nav-next' href='" << parent->subgroups.m[idx+1]->getAnchor(level-1, indivFileLevel)
                <<"' title='" << parent->subgroups.m[idx+1]->htmlName
                << "'>&#8658;</a> ";
          }
          if (generateIndex)
            oss << "<a href='doc-index.html'>Index</a>\n";
          if (items.size() > 0) {
            oss << "<a href='javascript:void(0)' onclick='revealAll()' class='mzn-nav-text'>reveal all</a>\n";
            oss << "<a href='javascript:void(0)' onclick='hideAll()' class='mzn-nav-text'>hide all</a>\n";
          }
          oss << "</div>";
        }
        if (!htmlName.empty()) {
          oss << "<div class='mzn-group-name'><a name='" << fullPath << "'>" << htmlName << "</a></div>\n";
          oss << "<div class='mzn-group-desc'>\n" << desc << "</div>\n";
        }
        
        if (subgroups.m.size() != 0) {
          oss << "<p>Sections:</p>\n";
          oss << "<ul>\n";
          for (GroupMap::Map::iterator it = subgroups.m.begin(); it != subgroups.m.end(); ++it) {
            oss << "<li><a href='" << (*it)->getAnchor(level, indivFileLevel) << "'>" << (*it)->htmlName << "</a>\n";
            
            if ((*it)->htmlName.empty()) {
              std::cerr << "Warning: undocumented group " << (*it)->fullPath << "\n";
            }
          }
          oss << "</ul>\n";
          if (parent==NULL && generateIndex) {
            oss << "<p><a href='doc-index.html'>Index</a></p>\n";
          }
          if (items.size() > 0)
            oss << "<p>Declarations in this section:</p>\n";
        }

        struct SortById {
          bool operator ()(const DocItem& i0, const DocItem& i1) {
            return i0.t < i1.t || (i0.t==i1.t && i0.id < i1.id);
          }
        } _cmp;
        std::stable_sort(items.begin(), items.end(), _cmp);

        int cur_t = -1;
        const char* dt[] = {"par","var","fun"};
        const char* dt_desc[] = {"Parameters","Variables","Functions and Predicates"};
        for (std::vector<DocItem>::const_iterator it = items.begin(); it != items.end(); ++it) {
          if (it->t != cur_t) {
            if (cur_t != -1)
              oss << "</div>\n";
            cur_t = it->t;
            oss << "<div class='mzn-decl-type-" << dt[cur_t] << "'>\n";
            oss << "<div class='mzn-decl-type-heading'>" << dt_desc[cur_t] << "</div>\n";
          }
          oss << it->doc;
        }
        if (cur_t != -1)
          oss << "</div>\n";
        
        if (level >= indivFileLevel) {
          for (unsigned int i=0; i<subgroups.m.size(); i++) {
            oss << subgroups.m[i]->toHTML(level+1, indivFileLevel, this, i, basename, generateIndex);
          }
        }
        
        oss << "</div>";
        return oss.str();
      }
      
    };

    GroupMap::~GroupMap() {
      for (Map::iterator it = m.begin(); it != m.end(); ++it) {
        delete *it;
      }
    }
    GroupMap::Map::iterator
    GroupMap::find(const std::string& n) {
      for (Map::iterator it = m.begin(); it != m.end(); ++it)
        if ((*it)->name == n)
          return it;
      return m.end();
    }

    void addToGroup(Group& gm, const std::string& group, DocItem& di) {
      std::vector<std::string> subgroups;
      size_t lastpos = 0;
      size_t pos = group.find(".");
      while (pos != std::string::npos) {
        subgroups.push_back(group.substr(lastpos, pos-lastpos));
        lastpos = pos+1;
        pos = group.find(".", lastpos);
      }
      subgroups.push_back(group.substr(lastpos, std::string::npos));
      
      GroupMap* cgm = &gm.subgroups;
      std::string gpath(gm.fullPath);
      for (unsigned int i=0; i<subgroups.size(); i++) {
        gpath += "-";
        gpath += subgroups[i];
        if (cgm->find(subgroups[i]) == cgm->m.end()) {
          cgm->m.push_back(new Group(subgroups[i], gpath));
        }
        Group& g = **cgm->find(subgroups[i]);
        if (i==subgroups.size()-1) {
          g.items.push_back(di);
        } else {
          cgm = &g.subgroups;
        }
      }
    }
    
    void setGroupDesc(Group& maingroup, const std::string& group, std::string htmlName, std::string s) {
      
      if (group == "MAIN") {
        if (!maingroup.htmlName.empty()) {
          std::cerr << "Warning: two descriptions for group `" << group << "'\n";
        }
        maingroup.htmlName = htmlName;
        maingroup.desc = s;
        return;
      }
      
      std::vector<std::string> subgroups;
      size_t lastpos = 0;
      size_t pos = group.find(".");
      while (pos != std::string::npos) {
        subgroups.push_back(group.substr(lastpos, pos-lastpos));
        lastpos = pos+1;
        pos = group.find(".", lastpos);
      }
      subgroups.push_back(group.substr(lastpos, std::string::npos));

      GroupMap* cgm = &maingroup.subgroups;
      std::string gpath(maingroup.fullPath);
      for (unsigned int i=0; i<subgroups.size(); i++) {
        gpath += "-";
        gpath += subgroups[i];
        if (cgm->find(subgroups[i]) == cgm->m.end()) {
          cgm->m.push_back(new Group(subgroups[i], gpath));
        }
        Group& g = **cgm->find(subgroups[i]);
        if (i==subgroups.size()-1) {
          if (!g.htmlName.empty()) {
            std::cerr << "Warning: two descriptions for group `" << group << "'\n";
          }
          g.htmlName = htmlName;
          g.desc = s;
        } else {
          cgm = &g.subgroups;
        }
      }
    }
    
    std::string extractArgWord(std::string& s, size_t n) {
      size_t start = n;
      while (start < s.size() && s[start]!=' ' && s[start]!='\t')
        start++;
      while (start < s.size() && (s[start]==' ' || s[start]=='\t'))
        start++;
      int end = start+1;
      while (end < s.size() && (isalnum(s[end]) || s[end]=='_' || s[end]=='.'))
        end++;
      std::string ret = s.substr(start,end-start);
      s = s.substr(0,n)+s.substr(end,std::string::npos);
      return ret;
    }

    std::string makeHTMLId(const std::string& ident) {
      std::ostringstream oss;
      oss << "I";
      bool prevWasSym = false;
      for (size_t i=0; i<ident.size(); i++) {
        bool isSym = true;
        switch (ident[i]) {
          case '!': oss << "-ex"; break;
          case '=': oss << "-eq"; break;
          case '*': oss << "-as"; break;
          case '+': oss << "-pl"; break;
          case '-': oss << "-mi"; break;
          case '>' : oss << "-gr"; break;
          case '<': oss << "-lt"; break;
          case '/': oss << "-dv"; break;
          case '\\': oss << "-bs"; break;
          case '~': oss << "-tl"; break;
          case '\'': oss << "-tk"; break;
          case ' ': break;
          case '\t': break;
          case '\n': break;
          case ':': oss << "-cl"; break;
          case '[': oss << "-bo"; break;
          case ']': oss << "-bc"; break;
          case '$': oss << "-dd"; break;
          case '(': oss << "-po"; break;
          case ')': oss << "-pc"; break;
          case ',': oss << "-cm"; break;
          default: oss << (prevWasSym ? "-" : "") << ident[i]; isSym = false; break;
        }
        prevWasSym = isSym;
      }
      return oss.str();
    }
    
  }

  class CollectFunctionsVisitor : public ItemVisitor {
  protected:
    EnvI& env;
    HtmlDocOutput::FunMap& _funmap;
    bool _includeStdLib;
  public:
    CollectFunctionsVisitor(EnvI& env0, HtmlDocOutput::FunMap& funmap, bool includeStdLib)
    : env(env0), _funmap(funmap), _includeStdLib(includeStdLib) {}
    bool enterModel(Model* m) {
      return _includeStdLib || m->filename()!="stdlib.mzn";
    }
    void vFunctionI(FunctionI* fi) {
      if (Call* docstring = Expression::dyn_cast<Call>(getAnnotation(fi->ann(), constants().ann.doc_comment))) {
        std::string ds = eval_string(env,docstring->args()[0]);
        std::string group("main");
        size_t group_idx = ds.find("@group");
        if (group_idx!=std::string::npos) {
          group = HtmlDocOutput::extractArgWord(ds, group_idx);
        }
        _funmap.insert(std::make_pair(fi, group));
      }
    }
  };
  
  class PrintHtmlVisitor : public ItemVisitor {
  protected:
    EnvI& env;
    HtmlDocOutput::Group& _maingroup;
    HtmlDocOutput::FunMap& _funmap;
    bool _includeStdLib;
    
    std::vector<std::string> replaceArgs(std::string& s) {
      std::vector<std::string> replacements;
      std::ostringstream oss;
      size_t lastpos = 0;
      size_t pos = std::min(s.find("\\a"), s.find("\\p"));
      size_t mathjax_open = s.find("\\(");
      size_t mathjax_close = s.rfind("\\)");
      if (pos == std::string::npos)
        return replacements;
      while (pos != std::string::npos) {
        oss << s.substr(lastpos, pos-lastpos);
        size_t start = pos;
        while (start < s.size() && s[start]!=' ' && s[start]!='\t')
          start++;
        while (start < s.size() && (s[start]==' ' || s[start]=='\t'))
          start++;
        int end = start+1;
        while (end < s.size() && (isalnum(s[end]) || s[end]=='_'))
          end++;
        if (s[pos+1]=='a') {
          replacements.push_back(s.substr(start,end-start));
          if (pos >= mathjax_open && pos <= mathjax_close) {
            oss << "{\\bf " << replacements.back() << "}";
          } else {
            oss << "<span class='mzn-arg'>" << replacements.back() << "</span>";
          }
        } else {
          if (pos >= mathjax_open && pos <= mathjax_close) {
            oss << "{\\bf " << s.substr(start,end-start) << "}";
          } else {
            oss << "<span class='mzn-parm'>" << s.substr(start,end-start) << "</span>";
          }
        }
        lastpos = end;
        pos = std::min(s.find("\\a", lastpos), s.find("\\p", lastpos));
      }
      oss << s.substr(lastpos, std::string::npos);
      s = oss.str();
      return replacements;
    }

    std::pair<std::string,std::string> extractArgLine(std::string& s, size_t n) {
      size_t start = n;
      while (start < s.size() && s[start]!=' ' && s[start]!='\t')
        start++;
      while (start < s.size() && (s[start]==' ' || s[start]=='\t'))
        start++;
      int end = start+1;
      while (end < s.size() && s[end]!=':')
        end++;
      std::string arg = s.substr(start,end-start);
      size_t doc_start = end+1;
      while (end < s.size() && s[end]!='\n')
        end++;
      std::string ret = s.substr(doc_start,end-doc_start);
      replaceArgs(ret);
      s = s.substr(0,n)+s.substr(end,std::string::npos);
      return make_pair(arg,ret);
    }
    
    
    std::string addHTML(const std::string& s) {
      std::ostringstream oss;
      size_t lastpos = 0;
      size_t pos = s.find('\n');
      bool inUl = false;
      oss << "<p>\n";
      while (pos != std::string::npos) {
        oss << s.substr(lastpos, pos-lastpos);
        size_t next = std::min(s.find('\n', pos+1),s.find('-', pos+1));
        if (next==std::string::npos) {
          lastpos = pos+1;
          break;
        }
        bool allwhite = true;
        for (size_t cur = pos+1; cur < next; cur++) {
          if (s[cur]!=' ' && s[cur]!='\t') {
            allwhite = false;
            break;
          }
        }
        if (allwhite) {
          if (s[next]=='-') {
            if (!inUl) {
              oss << "<ul>\n";
              inUl = true;
            }
            oss << "<li>";
          } else {
            if (inUl) {
              oss << "</ul>\n";
              inUl = false;
            } else {
              oss << "</p><p>\n";
            }
          }
          lastpos = next+1;
          pos = s.find('\n', lastpos);
        } else {
          lastpos = pos+1;
          if (s[pos]=='\n') {
            oss << " ";
          }
          if (s[next]=='-') {
            pos = s.find('\n', next+1);
          } else {
            pos = next;
          }
        }
      }
      oss << s.substr(lastpos, std::string::npos);
      if (inUl)
        oss << "</ul>\n";
      oss << "</p>\n";
      return oss.str();
    }
    
  public:
    PrintHtmlVisitor(EnvI& env0, HtmlDocOutput::Group& mg, HtmlDocOutput::FunMap& fm, bool includeStdLib) : env(env0), _maingroup(mg), _funmap(fm), _includeStdLib(includeStdLib) {}
    bool enterModel(Model* m) {
      if (!_includeStdLib && m->filename()=="stdlib.mzn")
        return false;
      const std::string& dc = m->docComment();
      if (!dc.empty()) {
        size_t gpos = dc.find("@groupdef");
        while (gpos != std::string::npos) {
          size_t start = gpos;
          while (start < dc.size() && dc[start]!=' ' && dc[start]!='\t')
            start++;
          while (start < dc.size() && (dc[start]==' ' || dc[start]=='\t'))
            start++;
          size_t end = start+1;
          while (end < dc.size() && (isalnum(dc[end]) || dc[end]=='_' || dc[end]=='.'))
            end++;
          std::string groupName = dc.substr(start,end-start);
          size_t doc_start = end+1;
          while (end < dc.size() && dc[end]!='\n')
            end++;
          std::string groupHTMLName = dc.substr(doc_start,end-doc_start);
          
          size_t next = dc.find("@groupdef", gpos+1);
          HtmlDocOutput::setGroupDesc(_maingroup, groupName, groupHTMLName,
                                      addHTML(dc.substr(end, next == std::string::npos ? next : next-end)));
          gpos = next;
        }
      }
      return true;
    }
    /// Visit variable declaration
    void vVarDeclI(VarDeclI* vdi) {
      if (Call* docstring = Expression::dyn_cast<Call>(getAnnotation(vdi->e()->ann(), constants().ann.doc_comment))) {
        std::string ds = eval_string(env,docstring->args()[0]);
        std::string group("main");
        size_t group_idx = ds.find("@group");
        if (group_idx!=std::string::npos) {
          group = HtmlDocOutput::extractArgWord(ds, group_idx);
        }
        
        std::ostringstream os;
        std::string sig = vdi->e()->type().toString(env)+" "+vdi->e()->id()->str().str();
        os << "<div class='mzn-vardecl' id='" << HtmlDocOutput::makeHTMLId(sig) << "'>\n";
        os << "<div class='mzn-vardecl-code'>\n";
        if (vdi->e()->ti()->type() == Type::ann()) {
          os << "<span class='mzn-kw'>annotation</span> ";
          os << "<span class='mzn-fn-id'>" << *vdi->e()->id() << "</span>";
        } else {
          os << *vdi->e()->ti() << ": " << *vdi->e()->id();
        }
        os << "</div><div class='mzn-vardecl-doc'>\n";
        os << addHTML(ds);
        os << "</div></div>";
        GCLock lock;
        HtmlDocOutput::DocItem di(vdi->e()->type().ispar() ? HtmlDocOutput::DocItem::T_PAR: HtmlDocOutput::DocItem::T_VAR,
                                  sig,
                                  sig,
                                  os.str());
        HtmlDocOutput::addToGroup(_maingroup, group, di);
      }
    }
    /// Visit function item
    void vFunctionI(FunctionI* fi) {
      if (Call* docstring = Expression::dyn_cast<Call>(getAnnotation(fi->ann(), constants().ann.doc_comment))) {
        std::string ds = eval_string(env,docstring->args()[0]);
        std::string group("main");
        size_t group_idx = ds.find("@group");
        if (group_idx!=std::string::npos) {
          group = HtmlDocOutput::extractArgWord(ds, group_idx);
        }

        size_t param_idx = ds.find("@param");
        std::vector<std::pair<std::string,std::string> > params;
        while (param_idx != std::string::npos) {
          params.push_back(extractArgLine(ds, param_idx));
          param_idx = ds.find("@param");
        }
        
        std::vector<std::string> args = replaceArgs(ds);
        
        UNORDERED_NAMESPACE::unordered_set<std::string> allArgs;
        for (unsigned int i=0; i<args.size(); i++)
          allArgs.insert(args[i]);
        for (unsigned int i=0; i<params.size(); i++)
          allArgs.insert(params[i].first);
        
        GCLock lock;
        for (unsigned int i=0; i<fi->params().size(); i++) {
          if (allArgs.find(fi->params()[i]->id()->str().str()) == allArgs.end()) {
            std::cerr << "Warning: parameter " << *fi->params()[i]->id() << " not documented for function "
                      << fi->id() << " at location " << fi->loc() << "\n";
          }
        }

        std::string sig;
        {
          GCLock lock;
          FunctionI* fi_c = new FunctionI(Location(),fi->id(),fi->ti(),fi->params());
          std::ostringstream oss_sig;
          oss_sig << *fi_c;
          sig = oss_sig.str();
          sig.resize(sig.size()-2);
        }

        std::ostringstream os;
        os << "<div class='mzn-fundecl' id='" << HtmlDocOutput::makeHTMLId(sig) << "'>\n";
        os << "<div class='mzn-fundecl-code'>";
        os << "<a href='javascript:void(0)' onclick='revealMore(this)' class='mzn-fundecl-more'>&#9664;</a>";

        std::ostringstream fs;
        if (fi->ti()->type() == Type::ann()) {
          fs << "annotation ";
          os << "<span class='mzn-kw'>annotation</span> ";
        } else if (fi->ti()->type() == Type::parbool()) {
          fs << "test ";
          os << "<span class='mzn-kw'>test</span> ";
        } else if (fi->ti()->type() == Type::varbool()) {
          fs << "predicate ";
          os << "<span class='mzn-kw'>predicate</span> ";
        } else {
          fs << "function " << *fi->ti() << ": ";
          os << "<span class='mzn-kw'>function</span> <span class='mzn-ti'>" << *fi->ti() << "</span>: ";
        }
        fs << fi->id() << "(";
        os << "<span class='mzn-fn-id'>" << fi->id() << "</span>(";
        size_t align = fs.str().size();
        for (unsigned int i=0; i<fi->params().size(); i++) {
          fs << *fi->params()[i]->ti() << ": " << *fi->params()[i]->id();
          if (i < fi->params().size()-1) {
            fs << ", ";
          }
        }
        bool splitArgs = (fs.str().size() > 70);
        for (unsigned int i=0; i<fi->params().size(); i++) {
          os << "<span class='mzn-ti'>" << *fi->params()[i]->ti() << "</span>: "
             << "<span class='mzn-id'>" << *fi->params()[i]->id() << "</span>";
          if (i < fi->params().size()-1) {
            os << ",";
            if (splitArgs) {
              os << "\n";
              for (unsigned int j=align; j--;)
                os << " ";
            } else {
              os << " ";
            }
          }
        }
        os << ")";
        
        if (fi->e()) {
          FunctionI* f_body = fi;
          bool alias;
          do {
            alias = false;
            Call* c = Expression::dyn_cast<Call>(f_body->e());
            if (c && c->args().size()==f_body->params().size()) {
              bool sameParams = true;
              for (unsigned int i=0; i<f_body->params().size(); i++) {
                Id* ident = c->args()[i]->dyn_cast<Id>();
                if (ident == NULL || ident->decl() != f_body->params()[i] || ident->str() != c->decl()->params()[i]->id()->str()) {
                  sameParams = false;
                  break;
                }
              }
              if (sameParams) {
                alias = true;
                f_body = c->decl();
              }
            }
          } while (alias);
          if (f_body->e()) {
            std::ostringstream body_os;
            Printer p(body_os, 70);
            p.print(f_body->e());

            std::string filename = f_body->loc().filename().str();
            size_t lastSlash = filename.find_last_of("/");
            if (lastSlash != std::string::npos) {
              filename = filename.substr(lastSlash+1, std::string::npos);
            }
            os << "<span class='mzn-fundecl-equals'> =</span>";
            os << "\n<div class='mzn-fundecl-more-code'>";
            os << "<div class='mzn-fundecl-body'>";
            os << body_os.str();
            os << "</div>\n";
            os << "(standard decomposition from "<< filename << ":" << f_body->loc().first_line()<<")";
            os << "</div>";
          }
        }
        
        os << "</div>\n<div class='mzn-fundecl-doc'>\n";

        if (fi->id().c_str()[0]=='\'') {
          std::string op = fi->id().str();
          op = op.substr(1,op.length()-2);
          const char* space = (op[0]>='a' ? " " : "");
          if (fi->params().size()==2) {
            os << "<p>Usage: <span class=\"mzn-arg\">" << *fi->params()[0]->id() << space
               << op << space << *fi->params()[1]->id() << "</span></p>";
          } else if (fi->params().size()==1) {
            os << "<p>Usage: <span class=\"mzn-arg\">" << op << space << *fi->params()[0]->id() << "</span></p>";
          }
        }

        std::string dshtml = addHTML(ds);

        os << dshtml;
        if (params.size() > 0) {
          os << "<div class='mzn-fundecl-params-heading'>Parameters</div>\n";
          os << "<ul class='mzn-fundecl-params'>\n";
          for (unsigned int i=0; i<params.size(); i++) {
            os << "<li><span class='mzn-arg'>" << params[i].first << "</span>: " << params[i].second << "</li>\n";
          }
          os << "</ul>\n";
        }
        os << "</div>";
        os << "</div>";
        
        HtmlDocOutput::DocItem di(HtmlDocOutput::DocItem::T_FUN, fi->id().str(), sig, os.str());
        HtmlDocOutput::addToGroup(_maingroup, group, di);
      }
    }
  };
  
  std::vector<HtmlDocument>
  HtmlPrinter::printHtml(EnvI& env, MiniZinc::Model* m, const std::string& basename, int splitLevel, bool includeStdLib, bool generateIndex) {
    using namespace HtmlDocOutput;
    Group g(basename,basename);
    FunMap funMap;
    CollectFunctionsVisitor fv(env,funMap,includeStdLib);
    iterItems(fv, m);
    PrintHtmlVisitor phv(env,g,funMap,includeStdLib);
    iterItems(phv, m);
    
    std::vector<HtmlDocument> ret;

    struct SI {
      Group* g;
      Group* p;
      int level;
      int idx;
      SI(Group* g0, Group* p0, int level0, int idx0) : g(g0), p(p0), level(level0), idx(idx0) {}
    };
    
    struct IndexEntry {
      std::string id;
      std::string sig;
      std::string link;
      std::string groupName;
      IndexEntry(const std::string& id0, const std::string& sig0,
                 const std::string& link0, const std::string& groupName0)
      : id(id0), sig(sig0), link(link0), groupName(groupName0) {
        size_t spacepos = id.find_last_of(' ');
        if (spacepos != std::string::npos) {
          id = id.substr(spacepos+1);
        }
      }
      bool operator<(const IndexEntry& e) const {
        if (!isalpha(id[0]) && isalpha(e.id[0]))
          return true;
        return id == e.id ? groupName < e.groupName : id < e.id;
      }
    };
    std::vector<IndexEntry> index;
    
    std::vector<SI> stack;
    stack.push_back(SI(&g,NULL,0,0));
    while (!stack.empty()) {
      Group& g = *stack.back().g;
      int curLevel = stack.back().level;
      int curIdx = stack.back().idx;
      Group* p = stack.back().p;
      stack.pop_back();
      for (auto it : g.items) {
        index.push_back(IndexEntry(it.id,it.sig,g.fullPath,g.htmlName));
      }
      ret.push_back(HtmlDocument(g.fullPath, g.htmlName, g.toHTML(curLevel, splitLevel, p, curIdx, basename, generateIndex)));
      if (curLevel < splitLevel) {
        for (unsigned int i=0; i<g.subgroups.m.size(); i++) {
          stack.push_back(SI(g.subgroups.m[i],&g,curLevel+1,i));
        }
      }
    }
    
    if (generateIndex) {
      std::sort(index.begin(), index.end());
      std::ostringstream oss;
      index.push_back(IndexEntry("","","",""));
      
      std::vector<std::string> idxSections;
      
      if (index.size() != 0) {
        if (isalpha(index[0].id[0])) {
          char idxSec_c = (char)toupper(index[0].id[0]);
          std::string idxSec(&idxSec_c,1);
          oss << "<h3 id='Idx" << idxSec << "'>" << idxSec << "</h3>\n";
          idxSections.push_back(idxSec);
        } else {
          oss << "<h3 id='IdxSymbols'>Symbols</h3>\n";
          idxSections.push_back("Symbols");
        }
      }
      oss << "<ul>\n";
      std::string prevId = index.size()==0 ? "" : index[0].id;
      std::vector<IndexEntry> curEntries;
      for (auto ie : index) {
        if (ie.id != prevId) {
          oss << "<li>";
          assert(curEntries.size() != 0);
          IndexEntry& cur = curEntries[0];
          if (curEntries.size()==1) {
            oss << cur.id << " <a href='"<< cur.link << ".html#" << HtmlDocOutput::makeHTMLId(cur.sig) << "'>" << "(" << cur.groupName << ")</a>";
          } else {
            oss << cur.id << " (";
            bool first = true;
            for (auto i_ie: curEntries) {
              if (first) {
                first = false;
              } else {
                oss << ", ";
              }
              oss << "<a href='" << i_ie.link << ".html#" << HtmlDocOutput::makeHTMLId(i_ie.sig) << "'>";
              oss << i_ie.groupName << "</a>";
            }
            oss << ")";
          }
          oss << "</li>\n";
          curEntries.clear();
        }
        if (isalpha(ie.id[0]) && ie.id[0] != prevId[0]) {
          char idxSec_c = (char)toupper(ie.id[0]);
          std::string idxSec(&idxSec_c,1);
          oss << "</ul>\n<h3 id='Idx" << idxSec << "'>" << idxSec << "</h3><ul>";
          idxSections.push_back(idxSec);
        }
        prevId = ie.id;
        if (curEntries.size()==0 || curEntries.back().groupName != ie.groupName) {
          curEntries.push_back(ie);
        }
      }
      oss << "</ul>\n";
      
      std::ostringstream oss_header;
      oss_header << "<div class='mzn-group-level-0'>\n";
      oss_header << "<div class='mzn-group-nav'>";
      oss_header << "<a class='mzn-nav-up' href='" << g.getAnchor(0, 1)
                 << "' title='" << g.htmlName
                 << "'>&#8679;</a> ";
      bool first = true;
      for (auto is : idxSections) {
        if (first) {
          first = false;
        } else {
          oss_header << " | ";
        }
        oss_header << "<a href='#Idx" << is << "'>" << is << "</a>";
      }

      oss_header << "</div>";

      oss_header << "<div class='mzn-group-name'>Index</div>\n";
      
      HtmlDocument idx("doc-index", "Index", oss_header.str()+oss.str());
      ret.push_back(idx);
    }
    return ret;
  }

}
