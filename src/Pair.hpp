#ifndef PAIRTYPE_H
#define PAIRTYPE_H

#include <utility>

typedef unsigned int vertex;

struct Pair {
 private:
  std::pair<vertex, vertex> p;

 public:
  const vertex& u() const;
  const vertex& v() const;
  Pair(const vertex& = -1, const vertex& = -1);
  bool operator==(const Pair&) const;
  bool operator<(const Pair&) const;

  // struct Hash {
  //   std::size_t operator()(const Pair&) const;
  // };
};

#endif /* PAIRTYPE_H */