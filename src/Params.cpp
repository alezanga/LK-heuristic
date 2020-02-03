#ifndef PARAMS_H
#define PARAMS_H

struct Params {
  // TODO: add description
  const bool print_console = true;
  const bool load_csv = true;
  const bool save_csv = false;
  const unsigned int N = 180;
  const unsigned int N_incr = 10;
  const unsigned int max_iter = 30;

  // Params for LK

  // Max lenght of alternating path to search for
  const unsigned int K = 30;
  // Max number of improving iterations (restarts)
  const unsigned int I = 1000;
  // Max number of alternatives to look for when considering edges to add (0
  // means unbounded)
  unsigned int max_neighbours = 5;
  // Min lenght of the path to start intensification
  const unsigned int intens_min_depth = 40;
  // Number of solutions to consider for intensification
  const unsigned int intens_min_sols = 1000;
  // Number of random restarts
  const unsigned int LK_iterations = 10;
};

#endif /* PARAMS_H */