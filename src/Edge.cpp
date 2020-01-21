#include "Edge.hpp"

Edge::Edge(const vertex& x, const vertex& y) : a(x), b(y) {}

bool Edge::operator==(const Edge& e) const {
  return (a == e.a && b == e.b) || (b == e.a && a == e.b);
}

std::size_t Edge::Hash::operator()(const Edge& e) const {
  return std::hash<vertex>()(e.a) ^ std::hash<vertex>()(e.b);
}