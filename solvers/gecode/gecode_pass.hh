
#ifndef __MINIZINC_GECODE_PASS_HH__
#define __MINIZINC_GECODE_PASS_HH__

#include <minizinc/flatten.hh>

namespace MiniZinc {

  class GecodePropagationPass : public Pass {
    std::string library;
    bool _sac;
    bool _shave;
    bool _bounds;
    unsigned int _npass;

    public:
    GecodePropagationPass(FlatteningOptions& opts, std::string library, bool sac, bool shave, bool bounds, unsigned int npass);

    std::string getLibrary();
    void run(Env& e);
  };

}

#endif
