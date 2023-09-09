//
// Created by Chu, Tong on 9/8/23.
//

#include "Shale.h"

// max value
std::vector<double> mathMax(std::vector<std::tuple<double, double, double>>& coef, double y) {
  std::vector<double> solutions;
  double sum_cons = 0.0;
  double sum_coef = 0.0;
  for (const auto& t : coef) {
    sum_cons += std::get<1>(t);
    sum_coef += std::get<2>(t);
  }
  std::sort(coef.begin(), coef.end(), [](const auto& a, const auto& b) {
    return std::get<0>(a) < std::get<0>(b);
  });
  double res = (sum_cons - y) / sum_coef;
  if (res <= std::get<0>(coef[0])) {
    solutions.push_back(res);
  }
  for (size_t i = 1; i < coef.size(); ++i) {
    sum_cons -= std::get<1>(coef[i - 1]);
    sum_coef -= std::get<2>(coef[i - 1]);
    res = (sum_cons - y) / sum_coef;
    if (res <= std::get<0>(coef[i])) {
      solutions.push_back(res);
      break;
    }
  }
  return solutions;
}

void ShaleOffline::init() {
  for (const auto& j : this->demand.getDemandKeys()) {
    this->alphaJ[j] = 0.0;
    double sum = 0.0;
    for (const auto& i : this->demand.getTargetSupply(j)) {
      double supply = this->supply.getSupply(i);
      if (supply < 0) {
        continue;
      }
      sum += supply;
    }
    if (sum <= 0.0) { // no mapping supply, just set thetaIJ as 1.0
      this->thetaIJ[j] = 1.0;
    } else {
      this->thetaIJ[j] = this->demand.getDemandBudget(j) / sum;
    }
  }
}

void ShaleOffline::stageOne(int iters) {
  while (iters > 0) {
    std::cout << "remaining iters round: " << iters << std::endl;

    // control alpha, update beta
    for (const auto& i : this->supply.getSupplyKeys()) {
      this->updateBeta(i);
    }

    // control beta, update alpha
    for (const auto& j : this->demand.getDemandKeys()) {
      this->updateAlpha(j);
    }
    iters--;
  }
}

void ShaleOffline::stageTwo() {
  for (const auto& i : this->supply.getSupplyKeys()) {
    this->sI[i] = this->supply.getSupply(i);
    this->updateBeta(i);
  }
  std::unordered_map<std::string, double> sigma;
  for (const auto& j : this->demand.getDemandKeys()) {
    // update logic here
    this->findSigma(j);

    for (const auto& i : this->demand.getTargetSupply(j)) {
      double g = std::max(0.0, this->thetaIJ[j] * (1.0 + (sigma[j] - this->betaI[i]) / this->demand.getV(j)));
      this->sI[i] -= std::min(this->sI[i], this->supply.getSupply(i) * g);
    }
  }
}

void ShaleOffline::output() {
  std::cout << "<--- alpha --->" << std::endl;
  for (const auto& kv : this->alphaJ) {
    std::cout << "demand alpha: " << kv.first << " : " << kv.second << std::endl;
  }
  std::cout << "<--- beta --->" << std::endl;
  for (const auto& kv : this->betaI) {
    std::cout << "supply beta: " << kv.first << " : " << kv.second << std::endl;
  }
  std::cout << "<--- sigma --->" << std::endl;
  for (const auto& kv : this->sigmaJ) {
    std::cout << "demand sigma: " << kv.first << " : " << kv.second << std::endl;
  }
  std::cout << "<--- sI --->" << std::endl;
  for (const auto& kv : this->sI) {
    std::cout << "supply sI: " << kv.first << " : " << kv.second << std::endl;
  }
}

void ShaleOffline::findSigma(const std::string& j) {
  // TBD
}

void ShaleOffline::updateBeta(const std::string& i) {
  std::vector<std::tuple<double, double, double>> coef;
  auto satisfyDemandList = this->supply.getSatisfyDemandList();
  for (const auto& j : satisfyDemandList[i]) {
    double a = this->thetaIJ[j] * (1.0 + this->alphaJ[j] / this->demand.getV(j));
    double b = this->thetaIJ[j] / this->demand.getV(j);
    coef.push_back(std::make_tuple(a / b, a, b));
  }
  std::vector<double> result = mathMax(coef, 1.0);
  if (result.empty() || result[0] < 0.0) {
    this->betaI[i] = 0.0;
  }
  else {
    this->betaI[i] = result[0];
  }
}

void ShaleOffline::updateAlpha(const std::string& j) {
  std::vector<std::tuple<double, double, double>> coef;
  for (const auto& i : this->demand.getTargetSupply(j)) {
    double s = this->supply.getSupply(i);
    double a = -1.0 * s * this->thetaIJ[j] * (1.0 - this->betaI[i] / this->demand.getV(j));
    double b = -1.0 * s * this->thetaIJ[j] / this->demand.getV(j);
    coef.push_back(std::make_tuple(a / b, a, b));
  }
  std::vector<double> result = mathMax(coef, -1.0 * this->demand.getDemandBudget(j));
  if (result.empty() || -1.0 * result[0] > this->demand.getPenalty(j)) {
    this->alphaJ[j] = this->demand.getPenalty(j); // aj = pj
  }
  else {
    this->alphaJ[j] = -1.0 * result[0];
  }
}

void ShaleOnline::init() {
  for (const auto& i : this->supply.getSupplyKeys()) {
    this->remainedI[i] = this->supply.getSupply(i);
  }
  for (const auto& j : this->demand.getDemandKeys()) {
    double sum = 0.0;
    for (const auto& i : this->demand.getTargetSupply(j)) {
      sum += this->supply.getSupply(i);
    }
    thetaIJ[j] = this->demand.getDemandBudget(j) / sum;
    this->allocationJ[j] = 0;
  }
}

void ShaleOnline::allocation(const std::string& i) {
  double s = 1.0;
  std::unordered_map<std::string, double> xIJ;
  if (this->betaI.find(i) == this->betaI.end()) {
    this->updateBeta(i);
  }
  for (const auto& j : this->supply.getSatisfyDemandList()[i]) {
    double g = std::max(0.0, this->thetaIJ[j] * (1.0 + (this->sigmaJ[j] - this->betaI[i]) / this->demand.getV(j)));
    xIJ[j] = std::min(s, g);
    s -= xIJ[j];
  }
  double sum = 0.0;
  for (const auto& kv : xIJ) {
    sum += kv.second;
  }
  if (sum < 1.0) {
    std::cout << "In supply: " << i << ", there is " << 1.0 - sum << " probability with no contract" << std::endl;
  }
  double r = (double)rand() / RAND_MAX;
  sum = 0.0;
  for (const auto& kv : xIJ) {
    sum += kv.second;
    if (r < sum) {
      this->allocationJ[kv.first]++;
      this->remainedI[i]--;
      break;
    }
  }
}

void ShaleOnline::updateBeta(const std::string& i) {
  std::vector<std::tuple<double, double, double>> coef;
  auto satisfyDemandList = this->supply.getSatisfyDemandList();
  for (const auto& j : satisfyDemandList[i]) {
    double a = this->thetaIJ[j] * (1.0 + this->alphaJ[j] / this->demand.getV(j));
    double b = this->thetaIJ[j] / this->demand.getV(j);
    coef.push_back(std::make_tuple(a / b, a, b));
  }
  std::vector<double> result = mathMax(coef, 1.0);
  if (result.empty() || result[0] < 0.0) {
    this->betaI[i] = 0.0;
  }
  else {
    this->betaI[i] = result[0];
  }
}

void ShaleOnline::print() {
  // demand allocation
  std::cout << "\nAllocation:" << std::endl;
  std::cout << "demand_node\tdemand\t\tallocation" << std::endl;
  for (const auto& kv : this->allocationJ) {
    std::cout << kv.first << "\t\t" << this->demand.getDemandBudget(kv.first) << "\t\t" << kv.second << std::endl;
  }

  // supply remaining
  std::cout << "\nRemaind:" << std::endl;
  std::cout << "supply_node\tinventory\tremaind" << std::endl;
  for (const auto& kv : this->remainedI) {
    std::cout << kv.first << "\t\t" << this->supply.getSupply(kv.first) << "\t\t" << kv.second << std::endl;
  }
}