#include <chrono>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "TSPmodel.hpp"
#include "TSPsolution.hpp"
#include "VariadicTable.hpp"
#include "costgen.hpp"

using std::cout;
using std::string;
using std::vector;
namespace fs = std::experimental::filesystem;

void testTimes() {
  const int step = 10;
  int N = 50;
  // Create new dir. Does nothing if it's already there.
  fs::create_directory("files");
  fs::path logd = fs::current_path() / "files";
  std::string logpath = (logd / "solutions.txt").string();
  std::ofstream file(logpath, std::ofstream::out);
  try {
    VariadicTable<int, double> results(
        {"Problem size (N)", "Time to solve (s)"});
    int executions = 0;
    while (executions < 1) {
      // Create TSP problem with N holes
      TSPmodel mod = TSPmodel(N, generateCosts(N));
      auto start = std::chrono::system_clock::now();
      mod.solve();
      auto end = std::chrono::system_clock::now();
      double elapsed_seconds =
          std::chrono::duration<double>(end - start).count();
      const TSPsolution sol = mod.getSolution();
      file << sol;
      results.addRow({N, elapsed_seconds});
      N += step;
      executions++;
    }
    cout << "Tabella dei risultati:\n";
    results.print(cout);
  } catch (std::exception& e) {
    std::cout << ">>> EXCEPTION: " << e.what() << std::endl;
  }
  file.close();
}

int main() {
  testTimes();
  return 0;
}
