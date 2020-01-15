#include "LK.hpp"
#include "Node.hpp"
#include "TSPsolution.hpp"
#include "Tour.hpp"

#include <algorithm>
#include <limits>

using std::pair;
using std::vector;

LK::LK(unsigned int N, double* C, const Tour& t, unsigned int max_neigh)
    : N(N), C(C), max_neighbours(max_neigh > 0 ? max_neigh : N - 3) {
  solutions.push_back(t);
}

void LK::solve() {
  bool improved = true;

  while (improved) {
    // Take last improved solution
    Tour current = solutions.back();
    // Improve the current solution with LK
    improved = improve(current);
    // Save improved solution
    solutions.push_back(current);
  }
  // Stop, since no run improved the gain
}

bool LK::improve(Tour& tour) {
  for (vertex t1 = 0; t1 < N; ++t1) {
    vector<vertex> around = tour.around(t1);
    for (const vertex& t2 : around) {
      // X tells whether edge (a, b) has been removed (if X[a*N+b]==true)
      vector<bool> X(N * N, false);
      // Start by removing edge (t1, t2)
      vector<vertex> L{t1, t2};
      X[t1 * N + t2] = X[t2 * N + t1] = true;
      // Gain by removing edge (t1, t2)
      double g0 = C[t1 * N + t2];
      // Find neighbours of t2. Y is empty since there are still no added
      // edges
      vector<vertex> neighbours =
          neighbourhood(t2, g0, tour, X, vector<bool>());

      // Keep track of number of neighbours tried (max N-3)
      unsigned int neighbours_explored = 0;

      for (const vertex& t3 : neighbours) {
        // Make a copy of L in order to try all edges (t2, t3)
        vector<vertex> L_copy(L);
        // Gain by adding edge (t2, t3)
        double gi = g0 - C[t2 * N + t3];
        // NOTE: t3 can't be the successor or predecessor of t2 in the tour
        // and neighbours are only the nodes which provide a positive gain
        // if added
        vector<bool> Y(N * N, false);

        // Add edge (t2, t3)
        L_copy.push_back(t3);
        // Mark it as added
        Y[t2 * N + t3] = Y[t3 * N + t2] = true;

        if (chooseX(tour, t1, t3, gi, L_copy, X, Y))
          // Stop improving and restart again
          return true;
        // else no tour could be closed, try other neighbours

        if (++neighbours_explored == max_neighbours) break;
      }
      // Finished all neighbours, no move improved the current tour. So start
      // by removing a different edge.
    }
    // The two edges didn't improve solution. Start from a different vertex.
  }
  // No improving solutions
  return false;
}

/**
 * Return possible vertices not around t2i in current tour for adding a new
 * edge. They are ranked from best gain to worst, in order to do a smarter
 * choice.
 * @param t2i node from which we want to search neighbours
 * @param gain gain for moves up to t2i
 * @param tour current tour
 * @param X vector to signal which edges have already been removed
 * @param Y vector to signal which edges have already been added
 */
vector<vertex> LK::neighbourhood(const vertex& t2i, double gain,
                                 const Tour& tour, const vector<bool>& X,
                                 const vector<bool>& Y) {
  // Possible neighbours are all nodes, excluding itself and the two
  // neighbours in the current tour. Each pair contains the potential gain and
  // the corresponding vertex
  vector<pair<double, int>> neighbours(
      N, {std::numeric_limits<double>::min(), -1});

  // For all possible vertices
  // TOCHECK: previoulsy was cycling on tour. Should not be a problem
  for (vertex n = 0; n < N; ++n) {
    // Gain by adding edge (t2i, n)
    double gi = gain - C[t2i * N + n];
    // Check if n is around t2i in tour, if (t2i, n) was already deleted and put
    // in X
    vector<vertex> ar_t2i = tour.around(t2i);
    if (n != ar_t2i[0] && n != ar_t2i[1] && !X[t2i * N + n] && gi > 0) {
      // TOCHECK: check also if !Y ?
      // NOTE: case n=t2i is already filtered by gi > 0 (since it is -inf)
      // Consider adding edge yi = (t_2i, t_2i+1) = (t2i, n)
      // Whole point of this subsequent part is ranking the possible
      // neighbours (t_2i+1), by seeing the potential gain they could allow by
      // removing the next edge
      vector<vertex> around_n = tour.around(n);
      // NOTE: This set of nodes can't contain t2i, since n was not around t2i
      // For each of the two possible edges to remove
      for (const vertex& succ_n : around_n) {
        // succ_n = t_2i+2, now search for a proper x_i+1
        // If (t_2i+1, t_2i+2) = (n, succ_n) has not been already broken (is in
        // X) and has not been already added (in Y)
        if (!X[n * N + succ_n] && (Y.empty() || !Y[n * N + succ_n])) {
          // Compute potential gain of removing x_i+1 and adding y_i
          double delta_g = C[n * N + succ_n] - C[t2i * N + n];
          if (neighbours[n].second == -1 || neighbours[n].first < delta_g)
            neighbours[n] = {delta_g, n};
          // Else skip update and go on
        }
      }
    }
  }
  // Sort by first element (so by potential gain) in descending order
  std::sort(neighbours.begin(), neighbours.end(), GreaterPair());
  // Remove potential gain from vector
  vector<vertex> ord_neighbours;
  for (unsigned int i = 0; i < N && neighbours[i].second != -1; ++i)
    ord_neighbours.push_back(neighbours[i].second);
  return ord_neighbours;
}

bool LK::chooseX(Tour& tour, const vertex& t1, const vertex& lasty, double gain,
                 vector<vertex> L, vector<bool> X, vector<bool> Y) {
  vector<vertex> around_lasty = tour.around(lasty);
  // For each of the two neighbours of lasty
  for (const vertex& t2i : around_lasty) {
    // Consider removing edge (lasty, t2i)
    double gi = gain + C[lasty * N + t2i];

    if (t2i != t1 && !X[lasty * N + t2i] && !Y[lasty * N + t2i]) {
      // TOCHECK: !Y can be removed?

      // TOCHECK: Should I really modify reference?
      vector<vertex> L_copy(L);
      vector<bool> X_copy(X);
      // Remove edge (lasty, t2i)
      L_copy.push_back(t2i);
      // Mark it as removed
      X_copy[lasty * N + t2i] = X_copy[t2i * N + lasty] = true;

      vector<vertex> L_relink(L_copy);
      vector<bool> Y_relink(Y);

      // Relink to t1 adding edge (t2i, t1)
      L_relink.push_back(t1);
      // Mark it as added
      Y_relink[t2i * N + t1] = Y_relink[t1 * N + t2i] = true;

      // Gain by relinking (adding edge (t2i, t1))
      double relink_gain = gi - C[t2i * N + t1];

      // Check to see if L_relink forms a valid tour
      pair<bool, vector<Node>> is_tour = tour.generate(L_relink);
      // If L_relink is a valid tour
      if (is_tour.first) {
        // If gain is positive
        if (relink_gain > 0) {  // TOCHECK: no need to Y = T_relink ?
          // Save new tour with relink
          tour.update(is_tour.second, relink_gain);
          // Restart from main loop
          return true;
        } else
          // Go on searching new edge to add, since it's not profitable to close
          // the tour now (gain < 0)
          // Corresponds to enlarging the k parameter of k-opt move
          return chooseY(tour, t1, t2i, gi, L_copy, X_copy, Y);
      }
      // If tour is not valid try the other neighbour
    }
    // else try the other neighbour
  }
  // No edge can be removed (either they are already removed or t1 is one of
  // the successor). Seach other starting edge
  return false;
}

bool LK::chooseY(Tour& tour, const vertex& t1, const vertex& lastx, double gain,
                 vector<vertex> L, vector<bool> X, vector<bool> Y) {
  vector<vertex> neighbours_ordered = neighbourhood(t1, gain, tour, X, Y);

  // Keep track of # of neighbours tried
  unsigned int neighbours_explored = 0;

  for (const vertex& t_odd : neighbours_ordered) {
    // TOCHECK: deep copy

    // Make a copy, since all trials for t_odd are independent
    vector<vertex> L_copy(L);
    vector<bool> Y_copy(Y);

    // Add edge (lastx, t_odd)
    L_copy.push_back(t_odd);
    // Mark it as added
    Y_copy[lastx * N + t_odd] = Y_copy[t_odd * N + lastx] = true;

    // Gain from adding (lastx, t_odd)
    double gc = gain - C[lastx * N + t_odd];

    // Stop at first improving tour
    if (chooseX(tour, t1, t_odd, gc, L_copy, X, Y_copy)) return true;
    // else try other neighbours
    if (++neighbours_explored == max_neighbours) break;
  }

  // No improving edge to add starting from lastx, so keep searching
  return false;
}

const TSPsolution LK::getSolution() const {
  Tour final_tour = solutions.back();
  return TSPsolution(final_tour.getObjVal(), N, nullptr, nullptr,
                     final_tour.toString());
}

Tour LK::initializeTour(const unsigned int N, const double* C) {
  vector<Node> r(N);
  for (vertex i = 0; i < N; ++i) {
    r[i] = (i == 0) ? Node(N - 1, i + 1)
                    : ((i == N - 1) ? Node(i - 1, 0) : Node(i - 1, i + 1));
  }
  return Tour(N, r, C);
}