#include "TSPsolution.hpp"

using std::ostream;
using std::string;

TSPsolution::TSPsolution(const double obj, const int N, const double* v,
                         const string* n, const string& stour)
    : varVals(v),
      nameMap(n),
      objVal(obj),
      N(N),
      asize((N - 1) * (2 * N - 1)),
      stour(stour) {}

TSPsolution::~TSPsolution() {
  if (varVals) delete[] varVals;
  if (nameMap) delete[] nameMap;
}

ostream& operator<<(ostream& out, const TSPsolution& sol) {
  out << "*************************************\n";
  out << "Size: " << sol.N << "\nObj. value: " << sol.objVal << "\n";
  if (!sol.stour.empty()) out << "Tour: " << sol.stour << "\n";
  if (sol.varVals && sol.nameMap) {
    for (int i = 0; i < sol.asize; ++i)
      out << sol.nameMap[i] << " : " << sol.varVals[i] << "\n";
  }
  return out << "\n" << std::endl;
}