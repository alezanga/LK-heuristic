#include "Tour.hpp"

#include <unordered_set>

using std::pair;
using std::set;
using std::unordered_set;
using std::vector;

Tour::Tour(unsigned int N, const vector<Pair>& ed, const double c)
    : N(N), tour(ed), cost(c){};

/**
 * Remove edge (L[i], L[i+1]) and add L[i-1] to L[i] successors, replacing
 * L[i+1]. Update visited with number of link to L[i-1] and L[i+1] vertexes
 * @param T tour to update
 * @param L list of alternating edges
 * @param i index of the first vertex of the edge to remove. i+1 is the second.
 */
void Tour::disconnect(vector<Pair>& T, const vector<vertex>& L,
                      unsigned int i) {
  if (i == L.size() - 1) return;
  // Disconnect from successor, and replace it with predecessor
  if (T[L[i]].u() == L[i + 1])
    T[L[i]] = Pair((i > 0) ? L[i - 1] : L[L.size() - 2], T[L[i]].v());
  else
    T[L[i]] = Pair((i > 0) ? L[i - 1] : L[L.size() - 2], T[L[i]].u());
  connect(T, L, i + 1);
}

/**
 * Add edge (L[i], L[i+1]) and remove L[i-1] from L[i] successors. Update
 * visited with number of link to L[i-1] and L[i+1] vertexes
 * @param T tour to update
 * @param L list of alternating edges
 * @param i index of the first vertex of the edge to remove. i+1 is the second.
 */
void Tour::connect(vector<Pair>& T, const vector<vertex>& L, unsigned int i) {
  // Connect to successor, and remove reference to predecessor
  if (T[L[i]].u() == L[i - 1])
    T[L[i]] = Pair(L[i + 1], T[L[i]].v());
  else
    T[L[i]] = Pair(L[i + 1], T[L[i]].u());
  disconnect(T, L, i + 1);
}

/**
 * Generate a new tour, by adding and removing edges in provided list
 * (alternatively).
 * @param tour current tour object from which to start
 * @param L list of vertices, starting from the first edge to remove to the last
 * edge to add
 * @return a pointer to new tour, or nullptr if it wasn't a tour
 */
pair<bool, Tour> Tour::generate(const Tour& tour, const vector<vertex>& L,
                                const double gain) {
  // Copy the current tour
  vector<Pair> tour_copy(tour.tour);

  // vector<int> visited(N, 2);

  // Start to generate new tour
  disconnect(tour_copy, L, 0);

  // TOCHECK: this check to see if it's a tour (and maybe can avoid some
  // passage)

  // Check if it's a tour: all N vertices must be visited exactly 2 times
  // SHOUlD BE DONE IN SUBSEQUENT CHECK
  // if (tour_copy.size() != N) return {false, vector<Pair>()};
  // for (const int& v : visited)
  //   if (v != 2) return {false, vector<Pair>()};

  std::unordered_set<vertex> encountered;
  vertex n = 0, pred = -1;
  while (encountered.insert(n).second) {
    vertex succ =
        (tour_copy[n].u() == pred) ? tour_copy[n].v() : tour_copy[n].u();
    pred = n;
    n = succ;
  }

  // If all vertices could be encountered exactly once it's a tour
  // Create new tour with updated cost
  auto gg = (tour.cost) - gain;
  if (encountered.size() == tour.N) return {true, Tour(tour.N, tour_copy, gg)};

  return {false, Tour(tour.N)};
}

/**
 * Gives the two vertices connected to n in current tour ordered by decreasing
 * cost of edge
 * @param n vertex
 * @param C cost matrix
 * @return vector of two elements
 */
vector<vertex> Tour::around(const vertex& n, const double* C) const {
  double c1 = C[n * N + tour[n].u()];
  double c2 = C[n * N + tour[n].v()];
  return c1 > c2 ? vector<vertex>{tour[n].u(), tour[n].v()}
                 : vector<vertex>{tour[n].v(), tour[n].u()};
}

std::string Tour::toString() const {
  std::string s_tour = "";
  vertex i = 0;
  std::unordered_set<vertex> visited;
  while (visited.size() < N) {
    s_tour += std::to_string(i) + " ";
    visited.insert(i);
    if (visited.find(tour[i].u()) != visited.end())
      i = tour[i].v();
    else
      i = tour[i].u();
  }
  return s_tour;
}

vector<vertex> Tour::toVector() const {
  vector<vertex> v;
  vertex i = 0;
  std::unordered_set<vertex> visited;
  while (visited.size() < N) {
    v.push_back(i);
    visited.insert(i);
    if (visited.find(tour[i].u()) != visited.end())
      i = tour[i].v();
    else
      i = tour[i].u();
  }
  return v;
}

double Tour::getObjVal() const { return cost; }

set<Pair> Tour::edgeSet() const {
  set<Pair> edges;
  for (unsigned int i = 0; i < N; ++i) {
    edges.insert(Pair(i, tour[i].u()));
    edges.insert(Pair(i, tour[i].v()));
  }
  return edges;
}

bool Tour::operator==(const Tour& t) const {
  return N == t.N && cost == t.cost && tour == t.tour;
}

/**
 * Returns whether this tour is better (cheaper) than the other.
 * The comparison doesn't make sense if the two tours are of different sizes.
 */
bool Tour::operator<(const Tour& t) const {
  if (N != t.N) throw std::string("Incomparable tours");
  if (cost < t.cost) return true;
  if (cost > t.cost) return false;
  // Costs are the same, so lexicographic compare of vector is used
  return tour < t.tour;
}

// Tour::Iterator::Iterator(vector<Pair>::iterator p) : ptr(p) {}
// bool Tour::Iterator::operator!=(const Iterator& o) const {
//   return ptr != o.ptr;
// }
// // Prefix (++it)
// Tour::Iterator Tour::Iterator::operator++() {
//   ++ptr;
//   return *this;
// }
// const Pair& Tour::Iterator::operator*() const { return *ptr; }

// Tour::Iterator Tour::begin() { return Tour::Iterator(Pairs.begin()); };
// Tour::Iterator Tour::end() { return Tour::Iterator(Pairs.end()); };