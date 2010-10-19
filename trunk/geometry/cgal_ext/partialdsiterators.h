// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDSITERATORS_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDSITERATORS_H_

#include <cassert>
#include "CGAL/config.h"

namespace ginsu {
namespace geometry {

// Iterate over a lists of void shells by following the shells' next_void_shell
// pointers.
template <class It>
class VoidShellIterator : public It {
 public:
  typedef  It                                  Iterator;
  typedef  std::forward_iterator_tag           iterator_category;
  typedef  VoidShellIterator<It>               Self;
  typedef  std::iterator_traits<It>            Traits;
  typedef  typename Traits::value_type         value_type;
  typedef  typename Traits::difference_type    difference_type;
  typedef  std::size_t                         size_type;
  typedef  typename Traits::reference          reference;
  typedef  typename Traits::pointer            pointer;

  VoidShellIterator() {}
  explicit VoidShellIterator(It i) : It(i) {}
  
  bool operator==(Iterator i) const { return  It::operator==(i); } 
  bool operator==(const Self& i) const { return  It::operator==(i); }
  bool operator!=(const Self& i) const { return !(*this == i); }
  
  Self& operator++() {
    *(static_cast<Iterator*>(this)) = (*this)->next_void_shell();
    return *this;
  }
  Self operator++(int) {
    Self tmp = *this;
    ++*this;
    return tmp;
  }
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDSCIRCULATORS_H_
