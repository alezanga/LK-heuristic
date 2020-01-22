#include "Tour.hpp"

#include <assert.h>

using std::pair;
using std::unordered_set;
using std::vector;

Tour::Tour(unsigned int N, const vector<Node>& ed, const double c)
    : N(N), tour(ed), cost(c){};

/**
 * Remove edge (L[i], L[i+1]) and add L[i-1] to L[i] successors, replacing
 * L[i+1]. Update visited with number of link to L[i-1] and L[i+1] vertexes
 * @param T tour to update
 * @param L list of alternating edges
 * @param i index of the first vertex of the edge to remove. i+1 is the second.
 */
void Tour::disconnect(vector<Node>& T, const vector<vertex>& L,
                      unsigned int i) {
  if (i == L.size() - 1) return;
  assert(T[L[i]].u == L[i + 1] || T[L[i]].v == L[i + 1]);
  // Disconnect from successor, and replace it with predecessor
  if (T[L[i]].u == L[i + 1])
    T[L[i]].u = (i > 0) ? L[i - 1] : L[L.size() - 2];
  else
    T[L[i]].v = (i > 0) ? L[i - 1] : L[L.size() - 2];
  connect(T, L, i + 1);
}

/**
 * Add edge (L[i], L[i+1]) and remove L[i-1] from L[i] successors. Update
 * visited with number of link to L[i-1] and L[i+1] vertexes
 * @param T tour to update
 * @param L list of alternating edges
 * @param i index of the first vertex of the edge to remove. i+1 is the second.
 */
void Tour::connect(vector<Node>& T, const vector<vertex>& L, unsigned int i) {
  assert(T[L[i]].u == L[i - 1] || T[L[i]].v == L[i - 1]);
  // Connect to successor, and remove reference to predecessor
  if (T[L[i]].u == L[i - 1])
    T[L[i]].u = L[i + 1];
  else
    T[L[i]].v = L[i + 1];
  disconnect(T, L, i + 1);
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

  // vector<int> visited(N, 2);

  // Start to generate new tour
  disconnect(tour_copy, L, 0);

  // TOCHECK: this check to see if it's a tour (and maybe can avoid some
  // passage)

  // Check if it's a tour: all N vertices must be visited exactly 2 times
  // SHOUlD BE DONE IN SUBSEQUENT CHECK
  // if (tour_copy.size() != N) return {false, vector<Node>()};
  // for (const int& v : visited)
  //   if (v != 2) return {false, vector<Node>()};

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

vector<vertex> Tour::toVector() const {
  vector<vertex> v;
  vertex i = 0;
  std::unordered_set<int> visited;
  while (visited.size() < N) {
    v.push_back(i);
    visited.insert(i);
    if (visited.find(tour[i].u) != visited.end())
      i = tour[i].v;
    else
      i = tour[i].u;
  }
  return v;
}

double Tour::getObjVal() const { return cost; }

unordered_set<Edge, Edge::Hash>* Tour::edgeSet() const {
  unordered_set<Edge, Edge::Hash>* edges = new unordered_set<Edge, Edge::Hash>;
  for (unsigned int i = 0; i < N; ++i) {
    edges->insert(Edge(i, tour[i].u));
    edges->insert(Edge(i, tour[i].v));
  }
  return edges;
}

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