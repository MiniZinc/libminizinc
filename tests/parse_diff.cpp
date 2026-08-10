/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* Differential test harness for the bison and tree-sitter parsers.
 *
 * Reads file paths on stdin, one line per case, parses each with both parsers
 * and compares the result. Exits non-zero if any case differs. A line may name
 * several files; `.dzn` ones are parsed as data, which under tree-sitter means
 * the DataZinc grammar rather than the MiniZinc one.
 *
 *   build/parse_diff [include path...] < filelist
 *
 * Three things are compared: the printed model, a structural dump of every
 * expression (the printer reconstructs parentheses from its own precedence
 * table, so it can render two different trees identically), and whatever either
 * parser wrote as a diagnostic.
 */

#include <minizinc/astiterator.hh>
#include <minizinc/model.hh>
#include <minizinc/parser.hh>
#include <minizinc/prettyprinter.hh>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace {

/// Records what `Printer` does not show. Printed text is not enough on its own:
/// the printer reconstructs parentheses from its own precedence table, so two
/// differently shaped trees can print identically.
class AstDumper : public MiniZinc::EVisitor {
public:
  explicit AstDumper(std::ostream& os) : _os(os) {}

  /// Called for every expression, top-down, so the sequence encodes the shape of
  /// the tree as well as the type of each node.
  bool enter(MiniZinc::Expression* e) {
    _os << "  e" << MiniZinc::Expression::eid(e) << " t" << MiniZinc::Expression::type(e).toInt();
    switch (MiniZinc::Expression::eid(e)) {
      case MiniZinc::Expression::E_BINOP:
        _os << " " << MiniZinc::Expression::cast<MiniZinc::BinOp>(e)->opToString();
        break;
      case MiniZinc::Expression::E_UNOP:
        _os << " " << MiniZinc::Expression::cast<MiniZinc::UnOp>(e)->opToString();
        break;
      case MiniZinc::Expression::E_CALL:
        _os << " " << MiniZinc::Expression::cast<MiniZinc::Call>(e)->id();
        break;
      case MiniZinc::Expression::E_ID:
        _os << " " << MiniZinc::Expression::cast<MiniZinc::Id>(e)->str();
        break;
      default:
        break;
    }
    _os << "\n";
    return true;
  }

  /// `isEnum` is a flag on the TypeInst rather than part of the packed type
  void vTypeInst(const MiniZinc::TypeInst* ti) { _os << "  ti enum=" << ti->isEnum() << "\n"; }
  /// `toplevel` decides whether a declaration needs a right-hand side, and is
  /// not visible in the printed model.
  void vVarDecl(const MiniZinc::VarDecl* vd) {
    _os << "  vd toplevel=" << vd->toplevel() << " introduced=" << vd->introduced() << "\n";
  }

private:
  std::ostream& _os;
};

std::string renderModel(MiniZinc::Model* m) {
  std::ostringstream oss;
  MiniZinc::Printer p(oss, 0, false);
  p.print(m);
  AstDumper td(oss);
  auto walk = [&](MiniZinc::Expression* e) {
    if (e != nullptr) {
      MiniZinc::top_down(td, e);
    }
  };
  for (auto& item : *m) {
    if (auto* vdi = item->dynamicCast<MiniZinc::VarDeclI>()) {
      walk(vdi->e());
    } else if (auto* fi = item->dynamicCast<MiniZinc::FunctionI>()) {
      walk(fi->ti());
      for (unsigned int i = 0; i < fi->paramCount(); i++) {
        walk(fi->param(i));
      }
      walk(fi->e());
      for (MiniZinc::ExpressionSetIter it = fi->ann().begin(); it != fi->ann().end(); ++it) {
        walk(*it);
      }
    } else if (auto* ci = item->dynamicCast<MiniZinc::ConstraintI>()) {
      walk(ci->e());
    } else if (auto* ai = item->dynamicCast<MiniZinc::AssignI>()) {
      walk(ai->e());
    } else if (auto* si = item->dynamicCast<MiniZinc::SolveI>()) {
      walk(si->e());
      for (MiniZinc::ExpressionSetIter it = si->ann().begin(); it != si->ann().end(); ++it) {
        walk(*it);
      }
    } else if (auto* oi = item->dynamicCast<MiniZinc::OutputI>()) {
      walk(oi->e());
    }
  }
  return oss.str();
}

/// Splits a whitespace-separated line into model files and data files.
void split_case(const std::string& line, std::vector<std::string>& models,
                std::vector<std::string>& data) {
  std::istringstream ls(line);
  for (std::string p; ls >> p;) {
    if (p.size() > 4 && p.compare(p.size() - 4, 4, ".dzn") == 0) {
      data.push_back(p);
    } else {
      models.push_back(p);
    }
  }
}

std::string parseOnce(const std::vector<std::string>& models, const std::vector<std::string>& data,
                      const std::vector<std::string>& includePaths, bool treeSitter) {
  MiniZinc::use_tree_sitter_parser = treeSitter;
  std::ostringstream out;
  std::ostringstream errs;
  try {
    MiniZinc::Env env;
    // A data file on its own is rejected with "No model given" before anything
    // is parsed, which both parsers would report identically -- so give the data
    // an empty model to be read into.
    const char* stub = models.empty() ? "% data only\n" : "";
    MiniZinc::Model* m = MiniZinc::parse(env, models, data, stub, "_stub.mzn", includePaths, {},
                                         /*isFlatZinc=*/false,
                                         /*ignoreStdlib=*/getenv("PARSE_DIFF_STDLIB") == nullptr,
                                         /*parseDocComments=*/true, /*verbose=*/false, errs);
    if (m == nullptr) {
      out << "<no model>\n";
    } else {
      MiniZinc::GCLock lock;
      out << renderModel(m);
    }
  } catch (MiniZinc::Exception& e) {
    out << "EXCEPTION: ";
    e.print(out);
  } catch (std::exception& e) {
    out << "EXCEPTION: " << e.what() << "\n";
  }
  // Warnings are the other half of what the parsers have to agree on. Note this
  // is only what the parser itself writes: diagnostics raised during type
  // checking, such as the data-file call warning, are not reached from here.
  out << errs.str();
  return out.str();
}

/// Prints the first differing line with a little context.
void reportDiff(const std::string& path, const std::string& a, const std::string& b) {
  std::istringstream as(a);
  std::istringstream bs(b);
  std::string la;
  std::string lb;
  int line = 0;
  while (true) {
    bool ga = static_cast<bool>(std::getline(as, la));
    bool gb = static_cast<bool>(std::getline(bs, lb));
    if (!ga && !gb) {
      return;
    }
    line++;
    if (!ga || !gb || la != lb) {
      std::cout << "=== DIFF " << path << " (line " << line << ")\n"
                << "  bison: " << (ga ? la : "<eof>") << "\n"
                << "  tsitr: " << (gb ? lb : "<eof>") << "\n";
      return;
    }
  }
}

}  // namespace

int main(int argc, char** argv) {
  std::vector<std::string> includePaths;
  for (int i = 1; i < argc; i++) {
    includePaths.emplace_back(argv[i]);
  }
  int bad = 0;
  int total = 0;
  for (std::string path; std::getline(std::cin, path);) {
    if (path.empty()) {
      continue;
    }
    total++;
    // PARSE_DIFF_ONLY=bison|tsitr runs a single parser, for timing comparisons
    const char* only = getenv("PARSE_DIFF_ONLY");
    bool skipBison = only != nullptr && std::string(only) == "tsitr";
    bool skipTs = only != nullptr && std::string(only) == "bison";
    std::vector<std::string> models;
    std::vector<std::string> data;
    split_case(path, models, data);
    std::string withBison = skipBison ? "" : parseOnce(models, data, includePaths, false);
    std::string withTreeSitter = skipTs ? "" : parseOnce(models, data, includePaths, true);
    if (only != nullptr) {
      continue;
    }
    if (const char* dir = getenv("PARSE_DIFF_DUMP")) {
      std::ofstream(std::string(dir) + "/bison.txt") << withBison;
      std::ofstream(std::string(dir) + "/tsitr.txt") << withTreeSitter;
    }
    if (withBison != withTreeSitter) {
      bad++;
      reportDiff(path, withBison, withTreeSitter);
    }
  }
  std::cout << total - bad << "/" << total << " files identical" << std::endl;
  return bad == 0 ? 0 : 1;
}
