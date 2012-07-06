#include <minizinc/context.hh>
#include <minizinc/exception.hh>

#include <algorithm>

namespace MiniZinc {

  void
  ASTContext::registerFn(FunctionI* fi) {
    FnMap::iterator i_id = fnmap.find(fi->_id);
    if (i_id == fnmap.end()) {
      // new element
      std::vector<FunctionI*> v; v.push_back(fi);
      fnmap.insert(std::pair<CtxStringH,std::vector<FunctionI*> >(fi->_id,v));
    } else {
      // add to list of existing elements
      std::vector<FunctionI*>& v = i_id->second;
      for (unsigned int i=0; i<v.size(); i++) {
        if (v[i]->_params->size() == fi->_params->size()) {
          bool alleq=true;
          for (unsigned int j=0; j<fi->_params->size(); j++) {
            if ((*v[i]->_params)[j]->_type != (*fi->_params)[j]->_type) {
              alleq=false; break;
            }
          }
          if (alleq)
            throw TypeError(fi->_loc,
              "function with the same type already defined");
        }
      }
      v.push_back(fi);
    }
  }

  namespace {
    class FunSort {
    public:
      bool operator()(FunctionI* x, FunctionI* y) const {
        if (x->_params->size() < y->_params->size())
          return true;
        if (x->_params->size() == y->_params->size()) {
          for (unsigned int i=0; i<x->_params->size(); i++) {
            switch ((*x->_params)[i]->_type.cmp((*y->_params)[i]->_type)) {
            case -1: return true;
            case 1: return false;
            }
          }
        }
        return false;
      }
    };
  }

  void
  ASTContext::sortFn(void) {
    FunSort funsort;
    for (FnMap::iterator it=fnmap.begin(); it!=fnmap.end(); ++it) {
      std::sort(it->second.begin(),it->second.end(),funsort);
    }

    // for (FnMap::iterator it=fnmap.begin(); it!=fnmap.end(); ++it) {
    //   for (unsigned int i=0; i<it->second.size(); i++) {
    //     std::cerr << "function ";
    //     std::cerr << it->second[i]->_ti->_type.toString() << " : ";
    //     std::cerr << it->second[i]->_id.str() << " (";
    //     for (unsigned int j=0; j<it->second[i]->_params->size(); j++) {
    //       std::cerr << (*it->second[i]->_params)[j]->_type.toString();
    //       if (j<it->second[i]->_params->size()-1) std::cerr<<",";
    //     }
    //     std::cerr << ")" << std::endl;
    //   }
    // }
  }

  FunctionI*
  ASTContext::matchFn(const CtxStringH& id,
                      const std::vector<Expression*>& args) const {
    FnMap::const_iterator it = fnmap.find(id);
    if (it == fnmap.end()) {
      return NULL;
    }
    const std::vector<FunctionI*>& v = it->second;
    for (unsigned int i=0; i<v.size(); i++) {
      FunctionI* fi = v[i];
      if (fi->_params->size() == args.size()) {
        bool match=true;
        for (unsigned int j=0; j<args.size(); j++) {
          if (!args[j]->_type.isSubtypeOf((*fi->_params)[j]->_type)) {
            match=false;
            break;
          }
        }
        if (match)
          return fi;
      }
    }
    return NULL;
  }
  
}
