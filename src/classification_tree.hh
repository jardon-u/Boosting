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
# include "tree.hh"
# include "observation.hh"


namespace classification
{

  ///
  /// \brief Classification tree
  /// Template parameter:
  /// typename \p T, point.
  /// typename \p INDEX, index policy.
  ///
  template <typename T, typename INDEX>
  struct classification_tree
  {
    typedef T point; ///< point type.
    typedef typename T::value_type value; ///< point coordinate type.

    // Not implemented with contiguous storage (which is needed).
    typedef std::deque< observation<const point*,int,double> > V;

    /// Ctor
    classification_tree()
      : tree_(0) {}

    /// Dtor
    virtual ~classification_tree();

    //// Member functions

    /// Fit classifier to data
    /// \p x_ data
    /// \p y_ labels
    /// \p w_ weights
    void fit( const std::vector<T>& x,
              const std::vector<int>& y,
              std::vector<double>& w);

    /// Recursively split region
    tree<point> * split( V v, int depth = 0 );

    /// Get splitting dimension j and threshold s
    void get_splitting( unsigned& j, value& s, const V& v );

    /// Get majority label in vector v
    double get_label( V& v );

    /// Apply classifier. return a label
    double operator()( const point& p );

    //// Internal
    classification_tree( const classification_tree& rh );
    classification_tree& operator=( const classification_tree& rh );

    static const std::string name()
    {
      return std::string("Classification tree (") + INDEX::name() + ")";
    }

    // Check
    bool all_equals( V &v );

    tree<point> * tree_; ///< tree
    enum { depth_limit = 2 };
    enum { max_node_size = 3 };
    enum { nb_cat = 2 };
  };

#   include "classification_tree.hxx"

} // end of namespace classification

#endif // slt_weak_classification_tree_h__
