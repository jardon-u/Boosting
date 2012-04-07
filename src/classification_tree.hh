// Ugo Jardonnet All Rights Reserved.
// Author: Ugo Jardonnet

#ifndef slt_weak_classification_tree_h__
# define slt_weak_classification_tree_h__

# include <vector>
# include <iostream>
# include <utility>
# include <limits>
# include <deque>
# include <cassert>
# include <cmath>

# include "boosting.hh"
# include "tree.hh"
# include "observation.hh"


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
    classification_tree(unsigned depth_limit   = 2,
                        unsigned max_node_size = 3,
                        unsigned nb_cat        = 2,
                        double   offset_ratio  = 10.)
      : tree_(nullptr),
        depth_limit(depth_limit),
        max_node_size(max_node_size),
        nb_cat(nb_cat),
        offset_ratio(offset_ratio)
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
    tree<point_t> * split( obs_t v, unsigned depth = 0 );

    /// Get splitting dimension j and threshold s
    void get_splitting( size_t& j, value_t& s, const obs_t& v );

    /// Get majority label in vector v
    label_t get_label( obs_t& v );

    /// Apply classifier. return a label
    double operator()( const point_t& p );

    //// Internal
    classification_tree( const classification_tree& rh );
    classification_tree& operator=( const classification_tree& rh );
    static const std::string name() {
      return std::string("Classification tree (") + INDEX::name() + ")";
    }

    // Check
    bool all_equals( obs_t &v );

  private:
    tree<point_t> * tree_; ///< underlying tree

    unsigned depth_limit;   ///< max depth of the tree
    unsigned max_node_size; ///< max observation per node
    unsigned nb_cat;        ///< number of classes

    const double offset_ratio; //< determine offset when exploring dimension
  };

#   include "classification_tree.hxx"

} // end of namespace classification

#endif // slt_weak_classification_tree_h__
