#include "LK.hpp"
#include "Node.hpp"
#include "TSPsolution.hpp"
#include "Tour.hpp"

#include <algorithm>
#include <limits>

using std::pair;
using std::vector;

// TODO: intensification. keep track of common edges
// TODO: 2-opt trials when a solution is found (nonsequential)
// TODO: random restart
// TODO: remove X and Y and just do a linear search on L
// TODO: is it possible to avoid copying L, and just pushing and popping?
// TODO: put a max K value, or just consider max complexity

LK::LK(unsigned int N, const double* C, const Tour& t, unsigned int max_neigh)
    : N(N), C(C), max_neighbours(max_neigh > 0 ? max_neigh : N - 3), G(0.0) {
  solutions.push_back(t);
}

bool LK::broken(const vector<vertex>& L, const vertex& a, const vertex& b) {
  // TOCHECK: L.size() - 1 not a problem since size >= 2 when this is called
  for (unsigned int i = 0; i < L.size() - 1; i += 2)
    if (((L[i] == a) && (L[i + 1] == b)) || ((L[i] == b) && (L[i + 1] == a)))
      return true;
  return false;
}

bool LK::joined(const vector<vertex>& L, const vertex& a, const vertex& b) {
  for (unsigned int i = 1; i < L.size() - 1; i += 2)
    if (((L[i] == a) && (L[i + 1] == b)) || ((L[i] == b) && (L[i + 1] == a)))
      return true;
  return false;
}

void LK::solve() {
  bool improved = true;

  while (improved) {
    // Reset gain
    G = 0.0;
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
      // Start by removing edge (t1, t2) = x1
      vector<vertex> L{t1, t2};
      // Gain by removing edge (t1, t2)
      double g0 = C[t1 * N + t2];
      // Find neighbours of t2. Y is empty since there are still no added
      // edges
      vector<vertex> neighbours = neighbourhood(t1, t2, g0, tour, L);

      // Try all edges (t2, t3) = y1
      for (const vertex& t3 : neighbours) {
        // Gain by adding edge (t2, t3)
        double gi = g0 - C[t2 * N + t3];
        // NOTE: t3 can't be the successor or predecessor of t2 in the tour
        // and neighbours are only the nodes which provide a positive gain
        // if added

        // Add edge (t2, t3)
        L.push_back(t3);

        if (chooseX(tour, t1, t3, gi, L, 2))
          // Stop improving and restart again
          return true;
        // else no tour could be closed, try other neighbours
        L.pop_back();  // remove t3
      }
      // Finished all neighbours, no move improved the current tour. So start
      // by removing a different edge.
    }
    // The edge (t1, t2) didn't improve solution. Start from a different vertex.
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
vector<vertex> LK::neighbourhood(const vertex& t1, const vertex& t2i,
                                 double gain, const Tour& tour,
                                 const vector<vertex>& L) {
  // Possible neighbours are all nodes, excluding itself and the two
  // neighbours in the current tour. Each pair contains the potential gain and
  // the corresponding vertex
  vector<pair<double, int>> neighbours(
      N, {std::numeric_limits<double>::lowest(), -1});

  // For all possible vertices
  // TOCHECK: previoulsy was cycling on tour. Should not be a problem
  for (vertex n = 0; n < N; ++n) {
    // Gain by adding edge (t2i, n)
    double gi = gain - C[t2i * N + n];
    // Check if n is around t2i in tour, if (t2i, n) was already deleted and put
    // in X
    vector<vertex> ar_t2i = tour.around(t2i);
    if (n != ar_t2i[0] && n != ar_t2i[1] && n != t2i && n != t1 &&
        !joined(L, t2i, n) && gi > 0) {
      // TOCHECK: I removed !X (!broken) because n is not around t2i so it
      // cannot have been removed.
      // Consider adding edge yi = (t_2i, t_2i + 1) =
      // (t2i, n) Whole point of this subsequent part is ranking the
      // possible neighbours (t_2i+1), by seeing the potential gain they
      // could allow by removing the next edge
      vector<vertex> around_n = tour.around(n);
      // NOTE: This set of nodes can't contain t2i, since n was not around t2i
      // For each of the two possible edges to remove
      for (const vertex& succ_n : around_n) {
        // succ_n = t_2i+2, now search for a proper x_i+1
        // If (t_2i+1, t_2i+2) = (n, succ_n) has not been already broken (is in
        // X) and has not been already added (in Y)
        if (!broken(L, n, succ_n)) {
          // TOCHECK: removed (Y.empty() || !Y[n * N + succ_n]) since no edge
          // belonging to the tour can be added
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
  // Remove potential gain from vector and return the number of best neighbour
  // (bounded by max)
  vector<vertex> ord_neighbours;
  for (unsigned int i = 0;
       i < N && neighbours[i].second != -1 && i < max_neighbours; ++i)
    ord_neighbours.push_back(neighbours[i].second);
  return ord_neighbours;
}

bool LK::chooseX(Tour& tour, const vertex& t1, const vertex& lasty, double gain,
                 vector<vertex> L, const int i) {
  vector<vertex> around_lasty = tour.around(lasty);
  // For each of the two neighbours of lasty
  for (const vertex& t2i : around_lasty) {
    // Consider removing edge (lasty, t2i)
    double gi = gain + C[lasty * N + t2i];

    if (t2i != t1 && !broken(L, lasty, t2i)) {
      // TOCHECK: removed !Y[lasty * N + t2i]. A tour edge cannot have been
      // joined

      // Remove edge (lasty, t2i)
      L.push_back(t2i);

      // Relink to t1 adding edge (t2i, t1)
      L.push_back(t1);

      // Gain by relinking (adding edge (t2i, t1))
      double relink_gain = gi - C[t2i * N + t1];

      // Check to see if L_relink forms a valid tour
      pair<bool, vector<Node>> is_tour = tour.generate(L);

      // Remove edge (t2i, t1), since we want to continue improving, if possible
      L.pop_back();

      // If L_relink is a valid tour
      if (is_tour.first) {
        // If gain is better found so far
        if (relink_gain > G) {
          // Save best improvement so far
          G = relink_gain;
          // Go on improving
          if (!chooseY(tour, t1, t2i, gi, L, i))
            // Save new tour with relink
            tour.update(is_tour.second, relink_gain);
          // Improvement found, restart
          return true;
        } else if (i <= 2) {
          // Limited backtracking if i == 2
          // Try to see if it is possible to ge a gainful tour later
          if (chooseY(tour, t1, t2i, gi, L, i)) return true;
          // If not, try the other node, which cannot relink with t1
          vertex alt_t2i =
              (t2i == around_lasty[0]) ? around_lasty[1] : around_lasty[0];
          double alt_gi = gain + C[lasty * N + alt_t2i];
          if (alt_t2i != t1 && !broken(L, lasty, alt_t2i)) {
            // Remove t2i
            L.pop_back();
            // Replace it with alt_t2i
            L.push_back(alt_t2i);
            return chooseY(tour, t1, alt_t2i, alt_gi, L, i);
          }
        }
        // No improvement over previous gain G (relink_gain <= G) & trying other
        // neighbour didn't succeed, or was not possible (i > 2), then terminate
        // choice of x_i and exit loop
        break;
      }
      // If tour is not valid try the other neighbour
      L.pop_back();  // remove t2i
    }
    // else try the other neighbour of lasty
  }
  // No edge can be removed. Search other starting edge
  return false;
}

bool LK::chooseY(Tour& tour, const vertex& t1, const vertex& lastx, double gain,
                 vector<vertex> L, const int i) {
  vector<vertex> neighbours_ordered = neighbourhood(t1, lastx, gain, tour, L);

  for (const vertex& t_odd : neighbours_ordered) {
    // Add edge (lastx, t_odd)
    L.push_back(t_odd);

    // Gain from adding (lastx, t_odd)
    double gc = gain - C[lastx * N + t_odd];

    // Stop at first improving tour
    if (chooseX(tour, t1, t_odd, gc, L, i + 1)) return true;
    // else pop t_odd and try other possibilities
    L.pop_back();
  }

  // No improving edge to add starting from lastx
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