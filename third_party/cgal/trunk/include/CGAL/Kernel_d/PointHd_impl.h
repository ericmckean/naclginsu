// Copyright (c) 2001  Utrecht University (The Netherlands),
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
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.5-branch/Kernel_d/include/CGAL/Kernel_d/PointHd_impl.h $
// $Id: PointHd_impl.h 35126 2006-11-10 13:36:37Z hemmer $
// 
//
// Author(s)     : Michael Seel <seel@mpi-sb.mpg.de>

#ifndef CGAL_POINTHD_C
#define CGAL_POINTHD_C
CGAL_BEGIN_NAMESPACE
#define PointHd PointHd2

template <class RT, class LA>
PointHd<RT,LA> PointHd<RT,LA>::
transform(const Aff_transformationHd<RT,LA>& t) const
{ typename LA::Vector res = t(vector_rep());
  return PointHd<RT,LA>(dimension(),res.begin(),res.end()); }

template <class RT, class LA>
VectorHd<RT,LA> PointHd<RT,LA>::operator-(const Origin&) const 
{ return VectorHd<RT,LA>(Base(*this)); }

template <class RT, class LA>
PointHd<RT,LA> PointHd<RT,LA>::operator+(const VectorHd<RT,LA> &v) const
{ PointHd<RT,LA> res(dimension()); 
  res.ptr()->homogeneous_add(ptr(), v.ptr()); 
  return res; 
}

template <class RT, class LA>
PointHd<RT,LA> PointHd<RT,LA>::operator-(const VectorHd<RT,LA> &v) const
{ PointHd<RT,LA> res(dimension()); 
  res.ptr()->homogeneous_sub(ptr(), v.ptr()); 
  return res; 
}

template <class RT, class LA>
PointHd<RT,LA>& PointHd<RT,LA>::operator+= (const VectorHd<RT,LA>& v)
{ int d = dimension(); 
  PointHd<RT,LA> old(*this); 
  *this = PointHd<RT,LA>(d); 
  ptr()->homogeneous_add(old.ptr(), v.ptr()); 
  return *this; 
}

template <class RT, class LA>
PointHd<RT,LA>& PointHd<RT,LA>::operator-= (const VectorHd<RT,LA>& v)
{ int d = dimension(); 
  PointHd<RT,LA> old(*this); 
  *this = PointHd<RT,LA>(d); 
  ptr()->homogeneous_sub(old.ptr(), v.ptr()); 
  return *this; 
}

template <class RT, class LA>
std::istream& operator>>(std::istream& I, PointHd<RT,LA>& p)
{ p.copy_on_write(); p.ptr()->read(I); 
  CGAL_assertion_msg((p.homogeneous(p.dimension()) > 0), 
  "operator>>: denominator of point must be larger than zero.");
  return I; 
}

template <class RT, class LA>
std::ostream& operator<<(std::ostream& O, const PointHd<RT,LA>& p)
{ p.ptr()->print(O,"PointHd"); return O; } 

#undef PointHd 
CGAL_END_NAMESPACE
#endif // CGAL_POINTHD_C

