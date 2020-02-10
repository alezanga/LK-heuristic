#ifndef TSP_SOLUTION
#define TSP_SOLUTION

#include <iostream>
#include <string>
#include <vector>

typedef unsigned int vertex;

struct TSPsolution {
  friend std::ostream& operator<<(std::ostream&, const TSPsolution&);
  TSPsolution(const double = 0.0, const unsigned int = 0,
              const std::vector<double>& = std::vector<double>(),
              const std::vector<std::string>& = std::vector<std::string>(),
              const std::string& = "",
              const std::vector<vertex>& = std::vector<vertex>(), int = 0);

  std::vector<double> varVals;
  std::vector<std::string> nameMap;
  double objVal;
  unsigned int N;
  std::string stour;
  std::vector<vertex> vtour;
  int improving_iterations_lk;
};

#endif /*TSP_SOLUTION*/