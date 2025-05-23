/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/config.hh>
#include <minizinc/copy.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/htmlprinter.hh>
#include <minizinc/model.hh>
#include <minizinc/prettyprinter.hh>

#include <cctype>
#include <sstream>
#include <utility>

namespace MiniZinc {

namespace HtmlDocOutput {

// Trim leading space:
// - always trim first line completely
// - second line defines the base indentation
std::string trim(const std::string& s0) {
  std::string s = s0;
  // remove carriage returns
  size_t j = 0;
  for (size_t i = 0; i < s.size(); i++) {
    if (s[i] != '\r') {
      s[j++] = s[i];
    }
  }
  s.resize(j);
  size_t first_line_indent = s.find_first_not_of(" \t");
  if (first_line_indent == std::string::npos) {
    return "";
  }
  size_t first_nl = s.find('\n');
  std::ostringstream oss;
  if (first_line_indent == first_nl) {
    // first line is empty
    oss << "\n";
  } else {
    // strip first line
    size_t end_of_first_line =
        first_nl == std::string::npos ? std::string::npos : first_nl - first_line_indent + 1;
    oss << s.substr(first_line_indent, end_of_first_line);
  }
  if (first_nl == std::string::npos) {
    return oss.str();
  }
  size_t unindent = s.find_first_not_of(" \t", first_nl + 1);
  if (unindent == std::string::npos) {
    return oss.str();
  }
  size_t pos = s.find('\n', first_nl + 1);
  if (unindent == 0 || unindent > pos) {
    oss << s.substr(first_nl + 1, std::string::npos);
    return oss.str();
  }
  size_t lastpos = unindent;
  while (pos != std::string::npos) {
    oss << s.substr(lastpos, pos - lastpos) << "\n";
    size_t next_indent = s.find_first_not_of(" \t", pos + 1);
    if (next_indent == std::string::npos || next_indent - (pos + 1) < unindent) {
      lastpos = next_indent;
    } else {
      lastpos = pos + 1 + unindent;
    }
    pos = (lastpos == std::string::npos ? lastpos : s.find('\n', lastpos));
  }
  if (lastpos != std::string::npos) {
    oss << s.substr(lastpos, std::string::npos);
  }
  return oss.str();
}

class DocItem {
public:
  enum DocType { T_PAR = 0, T_VAR = 1, T_FUN = 2, T_ANN = 3 };
  DocItem(const DocType& t0, std::string id0, std::string sig0, std::string doc0)
      : t(t0), id(std::move(id0)), sig(std::move(sig0)), doc(std::move(doc0)) {}
  DocType t;
  std::string id;
  std::string sig;
  std::string doc;
};

class Group;

class GroupMap {
public:
  typedef std::vector<Group*> Map;
  Map m;
  ~GroupMap();
  Map::iterator find(const std::string& n);
};

std::string escape_bs(const std::string& s) {
  std::ostringstream oss;
  for (auto c : s) {
    if (c == '<') {
      oss << "\\<";
    } else {
      oss << c;
    }
    if (c == '\\') {
      oss << '\\';
    }
  }
  return oss.str();
}

std::string ident_to_label(const std::string& s) {
  std::ostringstream oss;
  for (auto c : s) {
    switch (c) {
      case '\\':
        oss << "B";
        break;
      case '/':
        oss << "S";
        break;
      case '<':
        oss << "lt";
        break;
      case '>':
        oss << "gt";
        break;
      case '.':
        oss << "D";
        break;
      case '\'':
        break;
      default:
        oss << c;
        break;
    }
  }
  return oss.str();
}

class Group {
public:
  Group(std::string name0, std::string fullPath0)
      : name(std::move(name0)), fullPath(std::move(fullPath0)) {}
  std::string name;
  std::string fullPath;
  std::string desc;
  std::string htmlName;
  GroupMap subgroups;
  std::vector<DocItem> items;

  std::string getAnchor(int level, int indivFileLevel) const {
    if (level < indivFileLevel) {
      return fullPath + ".html";
    }
    return "#" + fullPath;
  }

  std::string toHTML(int level, int indivFileLevel, Group* parent, unsigned int idx,
                     const std::string& basename, bool generateIndex) {
    std::ostringstream oss;

    int realLevel = (level < indivFileLevel) ? 0 : level - indivFileLevel;
    oss << "<div class='mzn-group-level-" << realLevel << "'>\n";
    if (parent != nullptr) {
      oss << "<div class='mzn-group-nav'>";
      if (idx > 0) {
        oss << "<a class='mzn-nav-prev' href='"
            << parent->subgroups.m[idx - 1]->getAnchor(level - 1, indivFileLevel) << "' title='"
            << parent->subgroups.m[idx - 1]->htmlName << "'>&#8656;</a> ";
      }
      oss << "<a class='mzn-nav-up' href='" << parent->getAnchor(level - 1, indivFileLevel)
          << "' title='" << parent->htmlName << "'>&#8679;</a> ";
      if (idx < parent->subgroups.m.size() - 1) {
        oss << "<a class='mzn-nav-next' href='"
            << parent->subgroups.m[idx + 1]->getAnchor(level - 1, indivFileLevel) << "' title='"
            << parent->subgroups.m[idx + 1]->htmlName << "'>&#8658;</a> ";
      }
      if (generateIndex) {
        oss << "<a href='doc-index.html'>Index</a>\n";
      }
      if (!items.empty()) {
        oss << "<a href='javascript:void(0)' onclick='revealAll()' class='mzn-nav-text'>reveal "
               "all</a>\n";
        oss << "<a href='javascript:void(0)' onclick='hideAll()' class='mzn-nav-text'>hide "
               "all</a>\n";
      }
      oss << "</div>";
    }
    if (!htmlName.empty()) {
      oss << "<div class='mzn-group-name'><a name='" << fullPath << "'>" << htmlName
          << "</a></div>\n";
      oss << "<div class='mzn-group-desc'>\n" << desc << "</div>\n";
    }

    if (!subgroups.m.empty()) {
      oss << "<p>Sections:</p>\n";
      oss << "<ul>\n";
      for (auto& it : subgroups.m) {
        oss << "<li><a href='" << it->getAnchor(level, indivFileLevel) << "'>" << it->htmlName
            << "</a>\n";

        if (it->htmlName.empty()) {
          std::cerr << "Warning: undocumented group " << it->fullPath << "\n";
        }
      }
      oss << "</ul>\n";
      if (parent == nullptr && generateIndex) {
        oss << "<p><a href='doc-index.html'>Index</a></p>\n";
      }
      if (!items.empty()) {
        oss << "<p>Declarations in this section:</p>\n";
      }
    }

    struct SortById {
      bool operator()(const DocItem& i0, const DocItem& i1) {
        return i0.t < i1.t || (i0.t == i1.t && i0.id < i1.id);
      }
    } _cmp;
    std::stable_sort(items.begin(), items.end(), _cmp);

    int cur_t = -1;
    const char* dt[] = {"par", "var", "fun"};
    const char* dt_desc[] = {"Constants", "Variables", "Functions and Predicates", "Annotations"};
    for (const auto& item : items) {
      if (item.t != cur_t) {
        if (cur_t != -1) {
          oss << "</div>\n";
        }
        cur_t = item.t;
        oss << "<div class='mzn-decl-type-" << dt[cur_t] << "'>\n";
        oss << "<div class='mzn-decl-type-heading'>" << dt_desc[cur_t] << "</div>\n";
      }
      oss << item.doc;
    }
    if (cur_t != -1) {
      oss << "</div>\n";
    }

    if (level >= indivFileLevel) {
      for (unsigned int i = 0; i < subgroups.m.size(); i++) {
        oss << subgroups.m[i]->toHTML(level + 1, indivFileLevel, this, i, basename, generateIndex);
      }
    }

    oss << "</div>";
    return oss.str();
  }

  static std::string rstHeading(const std::string& s, int level, bool printHeading = true) {
    std::vector<char> levelChar({'#', '=', '-', '^', '+', '"', '~'});
    std::ostringstream oss;
    if (printHeading) {
      oss << s << "\n";
    }
    for (int i = 0; i < s.size(); i++) {
      oss << levelChar[level];
    }
    oss << "\n\n";
    return oss.str();
  }

  std::string toRST(int level) {
    std::ostringstream oss;
    if (!htmlName.empty()) {
      if (level == 0) {
        oss << ".. _ch-" << fullPath << ":\n\n";
      }
      oss << rstHeading(htmlName, level);
      oss << HtmlDocOutput::trim(desc) << "\n\n";
    }
    for (auto& i : subgroups.m) {
      oss << i->toRST(level + 1);
    }
    if (!items.empty()) {
      if (!subgroups.m.empty()) {
        oss << rstHeading("Other declarations", level + 1);
      }
      struct SortById {
        bool operator()(const DocItem& i0, const DocItem& i1) {
          return i0.t < i1.t || (i0.t == i1.t && i0.id < i1.id);
        }
      } _cmp;
      std::stable_sort(items.begin(), items.end(), _cmp);

      // Find the group identifier of this section
      std::string groupId;
      for (const auto& item : items) {
        auto pos = item.doc.find("\n.. _mzn_");
        if (pos != std::string::npos) {
          auto endPos = item.doc.find_first_of(':', pos);
          if (endPos != std::string::npos) {
            std::string refString = item.doc.substr(pos + 5, endPos - pos - 4);
            auto lastDot = refString.find_last_of('.');
            if (lastDot != std::string::npos) {
              groupId = refString.substr(0, lastDot);
              break;
            }
          }
        }
      }

      if (!groupId.empty()) {
        oss << ".. only:: builder_html\n\n";
        oss << "  In this section: ";
        std::string prevId;
        for (const auto& item : items) {
          if (item.id != prevId) {
            if (!prevId.empty()) {
              oss << ", ";
            }
            auto lastSpace = item.id.find_last_of(' ');
            std::string ident;
            if (lastSpace != std::string::npos) {
              ident = item.id.substr(lastSpace + 1);
            } else {
              ident = item.id;
            }
            auto codeRef = "<" + groupId + "." + ident_to_label(ident) + ">";
            oss << ":ref:`" << escape_bs(ident) << " " << codeRef << "`";
            prevId = item.id;
          }
        }
        oss << ".\n\n";
      }

      int cur_t = -1;
      int nHeadings = 0;
      for (const auto& item : items) {
        if (item.t != cur_t) {
          cur_t = item.t;
          nHeadings++;
        }
      }
      cur_t = -1;
      const char* dt_desc[] = {"Constants", "Variables", "Functions and Predicates", "Annotations"};
      for (const auto& item : items) {
        if (item.t != cur_t) {
          cur_t = item.t;
          if (nHeadings > 1) {
            oss << rstHeading(dt_desc[cur_t], subgroups.m.empty() ? level + 1 : level + 2);
          }
        }
        // replace headings marked by !!!!! with appropriate heading level
        std::istringstream doc(item.doc);
        std::string prevLine;
        for (std::string line; std::getline(doc, line);) {
          bool isHeading = !line.empty();
          for (auto c : line) {
            if (c != '!') {
              isHeading = false;
              break;
            }
          }
          if (isHeading) {
            oss << rstHeading(prevLine,
                              level + 1 + static_cast<int>(nHeadings > 1) +
                                  static_cast<int>(!subgroups.m.empty()),
                              false);
          } else {
            oss << line << "\n";
          }
          prevLine = line;
        }
      }
    }
    return oss.str();
  }
};

GroupMap::~GroupMap() {
  for (auto& it : m) {
    delete it;
  }
}
GroupMap::Map::iterator GroupMap::find(const std::string& n) {
  for (auto it = m.begin(); it != m.end(); ++it) {
    if ((*it)->name == n) {
      return it;
    }
  }
  return m.end();
}

void add_to_group(Group& gm, const std::string& group, DocItem& di) {
  std::vector<std::string> subgroups;
  size_t lastpos = 0;
  size_t pos = group.find('.');
  while (pos != std::string::npos) {
    subgroups.push_back(group.substr(lastpos, pos - lastpos));
    lastpos = pos + 1;
    pos = group.find('.', lastpos);
  }
  subgroups.push_back(group.substr(lastpos, std::string::npos));

  GroupMap* cgm = &gm.subgroups;
  std::string gpath(gm.fullPath);
  for (unsigned int i = 0; i < subgroups.size(); i++) {
    gpath += "-";
    gpath += subgroups[i];
    if (cgm->find(subgroups[i]) == cgm->m.end()) {
      cgm->m.push_back(new Group(subgroups[i], gpath));
    }
    Group& g = **cgm->find(subgroups[i]);
    if (i == subgroups.size() - 1) {
      g.items.push_back(di);
    } else {
      cgm = &g.subgroups;
    }
  }
}

void set_group_desc(Group& maingroup, const std::string& group, const std::string& htmlName,
                    const std::string& s) {
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
  size_t pos = group.find('.');
  while (pos != std::string::npos) {
    subgroups.push_back(group.substr(lastpos, pos - lastpos));
    lastpos = pos + 1;
    pos = group.find('.', lastpos);
  }
  subgroups.push_back(group.substr(lastpos, std::string::npos));

  GroupMap* cgm = &maingroup.subgroups;
  std::string gpath(maingroup.fullPath);
  for (unsigned int i = 0; i < subgroups.size(); i++) {
    gpath += "-";
    gpath += subgroups[i];
    if (cgm->find(subgroups[i]) == cgm->m.end()) {
      cgm->m.push_back(new Group(subgroups[i], gpath));
    }
    Group& g = **cgm->find(subgroups[i]);
    if (i == subgroups.size() - 1) {
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

std::string extract_arg_word(std::string& s, size_t n) {
  size_t start = n;
  while (start < s.size() && s[start] != ' ' && s[start] != '\t') {
    start++;
  }
  while (start < s.size() && (s[start] == ' ' || s[start] == '\t')) {
    start++;
  }
  size_t end = start + 1;
  while (end < s.size() && ((isalnum(s[end]) != 0) || s[end] == '_' || s[end] == '.')) {
    end++;
  }
  std::string ret = s.substr(start, end - start);
  s = s.substr(end, std::string::npos);
  return ret;
}

std::string make_html_id(const std::string& ident) {
  std::ostringstream oss;
  oss << "I";
  bool prevWasSym = false;
  for (char i : ident) {
    bool isSym = true;
    switch (i) {
      case '!':
        oss << "-ex";
        break;
      case '=':
        oss << "-eq";
        break;
      case '*':
        oss << "-as";
        break;
      case '+':
        oss << "-pl";
        break;
      case '-':
        oss << "-mi";
        break;
      case '>':
        oss << "-gr";
        break;
      case '<':
        oss << "-lt";
        break;
      case '/':
        oss << "-dv";
        break;
      case '\\':
        oss << "-bs";
        break;
      case '~':
        oss << "-tl";
        break;
      case '\'':
        oss << "-tk";
        break;
      case ' ':
      case '\t':
      case '\n':
        break;
      case ':':
        oss << "-cl";
        break;
      case '[':
        oss << "-bo";
        break;
      case ']':
        oss << "-bc";
        break;
      case '$':
        oss << "-dd";
        break;
      case '(':
        oss << "-po";
        break;
      case ')':
        oss << "-pc";
        break;
      case ',':
        oss << "-cm";
        break;
      default:
        oss << (prevWasSym ? "-" : "") << i;
        isSym = false;
        break;
    }
    prevWasSym = isSym;
  }
  return oss.str();
}

}  // namespace HtmlDocOutput

class PrintHtmlVisitor : public ItemVisitor {
protected:
  EnvI& _env;
  HtmlDocOutput::Group& _maingroup;
  bool _includeStdLib;

  static std::vector<std::string> replaceArgs(std::string& s) {
    std::vector<std::string> replacements;
    std::ostringstream oss;
    size_t lastpos = 0;
    size_t pos = std::min(s.find("\\a"), s.find("\\p"));
    size_t mathjax_open = s.find("\\(");
    size_t mathjax_close = s.rfind("\\)");
    if (pos == std::string::npos) {
      return replacements;
    }
    while (pos != std::string::npos) {
      oss << s.substr(lastpos, pos - lastpos);
      size_t start = pos;
      while (start < s.size() && s[start] != ' ' && s[start] != '\t') {
        start++;
      }
      while (start < s.size() && (s[start] == ' ' || s[start] == '\t')) {
        start++;
      }
      size_t end = start + 1;
      while (end < s.size() && ((isalnum(s[end]) != 0) || s[end] == '_')) {
        end++;
      }
      if (s[pos + 1] == 'a') {
        replacements.push_back(s.substr(start, end - start));
        if (pos >= mathjax_open && pos <= mathjax_close) {
          oss << "{\\bf " << replacements.back() << "}";
        } else {
          oss << "<span class='mzn-arg'>" << replacements.back() << "</span>";
        }
      } else {
        if (pos >= mathjax_open && pos <= mathjax_close) {
          oss << "{\\bf " << s.substr(start, end - start) << "}";
        } else {
          oss << "<span class='mzn-parm'>" << s.substr(start, end - start) << "</span>";
        }
      }
      lastpos = end;
      pos = std::min(s.find("\\a", lastpos), s.find("\\p", lastpos));
    }
    oss << s.substr(lastpos, std::string::npos);
    s = oss.str();
    return replacements;
  }

  static std::pair<std::string, std::string> extractArgLine(std::string& s, size_t n) {
    size_t start = n;
    while (start < s.size() && s[start] != ' ' && s[start] != '\t') {
      start++;
    }
    while (start < s.size() && (s[start] == ' ' || s[start] == '\t')) {
      start++;
    }
    size_t end = start + 1;
    while (end < s.size() && s[end] != ':') {
      end++;
    }
    std::string arg = s.substr(start, end - start);
    size_t doc_start = end + 1;
    while (end < s.size() && s[end] != '\n') {
      end++;
    }
    std::string ret = s.substr(doc_start, end - doc_start);
    replaceArgs(ret);
    s = s.substr(0, n) + s.substr(end, std::string::npos);
    return make_pair(arg, ret);
  }

  static std::string addHTML(const std::string& s) {
    std::ostringstream oss;
    size_t lastpos = 0;
    size_t pos = s.find('\n');
    bool inUl = false;
    oss << "<p>\n";
    while (pos != std::string::npos) {
      oss << s.substr(lastpos, pos - lastpos);
      size_t next = std::min(s.find('\n', pos + 1), s.find('-', pos + 1));
      if (next == std::string::npos) {
        lastpos = pos + 1;
        break;
      }
      bool allwhite = true;
      for (size_t cur = pos + 1; cur < next; cur++) {
        if (s[cur] != ' ' && s[cur] != '\t') {
          allwhite = false;
          break;
        }
      }
      if (allwhite) {
        if (s[next] == '-') {
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
        lastpos = next + 1;
        pos = s.find('\n', lastpos);
      } else {
        lastpos = pos + 1;
        if (s[pos] == '\n') {
          oss << " ";
        }
        if (s[next] == '-') {
          pos = s.find('\n', next + 1);
        } else {
          pos = next;
        }
      }
    }
    oss << s.substr(lastpos, std::string::npos);
    if (inUl) {
      oss << "</ul>\n";
    }
    oss << "</p>\n";
    return oss.str();
  }

public:
  PrintHtmlVisitor(EnvI& env, HtmlDocOutput::Group& mg, bool includeStdLib)
      : _env(env), _maingroup(mg), _includeStdLib(includeStdLib) {}
  bool enterModel(Model* m) {
    if (!_includeStdLib && FileUtils::base_name(m->filename().c_str()) == "stdlib.mzn") {
      return false;
    }
    const std::string& dc = m->docComment();
    if (!dc.empty()) {
      size_t gpos = dc.find("@groupdef");
      while (gpos != std::string::npos) {
        size_t start = gpos;
        while (start < dc.size() && dc[start] != ' ' && dc[start] != '\t') {
          start++;
        }
        while (start < dc.size() && (dc[start] == ' ' || dc[start] == '\t')) {
          start++;
        }
        size_t end = start + 1;
        while (end < dc.size() && ((isalnum(dc[end]) != 0) || dc[end] == '_' || dc[end] == '.')) {
          end++;
        }
        std::string groupName = dc.substr(start, end - start);
        size_t doc_start = end + 1;
        while (end < dc.size() && dc[end] != '\n') {
          end++;
        }
        std::string groupHTMLName = dc.substr(doc_start, end - doc_start);

        size_t next = dc.find("@groupdef", gpos + 1);
        HtmlDocOutput::set_group_desc(
            _maingroup, groupName, groupHTMLName,
            addHTML(dc.substr(end, next == std::string::npos ? next : next - end)));
        gpos = next;
      }
    }
    return true;
  }
  /// Visit variable declaration
  void vVarDeclI(VarDeclI* vdi) {
    if (Call* docstring = Expression::dynamicCast<Call>(
            get_annotation(Expression::ann(vdi->e()), _env.constants.ann.doc_comment))) {
      std::string ds = eval_string(_env, docstring->arg(0));
      std::string group("main");
      size_t group_idx = ds.find("@group");
      if (group_idx != std::string::npos) {
        group = HtmlDocOutput::extract_arg_word(ds, group_idx);
      }

      std::ostringstream os;
      std::string sig =
          vdi->e()->type().toString(_env) + " " + Printer::quoteId(vdi->e()->id()->str());
      os << "<div class='mzn-vardecl' id='" << HtmlDocOutput::make_html_id(sig) << "'>\n";
      os << "<div class='mzn-vardecl-code'>\n";
      if (vdi->e()->ti()->type() == Type::ann()) {
        os << "<span class='mzn-kw'>annotation</span> ";
        os << "<span class='mzn-fn-id'>" << Printer::quoteId(vdi->e()->id()->str()) << "</span>";
      } else {
        os << *vdi->e()->ti() << ": " << Printer::quoteId(vdi->e()->id()->str());
      }
      os << "</div><div class='mzn-vardecl-doc'>\n";
      os << addHTML(ds);
      os << "</div></div>";
      GCLock lock;
      HtmlDocOutput::DocItem::DocType dt =
          vdi->e()->type().isPar() ? (vdi->e()->type().isAnn() ? HtmlDocOutput::DocItem::T_ANN
                                                               : HtmlDocOutput::DocItem::T_PAR)
                                   : HtmlDocOutput::DocItem::T_VAR;
      HtmlDocOutput::DocItem di(dt, sig, sig, os.str());
      HtmlDocOutput::add_to_group(_maingroup, group, di);
    }
  }
  /// Visit function item
  void vFunctionI(FunctionI* fi) {
    if (Call* docstring = Expression::dynamicCast<Call>(
            get_annotation(fi->ann(), _env.constants.ann.doc_comment))) {
      std::string ds = eval_string(_env, docstring->arg(0));
      std::string group("main");
      size_t group_idx = ds.find("@group");
      if (group_idx != std::string::npos) {
        group = HtmlDocOutput::extract_arg_word(ds, group_idx);
      }

      size_t param_idx = ds.find("@param");
      std::vector<std::pair<std::string, std::string>> params;
      while (param_idx != std::string::npos) {
        params.push_back(extractArgLine(ds, param_idx));
        param_idx = ds.find("@param");
      }

      std::vector<std::string> args = replaceArgs(ds);

      std::unordered_set<std::string> allArgs;
      for (auto& arg : args) {
        allArgs.insert(arg);
      }
      for (auto& param : params) {
        allArgs.insert(param.first);
      }

      GCLock lock;
      for (unsigned int i = 0; i < fi->paramCount(); i++) {
        std::string param(fi->param(i)->id()->str().c_str(), fi->param(i)->id()->str().size());
        if (allArgs.find(param) == allArgs.end()) {
          std::cerr << "Warning: parameter " << *fi->param(i)->id()
                    << " not documented for function " << fi->id() << " at location " << fi->loc()
                    << "\n";
        }
      }

      std::string sig;
      {
        GCLock lock;
        std::vector<VarDecl*> params(fi->paramCount());
        for (unsigned int i = 0; i < fi->paramCount(); i++) {
          params[i] = fi->param(i);
        }
        auto* fi_c = new FunctionI(Location(), fi->id(), fi->ti(), params);
        std::ostringstream oss_sig;
        oss_sig << *fi_c;
        sig = oss_sig.str();
        sig.resize(sig.size() - 2);
      }

      std::ostringstream os;
      os << "<div class='mzn-fundecl' id='" << HtmlDocOutput::make_html_id(sig) << "'>\n";
      os << "<div class='mzn-fundecl-code'>";
      os << "<a href='javascript:void(0)' onclick='revealMore(this)' "
            "class='mzn-fundecl-more'>&#9664;</a>";

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
        os << "<span class='mzn-kw'>function</span> <span class='mzn-ti'>" << *fi->ti()
           << "</span>: ";
      }
      fs << Printer::quoteId(fi->id()) << "(";
      os << "<span class='mzn-fn-id'>" << Printer::quoteId(fi->id()) << "</span>(";
      size_t align = fs.str().size();
      for (unsigned int i = 0; i < fi->paramCount(); i++) {
        fs << *fi->param(i)->ti() << ": " << Printer::quoteId(fi->param(i)->id()->str());
        if (i < fi->paramCount() - 1) {
          fs << ", ";
        }
      }
      bool splitArgs = (fs.str().size() > 70);
      for (unsigned int i = 0; i < fi->paramCount(); i++) {
        os << "<span class='mzn-ti'>" << *fi->param(i)->ti()
           << "</span>: " << "<span class='mzn-id'>" << Printer::quoteId(fi->param(i)->id()->str())
           << "</span>";
        if (i < fi->paramCount() - 1) {
          os << ",";
          if (splitArgs) {
            os << "\n";
            for (auto j = static_cast<unsigned int>(align); (j--) != 0U;) {
              os << " ";
            }
          } else {
            os << " ";
          }
        }
      }
      os << ")";

      if (fi->e() != nullptr) {
        FunctionI* f_body = fi;
        bool alias;
        do {
          alias = false;
          Call* c = Expression::dynamicCast<Call>(f_body->e());
          if ((c != nullptr) && c->argCount() == f_body->paramCount()) {
            bool sameParams = true;
            for (unsigned int i = 0; i < f_body->paramCount(); i++) {
              Id* ident = Expression::dynamicCast<Id>(c->arg(i));
              if (ident == nullptr || ident->decl() != f_body->param(i) ||
                  ident->str() != c->decl()->param(i)->id()->str()) {
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
        if (f_body->e() != nullptr) {
          std::ostringstream body_os;
          Printer p(body_os, 70);
          p.print(f_body->e());

          std::string filename =
              std::string(f_body->loc().filename().c_str(), f_body->loc().filename().size());
          size_t lastSlash = filename.find_last_of('/');
          if (lastSlash != std::string::npos) {
            filename = filename.substr(lastSlash + 1, std::string::npos);
          }
          os << "<span class='mzn-fundecl-equals'> =</span>";
          os << "\n<div class='mzn-fundecl-more-code'>";
          os << "<div class='mzn-fundecl-body'>";
          os << body_os.str();
          os << "</div>\n";
          os << "(standard decomposition from " << filename << ":" << f_body->loc().firstLine()
             << ")";
          os << "</div>";
        }
      }

      os << "</div>\n<div class='mzn-fundecl-doc'>\n";

      if (fi->id().c_str()[0] == '\'') {
        std::string op = fi->id().substr(1, fi->id().size() - 2);
        ;
        const char* space = (op[0] >= 'a' ? " " : "");
        if (fi->paramCount() == 2) {
          os << "<p>Usage: <span class=\"mzn-arg\">" << *fi->param(0)->id() << space << op << space
             << *fi->param(1)->id() << "</span></p>";
        } else if (fi->paramCount() == 1) {
          os << "<p>Usage: <span class=\"mzn-arg\">" << op << space << *fi->param(0)->id()
             << "</span></p>";
        }
      }

      std::string dshtml = addHTML(ds);

      os << dshtml;
      if (!params.empty()) {
        os << "<div class='mzn-fundecl-params-heading'>Constants</div>\n";
        os << "<ul class='mzn-fundecl-params'>\n";
        for (auto& param : params) {
          os << "<li><span class='mzn-arg'>" << param.first << "</span>: " << param.second
             << "</li>\n";
        }
        os << "</ul>\n";
      }
      os << "</div>";
      os << "</div>";
      HtmlDocOutput::DocItem::DocType dt = (fi->ti()->type() == Type::ann() && fi->e() == nullptr)
                                               ? HtmlDocOutput::DocItem::T_ANN
                                               : HtmlDocOutput::DocItem::T_FUN;
      HtmlDocOutput::DocItem di(dt, Printer::quoteId(fi->id()), sig, os.str());
      HtmlDocOutput::add_to_group(_maingroup, group, di);
    }
  }
};

std::vector<HtmlDocument> HtmlPrinter::printHtml(EnvI& env, MiniZinc::Model* m,
                                                 const std::string& basename, int splitLevel,
                                                 bool includeStdLib, bool generateIndex) {
  using namespace HtmlDocOutput;
  Group g(basename, basename);
  PrintHtmlVisitor phv(env, g, includeStdLib);
  iter_items(phv, m);

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
    IndexEntry(std::string id0, std::string sig0, std::string link0, std::string groupName0)
        : id(std::move(id0)),
          sig(std::move(sig0)),
          link(std::move(link0)),
          groupName(std::move(groupName0)) {
      size_t spacepos = id.find_last_of(' ');
      if (spacepos != std::string::npos) {
        id = id.substr(spacepos + 1);
      }
    }
    bool operator<(const IndexEntry& e) const {
      if ((isalpha(id[0]) == 0) && (isalpha(e.id[0]) != 0)) {
        return true;
      }
      return id == e.id ? groupName < e.groupName : id < e.id;
    }
  };
  std::vector<IndexEntry> index;

  std::vector<SI> stack;
  stack.emplace_back(&g, nullptr, 0, 0);
  while (!stack.empty()) {
    Group& g = *stack.back().g;
    int curLevel = stack.back().level;
    int curIdx = stack.back().idx;
    Group* p = stack.back().p;
    stack.pop_back();
    for (const auto& it : g.items) {
      index.emplace_back(it.id, it.sig, g.fullPath, g.htmlName);
    }
    ret.emplace_back(g.fullPath, g.htmlName,
                     g.toHTML(curLevel, splitLevel, p, curIdx, basename, generateIndex));
    if (curLevel < splitLevel) {
      for (unsigned int i = 0; i < g.subgroups.m.size(); i++) {
        stack.emplace_back(g.subgroups.m[i], &g, curLevel + 1, i);
      }
    }
  }

  if (generateIndex) {
    std::sort(index.begin(), index.end());
    std::ostringstream oss;
    index.emplace_back("", "", "", "");

    std::vector<std::string> idxSections;

    if (!index.empty()) {
      if (isalpha(index[0].id[0]) != 0) {
        char idxSec_c = (char)toupper(index[0].id[0]);
        std::string idxSec(&idxSec_c, 1);
        oss << "<h3 id='Idx" << idxSec << "'>" << idxSec << "</h3>\n";
        idxSections.push_back(idxSec);
      } else {
        oss << "<h3 id='IdxSymbols'>Symbols</h3>\n";
        idxSections.emplace_back("Symbols");
      }
    }
    oss << "<ul>\n";
    std::string prevId = index.empty() ? "" : index[0].id;
    std::vector<IndexEntry> curEntries;
    for (auto ie : index) {
      if (ie.id != prevId) {
        oss << "<li>";
        assert(!curEntries.empty());
        IndexEntry& cur = curEntries[0];
        if (curEntries.size() == 1) {
          oss << cur.id << " <a href='" << cur.link << ".html#"
              << HtmlDocOutput::make_html_id(cur.sig) << "'>" << "(" << cur.groupName << ")</a>";
        } else {
          oss << cur.id << " (";
          bool first = true;
          for (const auto& i_ie : curEntries) {
            if (first) {
              first = false;
            } else {
              oss << ", ";
            }
            oss << "<a href='" << i_ie.link << ".html#" << HtmlDocOutput::make_html_id(i_ie.sig)
                << "'>";
            oss << i_ie.groupName << "</a>";
          }
          oss << ")";
        }
        oss << "</li>\n";
        curEntries.clear();
      }
      if ((isalpha(ie.id[0]) != 0) && ie.id[0] != prevId[0]) {
        char idxSec_c = (char)toupper(ie.id[0]);
        std::string idxSec(&idxSec_c, 1);
        oss << "</ul>\n<h3 id='Idx" << idxSec << "'>" << idxSec << "</h3><ul>";
        idxSections.push_back(idxSec);
      }
      prevId = ie.id;
      if (curEntries.empty() || curEntries.back().groupName != ie.groupName) {
        curEntries.push_back(ie);
      }
    }
    oss << "</ul>\n";

    std::ostringstream oss_header;
    oss_header << "<div class='mzn-group-level-0'>\n";
    oss_header << "<div class='mzn-group-nav'>";
    oss_header << "<a class='mzn-nav-up' href='" << g.getAnchor(0, 1) << "' title='" << g.htmlName
               << "'>&#8679;</a> ";
    bool first = true;
    for (const auto& is : idxSections) {
      if (first) {
        first = false;
      } else {
        oss_header << " | ";
      }
      oss_header << "<a href='#Idx" << is << "'>" << is << "</a>";
    }

    oss_header << "</div>";

    oss_header << "<div class='mzn-group-name'>Index</div>\n";

    HtmlDocument idx("doc-index", "Index", oss_header.str() + oss.str());
    ret.push_back(idx);
  }
  return ret;
}

namespace {

std::vector<std::string> replace_args_rst(std::string& s) {
  std::vector<std::string> replacements;
  std::ostringstream oss;
  size_t lastpos = 0;
  size_t pos = std::min(s.find("\\a"), s.find("\\p"));
  size_t mathjax_open = s.find("\\(");
  size_t mathjax_close = s.rfind("\\)");
  while (pos != std::string::npos) {
    oss << s.substr(lastpos, pos - lastpos);
    size_t start = pos;
    while (start < s.size() && s[start] != ' ' && s[start] != '\t') {
      start++;
    }
    while (start < s.size() && (s[start] == ' ' || s[start] == '\t')) {
      start++;
    }
    size_t end = start + 1;
    while (end < s.size() && ((isalnum(s[end]) != 0) || s[end] == '_')) {
      end++;
    }
    bool needSpace = pos != 0 && s[pos - 1] != ' ' && s[pos - 1] != '\n';
    bool needEndSpace = end != s.size() && s[end] != '\n' && s[end] != ',' && s[end] != '.';
    if (s[pos + 1] == 'a') {
      replacements.push_back(s.substr(start, end - start));
      if (pos >= mathjax_open && pos <= mathjax_close) {
        oss << "{\\bf " << replacements.back() << "}";
      } else {
        oss << (needSpace ? "\\ " : "") << "``" << replacements.back() << "``";
        if (needEndSpace) {
          oss << "\\ ";
        }
      }
    } else {
      if (pos >= mathjax_open && pos <= mathjax_close) {
        oss << "{\\bf " << s.substr(start, end - start) << "}";
      } else {
        oss << (needSpace ? "\\ " : "") << "``" << s.substr(start, end - start) << "``";
        if (needEndSpace) {
          oss << "\\ ";
        }
      }
    }
    lastpos = end;
    pos = std::min(s.find("\\a", lastpos), s.find("\\p", lastpos));
  }
  oss << s.substr(lastpos, std::string::npos);
  s = oss.str();

  std::ostringstream oss2;
  pos = std::min(s.find("\\("), s.find("\\)"));
  lastpos = 0;
  while (pos != std::string::npos) {
    if (s[pos + 1] == ')') {
      // remove trailing whitespace
      std::string t = s.substr(lastpos, pos - lastpos);
      size_t t_end = t.find_last_not_of(' ');
      if (t_end != std::string::npos) {
        t_end++;
      }
      oss2 << t.substr(0, t_end);
    } else {
      oss2 << s.substr(lastpos, pos - lastpos);
    }
    lastpos = pos + 2;
    if (s[pos + 1] == '(') {
      oss2 << ":math:`";
      lastpos = s.find_first_not_of(' ', lastpos);
    } else {
      oss2 << "`";
    }
    pos = std::min(s.find("\\(", lastpos), s.find("\\)", lastpos));
  }
  oss2 << s.substr(lastpos, std::string::npos);
  s = oss2.str();

  std::ostringstream oss3;
  pos = std::min(s.find("\\["), s.find("\\]"));
  lastpos = 0;
  while (pos != std::string::npos) {
    if (s[pos + 1] == ']') {
      // remove trailing whitespace
      std::string t = s.substr(lastpos, pos - lastpos);
      size_t t_end = t.find_last_not_of(' ');
      if (t_end != std::string::npos) {
        t_end++;
      }
      oss3 << t.substr(0, t_end);
    } else {
      oss3 << s.substr(lastpos, pos - lastpos);
    }
    lastpos = pos + 2;
    if (s[pos + 1] == '[') {
      oss3 << ":mzn:`";
      lastpos = s.find_first_not_of(' ', lastpos);
    } else {
      oss3 << "`";
    }
    pos = std::min(s.find("\\[", lastpos), s.find("\\]", lastpos));
  }
  oss3 << s.substr(lastpos, std::string::npos);
  s = oss3.str();

  std::ostringstream oss4;
  pos = s.find('|');
  lastpos = 0;
  while (pos != std::string::npos) {
    oss4 << s.substr(lastpos, pos - lastpos) << "\\|";
    lastpos = pos + 1;
    pos = s.find('|', lastpos);
  }
  oss4 << s.substr(lastpos, std::string::npos);
  s = oss4.str();

  return replacements;
}

std::pair<std::string, std::string> extract_arg_line_rst(std::string& s, size_t n) {
  size_t start = n;
  while (start < s.size() && s[start] != ' ' && s[start] != '\t') {
    start++;
  }
  while (start < s.size() && (s[start] == ' ' || s[start] == '\t')) {
    start++;
  }
  size_t end = start + 1;
  while (end < s.size() && s[end] != ':') {
    end++;
  }
  std::string arg = s.substr(start, end - start);
  size_t doc_start = end + 1;
  while (end < s.size() && s[end] != '\n') {
    end++;
  }
  std::string ret = s.substr(doc_start, end - doc_start);
  replace_args_rst(ret);
  s = s.substr(0, n) + s.substr(end, std::string::npos);
  return make_pair(arg, ret);
}

std::string common_string_prefix(const std::string& s, const std::string& t) {
  std::string prefix;
  unsigned int lastSpace = 0;
  for (unsigned int i = 0; i < std::min(s.size(), t.size()); i++) {
    if (s[i] != t[i]) {
      break;
    }
    if (s[i] == ' ') {
      lastSpace = i;
    } else if (i == std::min(s.size(), t.size()) - 1) {
      lastSpace = i + 1;
    }
    prefix += s[i];
  }
  return prefix.substr(0, lastSpace);
}

unsigned int max_line_width(const std::string& s) {
  unsigned int mlw = 0;
  unsigned int curlw = 0;
  for (auto c : s) {
    if (c == '\n') {
      mlw = std::max(mlw, curlw);
      curlw = 0;
    } else if ((c & 0xc0) != 0x80) {
      curlw++;
    }
  }
  mlw = std::max(mlw, curlw);
  return mlw;
}

struct FunDoc {
  std::string ident;
  std::string sig;
  std::string sigPretty;
  std::string doc;
  FunDoc(std::string ident0, std::string sig0, std::string sigPretty0, std::string doc0)
      : ident(std::move(ident0)),
        sig(std::move(sig0)),
        sigPretty(std::move(sigPretty0)),
        doc(std::move(doc0)) {}
};

}  // namespace

typedef std::map<std::string, std::vector<FunctionI*>> RSTFunMap;
typedef std::map<std::string, RSTFunMap> RSTGroupFunMap;

class RSTGroupVisitor : public ItemVisitor {
protected:
  EnvI& _env;
  HtmlDocOutput::Group& _maingroup;
  RSTGroupFunMap& _groupFunMap;
  bool _includeStdLib;

public:
  RSTGroupVisitor(EnvI& env, HtmlDocOutput::Group& mg, RSTGroupFunMap& groupFunMap,
                  bool includeStdLib)
      : _env(env), _maingroup(mg), _groupFunMap(groupFunMap), _includeStdLib(includeStdLib) {}
  bool enterModel(Model* m) {
    if (!_includeStdLib && FileUtils::base_name(m->filename().c_str()) == "stdlib.mzn") {
      return false;
    }
    const std::string& dc = m->docComment();
    if (!dc.empty()) {
      size_t gpos = dc.find("@groupdef");
      while (gpos != std::string::npos) {
        size_t start = gpos;
        while (start < dc.size() && dc[start] != ' ' && dc[start] != '\t') {
          start++;
        }
        while (start < dc.size() && (dc[start] == ' ' || dc[start] == '\t')) {
          start++;
        }
        size_t end = start + 1;
        while (end < dc.size() && ((isalnum(dc[end]) != 0) || dc[end] == '_' || dc[end] == '.')) {
          end++;
        }
        std::string groupName = dc.substr(start, end - start);
        size_t doc_start = end + 1;
        while (end < dc.size() && dc[end] != '\n') {
          end++;
        }
        std::string groupHTMLName = dc.substr(doc_start, end - doc_start);
        end++;

        size_t next = dc.find("@groupdef", gpos + 1);
        std::string groupDesc = dc.substr(end, next == std::string::npos ? next : next - end);
        replace_args_rst(groupDesc);
        HtmlDocOutput::set_group_desc(_maingroup, groupName, groupHTMLName, groupDesc);
        gpos = next;
      }
    }
    return true;
  }

  std::string outputFuncDocs(const std::vector<FunDoc>& funDocs, const std::string& group,
                             const std::string& ident) {
    // Check whether all functions share the same documentation
    bool allCommon = true;
    for (unsigned int i = 1; i < funDocs.size(); i++) {
      if (funDocs[i].doc != funDocs[i - 1].doc) {
        allCommon = false;
        break;
      }
    }

    std::ostringstream os;
    std::string myMainGroup = group.substr(0, group.find_first_of('.'));
    auto it = _maingroup.subgroups.find(myMainGroup);
    os << ".. index::\n";
    if (it != _maingroup.subgroups.m.end()) {
      os << "   pair: " << (*it)->htmlName << "; " << ident << "\n\n";
    } else {
      std::cerr << "did not find " << myMainGroup << "\n";
      os << "   single: " << ident << "\n\n";
    }

    std::ostringstream os_code;

    os_code << ".. code-block:: minizinc\n\n";

    if (allCommon) {
      for (const auto& fd : funDocs) {
        os_code << fd.sigPretty << "\n";
      }
    } else {
      int funCount = 1;
      for (const auto& fd : funDocs) {
        std::ostringstream number;
        number << "  " << std::setw(2) << funCount++ << ".";
        std::string number_s = number.str();
        os_code << number_s;
        auto indent = number_s.size();
        std::string sigString = fd.sigPretty;
        for (auto c : sigString) {
          os_code << c;
          if (c == '\n') {
            os_code << std::string(indent, ' ');
          }
        }
        os_code << "\n\n";
      }
    }
    os_code << "\n\n";

    std::ostringstream os_doc;

    if (allCommon) {
      os_doc << funDocs[0].doc;
    } else {
      std::vector<FunDoc> funDocsPlusEmpty = funDocs;
      funDocsPlusEmpty.emplace_back("", "", "", "");
      int funCount = 1;
      int startDoc = 1;
      std::string curDoc = funDocs[0].doc;
      for (const auto& fd : funDocsPlusEmpty) {
        if (fd.doc != curDoc) {
          // This is a new documentation, output previous
          std::ostringstream number;
          if (funCount - 1 == startDoc) {
            number << (funCount - 1) << ".\n";
          } else if (funCount - 2 == startDoc) {
            number << (funCount - 2) << ", " << (funCount - 1) << ".\n";
          } else {
            number << startDoc << "-" << (funCount - 1) << ".\n";
          }
          std::string number_s = number.str();
          os_doc << number_s << "  .. container:: mzncodedoc\n\n";
          auto indent = 4;  // number_s.size();
          std::string docString = curDoc;
          os_doc << std::string(indent, ' ');
          for (auto c : docString) {
            os_doc << c;
            if (c == '\n') {
              os_doc << std::string(indent, ' ');
            }
          }
          os_doc << "\n\n";
          startDoc = funCount;
          curDoc = fd.doc;
        }
        funCount++;
      }
    }

    auto code = os_code.str();
    auto doc = os_doc.str();

    auto codeRef = ".. _mzn_ref_" + group + "." + HtmlDocOutput::ident_to_label(ident) + ":";

    auto maxLineWidth = max_line_width(code);
    maxLineWidth = std::max(maxLineWidth, static_cast<unsigned int>(codeRef.size()));
    maxLineWidth = std::max(maxLineWidth, max_line_width(doc));

    os << ".. _mzn_" << group << "." << HtmlDocOutput::ident_to_label(ident) << ":\n\n";
    os << ".. rst-class:: mzn-docitem\n\n";

    os << ".. only:: builder_html\n\n";
    {
      os << "  " << ident << "\n";
      os << "  " << std::string(ident.size(), '!') << "\n\n";

      // Output as table
      os << "  .. container:: mznDocTable\n\n";

      os << "    +-" << std::string(maxLineWidth, '-') << "-+" << "\n";
      os << "    | " << codeRef << std::string(maxLineWidth - codeRef.size(), ' ') << " |\n";
      os << "    | " << std::string(maxLineWidth, ' ') << " |\n";
      unsigned int curLineWidth = 0;
      for (auto c : code) {
        if (curLineWidth == 0) {
          os << "    | ";
        }
        if (c == '\n') {
          // fill with space
          os << std::string(maxLineWidth - curLineWidth, ' ');
          os << " |\n";
          curLineWidth = 0;
        } else {
          os << c;
          if ((c & 0xc0) != 0x80) {
            curLineWidth++;
          }
        }
      }
      if (curLineWidth != 0) {
        os << std::string(maxLineWidth - curLineWidth, ' ');
        os << " |\n";
      }
      os << "    +-" << std::string(maxLineWidth, '-') << "-+" << "\n";
      curLineWidth = 0;
      for (auto c : doc) {
        if (curLineWidth == 0) {
          os << "    | ";
        }
        if (c == '\n') {
          // fill with space
          os << std::string(maxLineWidth - curLineWidth, ' ');
          os << " |\n";
          curLineWidth = 0;
        } else {
          os << c;
          if ((c & 0xc0) != 0x80) {
            curLineWidth++;
          }
        }
      }
      if (curLineWidth != 0) {
        os << std::string(maxLineWidth - curLineWidth, ' ');
        os << " |\n";
      }
      os << "    +-" << std::string(maxLineWidth, '-') << "-+" << "\n\n";
    }

    os << ".. only:: builder_latex\n\n";
    {
      // Output without table
      unsigned int curLineWidth = 0;
      for (auto c : code) {
        if (curLineWidth == 0) {
          os << "  ";
        }
        os << c;
        if (c == '\n') {
          curLineWidth = 0;
        } else if ((c & 0xc0) != 0x80) {
          curLineWidth++;
        }
      }
      if (curLineWidth != 0) {
        os << "\n";
      }
      os << "\n";
      curLineWidth = 0;
      for (auto c : doc) {
        if (curLineWidth == 0) {
          os << "  ";
        }
        os << c;
        if (c == '\n') {
          curLineWidth = 0;
        } else if ((c & 0xc0) != 0x80) {
          curLineWidth++;
        }
      }
      if (curLineWidth != 0) {
        os << "\n";
      }
      os << "\n\n";
      os << "  .. raw:: latex" << "\n\n";
      os << "     \\noindent\\makebox[\\linewidth]{\\rule{\\textwidth}{0.4pt}}\n\n";
    }
    return os.str();
  }

  /// Visit variable declaration
  void vVarDeclI(VarDeclI* vdi) {
    if (Call* docstring = Expression::dynamicCast<Call>(
            get_annotation(Expression::ann(vdi->e()), _env.constants.ann.doc_comment))) {
      std::string ds = eval_string(_env, docstring->arg(0));
      std::string group("main");
      size_t group_idx = ds.find("@group");
      if (group_idx != std::string::npos) {
        group = HtmlDocOutput::extract_arg_word(ds, group_idx);
      }

      std::string sig =
          vdi->e()->type().toString(_env) + " " + Printer::quoteId(vdi->e()->id()->str());

      std::ostringstream os_sig;
      if (vdi->e()->ti()->type() == Type::ann()) {
        os_sig << "  annotation " << Printer::quoteId(vdi->e()->id()->str());
      } else {
        os_sig << "  " << *vdi->e()->ti() << ": " << Printer::quoteId(vdi->e()->id()->str());
      }

      FunDoc funDoc = FunDoc(Printer::quoteId(vdi->e()->id()->str()), sig, os_sig.str(),
                             HtmlDocOutput::trim(ds));

      GCLock lock;
      HtmlDocOutput::DocItem::DocType dt =
          vdi->e()->type().isPar() ? (vdi->e()->type().isAnn() ? HtmlDocOutput::DocItem::T_ANN
                                                               : HtmlDocOutput::DocItem::T_PAR)
                                   : HtmlDocOutput::DocItem::T_VAR;
      HtmlDocOutput::DocItem di(
          dt, Printer::quoteId(vdi->e()->id()->str()), sig,
          outputFuncDocs({funDoc}, group, Printer::quoteId(vdi->e()->id()->str())));
      HtmlDocOutput::add_to_group(_maingroup, group, di);
    }
  }

  void vFunctionI(FunctionI* fi) {
    Expression* ann = get_annotation(fi->ann(), _env.constants.ann.doc_comment);
    if (Call* docstring = Expression::dynamicCast<Call>(ann)) {
      std::string ds = eval_string(_env, docstring->arg(0));
      std::string group("main");
      size_t group_idx = ds.find("@group");
      if (group_idx != std::string::npos) {
        group = HtmlDocOutput::extract_arg_word(ds, group_idx);
      }
      auto& groupMap = _groupFunMap.emplace(group, RSTFunMap()).first->second;
      auto& funs = groupMap.emplace(fi->id().c_str(), std::vector<FunctionI*>()).first->second;
      funs.push_back(fi);
    }
  }

  void outputFunctions() {
    for (const auto& g : _groupFunMap) {
      for (const auto& f : g.second) {
        std::vector<FunDoc> funDocs;
        bool isAnn = true;
        for (auto* fi : f.second) {
          isAnn = isAnn && fi->ti()->type() == Type::ann() && fi->e() == nullptr;
          Expression* ann = get_annotation(fi->ann(), _env.constants.ann.doc_comment);
          if (Call* docstring = Expression::dynamicCast<Call>(ann)) {
            std::string ds = eval_string(_env, docstring->arg(0));
            std::string group("main");
            size_t group_idx = ds.find("@group");
            if (group_idx != std::string::npos) {
              group = HtmlDocOutput::extract_arg_word(ds, group_idx);
            }

            size_t param_idx = ds.find("@param");
            std::vector<std::pair<std::string, std::string>> params;
            while (param_idx != std::string::npos) {
              params.push_back(extract_arg_line_rst(ds, param_idx));
              param_idx = ds.find("@param");
            }

            std::vector<std::string> args = replace_args_rst(ds);

            std::unordered_set<std::string> allArgs;
            for (auto& arg : args) {
              allArgs.insert(arg);
            }
            for (auto& param : params) {
              allArgs.insert(param.first);
            }

            GCLock lock;
            for (unsigned int i = 0; i < fi->paramCount(); i++) {
              if (allArgs.find(std::string(fi->param(i)->id()->str().c_str(),
                                           fi->param(i)->id()->str().size())) == allArgs.end()) {
                std::cerr << "Warning: parameter " << *fi->param(i)->id()
                          << " not documented for function " << fi->id() << " at location "
                          << fi->loc() << "\n";
              }
            }

            std::string sig;
            {
              GCLock lock;
              std::vector<VarDecl*> params(fi->paramCount());
              for (unsigned int i = 0; i < fi->paramCount(); i++) {
                params[i] = fi->param(i);
              }
              auto* fi_c = new FunctionI(Location(), fi->id(), fi->ti(), params);
              std::ostringstream oss_sig;
              oss_sig << *fi_c;
              sig = oss_sig.str();
              sig.resize(sig.size() - 2);
            }

            std::ostringstream os_sig;
            std::ostringstream os_doc;
            std::ostringstream fs;

            if (fi->ti()->type() == Type::ann()) {
              fs << "annotation ";
            } else if (fi->ti()->type() == Type::parbool()) {
              fs << "test ";
            } else if (fi->ti()->type() == Type::varbool()) {
              fs << "predicate ";
            } else {
              fs << "function " << *fi->ti() << ": ";
            }
            fs << Printer::quoteId(fi->id()) << "(";
            os_sig << "  " << fs.str();
            size_t align = fs.str().size();
            for (unsigned int i = 0; i < fi->paramCount(); i++) {
              fs << *fi->param(i)->ti();
              std::ostringstream fid;
              fid << *fi->param(i)->id();
              if (!fid.str().empty()) {
                fs << ": " << Printer::quoteId(fi->param(i)->id()->str());
              }
              if (i < fi->paramCount() - 1) {
                fs << ", ";
              }
            }
            bool splitArgs = (fs.str().size() > 70);
            for (unsigned int i = 0; i < fi->paramCount(); i++) {
              os_sig << *fi->param(i)->ti();
              std::ostringstream fid;
              fid << *fi->param(i)->id();
              if (!fid.str().empty()) {
                os_sig << ": " << Printer::quoteId(fi->param(i)->id()->str());
              }
              if (i < fi->paramCount() - 1) {
                os_sig << ",";
                if (splitArgs) {
                  os_sig << "\n  ";
                  for (auto j = static_cast<unsigned int>(align); (j--) != 0U;) {
                    os_sig << " ";
                  }
                } else {
                  os_sig << " ";
                }
              }
            }
            os_sig << ")";

            os_doc << HtmlDocOutput::trim(ds) << "\n\n";
            if (fi->id().c_str()[0] == '\'') {
              std::string op = fi->id().substr(1, fi->id().size() - 2);
              if (fi->paramCount() == 2) {
                os_doc << "Usage: ``" << *fi->param(0)->id() << " " << op << " "
                       << *fi->param(1)->id() << "``\n\n";
              } else if (fi->paramCount() == 1) {
                os_doc << "Usage: ``" << op << " " << *fi->param(0)->id() << "``\n\n";
              }
            }

            if (!params.empty()) {
              os_doc << "Parameters:\n\n";
              for (auto& param : params) {
                os_doc << "- ``" << param.first << "``: " << param.second << "\n";
              }
              os_doc << "\n";
            }
            os_doc << "\n";

            auto os_doc_s = os_doc.str();
            auto first_non_ws = os_doc_s.find_first_not_of(" \n");
            if (first_non_ws == std::string::npos) {
              first_non_ws = 0;
            }
            auto last_non_ws = os_doc_s.find_last_not_of(" \n");
            if (last_non_ws == std::string::npos) {
              last_non_ws = os_doc_s.size() - 1;
            }
            os_doc_s = os_doc_s.substr(first_non_ws, last_non_ws - first_non_ws + 1);

            funDocs.emplace_back(Printer::quoteId(fi->id()), sig, os_sig.str(), os_doc_s);
          }
        }

        // Now check whether all functions share the same documentation
        bool allCommon = true;
        for (unsigned int i = 1; i < funDocs.size(); i++) {
          if (funDocs[i].doc != funDocs[i - 1].doc) {
            allCommon = false;
            break;
          }
        }

        std::ostringstream os;
        std::string myMainGroup = g.first.substr(0, g.first.find_first_of('.'));
        auto it = _maingroup.subgroups.find(myMainGroup);
        os << ".. index::\n";
        if (it != _maingroup.subgroups.m.end()) {
          os << "   pair: " << (*it)->htmlName << "; " << f.first << "\n\n";
        } else {
          std::cerr << "did not find " << myMainGroup << "\n";
          os << "   single: " << f.first << "\n\n";
        }

        std::string codeblock = outputFuncDocs(funDocs, g.first, f.first);
        HtmlDocOutput::DocItem::DocType dt =
            isAnn ? HtmlDocOutput::DocItem::T_ANN : HtmlDocOutput::DocItem::T_FUN;

        HtmlDocOutput::DocItem di(dt, f.first, funDocs.front().sig, codeblock);
        HtmlDocOutput::add_to_group(_maingroup, g.first, di);
      }
    }
  }
};

std::vector<HtmlDocument> RSTPrinter::printRST(EnvI& env, MiniZinc::Model* m,
                                               const std::string& basename, int splitLevel,
                                               bool includeStdLib, bool generateIndex) {
  using namespace HtmlDocOutput;
  Group g(basename, basename);
  RSTGroupFunMap groupFunMap;
  RSTGroupVisitor rgv(env, g, groupFunMap, includeStdLib);
  iter_items(rgv, m);
  rgv.outputFunctions();

  std::vector<HtmlDocument> ret;

  {
    std::ostringstream oss;
    oss << ".. _ch-" << g.fullPath << ":\n\n";
    oss << Group::rstHeading(g.htmlName, 0);
    oss << trim(g.desc) << "\n";
    oss << ".. toctree::\n";
    oss << "  :maxdepth: 2\n\n";
    for (auto* sg : g.subgroups.m) {
      oss << "  " << sg->fullPath << "\n";
    }
    ret.emplace_back(g.fullPath, g.htmlName, oss.str());
  }

  for (auto* sg : g.subgroups.m) {
    if (!sg->items.empty()) {
      // Need to create sub-group so these don't get lost
      auto* others = new Group("otherdeclarations", sg->fullPath + "-otherdeclarations");
      others->htmlName = "Other declarations";
      others->items = sg->items;
      sg->subgroups.m.push_back(others);
    }

    // Split sub-group into sub-pages
    std::ostringstream oss;
    oss << ".. _ch-" << sg->fullPath << ":\n\n";
    oss << Group::rstHeading(sg->htmlName, 0);
    oss << trim(sg->desc) << "\n";
    oss << ".. toctree::\n";
    //    oss << "  :hidden:\n\n";
    for (auto* ssg : sg->subgroups.m) {
      oss << "  " << ssg->fullPath << "\n";
    }
    oss << "\n";
    ret.emplace_back(sg->fullPath, sg->htmlName, oss.str());

    for (auto* ssg : sg->subgroups.m) {
      ret.emplace_back(ssg->fullPath, ssg->htmlName, ssg->toRST(0));
    }
  }
  return ret;
}

}  // namespace MiniZinc
