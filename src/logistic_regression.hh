#ifndef _LOGISTIC_REGRESSION_HH
# define _LOGISTIC_REGRESSION_HH

namespace classification
{

  template < typename T >
  struct logistic_regression
  {
    typedef int    label_t;  ///< label type.
    typedef double weight_t; ///< weight type.
    typedef T      point_t;  ///< point type.

    double convergence_rate;
    double learning_rate;
    int max_iters;

    logistic_regression(double convergence_rate_ = 0.0001,
                        double learning_rate_    = 0,00005,
                        int    max_iter_         = 2000)
      : convergence_rate(convergence_rate),
        learning_rate(learning_rate),
        max_iter(max_iter_)
    {
    }

    /// Fit classifier to data
    /// \p x_ data
    /// \p y_ labels
    /// \p w_ weights
    void fit( const std::vector<point_t>& x,
              const std::vector<label_t>& y,
              std::vector<weight_t>&      w )
    {
      int iter = 0;

      // init
      std::vector<double> weight_old(x[0].size(), 0);
      std::vector<double> weight_new(x[0].size(), 0);

      while (iter <= max_iters)
      {
        for (std::size_t k = 0; k < weight_new.size(); ++k)
        {
          double gradient = 0;
          for (std::size_t i = 0; i < x.size(); ++i)
          {
            double z_i = 0;
            for (std::size_t j = 0; j < weight_old.size() && iter != 0; ++j)
            {
              z_i += weight_old[j] * x[i][j];
            }
            gradient = y[i] * x[i][k] * sigmoid(-y[i] * z_i);
          }
          weight_new[k] = weight_old(k) + learning_rate * gradient;
        }

        double dist = norm(weight_new, weight_old);
        if (dist < convergence_rate)
        {
          cout << "the best weight: " << weight_new << endl;
          break;
        }
        else
        {
          weight_old.swap(weight_new);
        }

        iter++;
      }
    }

    /// Apply classifier. return a label
    double operator()( const point_t& p )
    {
    }
  };

}

#endif /* _LOGISTIC_REGRESSION_HH */

