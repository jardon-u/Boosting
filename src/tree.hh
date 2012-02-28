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
    :  f(fun), ttrue(0), tfalse(0), label(0)
  {	}

  template <typename T>
  tree<T>::tree(const std::function<bool(T)> fun, double label_)
    :  f(fun), ttrue(0), tfalse(0), label(label_)
  {	}


  template <typename T>
  inline
  tree<T>::tree( const tree& rh )
    : f(rh.f), ttrue(0), tfalse(0)
  {
    label = rh.label;
    if (rh.ttrue != 0)
      ttrue  = new tree(*rh.ttrue);
    if (rh.tfalse != 0)
      tfalse = new tree(*rh.tfalse);
  }


  template <typename T>
  inline
  tree<T> * tree<T>::get_region( const T& p )
  {
    tree * t = this;

    while (t != 0 && (t->ttrue != 0 || t->tfalse != 0))
    {
      if ((t->f)(p))
        t = t->ttrue;
      else
        t = t->tfalse;
    }
    if (t == 0)
      throw std::runtime_error("get_region failed");

    return t;
  }


  template <typename T>
  inline
  tree<T>& tree<T>::operator=( const tree& t )
  {
    if (&t != this)
    {
      tree * tmp_true  = 0;
      tree * tmp_false = 0;
      std::function<bool(T)> * tmp_fun = 0;

      try {
        if (t.ttrue != 0)
          tmp_true  = new tree(*t.ttrue);
        if (t.tfalse != 0)
          tmp_false = new tree(*t.tfalse);
        if (t.fun != 0)
          tmp_fun   = t.f->clone(); // virtual constructor idiom
      } catch (...) {
        // Mandatory if tmp_true creation succeed but tmp_false failed
        delete tmp_true;
        delete tmp_false;
        delete tmp_fun;
        throw;
      }

      delete ttrue;
      delete tfalse;
      delete f;

      ttrue  = tmp_true;
      tfalse = tmp_false;
      f      = tmp_fun;
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
