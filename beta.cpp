#include <iomanip>
#include <iostream>
#include <vector>

long long int bc_rec(long long int n, long long int k, long long int k0,
                     std::vector<long long int>& t) {
  long long int idx = n * k0 + k;
  if (t[idx] >= 0) return t[idx];
  if (k == 0) {
    t[idx] = 1;
    return 1;
  }
  t[idx] = bc_rec(n - 1, k - 1, k0, t) + bc_rec(n - 1, k, k0, t);
  return t[idx];
}

long long int bc(long long int n, long long int k) {
  std::vector<long long int> t((n + 1) * (k + 1), -1);
  return bc_rec(n, k, k + 1, t);
}

int main() {
  // std::cout << "Pascal's triangle:\n";
  // for(int n = 1; n < 10; ++n) {
  //     std::cout << std::string(20-n*2, ' ');
  //     for(int k = 1; k < n; ++k)
  //         std::cout << std::setw(3) << bc(n,k) << ' ';
  //     std::cout << '\n';
  // }
  std::cerr << bc(5, 3) << std::endl;
}