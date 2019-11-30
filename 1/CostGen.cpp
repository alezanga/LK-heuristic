#include <ilcplex/cplex.h>
#include <chrono>
#include <cmath>
#include <random>

#include "CostGen.hpp"

using std::pair;
using std::vector;

CostGen::CostGen() : X(0), Y(0) {}

void CostGen::adjustSize(const int N) {
  // PRE = X * Y < N
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine re(seed);
  // int min = static_cast<int>(std::ceil(sqrt(N)));
  std::uniform_int_distribution<int> randomIncrease(N, 3 * N);
  X += randomIncrease(re);
  Y += randomIncrease(re);
}

double* CostGen::generateCosts(const int N) {
  // Increase grid size if necessary
  if (X * Y < N) adjustSize(N);
  // POST: X * Y > N

  // TODO: check
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

  // auto start = std::chrono::system_clock::now();
  vector<pair<int, int>> tempvec(vertices.cbegin(), vertices.cend());
  for (auto i = tempvec.cbegin(); i != tempvec.cend(); ++i) {
    // const pair<int, int> v = tempvec[i];
    const int xi = i->first;
    const int yi = i->second;
    for (auto j = tempvec.cbegin(); j != tempvec.cend(); ++j) {
      // const pair<int, int> u = tempvec[j];
      if (i != j) {
        const int xj = j->first;
        const int yj = j->second;
        *CCopy = sqrt(pow(abs(xi - xj), 2.0) + pow(abs(yi - yj), 2.0));
      }
      CCopy++;
    }
  }
  // auto end = std::chrono::system_clock::now();
  // double elapsed_seconds = std::chrono::duration<double>(end -
  // start).count(); std::cout << "Time to do:  " << elapsed_seconds; for (int i
  // = 0; i < N; ++i) {
  //   for (int j = 0; j < N; ++j) {
  //     std::cout << std::setprecision(1) << C[i * N + j] << "  ";
  //   }
  //   std::cout << std::endl;
  // }
  return C;
}

// double* C = new double[N * N];
// std::uniform_real_distribution<double> unif(0, 1001);
// for (int i = 0; i < (N * N); ++i) C[i] = unif(re);