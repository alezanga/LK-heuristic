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
  std::unordered_map<std::string,
                     std::variant<bool, unsigned int, std::vector<std::string>,
                                  std::vector<uint>>>
      _params;

  static bool is_number(const std::string& s) {
    return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) {
                           return !std::isdigit(c);
                         }) == s.end();
  }

  static bool is_bool(const std::string& s) {
    return !s.empty() && (s == "false" || s == "true");
  }

  static bool is_list(const std::string& s) {
    return !s.empty() && s.front() == '[' && s.back() == ']';
  }

  /**
   * Parse a string in the format: [a, b, c, d]
   * @param s input string
   * @return vector of strings with content of list
   */
  static std::vector<std::string> parseStringList(std::string& s) {
    std::vector<std::string> list;
    s.pop_back();        // remove ']'
    s.erase(s.begin());  // remove '['
    if (s.empty()) {
      std::cerr << "Warning: no instances to read!";
      return list;
    }
    size_t prev_pos = 0;
    for (size_t pos = s.find(','); pos != std::string::npos;
         prev_pos = pos + 1, pos = s.find(',', prev_pos)) {
      std::string token = s.substr(prev_pos, (pos - prev_pos));
      trim(token);
      list.push_back(std::move(token));
    }
    std::string last_token = s.substr(prev_pos);
    trim(last_token);
    list.push_back(std::move(last_token));
    return list;
  }

  static std::vector<uint> parseIntList(std::string& s) {
    std::vector<uint> list;
    s.pop_back();        // remove ']'
    s.erase(s.begin());  // remove '['
    if (s.empty()) {
      std::cerr << "Warning: no instances to read!";
      return list;
    }
    size_t prev_pos = 0;
    for (size_t pos = s.find(','); pos != std::string::npos;
         prev_pos = pos + 1, pos = s.find(',', prev_pos)) {
      std::string token = s.substr(prev_pos, (pos - prev_pos));
      trim(token);
      list.push_back(std::stoi(token));
    }
    std::string last_token = s.substr(prev_pos);
    trim(last_token);
    list.push_back(std::stoi(last_token));
    return list;
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

      // Split string into key and value
      std::istringstream iss(line);
      std::string key, value;
      std::getline(iss, key, ':');
      std::getline(iss, value);
      trim(key);
      trim(value);

      // Process pair (key,value)
      std::variant<bool, unsigned int, std::vector<std::string>,
                   std::vector<uint>>
          vval;
      if (is_number(value))
        vval = static_cast<unsigned int>(std::stoi(value));
      else if (is_bool(value))
        vval = (value == "true") ? true : false;
      else if (is_list(value))
        if (key == "instances_to_read")  // list of strings
          vval = parseStringList(value);
        else  // list of ints
          vval = parseIntList(value);
      else
        throw std::ios_base::failure("Unsupported value in yaml: " + value);

      _params.insert({key, vval});
    }
  }

  Params config() const {
    Params p;
    p.print_console = std::get<bool>(_params.at("print_console"));
    p.generate_instances = std::get<bool>(_params.at("generate_instances"));
    p.instances_to_read =
        std::get<std::vector<std::string>>(_params.at("instances_to_read"));
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

    p.restarts = std::get<std::vector<uint>>(_params.at("restarts"));
    p.size_to_test = std::get<std::vector<uint>>(_params.at("size_to_test"));
    p.runs_per_instance = std::get<uint>(_params.at("runs_per_instance"));
    return p;
  }
};

#endif /* YAMLP_H */