#ifndef TOUR
#define TOUR

#include <string>
#include <vector>

#include "Node.hpp"

typedef unsigned int vertex;

class Tour {
 private:
  unsigned int N;
  std::vector<Node> tour;
  double cost;

  static void connect(std::vector<Node>&, const std::vector<vertex>&,
                      unsigned int, std::vector<int>&);
  static void disconnect(std::vector<Node>&, const std::vector<vertex>&,
                         unsigned int, std::vector<int>&);

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

  Tour(unsigned int, std::vector<Node> = std::vector<Node>(),
       const double* = nullptr);

  std::pair<bool, std::vector<Node>> generate(const std::vector<vertex>&) const;

  void update(const std::vector<Node>, const double);

  std::vector<vertex> around(const vertex&) const;

  std::string toString() const;
  double getObjVal() const;
};

#endif /* TOUR */