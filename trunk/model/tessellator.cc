// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "model/tessellator.h"
#include "model/component.h"
#include "model/kernel.h"
#include "model/mesh.h"
#include <CGAL/Nef_3/SNC_decorator.h>
#include <CGAL/Polyhedron_3.h>

namespace {
// User data passed to the glu callback functions.
struct UserData {
  ginsu::model::Tessellator* tessellator;
  ginsu::model::Tessellator::TriangleData triangle_data;
};

template<typename T> float ToFloat(T val) {
  return static_cast<float>(CGAL::to_double(val));
}

}  // anonymous namespace

namespace ginsu {
namespace model {

//void Tessellator::Tessellate(const Component& component) {
//  GLUtesselator* glu_tess = CreateGluTessellator();
//  // TODO(gwink): Using a fixed-size array to accumulate vertices, for now.
//  double vertices[64][3];
//  int vertex_count = 0;
//  // Iterate over the faces in the component.
//  Mesh::Halffacet_const_iterator facet;
//  CGAL_forall_facets(facet, *(component.mesh()->sncp())) {
//    UserData user_data;
//    Vector_3 normal = facet->plane().orthogonal_vector();
//    user_data.triangle_data.normal_x = ToFloat(normal.x());
//    user_data.triangle_data.normal_y = ToFloat(normal.y());
//    user_data.triangle_data.normal_z = ToFloat(normal.z());
//    user_data.tessellator = this;
//
//    gluTessBeginPolygon(glu_tess, &user_data);
//
//    Mesh::Halffacet_cycle_const_iterator cycle;
//    CGAL_forall_facet_cycles_of(cycle, facet) {
//      // Only consider non-trivial cycles.
//      if (cycle.is_shalfedge()) {
//        gluTessBeginContour(glu_tess);
//
//        // Iterate over vertices of cycle.
//        Mesh::SHalfedge_const_handle half_edge = cycle;
//        Mesh::SHalfedge_around_facet_const_circulator circulator(half_edge);
//        Mesh::SHalfedge_around_facet_const_circulator end(circulator);
//        CGAL_For_all(circulator, end) {
//          Point_3 point = circulator->source()->source()->point();
//
//          vertices[vertex_count][0] = ToFloat(point.x());
//          vertices[vertex_count][1] = ToFloat(point.y());
//          vertices[vertex_count][2] = ToFloat(point.z());
//          gluTessVertex(glu_tess, vertices[vertex_count],
//                        vertices[vertex_count]);
//          ++vertex_count;
//        }
//        gluTessEndContour(glu_tess);
//      }
//    }
//    gluTessEndPolygon(glu_tess);
//  }
//
//  gluDeleteTess(glu_tess);
//}

void Tessellator::Tessellate(const Component& component) {
  GLUtesselator* glu_tess = CreateGluTessellator();
  // Iterate over the facets in the component.
  Mesh::Facet_const_iterator facet;
  for (facet = component.mesh()->facets_begin();
       facet != component.mesh()->facets_end();
       ++facet) {
    UserData user_data;
    Mesh::Facet::Halfedge_around_facet_const_circulator  edge;
    edge = facet->facet_begin();

    // Compute a plane from the first three vertices.
    Mesh::Facet::Plane_3  plane(edge->vertex()->point(),
                                edge->next()->vertex()->point(),
                                edge->next()->next()->vertex()->point());
    Vector_3 normal = plane.orthogonal_vector();
    user_data.triangle_data.normal_x = ToFloat(normal.x());
    user_data.triangle_data.normal_y = ToFloat(normal.y());
    user_data.triangle_data.normal_z = ToFloat(normal.z());
    user_data.tessellator = this;

    // TODO(gwink): Using a fixed-size array to accumulate vertices, for now.
    double vertices[64][3];
    int vertex_count = 0;
    gluTessBeginPolygon(glu_tess, &user_data);
    gluTessBeginContour(glu_tess);
    do {
      Point_3 point = edge->vertex()->point();

      vertices[vertex_count][0] = ToFloat(point.x());
      vertices[vertex_count][1] = ToFloat(point.y());
      vertices[vertex_count][2] = ToFloat(point.z());
      gluTessVertex(glu_tess, vertices[vertex_count],
                    vertices[vertex_count]);
      ++vertex_count;
    } while(++edge != facet->facet_begin());
    gluTessEndContour(glu_tess);
    gluTessEndPolygon(glu_tess);
  }

  gluDeleteTess(glu_tess);
}

void Tessellator::BeginTriangleData(const TriangleData& triangles) {
  // Subclass implements.
}

void Tessellator::AddVertex(const Vertex& vertex) {
  // Subclass implements.
}

void Tessellator::EndTriangleData() {
  // Subclass implements.
}

void Tessellator::EdgeFlag(bool flag) {
  // Subclass implements.
}

GLUtesselator* Tessellator::CreateGluTessellator() {
  GLUtesselator* glu_tess = gluNewTess();
  gluTessCallback(glu_tess, GLenum(GLU_TESS_VERTEX_DATA),
                  (CallbackFunc) &VertexGluCallback);
  gluTessCallback(glu_tess, GLenum(GLU_TESS_BEGIN_DATA),
                  (CallbackFunc) &BeginGluCallback);
  gluTessCallback(glu_tess, GLenum(GLU_TESS_END_DATA),
                  (CallbackFunc) &EndGluCallback);
  gluTessCallback(glu_tess, GLenum(GLU_TESS_ERROR_DATA),
                  (CallbackFunc) &ErrorGluCallback);
  gluTessCallback(glu_tess, GLenum(GLU_TESS_EDGE_FLAG_DATA),
                  (CallbackFunc) &EdgeFlagGluCallback);
  gluTessProperty(glu_tess, GLenum(GLU_TESS_WINDING_RULE),
                  GLU_TESS_WINDING_POSITIVE);
  return glu_tess;
}

void Tessellator::BeginGluCallback(GLenum which, void* user_data) {
  UserData* data = reinterpret_cast<UserData*>(user_data);
  data->triangle_data.flavor = kTriangles;
  switch(which) {
    case GL_TRIANGLE_FAN: {
      data->triangle_data.flavor = kTriangleFan;
      break;
    }
    case GL_TRIANGLE_STRIP: {
      data->triangle_data.flavor = kTriangleStrip;
      break;
    }
    default: {
      break;
    }
  }
  data->tessellator->BeginTriangleData(data->triangle_data);
}

void Tessellator::EndGluCallback(void* user_data) {
  UserData* data = reinterpret_cast<UserData*>(user_data);
  data->tessellator->EndTriangleData();
}

void Tessellator::ErrorGluCallback(GLenum errorCode, void* user_data) {
  UserData* data = reinterpret_cast<UserData*>(user_data);
}

void Tessellator::VertexGluCallback(void* vertex, void* user_data) {
  UserData* data = reinterpret_cast<UserData*>(user_data);
  GLdouble* point = reinterpret_cast<GLdouble*>(vertex);

  Vertex v;
  v.x = ToFloat(point[0]);
  v.y = ToFloat(point[1]);
  v.z = ToFloat(point[2]);
  data->tessellator->AddVertex(v);
}

void Tessellator::EdgeFlagGluCallback(GLboolean flag, void *user_data) {
  UserData* data = static_cast<UserData*>(user_data);
  data->tessellator->EdgeFlag(flag);
}

}  // namespace model
}  // namespace ginsu
