#include <cmath>
#include <experimental/filesystem>
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

void test(const Params& P) {
  TSPinstance data;
  // For each size to test
  for (unsigned int size : P.size_to_test) {
    for (unsigned int inst = 0; inst < P.instances_per_size; ++inst) {
      data.generateRandomPolygons(size);
      double* C = data.costMatrix();
      for (unsigned int run = 0; run < P.runs_per_instances; ++run) {
        auto [solh, timeh] = utils::runILK(P, size, C, nullStream);
        auto [solo, timeo] = utils::runOptimal(P, size, C, nullStream);
        // TODO
      }
      delete[] C;
    }
  }
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