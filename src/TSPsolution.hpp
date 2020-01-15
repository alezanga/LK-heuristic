#ifndef TSP_SOLUTION
#define TSP_SOLUTION

#include <iostream>
#include <string>

struct TSPsolution {
  friend std::ostream& operator<<(std::ostream&, const TSPsolution&);
  TSPsolution(const double, const int, const double* = nullptr,
              const std::string* = nullptr, const std::string& = "");
  ~TSPsolution();

  const double* varVals;
  const std::string* nameMap;
  const double objVal;
  const int N;
  const int asize;
  const std::string stour;
};

#endif /*TSP_SOLUTION*/