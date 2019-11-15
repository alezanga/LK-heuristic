#include <ilcplex/cplex.h>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <unordered_set>

using std::pair;
using std::unordered_set;

struct hash_pair {
  template <class T, class U>
  std::size_t operator()(const pair<T, U>& p) const {
    return std::hash<T>()(p.first) ^ std::hash<U>()(p.second);
  }
};

double* generateCosts(const int N) {
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine re(seed);
  std::uniform_int_distribution<int> dist(N / 2, 5 * N);  // TODO: check this.
  const int X = dist(re);
  const int Y = dist(re);

  // TODO: probably can use something more efficient
  unordered_set<pair<int, int>, hash_pair> vertices;

  std::uniform_int_distribution<int> genNodeX(0, X);
  std::uniform_int_distribution<int> genNodeY(0, Y);
  unsigned int nsize = static_cast<unsigned int>(N);
  while (vertices.size() < nsize) {
    int x = genNodeX(re);
    int y = genNodeY(re);
    vertices.insert({x, y});
  }

  double* C = new double[N * N];
  double* CCopy = C;
  std::fill_n(C, N * N, CPX_INFBOUND);

  // int row = 0;
  // TODO: number of operations
  for (auto i = vertices.cbegin(); i != vertices.cend(); ++i) {
    const pair<int, int> v = *i;
    const int x = v.first;
    const int y = v.second;
    // int col = 0;
    for (auto j = vertices.cbegin(); j != vertices.cend(); ++j) {
      if (i != j) {
        const int w = j->first;
        const int z = j->second;
        *CCopy = sqrt(pow(abs(x - w), 2.0) + pow(abs(y - z), 2.0));
      }
      CCopy++;
      // col++;
    }
    // row++;
  }
  return C;
}

// for (int i = 0; i < N; ++i) {
//   for (int j = 0; j < N; ++j) {
//     std::cout << std::fixed << C[i * N + j] << "  ";
//   }
//   std::cout << std::endl;
// }

// double* C = new double[N * N];
// std::uniform_real_distribution<double> unif(0, 1001);
// for (int i = 0; i < (N * N); ++i) C[i] = unif(re);