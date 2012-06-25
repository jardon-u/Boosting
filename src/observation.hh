// Ugo Jardonnet All Rights Reserved.
// Author: Ugo Jardonnet

#ifndef _OBSERVARTION_HH
# define _OBSERVARTION_HH

namespace classification
{

  template < typename X, typename Y, typename W >
  struct observation
  {
    typedef X point_t;
    typedef Y label_t;
    typedef W weight_t;

    observation(X x_, Y y_, W w_)
      : x(x_), y(y_), w(w_)
    {}
    const X x;
    const Y y;
    const W w;
  };

}

#endif /* _OBSERVARTION_HH */

