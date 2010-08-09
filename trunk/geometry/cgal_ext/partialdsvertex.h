// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VERTEX_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VERTEX_H_

#include <CGAL/basic.h>
#include "geometry/cgal_ext/partialdsentity.h"

namespace ginsu {
namespace geometry {

// PartialDSVertex: template class for vertex entity in the partial-entity
// data structure. The template parameters are:
//   TypeRefs: gives access to the types declared and used within the PartialDS
//             data structure. (See PartialDSTypes in partialds.h.)
//   PointType: the geometry information stored within each vertex.
template <class TypeRefs, class PointType>
class PartialDSVertex : public PartialDSEntity<TypeRefs> {
 public:
  typedef TypeRefs                              PartialDS;
  typedef PointType                             Point;
  typedef PartialDSVertex<TypeRefs, Point>      Self;

  typedef typename PartialDS::VertexHandle      VertexHandle;
  typedef typename PartialDS::VertexConstHandle VertexConstHandle;

  PartialDSVertex() { }

  // TODO(gwink): Declare this accessors once PFace and PVertex have been
  // defined.
  // EntityHandle parent();
  // EntityConstHandle parent() const;
  // void set_parent(EntityHandle parent);

  const Point& point() const { return p_; }
  void set_point(const Point& p) { p_ = p; }

 private:
  // TODO(gwink): Implement the parent pointer once the corresponding types
  // have been defined.
  // union parent {
  //  PFaceHandle pface_;
  //  PVertexHandle pvertex_;
  // };
  Point p_;
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VERTEX_H_
