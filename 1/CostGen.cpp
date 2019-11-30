#include <ilcplex/cplex.h>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>

#include "CostGen.hpp"

using std::pair;
using std::unordered_set;

CostGen::CostGen() : X(0), Y(0) {}

void CostGen::adjustSize(const int N) {
  // PRE = X * Y < N
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine re(seed);
  int min = static_cast<int>(std::ceil(sqrt(N)));
  std::uniform_int_distribution<int> randomIncrease(min, 3 * N);
  X += randomIncrease(re);
  Y += randomIncrease(re);
}

double* CostGen::generateCosts(const int N) {
  // Increase grid size if necessary
  if (X * Y < N) adjustSize(N);
  // POST: X * Y > N

  // TODO: probably can use something more efficient
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine re(seed);
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

  // TODO: number of operations
  for (auto i = vertices.cbegin(); i != vertices.cend(); ++i) {
    const pair<int, int> v = *i;
    const int xi = v.first;
    const int yi = v.second;
    // int col = 0;
    for (auto j = vertices.cbegin(); j != vertices.cend(); ++j) {
      if (i != j) {
        const int xj = j->first;
        const int yj = j->second;
        *CCopy = sqrt(pow(abs(xi - xj), 2.0) + pow(abs(yi - yj), 2.0));
      }
      CCopy++;
    }
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