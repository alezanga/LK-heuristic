#include <string>

struct cpxenv;
struct cpxlp;
typedef cpxenv* Env;
typedef cpxlp* Prob;
struct TSPsolution;

struct TSPmodel {
 public:
  TSPmodel(int, double*);
  ~TSPmodel();
  void solve() const;
  const TSPsolution getSolution() const;

 private:
  Env env;
  Prob lp;
  int n_var, N;
  double* C;
  std::string* nameMap;
  void setupLP() const;
};