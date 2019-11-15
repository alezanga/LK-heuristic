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

/*struct equal_to_pair {
  template <class T, class U>
  bool operator()(const pair<T, U>& p1, const pair<T, U>& p2) const {
    pair<T, U>{a1, b1} = p1;
    pair<T, U>{a2, b2} = p2;
    return p1 == p2 || (a1 == b2 && b1 == a2);
  }
};*/

double* generateCosts(const int N) {
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine re(seed);
  /*std::cout << "d1";
  std::uniform_int_distribution<int> dist(N / 2, 5 * N);
  const int X = dist(re);
  const int Y = dist(re);
  std::cout << "d2";

  unordered_set<pair<int, int>, hash_pair> vertices;

  std::uniform_int_distribution<int> genNodeX(0, X);
  std::uniform_int_distribution<int> genNodeY(0, Y);
  unsigned int nsize = static_cast<unsigned int>(N);
  while (vertices.size() < nsize) {
    std::cout << "d3";

    int x = genNodeX(re);
    int y = genNodeY(re);
    vertices.insert({x, y});
  }

  double* C = new double[N * N];

  // int row = 0;
  std::cout << "d4";

  for (auto i = vertices.cbegin(); i != vertices.cend(); ++i) {
    const pair<int, int> v = *i;
    const int x = v.first;
    const int y = v.second;
    // int col = 0;
    for (auto j = vertices.cbegin(); j != vertices.cend(); ++j) {
      std::cout << "Hello";
      if (i != j) {
        const int w = j->first;
        const int z = j->second;
        *C = sqrt(pow(abs(x - w), 2.0) + pow(abs(y - z), 2.0));
      }
      C++;
    }
  }*/

  double* C = new double[N * N];
  std::uniform_real_distribution<double> unif(0, 1001);
  for (int i = 0; i < (N * N); ++i) C[i] = unif(re);
  return C;
}