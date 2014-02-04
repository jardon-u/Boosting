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

    /// Fit classifier to data
    /// \p x_ data
    /// \p y_ labels
    /// \p w_ weights
    void fit( const std::vector<point_t>& x,
              const std::vector<label_t>& y,
              std::vector<weight_t>&      w )
    {
    }

    /// Apply classifier. return a label
    double operator()( const point_t& p )
    {
    }
  };

}

#endif /* _LOGISTIC_REGRESSION_HH */

