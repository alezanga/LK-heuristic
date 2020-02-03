#ifndef DATATYPES_H
#define DATATYPES_H

#include <functional>

typedef unsigned int vertex;

struct Pair {
  vertex u, v;
  Pair(const vertex& = -1, const vertex& = -1);
  bool operator==(const Pair&) const;

  struct Hash {
    std::size_t operator()(const Pair&) const;
  };
};

#endif /* DATATYPES_H */