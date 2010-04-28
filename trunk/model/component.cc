// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "model/component.h"
#include <CGAL/Polyhedron_3.h>

namespace ginsu {
namespace model {

// We'll use polyhedra as a an intermediate step when creating meshes.
typedef CGAL::Polyhedron_3<Kernel> Polyhedron_3;
typedef Polyhedron_3::Halfedge_handle HalfEdge;

Component* Component::MakeCube() {
  // Make a cube per CGAL::Polyhedron_3 user manual. Start with a tetrahedron,
  // and use a series of euler operator to transform it into a cube.
  Polyhedron_3 p;
  HalfEdge e, f, g, h;
  h = p.make_tetrahedron(Point_3(1.0, 0.0, 0.0),
                         Point_3(0.0, 0.0, 1.0),
                         Point_3(0.0, 0.0, 0.0),
                         Point_3(0.0, 1.0, 0.0));
  g = h->next()->opposite()->next();
  p.split_edge(h->next());
  p.split_edge(g->next());
  p.split_edge(g);
  h->next()->vertex()->point() = Point_3(1.0, 0.0, 1.0);
  g->next()->vertex()->point() = Point_3(0.0, 1.0, 1.0);
  g->opposite()->vertex()->point() = Point_3(1.0, 1.0, 0.0);
  f = p.split_facet(g->next(), g->next()->next()->next());
  e = p.split_edge(f);
  e->vertex()->point() = Point_3(1.0, 1.0, 1.0);
  p.split_facet(e, f->next()->next());
  CGAL_postcondition(p.is_valid());

  // Now make a mesh (nef polyhedron) from the polyhedron.
  Component* component = new Component();
  component->mesh_ = Mesh(p);
  return component;
}

Component::Component() 
  : transform_(CGAL::Identity_transformation()),
    mesh_(Mesh::EMPTY) {
}

}  // namespace model
}  // namespace ginsu
