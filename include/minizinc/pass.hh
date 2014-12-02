
#include <minizinc/flatten.hh>
#include <string>

namespace MiniZinc {
  class Pass {
    private:
      FlatteningOptions fopts;

    public:
      Pass(FlatteningOptions& opts) : fopts(opts) {};
      std::string getLibrary();
      FlatteningOptions& flatteningOptions() {return fopts;};
      void run(Env& env);
  };

  class CompilePass : public Pass {
    private:
      std::string library;

    public:
      CompilePass(FlatteningOptions& opts, std::string globals_library) : Pass(opts), library(globals_library) {}
      std::string getLibrary() { return library; }
      void run(Env& env) { };
  };
}

