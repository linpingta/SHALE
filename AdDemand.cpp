//
// Created by linpingta.
//

#include "AdDemand.h"
#include <sstream>

void AdDemand::loadDemand(const std::string& file, std::unordered_map<std::string, std::vector<std::string>>& satisfyDemand) {
  std::ifstream f(file);
  if (!f.is_open()) {
    std::cerr << "Error open demand file" << file << std::endl;
    exit(1);
  }

  std::string line;
  while (std::getline(f, line)) {
    if (line.find("#") != std::string::npos) {
      continue;
    }

    std::istringstream iss(line);
    std::string demandID, ii;
    int budget;
    double penalty;
    iss >> demandID >> budget >> penalty >> ii;
    this->demandBudgetPair[demandID] = budget;
    this->demandPenaltyPair[demandID] = penalty;

    std::istringstream iiss(ii);
    std::string supplyID;
    while (std::getline(iiss, supplyID, ',')) {
      this->targetSupply[demandID].push_back(supplyID);
      satisfyDemand[supplyID].push_back(demandID);
    }

  }
}

int AdDemand::getDemandBudget(const std::string& demandID) {
  auto iter = this->demandBudgetPair.find(demandID);
  if (iter != this->demandBudgetPair.end()) {
    return iter->second;
  }
  return -1;
}

double AdDemand::getPenalty(const std::string &demandID) {
  auto iter = this->demandPenaltyPair.find(demandID);
  if (iter != this->demandPenaltyPair.end()) {
    return iter->second;
  }
  return -1.0;
}

std::vector<std::string> AdDemand::getTargetSupply(const std::string& demandID) {
  auto iter = this->targetSupply.find(demandID);
  if (iter != this->targetSupply.end()) {
    return iter->second;
  }
  return std::vector<std::string>();
}

std::vector<std::string> AdDemand::getDemandKeys(){
  std::vector<std::string> keys;
  for (const auto& pair: this->demandBudgetPair) {
    keys.push_back(pair.first);
  }
  return keys;
}

void AdDemand::print(){
  std::cout << "\nDemand:" << std::endl;
  std::cout << "demand_node\tdemand\tpenalty\ttarget_supply" << std::endl;
  for (const auto& j : this->getDemandKeys()) {
    std::cout <<  "demand: " << j << ", \t budget: " << this->getDemandBudget(j) << ", \t penalty: " << this->getPenalty(j) << "\t";
    for (const auto& i : this->getTargetSupply(j)) {
      std::cout << " \t supply_id: " << i << ",";
    }
    std::cout << std::endl;
  }

}