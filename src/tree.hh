// Ugo Jardonnet All Rights Reserved.
// Author: Ugo Jardonnet

#ifndef tree_h__
# define tree_h__

# include <vector>
# include <stdexcept>
# include <functional>

# include "tree.hh"

// core directories don't follow the namespace rule

namespace classification
{

  /// \brief tree
  /// Node are labeled with functions
  /// \p type of value
  template <typename T>
  class tree : public std::enable_shared_from_this<tree<T>>
  {
  public:
    /// Copy Ctor
    tree(const tree& t);

    /// Ctor with label
    /// \p fun Splitting function
    /// \p label
    tree(const std::function<bool(T)> fun, double label);

    /// Ctor
    /// \p fun Splitting function
    tree(const std::function<bool(T)> fun);

    /// return pointer deepest node holding \p p.
    std::shared_ptr<tree> get_region(const T& p);

    /// Assignment operator performing deep copy.
    tree& operator=(const tree& t);

    /// Dtor
    ~tree();

    const std::function<bool(T)> f; ///< Splitting function.
    std::shared_ptr<tree> ttrue;    ///< Map the subset f(p).
    std::shared_ptr<tree> tfalse;   ///< Map the subset !f(p).
    double label;     ///< label of the branch.
  };


  /// Implementation -

  template <typename T>
  tree<T>::tree(const std::function<bool(T)> fun)
    :  f(fun), ttrue(nullptr), tfalse(nullptr), label(0)
  {	}

  template <typename T>
  tree<T>::tree(const std::function<bool(T)> fun, double label_)
    :  f(fun), ttrue(nullptr), tfalse(nullptr), label(label_)
  {	}


  // template <typename T>
  // inline
  // tree<T>::tree( const tree& rh )
  //   : f(rh.f), ttrue(nullptr), tfalse(nullptr)
  // {
  //   label = rh.label;
  //   if (rh.ttrue != nullptr)
  //     ttrue  = new tree(*rh.ttrue);
  //   if (rh.tfalse != nullptr)
  //     tfalse = new tree(*rh.tfalse);
  // }


  template <typename T>
  inline
  std::shared_ptr<tree<T>> tree<T>::get_region( const T& p )
  {
    std::shared_ptr<tree> t = tree<T>::shared_from_this();

    while (t != nullptr && (t->ttrue != nullptr || t->tfalse != nullptr))
    {
      t = ((t->f)(p)) ? t->ttrue : t->tfalse;
    }
    if (t == nullptr)
      throw std::runtime_error("get_region failed");

    return t;
  }


  template <typename T>
  inline
  tree<T>& tree<T>::operator=( const tree& rh )
  {
    if (&rh != this)
    {
      //tree * tmp_true  = nullptr;
      //tree * tmp_false = nullptr;

      f = rh.f;
      // try {
      //   if (rh.ttrue != nullptr)
      //     tmp_true  = new tree(*rh.ttrue);
      //   if (rh.tfalse != nullptr)
      //     tmp_false = new tree(*rh.tfalse);
      // } catch (...) {
      //   // Mandatory if tmp_true creation succeed but tmp_false failed
      //   delete tmp_true;
      //   delete tmp_false;
      //   throw;
      // }

      // delete ttrue;
      // delete tfalse;

      ttrue  = std::move(rh.ttrue);
      tfalse = std::move(rh.tfalse);
      label  = rh.label;
    }
    return *this;
  }

  template <typename T>
  inline
  tree<T>::~tree()
  {
    //delete tfalse;
    //delete ttrue;
  }

} // end of namespace classification

#endif // tree_h__
