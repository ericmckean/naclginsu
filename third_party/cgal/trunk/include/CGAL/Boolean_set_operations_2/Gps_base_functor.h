// Copyright (c) 2005  Tel-Aviv University (Israel).
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
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.5-branch/Boolean_set_operations_2/include/CGAL/Boolean_set_operations_2/Gps_base_functor.h $
// $Id: Gps_base_functor.h 37232 2007-03-19 07:21:47Z afabri $
// 
//
// Author(s)     : Baruch Zukerman <baruchzu@post.tau.ac.il>

#ifndef CGAL_GPS_BASE_FUNCTOR_H
#define CGAL_GPS_BASE_FUNCTOR_H

CGAL_BEGIN_NAMESPACE


template <class Arrangement_>
class Gps_base_functor
{
public:

  typedef Arrangement_       Arrangement_2;

  typedef typename Arrangement_2::Face_const_handle       Face_const_handle;
  typedef typename Arrangement_2::Vertex_const_handle     Vertex_const_handle;
  typedef typename Arrangement_2::Halfedge_const_handle   Halfedge_const_handle;

  typedef typename Arrangement_2::Face_handle        Face_handle;
  typedef typename Arrangement_2::Halfedge_handle    Halfedge_handle;
  typedef typename Arrangement_2::Vertex_handle      Vertex_handle;
  


   void create_face (Face_const_handle ,
                     Face_const_handle,
                     Face_handle )
  {}

  void create_vertex(Halfedge_const_handle ,
                     Halfedge_const_handle ,
                     Vertex_handle )
  {}

  void create_vertex(Vertex_const_handle ,
                     Vertex_const_handle ,
                     Vertex_handle  )
  {}

  void create_vertex(Vertex_const_handle ,
                     Halfedge_const_handle ,
                     Vertex_handle )
  {}

  void create_vertex(Halfedge_const_handle ,
                     Vertex_const_handle ,
                     Vertex_handle )
  {}

  void create_vertex(Face_const_handle ,
                     Vertex_const_handle ,
                     Vertex_handle )
  {}

  void create_vertex(Vertex_const_handle ,
                     Face_const_handle ,
                     Vertex_handle )
  {}

  void create_edge(Halfedge_const_handle ,
                   Halfedge_const_handle ,
                   Halfedge_handle )
  {}

  void create_edge(Halfedge_const_handle ,
                   Face_const_handle ,
                   Halfedge_handle )
  {}

  void create_edge(Face_const_handle ,
                   Halfedge_const_handle ,
                   Halfedge_handle )
  {}

};


CGAL_END_NAMESPACE

#endif
