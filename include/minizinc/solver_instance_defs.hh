#ifndef SOLVER_INSTANCE_DEFS_HH
#define SOLVER_INSTANCE_DEFS_HH

#include <minizinc/flatten.hh>

namespace MiniZinc {

template <class Var>
class MultipleObjectivesTemplate {
public:
  class Objective {
  public:
    Objective() {}
    Objective(Var v, double w) : _objVar{v}, _weight(w) {}
    Var getVariable() const { return _objVar; }
    double getWeight() const { return _weight; }
    void setVariable(Var vd) { _objVar = vd; }
    void setWeight(double w) { _weight = w; }

  private:
    Var _objVar{};
    double _weight = 1.0;
  };
  using ArrayOfObjectives = std::vector<Objective>;
  const ArrayOfObjectives& getObjectives() const { return _objs; }
  size_t size() const { return _objs.size(); }
  void add(const Objective& obj) { _objs.push_back(obj); }

private:
  ArrayOfObjectives _objs;
};

/// Typical MultipleObjectives
using MultipleObjectives = MultipleObjectivesTemplate<Expression*>;

}  // namespace MiniZinc

#endif  // SOLVER_INSTANCE_DEFS_HH
