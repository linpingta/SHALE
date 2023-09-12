//
// Created by linpingta.
//

#include "Shale.h"

// max value
// max(coef.1 - coef.2 * x) = y
std::vector<double> mathMax(std::vector<std::tuple<double, double, double>>& coef, double y) {
  std::vector<double> solutions;
  double sum_cons = 0.0;
  double sum_coef = 0.0;
  for (const auto& t : coef) {
    sum_cons += std::get<1>(t);
    sum_coef += std::get<2>(t);
  }
  if (sum_coef == 0.0) {
    return solutions;
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


// Shale Offline
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

  for (const auto& pair : this->betaI) {
    std::cout << "betaI key: " << pair.first << ", value: " << pair.second << std::endl;
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

std::vector<double> ShaleOffline::updateSigma(const std::string& j) {
  std::vector<double> solutions;
  std::vector<std::tuple<double, double, double, double, double>> coef;

  // Assuming you have these member functions and variables defined in your class
  auto targetSupply = this->demand.getTargetSupply(j);
  double demand_v = this->demand.getV(j);

  for (auto i : targetSupply) {
    double s = this->supply.getSupply(i);
    double a = s * this->thetaIJ[j] * (1.0 - this->betaI[i] / demand_v);
    double b = s * this->thetaIJ[j] / demand_v;
    coef.push_back(std::make_tuple((this->sI[i] - a) / b, a / b, a, b, this->sI[i]));
  }

  std::sort(coef.begin(), coef.end(), [](const auto &t1, const auto &t2) {
    return std::get<1>(t1) < std::get<1>(t2);
  });

  for (size_t k = 0; k < coef.size(); ++k) {
    std::vector<std::tuple<double, double, double, double, double>> temp(coef.begin() + k, coef.end());
    std::sort(temp.begin(), temp.end(), [](const auto &t1, const auto &t2) {
      return std::get<0>(t1) < std::get<0>(t2);
    });

    double sum_remained = 0.0;
    double sum_cons = 0.0;
    double sum_coef = 0.0;

    for (const auto &t : temp) {
      sum_cons += std::get<2>(t);
      sum_coef += std::get<3>(t);
      sum_remained += std::get<4>(t);
    }

    double res = (sum_cons - this->demand.getDemandBudget(j)) / sum_coef;

    if (sum_remained < this->demand.getDemandBudget(j)) {
      continue;
    }

    if (k == 0) {
      if (res <= std::get<0>(temp[0]) && res <= std::get<1>(coef[k])) {
        solutions.push_back(res);
      }
    } else {
      if (res <= std::get<0>(temp[0]) && res <= std::get<1>(coef[k]) && res >= std::get<1>(coef[k - 1])) {
        solutions.push_back(res);
      }
    }

    sum_remained = 0.0;

    for (size_t t = 1; t < temp.size(); ++t) {
      sum_cons -= std::get<2>(temp[t - 1]);
      sum_coef -= std::get<3>(temp[t - 1]);
      sum_remained += std::get<4>(temp[t - 1]);
      res = (sum_cons + sum_remained - this->demand.getDemandBudget(j)) / sum_coef;

      if (k == 0) {
        if (res <= std::get<0>(temp[t]) && res >= std::get<0>(temp[t - 1]) && res <= std::get<1>(coef[k])) {
          solutions.push_back(res);
        }
      } else {
        if (res <= std::get<0>(temp[t]) && res >= std::get<0>(temp[t - 1]) &&
            res <= std::get<1>(coef[k]) && res >= std::get<1>(coef[k - 1])) {
          solutions.push_back(res);
        }
      }
    }
  }

  return solutions;
}

void ShaleOffline::findSigma(const std::string& j) {
  std::vector<double> result = this->updateSigma(j);
  if (result.empty()) {
    this->sigmaJ[j] = std::numeric_limits<double>::infinity();
  } else {
    this->sigmaJ[j] = -1.0 * result[0];
  }
}

void ShaleOffline::updateBeta(const std::string& i) {
  std::vector<std::tuple<double, double, double>> coef;

  // iterate through j, sum j belongs to i
  auto satisfyDemandList = this->supply.getSatisfyDemandList();
  for (const auto& j : satisfyDemandList[i]) {
    // max(sum(thetaJ * (1+ (alphaJ - betaI) / V)) = 1
    // a = sum(thetaJ * (1+ alphaJ / V))
    // b = sum(thetaJ * 1/V) * betaI
    // max(a - b * betaI) = 1
    double a = this->thetaIJ[j] * (1.0 + this->alphaJ[j] / this->demand.getV(j));
    double b = this->thetaIJ[j] / this->demand.getV(j);
    coef.emplace_back(std::make_tuple(a / b, a, b));
  }
  std::vector<double> result = mathMax(coef, 1.0);
  if (result.empty() || result[0] < 0.0) {
    this->betaI[i] = 0.0;
  }
  else {
    this->betaI[i] = result[0];
  }
  std::cout <<  "update supply beta: " << i << " with betaI: " << this->betaI[i] << std::endl;
}

void ShaleOffline::updateAlpha(const std::string& j) {
  std::vector<std::tuple<double, double, double>> coef;
  // iterate through i, sum i belong to j
  for (const auto& i : this->demand.getTargetSupply(j)) {
    double s = this->supply.getSupply(i);
    double a = -1.0 * s * this->thetaIJ[j] * (1.0 - this->betaI[i] / this->demand.getV(j));
    double b = -1.0 * s * this->thetaIJ[j] / this->demand.getV(j);
    coef.emplace_back(std::make_tuple(a / b, a, b));
  }
  std::vector<double> result = mathMax(coef, -1.0 * this->demand.getDemandBudget(j));
  if (result.empty() || -1.0 * result[0] > this->demand.getPenalty(j)) {
    this->alphaJ[j] = this->demand.getPenalty(j); // aj = pj
  }
  else {
    this->alphaJ[j] = -1.0 * result[0];
  }
  std::cout <<  "update demand alpha: " << j << " with alphaJ: " << this->alphaJ[j] << std::endl;
}


// Shale Online
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

  auto demandList = this->supply.getSatisfyDemandList();
  for (const auto& j : demandList[i]) {
//    std::cout << "thetaIJ: " << this->thetaIJ[j] << std::endl;
    double g = std::max(0.0, this->thetaIJ[j] * (1.0 + (this->sigmaJ[j] - this->betaI[i]) / this->demand.getV(j)));
    xIJ[j] = std::min(s, g);
    s -= xIJ[j];
  }
//  std::cout << "s value: " << s << std::endl;

  double sum = 0.0;
  for (const auto& kv : xIJ) {
    sum += kv.second;
  }
//  if (sum < 1.0) {
//    std::cout << "In supply: " << i << ", there is " << 1.0 - sum << " probability with no contract" << std::endl;
//  }
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
    std::cout << kv.first << "\t" << this->demand.getDemandBudget(kv.first) << "\t" << kv.second << std::endl;
  }

  // supply remaining
  std::cout << "\nRemained:" << std::endl;
  std::cout << "supply_node\tinventory\tremained" << std::endl;
  for (const auto& kv : this->remainedI) {
    std::cout << kv.first << "\t" << this->supply.getSupply(kv.first) << "\t" << kv.second << std::endl;
  }
}