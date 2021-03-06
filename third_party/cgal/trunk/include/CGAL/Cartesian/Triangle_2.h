// Copyright (c) 2000  Utrecht University (The Netherlands),
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
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.5-branch/Cartesian_kernel/include/CGAL/Cartesian/Triangle_2.h $
// $Id: Triangle_2.h 49057 2009-04-30 14:03:52Z spion $
// 
//
// Author(s)     : Andreas Fabri, Herve Bronnimann

#ifndef CGAL_CARTESIAN_TRIANGLE_2_H
#define CGAL_CARTESIAN_TRIANGLE_2_H

#include <CGAL/Cartesian/predicates_on_points_2.h>
#include <CGAL/array.h>

CGAL_BEGIN_NAMESPACE

template <class R_>
class TriangleC2
{
  typedef typename R_::FT                   FT;
  typedef typename R_::Point_2              Point_2;
  typedef typename R_::Vector_2             Vector_2;
  typedef typename R_::Triangle_2           Triangle_2;

  typedef cpp0x::array<Point_2, 3>          Rep;
  typedef typename R_::template Handle<Rep>::type  Base;

  Base base;

public:
  typedef R_                                    R;

  TriangleC2() {}

  TriangleC2(const Point_2 &p, const Point_2 &q, const Point_2 &r)
    : base(CGAL::make_array(p, q, r)) {}


  const Point_2 &
  vertex(int i) const
  {
    if (i>2) i = i%3;
    else if (i<0) i = (i%3) + 3;
    return (i==0) ? get(base)[0] :
      (i==1) ? get(base)[1] :
      get(base)[2];
  }
  
  const Point_2 &
  operator[](int i) const
  {
    return vertex(i);
  }

};

CGAL_END_NAMESPACE

#endif // CGAL_CARTESIAN_TRIANGLE_2_H
