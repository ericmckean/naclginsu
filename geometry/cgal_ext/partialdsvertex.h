// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VERTEX_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VERTEX_H_

#include "CGAL/basic.h"
#include "geometry/cgal_ext/partialdsentity.h"

namespace ginsu {
namespace geometry {

template <class T> class PartialDS;

// PartialDSVertex: template class for vertex entity in the partial-entity
// data structure.
// The template parameters are:
//   TypeRefs: gives access to the types declared and used within the PartialDS
//             data structure. (See PartialDSTypes in partialds.h.)
//   PointType: the geometry information stored within each vertex.
template <class TypeRefs, class PointType>
class PartialDSVertex : public PartialDSEntity<TypeRefs> {
 public:
  typedef TypeRefs                               PartialDSTypes;
  typedef PointType                              Point;
  typedef PartialDSVertex<TypeRefs, Point>       Self;
  typedef PartialDSEntity<TypeRefs>              Base;

  typedef typename PartialDSTypes::VertexHandle       VertexHandle;
  typedef typename PartialDSTypes::VertexConstHandle  VertexConstHandle;
  typedef typename PartialDSTypes::PVertexHandle      PVertexHandle;
  typedef typename PartialDSTypes::PVertexConstHandle PVertexConstHandle;
  typedef typename PartialDSTypes::EdgeConstHandle    EdgeConstHandle;

  PartialDSVertex() : parent_pvertex_(NULL) { }

  // Accessors
  PVertexConstHandle parent_pvertex() const { return parent_pvertex_; }
  PVertexHandle parent_pvertex() { return parent_pvertex_; }
  const Point& point() const { return p_; }

  // Return true if the vertex is an isolated vertex, and false otherwise.
  bool IsIsolated() const {
    // Follow the parent chain edge up to the edge and check if it has
    // the same p-vertex as start and end vertices.
    PVertexConstHandle pv = parent_pvertex();
    EdgeConstHandle e = pv->parent_edge();
    return (e->start_pvertex() == pv) && (e->end_pvertex() == pv);
  }

 protected:
  friend class PartialDS<typename PartialDSTypes::Traits>;

  // Mutators
  void set_parent_pvertex(PVertexHandle pvertex) { parent_pvertex_ = pvertex; }
  void set_point(const Point& p) { p_ = p; }

 private:
  PVertexHandle parent_pvertex_; 
  Point p_;
};

}  // namespace geometry
}  // namespace ginsu

#endif  // GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VERTEX_H_
