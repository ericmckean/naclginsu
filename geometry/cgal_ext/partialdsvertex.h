// Copyright (c) 2010 The Ginsu Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VERTEX_H_
#define GINSU_GEOMETRY_CGAL_EXT_PARTIALDS_VERTEX_H_

#include "CGAL/basic.h"
#include "geometry/cgal_ext/partialdscirculators.h"
#include "geometry/cgal_ext/partialdsentity.h"

namespace ginsu {
namespace geometry {

template <class T> class PartialDS;
template <class T> class PartialDSUtils;

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

  typedef circulator::PVertexOfVertexCirculator<PVertexConstHandle>
                                                      PVertexConstCirculator;
  typedef circulator::PVertexOfVertexCirculator<PVertexHandle>
                                                      PVertexCirculator;

  PartialDSVertex() : parent_pvertex_(NULL) { }

  // Accessors
  PVertexConstHandle parent_pvertex() const { return parent_pvertex_; }
  PVertexHandle parent_pvertex() { return parent_pvertex_; }
  const Point& point() const { return p_; }

  // Iterate over the p-vertex about this vertex.
  PVertexConstCirculator pvertex_begin() const {
    return PVertexConstCirculator(parent_pvertex());
  }
  PVertexCirculator pvertex_begin() {
    return PVertexCirculator(parent_pvertex());
  }

  // Return true if the vertex is an isolated vertex, and false otherwise.
  bool IsIsolated() const {
    // Follow the parent chain edge up to the edge and check if it has
    // the same p-vertex as start and end vertices.
    PVertexConstHandle pv = parent_pvertex();
    EdgeConstHandle e = pv->parent_edge();
    return (e->start_pvertex() == pv) && (e->end_pvertex() == pv);
    // TODO(gwink): rule-out a vertex loop.
  }

  // Return the number of p-vertices associated with this vertex.
  int GetPVertexCount() const {
    if (parent_pvertex() == NULL) return 0;

    PVertexConstCirculator start = pvertex_begin(), i = start;
    int count = 0;
    do { ++count; } while(++i != start);
    return count;
  }

 protected:
  friend class PartialDS<typename PartialDSTypes::Traits>;
  friend class PartialDSUtils<PartialDSTypes>;

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
