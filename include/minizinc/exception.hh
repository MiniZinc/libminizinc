#include <exception>

#include <minizinc/ast.hh>
#include <string>

namespace MiniZinc {

  class TypeError : public std::exception {
  protected:
    Location _loc;
    std::string _msg;
  public:
    TypeError(const Location& loc, const std::string& msg)
      : _loc(loc), _msg(msg) {}
    ~TypeError(void) throw() {}
    virtual const char* what(void) const throw() {
      return "MiniZinc: type error";
    }
    const Location& loc(void) const { return _loc; }
    const std::string& msg(void) const { return _msg; }
  };

}
