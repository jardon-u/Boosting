// Ugo Jardonnet All Rights Reserved.
// Author: Ugo Jardonnet

#ifndef fun_h__
# define fun_h__

// core directories don't follow the namespace rule

namespace classification
{

  ////FIXME: followings could be sexier.
  // Use STL Function object adapters.
  // or not ... useful for binding etc... but else..

  template <typename T>
  struct fun
  {
    virtual bool operator()(const T&) const = 0;
    virtual bool operator()(const T&) = 0;
    virtual fun * clone() const = 0;

    // FIXME: Specific to stump functions (f_inf).
    virtual unsigned feature() const = 0;
    virtual typename T::value_type value() const = 0;
  };


  template <typename T>
  struct f_inf : public fun<T>
  {
    f_inf(unsigned dim, typename T::value_type t) //FIXME: Assume type value exists?
      : i(dim), t(t)
    {}

    virtual bool operator()(const T& x) const
    {
      return x[i] <= t;
    }
    virtual bool operator()(const T& x)
    {
      return x[i] <= t;
    }

    virtual typename T::value_type get() const
    {
      return t;
    }

    virtual f_inf * clone() const
    {
      return new f_inf(*this);
    }

    unsigned feature() const
    {
      return i;
    }

    typename T::value_type value() const
    {
      return t;
    }

    unsigned i;
    typename T::value_type t;
  };


  template <typename T>
  struct f_true : public fun<T>
  {
    virtual bool operator()(const T& ) const
    {
      return true;
    }
    virtual bool operator()(const T& )
    {
      return true;
    }

    virtual f_true * clone() const
    {
      return new f_true(*this);
    }

    // FIXME:  very ugly.
    // We should not have to provide these methods.
    unsigned feature() const
    {
      return 0;
    }

    typename T::value_type value() const
    {
      return 0;
    }
  };


  template <typename T>
  struct f_not : fun<T>
  {
    explicit f_not(const fun<T>& f) : f(f)	{}

    virtual bool operator()(const T& x) const
    {
      return ! f(x);
    }
    virtual bool operator()(const T& x) 
    {
      return ! f(x);
    }

    f_not& operator=(const f_not& rh)
    {
      if (&f != this)
      {
        this->f = rh.f;
      }
      return *this;
    }

    virtual f_not * clone() const
    {
      return new f_not(*this);
    }

    unsigned feature() const
    {
      return f.feature();
    }

    typename T::value_type value() const
    {
      return f.value();
    }

    const fun<T>& f;
  };

} // end of namespace classification

#endif // fun_h__
