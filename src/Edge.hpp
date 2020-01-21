#ifndef EDGE_H
#define EDGE_H

#include <functional>

typedef unsigned int vertex;

struct Edge {
  vertex a, b;
  Edge(const vertex&, const vertex&);
  bool operator==(const Edge&) const;

  struct Hash {
    std::size_t operator()(const Edge&) const;
  };
};

#endif /* EDGE_H */