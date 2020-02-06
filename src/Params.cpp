#ifndef PARAMS_H
#define PARAMS_H

enum mode_type { load, fast };

struct Params {
  // TODO: add description
  const bool print_console = false;
  const bool generate_instances = false;
  const mode_type mode = load;
  const bool solve_cplex = false;
  const bool solve_heur = true;
  const unsigned int N_min = 10;
  const unsigned int N_incr = 5;
  const unsigned int N_max = 105;

  // Params for LK

  // Max lenght of alternating path to search for
  const unsigned int K = 100;
  // Max number of improving iterations (restarts)
  const unsigned int I = 1000;
  // Max number of alternatives to look for when considering edges to add (0
  // means unbounded)
  unsigned int max_neighbours = 7;
  // Min lenght of the path to start intensification
  const unsigned int intens_min_depth = 4;
  // Number of tours to consider for intensification
  const unsigned int intens_n_tours = 20;
  // Number of random restarts
  const unsigned int LK_iterations = 10;
};

#endif /* PARAMS_H */