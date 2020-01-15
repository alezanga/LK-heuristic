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
  unsigned int X, Y;
  std::unordered_set<std::pair<unsigned int, unsigned int>, hash_pair> vertices;
  void adjustSize(const unsigned int);

 public:
  CostGen();
  double* generateCosts(const unsigned int);
  void saveToCsv(double*, unsigned int N, const std::string&);
  std::pair<unsigned int, double*> loadFromCsv(const std::string&);
};

#endif /*COST_GEN*/