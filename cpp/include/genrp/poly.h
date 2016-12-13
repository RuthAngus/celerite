#ifndef _GENRP_POLY_H_
#define _GENRP_POLY_H_

#include <cmath>
#include <vector>
#include <Eigen/Core>

namespace genrp {

double polyval (const Eigen::VectorXd& p, const double x) {
  double result = 0.0;
  for (size_t i = 0; i < p.rows(); ++i) result = result * x + p[i];
  return result;
}

Eigen::VectorXd polyadd (const Eigen::VectorXd& p1, const Eigen::VectorXd& p2) {
  int n1 = p1.rows(),
      n2 = p2.rows(),
      n = std::max(p1.rows(), p2.rows());
  Eigen::VectorXd result(n);
  result.setConstant(0.0);
  for (int i = 0; i < n; ++i) {
    if (i < n1) result[n - i - 1] += p1[n1 - i - 1];
    if (i < n2) result[n - i - 1] += p2[n2 - i - 1];
  }
  return result;
}

Eigen::VectorXd polymul (const Eigen::VectorXd& p1, const Eigen::VectorXd& p2) {
  int n1 = p1.rows(),
      n2 = p2.rows(),
      n = n1 + n2 - 1;
  Eigen::VectorXd result(n);
  result.setConstant(0.0);
  for (int i = 0; i < n; ++i)
    for (int j = 0; j < n2; ++j)
      if (i - j >= 0 && i - j < n1 && j >= 0)
        result[i] += p1[i - j] * p2[j];
  return result;
}

Eigen::VectorXd polyrem (const Eigen::VectorXd& u, const Eigen::VectorXd& v) {
  int m = u.rows() - 1,
      n = v.rows() - 1,
      p = m - n + 1;
  double d, scale = 1.0 / v[0];
  Eigen::VectorXd q = Eigen::VectorXd::Zero(std::max(p, 1)),
                  r = u;  // This makes a copy!
  for (int k = 0; k < p; ++k) {
    d = scale * r[k];
    q[k] = d;
    for (int i = 0; i < n+1; ++i) r[k+i] -= d*v[i];
  }
  int strt;
  for (strt = 0; strt < m; ++strt) {
    if (std::abs(r[strt]) > 1e-14) break;
  }
  return r.tail(m + 1 - strt);
}

Eigen::VectorXd polyder (const Eigen::VectorXd& p) {
  int n = p.rows() - 1;
  Eigen::VectorXd d = p;  // Copy.
  for (int i = 0; i < n; ++i) {
    d[i] *= n - i;
  }
  return d.head(n);
}

std::vector<Eigen::VectorXd> polysturm (const Eigen::VectorXd& p) {
  int n = p.rows() - 1;
  std::vector<Eigen::VectorXd> sturm;
  Eigen::VectorXd p0 = p, p1 = polyder(p0), tmp;
  sturm.push_back(p0);
  sturm.push_back(p1);
  for (int k = 0; k < n; ++k) {
    tmp = p1;
    p1 = polyrem(p0, p1);
    p1 *= -1.0;
    p0 = tmp;
    sturm.push_back(p1);
    if (p1.rows() == 1) break;
  }
  return sturm;
}

template <typename T>
int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

// Count the positive roots of a polynomial using Sturm's theorem.
int polycountroots (const Eigen::VectorXd& p) {
  int n = p.rows() - 1,
      count = 0;

  // Compute the initial signs and count any initial sign change.
  Eigen::VectorXd p0 = p, p1 = polyder(p0), tmp;
  int s_0 = sgn(p1[p1.rows() - 1]),
      s_inf = sgn(p1[0]),
      s;
  count += (sgn(p0[p0.rows() - 1]) != s_0);
  count -= (sgn(p0[0]) != s_inf);

  // Loop over the Sturm sequence and compute each polynomial.
  for (int k = 0; k < n; ++k) {
    tmp = p1;
    p1 = polyrem(p0, p1);
    p1 *= -1.0;
    p0 = tmp;

    // Count the roots for this next polynomial.
    s = s_0;
    s_0 = sgn(p1[p1.rows() - 1]);
    count += (s != s_0);
    s = s_inf;
    s_inf = sgn(p1[0]);
    count -= (s != s_inf);

    if (p1.rows() == 1) break;
  }
  return count;
}

};

#endif
