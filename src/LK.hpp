#ifndef LIN_K
#define LIN_K

#include <set>
#include <vector>

#include "Params.cpp"

struct Tour;
struct Pair;
struct TSPsolution;

typedef unsigned int vertex;
typedef std::vector<std::pair<std::set<Pair>, double>> heap_set;

class LK {
 private:
  unsigned int N;
  const double* C;
  std::set<Tour>& solutions;
  std::set<Tour>::iterator current_it;
  // Parameters for local search procedure
  double G;
  // A maxheap with best T tours, and sorted vector with their intersection
  std::pair<heap_set, std::vector<Pair>> good_edges;
  bool intensify;
  bool restart;
  Params P;

  static bool broken(const std::vector<vertex>&, const vertex&, const vertex&);
  static bool joined(const std::vector<vertex>&, const vertex&, const vertex&);
  void intersectGoodEdges();
  void updateGoodEdges(const Tour&);

  std::vector<vertex> neighbourhood(const vertex&, const vertex&, double,
                                    const Tour&, const std::vector<vertex>&,
                                    const unsigned int);

  /**
   * Choose one edge to remove from the tour, starting from node lasty. The
   * removed node will be one of the two successor of last in tour.
   * @param t1: initial node for current k-opt tour
   * @param lasty: last node added to Y (which is t_2i-1)
   */
  bool chooseX(Tour&, const vertex&, const vertex&, double,
               std::vector<vertex>&, const unsigned int);

  /**
   * Choose one edge to add to the tour, starting from node lastx.
   * @param t1: initial node for current k-opt tour
   * @param lastx: last node removed from original tour (added to X)
   */
  bool chooseY(Tour&, const vertex&, const vertex&, double,
               std::vector<vertex>&, const unsigned int);

  bool improve(Tour&);

 public:
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
  LK(unsigned int, const double*, std::set<Tour>&,
     const std::set<Tour>::iterator&);
  void solve();
  const TSPsolution getSolution() const;
};

#endif /* LIN_K */