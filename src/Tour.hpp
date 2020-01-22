#ifndef TOUR_H
#define TOUR_H

#include <string>
#include <unordered_set>
#include <vector>

#include "datatypes.hpp"

typedef unsigned int vertex;

class Tour {
 private:
  unsigned int N;
  std::vector<Node> tour;
  double cost;

  static void connect(std::vector<Node>&, const std::vector<vertex>&,
                      unsigned int);
  static void disconnect(std::vector<Node>&, const std::vector<vertex>&,
                         unsigned int);

 public:
  // class Iterator {
  //   std::vector<Node>::iterator ptr;

  //  public:
  //   Iterator(std::vector<Node>::iterator);
  //   Iterator operator++();
  //   bool operator!=(const Iterator&) const;
  //   const Node& operator*() const;
  // };

  // Iterator begin();
  // Iterator end();

  Tour(unsigned int, const std::vector<Node>& = std::vector<Node>(),
       const double = 0.0);

  std::pair<bool, std::vector<Node>> generate(const std::vector<vertex>&) const;

  void update(const std::vector<Node>, const double);

  std::vector<vertex> around(const vertex&) const;

  std::unordered_set<Edge, Edge::Hash>* edgeSet() const;

  std::string toString() const;
  std::vector<vertex> toVector() const;
  double getObjVal() const;
};

#endif /* TOUR_H */