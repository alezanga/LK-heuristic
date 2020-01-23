#ifndef PARAMS_H
#define PARAMS_H

struct Params {
  // TODO: add description
  const bool print_console = true;
  const bool load_csv = true;
  const bool save_csv = false;
  const unsigned int N = 10;
  const unsigned int N_incr = 10;
  const unsigned int max_iter = 1000;

  // Params for LK
  const unsigned int K = 50;
  unsigned int max_neighbours = 0;
  const unsigned int intens_min_depth = 4;
  const unsigned int intens_min_sols = 7;
  const unsigned int LK_iterations = 1;
};

#endif /* PARAMS_H */