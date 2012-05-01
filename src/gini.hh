// Ugo Jardonnet All Rights Reserved.
// Author: Ugo Jardonnet

#ifndef _GINI_HH
# define _GINI_HH

# include "observation.hh"
# include <functional>

namespace classification
{

  // This is a fully static object.
  // It pacts together a bunch of function (See c++ policy).

  /// \brief Misclass Index Policy
  /// Splitting based on misclassification error.
  struct gini
  {
    /// Compute the weighted index summation for label 1 and -1.
    /// \p set original set of observation.
    /// \p fun subset function
    template <typename S, typename T>
    static double compute(const S& set, const std::function<bool(T)>& fun,
                          unsigned nb_cat)
    {
      double left  = index(set, fun,                    nb_cat);
      double right = index(set, std::negate<bool>(fun), nb_cat);
      return left + right;
    }

    /// Compute the index of one subset.
    template <typename S, typename F>
    static double index2(const S& v, const F& fun, unsigned nb_cat)
    {
      //normalization factor
      float total_count = 0;
      for (unsigned i = 0; i < v.size(); i++)
        if (fun(*v[i].x))
          total_count += v[i].w;
      float total_count2 = total_count * total_count;

      //cat count
      float count[10] = {0};
      for (unsigned i = 0; i < v.size(); i++)
        if (fun(*v[i].x))
        {
          count[v[i].y] += v[i].w;
        }

      //compute gini index
      float gini = 0;
      if (total_count2)
      {
        for (unsigned i = 1; i < nb_cat; i++)
          for (unsigned j = i+1; j<= nb_cat; j++)
            gini += count[i] * count[j] / total_count2;
      }

      return gini;
    }

    /// Return the name of the index class.
    static const std::string name()
    {
      return "Gini index";
    }
  };

} // end of namespace classification


#endif /* _GINI_HH */

