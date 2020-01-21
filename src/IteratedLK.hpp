#ifndef ITER_LK_H
#define ITER_LK_H

#include <fstream>
#include <vector>

struct Tour;
struct TSPsolution;
struct Params;
typedef unsigned int vertex;

Tour* permuteTour(std::vector<vertex>, const double*);

std::vector<vertex>* naiveTour(const unsigned int);

std::pair<TSPsolution, double> iterated_LK(const Params&, const unsigned int,
                                           const double*, std::ofstream&);

#endif /* ITER_LK_H */