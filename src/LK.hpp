#ifndef LIN_K
#define LIN_K

#include <unordered_set>
#include <vector>

#include "Pair.hpp"
#include "Params.cpp"

struct Tour;
struct TSPsolution;

class LK {
 private:
  unsigned int N;
  const double* C;
  std::vector<Tour>& solutions;
  double G;
  std::unordered_set<Pair, Pair::Hash>* good_edges;
  bool intensify;
  Params P;

  static bool broken(const std::vector<vertex>&, const vertex&, const vertex&);
  static bool joined(const std::vector<vertex>&, const vertex&, const vertex&);
  void updateGoodEdges(const std::vector<vertex>&);

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
  LK(unsigned int, const double*, std::vector<Tour>&);
  ~LK();
  void solve();
  const TSPsolution getSolution() const;
};

#endif /* LIN_K */