#ifndef PARAMS_H
#define PARAMS_H

struct Params {
  // TODO: add description
  const bool print_console = true;
  const bool load_csv = true;
  const bool save_csv = false;
  const unsigned int N = 50;
  const unsigned int N_incr = 5;
  const unsigned int max_iter = 10;

  // Params for LK
  unsigned int max_neighbours = 10;
  const unsigned int intens_min_depth = 4;
  const unsigned int intens_min_sols = 7;
  const unsigned int LK_iterations = 5;
};

#endif /* PARAMS_H */