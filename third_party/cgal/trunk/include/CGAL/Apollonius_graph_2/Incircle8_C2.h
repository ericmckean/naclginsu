// Copyright (c) 2007 Foundation for Research and Technology-Hellas (Greece).
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
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.5-branch/Apollonius_graph_2/include/CGAL/Apollonius_graph_2/Incircle8_C2.h $
// $Id: Incircle8_C2.h 44317 2008-07-22 12:29:01Z spion $
// 
//
// Author(s)     : Menelaos Karavelas <mkaravel@iacm.forth.gr>


#ifndef CGAL_APOLLONIUS_GRAPH_2_INCIRCLE8_C2_H
#define CGAL_APOLLONIUS_GRAPH_2_INCIRCLE8_C2_H

#include <CGAL/determinant.h>
#include <CGAL/Apollonius_graph_2/Incircle_C2.h>


CGAL_BEGIN_NAMESPACE

CGAL_APOLLONIUS_GRAPH_2_BEGIN_NAMESPACE

//--------------------------------------------------------------------

template < class K, class MTag >
class Vertex_conflict8_2
  : public Vertex_conflict_2<K,MTag>
{
private:
  typedef Vertex_conflict_2<K,MTag>         Base;

public:
  typedef typename Base::Kernel             Kernel;
  typedef typename Base::Method_tag         Method_tag;
  typedef typename Base::Site_2             Site_2;
  typedef typename Base::FT                 FT;
  typedef typename Base::Sign               Sign;

public:
  typedef Sign                result_type;
  typedef Site_2              argument_type;

  inline
  Sign predicate(const FT& Exp, const FT& Eyp, const FT& Erp,
		 const FT& Exrp, const FT& Eyrp, const FT& Exyp,
		 const Field_with_sqrt_tag&) const
  {
    FT G = CGAL::square(Exp) + CGAL::square(Eyp) - CGAL::square(Erp);
    return CGAL::sign(Exp * Exrp + Eyp * Eyrp + Exyp * CGAL::sqrt(G));
  }

  inline
  Sign predicate(const FT& Exp, const FT& Eyp, const FT& Erp,
		 const FT& Exrp, const FT& Eyrp, const FT& Exyp,
		 const Integral_domain_without_division_tag&) const
  {
    Sign sA = CGAL::sign(Exp * Exrp + Eyp * Eyrp);
    Sign sB = CGAL::sign(Exyp);

    if ( sA == CGAL::ZERO ) { return sB; }
    if ( sB == CGAL::ZERO ) { return sA; }
    if ( sA == sB ) { return sA; }

    Sign s =
      CGAL::sign(CGAL::square(Exrp) + CGAL::square(Eyrp) - CGAL::square(Exyp));

    return sA * s;
  }

  inline
  Sign operator()(const Site_2& p1, const Site_2& p2,
		  const Site_2& p3, const Site_2& q) const
  {
#ifdef AG2_PROFILE_PREDICATES
    ag2_predicate_profiler::incircle_counter++;
#endif

    FT xj = p2.x() - p1.x();
    FT xk = p3.x() - p1.x();
    FT xl = q.x() - p1.x();

    FT yj = p2.y() - p1.y();
    FT yk = p3.y() - p1.y();
    FT yl = q.y() - p1.y();

    FT rj = p2.weight() - p1.weight();
    FT rk = p3.weight() - p1.weight();
    FT rl = q.weight() - p1.weight();

    FT pj = CGAL::square(xj) + CGAL::square(yj) - CGAL::square(rj);
    FT pk = CGAL::square(xk) + CGAL::square(yk) - CGAL::square(rk);
    FT pl = CGAL::square(xl) + CGAL::square(yl) - CGAL::square(rl);

    FT Exp = determinant(xj, pj, xk, pk);
    FT Eyp = determinant(yj, pj, yk, pk);
    FT Erp = determinant(rj, pj, rk, pk);

    FT Exy = determinant(xj, yj, xk, yk);
    FT Exr = determinant(xj, rj, xk, rk);
    FT Eyr = determinant(yj, rj, yk, rk);

    FT Exyp = xl * Eyp - yl * Exp + pl * Exy;
    FT Exrp = xl * Erp - rl * Exp + pl * Exr;
    FT Eyrp = yl * Erp - rl * Eyp + pl * Eyr;

    return predicate(Exp, Eyp, Erp, Exrp, Eyrp, Exyp, Method_tag());
  }


  inline
  Sign operator()(const Site_2& p1, const Site_2& p2,
		  const Site_2& q) const
  {
    return Base::operator()(p1, p2, q);
  }  

};

//--------------------------------------------------------------------

CGAL_APOLLONIUS_GRAPH_2_END_NAMESPACE

CGAL_END_NAMESPACE

#endif // CGAL_APOLLONIUS_GRAPH_2_INCIRCLE8_C2_H
