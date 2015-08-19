
#ifndef __MINIZINC_GECODE_PASS_HH__
#define __MINIZINC_GECODE_PASS_HH__

#include <minizinc/flatten.hh>
#include <minizinc/options.hh>

namespace MiniZinc {

  class GecodePass : public Pass {
    Options gopts;

    public:
    GecodePass(Options& gopts);

    bool pre(Env* e);
    Env* run(Env* e);
  };

}

#endif
