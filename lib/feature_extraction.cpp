#include <minizinc/eval_par.hh>
#include <minizinc/feature_extraction.hh>
#include <minizinc/utils.hh>
#include <algorithm>
#include <iostream>
#include <numeric>
#include "eval_par.cpp"

namespace MiniZinc {

/*
 * A Domain in this context consists of at least one pair of int vals.
 * The pair encodes a range (min, max).
 * If the Domain consists of multiple ranges, they are disjoint.
 */

class Domain {
public:
  class Range {
  public:
    long long min;
    long long max;

    // Constructor
    Range(long long minValue, long long maxValue) : min(minValue), max(maxValue) {}

    // Overload less-than operator for sorting
    bool operator<(const Range& other) const { return min < other.min; }
  };

private:
  std::vector<Range> _ranges;
  long long _width = -1;

public:
  const std::vector<Range>& getRanges() const { return _ranges; }

  // Constructor
  Domain(const std::vector<Range>& inputRanges) : _ranges(inputRanges) {
    std::sort(_ranges.begin(), _ranges.end());
    size();
  }

  // Merge two domains without altering the originals
  static Domain merge(const Domain& d1, const Domain& d2) {
    std::vector<Range> allRanges = d1._ranges;
    allRanges.insert(allRanges.end(), d2._ranges.begin(), d2._ranges.end());
    std::sort(allRanges.begin(), allRanges.end());

    std::vector<Range> mergedRanges;
    if (!allRanges.empty()) {
      mergedRanges.push_back(allRanges[0]);
    }

    for (const auto& range : allRanges) {
      Range& lastRange = mergedRanges.back();
      if (range.min <= lastRange.max + 1) {
        // Merge overlapping or adjacent ranges
        lastRange.max = std::max(lastRange.max, range.max);
      } else {
        // Add new non-overlapping range
        mergedRanges.push_back(range);
      }
    }

    return Domain(mergedRanges);
  }

  // Convert a domain encoded as IntSetVal into a Domain class object
  static inline Domain from(IntSetVal& set) {
    std::vector<Domain::Range> ranges;
    for (int i = 0; i < set.size(); i++) {
      ranges.push_back({set.min(i).toInt(), set.max(i).toInt()});
    }
    return Domain(ranges);
  }

  void printRanges() const {
    for (const auto& range : _ranges) {
      std::cout << "Range: [" << range.min << ", " << range.max << "]\n";
    }
  }

  // width = Sum (r.max - r.min + 1) for all ranges r in the domain
  long long inline width() {
    if (_width == -1) {
      long long size = 0;
      for (const auto& range : _ranges) {
        size += (range.max - range.min + 1);
      }
      _width = size;
    }

    return _width;
  }

  // Number of ranges in the domain
  int inline size() const { return _ranges.size(); }
};

class BipartiteGraph {
private:
  int uSize;  // Number of vertices in set U
  int vSize;  // Number of vertices in set V
  bool uAutoResize;
  bool vAutoResize;
  std::vector<std::vector<int>> adjacencyMatrix;  // Adjacency matrix
public:
  // Constructor to initialize the graph with sizes of U and V
  BipartiteGraph(int uSize, int vSize)
      : uSize(uSize),
        vSize(vSize),
        uAutoResize(uSize == 0),
        vAutoResize(vSize == 0),
        adjacencyMatrix(uSize, std::vector<int>(vSize, 0)) {}

  // Method to add an edge between vertex u in U and vertex v in V
  // We can not use iterators to determine the num of vardecls and constraints 
  void addEdge(int u, int v) {
    if (u < 0 || v < 0) {
      std::cerr << "Invalid vertex index." << std::endl;
      return;
    }

    // Resize the matrix if necessary & allowed
    if (uAutoResize && u >= uSize) {
      adjacencyMatrix.resize(u + 1, std::vector<int>(vSize, 0));
      uSize = u + 1;
    }
    if (vAutoResize && v >= vSize) {
      for (auto& row : adjacencyMatrix) {
        row.resize(v + 1, 0);
      }
      vSize = v + 1;
    }

    // we will only enter this block if:
    // autoResize is on || the index is within predefined bounds
    // all other values will be dropped
    if (u < uSize && v < vSize) {
      adjacencyMatrix[u][v] = 1;
    }
  }

  std::string formatMatrix() const {
    std::ostringstream oss;
    for (int i = 0; i < adjacencyMatrix.size(); i++) {
      const auto& row = adjacencyMatrix[i];
      for (const auto& cell : row) {
        oss << cell;
      }
      if (i != adjacencyMatrix.size() - 1) {
        oss << "|";
      }
    }
    return oss.str();
  }

  std::vector<std::vector<int>> getAdjecencyMatrix() const { 
    return adjacencyMatrix;
  }

  int currentVSize() const { return vSize;}
};


static std::vector<long long> calculate_domain_widths(std::vector<Domain>& domains) {
  std::vector<long long> domain_sizes;

  for (auto& d : domains) {
    domain_sizes.push_back(d.width());
  }

  return domain_sizes;
}

static std::vector<double> domain_overlap_avgs(std::vector<Domain>& domains) {
  std::vector<double> domain_overlaps;

  // pair the domains up for comparison
  for (int i = 0; i < domains.size(); i++) {
    auto& d1 = domains[i];
    for (int j = i + 1; j < domains.size(); j++) {
      auto& d2 = domains[j];
      auto merged_domains = Domain::merge(d1, d2);
      // the sum(range_overlaps) <==> domain_overlap
      std::vector<double> range_overlaps;
      // go over the individual ranges of the first domain
      for (int ii = 0; ii < d1.size(); ii++) {
        auto& p1 = d1.getRanges()[ii];
        // go over the individual ranges of the second domain
        for (int jj = 0; jj < d2.size(); jj++) {
          auto& p2 = d2.getRanges()[jj];
          auto overlap_start = std::max(p1.min, p2.min);
          auto overlap_end = std::min(p1.max, p2.max);
          if (overlap_start <= overlap_end) {
            auto range_overlap = overlap_end - overlap_start + 1;
            range_overlaps.push_back(range_overlap / (double)merged_domains.width());
          }
        }
      }
      // adds a 0 in case the domain did not overlap
      double total_domain_overlap = std::accumulate(range_overlaps.begin(), range_overlaps.end(), 0.0);
      domain_overlaps.push_back(total_domain_overlap);
    }
  }

  return domain_overlaps;
}

static bool is_var_defined_by_call(Call* call, EnvI& envi, Id* var) { 

  auto& ann = Expression::ann(call);
  std::vector<Expression*> removeAnns;
  for (ExpressionSetIter anns = ann.begin(); anns != ann.end(); ++anns) {
    if (Call* c = Expression::dynamicCast<Call>(*anns)) {
      if (c->id() == envi.constants.ann.defines_var) {
        for (unsigned int i = 0; i < c->argCount(); i++) {
          auto a = c->arg(i);
          if (Expression::isa<Id>(a)) {
            auto id = Expression::cast<Id>(a);
            if (Expression::equal(id, var)) {
              return true;
            }
          }
        }
      }
    }
  }
  return false;
}

static bool is_call_using_var_defined_by_other(Call* call, EnvI& envi, Id* var) {
  bool isDefined = Expression::ann(var->decl()).contains(envi.constants.ann.is_defined_var);
  return (isDefined && !is_var_defined_by_call(call, envi, var));
}

static void add_to_constraint_histogram(FlatModelFeatureVector& features, const char* constraintName) {
  features.ct_histogram[constraintName]++;
}

static void add_to_annotation_histogram(FlatModelFeatureVector& features, Expression* annotations) {
  for (auto ann : Expression::ann(annotations)) {
    if (Expression::isa<Id>(ann)) {
      const Id* ident = Expression::cast<Id>(ann);
      if (ident->decl() != nullptr) {
        ident = ident->decl()->id();
      }
      if (ident->idn() == -1) {
        features.ann_histogram[ident->v().c_str()]++;
      }
    }
  }
}

static double average_decision_variables_in_constraints(BipartiteGraph& constraintGraph) {
  double result = 0;
  if (constraintGraph.currentVSize() > 0) {
    for (auto& row : constraintGraph.getAdjecencyMatrix()) {
      result += mean(row);
    }
  }
  return result;
}

FlatModelFeatureVector extract_feature_vector(Env& m, FlatModelFeatureVector::Options& o) {
  Model* flat = m.flat();
  FlatModelFeatureVector features;
  std::vector<Domain> domains;
  std::map<std::string, int> varIdToCustomIdMap;
  int varIdCounter = 0;
  int constraintIdCounter = 0;
  BipartiteGraph constraintGraph = BipartiteGraph(0, 0);

  if (o.vDimensions > 0 || o.cDimensions > 0) {
    constraintGraph = BipartiteGraph(o.vDimensions, o.cDimensions);
  }

  for (auto& i : *flat) {
    if (!i->removed()) {
      if (auto* vdi = i->dynamicCast<VarDeclI>()) {
        Type t = vdi->e()->type();
        // iterate over every var decl that is not an array
        if (t.isvar() && t.dim() == 0) {
          if (t.isSet()) {
            // todo handle other sets or constraint to intSet
            features.n_set_vars++;
            Expression* domain = vdi->e()->ti()->domain();
            IntSetVal* bounds = eval_intset(m.envi(), domain);
            Domain d = Domain::from(*bounds);
            domains.push_back(d);
          } else if (t.isint()) {
            features.n_int_vars++;
            Expression* domain = vdi->e()->ti()->domain();
            if (domain != nullptr) {
              IntSetVal* bounds = eval_intset(m.envi(), domain);
              Domain d = Domain::from(*bounds);
              domains.push_back(d);
            } else {
              //std::cout << "Nullptr Domain in FeatureExtraction - is this a bug?" << std::endl; //TODO discuss with maintainers
            }
          } else if (t.isbool()) {
            features.n_bool_vars++;
            Domain d = Domain({{0, 1}});
            domains.push_back(d);
          } else if (t.isfloat()) {
            // currently omitted in model training
          }
          GCLock lock;
          varIdToCustomIdMap[vdi->e()->id()->str().c_str()] = varIdCounter;
          features.customIdToVarNameMap[varIdCounter++] = vdi->e()->id()->str().c_str();
          add_to_annotation_histogram(features, vdi->e());
        }
      } else if (auto* ci = i->dynamicCast<ConstraintI>()) {
        if (Call* call = Expression::dynamicCast<Call>(ci->e())) {
          if (call->argCount() > 0) {
            Type all_t;
            auto constraintId = constraintIdCounter++;
            const char* constraintName = call->id().c_str();
            // skip everything float related in this section todo make floats optional for feature vector
            if (std::strstr(constraintName, "float") != nullptr) {
              continue;
            }
            features.customIdToConstraintNameMap[constraintId] = constraintName;
            int foreignDefinedVarsUsedByCall = 0;
            add_to_constraint_histogram(features, constraintName);
            for (unsigned int i = 0; i < call->argCount(); i++) {
              Type t = Expression::type(call->arg(i));
              if (t.isvar()) {
                if (t.st() == Type::ST_SET ||
                    (t.bt() == Type::BT_FLOAT && all_t.st() != Type::ST_SET) ||
                    (t.bt() == Type::BT_INT && all_t.bt() != Type::BT_FLOAT &&
                     all_t.st() != Type::ST_SET) ||
                    (t.bt() == Type::BT_BOOL && all_t.bt() != Type::BT_INT &&
                     all_t.bt() != Type::BT_FLOAT && all_t.st() != Type::ST_SET)) {
                  all_t = t;

                  const auto a = call->arg(i);
                  // add variable argument to constraint graph
                  if (Expression::isa<Id>(a)) {
                    GCLock lock;
                    Id* id = Expression::cast<Id>(a);
                    constraintGraph.addEdge(varIdToCustomIdMap[id->str().c_str()], constraintId);

                    if (is_call_using_var_defined_by_other(call, m.envi(), id)) {
                      foreignDefinedVarsUsedByCall++;
                    }
                  }
                  // add array arguments elements to constraint graphs if they are variables
                  else if (Expression::isa<ArrayLit>(a)) {
                    const auto* di = Expression::cast<ArrayLit>(a);
                    for (auto v : di->getVec()) {
                      if (Expression::isa<Id>(v)) {
                        GCLock lock;
                        Id* id = Expression::cast<Id>(v);
                        if (id->decl() != nullptr) {
                          id = id->decl()->id();
                        }
                        constraintGraph.addEdge(varIdToCustomIdMap[id->str().c_str()],
                                                constraintId);

                        if (is_call_using_var_defined_by_other(call, m.envi(), id)) {
                          foreignDefinedVarsUsedByCall++;
                        }
                      }
                    }
                  }
                }
              }
            }
            if (all_t.isvar()) {
              if (all_t.st() == Type::ST_SET) {
                features.n_set_ct++;
              } else if (all_t.bt() == Type::BT_INT) {
                features.n_int_ct++;
              } else if (all_t.bt() == Type::BT_BOOL) {
                features.n_bool_ct++;
              } else if (all_t.bt() == Type::BT_FLOAT) {
                // currently omitted in model training
              }
            }
            if (foreignDefinedVarsUsedByCall > 1) {
              features.n_meta_ct++;
            }
          }
        }
      }
    }
  }

  features.constraint_graph = constraintGraph.formatMatrix();
  auto domain_sizes = calculate_domain_widths(domains);
  features.domain_widths = domain_sizes;  //copying intentional

  features.avg_decision_vars_in_cts = average_decision_variables_in_constraints(constraintGraph);

  if (!domain_sizes.empty()) {
    features.std_dev_domain_size = std::round(stdDev(domain_sizes) * 1000) / 1000.0;

    double d = mean(domain_sizes);
    features.avg_domain_size = mean(domain_sizes);

    std::sort(domain_sizes.begin(), domain_sizes.end());
    features.median_domain_size = domain_sizes[domain_sizes.size() / 2];

    auto overlaps = domain_overlap_avgs(domains);
    features.n_disjoint_domain_pairs = std::count(overlaps.begin(), overlaps.end(), 0.0);

    features.avg_domain_overlap = mean(overlaps);
  }
  features.n_total_ct += features.n_set_ct + features.n_int_ct + features.n_bool_ct;
  return features;
}

}  // namespace MiniZinc