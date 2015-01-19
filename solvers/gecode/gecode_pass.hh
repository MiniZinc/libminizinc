
#ifndef __MINIZINC_GECODE_PASS_HH__
#define __MINIZINC_GECODE_PASS_HH__

#include <minizinc/flatten.hh>
#include <minizinc/options.hh>

namespace MiniZinc {

  class GecodePass : public Pass {
    std::string library;
    bool _sac;
    bool _shave;
    bool _bounds;
    unsigned int _npass;
    Options& gopts;
    bool presolve_model;

    public:
    GecodePass(FlatteningOptions& opts,
               Options& gopts,
               std::string library,
               bool mod,
               bool sac,
               bool shave,
               bool bounds,
               unsigned int npass);

    std::string getLibrary();
    void run(Env& e);
  };

}

#endif
