#ifndef YAMLP_H
#define YAMLP_H

#include <algorithm>
// #include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <variant>

#include "params.hpp"

class YamlP {
 private:
  std::unordered_map<std::string, std::variant<bool, unsigned int, mode_type>>
      _params;

  static bool is_number(const std::string& s) {
    return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) {
                           return !std::isdigit(c);
                         }) == s.end();
  }

  static bool is_bool(const std::string& s) {
    return !s.empty() && (s == "false" || s == "true");
  }

  static bool is_enum(const std::string& s) {
    return !s.empty() && (s == "fast" || s == "load");
  }

  static void ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    [](int ch) { return !std::isspace(ch); }));
  }

  // trim from end (in place)
  static void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](int ch) { return !std::isspace(ch); })
                .base(),
            s.end());
  }

  // trim from both ends (in place)
  static void trim(std::string& s) {
    ltrim(s);
    rtrim(s);
  }

 public:
  YamlP() = default;

  void readConfig(const std::string& filepath) {
    std::ifstream infile(filepath);
    if (!infile.good())
      throw std::ios_base::failure("Failure opening file YAML");
    std::string line;
    while (std::getline(infile, line)) {
      ltrim(line);
      if (line.empty() || line[0] == '#') continue;
      std::istringstream iss(line);
      std::string key, value;
      if (!(iss >> key >> value)) break;  // error
      trim(key);
      trim(value);
      key.pop_back();  // remove ':'
      // process pair (a,b)
      std::variant<bool, unsigned int, mode_type> vval;
      if (is_number(value))
        vval = static_cast<unsigned int>(std::stoi(value));
      else if (is_bool(value))
        vval = (value == "true") ? true : false;
      else if (is_enum(value))
        vval = (value == "fast") ? fast : load;
      else
        throw std::ios_base::failure("Unsupported value in yaml: " + value);

      _params.insert({key, vval});
    }
  }

  Params config() const {
    Params p;
    p.print_console = std::get<bool>(_params.at("print_console"));
    p.generate_instances = std::get<bool>(_params.at("generate_instances"));
    p.mode = std::get<mode_type>(_params.at("mode"));
    p.solve_cplex = std::get<bool>(_params.at("solve_cplex"));
    p.solve_heur = std::get<bool>(_params.at("solve_heur"));
    p.solve_heur = std::get<bool>(_params.at("solve_heur"));
    p.N_min = std::get<unsigned int>(_params.at("N_min"));
    p.N_max = std::get<unsigned int>(_params.at("N_max"));
    p.N_incr = std::get<unsigned int>(_params.at("N_incr"));
    p.K = std::get<unsigned int>(_params.at("K"));
    p.I = std::get<unsigned int>(_params.at("I"));
    p.max_neighbours = std::get<unsigned int>(_params.at("max_neighbours"));
    p.backtracking_threshold =
        std::get<unsigned int>(_params.at("backtracking_threshold"));
    p.intens_min_depth = std::get<unsigned int>(_params.at("intens_min_depth"));
    p.intens_n_tours = std::get<unsigned int>(_params.at("intens_n_tours"));
    p.LK_iterations = std::get<unsigned int>(_params.at("LK_iterations"));
    return p;
  }
};

#endif /* YAMLP_H */