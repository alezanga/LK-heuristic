#include "Pair.hpp"

Pair::Pair(const vertex& a, const vertex& b) : u(a), v(b) {}

bool Pair::operator==(const Pair& o) const {
  return (v == o.v && u == o.u) || (v == o.u && u == o.v);
}

std::size_t Pair::Hash::operator()(const Pair& e) const {
  return std::hash<vertex>()(e.u) ^ std::hash<vertex>()(e.v);
}