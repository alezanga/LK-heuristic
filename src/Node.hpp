#ifndef NODE_H
#define NODE_H

typedef unsigned int vertex;

struct Node {
  Node(vertex a = -1, vertex b = -1);
  vertex u, v;
  bool operator==(const Node&) const;
};

#endif /* NODE_H */