#include <minizinc/eval_par.hh>
#include <minizinc/feature_extraction.hh>
#include <minizinc/utils.hh>
#include <numeric>
#include <iostream>
#include <algorithm>

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
  int inline size() const { return _ranges.size();}

};

static std::vector<double> domain_overlap_avgs(std::vector<std::pair<double, double>>& domains) {
  std::vector<double> result;
  for (int i = 0; i < domains.size(); i++) {
    auto p1 = domains[i];
    for (int j = i + 1; j < domains.size(); j++) {
      auto p2 = domains[j];
      auto overlap_start = std::max(p1.first, p2.first);
      auto overlap_end = std::min(p1.second, p2.second);
      if (overlap_start <= overlap_end) {
        auto overlap = abs(overlap_start - overlap_end) + 1;
        auto combinedDomains = std::max(p1.second, p2.second) - std::min(p1.first, p2.first) + 1;
        result.push_back(overlap / combinedDomains);
      } else {
        result.push_back(0);
      }
    }
  }
  return result;
}

static std::vector<long long> calculate_domain_width(std::vector<Domain>& domains) { 
  
  std::vector<long long> domain_sizes;

  for (auto& d : domains) {
    domain_sizes.push_back(d.width());
  }

  return domain_sizes;
}

static std::vector<double> domain_overlap_avgs2(std::vector<Domain>& domains) {

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
            range_overlaps.push_back(range_overlap / (double) merged_domains.width());
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


FlatModelFeatureVector extract_feature_vector(Env& m) {
  Model* flat = m.flat();
  FlatModelFeatureVector features;
  std::vector<Domain> domains;

  for (auto& i : *flat) {
    if (!i->removed()) {
      if (auto* vdi = i->dynamicCast<VarDeclI>()) {
        Type t = vdi->e()->type();
        // iterate over every var decl that is not an array
        if (t.isvar() && t.dim() == 0) {
          if (t.isSet()) {
            features.n_set_vars++;
            Expression* domain = vdi->e()->ti()->domain();
            IntSetVal* bounds = eval_intset(m.envi(), domain);
            Domain d = Domain::from(*bounds);
            domains.push_back(d);
          } else if (t.isint()) {
            features.n_int_vars++;
            Expression* domain = vdi->e()->ti()->domain();
            IntSetVal* bounds = eval_intset(m.envi(), domain);
            Domain d = Domain::from(*bounds);
            domains.push_back(d);
          } else if (t.isbool()) {
            features.n_bool_vars++;
            Domain d = Domain({{0, 1}});
            domains.push_back(d);
          } else if (t.isfloat()) {
            // currently ommited in model training
          }
        }
      } else if (auto* ci = i->dynamicCast<ConstraintI>()) {
        if (Call* call = Expression::dynamicCast<Call>(ci->e())) {
          if (call->id().endsWith("_reif")) {
            // currently ommited in model training
          } else if (call->id().endsWith("_imp")) {
            // currently ommited in model training
          }
          if (call->argCount() > 0) {
            Type all_t;
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
                // currently ommited in model training
              }
            }
          }
        }
      }
    }
  }

  auto domain_sizes = calculate_domain_width(domains);

  if (!domain_sizes.empty()) {
    features.std_dev_domain_size = new double(std::round(stdDev(domain_sizes) * 1000) / 1000.0);

    double d = mean(domain_sizes);
    features.avg_domain_size = new double(mean(domain_sizes));

    std::sort(domain_sizes.begin(), domain_sizes.end());
    features.median_domain_size = new double(domain_sizes[domain_sizes.size() / 2]);

    auto overlaps = domain_overlap_avgs2(domains);
    features.n_disjoint_domain_pairs = new int(std::count(overlaps.begin(), overlaps.end(), 0.0));

    features.avg_domain_overlap = new double(mean(overlaps));
  }
  features.n_total_ct += features.n_set_ct + features.n_int_ct + features.n_bool_ct;
  return features;
}

}  // namespace MiniZinc