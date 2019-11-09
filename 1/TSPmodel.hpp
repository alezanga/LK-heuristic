#include <string>
#include <vector>
// #include "cpxmacro.hpp"

struct cpxenv;
struct cpxlp;
typedef const cpxenv* CEnv;
typedef cpxenv* Env;
typedef cpxlp* Prob;

struct TSPmodel {
  TSPmodel(int, double*);
  ~TSPmodel();
  void solve();
  void printSolution();

 private:
  Env env;
  Prob lp;
  int n_var;
  int N;
  double* C;
  void setupProblem();
  void setupLP(CEnv, Prob, double*, std::vector<std::string>&);
};