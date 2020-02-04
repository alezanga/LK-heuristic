#include "LK.hpp"
#include "Pair.hpp"
#include "TSPsolution.hpp"
#include "Tour.hpp"

#include <assert.h>
#include <algorithm>
#include <limits>
#include <set>

using std::pair;
using std::set;
using std::vector;

// TOCHECK: intensification. keep track of common edges. Should work

/**
 * Compute intersection of all set in good_edges. Then replace good_edges.second
 * with the new intersection.
 */
void LK::intersectGoodEdges() {
  // Update intersection
  good_edges.second.clear();
  vector<pair<set<Pair>, double>>::const_iterator it =
      good_edges.first.cbegin();
  // Work with vectors for more efficiency
  vector<Pair> last_intersection(it->first.cbegin(), it->first.cend());
  vector<Pair> tmp;
  for (; it != good_edges.first.cend(); it++) {
    const auto& [edgeset, _c] = *it;
    (void)_c;  // unused variable
    std::set_intersection(edgeset.begin(), edgeset.end(),
                          last_intersection.begin(), last_intersection.end(),
                          std::back_inserter(tmp));
    std::swap(last_intersection, tmp);
    tmp.clear();
  }
  good_edges.second = last_intersection;
}

/**
 * Update the heap of best tours if necessary and update the intersection of
 * their edges.
 * @param newTour the newly generated tour
 */
void LK::updateGoodEdges(const Tour& newTour) {
  if (good_edges.first.empty())
    good_edges.first.push_back(
        {std::move(newTour.edgeSet()), newTour.getObjVal()});
  // goodEdges.first is a maxheap, so tour of highest cost is at position 0
  else {
    auto less = [](const pair<set<Pair>, double>& a,
                   const pair<set<Pair>, double>& b) {
      return a.second < b.second;
    };
    if (good_edges.first.size() < P.intens_n_tours) {
      good_edges.first.push_back(
          {std::move(newTour.edgeSet()), newTour.getObjVal()});
      std::push_heap(good_edges.first.begin(), good_edges.first.end(), less);
    } else {
      assert(good_edges.first.at(0).second > newTour.getObjVal());
      // New best solution
      // Pop worst tour in heap
      std::pop_heap(good_edges.first.begin(), good_edges.first.end(), less);
      good_edges.first.pop_back();
      // Push the new one
      good_edges.first.push_back(
          {std::move(newTour.edgeSet()), newTour.getObjVal()});
      std::push_heap(good_edges.first.begin(), good_edges.first.end(), less);

      // Update intersection
      if (intensify) intersectGoodEdges();
    }
    // else nothing to do
  }
}

/**
 * Build Lin-Kernighan heuristic solver
 *
 * @param N size of the problem
 * @param C costs matrix
 * @param t initial tour
 * @param max_neigh maximum number of neighbours to consider when searching an
 * edge to add
 * @param int_depth minimum depth to apply intensification
 * @param int_sols minimum number of solutions before applying intensification
 */
LK::LK(unsigned int N, const double* C, set<Tour>& vt,
       const std::set<Tour>::iterator& curr)
    : N(N), C(C), solutions(vt), current_it(curr), G(0.0), intensify(false) {
  if (P.max_neighbours == 0) P.max_neighbours = N - 3;
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
  unsigned int i = 0;
  while (improved && i < P.I) {
    // Reset gain
    G = 0.0;
    // Take last improved solution
    Tour curr_tour = *current_it;
    // Improve the current solution with LK
    improved = improve(curr_tour);
    std::cout << i << " - improved: " << improved << std::endl;
    // Save improved solution
    auto [new_it, ins] = solutions.insert(curr_tour);
    if (ins)
      current_it = new_it;
    else
      break;  // Cannot happen, since every solution is improving
    i++;
    // If we obtained min number of local optima, start intensification
    if (!intensify && i > P.intens_n_tours) {
      intensify = true;
      intersectGoodEdges();
    }
  }
  // Stop, since no run improved the gain or max number of iterations reached
}

bool LK::improve(Tour& tour) {
  for (vertex t1 = 0; t1 < N; ++t1) {
    vector<vertex> around = tour.around(t1, C);
    for (const vertex& t2 : around) {
      // Start by removing edge (t1, t2) = x1
      // If it is a good edge then stop and try the other neighbour
      bool removable =
          !intensify || 1 < P.intens_min_depth ||
          !std::binary_search(good_edges.second.cbegin(),
                              good_edges.second.cend(), Pair(t1, t2));
      if (!removable) continue;

      // Remove (t1, t2)
      vector<vertex> L{t1, t2};
      // Gain by removing edge (t1, t2)
      double g0 = C[t1 * N + t2];
      // Find neighbours of t2. Y is empty since there are still no added
      // edges
      vector<vertex> neighbours = neighbourhood(t1, t2, g0, tour, L, 1);

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
                                 const vector<vertex>& L,
                                 const unsigned int i) {
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
    vector<vertex> ar_t2i = tour.around(t2i, C);
    if (n != ar_t2i[0] && n != ar_t2i[1] && n != t2i && n != t1 &&
        !joined(L, t2i, n) && gi > 0) {
      // TOCHECK: I removed !X (!broken) because n is not around t2i so it
      // cannot have been removed.
      // Consider adding edge yi = (t_2i, t_2i + 1) =
      // (t2i, n) Whole point of this subsequent part is ranking the
      // possible neighbours (t_2i+1), by seeing the potential gain they
      // could allow by removing the next edge
      vector<vertex> around_n = tour.around(n, C);
      // NOTE: This set of nodes can't contain t2i, since n was not around t2i
      // For each of the two possible edges to remove
      for (const vertex& succ_n : around_n) {
        // succ_n = t_2i+2, now search for a proper x_i+1
        // If (t_2i+1, t_2i+2) = (n, succ_n) has not been already broken and is
        // not a good edge
        bool removable =
            !intensify || i + 1 < P.intens_min_depth ||
            !std::binary_search(good_edges.second.cbegin(),
                                good_edges.second.cend(), Pair(n, succ_n));
        if (!broken(L, n, succ_n) && removable) {
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
  std::sort(neighbours.begin(), neighbours.end(),
            [](const pair<double, int>& a, const pair<double, int>& b) {
              return a.first > b.first;
            });
  // Remove potential gain from vector and return the number of best neighbour
  // (bounded by max)
  vector<vertex> ord_neighbours;
  for (unsigned int i = 0;
       i < N && neighbours[i].second != -1 && i < P.max_neighbours; ++i)
    ord_neighbours.push_back(neighbours[i].second);
  return ord_neighbours;
}

bool LK::chooseX(Tour& tour, const vertex& t1, const vertex& lasty, double gain,
                 vector<vertex>& L, const unsigned int i) {
  unsigned int backtraking_threshold = 2;
  vector<vertex> around_lasty = tour.around(lasty, C);
  bool improvedx = false;
  bool try_both_neighbours = false;
  bool stop = false;

  // For each of the two neighbours of lasty
  for (unsigned int j = 0; j < around_lasty.size() && !stop; ++j) {
    vertex t2i = around_lasty[j];
    // Consider removing edge (lasty, t2i)
    double gi = gain + C[lasty * N + t2i];

    bool removable =
        !intensify || i < P.intens_min_depth ||
        !std::binary_search(good_edges.second.cbegin(),
                            good_edges.second.cend(), Pair(lasty, t2i));
    if (t2i != t1 && !broken(L, lasty, t2i) && removable) {
      // TOCHECK: removed !Y[lasty * N + t2i]

      // Remove edge (lasty, t2i)
      L.push_back(t2i);

      if (try_both_neighbours) {
        //"try_both_neighbours" implies previous choice was a tour.
        improvedx = chooseY(tour, t1, t2i, gi, L, i);
        // Neighbour has been tried
        try_both_neighbours = false;
        stop = true;
      } else {
        // Relink to t1 adding edge (t2i, t1)
        L.push_back(t1);

        // Gain by relinking (adding edge (t2i, t1))
        double relink_gain = gi - C[t2i * N + t1];

        // Check to see if L_relink forms a valid tour
        auto [is_tour, new_tour] = Tour::generate(tour, L, relink_gain);

        // Remove edge (t2i, t1), since we want to continue improving, if
        // possible
        L.pop_back();  // remove t1

        if (is_tour) {
          bool found = solutions.find(new_tour) != solutions.end();
          if (found) {
            // Tour already present -> always stop
            improvedx = false;
            stop = true;
          } else if (relink_gain > G) {
            // Tour is actually a new improving solution
            G = relink_gain;
            if (i > P.K || !chooseY(tour, t1, t2i, gi, L, i)) {
              // If it cannot improve further
              // Replace it with new improving tour
              tour = new_tour;
              updateGoodEdges(tour);
            }
            // either chooseY or new_tour improved
            improvedx = true;
            stop = true;
          } else if (i <= backtraking_threshold) {
            // Try to go see if it becomes a gainful tour later on
            improvedx = chooseY(tour, t1, t2i, gi, L, i);
            if (!improvedx) {
              // No improvement from going on with current tour, so let's try
              // other neighbour (in this case stop will be == false)
              try_both_neighbours = true;
              // Add to vector the other possibility for t2i
              if (j == 1) around_lasty.push_back(around_lasty[0]);
            } else
              stop = true;  // improved
          } else {
            // if it is a tour not seen before but with bad gain and no
            // backtraking is allowed
            improvedx = false;
            stop = true;  // no point into going on since it will not be a tour
          }
        }  // if tour
        // If it's not a tour try other neighbour
      }
      if (!improvedx) L.pop_back();  // remove t2i
    }                                // if
  }                                  // for
  return improvedx;
}

bool LK::chooseY(Tour& tour, const vertex& t1, const vertex& lastx, double gain,
                 vector<vertex>& L, const unsigned int i) {
  vector<vertex> neighbours_ordered =
      neighbourhood(t1, lastx, gain, tour, L, i);

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
  // TODO: maybe I can avoid passing both string and vector. I could also
  // use python to plit the string
  Tour final_tour = *current_it;
  return TSPsolution(final_tour.getObjVal(), N, nullptr, nullptr,
                     final_tour.toString(), final_tour.toVector());
}
