// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDSCIRCULATORS_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDSCIRCULATORS_H_

#include <cassert>
#include "CGAL/circulator_bases.h"
#include "CGAL/config.h"

namespace ginsu {
namespace geometry {
namespace circulator {

// A circulator to visit p-edges around a loop. Template class |It| is a
// PartialDSPEdge iterator - such as PEdgeHandle - or an equivalent p-edge
// class or const version.  (For an example, see functions PartialDSLoop::begin
// and PartialDSLoop::FindPEdge.
template <class It>
class PEdgeLoopCirculator : public It {
 public:
  typedef  It                                  Iterator;
  typedef  CGAL::Bidirectional_circulator_tag  iterator_category;
  typedef  PEdgeLoopCirculator<It>             Self;
  typedef  std::iterator_traits<It>            Traits;
  typedef  typename Traits::value_type         value_type;
  typedef  typename Traits::difference_type    difference_type;
  typedef  std::size_t                         size_type;
  typedef  typename Traits::reference          reference;
  typedef  typename Traits::pointer            pointer;

  PEdgeLoopCirculator() {}
  explicit PEdgeLoopCirculator(It i) : It(i) {}
  template <class It2>
  PEdgeLoopCirculator(const PEdgeLoopCirculator<It2> &c)
      : It(static_cast<const It2&>(c)) {}

  bool operator==(CGAL::Nullptr_t p) const {
    assert(p == NULL);
    return It::operator==(It());
  }
  bool operator!=(CGAL::Nullptr_t p) const { return !(*this == p); }
  bool operator==(const Self& i)    const { return  It::operator==(i); }
  bool operator!=(const Self& i)    const { return !(*this == i); }

  Self& operator++() {
    *(static_cast<Iterator*>(this)) = (*this)->loop_next();
    return *this;
  }
  Self  operator++(int) {
    Self tmp = *this;
    ++*this;
    return tmp;
  }

  Self& operator--() {
    *(static_cast<Iterator*>(this)) = (*this)->loop_prev();
    return *this;
  }
  Self operator--(int) {
    Self tmp = *this;
    --*this;
    return tmp;
  }
};

// A circulator to visit p-edges that form a radial fan about an edge. Template
// class |It| is a PartialDSPEdge iterator - such as PEdgeHandle - or an
// equivalent p-edge class or const version. (For an example, see functions
// PartialDSEdge::begin and PartialDSEdge::FindRadialPEdge. 
template <class It>
class PEdgeRadialCirculator : public It {
 public:
  typedef  It                                  Iterator;
  typedef  CGAL::Bidirectional_circulator_tag  iterator_category;
  typedef  PEdgeRadialCirculator<It>           Self;
  typedef  std::iterator_traits<It>            Traits;
  typedef  typename Traits::value_type         value_type;
  typedef  typename Traits::difference_type    difference_type;
  typedef  std::size_t                         size_type;
  typedef  typename Traits::reference          reference;
  typedef  typename Traits::pointer            pointer;

  PEdgeRadialCirculator() {}
  explicit PEdgeRadialCirculator(It i) : It(i) {}
  template <class It2>
  PEdgeRadialCirculator(const PEdgeRadialCirculator<It2> &c)
      : It(static_cast<const It2&>(c)) {}

  bool operator==(CGAL::Nullptr_t p) const {
    assert(p == NULL);
    return It::operator==(It());
  }
  bool operator!=(CGAL::Nullptr_t p) const { return !(*this == p); }
  bool operator==(const Self& i)    const { return  It::operator==(i); }
  bool operator!=(const Self& i)    const { return !(*this == i); }

  Self& operator++() {
    *(static_cast<Iterator*>(this)) = (*this)->radial_next();
    return *this;
  }
  Self  operator++(int) {
    Self tmp = *this;
    ++*this;
    return tmp;
  }

  Self& operator--() {
    *(static_cast<Iterator*>(this)) = (*this)->radial_prev();
    return *this;
  }
  Self operator--(int) {
    Self tmp = *this;
    --*this;
    return tmp;
  }
};

// A circulator to visit the cloud of p-vertices around a vertex. Template
// class |It| is a PartialDSPVertex iterator - such as PVertexHandle - or an
// equivalent p-vertex iterator or const version. 
template <class It>
class PVertexOfVertexCirculator : public It {
 public:
  typedef  It                                  Iterator;
  typedef  CGAL::Forward_circulator_tag        iterator_category;
  typedef  PVertexOfVertexCirculator<It>       Self;
  typedef  std::iterator_traits<It>            Traits;
  typedef  typename Traits::value_type         value_type;
  typedef  typename Traits::difference_type    difference_type;
  typedef  std::size_t                         size_type;
  typedef  typename Traits::reference          reference;
  typedef  typename Traits::pointer            pointer;

  PVertexOfVertexCirculator() {}
  explicit PVertexOfVertexCirculator(It i) : It(i) {}
  template <class It2>
  PVertexOfVertexCirculator(const PVertexOfVertexCirculator<It2> &c)
      : It(static_cast<const It2&>(c)) {}

  bool operator==(CGAL::Nullptr_t p) const {
    assert(p == NULL);
    return It::operator==(It());
  }
  bool operator!=(CGAL::Nullptr_t p) const { return !(*this == p); }
  bool operator==(const Self& i)    const { return  It::operator==(i); }
  bool operator!=(const Self& i)    const { return !(*this == i); }

  Self& operator++() {
    *(static_cast<Iterator*>(this)) = (*this)->next_pvertex();
    return *this;
  }
  Self  operator++(int) {
    Self tmp = *this;
    ++*this;
    return tmp;
  }
};

// A circulator to visit the p-faces that make up a shell. Template class |It|
// is a PartialDSPFace iterator - such as PFaceHandle - or an equivalent
// p-vertex iterator or const version. 
template <class It>
class PFaceOfShellCirculator : public It {
 public:
  typedef  It                                  Iterator;
  typedef  CGAL::Forward_circulator_tag        iterator_category;
  typedef  PFaceOfShellCirculator<It>          Self;
  typedef  std::iterator_traits<It>            Traits;
  typedef  typename Traits::value_type         value_type;
  typedef  typename Traits::difference_type    difference_type;
  typedef  std::size_t                         size_type;
  typedef  typename Traits::reference          reference;
  typedef  typename Traits::pointer            pointer;

  PFaceOfShellCirculator() {}
  explicit PFaceOfShellCirculator(It i) : It(i) {}
  template <class It2>
  PFaceOfShellCirculator(const PFaceOfShellCirculator<It2> &c)
      : It(static_cast<const It2&>(c)) {}

  bool operator==(CGAL::Nullptr_t p) const {
    assert(p == NULL);
    return It::operator==(It());
  }
  bool operator!=(CGAL::Nullptr_t p) const { return !(*this == p); }
  bool operator==(const Self& i)    const { return  It::operator==(i); }
  bool operator!=(const Self& i)    const { return !(*this == i); }

  Self& operator++() {
    *(static_cast<Iterator*>(this)) = (*this)->next_pface();
    return *this;
  }
  Self  operator++(int) {
    Self tmp = *this;
    ++*this;
    return tmp;
  }
};

// Stl-style find_if function for circulators.
template <class Circulator, class Predicate>
typename Circulator::Iterator find_if(Circulator start, Predicate pred) {
  const static typename Circulator::Iterator kNullIterator;
  
  assert(start != NULL);
  if (start == NULL) return kNullIterator;

  Circulator end = start;
  do {
    if (pred(start)) {
      return start;
    }
    ++start;
  } while(start != end);

  return kNullIterator;
}

}  // namespace circulator
}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDSCIRCULATORS_H_
