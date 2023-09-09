//
// Created by Chu, Tong on 9/8/23.
//

#include "AdSupply.h"
#include <sstream>

int AdSupply::getSupply(const std::string& supplyID) {
  auto iter = this->supplyPair.find(supplyID);

  if (iter != this->supplyPair.end()) {
    return iter->second;
  }
  return -1;
}


std::vector<std::string> AdSupply::getSupplyKeys(){
  std::vector<std::string> keys;
  for (const auto& pair: this->supplyPair) {
    keys.push_back(pair.first);
  }
  return keys;
}


void AdSupply::loadInventory(const std::string& file) {
  std::ifstream f(file);
  if (!f.is_open()) {
    std::cerr << "Error for opening supply file: " << file << std::endl;
  }

  std::string line;
  while (std::getline(f, line)) {
    if (line.find("#") != std::string::npos) {
      continue;
    }

    std::istringstream iss(line);
    std::string supplyID = "";
    int inventoryValue = -1;
    iss >> supplyID >> inventoryValue;

    supplyPair[supplyID] = inventoryValue;
  }
}

void AdSupply::print() {
  std::cout << "\nSupply:" << std::endl;
  std::cout << "supply_node\tinventory\tsatisfy_demand" << std::endl;
  for (const auto& i : this->getSupplyKeys()) {
    std::cout << i << "\t\t" << this->getSupply(i) << "\t\t";
    for (const auto& j : this->satisfyDemandList[i]) {
      std::cout << j << ",";
    }
    std::cout << std::endl;
  }
}