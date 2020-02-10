#ifndef ITER_LK_H
#define ITER_LK_H

#include <fstream>
#include <vector>

struct Tour;
struct TSPsolution;
struct Params;
typedef unsigned int vertex;

Tour createTour(std::vector<vertex>, const double*, const bool);

std::vector<vertex>* naiveTour(const unsigned int);
double nearestNeighbour(const double*, const unsigned int);

std::pair<TSPsolution, double> iterated_LK(const Params&, const unsigned int,
                                           const double*, std::ostream&);

#endif /* ITER_LK_H */