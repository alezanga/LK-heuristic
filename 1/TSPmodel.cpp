#include "TSPmodel.hpp"
#include <iostream>
#include "cpxmacro.hpp"

using std::string;
using std::vector;

// error status and messagge buffer
int status;
char errmsg[BUF_SIZE];

const int N = 4;
const int NAME_SIZE = 512;
char name[NAME_SIZE];

// clang-format off
// const double C[N * N] = {0.0, 3.0, 2.0, 3.0, 
//                          2.0, 0.0, 1.0, 1.5,
//                          2.0, 1.0, 0.0, 2.5, 
//                          3.0, 4.0, 3.0, 0.0};
// clang-format on

TSPmodel::TSPmodel(int N, double* C) : N(N), C(C) { setupProblem(); }

TSPmodel::~TSPmodel() {
  delete[] C;
  CPXfreeprob(env, &lp);
  CPXcloseCPLEX(&env);
}

void TSPmodel::setupLP(CEnv env, Prob lp, double* C, vector<string>& nameMap) {
  // * ---- ADD VARIABLES ----

  vector<vector<int>> xMap;
  xMap.resize(N);
  for (int i = 0; i < N; ++i) xMap[i].resize(N, -1);

  // x_ij
  const int x_start = CPXgetnumcols(env, lp);

  int x_position = x_start;
  for (int i = 0; i < N; ++i)
    for (int j = 1; j < N; ++j) {
      if (i == j) continue;
      char xtype = 'C';
      double lb = 0.0;  // greater than 0
      double ub = N;
      double obj = 0.0;  // not present in obj. function
      snprintf(name, NAME_SIZE, "x_%i_%i", i, j);
      char* xname = static_cast<char*>(name);
      // printf("%c %c %c\n", nameN[i], nameN[j], *xname);
      CHECKED_CPX_CALL(CPXnewcols, env, lp, 1, &obj, &lb, &ub, &xtype, &xname);

      nameMap[x_position] = name;
      xMap[i][j] = x_position++;
    }

  vector<vector<int>> yMap;
  yMap.resize(N);
  for (int i = 0; i < N; ++i) yMap[i].resize(N, -1);

  // y_ij
  const int y_start = CPXgetnumcols(env, lp);

  int y_position = y_start;
  for (int i = 0; i < N; ++i)
    for (int j = 0; j < N; ++j) {
      // NOTE: j = 0 MUST be considered (for y).
      if (i == j) continue;
      char ytype = 'B';
      double lb = 0.0;  // greater than 0
      double ub = 1.0;
      double obj = (j == 0) ? 0 : C[i * N + j];  // TODO: check this.
      snprintf(name, NAME_SIZE, "y_%i_%i", i, j);
      char* yname = static_cast<char*>(name);
      CHECKED_CPX_CALL(CPXnewcols, env, lp, 1, &obj, &lb, &ub, &ytype, &yname);

      nameMap[y_position] = name;
      yMap[i][j] = y_position++;
    }

  // const int y_end = CPXgetnumcols(env, lp);
  // printf("%d", y_end);

  // * ---- ADD CONSTRAINTS ----
  // In-flow - out-flow = 1
  for (int k = 1; k < N; ++k) {
    const int sum1 = N - 1;
    const int sum2 = N - 2;
    int lside[sum1 + sum2];
    double coeff[sum1 + sum2];
    std::fill_n(coeff, sum1, 1.0);
    std::fill_n(coeff + sum1, sum2, -1.0);
    char sense = 'E';
    const double rside = 1.0;

    // sum_{i:(i,k) in A} x_ik
    int idx = 0;
    for (int i = 0; i < N; ++i) {
      if (i == k) continue;
      lside[idx++] = xMap[i][k];  // x_ik
    }

    // sum_{j:(k,j) j!=0} x_kj
    for (int j = 1; j < N; ++j) {
      if (j == k) continue;
      lside[idx++] = xMap[k][j];  // x_kj
    }

    int matbeg = 0;
    CHECKED_CPX_CALL(CPXaddrows, env, lp, 0, 1, sum1 + sum2, &rside, &sense,
                     &matbeg, lside, coeff, nullptr, nullptr);
  }

  // sum_{j:(i,j) in A, j!= 0} y_ij = 1 forall i
  for (int i = 0; i < N; ++i) {
    int lside[N - 1];
    double coeff[N - 1];
    std::fill_n(coeff, N - 1, 1.0);  // All coefficients are 1.0
    char sense = 'E';
    const double rside = 1.0;

    // Populate lside with: sum_{j:(i,j) in A} y_ij
    int idx = 0;
    // NOTE: j from 0
    for (int j = 0; j < N; ++j) {
      if (i == j) continue;
      lside[idx++] = yMap[i][j];
    }

    int matbeg = 0;
    CHECKED_CPX_CALL(CPXaddrows, env, lp, 0, 1, N - 1, &rside, &sense, &matbeg,
                     lside, coeff, nullptr, nullptr);
  }

  // sum_{i:(i,j) in A} y_ij = 1 forall j (!= 0)
  // NOTE: j from 0
  for (int j = 0; j < N; ++j) {
    int lside[N - 1];
    double coeff[N - 1];
    std::fill_n(coeff, N - 1, 1.0);  // All coefficients are 1.0
    char sense = 'E';
    const double rside = 1.0;

    // sum_{i:(i,j) in A} y_ij
    int idx = 0;
    for (int i = 0; i < N; ++i) {
      if (i == j) continue;
      lside[idx++] = yMap[i][j];
    }

    int matbeg = 0;
    CHECKED_CPX_CALL(CPXaddrows, env, lp, 0, 1, N - 1, &rside, &sense, &matbeg,
                     lside, coeff, nullptr, nullptr);
  }

  // x_ij <= (N-1)y_ij forall (i,j) in A, j != 0
  // x_ij - (N-1)y_ij <= 0
  for (int i = 0; i < N; ++i)
    for (int j = 1; j < N; ++j) {
      if (i == j) continue;
      int lside[2] = {xMap[i][j], yMap[i][j]};
      double coeff[2] = {1.0, -(static_cast<double>(N) - 1.0)};
      char sense = 'L';
      const double rside = 0.0;

      int matbeg = 0;
      CHECKED_CPX_CALL(CPXaddrows, env, lp, 0, 1, 2, &rside, &sense, &matbeg,
                       lside, coeff, nullptr, nullptr);
    }
  CHECKED_CPX_CALL(CPXwriteprob, env, lp, "Lab1TSP.lp", 0);
  // CHECKED_CPX_CALL(CPXwriteprob, env, lp, "Lab1TSP.sav", 0);
}

void TSPmodel::setupProblem() {
  DECL_ENV(env);
  DECL_PROB(env, lp);

  n_var = (N - 1) * (2 * N - 1);
  vector<string> nameMap(n_var);
  // setup LP
  setupLP(env, lp, C, nameMap);
}

void TSPmodel::solve() {
  try {
    // optimize
    CHECKED_CPX_CALL(CPXmipopt, env, lp);
  } catch (std::exception& e) {
    std::cout << ">>>EXCEPTION: " << e.what() << std::endl;
  }
}

void TSPmodel::printSolution() {
  // print
  double objval;
  CHECKED_CPX_CALL(CPXgetobjval, env, lp, &objval);
  std::cout << "Objval: " << objval << std::endl;
  int n = CPXgetnumcols(env, lp);
  if (n != n_var) {
    throw std::runtime_error(std::string(__FILE__) + ":" + STRINGIZE(__LINE__) +
                             ": " + "different number of variables");
  }
  std::vector<double> varVals;
  varVals.resize(n);
  CHECKED_CPX_CALL(CPXgetx, env, lp, &varVals[0], 0, n - 1);
  for (int i = 0; i < n; ++i) {
    // std::cout << nameMap[i] << " : " << varVals[i] << std::endl;
    // to read variables names the API function ``CPXgetcolname'' may be used
    // (it is rather tricky, see the API manual if you like...) status =
    // CPXgetcolname (env, lp, cur_colname, cur_colnamestore,  cur_storespace,
    // &surplus, 0, cur_numcols-1);
  }
  CHECKED_CPX_CALL(CPXsolwrite, env, lp, "Lab1TSP.sol");
}

// int main(int argc, char const* argv[]) {}