#include <algorithm>
#include <chrono>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "CostGen.hpp"
#include "IteratedLK.hpp"
#include "Params.cpp"
#include "TSPmodel.hpp"
#include "TSPsolution.hpp"
#include "Tour.hpp"
#include "VariadicTable.hpp"

using std::cout;
using std::pair;
using std::string;
using std::vector;
namespace fs = std::experimental::filesystem;

// Global vector with time threshold to visualize when finished
vector<double> rangeThreshold = {0.1, 1, 10, 100};

/**
 * Returns int index corresponding to the range where 'time' belongs
 */
unsigned int timeRange(double time) {
  unsigned int x = 0;
  while (x < rangeThreshold.size() && (time - rangeThreshold[x]) > 0) x++;
  return x;
}

void testTimes(const Params& P) {
  // Create new dir. Does nothing if it's already there.
  fs::create_directory("files");
  fs::path logd = fs::current_path() / "files";
  std::ofstream sol_cplex((logd / "solCPLEX.txt").string(), std::ofstream::out);
  std::ofstream sol_lk((logd / "solLK.txt").string(), std::ofstream::out);
  std::ofstream fileres((logd / "results.txt").string(), std::ofstream::out);

  const std::vector<string> fileToRead{"tsp60.csv"};

  CostGen gencost;

  fs::create_directory("instances");
  fs::path insd = fs::current_path() / "instances";
  unsigned int N = P.N;
  try {
    // Declare tables to hold results
    VariadicTable<int, double> resultsTable(
        {"Problem size (N)", "Time to solve (s)"});
    VariadicTable<string, string> rangesTable(
        {"Solving time interval (s)", "Problem size (N)"});
    // Declare a vector to keep track of problem size and time to solve
    vector<string> rangeSize(rangeThreshold.size() + 1, "");
    fs::directory_iterator dit = fs::directory_iterator(insd);
    for (uint e = 0; e < P.max_iter && (!P.load_csv || dit != fs::end(dit));
         ++e, ++dit) {
      // Create/load cost matrix
      double* costs = nullptr;
      std::chrono::_V2::system_clock::time_point start, end;
      double elapsed_seconds;
      if (P.load_csv) {
        string filename = dit->path().filename().string();
        if (std::find(fileToRead.begin(), fileToRead.end(), filename) ==
            fileToRead.end())
          continue;
        pair<unsigned int, double*> instance =
            gencost.loadFromCsv(dit->path().string());
        N = instance.first;
        costs = instance.second;
      } else {
        costs = gencost.generateCosts(N);
        if (P.save_csv) gencost.saveToCsv(costs, N, (insd / "tsp_").string());
      }

      // LK model
      pair<TSPsolution, double> lk_res = iterated_LK(P, N, costs, sol_lk);
      sol_lk << "##### BEST SOLUTION (size " << N << ") #####\n"
             << "Total time: " << lk_res.second << "\n"
             << lk_res.first << std::endl;

      // Create CPLEX TSP problem with N holes
      TSPmodel mod = TSPmodel(N, costs, fs::path::preferred_separator);
      // Solve problem and measure time
      start = std::chrono::system_clock::now();
      mod.solve();
      end = std::chrono::system_clock::now();
      elapsed_seconds = std::chrono::duration<double>(end - start).count();
      sol_cplex << "##### OPTIMAL SOLUTION (size " << N << ") #####\n";
      // Log solution to file and add element to display tables
      sol_cplex << mod.getSolution();
      resultsTable.addRow({N, elapsed_seconds});
      rangeSize[timeRange(elapsed_seconds)] += string(" ") += std::to_string(N);

      // Increment
      if (!P.load_csv) N += P.N_incr;
      // Delete costs matrix
      delete[] costs;
    }

    // Prepare table with time ranges
    for (unsigned int i = 0; i < rangeThreshold.size(); ++i) {
      string doubleString = std::to_string(rangeThreshold[i]);
      string col1 = doubleString.substr(0, doubleString.find(".") + 2);
      rangesTable.addRow({"<= " + col1, rangeSize[i]});  // \u2264
    }
    string lastString = std::to_string(rangeThreshold.back());
    string col1 = lastString.substr(0, lastString.find(".") + 2);
    rangesTable.addRow({">  " + col1, rangeSize.back()});

    // Print to file
    fileres << "--- SUMMARY OF RESULTS ---\n\n";
    fileres << "Execution times:\n";
    resultsTable.print(fileres);
    fileres << "\nDistribution of problems in each time range:\n";
    rangesTable.print(fileres);
    // Print to console
    if (P.print_console) {
      cout << "\n--- SUMMARY OF RESULTS ---\n";
      resultsTable.print(cout);
      rangesTable.print(cout);
    }
  } catch (std::exception& e) {
    std::cout << ">>> EXCEPTION: " << e.what() << std::endl;
  }
  sol_cplex.close();
  sol_lk.close();
  fileres.close();
}

int main() {
  testTimes(Params());
  return 0;
}
