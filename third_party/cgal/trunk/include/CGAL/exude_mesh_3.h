// Copyright (c) 2009 INRIA Sophia-Antipolis (France).
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
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.6-branch/Mesh_3/include/CGAL/exude_mesh_3.h $
// $Id: exude_mesh_3.h 53413 2009-12-15 13:19:38Z stayeb $
//
//
// Author(s)     : Stephane Tayeb
//
//******************************************************************************
// File Description : exude_mesh_3 function definition.
//******************************************************************************

#ifndef CGAL_EXUDE_MESH_3_H
#define CGAL_EXUDE_MESH_3_H

#include <CGAL/Mesh_3/sliver_criteria.h>
#include <CGAL/Mesh_3/Slivers_exuder.h>
#include <CGAL/Mesh_optimization_return_code.h>

CGAL_BEGIN_NAMESPACE
    
namespace parameters { namespace default_values {
  const double exude_sliver_bound = 0.;
} } // end namespace parameters::default_values



BOOST_PARAMETER_FUNCTION(
  (Mesh_optimization_return_code),
  exude_mesh_3,
  parameters::tag,
  (required (in_out(c3t3),*) )
  (optional
    (time_limit_, *, 0 )
    (sliver_bound_, *, parameters::default_values::exude_sliver_bound )
  )
)
{
  return exude_mesh_3_impl(c3t3, time_limit_, sliver_bound_);
}



template <typename C3T3> 
Mesh_optimization_return_code
exude_mesh_3_impl(C3T3& c3t3,
                  const double time_limit,
                  const double sliver_bound)
{
  typedef typename C3T3::Triangulation::Geom_traits Gt;
  typedef Mesh_3::Min_dihedral_angle_criterion<Gt> Sc;
  //typedef Mesh_3::Radius_radio_criterion<Gt> Sc;
  typedef typename Mesh_3::Slivers_exuder<C3T3, Sc> Exuder;
  
  // Create exuder
  Exuder exuder(c3t3);

  // Set time_limit
  exuder.set_time_limit(time_limit);
  
  // Launch exudation
  return exuder(sliver_bound);
}


CGAL_END_NAMESPACE

#endif // CGAL_EXUDE_MESH_3_H
