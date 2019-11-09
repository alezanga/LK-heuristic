#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include "TSPmodel.hpp"
#include "VariadicTable.hpp"

using std::cout;
using std::string;
using std::vector;

double* generateCosts(const int N) {
  double* C = new double[N * N];
  std::uniform_real_distribution<double> unif(0, 1001);
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine re(seed);
  for (int i = 0; i < (N * N); ++i) C[i] = unif(re);
  return C;
}

void testTimes() {
  const int step = 10;
  int N = 4;
  // double elapsed_seconds = 0;
  VariadicTable<int, double> results({"Problem size (N)", "Time to solve (s)"});
  int executions = 0;
  while (executions < 1) {
    // Create TSP problem with N holes
    TSPmodel mod = TSPmodel(N, generateCosts(N));
    auto start = std::chrono::system_clock::now();
    mod.solve();
    auto end = std::chrono::system_clock::now();
    double elapsed_seconds = (end - start).count();
    results.addRow({N, elapsed_seconds});
    N += step;
    executions++;
  }
  cout << "Tabella dei risultati:\n";
  results.print(cout);
}

int main() {
  testTimes();
  return 0;
}
