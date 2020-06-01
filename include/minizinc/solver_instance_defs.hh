#ifndef SOLVER_INSTANCE_DEFS_HH
#define SOLVER_INSTANCE_DEFS_HH

#include <minizinc/flatten.hh>

namespace MiniZinc {

template <class Var>
class MultipleObjectivesTemplate {
public:
  class Objective {
  public:
    Objective() { }
    Objective(Var v, double w) : objVar_{v}, weight_(w) { }
    Var getVariable() const { return objVar_; }
    double getWeight() const { return weight_; }
    void setVariable(Var vd) { objVar_=vd; }
    void setWeight(double w) { weight_=w; }
  private:
    Var objVar_ {};
    double weight_ = 1.0;
  };
  using ArrayOfObjectives = std::vector<Objective>;
  const ArrayOfObjectives& getObjectives() const { return objs_; }
  size_t size() const { return objs_.size(); }
  void add(const Objective& obj) { objs_.push_back(obj); }
private:
  ArrayOfObjectives objs_;
};

/// Typical MultipleObjectives
using MultipleObjectives = MultipleObjectivesTemplate<Expression*>;

}

#endif // SOLVER_INSTANCE_DEFS_HH
