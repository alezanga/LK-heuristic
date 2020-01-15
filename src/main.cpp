#include <chrono>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "CostGen.hpp"
#include "LK.hpp"
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

void testTimes(const bool print_console, const bool load_csv, unsigned int N,
               const unsigned int N_incr, const int max_iter) {
  // Create new dir. Does nothing if it's already there.
  fs::create_directory("files");
  fs::path logd = fs::current_path() / "files";
  std::ofstream filesol((logd / "solutions.txt").string(), std::ofstream::out);
  std::ofstream fileres((logd / "results.txt").string(), std::ofstream::out);

  CostGen gencost;

  fs::create_directory("instances");
  fs::path insd = fs::current_path() / "instances";

  try {
    // Declare tables to hold results
    VariadicTable<int, double> resultsTable(
        {"Problem size (N)", "Time to solve (s)"});
    VariadicTable<string, string> rangesTable(
        {"Solving time interval (s)", "Problem size (N)"});
    // Declare a vector to keep track of problem size and time to solve
    vector<string> rangeSize(rangeThreshold.size() + 1, "");
    fs::directory_iterator dit = fs::directory_iterator(insd);
    for (int e = 0; e < max_iter && (!load_csv || dit != fs::end(dit)); ++e) {
      // Create/load cost matrix
      double* costs = nullptr;
      if (load_csv) {
        pair<unsigned int, double*> instance =
            gencost.loadFromCsv(dit->path().string());
        N = instance.first;
        costs = instance.second;
      } else {
        costs = gencost.generateCosts(N);
      }

      // LK model
      LK lk(N, costs, LK::initializeTour(N, costs));
      // Solve problem and measure time
      auto start = std::chrono::system_clock::now();
      lk.solve();
      auto end = std::chrono::system_clock::now();
      double elapsed_seconds =
          std::chrono::duration<double>(end - start).count();
      cout << "Solved in " << elapsed_seconds << "\n";
      cout << lk.getSolution();

      // Create CPLEX TSP problem with N holes
      TSPmodel mod = TSPmodel(N, costs, fs::path::preferred_separator);
      // Solve problem and measure time
      start = std::chrono::system_clock::now();
      mod.solve();
      end = std::chrono::system_clock::now();
      elapsed_seconds = std::chrono::duration<double>(end - start).count();
      // Log solution to file and add element to display tables
      filesol << mod.getSolution();
      resultsTable.addRow({N, elapsed_seconds});
      rangeSize[timeRange(elapsed_seconds)] += string(" ") += std::to_string(N);

      // Increment
      if (load_csv)
        ++dit;
      else
        N += N_incr;
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
    if (print_console) {
      cout << "\n--- SUMMARY OF RESULTS ---\n";
      resultsTable.print(cout);
      rangesTable.print(cout);
    }
  } catch (std::exception& e) {
    std::cout << ">>> EXCEPTION: " << e.what() << std::endl;
  }
  filesol.close();
  fileres.close();
}

int main() {
  // TODO: add description
  const bool load_csv_instances = false;
  const unsigned int N_init = 10;
  const unsigned int N_incr = 5;
  const int max_iter = 10;
  const bool print_console = true;

  testTimes(print_console, load_csv_instances, N_init, N_incr, max_iter);
  return 0;
}
