#include "IteratedLK.hpp"
#include "LK.hpp"
#include "Params.cpp"
#include "TSPsolution.hpp"
#include "Tour.hpp"
#include "datatypes.hpp"

#include <algorithm>
#include <chrono>
#include <random>

using std::ofstream;
using std::pair;
using std::vector;

vector<vertex>* naiveTour(const unsigned int N) {
  // vector<Node> t(N);
  // t[0] = Node(N - 1, 1);
  // double cost = C[(N - 1) * N] + C[1];
  // for (vertex i = 1; i < N - 1; ++i, cost += C[i * N + i + 1])
  //   t[i] = Node(i - 1, i + 1);
  // t[N - 1] = Node(N - 2, 0);
  // return new Tour(N, t, cost);
  vector<vertex>* v = new vector<vertex>(N);
  for (vertex i = 0; i < N; ++i) (*v)[i] = i;
  return v;
}

Tour* permuteTour(vector<vertex> atour, const double* C) {
  const unsigned int N = atour.size();
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine re(seed);
  std::shuffle(atour.begin(), atour.end(), re);

  vector<Node> t(N);
  t[atour[0]] = Node(atour[N - 1], atour[1]);
  double cost = C[atour[N - 1] * N + atour[0]] + C[atour[0] * N + atour[1]];
  for (vertex i = 1; i < N - 1; cost += C[atour[i] * N + atour[i + 1]], ++i)
    t[atour[i]] = Node(atour[i - 1], atour[i + 1]);
  t[atour[N - 1]] = Node(atour[N - 2], atour[0]);

  return new Tour(N, t, cost);
}

pair<TSPsolution, double> iterated_LK(const Params& P, const unsigned int N,
                                      const double* C, ofstream& log) {
  vector<vertex>* tour0 = naiveTour(N);
  double tot_seconds = 0.0, part_seconds = 0.0;
  log << "*** ITERATION OF LIN-KERNIGHAN HEURISTIC ***\n";
  TSPsolution best(-1, 0, nullptr);
  for (unsigned int i = 0; i < P.LK_iterations;
       ++i, tot_seconds += part_seconds) {
    log << "Iteration " << i + 1 << "/" << P.LK_iterations << "\n";
    Tour* tour = permuteTour(*tour0, C);
    LK heur(N, C, *tour);
    auto start = std::chrono::system_clock::now();
    heur.solve();
    auto end = std::chrono::system_clock::now();
    part_seconds = std::chrono::duration<double>(end - start).count();
    TSPsolution sol = heur.getSolution();
    if (best.objVal == -1 || sol.objVal < best.objVal) best = sol;
    log << "Solved in: " << part_seconds << " sec\n" << sol;
    delete tour;
  }
  delete tour0;
  return {best, tot_seconds};
}
