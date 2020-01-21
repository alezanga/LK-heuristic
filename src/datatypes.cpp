#include "datatypes.hpp"

Node::Node(const vertex& a, const vertex& b) : u(a), v(b) {}

bool Node::operator==(const Node& o) const {
  return (v == o.v && u == o.u) || (v == o.u && u == o.v);
}

Edge::Edge(const vertex& x, const vertex& y) : a(x), b(y) {}

bool Edge::operator==(const Edge& e) const {
  return (a == e.a && b == e.b) || (b == e.a && a == e.b);
}

std::size_t Edge::Hash::operator()(const Edge& e) const {
  return std::hash<vertex>()(e.a) ^ std::hash<vertex>()(e.b);
}