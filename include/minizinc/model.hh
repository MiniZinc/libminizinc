#ifndef __MINIZINC_MODEL_HH__
#define __MINIZINC_MODEL_HH__

#include <vector>
#include <minizinc/ast.hh>

namespace MiniZinc {
  
  class Model {
  public:
    CtxStringH _filename;
    CtxStringH _filepath;
    Model* _parent;
    std::vector<Item*> _items;
    
    Model(void) : _filename(NULL), _filepath(NULL), _parent(NULL) {}
    
    void addItem(Item* i) { _items.push_back(i); }
    
    Model* parent(void) const { return _parent; }
    void setParent(Model* p) { assert(_parent==NULL); _parent = p; }
    
    CtxStringH filename(void) const { return _filename; }
    CtxStringH filepath(void) const { return _filepath; }
    
    void setFilename(const ASTContext& ctx, const std::string& f) {
      assert(_filename.size()==0);
      _filename = CtxStringH(ctx,f);
    }
    void setFilepath(const ASTContext& ctx, const std::string& f) {
      assert(_filepath.size()==0);
      _filepath = CtxStringH(ctx,f);
    }
  };
  
}

#endif
