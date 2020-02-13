#include <algorithm>
#include <chrono>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "CPLEX.hpp"
#include "TSPinstance.hpp"
#include "TSPsolution.hpp"
#include "Tour.hpp"
#include "utilities.hpp"
#include "utils/params.hpp"
#include "utils/python_adapter.hpp"
#include "utils/variadic_table.hpp"
#include "utils/yaml_parser.hpp"

using std::cout;
using std::ofstream;
using std::pair;
using std::string;
using std::vector;
namespace fs = std::experimental::filesystem;

// Global vector with time threshold to visualize when finished
// vector<double> rangeThreshold = {0.1, 1, 10, 100};

/**
 * Returns int index corresponding to the range where 'time' belongs
 */
// unsigned int timeRange(double time) {
//   unsigned int x = 0;
//   while (x < rangeThreshold.size() && (time - rangeThreshold[x]) > 0) x++;
//   return x;
// }

/**
 * Create instances and write them to csv
 */
void generateInstances(const Params& P) {
  // Create dir and set path
  fs::create_directory("instances");
  fs::path insd = fs::current_path() / "instances";

  unsigned int size = P.N_min;
  TSPinstance coords;
  while (size <= P.N_max) {
    coords.generateRandomPolygons(size);
    coords.saveToCsv((insd / "tsp_").string() + std::to_string(size));
    size += P.N_incr;
  }
}

pair<TSPsolution, double> runHeuristic(const Params& P, double* costs,
                                       const TSPinstance& coords,
                                       unsigned int N, ofstream& log_lk,
                                       const PyWrapper& py) {
  cout << "Solving with heuristic..." << std::endl;
  // LK model
  pair<TSPsolution, double> lk_res = utils::runILK(P, N, costs, log_lk);
  log_lk << "##### BEST SOLUTION (size " << N << ") #####\n"
         << "Total time: " << lk_res.second << " sec\n"
         << lk_res.first;
  log_lk << "----------------------------------------------------\n"
         << std::endl;

  py.plot_points(*(coords.getPoints()), lk_res.first.vtour,
                 "path_" + std::to_string(N));

  return lk_res;
}

void testTimes(const Params& P) {
  // Create new dir. Does nothing if it's already there.
  fs::create_directory("files");
  fs::path logd = fs::current_path() / "files";
  std::ofstream sol_cplex((logd / "solCPLEX.txt").string(), std::ofstream::out);
  std::ofstream sol_lk((logd / "solLK.txt").string(), std::ofstream::out);
  std::ofstream fileres((logd / "results.txt").string(), std::ofstream::out);

  TSPinstance coords;

  fs::path insd = fs::current_path() / "instances";
  try {
    // Declare tables to hold results
    VariadicTable<int, double> resultsTable(
        {"Problem size (N)", "Time to solve (s)"});
    VariadicTable<string, string> rangesTable(
        {"Solving time interval (s)", "Problem size (N)"});
    // Declare a vector to keep track of problem size and time to solve
    // vector<string> rangeSize(rangeThreshold.size() + 1, "");
    fs::directory_iterator dit = fs::directory_iterator(insd);
    vector<pair<unsigned int, double>> cplex_times;
    vector<pair<unsigned int, double>> cplex_values;
    vector<pair<unsigned int, double>> heur_times;
    vector<pair<unsigned int, double>> heur_values;
    vector<pair<unsigned int, double>> nn_values;
    vector<pair<unsigned int, double>> nn_times;

    PyWrapper Py;

    pair<TSPsolution, double> solopt, solheur;
    unsigned int N = 0;

    for (auto& file : fs::directory_iterator(insd)) {
      string filename = file.path().filename().string();
      string stem = file.path().stem().string();
      if (std::find(P.instances_to_read.begin(), P.instances_to_read.end(),
                    stem) == P.instances_to_read.end())
        continue;
      // Load from file
      N = coords.loadFromCsv(file.path().string());
      double* cost = coords.costMatrix();
      cout << "Loaded file " << filename << std::endl;
      if (P.solve_heur) {
        solheur = runHeuristic(P, cost, coords, N, sol_lk, Py);
        heur_times.push_back({N, solheur.second});
        heur_values.push_back({N, solheur.first.objVal});
      }
      if (P.solve_cplex) {
        solopt = utils::runOptimal(P, N, cost, sol_cplex);
        cplex_times.push_back({N, solopt.second});
        cplex_values.push_back({N, solopt.first.objVal});
      }
      // Nearest neighbour
      auto start = std::chrono::system_clock::now();
      double nncost = utils::nearestNeighbour(cost, N);
      auto end = std::chrono::system_clock::now();
      double nn_time = std::chrono::duration<double>(end - start).count();
      nn_times.push_back({N, nn_time});
      nn_values.push_back({N, nncost});

      cout << std::endl;

      delete[] cost;
    }

    // Plot data with Python
    Py.plot_times(cplex_times, heur_times,
                  "Execution times for each problem size (N)", "times");
    Py.plot_errors(cplex_values, heur_values, nn_values,
                   "Objective values for problem size (N)", "values");
  }

  // resultsTable.addRow({N, elapsed_seconds});
  // rangeSize[timeRange(elapsed_seconds)] += string(" ") +=
  // std::to_string(N);

  // Prepare table with time ranges
  // for (unsigned int i = 0; i < rangeThreshold.size(); ++i) {
  //   string doubleString = std::to_string(rangeThreshold[i]);
  //   string col1 = doubleString.substr(0, doubleString.find(".") + 2);
  //   rangesTable.addRow({"<= " + col1, rangeSize[i]});  // \u2264
  // }
  // string lastString = std::to_string(rangeThreshold.back());
  // string col1 = lastString.substr(0, lastString.find(".") + 2);
  // rangesTable.addRow({">  " + col1, rangeSize.back()});

  // // Print to file
  // fileres << "--- SUMMARY OF RESULTS ---\n\n";
  // fileres << "Execution times:\n";
  // resultsTable.print(fileres);
  // fileres << "\nDistribution of problems in each time range:\n";
  // rangesTable.print(fileres);
  // // Print to console
  // if (P.print_console) {
  //   cout << "\n--- SUMMARY OF RESULTS ---\n";
  //   resultsTable.print(cout);
  //   rangesTable.print(cout);
  // }
  catch (std::exception& e) {
    std::cout << ">>> EXCEPTION: " << e.what() << std::endl;
  }
  sol_cplex.close();
  sol_lk.close();
  fileres.close();
}

int main() {
  // Read config file
  YamlP* parser = new YamlP;
  parser->readConfig("src/config.yml");
  Params P = parser->config();
  delete parser;

  if (P.generate_instances) generateInstances(P);
  if (P.solve_cplex || P.solve_heur) testTimes(P);
  return 0;
}
