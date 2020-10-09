#pragma once

#include <algorithm>
#include <vector>

namespace Algorithms {

/// An interface to some min-cut algorithm for undirected graphs
class MinCut {
public:
  /// INPUT
  int nNodes = 0;
  std::vector<std::pair<int, int> > edges;
  std::vector<double> weights;
  /// OUTPUT
  std::vector<bool> parities;
  double wMinCut = 1e100;
  /// Invocation
  static void solve();
};

}  // namespace Algorithms