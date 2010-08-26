// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VERTEX_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VERTEX_H_

#include <assert.h>
#include <CGAL/basic.h>
#include "geometry/cgal_ext/partialdsentity.h"

namespace ginsu {
namespace geometry {

// PartialDSVertex: template class for vertex entity in the partial-entity
// data structure.
// The template parameters are:
//   TypeRefs: gives access to the types declared and used within the PartialDS
//             data structure. (See PartialDSTypes in partialds.h.)
//   PointType: the geometry information stored within each vertex.
template <class TypeRefs, class PointType>
class PartialDSVertex : public PartialDSEntity<TypeRefs> {
 public:
  typedef TypeRefs                               PartialDS;
  typedef PointType                              Point;
  typedef PartialDSVertex<TypeRefs, Point>       Self;
  typedef PartialDSEntity<TypeRefs>              Base;

  typedef typename PartialDS::VertexHandle       VertexHandle;
  typedef typename PartialDS::VertexConstHandle  VertexConstHandle;
  typedef typename PartialDS::PVertexHandle      PVertexHandle;
  typedef typename PartialDS::PVertexConstHandle PVertexConstHandle;

  PartialDSVertex() : parent_vertex_(NULL) { }

  PVertexConstHandle parent_vertex() const { return parent_vertex_; }
  void set_parent_vertex(PVertexHandle pvertex) { parent_vertex_ = pvertex; }

  const Point& point() const { return p_; }
  void set_point(const Point& p) { p_ = p; }

 private:
  PVertexHandle parent_vertex_; 
  Point p_;
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VERTEX_H_
