#ifndef LIN_K
#define LIN_K

#include <vector>

class Tour;
typedef unsigned int vertex;
struct TSPsolution;

struct GreaterPair {
  template <class T, class U>
  bool operator()(const std::pair<T, U>& a, const std::pair<T, U>& b) const {
    return a.first > b.first;
  }
};

class LK {
 private:
  unsigned int N;
  double* C;
  std::vector<Tour> solutions;
  unsigned int max_neighbours;

  std::vector<vertex> neighbourhood(const vertex&, double, const Tour&,
                                    const std::vector<bool>&,
                                    const std::vector<bool>&);

  /**
   * Choose one edge to remove from the tour, starting from node lasty. The
   * removed node will be one of the two successor of last in tour.
   * @param t1: initial node for current k-opt tour
   * @param lasty: last node added to Y (which is t_2i-1)
   */
  bool chooseX(Tour&, const vertex&, const vertex&, double, std::vector<vertex>,
               std::vector<bool>, std::vector<bool>);

  /**
   * Choose one edge to add to the tour, starting from node lastx.
   * @param t1: initial node for current k-opt tour
   * @param lastx: last node removed from original tour (added to X)
   */
  bool chooseY(Tour&, const vertex&, const vertex&, double, std::vector<vertex>,
               std::vector<bool>, std::vector<bool>);

  bool improve(Tour&);

 public:
  static Tour initializeTour(const unsigned int, const double*);
  LK(unsigned int, double*, const Tour&, unsigned int = 0);
  void solve();
  const TSPsolution getSolution() const;
};

#endif /* LIN_K */