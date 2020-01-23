#ifndef TSP_INSTANCE
#define TSP_INSTANCE

class TSPinstance {
  unsigned int N;
  std::vector<std::pair<double, double>>* points;

  void randomPolygon(unsigned int, unsigned int, unsigned int);

 public:
  TSPinstance();
  ~TSPinstance();
  const std::vector<std::pair<double, double>>* getPoints() const;
  void generateRandomPolygons(unsigned int);
  double* costMatrix() const;
  void saveToCsv(const std::string&);
  unsigned int loadFromCsv(const std::string&);
};

#endif /* TSP_INSTANCE */