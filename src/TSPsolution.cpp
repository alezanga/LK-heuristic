#include "TSPsolution.hpp"

using std::ostream;
using std::string;
using std::vector;

TSPsolution::TSPsolution(const double obj, const unsigned int N,
                         const vector<double>& varmap,
                         const vector<string>& namemap, const string& stour,
                         const vector<vertex>& vetour)
    : varVals(varmap),
      nameMap(namemap),
      objVal(obj),
      N(N),
      stour(stour),
      vtour(vetour) {}
ostream& operator<<(ostream& out, const TSPsolution& sol) {
  out << "*************************************\n";
  out << "Size: " << sol.N << "\nObj. value: " << sol.objVal << "\n";
  if (!sol.stour.empty()) out << "Tour: " << sol.stour << "\n";
  // if (sol.varVals && sol.nameMap) {
  //   for (int i = 0; i < sol.asize; ++i)
  //     out << sol.nameMap[i] << " : " << sol.varVals[i] << "\n";
  // }
  return out << std::endl;
}