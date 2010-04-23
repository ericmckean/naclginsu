// Copyright (c) 1999  Utrecht University (The Netherlands),
// ETH Zurich (Switzerland), Freie Universitaet Berlin (Germany),
// INRIA Sophia-Antipolis (France), Martin-Luther-University Halle-Wittenberg
// (Germany), Max-Planck-Institute Saarbruecken (Germany), RISC Linz (Austria),
// and Tel-Aviv University (Israel).  All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; version 2.1 of the License.
// See the file LICENSE.LGPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.5-branch/Kernel_23/include/CGAL/Segment_3.h $
// $Id: Segment_3.h 42932 2008-04-17 10:13:31Z spion $
// 
//
// Author(s)     : Andreas Fabri, Stefan Schirra

#ifndef CGAL_SEGMENT_3_H
#define CGAL_SEGMENT_3_H

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <CGAL/Kernel/Return_base_tag.h>
#include <CGAL/Bbox_3.h>
#include <CGAL/Dimension.h>

CGAL_BEGIN_NAMESPACE

template <class R_>
class Segment_3 : public R_::Kernel_base::Segment_3
{
  typedef typename R_::RT                    RT;
  typedef typename R_::FT                    FT;
  typedef typename R_::Point_3               Point_3;
  typedef typename R_::Vector_3              Vector_3;
  typedef typename R_::Direction_3           Direction_3;
  typedef typename R_::Line_3                Line_3;
  typedef typename R_::Aff_transformation_3  Aff_transformation_3;

  typedef Segment_3                          Self;
  BOOST_STATIC_ASSERT((boost::is_same<Self, typename R_::Segment_3>::value));

public:

  typedef Dimension_tag<3>  Ambient_dimension;
  typedef Dimension_tag<1>  Feature_dimension;

  typedef typename R_::Kernel_base::Segment_3  Rep;

  const Rep& rep() const
  {
    return *this;
  }

  Rep& rep()
  {
    return *this;
  }

  typedef          R_                       R;

  Segment_3() {}

  Segment_3(const Rep& s)
      : Rep(s) {}

  Segment_3(const Point_3& sp, const Point_3& ep)
    : Rep(typename R::Construct_segment_3()(Return_base_tag(), sp, ep)) {}

// FIXME TODO : Use Qrt here !
  //typename Qualified_result_of<typename R::Construct_source_3, Segment_3>::type
  Point_3
  source() const
  { 
    return R_().construct_source_3_object()(*this);
  }

  //typename Qualified_result_of<typename R::Construct_target_3, Segment_3>::type
  Point_3
  target() const
  {
    return R_().construct_target_3_object()(*this);
  }

  //typename Qualified_result_of<typename R::Construct_source_3, Segment_3>::type
  Point_3
  start() const
  {
    return source();
  }

  //typename Qualified_result_of<typename R::Construct_target_3, Segment_3>::type
  Point_3
  end() const
  {
    return target();
  }
 
  //typename Qualified_result_of<typename R::Construct_min_vertex_2, Segment_2>::type
  Point_3
  min BOOST_PREVENT_MACRO_SUBSTITUTION () const;

  //typename Qualified_result_of<typename R::Construct_max_vertex_2, Segment_2>::type
  Point_3
  max BOOST_PREVENT_MACRO_SUBSTITUTION () const;

  //typename Qualified_result_of<typename R::Construct_vertex_2, Segment_2, int>::type
  Point_3
  vertex(int i) const;

  //typename Qualified_result_of<typename R::Construct_vertex_2, Segment_2, int>::type
  Point_3
  point(int i) const
  { return vertex(i); }

  //typename Qualified_result_of<typename R::Construct_vertex_2, Segment_2, int>::type
  Point_3
  operator[](int i) const
  { return vertex(i); }


  Segment_3 transform(const Aff_transformation_3 &t) const
  {
    return Segment_3(t.transform(this->source()), t.transform(this->target()));
  }

  FT squared_length() const
  {
    return squared_distance(this->target(), this->source());
  }

  Vector_3 to_vector() const
  {
    return R().construct_vector_3_object()(*this);
  }

  bool has_on(const Point_3 &p) const
  { // TODO : use one predicate.
    return R_().are_ordered_along_line_3_object()(source(), 
					         p, 
					         target());
  }

  Segment_3 opposite() const
  {
    return R().construct_opposite_segment_3_object()(*this);
  }

  Direction_3 direction() const
  {
    typename R::Construct_vector_3 construct_vector;
    return Direction_3( construct_vector( source(), target()));
  }

  bool is_degenerate() const
  {
    return R().is_degenerate_3_object()(*this);
  }

  Bbox_3 bbox() const
  {
    return R().construct_bbox_3_object()(*this);
  }

  Line_3
  supporting_line() const
  {
    return R().construct_line_3_object()(*this);
  }

};

template < class R_ >
CGAL_KERNEL_INLINE
//typename Qualified_result_of<typename R_::Construct_min_vertex_2, Segment_2<R_> >::type
typename R_::Point_3
Segment_3<R_>::min BOOST_PREVENT_MACRO_SUBSTITUTION () const
{
  typename R_::Less_xyz_3 less_xyz; 
  return less_xyz(source(),target()) ? source() : target();
}

template < class R_ >
CGAL_KERNEL_INLINE
//typename Qualified_result_of<typename R_::Construct_max_vertex_2, Segment_2<R_> >::type
typename R_::Point_3
Segment_3<R_>::max BOOST_PREVENT_MACRO_SUBSTITUTION () const
{
  typename R_::Less_xyz_3 less_xyz; 
  return less_xyz(source(),target()) ? target() : source();
}

template < class R_ >
CGAL_KERNEL_INLINE
//typename Qualified_result_of<typename R_::Construct_vertex_2, Segment_2<R_>, int >::type
typename R_::Point_3
Segment_3<R_>::vertex(int i) const
{
  return (i%2 == 0) ? source() : target();
}


template < class R >
std::ostream &
operator<<(std::ostream &os, const Segment_3<R> &s)
{
    switch(os.iword(IO::mode)) {
    case IO::ASCII :
        return os << s.source() << ' ' << s.target();
    case IO::BINARY :
        return os << s.source() << s.target();
    default:
        return os << "Segment_3(" << s.source() <<  ", " << s.target() << ")";
    }
}

template < class R >
std::istream &
operator>>(std::istream &is, Segment_3<R> &s)
{
    typename R::Point_3 p, q;

    is >> p >> q;

    if (is)
        s = Segment_3<R>(p, q);
    return is;
}

CGAL_END_NAMESPACE

#endif // CGAL_SEGMENT_3_H
