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
  //FIXME: C++11 signature style is not super readable... needed for get_label
  //FIXME: maybe use classic style for prototype..

  ///
  /// \brief Classification tree
  /// Template parameter:
  /// typename \p T, point type.
  /// typename \p INDEX, index policy.
  /// typename \p OBS, Observation vector type.
  ///
  template < typename T,
             typename INDEX,
             typename OBS = std::deque< observation<const T*, int, double> >
            // deque Not implemented with contiguous storage (which is needed).
            >
  struct classification_tree
  {
    typedef int    label_t;  ///< label type.
    typedef double weight_t; ///< weight type.
    typedef T      point_t;  ///< point type.
    typedef typename T::value_type value_t; ///< point coordinate type.

    //OBS could be replaced by a template template parameter Container.
    //But it seems not easily compatible with the STL.
    //typedef Container< observation<const point_t*, label_t, weight_t> > obs_t;
    typedef OBS obs_t;

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
    void fit( const std::vector<point_t>& x,
              const std::vector<label_t>& y,
              std::vector<weight_t>&      w );

    /// Recursively split region
    auto split( obs_t v, int depth = 0 ) -> tree<point_t> *;

    /// Get splitting dimension j and threshold s
    auto get_splitting( size_t& j, value_t& s, const obs_t& v ) -> void;

    /// Get majority label in vector v
    auto get_label( obs_t& v ) -> label_t;

    /// Apply classifier. return a label
    auto operator()( const point_t& p ) -> double;

    //// Internal
    classification_tree( const classification_tree& rh );
    classification_tree& operator=( const classification_tree& rh );

    static auto name() -> const std::string
    {
      return std::string("Classification tree (") + INDEX::name() + ")";
    }

    // Check
    auto all_equals( obs_t &v ) -> bool;

    tree<point_t> * tree_; ///< tree

    //FIXME: The following must be dynamic
    enum { depth_limit   = 2 };
    enum { max_node_size = 3 };
    enum { nb_cat        = 2 };
  };

#   include "classification_tree.hxx"

} // end of namespace classification

#endif // slt_weak_classification_tree_h__
