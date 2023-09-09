//
// Created by linpingta on 9/8/23.
//


#ifndef SHALE__ADSUPPLY_H_
#define SHALE__ADSUPPLY_H_

#include <iostream>
#include <unordered_map>
#include <fstream>
#include <vector>

class AdSupply {
 public:
  AdSupply() {
  }

  void loadInventory(const std::string& file); // load inventory from file
  int getSupply(const std::string& supplyID); // get inventory with supplyID
  std::vector<std::string> getSupplyKeys(); // get all supply IDs
  std::unordered_map<std::string, std::vector<std::string>> getSatisfyDemandList() {
    return this->satisfyDemandList;
  }
  void setSatisfyDemandList(const std::unordered_map<std::string, std::vector<std::string>>& inputDemandList) {
    this->satisfyDemandList = inputDemandList;
  }
  void print();

 private:
  std::unordered_map<std::string, int> supplyPair; // key as supplyId, value as inventoryValue
  std::unordered_map<std::string, std::vector<std::string>> satisfyDemandList; // key as supplyId, value as demandId list
};

#endif //SHALE__ADSUPPLY_H_
