//
// Created by linpingta.
//

#ifndef SHALE__SHALE_H_
#define SHALE__SHALE_H_

#include "AdSupply.h"
#include "AdDemand.h"

#include <iostream>
#include <unordered_map>
#include <limits>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iterator>

// main class for shale algorithm
// Shale Offline
class ShaleOffline {
 public:
  ShaleOffline(AdSupply& supply, AdDemand& demand): supply(supply), demand(demand) {
    init();
  }
  void stageOne(int iters);
  void stageTwo();
  void print();

  std::unordered_map<std::string, double> getAlphaJ() {
    return this->alphaJ;
  }
  std::unordered_map<std::string, double> getSigmaJ() {
    return this->sigmaJ;
  }

 private:
  void init();
  void updateAlpha(const std::string& j); // update alphaJ
  void findSigma(const std::string& j); // find sigmaJ
  void updateBeta(const std::string& i); // update betaI
  std::vector<double> updateSigma(const std::string& j); // update sigmaJ
  std::vector<std::string> getDemandAllocationOrder(); // change demand order as allocation order

 private:
  AdSupply supply;
  AdDemand demand;
  std::unordered_map<std::string, double> alphaJ;  // demand Aj, offline aj
  std::unordered_map<std::string, double> betaI;   // supply Bi
  std::unordered_map<std::string, double> thetaIJ; // theta ij, the optimal goal
  std::unordered_map<std::string, double> sigmaJ;  // demand sigmaJ, sigmaJ which influence online
  std::unordered_map<std::string, double> sI;      // si
};


// Shale Online
class ShaleOnline {
 public:
  ShaleOnline(AdSupply& supply, AdDemand& demand, std::unordered_map<std::string, double>& alphaJ, std::unordered_map<std::string, double>& sigmaJ)
  : supply(supply), demand(demand), alphaJ(alphaJ), sigmaJ(sigmaJ) {
    init();
  }
  void allocation(const std::string& i); // do allocation when supply i comes
  void print();

 private:
  void init();
  void updateBeta(const std::string& i); // update betaI
  std::vector<std::string> getDemandAllocationOrderV2(const std::string& i); // change demand order as allocation order

 private:
  AdSupply& supply;
  AdDemand& demand;
  std::unordered_map<std::string, double> alphaJ;
  std::unordered_map<std::string, double> betaI;
  std::unordered_map<std::string, double> sigmaJ;
  std::unordered_map<std::string, double> thetaIJ;
  std::unordered_map<std::string, int> allocationJ;
  std::unordered_map<std::string, int> remainedI;
};

#endif //SHALE__SHALE_H_
