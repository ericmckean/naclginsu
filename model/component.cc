// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "model/component.h"
#include "model/mesh.h"
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>

namespace ginsu {
namespace model {

// We'll use polyhedra as a an intermediate step when creating meshes.
typedef CGAL::Polyhedron_3<Kernel> Polyhedron_3;
typedef Polyhedron_3::Halfedge_handle HalfEdge;
typedef Polyhedron_3::HalfedgeDS HalfedgeDS;

// We use a modifier to build polyhedra incrementally. A modifier is a special
// class that's allowed to work on the internal representation - specifically
// the half-edge data structure - of a polyedron.
class PolyhedronBuilder : public CGAL::Modifier_base<HalfedgeDS> {
 public:
  PolyhedronBuilder() : type_(kUnknown) {}
 
  // Build a truncated cone.
  void SetupTruncatedCone(float top_radius, float bottom_radius) {
    type_ = kTruncatedCone;
    cone_top_radius_ = top_radius;
    cone_bottom_radius_ = bottom_radius;
  }

  // Operator() mandated by base class.
  void operator()(HalfedgeDS& half_edge_ds) {
    switch(type_) {
      case kUnknown: {
        // Somebody forgot to call Setup*** to specify the primitive.
        break;
      }
      case kTruncatedCone: {
        BuildTruncatedCone(half_edge_ds);
        break;
      }
    }
  }

 protected:
  void BuildTruncatedCone (HalfedgeDS& half_edge_ds) {
    // Sanitize parameters.
    cone_top_radius_ = ::fabs(cone_top_radius_);
    cone_bottom_radius_ = ::fabs(cone_bottom_radius_);
    if (cone_top_radius_ == 0.0f && cone_bottom_radius_ == 0.0f) {
      return;
    }

    // We use CGAL's incremental builder.
    CGAL::Polyhedron_incremental_builder_3<HalfedgeDS> builder(half_edge_ds);

    // A truncated cone with 20 faces around. We must take care of the cases
    // where either the top or bottom slice is degenerate (radius == 0).
    static const int kNumFaces = 20;
    static const float kPi = 3.14159265358979323846f;
    bool top_degenerate = (cone_top_radius_ == 0.0f);
    bool bottom_degenerate = (cone_bottom_radius_ == 0.0f);
    int num_vertices = kNumFaces * 2;
    if (top_degenerate || bottom_degenerate) {
      // Either the top or bottom slice degenerates to a single vertex.
      num_vertices = kNumFaces + 1;
    }
    builder.begin_surface(num_vertices, kNumFaces + 2);

    // Add vertices.
    float theta = 0.0f;
    float delta_theta = 2.0f * kPi / kNumFaces;
    if (top_degenerate) {
      // A single vertex at the top.
      builder.add_vertex(Point_3(0.0f, 1.0f, 0.0f));
    }
    if (bottom_degenerate) {
      // A single vertex at the bottom.
      builder.add_vertex(Point_3(0.0f, 0.0f, 0.0f));
    }
    for (int i = 0; i < kNumFaces; ++i, theta += delta_theta) {
      float cos_theta = cos(theta);
      float sin_theta = sin(theta);
      if (!top_degenerate) {
        builder.add_vertex(Point_3(cone_top_radius_ * cos_theta,
                                   1.0f,
                                   cone_top_radius_ * sin_theta));
      }
      if (!bottom_degenerate) {
        builder.add_vertex(Point_3(cone_bottom_radius_ * cos_theta,
                                   0.0f,
                                   cone_bottom_radius_ * sin_theta));
      }
    }

    // Build the side faces, oriented CCW.
    for (int i = 0; i < kNumFaces; ++i) {
      builder.begin_facet();
      if (top_degenerate) {
        builder.add_vertex_to_facet(0);
        builder.add_vertex_to_facet(i);
        builder.add_vertex_to_facet((i + 1) % num_vertices);
      } else if (bottom_degenerate) {
        builder.add_vertex_to_facet(0);
        builder.add_vertex_to_facet((i + 1) % num_vertices);
        builder.add_vertex_to_facet(i);
      } else {
        builder.add_vertex_to_facet(i);
        builder.add_vertex_to_facet(i + 1);
        builder.add_vertex_to_facet((i + 3) % num_vertices);
        builder.add_vertex_to_facet((i + 2) % num_vertices);
      }
      builder.end_facet();
    }

    // Build the caps for the top and bottom slice, again with ccw orientation.
    if (!top_degenerate) {
      int i = bottom_degenerate ? 1 : 0;
      int i_increment = bottom_degenerate ? 1 : 2;
      builder.begin_facet();
      for (int face = 0; face < kNumFaces; ++face, i += i_increment) {
        builder.add_vertex_to_facet(i);
      }
      builder.end_facet();
    }
    if (!bottom_degenerate) {
      int i = num_vertices - 1;
      int i_decrement = top_degenerate ? 1 : 2;
      builder.begin_facet();
      for (int face = 0; face < kNumFaces; ++face, i -= i_decrement) {
        builder.add_vertex_to_facet(i);
      }
      builder.end_facet();
    }

    builder.end_surface();
  }

 private:
  // Type of primitive to be created.
  enum Type { kUnknown, kTruncatedCone };
  Type type_;
  // Construction parameters for various primitives.
  float cone_top_radius_;
  float cone_bottom_radius_;
};


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
  component->Init(new Mesh(p));
  return component;
}

Component* Component::MakeTruncatedCone(float top_radius,
                                        float bottom_radius) {
  // Use the builder to create a truncated cone.
  Polyhedron_3 cone;
  PolyhedronBuilder builder;
  builder.SetupTruncatedCone(top_radius, bottom_radius);
  cone.delegate(builder);
  CGAL_postcondition(cone.is_valid());
 
  // Now we're ready to make a component.
  Component* component = NULL;
  if (cone.is_valid()) {
    component = new Component();
    component->Init(new Mesh(cone));
  }
  return component;
}

Component::Component() {
}

void Component::Init(Mesh* mesh) {
  transform_.reset(new AffineTransform3D(CGAL::Identity_transformation()));
  mesh_.reset(mesh);
}

}  // namespace model
}  // namespace ginsu
