
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
    Options gopts;
    bool presolve_model;

    public:
    GecodePass(FlatteningOptions& opts,
               Options& gopts,
               std::string lib = "gecode",
               bool mod = false,
               bool sac = false,
               bool shave = false,
               bool bounds = true,
               unsigned int npass = 1);

    std::string getLibrary();
    void run(Env& e);
  };

}

#endif
