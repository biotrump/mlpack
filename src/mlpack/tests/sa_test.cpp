/*
 * @file sa_test.cpp
 * @auther Zhihao Lou
 *
 * Test file for SA (simulated annealing).
 *
 * This file is part of MLPACK 1.0.10.
 *
 * MLPACK is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * MLPACK is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details (LICENSE.txt).
 *
 * You should have received a copy of the GNU General Public License along with
 * MLPACK.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <mlpack/core.hpp>
#include <mlpack/core/optimizers/sa/sa.hpp>
#include <mlpack/core/optimizers/sa/exponential_schedule.hpp>
#include <mlpack/core/optimizers/lbfgs/test_functions.hpp>

#include <mlpack/core/metrics/ip_metric.hpp>
#include <mlpack/core/metrics/lmetric.hpp>
#include <mlpack/core/metrics/mahalanobis_distance.hpp>

#include <boost/test/unit_test.hpp>
#include "old_boost_test_definitions.hpp"

using namespace std;
using namespace arma;
using namespace mlpack;
using namespace mlpack::optimization;
using namespace mlpack::optimization::test;
using namespace mlpack::metric;

BOOST_AUTO_TEST_SUITE(SATest);

BOOST_AUTO_TEST_CASE(GeneralizedRosenbrockTest)
{
  size_t dim = 50;
  GeneralizedRosenbrockFunction f(dim);

  ExponentialSchedule schedule(1e-5);
  SA<GeneralizedRosenbrockFunction, ExponentialSchedule>
      sa(f, schedule, 10000000, 1000., 1000, 100, 1e-9, 3, 20, 0.3, 0.3);
  arma::mat coordinates = f.GetInitialPoint();
  const double result = sa.Optimize(coordinates);

  BOOST_REQUIRE_SMALL(result, 1e-6);
  for (size_t j = 0; j < dim; ++j)
      BOOST_REQUIRE_CLOSE(coordinates[j], (double) 1.0, 1e-2);
}

// The Rosenbrock function is a simple function to optimize.
BOOST_AUTO_TEST_CASE(RosenbrockTest)
{
  RosenbrockFunction f;
  ExponentialSchedule schedule(1e-5);
  SA<RosenbrockFunction> //sa(f, schedule); // All default parameters.
      sa(f, schedule, 10000000, 1000., 1000, 100, 1e-11, 3, 20, 0.3, 0.3);
  arma::mat coordinates = f.GetInitialPoint();

  const double result = sa.Optimize(coordinates);

  BOOST_REQUIRE_SMALL(result, 1e-6);
  BOOST_REQUIRE_CLOSE(coordinates[0], 1.0, 1e-3);
  BOOST_REQUIRE_CLOSE(coordinates[1], 1.0, 1e-3);
}

/**
 * The Rastigrin function, a (not very) simple nonconvex function.  It is
 * defined by
 *
 *   f(x) = 10n + \sum_{i = 1}^{n} (x_i^2 - 10 cos(2 \pi x_i)).
 *
 * It has very many local minima, so finding the true global minimum is
 * difficult.  The function is two-dimensional, and has minimum 0 where
 * x = [0 0].  We are only using it for simulated annealing, so there is no need
 * to implement the gradient.
 */
class RastrigrinFunction
{
 public:
  double Evaluate(const arma::mat& coordinates) const
  {
    double objective = 20; // 10 * n, n = 2.
    objective += std::pow(coordinates[0], 2.0) -
        10 * std::cos(2 * M_PI * coordinates[0]);
    objective += std::pow(coordinates[1], 2.0) -
        10 * std::cos(2 * M_PI * coordinates[1]);

    return objective;
  }

  arma::mat GetInitialPoint() const
  {
    return arma::mat("-3 -3");
  }
};

BOOST_AUTO_TEST_CASE(RastrigrinFunctionTest)
{
  // Simulated annealing isn't guaranteed to converge (except in very specific
  // situations).  If this works 1 of 3 times, I'm fine with that.  All I want
  // to know is that this implementation will escape from local minima.
  size_t successes = 0;

  for (size_t trial = 0; trial < 3; ++trial)
  {
    RastrigrinFunction f;
    ExponentialSchedule schedule(3e-6);
    SA<RastrigrinFunction> //sa(f, schedule);
        sa(f, schedule, 20000000, 100, 50, 1000, 1e-9, 2, 0.2, 0.01, 0.1);
    arma::mat coordinates = f.GetInitialPoint();

    const double result = sa.Optimize(coordinates);

    if ((std::abs(result) < 1e-3) &&
        (std::abs(coordinates[0]) < 1e-3) &&
        (std::abs(coordinates[1]) < 1e-3))
      ++successes;
  }

  BOOST_REQUIRE_GE(successes, 1);
}

BOOST_AUTO_TEST_SUITE_END();
