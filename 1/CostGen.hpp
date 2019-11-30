#ifndef COST_GEN
#define COST_GEN

#include <unordered_set>

struct hash_pair {
  template <class T, class U>
  std::size_t operator()(const std::pair<T, U>& p) const {
    return std::hash<T>()(p.first) ^ std::hash<U>()(p.second);
  }
};

class CostGen {
  int X, Y;
  std::unordered_set<std::pair<int, int>, hash_pair> vertices;
  void adjustSize(const int);

 public:
  CostGen();
  double* generateCosts(const int);
};

#endif /*COST_GEN*/