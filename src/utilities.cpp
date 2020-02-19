#include "utilities.hpp"
#include "CPLEX.hpp"
#include "LK.hpp"
#include "Pair.hpp"
#include "TSPsolution.hpp"
#include "Tour.hpp"
#include "utils/params.hpp"

#include <algorithm>
#include <chrono>
#include <random>
#include <set>

using std::ostream;
using std::pair;
using std::set;
using std::vector;

namespace fs = std::experimental::filesystem;

/**
 * @param N number of tour vertices
 * @return pointer to list of vertices
 */
vector<vertex>* utils::naiveTour(const unsigned int N) {
  vector<vertex>* v = new vector<vertex>(N);
  for (vertex i = 0; i < N; ++i) (*v)[i] = i;
  return v;
}

/**
 * Cheapest neighbour greedy heuristic
 *
 * @param C cost matrix
 * @param N number of tour vertices
 * @return cost of final tour
 */
double utils::nearestNeighbour(const double* C, const unsigned int N) {
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine re(seed);
  std::uniform_int_distribution<unsigned int> rndInt(0, N - 1);
  vector<vertex>* tour = new vector<vertex>;
  vector<bool> visited(N, false);
  double cost = 0.0;
  tour->push_back(rndInt(re));
  for (vertex v = tour->back(); !visited[v]; v = tour->back()) {
    double bestCost = std::numeric_limits<double>::max();
    vertex cheapestNeighbour = N;
    for (vertex z = 0; z < N; ++z)
      if (z != v && !visited[z] && C[v * N + z] < bestCost) {
        bestCost = C[v * N + z];
        cheapestNeighbour = z;
      }
    if (cheapestNeighbour != N) tour->push_back(cheapestNeighbour);
    cost += C[v * N + cheapestNeighbour];
    visited[v] = true;
  }
  delete tour;
  return cost;
}

/**
 * Create a tour object from a list of vertices
 *
 * @param atour vertices in the tour (order matters)
 * @param C cost matrix
 * @param permute if true a random permutation of the provided tour is returned
 * @return Tour object
 */
Tour utils::createTour(vector<vertex> atour, const double* C,
                       const bool permute) {
  const unsigned int N = atour.size();

  if (permute) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine re(seed);
    std::shuffle(atour.begin(), atour.end(), re);
  }

  vector<Pair> t(N);
  t[atour[0]] = Pair(atour[N - 1], atour[1]);
  double cost = C[atour[N - 1] * N + atour[0]] + C[atour[0] * N + atour[1]];
  for (vertex i = 1; i < N - 1; cost += C[atour[i] * N + atour[i + 1]], ++i)
    t[atour[i]] = Pair(atour[i - 1], atour[i + 1]);
  t[atour[N - 1]] = Pair(atour[N - 2], atour[0]);

  return Tour(N, t, cost);
}

/**
 * Runs LK heuristic with restarts
 * @param P hyper-parameters obejct
 * @param N number of vertices
 * @param C cost matrix
 * @param log stream where to log iterations
 * @return pair with solution object and total execution time
 */
pair<TSPsolution, double> utils::runILK(const Params& P, const unsigned int N,
                                        const double* C, ostream& log) {
  vector<vertex>* tour0 = naiveTour(N);
  // vector<vertex>* tour0 = nearestNeighbour(C, N);
  double tot_seconds = 0.0, part_seconds = 0.0;
  log << "*** ITERATION OF LIN-KERNIGHAN HEURISTIC ***\n";
  TSPsolution best(-1.0);
  set<Tour> exploredSolutions;
  // First tour generated with nearest neighbour
  for (unsigned int i = 0; i < P.LK_iterations;
       ++i, tot_seconds += part_seconds) {
    // Create a new permutation of current tour and add to solutions
    auto pit = exploredSolutions.insert(std::move(createTour(*tour0, C, true)));
    log << "Iteration " << i + 1 << "/" << P.LK_iterations << "\n";
    LK heur(P, N, C, exploredSolutions, pit.first);
    auto start = std::chrono::system_clock::now();
    heur.solve();
    auto end = std::chrono::system_clock::now();
    part_seconds = std::chrono::duration<double>(end - start).count();
    TSPsolution sol = heur.getSolution();
    if (best.objVal == -1.0 || sol.objVal < best.objVal) best = sol;
    log << "Solved in: " << part_seconds << " sec\n" << sol;
  }
  delete tour0;
  return {best, tot_seconds};
}

/**
 * Runs CPLEX exact method
 * @param P hyper-parameters obejct
 * @param N number of vertices
 * @param costs cost matrix
 * @param log_cplex stream where to log results
 * @return pair with solution object and total execution time
 */
pair<TSPsolution, double> utils::runOptimal(const Params& P,
                                            const unsigned int N,
                                            const double* costs,
                                            ostream& log_cplex) {
  std::cout << "Solving with optimal algorithm..." << std::endl;
  std::chrono::_V2::system_clock::time_point start, end;
  double elapsed_seconds;
  // Create CPLEX TSP problem with N holes
  CplexModel mod = CplexModel(N, costs, fs::path::preferred_separator);
  // Solve problem and measure time
  start = std::chrono::system_clock::now();
  mod.solve();
  end = std::chrono::system_clock::now();
  elapsed_seconds = std::chrono::duration<double>(end - start).count();
  log_cplex << "##### OPTIMAL SOLUTION (size " << N << ") #####\n";
  // Log solution to file and add element to display tables
  TSPsolution cplex_res = mod.getSolution();
  log_cplex << "Solved in: " << elapsed_seconds << " sec\n" << cplex_res;

  return {cplex_res, elapsed_seconds};
}