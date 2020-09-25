/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/flatten_internal.hh>
#include <minizinc/pathfileprinter.hh>

#include <sstream>

namespace MiniZinc {

using std::string;
using std::vector;

PathFilePrinter::PathFilePrinter(std::ostream& o, EnvI& /*env*/) : _os(o), _constraintIndex(0) {}

void PathFilePrinter::addBetterName(Id* id, const string& name, const string& path,
                                    bool overwrite = false) {
  string oname;
  string opath;

  auto it = _betternames.find(id);
  if (it != _betternames.end()) {
    oname = it->second.first;
    opath = it->second.second;
  }

  if (!name.empty() && (overwrite || oname.empty())) {
    oname = name;
  }
  if (!path.empty() && (overwrite || opath.empty())) {
    opath = path;
  }

  _betternames[id] = NamePair(oname, opath);
}

string path2name(const string& path) {
  std::stringstream name;

  size_t idpos = path.rfind("id:");
  if (idpos != string::npos) {
    idpos += 3;
    size_t semi = path.find(';', idpos);
    if (semi != string::npos) {
      // Variable name
      name << path.substr(idpos, semi - idpos);

      // Check for array
      int dim = 0;
      size_t ilpos = semi - idpos;
      do {
        ilpos = path.find("il:", ilpos);
        if (ilpos != string::npos) {
          ilpos += 3;
          semi = path.find(';', ilpos);
          if (semi != string::npos) {
            if (dim == 0) {
              name << "[";
            } else {
              name << ",";
            }
            name << path.substr(ilpos, semi - ilpos);
            dim++;
          }
        }
      } while (ilpos != string::npos);

      if (dim > 0) {
        name << "?]";
      }

      // Check for anon
      if (path.find(":anon") != string::npos || path.find('=') != string::npos) {
        name.str("");
        name.clear();
      }
    }
  }

  return name.str();
}

void PathFilePrinter::print(Model* m) {
  // Build map
  for (VarDeclIterator vdit = m->vardecls().begin(); vdit != m->vardecls().end(); ++vdit) {
    VarDecl* e = vdit->e();
    for (ExpressionSetIter it = e->ann().begin(); it != e->ann().end(); ++it) {
      if (Call* ca = (*it)->dynamicCast<Call>()) {
        ASTString cid = ca->id();
        if (cid == constants().ann.output_array) {
          if (auto* rhs = e->e()->dynamicCast<ArrayLit>()) {
            for (unsigned int ind = 0; ind < rhs->size(); ind++) {
              if (Id* id = (*rhs)[ind]->dynamicCast<Id>()) {
                std::stringstream bettername;
                bettername << *e->id() << "[";

                // Array of sets
                ArrayLit& dimsets = *ca->arg(0)->cast<ArrayLit>();
                vector<IntVal> dims(dimsets.size(), 1);
                for (unsigned int i = 0; i < dimsets.size(); i++) {
                  auto* sl = dimsets[i]->cast<SetLit>();
                  dims[i] = sl->isv()->card();
                }
                vector<IntVal> dimspan(dims.size(), 1);
                for (unsigned int i = 0; i < dims.size(); i++) {
                  for (unsigned int j = i + 1; j < dims.size(); j++) {
                    dimspan[i] *= dims[j];
                  }
                }

                IntVal curind = ind;
                for (unsigned int i = 0; i < dims.size() - 1; i++) {
                  IntVal thisind = curind / dimspan[i];
                  curind -= thisind * dimspan[i];
                  bettername << dimsets[i]->cast<SetLit>()->isv()->min() + thisind << ",";
                }
                bettername << dimsets[dimsets.size() - 1]->cast<SetLit>()->isv()->min() + curind
                           << "]";

                addBetterName(id, bettername.str(), "", true);
              }
            }
          }
        } else if (ca->id() == constants().ann.mzn_path) {
          auto* sl = ca->arg(0)->cast<StringLit>();
          addBetterName(e->id(), path2name(string(sl->v().c_str(), sl->v().size())),
                        string(sl->v().c_str()));
        }
      }
    }
  }

  // Print values
  for (Item* item : *m) {
    print(item);
  }
}

void PathFilePrinter::print(Item* item) {
  if (auto* vdi = item->dynamicCast<VarDeclI>()) {
    Id* id = vdi->e()->id();
    NamePair np = _betternames[id];
    if (!np.first.empty() || !np.second.empty()) {
      // FlatZinc name
      _os << *id << "\t";

      // Nice name
      if (np.first.empty()) {
        _os << *id << "\t";
      } else {
        string name = np.first;
        _os << name;
        if (name.find('?') != string::npos) {
          _os << "(" << *id << ")";
        }
        _os << "\t";
      }

      // Path
      _os << np.second << std::endl;
    }
  } else if (auto* ci = item->dynamicCast<ConstraintI>()) {
    StringLit* sl = nullptr;
    Expression* e = ci->e();
    for (ExpressionSetIter it = e->ann().begin(); it != e->ann().end(); ++it) {
      if (Call* ca = (*it)->dynamicCast<Call>()) {
        ASTString cid = ca->id();
        if (cid == constants().ann.mzn_path) {
          sl = ca->arg(0)->cast<StringLit>();
        }
      }
    }

    _os << _constraintIndex << "\t";
    _os << _constraintIndex << "\t";
    if (sl != nullptr) {
      _os << sl->v();
    } else {
      _os << "";
    }
    _os << std::endl;
    _constraintIndex++;
  }
}
}  // namespace MiniZinc
