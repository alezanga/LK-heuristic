#include <cmath>
#include <experimental/filesystem>
#include <fstream>
#include <string>
#include <tuple>
#include <vector>

#include "CPLEX.hpp"
#include "TSPinstance.hpp"
#include "TSPsolution.hpp"
#include "utilities.hpp"
#include "utils/params.hpp"
#include "utils/variadic_table.hpp"
#include "utils/yaml_parser.hpp"

class NullBuffer : public std::streambuf {
 public:
  int overflow(int c) { return c; }
};

NullBuffer _nullBuff = NullBuffer();
std::ostream nullStream(&_nullBuff);

using std::ofstream;
using std::pair;
using std::string;
using std::tuple;
using std::vector;
namespace fs = std::experimental::filesystem;

/**
 * Compute mean, std deviation, min e max values for each row
 */
tuple<vector<double>, vector<double>, vector<double>, vector<double>>
statistics(const vector<vector<double>>& data, uint N) {
  // Compute mean, min, max
  vector<double> mean;
  vector<double> min;
  vector<double> max;
  for (const vector<double>& col : data)
    // time_res is the times for a single instance
    if (mean.empty())
      mean = min = max = col;
    else
      for (uint i = 0; i < col.size(); ++i) {
        mean[i] += col[i];
        min[i] = (min[i] > col[i]) ? col[i] : min[i];
        max[i] = (max[i] < col[i]) ? col[i] : max[i];
      }
  for (double& time : mean) time /= N;

  // Compute std. deviation
  vector<double> stdev(N, 0);
  for (const vector<double>& col : data)
    for (uint i = 0; i < col.size(); ++i) stdev[i] += pow(col[i] - mean[i], 2);
  for (double& psum : stdev) psum = sqrt(psum / N);

  return {mean, stdev, min, max};
}

void test(Params& P) {
  // Set up output folder and file
  fs::create_directory("files");
  fs::path logd = fs::current_path() / "files";
  ofstream fileres((logd / "results.txt").string(), ofstream::out);
  ofstream log((logd / "test_log.txt").string(), ofstream::out);

  VariadicTable<uint, double> plot_data({"Size", "Opt time (s)"});

  TSPinstance data;
  // For each size to test
  for (unsigned int size : P.size_to_test) {
    vector<vector<double>> time_size;
    vector<vector<double>> err_size;
    data.generateRandomPolygons(size);
    double* C = data.costMatrix();
    auto [solo, timeo] = utils::runOptimal(P, size, C, logd);
    plot_data.addRow({size, timeo});

    for (unsigned int inst = 0; inst < P.runs_per_instance; ++inst) {
      vector<double> time_run;
      vector<double> err_run;
      for (unsigned int restart : P.restarts) {
        P.LK_iterations = restart;
        auto [solh, timeh] = utils::runILK(P, size, C, log);
        double err = abs(solh.objVal - solo.objVal) / solo.objVal * 100;
        time_run.push_back(timeh);
        err_run.push_back(err);
      }
      time_size.push_back(std::move(time_run));
      err_size.push_back(std::move(err_run));
    }

    delete[] C;

    VariadicTable<uint, double, double, double, double> errors(
        {"LK restarts", "Mean error (%)", "Std. dev", "Min", "Max"});
    VariadicTable<uint, double, double, double, double> times(
        {"LK restarts", "Mean time (s)", "Std. dev", "Min", "Max"});
    // Compute mean, stdvar, min, max
    auto [emean, estd, emin, emax] = statistics(err_size, P.runs_per_instance);
    auto [tmean, tstd, tmin, tmax] = statistics(time_size, P.runs_per_instance);

    for (uint i = 0; i < P.restarts.size(); ++i) {
      errors.addRow({P.restarts[i], emean[i], estd[i], emin[i], emax[i]});
      times.addRow({P.restarts[i], tmean[i], tstd[i], tmin[i], tmax[i]});
    }

    errors.setColumnPrecision(vector<int>{1, 3, 3, 3, 3});
    times.setColumnPrecision(vector<int>{1, 3, 3, 3, 3});

    fileres << "******************************************\n"
            << "               Size " << size << "\n"
            << "******************************************\n";
    fileres << "Statistics on relative percentage error\n";
    errors.print(fileres);
    fileres << "\nStatistics on execution time\n";
    times.print(fileres);
    fileres << std::endl;
  }
  plot_data.setColumnPrecision(vector<int>{1, 3});
  fileres << "\nOPT times\n";
  plot_data.print(fileres);
  fileres.close();
}

int main() {
  // Read config file
  YamlP* parser = new YamlP;
  parser->readConfig("src/config.yml");
  Params P = parser->config();
  delete parser;

  test(P);
  return 0;
}