// Copyright (c) 2006-2007  INRIA Sophia-Antipolis (France).
// Copyright (c) 2008       GeometryFactory (France)
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
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.5-branch/Surface_mesher/include/CGAL/Surface_mesher_generator.h $
// $Id: Surface_mesher_generator.h 46548 2008-10-29 13:42:19Z lrineau $
//
// Author(s)     : Laurent Rineau

#ifndef CGAL_SURFACE_MESH_GENERATOR_H
#define CGAL_SURFACE_MESH_GENERATOR_H

#include <CGAL/Surface_mesher/Surface_mesher.h>
#include <CGAL/Surface_mesher/Surface_mesher_manifold.h>
#include <CGAL/Surface_mesher/Surface_mesher_edges_level.h>
#include <CGAL/Surface_mesher/Surface_mesher_visitor.h>
#include <CGAL/Surface_mesher/Surface_mesher_edges_level_visitor.h>
#include <CGAL/Surface_mesh_traits_generator_3.h>
#include <CGAL/Surface_mesh_complex_2_in_triangulation_3.h>

#include <CGAL/Surface_mesher/Verbose_flag.h>

// meta:programmation (Edge_generator and Facet_generator)
#include <CGAL/Surface_mesher/Types_generators.h>

namespace CGAL {

  struct Non_manifold_tag {};
  struct Manifold_tag {};
  struct Manifold_with_boundary_tag {};

//   struct Dynamic_manifold_tag {
//     enum Tag { Manifold = 0, Non_manifold = 1, Manifold_with_boundary = 2 };
//   };

  template <
    typename C2T3,
    typename SurfaceMeshTraits_3,
    typename Criteria,
    typename Tag // generic version: generates a compile time error
  >
  struct Make_surface_mesh_helper {
    template <typename T>
    struct Tag_does_not_exist_error {};
    typedef Tag_does_not_exist_error<Tag> Mesher_base;
  };

  template <
    typename C2T3,
    typename SurfaceMeshTraits_3,
    typename Criteria
  >
  struct Make_surface_mesh_helper<
    C2T3,
    SurfaceMeshTraits_3,
    Criteria,
    Non_manifold_tag> // Non_manifold_tag partial specialization
  {
    typedef Surface_mesher::Surface_mesher_base<
      C2T3,
      typename SurfaceMeshTraits_3::Surface_3,
      SurfaceMeshTraits_3,
      Criteria> Mesher_base;
  };

  template <
    typename C2T3,
    typename SurfaceMeshTraits_3,
    typename Criteria
  >
  struct Make_surface_mesh_helper<
    C2T3,
    SurfaceMeshTraits_3,
    Criteria,
    Manifold_with_boundary_tag> // Manifold_with_boundary_tag partial
                                // specialization
  {
    typedef Surface_mesher::Surface_mesher_regular_edges_base<
      C2T3,
      typename SurfaceMeshTraits_3::Surface_3,
      SurfaceMeshTraits_3,
      Criteria,
      true> /* true means "with boundary"*/ Regular_edge_base;

    typedef Surface_mesher::Surface_mesher_manifold_base<
      C2T3,
      typename SurfaceMeshTraits_3::Surface_3,
      SurfaceMeshTraits_3,
      Criteria,
      Regular_edge_base
      > Mesher_base;
  };

  template <
    typename C2T3,
    typename SurfaceMeshTraits_3,
    typename Criteria
  >
  struct Make_surface_mesh_helper<
    C2T3,
    SurfaceMeshTraits_3,
    Criteria,
    Manifold_tag> // Manifold_tag partial specialization
  {
    typedef Surface_mesher::Surface_mesher_regular_edges_base<
      C2T3,
      typename SurfaceMeshTraits_3::Surface_3,
      SurfaceMeshTraits_3,
      Criteria> Regular_edge_without_boundary_base;

    typedef Surface_mesher::Surface_mesher_manifold_base<
      C2T3,
      typename SurfaceMeshTraits_3::Surface_3,
      SurfaceMeshTraits_3,
      Criteria,
      Regular_edge_without_boundary_base
      > Mesher_base;
  };

  template <typename C2T3,
            typename SurfaceMeshTraits_3,
            typename Criteria,
            typename Tag,
            Surface_mesher::Verbose_flag verbosity = Surface_mesher::NOT_VERBOSE >
  struct Surface_mesher_generator
  {
    typedef typename Make_surface_mesh_helper<
      C2T3,
      SurfaceMeshTraits_3,
      Criteria,
      Tag>::Mesher_base Mesher_base;
  
    typedef Surface_mesher::Surface_mesher<
      Mesher_base,
      typename Surface_mesher::details::Facet_generator<Mesher_base>::type,
      Null_mesher_level,
      verbosity> Mesher;
  
    typedef Mesher type;
    typedef Mesher Type;
  }; // end struct Surface_mesher_generator
  
} // end namespace CGAL

#endif // CGAL_SURFACE_MESH_GENERATOR_H
