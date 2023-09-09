#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>

#include "AdDemand.h"
#include "AdSupply.h"
#include "Shale.h"

int main() {
  AdSupply supply;
  supply.loadInventory("./supply.txt");

  AdDemand demand;
  std::unordered_map<std::string, std::vector<std::string>> satisfyDemandList;
  demand.loadDemand("./demand.txt", satisfyDemandList);
  supply.setSatisfyDemandList(satisfyDemandList);

  ShaleOffline shaleOffline(supply, demand);
  shaleOffline.stageOne(5);
  shaleOffline.stageTwo();
  shaleOffline.output();

  // fake the online logic
  std::unordered_map<std::string, double> alphaJ = shaleOffline.getAlphaJ();
  std::unordered_map<std::string, double> sigmaJ = shaleOffline.getSigmaJ();
  ShaleOnline shaleOnline(supply, demand, alphaJ, sigmaJ);
  for (const auto& i : supply.getSupplyKeys()) {
    int inventory = supply.getSupply(i);
    while (inventory > 0) {
      shaleOnline.allocation(i);
      --inventory;
    }
  }

  demand.print();
  supply.print();
  shaleOnline.print();

  return 0;
}