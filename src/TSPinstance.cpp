#include <ilcplex/cplex.h>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

#include "TSPinstance.hpp"

using std::pair;
using std::string;
using std::vector;

TSPinstance::TSPinstance() : points(nullptr) {}

TSPinstance::~TSPinstance() { delete points; }

const vector<pair<double, double>>* TSPinstance::getPoints() const {
  return points;
}

void TSPinstance::randomPolygon(unsigned int n_sides, unsigned int h,
                                unsigned int w) {
  unsigned int seed =
      std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine re(seed);
  std::uniform_real_distribution<double> randomXCoord(0, w);
  std::uniform_real_distribution<double> randomYCoord(0, h);
  std::uniform_real_distribution<double> randomRadius(N / 10.0, N / 6.0);
  double angle = 0;
  double angle_increment = 2.0 * M_PI / n_sides;
  double x_centre = randomXCoord(re);
  double y_centre = randomYCoord(re);
  double radius = randomRadius(re);
  for (unsigned int i = 0; i < n_sides; ++i) {
    double x = x_centre + radius * cos(angle);
    double y = y_centre + radius * sin(angle);
    points->push_back({x, y});
    angle += angle_increment;
  }
}

/**
 * Generate a random number of poligons using exactly N vertices
 * @param N number of vertices
 */
void TSPinstance::generateRandomPolygons(unsigned int N) {
  // Set current N
  this->N = N;

  // Clean previous vector and reallocate
  if (points) delete points;
  points = new vector<pair<double, double>>;

  unsigned int seed1 =
      std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine re(seed1);

  // Coordinate space (width * height)
  // std::uniform_int_distribution<unsigned int> randomSize(0, N);
  unsigned int width = N;
  unsigned int height = N;

  // Number of polygons to draw
  std::uniform_int_distribution<unsigned int> randomUInt(1, N / 10);
  unsigned n_polygons = randomUInt(re);

  // Reserve a minimum of 3 sides for each polygon
  vector<unsigned int> sides_polygons(n_polygons, 3);
  unsigned int remainder = N - n_polygons * 3;
  // Split N into n_polygons numbers such that their sum is N.
  std::uniform_int_distribution<unsigned int> randomIndex(0, n_polygons - 1);
  while (remainder > 0) {
    unsigned int max_incr = (remainder > N / 10) ? N / 10 : remainder;
    std::uniform_int_distribution<unsigned int> randomIncrement(1, max_incr);
    unsigned int inc = randomIncrement(re);
    sides_polygons[randomIndex(re)] += inc;
    remainder -= inc;
  }

  for (unsigned int sides : sides_polygons) randomPolygon(sides, height, width);
}

/**
 * Compute distances between all points in current instance
 * @return cost matrix of size N * N
 */
double* TSPinstance::costMatrix() const {
  double* C = new double[N * N];
  std::fill_n(C, N * N, CPX_INFBOUND);
  for (unsigned int i = 0; i < N; ++i) {
    pair<double, double> p = points->at(i);
    for (unsigned int j = i + 1; j < N; ++j) {
      pair<double, double> q = points->at(j);
      C[i * N + j] = C[j * N + i] = sqrt(pow(abs(p.first - q.first), 2.0) +
                                         pow(abs(p.second - q.second), 2.0));
    }
  }
  return C;
}

void TSPinstance::saveToCsv(const string& filepath) {
  std::ofstream file;
  file.open(filepath + ".csv", std::fstream::out);
  file << std::to_string(N) << "\n";
  for (pair<double, double> p : *points)
    file << std::to_string(p.first) << "," << std::to_string(p.second) << "\n";
  file.close();
}

unsigned int TSPinstance::loadFromCsv(const string& filepath) {
  std::ifstream file;
  file.open(filepath, std::fstream::in);

  string s_N;
  getline(file, s_N, '\n');
  // Set N
  N = std::stoi(s_N);

  // Clean and reallocate
  if (points) delete points;
  points = new vector<pair<double, double>>(N);

  // Read all N points in file
  for (unsigned int i = 0; i < N; ++i) {
    if (file.peek() == EOF)
      std::cout << "Error: not enough lines in file (" << i << "/" << N << ")";
    string val;
    getline(file, val, ',');
    double x = std::stod(val);
    getline(file, val, '\n');
    double y = std::stod(val);
    (*points)[i] = {x, y};
  }

  file.close();
  return N;
}