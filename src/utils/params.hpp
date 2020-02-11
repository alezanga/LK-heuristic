#ifndef PARAMS_H
#define PARAMS_H

class YamlP;

struct Params {
  friend class YamlP;

 private:
  Params() = default;

 public:
  bool print_console;
  bool generate_instances;
  bool solve_cplex;
  bool solve_heur;
  unsigned int N_min;
  unsigned int N_incr;
  unsigned int N_max;

  std::vector<std::string> instances_to_read;

  // Params for LK

  // Max lenght of alternating path to search for
  unsigned int K;
  // Max number of improving iterations (restarts)
  unsigned int I;
  // Max number of alternatives to look for when considering edges to add (0
  // means unbounded)
  unsigned int max_neighbours;
  unsigned int backtracking_threshold;
  // Min lenght of the path to start intensification
  unsigned int intens_min_depth;
  // Number of tours to consider for intensification
  unsigned int intens_n_tours;
  // Number of random restarts
  unsigned int LK_iterations;
};

#endif