
#ifndef __MINIZINC_GECODE_PASS_HH__
#define __MINIZINC_GECODE_PASS_HH__

#include <minizinc/flatten.hh>
#include <minizinc/options.hh>

namespace MiniZinc {

  class GecodePass : public Pass {
    std::string library;
    Options gopts;

    public:
    GecodePass(FlatteningOptions& opts,
               Options& gopts,
               std::string lib = "gecode");

    std::string getLibrary();
    void run(Env& e);
  };

}

#endif
