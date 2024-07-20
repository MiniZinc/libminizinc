#include <minizinc/feature_extraction.hh>
#include <minizinc/utils.hh>
#include <minizinc/eval_par.hh>

namespace MiniZinc {
std::vector<double> domain_overlap_avgs(std::vector<std::pair<double, double>>& domains) {
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

FlatModelFeatureVector extract_feature_vector(Env& m) {
  Model* flat = m.flat();
  FlatModelFeatureVector features;
  std::vector<double> domain_sizes;
  std::vector<std::pair<double, double>> domains;

  for (auto& i : *flat) {
    if (!i->removed()) {
      if (auto* vdi = i->dynamicCast<VarDeclI>()) {
        Type t = vdi->e()->type();
        if (t.isvar() && t.dim() == 0) {
          if (t.isSet()) {
           
            features.n_set_vars++;
            Expression* domain = vdi->e()->ti()->domain();
            IntSetVal* val = eval_intset(m.envi(), domain);
            auto boundPair = std::make_pair(val->min().toInt(), val->max().toInt());

            auto length = abs(boundPair.first - boundPair.second) + 1;
            domain_sizes.push_back(length);
            domains.push_back(boundPair);
          } else if (t.isint()) {
            features.n_int_vars++;
            Expression* domain = vdi->e()->ti()->domain();
            IntSetVal* bounds = eval_intset(m.envi(), domain);
            auto boundPair = std::make_pair(bounds->min().toInt(), bounds->max().toInt());
            auto length = abs(boundPair.first - boundPair.second) + 1;
            domain_sizes.push_back(length);
            domains.push_back(boundPair);
          } else if (t.isbool()) {
            features.n_bool_vars++;
          } else if (t.isfloat()) {
            //features.n_float_vars++;
            Expression* domain = vdi->e()->ti()->domain();
            FloatSetVal* bounds = eval_floatset(m.envi(), domain);
            auto boundPair = std::make_pair(bounds->min().toDouble(), bounds->max().toDouble());
            auto length = abs(boundPair.first - boundPair.second) + 1;
            domain_sizes.push_back(length);
            domains.push_back(boundPair);
          }
        }
      } else if (auto* ci = i->dynamicCast<ConstraintI>()) {
        if (Call* call = Expression::dynamicCast<Call>(ci->e())) {
          if (call->id().endsWith("_reif")) {
            //features.n_reif_ct++;
          } else if (call->id().endsWith("_imp")) {
            //features.n_imp_ct++;
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
                //features.n_float_ct++;
              }
            }
          }
        }
      }
    }
  }
  if (!domain_sizes.empty()) {
    features.std_dev_domain_size = new double(std::round(stdDev(domain_sizes) * 1000) / 1000.0);

    features.avg_domain_size = new double(mean(domain_sizes));

    std::sort(domain_sizes.begin(), domain_sizes.end());
    features.median_domain_size = new double(domain_sizes[domain_sizes.size() / 2]);

    auto overlaps = domain_overlap_avgs(domains);
    features.n_disjoint_domain_pairs = new int(std::count(overlaps.begin(), overlaps.end(), 0.0));

    features.avg_domain_overlap = new double(mean(overlaps));
  }
  features.n_total_ct += features.n_set_ct + features.n_int_ct + features.n_bool_ct;
 // +stats.n_float_ct;
  return features;
}

}