// Copyright (c) 2003-2008  INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.6-branch/Circular_kernel_2/include/CGAL/Circular_kernel_intersections.h $
// $Id: Circular_kernel_intersections.h 52628 2009-10-20 08:59:26Z lrineau $
//
// Author(s)     : Monique Teillaud, Sylvain Pion, Pedro Machado

// Partially supported by the IST Programme of the EU as a Shared-cost
// RTD (FET Open) Project under Contract No  IST-2000-26473 
// (ECG - Effective Computational Geometry for Curves and Surfaces) 
// and a STREP (FET Open) Project under Contract No  IST-006413 
// (ACS -- Algorithms for Complex Shapes)

#ifndef CGAL_CIRCULAR_KERNEL_INTERSECTIONS_H
#define CGAL_CIRCULAR_KERNEL_INTERSECTIONS_H 

#include <CGAL/Circle_2.h>
#include <CGAL/Circular_arc_2.h>
#include <CGAL/Line_arc_2.h>
#include <CGAL/Line_2.h>
#include <CGAL/Circular_arc_point_2.h>
#include <CGAL/Object.h>

CGAL_BEGIN_NAMESPACE

#define CGAL_CIRCULAR_KERNEL_MACRO_GLOBAL_FUNCTION_INTERSECTION_(A,B) \
template < class OutputIterator, class K > \
OutputIterator \
intersection(const A <K> &c1, const B <K> &c2, OutputIterator res) \
{ \
  return typename K::Intersect_2()(c1, c2, res); \
} \
namespace internal { \
  template <class K> \
  inline \
  bool \
  do_intersect(const typename K::A &c1, const typename K::B &c2, const K&) \
  { \
		std::vector< Object > res; \
    typename K::Intersect_2()(c1,c2,std::back_inserter(res)); \
		return res.size() != 0; \
  } \
} \
template <class K> \
inline \
bool \
do_intersect(const A <K> &c1, const B <K> &c2) \
{ \
  return typename K::Do_intersect_2()(c1, c2); \
}

// Circle_2 Circle_2 already has its do_intersect
// so it needs only the global intersection
template < class OutputIterator, class K >
OutputIterator
intersection(const Circle_2 <K> &c1, const Circle_2 <K> &c2, OutputIterator res)
{
  return typename K::Intersect_2()(c1, c2, res);
}

template < class OutputIterator, class K >
OutputIterator
intersection(const Circle_2 <K> &c1, const Line_2 <K> &c2, OutputIterator res)
{
  return typename K::Intersect_2()(c1, c2, res);
}

template < class OutputIterator, class K >
OutputIterator
intersection(const Line_2 <K> &c1, const Circle_2 <K> &c2, OutputIterator res)
{
  return typename K::Intersect_2()(c1, c2, res);
}

CGAL_CIRCULAR_KERNEL_MACRO_GLOBAL_FUNCTION_INTERSECTION_(Circular_arc_2, Circular_arc_2)
CGAL_CIRCULAR_KERNEL_MACRO_GLOBAL_FUNCTION_INTERSECTION_(Line_arc_2, Line_arc_2)
CGAL_CIRCULAR_KERNEL_MACRO_GLOBAL_FUNCTION_INTERSECTION_(Line_arc_2, Circle_2)
CGAL_CIRCULAR_KERNEL_MACRO_GLOBAL_FUNCTION_INTERSECTION_(Circle_2, Line_arc_2)
CGAL_CIRCULAR_KERNEL_MACRO_GLOBAL_FUNCTION_INTERSECTION_(Circular_arc_2, Circle_2)
CGAL_CIRCULAR_KERNEL_MACRO_GLOBAL_FUNCTION_INTERSECTION_(Circle_2, Circular_arc_2)
CGAL_CIRCULAR_KERNEL_MACRO_GLOBAL_FUNCTION_INTERSECTION_(Line_arc_2, Circular_arc_2)
CGAL_CIRCULAR_KERNEL_MACRO_GLOBAL_FUNCTION_INTERSECTION_(Circular_arc_2, Line_arc_2)
CGAL_CIRCULAR_KERNEL_MACRO_GLOBAL_FUNCTION_INTERSECTION_(Line_2, Circular_arc_2)
CGAL_CIRCULAR_KERNEL_MACRO_GLOBAL_FUNCTION_INTERSECTION_(Line_2, Line_arc_2)
CGAL_CIRCULAR_KERNEL_MACRO_GLOBAL_FUNCTION_INTERSECTION_(Circular_arc_2, Line_2)
CGAL_CIRCULAR_KERNEL_MACRO_GLOBAL_FUNCTION_INTERSECTION_(Line_arc_2, Line_2)

CGAL_END_NAMESPACE

#endif // CGAL_CIRCULAR_KERNEL_INTERSECTIONS_H
