// Ugo Jardonnet All Rights Reserved.
// Author: Ugo Jardonnet

/// Implementation -

namespace make
{
  template <typename Container>
  inline
  std::vector<double>
  max(const Container& v)
  {
    std::vector<double> max(*v[0].x);

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

  template <typename C>
  inline
  std::vector<double>
  min(const C& v)
  {
    std::vector<double> min(*v[0].x);

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

  //FIXME: make generic and pull out
  template <typename C>
  inline
  std::pair<std::vector<double>, std::vector<double>>
  min_max(const C& v)
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

}

template < typename T, typename INDEX, template <class, class> class C >
inline
void
classification_tree<T,INDEX,C>::fit(const std::vector<T>&   x,
                                    const std::vector<int>& y,
                                    std::vector<double>&    w)
{
  //Group data // FIXME: v should be the unique argument?
  obs_t v;
  for (std::size_t i = 0; i < x.size(); i++)
  {
    if (w[i] != w[i] || w[i] == 0)
      throw std::runtime_error("error w[i] wrong");
    v.push_back(observation<const T*,int,double>(&x[i],y[i],w[i]));
  }

  // Create tree
  std::cout << "ct: Fitting " << v.size() << " observations" << std::endl;
  tree_ = split(v);
}

template < typename T, typename INDEX, template <class, class> class C >
inline
double classification_tree<T,INDEX,C>::variance(double sum, double sum2, int n)
{
  double mean = sum / n;
  return (sum2 - sum * mean) / n;
}

template < typename T, typename INDEX, template <class, class> class C >
inline
void
classification_tree<T,INDEX,C>::get_splitting(unsigned&     j,
                                              value_t&      s,
                                              const obs_t&  v)
{
  double max_gain = std::numeric_limits<double>::max();

  auto minmax = make::min_max(v);
  std::vector<double>& min = minmax.first;
  std::vector<double>& max = minmax.second;
  std::vector<double> offsets(max.size(), 0);

  for (std::size_t i = 0; i < min.size(); i++)
    offsets[i] = (max[i] - min[i]) / v.size(); // offset_ratio;

  for (std::size_t dim = 0; dim < (*v[0].x).size(); dim++) // dim <=> j
  {
    //std::cout << "dim " << dim <<
    //  " min: " << min[dim] << " max: " << max[dim] << " " << v.size() << std::endl;

    // no variation on this dimension.
    if (offsets[dim] < std::numeric_limits<double>::epsilon())
      continue;

    std::size_t nb_slices = 100; // v.size(); // weak heuristic
    std::vector< std::array<double,3> > slice(nb_slices, std::array<double,3>{{0,0,0}});
    double total_sum  = 0;
    double total_sum2 = 0;

    /// This piece of code basically sorts observations on dimension /dim/ with
    /// a bucket sort.
    for (std::size_t i = 0; i < v.size(); i++)
    {
      const label_t& y = v[i].y;

      //FIXME: always the same for any dimension
      total_sum  += y;
      total_sum2 += y * y;

      int bucket = ((*v[i].x)[dim] - min[dim]) /
        (double)((max[dim] - min[dim])) * (slice.size() - 1);

      slice[bucket][0] += y;
      slice[bucket][1] += y * y;
      slice[bucket][2] += 1;

      if (0)
      {
        // bucket label position
        std::cout << bucket << " " << y << " " << (*v[i].x)[dim] << std::endl;
      }
    } // end for each observations

    /// Here we iterate on dimension /dim/ from min to max. Each slice contains
    /// the sum and sum2 of labels for this slice as well as the number of
    /// observation in this slice. Summing from left to right we can compute
    /// variance on the left and right of the current slice (and so the
    /// threshold).
    double left_sum  = 0;
    double left_sum2 = 0;
    int    nb_left   = 0;
    for (std::size_t i = 0; i < slice.size() - 1; i++)
    {
      left_sum  += slice[i][0];
      left_sum2 += slice[i][1];
      nb_left   += slice[i][2];
      double vleft  = variance(left_sum,  left_sum2, nb_left);

      double right_sum  = total_sum  - left_sum;
      double right_sum2 = total_sum2 - left_sum2;
      double vright = variance(right_sum, right_sum2, v.size() - nb_left);

      double gain = vleft + vright;

      // Save j and s associated to the minimum sum
      //FIXME: we want to maximize margin here
      // i.e. when consecutive slices have the same gain
      // we want to take the threshold in the middle.
      if (gain <= max_gain)
      {
        max_gain = gain;
        j = dim;
        s =  min[dim] + ((i+1) / (double) slice.size()) * (max[dim] - min[dim]);

        if (0)
        {
          std::cout << "-----------------" << std::endl;
          std::cout << "i  " << i << std::endl;
          std::cout << "s  " << s << std::endl;
          std::cout << "left_sum  " << left_sum  << std::endl;
          std::cout << "left_sum2 " << left_sum2 << std::endl;
          std::cout << vleft << " vleft|vright " << vright << std::endl;
          std::cout << "dim " << dim << " gain: " << max_gain << " -> " << gain << std::endl;
          std::cout << "-----------------" << std::endl;
        }
      }
    } // end for each slices
  } // end for each dims

  std::cerr << "ct: Split (dim,value): (" << j << "," << s << ")" << std::endl;
}


template < typename T, typename INDEX, template <class, class> class C >
inline
auto classification_tree<T,INDEX,C>::get_maj_label(obs_t& v) -> label_t
{
  assert(v.size() != 0 && "Get label failed");

  //FIXME: Pull this code out

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
    return 0; //FIXME: other strategy when no majority

  // Only valid for the two classes (1,-1) problem
  //double s = 0;
  //for (auto e : v)
  //  s += e.y;

  //return s / v.size();

//   unsigned count[nb_cat];

//   for (unsigned i = 0; i < v.size(); i++)
//     count[v[i].y]++;

//   unsigned imax = 0;
//   unsigned  max = 0;
//   for (unsigned c = 0; c < nb_cat; c++)
//   {
//     if (count[c] > max)
//     {
//       imax = c;
//       max = count[c];
//     }
//   }

//   return imax;
}


template < typename T, typename INDEX, template <class, class> class C >
inline
tree<T> *
classification_tree<T,INDEX,C>::split(obs_t v, unsigned depth)
{
  //std::cerr << "ct: split vector size "<< v.size() << std::endl;
  if (v.size() == 0)
    return 0;

  if (all_labels_equals(v))
    return new tree<point_t>( true_lambda, v[0].y );

  label_t majority_label = get_maj_label(v);
  if (v.size() <= max_node_size || depth >= depth_limit)
    return new tree<point_t>( true_lambda, majority_label );

  size_t  j; // dimension to split
  value_t s; // splitting point
  get_splitting(j, s, v);

  auto splitting_lambda = [j,s](point_t x)->bool{ return x[j] <= s; };
  tree<point_t> * node = new tree<point_t>(splitting_lambda);

  // Split v (copy to new vectors IS an optimization)
  obs_t v1, v2;
  for (auto e : v)
  {
    if (node->f(*e.x))
      v1.push_back(e);
    else
      v2.push_back(e);
  }

  //std::cerr << "ct: split => [" << v1.size() << "|" << v2.size() << std::endl;

  // One of the Branch is empty. 2 classes exist.
  // Node may be under the min size but labels was == 0.
  if (v1.size() == 0 || v2.size() == 0)
  {
    assert (v1.size() != v2.size());

    //std::cerr << "(" << j << "," << s << ")" << std::endl;

    if (majority_label == 0)
    {
      std::cerr << "(" << v1.size() << " ; " << v2.size() << ")" << std::endl;
      std::cerr << "-- " << majority_label <<  " vsize " << v.size() << std::endl;
      //for(unsigned i = 0; i < v.size(); i++)
      //  std::cerr << v[i].y  << " " << (*v[i].x)  << std::endl;
      std::cerr << "---------" << std::endl;
      // WARNING: We are not able to split. Node is randomly classified.
      majority_label = v[0].y;
    }
    else // no ? (ie. labels are different and splitting didn't split anything)
      throw std::runtime_error("Unexpected: label!=0, splitting failed");

    return new tree<point_t>( true_lambda, majority_label );
  }

  v.clear(); // keep memory usage +/- constant

  // Recursively launch sub-splitting
  node->ttrue  = split(v1, depth+1);
  node->tfalse = split(v2, depth+1);

  return node;
}


template < typename T, typename INDEX, template <class, class> class C >
inline auto
classification_tree<T,INDEX,C>::operator()(const point_t& p)
-> double
{
  assert(tree_ != 0 && "Classification failure");
  tree<point_t> * region = tree_->get_region(p);

  //if (region->label == 0)
  //  throw std::runtime_error("0 Classified");
  if (region->label != region->label) // check for NaN value
    throw std::runtime_error("NaN value during classification");

  return region->label;
}


template < typename T, typename INDEX, template <class, class> class C >
inline auto
classification_tree<T,INDEX,C>::all_labels_equals( obs_t &v )
-> bool
{
  label_t first = v[0].y;
  for (auto e : v)
  {
    if (e.y != first)
      return false;
  }
  return true;
}


template < typename T, typename INDEX, template <class, class> class C >
inline
classification_tree<T,INDEX,C>::
classification_tree(const classification_tree<T,INDEX,C>& rh)
  :  tree_(nullptr),
     depth_limit(rh.depth_limit),
     max_node_size(rh.max_node_size),
     nb_cat(rh.nb_cat),
     offset_ratio(rh.offset_ratio)
{
  if (rh.tree_ != 0)
    tree_ = new tree<point_t>(*rh.tree_);
}


template < typename T, typename INDEX, template <class, class> class C >
inline auto
classification_tree<T,INDEX,C>::
operator=(const classification_tree<T,INDEX,C>& rh)
-> classification_tree<T,INDEX,C>&
{
  depth_limit   = rh.depth_limit;
  max_node_size = rh.max_node_size;
  nb_cat        = rh.nb_cat;
  offset_ratio  = rh.offset_ratio;

  if (this != &rh)
  {
    if (rh.tree_ != 0)
      tree_  = new tree<point_t>(*rh.tree_);
  }
  return *this;
}


template < typename T, typename INDEX, template <class, class> class C >
inline
classification_tree<T,INDEX,C>::~classification_tree()
{
  delete tree_;
}
