#ifndef _MATH_HH
# define _MATH_HH

namespace classification
{

  namespace math
  {

    template <typename OBS>
    inline
    typename OBS::value_type::point_t
    max(const OBS& v)
    {
      typename OBS::value_type::point_t max(*v[0].x);

      for (std::size_t i = 0; i < v.size(); i++)
      {
        for (std::size_t j = 0; j < max.size(); j++)
        {
          if ((*v[i].x)[j] > max[j])
            max[j] = (*v[i].x)[j];
        }
      }

      return max;
    }

    template <typename OBS>
    inline
    typename OBS::value_type::point_t
    min(const OBS& v)
    {
      typename OBS::value_type::point_t min(*v[0].x);

      for (std::size_t i = 0; i < v.size(); i++)
      {
        for (std::size_t j = 0; j < min.size(); j++)
        {
          if ((*v[i].x)[j] < min[j])
            min[j] = (*v[i].x)[j];
        }
      }

      return min;
    }


    template <typename OBS>
    inline
    std::pair<std::vector<double>,
              std::vector<double>>
    min_max(const OBS& v)
    {
      std::vector<double> min(*v[0].x);
      std::vector<double> max(*v[0].x);

      for (std::size_t i = 0; i < v.size(); i++)
      {
        for (std::size_t j = 0; j < min.size(); j++)
        {
          const std::vector<double>& e = *v[i].x;
          if ( e[j] < min[j] )
            min[j] = e[j];
          if ( e[j] > max[j] )
            max[j] = e[j];
        }
      }

      return std::make_pair(min, max);
    }


    template < typename OBS >
    inline
    typename OBS::value_type::label_t
    majority_label(OBS& v)
    {
      std::size_t count = 0;
      std::size_t maj_index;

      for (std::size_t i = 0; i < v.size(); i++) {
        if (count == 0) {
          maj_index = i;
        }
        if (v[i].y == v[maj_index].y)
          count++;
        else
          count--;
      }
      count = 0;
      for (std::size_t i = 0; i < v.size(); i++) {
        if (v[i].y == v[maj_index].y) {
          count++;
        }
      }
      if (count > v.size() / 2)
        return v[maj_index].y;
      else
        return 0; //FIXME: other strategy when no majority ?
    }

  } // end of namespace math

} // end of namespace classification

#endif /* _MATH_HH */

