#include <cmath>
#include <experimental/filesystem>
#include <vector>

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

using std::vector;
namespace fs = std::experimental::filesystem;

vector<unsigned int> Ks = {100};
vector<unsigned int> MNs{2, 5};
vector<unsigned int> BTs{2, 3, 4};
vector<unsigned int> IMDs{5, 8, 10};
vector<unsigned int> INTs{40, 50, 60};

void calibrateLK(Params P) {
  unsigned int N = 90;
  P.LK_iterations = 1;

  TSPinstance inst;
  inst.loadFromCsv("instances/tsp_90.csv");
  double* costsM = inst.costMatrix();

  fs::path logd = fs::current_path() / "files";
  std::ofstream cal_log((logd / "calibration_out.txt").string(),
                        std::ofstream::out);

  double opt_val = 701.413;

  VariadicTable<uint, uint, uint, uint, uint, double, double, double, int,
                double, double>
      table({"Max K", "Max neigh.", "Backtr. depth", "Intens. depth",
             "Intens. tours", "Avg error (%)", "Best error (%)",
             "Worst error (%)", "Avg impr. iterat.", "Tot time", "Score"});
  double best_score = 0.0;
  for (auto k : Ks) {
    P.K = k;
    for (auto max_neigh : MNs) {
      P.max_neighbours = max_neigh;
      for (auto back_depth : BTs) {
        P.backtracking_threshold = back_depth;
        for (auto int_min_depth : IMDs) {
          P.intens_min_depth = int_min_depth;
          for (auto int_n_tour : INTs) {
            P.intens_n_tours = int_n_tour;
            double avg_error = 0.0;
            double best_error = std::numeric_limits<double>::infinity();
            double worst_error = std::numeric_limits<double>::lowest();
            double tot_time = 0.0;
            int improving_it = 0;
            int iterations = 10;
            for (int iter = 0; iter < iterations; ++iter) {
              auto [sol, time] = utils::runILK(P, N, costsM, nullStream);
              double error = abs(sol.objVal - opt_val) / opt_val;
              if (error < best_error) best_error = error;
              if (error > worst_error) worst_error = error;
              improving_it += sol.improving_iterations_lk;
              avg_error += error;
              tot_time += time;
            }
            avg_error = avg_error / iterations * 100;
            double score = 1.0 / ((tot_time * avg_error) + 1.0);
            if (score > best_score) best_score = score;
            table.addRow({k, max_neigh, back_depth, int_min_depth, int_n_tour,
                          avg_error, best_error * 100, worst_error * 100,
                          improving_it / iterations, tot_time, score});
          }
        }
      }
    }
  }
  cal_log << "Best score: " << best_score << "\n";
  delete[] costsM;
  table.print(cal_log);
  cal_log.close();
}

int main() {
  // Read config file
  YamlP* parser = new YamlP;
  parser->readConfig("src/config.yml");
  Params P = parser->config();
  delete parser;

  calibrateLK(P);
  return 0;
}
