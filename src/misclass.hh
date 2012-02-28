// Ugo Jardonnet All Rights Reserved.
// Author: Ugo Jardonnet

#ifndef slt_misclass_h__
# define slt_misclass_h__

# include "observation.hh"

namespace classification
{

  // This is a fully static object.
  // It pacts together a bunch of function (See c++ policy).

  /// \brief Misclass Index Policy
  /// Splitting based on misclassification error.
  struct misclass
  {
    /// Compute the weighted index summation for label 1 and -1.
    /// \p set original set of observation.
    /// \p fun subset function
    template <typename S, typename T>
    static double compute(const S& set, const std::function<bool(T)>& fun)
    {
      //f_not<T> notfun(fun);
      std::function<bool(T)> notfun = [fun](T t)->bool { return !fun(t); };
      double one = (index(set,fun, 1) + index(set, notfun, -1)) / set.size();
      double two = (index(set,fun, -1) + index(set, notfun, 1)) / set.size();
      return std::min(one,two);
    }

    /// Compute the index of one subset.
    template <typename S, typename F>
    static double index(const S& v, const F& fun, int label)
    {
      unsigned  n = 0;
      double    k = 0;
      double wsum = 0;
      for (unsigned i = 0; i < v.size(); i++)
      {
        if (fun(*v[i].x))
        {
          k  += v[i].w * static_cast<int>(v[i].y != label);
          n++;
        }
        wsum += v[i].w;
      }
      return (n * k) / wsum;
    }

    /// Return the name of the index class.
    static const std::string name()
    {
      return "Misclass";
    }
  };

} // end of namespace classification

#endif // slt_misclass_h__
