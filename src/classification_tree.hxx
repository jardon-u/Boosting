// Ugo Jardonnet All Rights Reserved.
// Author: Ugo Jardonnet


/// Implementation -


namespace make
{
  template <typename T>
  inline
  std::vector<double>
  max(const std::deque<T>& v)
  {
    std::vector<double> max(*v[0].x);

    unsigned dim = (v[0].x)->size();
    for (unsigned i = 0; i < v.size(); i++)
    {
      for (unsigned j = 0; j < dim; j++)
      {
        if ((*v[i].x)[j] > max[j])
          max[j] = (*v[i].x)[j];
      }
    }

    return max;
  }

  template <typename T>
  inline
  std::vector<double>
  min(const std::deque<T>& v)
  {
    std::vector<double> min(*v[0].x);

    unsigned dim = (v[0].x)->size();
    for (unsigned i = 0; i < v.size(); i++)
    {
      for (unsigned j = 0; j < dim; j++)
      {
        if ((*v[i].x)[j] < min[j])
          min[j] = (*v[i].x)[j];
      }
    }

    return min;
  }

}

template <typename T, typename INDEX>
inline
void classification_tree<T,INDEX>::fit(const std::vector<T>& x,
                                       const std::vector<int>& y,
                                       std::vector<double>& w)
{
  //Group data // FIXME: v should be the unique argument.
  V v;
  for (std::size_t i = 0; i < x.size(); i++)
  {
    if (w[i] != w[i] || w[i] == 0)
      throw std::runtime_error("error w[i] wrong");
    v.push_back(observation<const T*,int,double>(&x[i],y[i],w[i]));
  }

  // Create tree
  tree_ = split(v);
}


template <typename T, typename INDEX>
inline
void classification_tree<T,INDEX>::get_splitting(unsigned& j, value& s, const V& v)
{
  double max_gain = std::numeric_limits<double>::max();

  std::vector<double> min = make::min(v);
  std::vector<double> max = make::max(v);
  std::vector<double> offset(max);
  for (unsigned i = 0; i < min.size(); i++)
    offset[i] = (max[i] - min[i]) / 100.;
  for (unsigned dim = 0; dim < offset.size(); dim++) // dim <=> j
  {
    //std::cout << offset[dim] << std::endl;
    // no variation on this dimension.
    if (offset[dim] < std::numeric_limits<double>::epsilon())
      continue;

    for (value s_ = min[dim] - offset[dim];
         s_ <= max[dim] + offset[dim]; // Inclusion of the last point
         s_ += offset[dim])
    {
      // Compute sum
      f_inf<point> fun(dim,s_);
      double gain = INDEX::compute(v,fun/*, nb_cat*/);

      // Save j and s associated to the minimum sum
      if (gain < max_gain) // this test should be part of INDEX
      {
        max_gain = gain;
        j = dim;
        s = s_;
      }
    }
  }
  std::cerr << "split (" << j << ", " << s << ')' << std::endl;
}


template <typename T, typename INDEX>
inline
double classification_tree<T,INDEX>::get_label(V& v)
{
  assert(v.size() != 0 && "Get label failed");

  // Only valid for the two classes (1,-1) problem
  double s = 0;
  for (unsigned i = 0; i < v.size(); i++)
    s += v[i].y;

  return s / v.size();

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


template <typename T, typename INDEX>
inline
tree<T> * classification_tree<T,INDEX>::split(V v, int depth)
{
  unsigned    j = 0; // dim (orientation)
  value       s = (*v[0].x)[0]; // s_  (split point)

  //std::cerr << "## "<<  v.size() << std::endl;

  // No point
  if (v.size() == 0)
    return 0;

  // If all labels are equal do not split
  if (all_equals(v))
    return new tree<point>(new f_true<point>(), static_cast<double>(v[0].y));

  unsigned label = get_label(v);

  // If only few points remaining do not split
  if (v.size() <= max_node_size || depth >= depth_limit)
    return new tree<point>(new f_true<point>(), label);

  get_splitting(j, s, v);
  //std::cout << "Splitting done." << std::endl;

  // Construct point subsets
  tree<point> * t = new tree<point>(new f_inf<point>(j, s));
  V v1, v2; // Comment: the use of these vectors IS an optimization.
  //std::cerr << "Start sub vector allocation: {";
  for (unsigned i = 0; i < v.size(); i++)
  {
    if ((*t->f)(*v[i].x))
      v1.push_back(v[i]);
    else
      v2.push_back(v[i]);
  }
  //std::cerr << "}" << std::endl;

  std::cerr << "(" << v1.size() << " ; " << v2.size() << ")" << std::endl;

  // One of the Branch is empty. 2 classes exist.
  // Node may be under the min size but labels was == 0.
  if (v1.size() == 0 || v2.size() == 0)
  {
    assert (v1.size() != v2.size());

    //std::cerr << "(" << j << "," << s << ")" << std::endl;

    if (label == 0)
    {
      std::cerr << "(" << v1.size() << " ; " << v2.size() << ")" << std::endl;
      std::cerr << "-- " << label <<  " vsize " << v.size() << std::endl;
      //for(unsigned i = 0; i < v.size(); i++)
      //  std::cerr << v[i].y  << " " << (*v[i].x)  << std::endl;
      std::cerr << "---------" << std::endl;
      // WARNING: We are not able to split. Node is randomly classified.
      label = static_cast<double>(v[0].y);
    }
    else //else may happen? (ie. labels are different and splitting didn't split anything)
      throw std::runtime_error("Unexpected: label!=0, splitting failed");

    return new tree<point>(new f_true<point>(), label);
  }

  v.clear(); // keep memory usage +/- constant

  // Recursively launch sub-splitting
  t->ttrue  = split(v1, depth+1);
  t->tfalse = split(v2, depth+1);

  // Return split node
  return t;
}


template <typename T, typename INDEX>
inline
double classification_tree<T,INDEX>::operator()(const point& p)
{
  assert(tree_ != 0 && "Classification failure");
  tree<point> * region = tree_->get_region(p);

  //if (region->label == 0)
  //  throw std::runtime_error("0 Classified");
  if (region->label != region->label) // check for NaN value
    throw std::runtime_error("NaN value during classification");

  return region->label;
}


template <typename T, typename INDEX>
bool classification_tree<T,INDEX>::all_equals( V &v )
{
  for (unsigned i = 1; i < v.size(); i++)
  {
    if (v[i].y != v[0].y)
      return false;
  }
  return true;
}


template <typename T, typename INDEX>
inline
classification_tree<T,INDEX>::classification_tree(const classification_tree<T,INDEX>& rh)
   :  tree_(0)
{
  if (rh.tree_ != 0)
    tree_ = new tree<point>(*rh.tree_);
}


template <typename T, typename INDEX>
inline
classification_tree<T,INDEX>&
classification_tree<T,INDEX>::operator=(const classification_tree<T,INDEX>& rh)
{
  if (this != &rh)
  {
    if (rh.tree_ != 0)
      tree_  = new tree<point>(*rh.tree_);
  }
  return *this;
}


template <typename T, typename INDEX>
inline
classification_tree<T,INDEX>::~classification_tree()
{
  delete tree_;
}
