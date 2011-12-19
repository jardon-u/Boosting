// Ugo Jardonnet All Rights Reserved.
// Author: Ugo Jardonnet

#ifndef _OBSERVARTION_HH
# define _OBSERVARTION_HH

namespace classification
{

  template < typename X, typename Y, typename Z >
  struct observation
  {
    observation(X x_, Y y_, Z w_)
      : x(x_), y(y_), w(w_)
    {}
    const X x;
    const Y y;
    const Z w;
  };

}

#endif /* _OBSERVARTION_HH */

