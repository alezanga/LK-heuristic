#ifndef DATATYPES_H
#define DATATYPES_H

#include <functional>

typedef unsigned int vertex;

struct Node {
  vertex u, v;
  Node(const vertex& = -1, const vertex& = -1);
  bool operator==(const Node&) const;
};

struct Edge {
  vertex a, b;
  Edge(const vertex&, const vertex&);
  bool operator==(const Edge&) const;

  struct Hash {
    std::size_t operator()(const Edge&) const;
  };
};

#endif /* DATATYPES_H */