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

/**
 * Generate a random regular polygon with n_sides
 * @param h center height range  will be in [0, h)
 * @param w center width range in [0, w)
 */
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

  // Max number of sides for polygons
  unsigned int max_sides = 10;
  unsigned int min_sides = 3;
  // Distribution for number of sides
  std::uniform_int_distribution<unsigned int> randomUInt(min_sides, max_sides);
  vector<unsigned int> sides_pol;
  unsigned int sides_sum = 0;
  unsigned int remainder = 0;
  while (sides_sum < N) {
    unsigned int s = randomUInt(re);
    if (s > (N - sides_sum)) {
      remainder = N - sides_sum;
      break;
    }
    sides_pol.push_back(s);
    sides_sum += s;
  }
  // Fix polygons adding or removing sides to desired number
  if (remainder) {
    if (remainder < min_sides) {  // cannot make a polygon
                                  // Remove some edges from previous polygons
      for (unsigned int& np : sides_pol) {
        if (np < max_sides) {
          np++;  // add a side to a polygon
          remainder--;
          if (remainder == 0) break;
        }
      }
      // If still there are missing points
      if (remainder) {
        for (unsigned int& np : sides_pol) {
          if (np > min_sides) {
            np--;  // remove a side from a polygon
            remainder++;
            if (remainder == min_sides) break;
          }
        }
      }
    }
    // remainder >= 3
    if (remainder) sides_pol.push_back(remainder);
  }
  // if remainder == 0 nothing to do

  // Build each polygon
  for (unsigned int sides : sides_pol) randomPolygon(sides, height, width);
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

/**
 * Save instance to csv
 */
void TSPinstance::saveToCsv(const string& filepath) {
  std::ofstream file;
  file.open(filepath + ".csv", std::fstream::out);
  file << std::to_string(N) << "\n";
  for (pair<double, double> p : *points)
    file << std::to_string(p.first) << "," << std::to_string(p.second) << "\n";
  file.close();
}

/**
 * Load instance from csv
 */
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