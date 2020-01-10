#include "TSPsolution.hpp"

using std::ostream;
using std::string;

TSPsolution::TSPsolution(const double* v, const string* n, const double obj,
                         const int N)
    : varVals(v), nameMap(n), objVal(obj), N(N), asize((N - 1) * (2 * N - 1)) {}

TSPsolution::~TSPsolution() {
  delete[] varVals;
  delete[] nameMap;
}

ostream& operator<<(ostream& out, const TSPsolution& sol) {
  out << "*************************************\n";
  out << "Size: " << sol.N << "\nObj. value: " << sol.objVal << "\n";
  for (int i = 0; i < sol.asize; ++i)
    out << sol.nameMap[i] << " : " << sol.varVals[i] << "\n";
  return out << "\n" << std::endl;
}