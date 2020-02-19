#ifndef TOUR_H
#define TOUR_H

#include <set>
#include <string>
#include <vector>

#include "Pair.hpp"

typedef unsigned int vertex;

class Tour {
 private:
  unsigned int N;
  std::vector<Pair> tour;
  double cost;

  static void connect(std::vector<Pair>&, const std::vector<vertex>&,
                      unsigned int);
  static void disconnect(std::vector<Pair>&, const std::vector<vertex>&,
                         unsigned int);

 public:
  Tour(unsigned int, const std::vector<Pair>& = std::vector<Pair>(),
       const double = 0.0);

  static std::pair<bool, Tour> generate(const Tour&, const std::vector<vertex>&,
                                        const double);

  std::vector<vertex> around(const vertex&, const double*) const;

  std::set<Pair> edgeSet() const;

  bool operator==(const Tour&) const;
  bool operator<(const Tour&) const;
  std::string toString() const;
  std::vector<vertex> toVector() const;
  double getObjVal() const;
};

#endif /* TOUR_H */