// Ugo Jardonnet All Rights Reserved.
// Author: Ugo Jardonnet


#ifndef ADABOOST_H
# define ADABOOST_H

# include <cmath>

namespace classification
{

  /// committee method
  template < typename C >
  class adaboost
  {
  public:
    typedef typename C::point_t feature_t;
    typedef typename C::label_t label_t;

    adaboost(const std::vector<feature_t>& x,
             const std::vector<label_t>&   y,
             unsigned nb_iter)
      : M(nb_iter), N(x.size()), x(x), y(y)
    {
      if (N == 0)
        throw std::runtime_error("Learning error: No data");
      err_.resize(N);
      g.reserve(nb_iter);
      alpha_ = std::vector<double>(M, 1);
    }

    void boost()
    {
      std::vector<double> w(N,1./N);
      for (size_t m = 0; m < M; m++)
      {
        g[m].fit(x,y,w);

        err_[m] = error(m,w);
        std::cerr << "Iter " << m << ": " << err_[m] << std::endl;
        if (err_[m] == 0 || err_[m] >= 0.5)
        {
          M = m+1;
          std::cout << "Error mismatch: " << err_[m] << std::endl;
          return;
        }
        alpha_[m] = - 0.5 * std::log((1 - err_[m]) / err_[m]);

        float sum = 0;
        for (size_t i = 0; i < N; i++)
        {
          w[i] = w[i] * std::exp(-1 * alpha_[m] *
                                 static_cast<int>(y[i] != g[m](x[i])));
          sum += w[i];
        }
        for (size_t i = 0; i < N; i++)
          w[i] /= sum;
      }

    }

    double error(int m, const std::vector<double>& w)
    {
      double err = 0;
      for (size_t i = 0; i < N; i++)
        err += w[i] * static_cast<int>(y[i] != g[m](x[i]));
      return err;
    }

    double operator()(const feature_t& x)
    {
      double ret = 0;
      double norm = 0;
      for (unsigned m = 0; m < M; m++)
      {
        ret += alpha_[m] * g[m](x);
        norm += alpha_[m];
      }
      if (ret / norm > 0)
        return 1;
      return -1;
    }

    std::vector<C>      g;
    std::vector<double> alpha_;
    std::vector<double> err_;

    unsigned M, N;

    const std::vector<feature_t>&   x;
    const std::vector<label_t>&     y;
  };


}

#endif
