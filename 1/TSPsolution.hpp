#include <iostream>
#include <string>

struct TSPsolution {
  friend std::ostream& operator<<(std::ostream&, const TSPsolution&);
  TSPsolution(const double*, const std::string*, const double, const int);
  ~TSPsolution();

  const double* varVals;
  const std::string* nameMap;
  const double objVal;
  const int N;
  const int asize;
};