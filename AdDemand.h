//
// Created by linpingta.
//

#ifndef SHALE__ADDEMAND_H_
#define SHALE__ADDEMAND_H_

#include <iostream>
#include <unordered_map>
#include <fstream>
#include <vector>

class AdDemand {
 public:
  AdDemand() {}

  void loadDemand(const std::string& file, std::unordered_map<std::string, std::vector<std::string>>& satisfyDemand);
  int getDemandBudget(const std::string& demandID);
  double getPenalty(const std::string& demandID);
  std::vector<std::string> getDemandKeys();
  std::vector<std::string> getTargetSupply(const std::string& demandID);
  void print();
 public:
  double getV(const std::string& j) { return 1.0; } // for simple assumption
 private:
  std::unordered_map<std::string, int> demandBudgetPair;
  std::unordered_map<std::string, double> demandPenaltyPair;
  std::unordered_map<std::string, std::vector<std::string>> targetSupply;
};

#endif //SHALE__ADDEMAND_H_
