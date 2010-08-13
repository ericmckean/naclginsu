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
// data structure. The template parameters are:
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

  typedef typename Base::VertexFlavor            VertexFlavor;
  typedef typename PartialDS::VariantHandle      VariantHandle;
  typedef typename PartialDS::VertexHandle       VertexHandle;
  typedef typename PartialDS::VertexConstHandle  VertexConstHandle;
  typedef typename PartialDS::PVertexHandle      PVertexHandle;
  typedef typename PartialDS::PVertexConstHandle PVertexConstHandle;
  typedef typename PartialDS::PFaceHandle        PFaceHandle;
  typedef typename PartialDS::PFaceConstHandle   PFaceConstHandle;

  PartialDSVertex() : parent_(NULL) { }

  VertexFlavor flavor() const { return flavor_; }
  void set_flavor(VertexFlavor flavor) { flavor_ = flavor; }
  
  PFaceConstHandle GetPFaceParent() const {
    assert(flavor() == Base::kIsolatedVertex);
    return PartialDS::AsPFace(parent_);
  }
  PVertexConstHandle GetPVertexParent() const {
    assert(flavor() == Base::kNormalVertex);
    return PartialDS::AsPVertex(parent_);
  }
  void set_parent(VariantHandle parent) { parent_ = parent; }

  const Point& point() const { return p_; }
  void set_point(const Point& p) { p_ = p; }

 private:
  VertexFlavor flavor_;  // Type of vertex.
  VariantHandle parent_;  // Either a pface or a pvertex.
  Point p_;
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VERTEX_H_
