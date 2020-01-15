#include "Node.hpp"

Node::Node(vertex a, vertex b) : u(a), v(b) {}

bool Node::operator==(const Node& o) const {
  return (v == o.v && u == o.u) || (v == o.u && u == o.v);
}