// Ugo Jardonnet All Rights Reserved.
// Author: Ugo Jardonnet

#ifndef slt_weak_classification_tree_h__
# define slt_weak_classification_tree_h__

# include <vector>
# include <array>
# include <iostream>
# include <utility>
# include <limits>
# include <deque>
# include <cassert>
# include <cmath>
# include <future>

# include "boosting.hh"
# include "tree.hh"
# include "observation.hh"
# include "math.hh"


namespace classification
{

  ///
  /// \brief Classification tree
  /// Template parameter:
  /// typename \p T, point type.
  /// typename \p INDEX, index policy.
  /// typename \p C, template template STL container
  ///
  template < typename T,
             typename INDEX,
             template < class U, class V = std::allocator<U> >
             class C = std::deque >
  struct classification_tree
  {
    typedef int    label_t;  ///< label type.
    typedef double weight_t; ///< weight type.
    typedef T      point_t;  ///< point type.
    typedef typename T::value_type value_t; ///< point coordinate type.

    typedef C< observation<const point_t*, label_t, weight_t> > obs_t;

    std::function<bool(point_t)> true_lambda;

    /// Ctor
    classification_tree(unsigned depth_limit   = 3,
                        unsigned max_node_size = 3,
                        unsigned nb_cat        = 2,
                        unsigned nb_slices     = 1000.)
      : tree_(nullptr),
        depth_limit(depth_limit),
        max_node_size(max_node_size),
        nb_cat(nb_cat),
        nb_slices(nb_slices)
    {
      true_lambda = [](point_t)->bool { return true; };
    }

    /// Dtor
    ~classification_tree();

    //// Member functions

    /// Fit classifier to data
    /// \p x_ data
    /// \p y_ labels
    /// \p w_ weights
    void fit( const std::vector<point_t>& x,
              const std::vector<label_t>& y,
              std::vector<weight_t>&      w );

    /// Recursively split region
    std::shared_ptr<tree<point_t>> split( obs_t v, unsigned depth = 0 );

    /// Get splitting dimension j and threshold s
    void get_splitting( size_t& j, value_t& s, const obs_t& v );

    /// Apply classifier. return a label
    double operator()( const point_t& p );

    /// Get the current bucket for an observation
    template < typename M >
    double get_bucket(const M&                                 minmax,
                      const std::size_t&                       dim,
                      const std::vector<std::array<double,3>>& slice,
                      const typename obs_t::value_type&        vi);

    //// Internal
    classification_tree( const classification_tree& rh );
    classification_tree& operator=( const classification_tree& rh );
    static const std::string name() {
      return std::string("Classification tree (") + INDEX::name() + ")";
    }

    // Check
    bool all_labels_equals( obs_t &v );

  private:
    double variance(double sum, double sum2, int n);

    std::shared_ptr<tree<point_t>> tree_; ///< underlying tree

    unsigned depth_limit;   ///< max depth of the tree
    unsigned max_node_size; ///< max observation per node
    unsigned nb_cat;        ///< number of classes

    const unsigned nb_slices; //< determine nb slices when exploring dimension
  };

#   include "classification_tree.hxx"

} // end of namespace classification

#endif // slt_weak_classification_tree_h__
