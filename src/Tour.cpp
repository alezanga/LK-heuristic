#include "Tour.hpp"

#include <assert.h>
#include <unordered_set>

using std::pair;
using std::vector;

Tour::Tour(unsigned int N, vector<Node> ed, const double* C) : N(N), tour(ed) {
  // Compute the tour cost
  double tour_cost = 0.0;
  vertex pred = -1, n = 0;
  bool enter = true;
  while (enter || n != 0) {
    enter = false;
    if (ed[n].u != pred) {
      tour_cost += C[n * N + ed[n].u];
      pred = n;
      n = ed[n].u;
    } else {
      tour_cost += C[n * N + ed[n].v];
      pred = n;
      n = ed[n].v;
    }
  }
  cost = tour_cost;
};

/**
 * Remove edge (L[i], L[i+1]) and add L[i-1] to L[i] successors, replacing
 * L[i+1]. Update visited with number of link to L[i-1] and L[i+1] vertexes
 * @param T tour to update
 * @param L list of alternating edges
 * @param i index of the first vertex of the edge to remove. i+1 is the second.
 * @param visited visited[i] contains the number of edges that touch vertex i.
 */
void Tour::disconnect(vector<Node>& T, const vector<vertex>& L, unsigned int i,
                      vector<int>& visited) {
  if (i == L.size() - 1) return;
  assert(T[L[i]].u == L[i + 1] || T[L[i]].v == L[i + 1]);
  // Disconnect from successor, and replace it with predecessor
  if (T[L[i]].u == L[i + 1])
    T[L[i]].u = (i > 0) ? L[i - 1] : L[L.size() - 2];
  else
    T[L[i]].v = (i > 0) ? L[i - 1] : L[L.size() - 2];
  if (i == 0)
    visited[L[L.size() - 2]]++;  // predecessor of zero is before last one
  else
    visited[L[i - 1]]++;
  visited[L[i + 1]]--;
  connect(T, L, i + 1, visited);
}

/**
 * Add edge (L[i], L[i+1]) and remove L[i-1] from L[i] successors. Update
 * visited with number of link to L[i-1] and L[i+1] vertexes
 * @param T tour to update
 * @param L list of alternating edges
 * @param i index of the first vertex of the edge to remove. i+1 is the second.
 * @param visited visited[i] contains the number of edges that touch vertex i.
 */
void Tour::connect(vector<Node>& T, const vector<vertex>& L, unsigned int i,
                   vector<int>& visited) {
  assert(T[L[i]].u == L[i - 1] || T[L[i]].v == L[i - 1]);
  // Connect to successor, and remove reference to predecessor
  if (T[L[i]].u == L[i - 1])
    T[L[i]].u = L[i + 1];
  else
    T[L[i]].v = L[i + 1];
  visited[L[i + 1]]++;
  visited[L[i - 1]]--;
  disconnect(T, L, i + 1, visited);
}

/**
 * Generate a new tour, by adding and removing edges in provided list
 * (alternatively).
 * @param L list of vertices, starting from the first edge to remove to the last
 * edge to add
 * @return if first element is true the second element is the new tour,
 * otherwise it's a default constructed placeholder
 */
pair<bool, vector<Node>> Tour::generate(const vector<vertex>& L) const {
  // Copy the current tour
  vector<Node> tour_copy(tour);

  vector<int> visited(N, 2);

  // Start to generate new tour
  disconnect(tour_copy, L, 0, visited);

  // TOCHECK: this check to see if it's a tour could be improved (and maybe can
  // avoid some passage)

  // Check if it's a tour: all N vertices must be visited exactly 2 times
  if (tour_copy.size() != N) return {false, vector<Node>()};
  for (const int& v : visited)
    if (v != 2) return {false, vector<Node>()};

  std::unordered_set<vertex> encountered;
  vertex n = 0, pred = -1;
  while (encountered.insert(n).second) {
    vertex succ = (tour_copy[n].u == pred) ? tour_copy[n].v : tour_copy[n].u;
    pred = n;
    n = succ;
  }

  // If all vertices could be encountered exactly once it's a tour
  if (encountered.size() == N) return {true, tour_copy};

  return {false, vector<Node>()};
}

void Tour::update(const vector<Node> t, const double gain) {
  // Replace tour
  tour = t;
  // Update cost
  cost -= gain;
}

vector<vertex> Tour::around(const vertex& n) const {
  return vector<vertex>{tour[n].u, tour[n].v};
}

std::string Tour::toString() const {
  std::string s_tour = "";
  vertex i = 0;
  std::unordered_set<int> visited;
  while (visited.size() < N) {
    s_tour += std::to_string(i) + " ";
    visited.insert(i);
    if (visited.find(tour[i].u) != visited.end())
      i = tour[i].v;
    else
      i = tour[i].u;
  }
  return s_tour;
}

double Tour::getObjVal() const { return cost; }

// Tour::Iterator::Iterator(vector<Node>::iterator p) : ptr(p) {}
// bool Tour::Iterator::operator!=(const Iterator& o) const {
//   return ptr != o.ptr;
// }
// // Prefix (++it)
// Tour::Iterator Tour::Iterator::operator++() {
//   ++ptr;
//   return *this;
// }
// const Node& Tour::Iterator::operator*() const { return *ptr; }

// Tour::Iterator Tour::begin() { return Tour::Iterator(Nodes.begin()); };
// Tour::Iterator Tour::end() { return Tour::Iterator(Nodes.end()); };