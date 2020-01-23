#ifndef TSP_SOLUTION
#define TSP_SOLUTION

#include <iostream>
#include <string>
#include <vector>

typedef unsigned int vertex;

struct TSPsolution {
  friend std::ostream& operator<<(std::ostream&, const TSPsolution&);
  TSPsolution(const double, const unsigned int, double* = nullptr,
              std::string* = nullptr, const std::string& = "",
              const std::vector<vertex>& = std::vector<vertex>());
  TSPsolution& operator=(const TSPsolution&);
  ~TSPsolution();

  double* varVals;
  std::string* nameMap;
  double objVal;
  int N;
  int asize;
  std::string stour;
  std::vector<vertex> vtour;
};

#endif /*TSP_SOLUTION*/