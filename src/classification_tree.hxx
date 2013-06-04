// Ugo Jardonnet All Rights Reserved.
// Author: Ugo Jardonnet

/// Implementation -

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
template < typename M >
inline
double classification_tree<T,INDEX,C>::
get_bucket(const M&                                 minmax,
           const std::size_t&                       dim,
           const std::vector<std::array<double,3>>& slice,
           const typename obs_t::value_type&        vi)
{
  if (0)
  {
    std::cout << "(" << (*vi.x)[dim] << "-" << minmax.first[dim] << ") / " <<
      "(" << minmax.second[dim] << "-" << minmax.first[dim] << ")" <<
      " * (" << slice.size() << "- 1)" << std::endl;
  }

  // assume minmax.first[dim] != minmax.second[dim]
  int bucket = ((*vi.x)[dim] - minmax.first[dim]) /
    (double)((minmax.second[dim] - minmax.first[dim])) * (slice.size() - 1);

  return bucket;
}


template < typename T, typename INDEX, template <class, class> class C >
inline
void
classification_tree<T,INDEX,C>::get_splitting(std::size_t&  j,
                                              value_t&      s,
                                              const obs_t&  v)
{
  double max_gain = std::numeric_limits<double>::max();

  auto minmax = classification::math::min_max(v);
  const std::vector<double>& min = minmax.first;
  const std::vector<double>& max = minmax.second;
  //std::vector<double> offsets(max.size(), 0);

  //for (std::size_t i = 0; i < min.size(); i++)
  //  offsets[i] = (max[i] - min[i]) / v.size();
    //offsets[i] = (max[i] - min[i]) / offset_ratio;

  // Compute total label sum and sum2 for all the observation in v
  double total_sum  = 0;
  double total_sum2 = 0;
  for (auto vi : v)
  {
    auto y  = vi.y * vi.w;
    total_sum  += y;
    total_sum2 += y * y;
  }

  for (std::size_t dim = 0; dim < (*v[0].x).size(); dim++) // dim <=> j
  {
    //std::cout << "dim " << dim <<
    //  " min: " << min[dim] << " max: " << max[dim] << " " << v.size() << std::endl;

    // no variation on this dimension.
    //if (offsets[dim] < std::numeric_limits<double>::epsilon())
    //  continue;
    if (min[dim] == max[dim])
      continue;

    std::vector< std::array<double,3> > slice(nb_slices, std::array<double,3>{{0,0,0}});

    /// This piece of code basically sorts observations on dimension /dim/ with
    /// a bucket sort.
    for (auto vi : v)
    {
      auto y  = vi.y * vi.w;

      int bucket = get_bucket(minmax, dim, slice, vi);
      slice[bucket][0] += y;
      slice[bucket][1] += y * y;
      slice[bucket][2] += 1;

      if (0)
      {
        // bucket label position
        std::cout << bucket << " " << y << " " << (*vi.x)[dim] << std::endl;
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
tree<T> *
classification_tree<T,INDEX,C>::split(obs_t v, unsigned depth)
{
  //std::cerr << "ct: split vector size "<< v.size() << std::endl;
  if (v.size() == 0)
    return 0;

  if (all_labels_equals(v))
    return new tree<point_t>( true_lambda, v[0].y );

  label_t majority_label = math::majority_label(v);
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
     nb_slices(rh.nb_slices)
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
  nb_slices     = rh.nb_slices;

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
