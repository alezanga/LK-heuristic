#include "Pair.hpp"

using std::pair;

Pair::Pair(const vertex& a, const vertex& b) {
  if (a < b)
    p = pair<vertex, vertex>(a, b);
  else
    p = pair<vertex, vertex>(b, a);
}

const vertex& Pair::u() const { return p.first; }
const vertex& Pair::v() const { return p.second; }

bool Pair::operator==(const Pair& o) const { return p == o.p; }

bool Pair::operator<(const Pair& o) const { return p < o.p; }