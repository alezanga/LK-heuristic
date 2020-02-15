#ifndef ITER_LK_H
#define ITER_LK_H

#include <fstream>
#include <vector>

struct Tour;
struct TSPsolution;
struct Params;
typedef unsigned int vertex;

namespace utils {

Tour createTour(std::vector<vertex>, const double*, const bool);

std::vector<vertex>* naiveTour(const unsigned int);
double nearestNeighbour(const double*, const unsigned int);

std::pair<TSPsolution, double> runILK(const Params&, const unsigned int,
                                      const double*, std::ostream&);

std::pair<TSPsolution, double> runOptimal(const Params&, const unsigned int,
                                          const double*, std::ostream&);

}  // namespace utils

#endif /* ITER_LK_H */