#include <ilcplex/cplex.h>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

#include "CostGen.hpp"

using std::pair;
using std::string;
using std::vector;

CostGen::CostGen() : X(0), Y(0) {}

unsigned int uabs(const unsigned int a, const unsigned int b) {
  return (a > b) ? a - b : b - a;
}

void CostGen::adjustSize(const unsigned int N) {
  // PRE = X * Y < N
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine re(seed);
  // int min = static_cast<int>(std::ceil(sqrt(N)));
  std::uniform_int_distribution<unsigned int> randomIncrease(N, 3 * N);
  X += randomIncrease(re);
  Y += randomIncrease(re);
}

double* CostGen::generateCosts(const unsigned int N) {
  // Increase grid size if necessary
  if (X * Y < N) adjustSize(N);
  // POST: X * Y > N

  // TODO: check
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine re(seed);
  std::uniform_int_distribution<unsigned int> genNodeX(0, X);
  std::uniform_int_distribution<unsigned int> genNodeY(0, Y);
  while (vertices.size() < N) {
    int x = genNodeX(re);
    int y = genNodeY(re);
    vertices.insert({x, y});
  }

  double* C = new double[N * N];
  double* CCopy = C;
  std::fill_n(C, N * N, CPX_INFBOUND);

  // auto start = std::chrono::system_clock::now();
  vector<pair<unsigned int, unsigned int>> tempvec(vertices.cbegin(),
                                                   vertices.cend());
  for (auto i = tempvec.cbegin(); i != tempvec.cend(); ++i) {
    // const pair<int, int> v = tempvec[i];
    const unsigned int xi = i->first;
    const unsigned int yi = i->second;
    for (auto j = tempvec.cbegin(); j != tempvec.cend(); ++j) {
      // const pair<int, int> u = tempvec[j];
      if (i != j) {
        const unsigned int xj = j->first;
        const unsigned int yj = j->second;
        *CCopy = sqrt(pow(uabs(xi, xj), 2.0) + pow(uabs(yi, yj), 2.0));
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

void CostGen::saveToCsv(double* C, unsigned int N, const string& path) {
  std::ofstream file;
  file.open(path + std::to_string(N) + string(".csv"), std::fstream::out);
  file << std::to_string(N) << "\n";
  for (unsigned int i = 0; i < N; ++i) {
    for (unsigned int j = 0; j < N; ++j) {
      if (i == j)
        file << "inf"
             << ",";
      else
        file << std::to_string(C[i * N + j]) << ",";
    }
    file << "\n";
  }
  file.close();
}

pair<unsigned int, double*> CostGen::loadFromCsv(const string& filepath) {
  std::ifstream file;
  file.open(filepath, std::fstream::in);

  string s_N;
  getline(file, s_N, '\n');
  unsigned int N = std::stoi(s_N);

  double* C = new double[N * N];

  for (unsigned int i = 0; i < N; ++i) {
    for (unsigned int j = 0; j < N; ++j) {
      if (file.peek() == EOF)
        std::cout << "Problem reading file: not enough element (" << i * j
                  << "/" << N << ")";
      string val;
      if (j == N - 1)
        getline(file, val, '\n');
      else
        getline(file, val, ',');

      C[i * N + j] = (val == "inf") ? CPX_INFBOUND : std::stod(val);
    }
  }

  file.close();

  return {N, C};
}

// double* C = new double[N * N];
// std::uniform_real_distribution<double> unif(0, 1001);
// for (int i = 0; i < (N * N); ++i) C[i] = unif(re);