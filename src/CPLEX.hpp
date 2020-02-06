#ifndef TSP_MODEL
#define TSP_MODEL

#include <string>

struct cpxenv;
struct cpxlp;
typedef cpxenv* Env;
typedef cpxlp* Prob;
struct TSPsolution;

struct CplexModel {
 public:
  CplexModel(int, double*, const char = '/');
  ~CplexModel();
  void solve() const;
  const TSPsolution getSolution() const;

 private:
  Env env;
  Prob lp;
  int n_var, N;
  double* C;
  std::vector<std::string> nameMap;
  void setupLP(const char);
};

#endif /*TSP_MODEL*/