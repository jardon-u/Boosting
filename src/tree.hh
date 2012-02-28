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
  class tree
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
    tree * get_region(const T& p);

    /// Assignment operator performing deep copy.
    tree& operator=(const tree& t);

    /// Dtor
    ~tree();

    const std::function<bool(T)> f; ///< Splitting function.
    tree * ttrue;     ///< Map the subset f(p).
    tree * tfalse;    ///< Map the subset !f(p).
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


  template <typename T>
  inline
  tree<T>::tree( const tree& rh )
    : f(rh.f), ttrue(nullptr), tfalse(nullptr)
  {
    label = rh.label;
    if (rh.ttrue != nullptr)
      ttrue  = new tree(*rh.ttrue);
    if (rh.tfalse != nullptr)
      tfalse = new tree(*rh.tfalse);
  }


  template <typename T>
  inline
  tree<T> * tree<T>::get_region( const T& p )
  {
    tree * t = this;

    while (t != nullptr && (t->ttrue != nullptr || t->tfalse != nullptr))
    {
      if ((t->f)(p))
        t = t->ttrue;
      else
        t = t->tfalse;
    }
    if (t == nullptr)
      throw std::runtime_error("get_region failed");

    return t;
  }


  template <typename T>
  inline
  tree<T>& tree<T>::operator=( const tree& t )
  {
    if (&t != this)
    {
      tree * tmp_true  = nullptr;
      tree * tmp_false = nullptr;

      f = t.f;
      try {
        if (t.ttrue != nullptr)
          tmp_true  = new tree(*t.ttrue);
        if (t.tfalse != nullptr)
          tmp_false = new tree(*t.tfalse);
      } catch (...) {
        // Mandatory if tmp_true creation succeed but tmp_false failed
        delete tmp_true;
        delete tmp_false;
        throw;
      }

      delete ttrue;
      delete tfalse;

      ttrue  = tmp_true;
      tfalse = tmp_false;
      label  = t.label;
    }
    return *this;
  }

  template <typename T>
  inline
  tree<T>::~tree()
  {
    delete tfalse;
    delete ttrue;
  }

} // end of namespace classification

#endif // tree_h__
